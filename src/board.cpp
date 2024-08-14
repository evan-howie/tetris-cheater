#include "../includes/board.h"
#include <iostream>
#include <random>
#include <cstring>

Board::Board(unsigned int w, unsigned int h, unsigned int _tile_size) : width{w}, height{h}, tile_size{_tile_size}{}

Board::~Board(){
    delete[] board;

    if (!shm_enabled) return;
    // Unmap the shared memory
    if (munmap(shm_board, width * height) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    // Close the shared memory object
    if (close(shmfd) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }
}

void Board::init(){
    initSharedMemory();

    board = new unsigned char[width * height];
    for (unsigned int i = 0; i < width * height; ++i)
        board[i] = empty_cell;

    next_queue.init(this);
    cur_piece = next_queue.pop();
    held_piece = Tetramino{this};
}

// called once per iteration of the game loop
void Board::update(){
    // TODO: gravity
    if(keys.right || keys.left)
        incrementDAS();
    else {
        cur_das_time = DAS;
        cur_arp_time = ARP;
    }

    incrementGravity();
    incrementLockTimer();

    if (shm_enabled){
        writeToSharedMem();
    }
}

void Board::incrementDAS(){
    cur_das_time = std::max(0.0, cur_das_time - GAME_LOOP_PERIOD_ms);
    if (cur_das_time > 0.0) return;
    incrementARR();
}

void Board::incrementARR(){
    cur_arp_time = std::max(0.0, cur_arp_time - GAME_LOOP_PERIOD_ms);

    if (cur_arp_time > 0.0) return;

    if (keys.left){
        if(ARR >= 20) cur_piece.slideLeft();
        else cur_piece.moveLeft();
    } else if (keys.right){
        if(ARR >= 20) cur_piece.slideRight();
        else cur_piece.moveRight();
    }
    cur_arp_time = ARP;
}

void Board::incrementGravity(){
    ++gravity_counter;
    if (gravity_counter != gravity) return;

    gravity_counter = 0;
    cur_piece.moveDown();
}

void Board::incrementLockTimer() {
    std::pair<int, int> pos = cur_piece.getPos();
    std::vector<std::vector<unsigned char>> shape = cur_piece.getShape();
    is_empty_space_below = !cur_piece.testShape(shape, {pos.first, pos.second - 1});

    if (!is_empty_space_below){
        resetLockTimer();
        return;
    }

    cur_lock_timer = std::max(0.0, cur_lock_timer - GAME_LOOP_PERIOD_ms);
    hard_lock_timer = std::max(0.0, hard_lock_timer - GAME_LOOP_PERIOD_ms);

    if (cur_lock_timer > 0.0 && hard_lock_timer > 0.0) return;

    placePiece();
}

void Board::hardDrop(){
    while(cur_piece.moveDown());
    placePiece();
}

void Board::placePiece(){
    resetHardLockTimer();
    resetLockTimer();
    // place piece on board
    for (int y = 0 ; y < cur_piece.getShape().size() ; ++y){
        for (int x = 0 ; x < cur_piece.getShape()[y].size() ; ++x){
            if (!cur_piece.isMino(x, y)) continue;

            setMino(
                cur_piece.getPos().first - cur_piece.getOrigin().first + x, 
                cur_piece.getPos().second - cur_piece.getOrigin().second+ y, 
                cur_piece.at(x, y));
        }
    }

    clearRows();
    cur_piece = next_queue.pop();
}


void Board::hold(){
    cur_piece.reset();
    if(held_piece.isEmpty()){
        held_piece = cur_piece;
        cur_piece = next_queue.pop();
    } else {
        std::swap(cur_piece, held_piece);
    }
}


void Board::handleInput(sf::Event e){
    if (e.type != sf::Event::KeyPressed && e.type != sf::Event::KeyReleased) return;
    
    if(e.type == sf::Event::KeyPressed){
        switch(e.key.code){
            case sf::Keyboard::Left:
                if (!keys.left)
                    cur_piece.moveLeft();
                keys.left = true;
                break;
            case sf::Keyboard::Right:
                if (!keys.right)
                    cur_piece.moveRight();
                keys.right = true;
                break;
            case sf::Keyboard::C:
                hold();
                break;
            case sf::Keyboard::Down:
                cur_piece.slideDown();
                break;
            case sf::Keyboard::Space:
                hardDrop();
                break;
            case sf::Keyboard::Z:
                cur_piece.rotateCCW();
                break;
            case sf::Keyboard::X:
                cur_piece.rotateCW();
                break;
            case sf::Keyboard::A:
                // TODO: change to actual 180 spin
                cur_piece.rotateCW();
                cur_piece.rotateCW();
            default:
                break;
        }
    } else {
        switch(e.key.code){
            case sf::Keyboard::Left:
                keys.left = false;
                break;
            case sf::Keyboard::Right:
                keys.right = false;
                break;
            default:
                break;
        }
        if (!keys.left && !keys.right)
            cur_das_time = DAS;
    }
}


// called every drop
void Board::clearRows(){
    // shift down starting from all full rows
    for (int row = 0 ; row < height ; ++row){
        while (isRowFull(row)){
            shiftDown(row);
        }
    }
}

void Board::shiftDown(unsigned int start_row){
    // shift all rows down 1 starting from start_row (inclusive)
    for (int y = start_row ; y < height - 1 ; ++y){
        for (int x = 0 ; x < width ; ++x){
            setMino(x, y, getMino(x, y + 1));
        }
    }

    //empty top row
    for (int x = 0 ; x < width ; ++x){
        setMino(x, height - 1, empty_cell);
    }
}

bool Board::isRowFull(unsigned int row){
    for (int x = 0 ; x < width ; ++x){
        if (!isMino(x, row)) return false;
    }
    return true;
}

void Board::writeToSharedMem(){
    // write board
    for (int j = 0 ; j < height ; ++j) {
        for (int i = 0 ; i < width; ++i) {
            unsigned char cell = board[i + j * width];
            shm_board[i + (height - j - 1) * (width + 1)] = (cell == empty_cell) ? SHM_CELL_EMPTY : SHM_CELL_FULL; 
        }
        shm_board[(height - j - 1) * (width + 1) + width] = '\n';
    }

    // write cur_piece
    std::vector<std::vector<unsigned char>> cur_piece_shape = cur_piece.getShape();
    std::pair<int, int> pos = cur_piece.getPos();
    std::pair<int, int> origin = cur_piece.getOrigin();

    for(int y = 0 ; y < cur_piece_shape.size() ; ++y){
        for (int x = 0 ; x < cur_piece_shape.size() ; ++x){
            unsigned char cell = cur_piece.isMino(x, y) ? SHM_CELL_FULL : SHM_CELL_EMPTY;
            int cell_x = pos.first + x - origin.first;
            int cell_y = height - pos.second - y - 1 + origin.second;

            if (!cur_piece.isMino(x, y)) continue;

            shm_board[cell_x + cell_y * (width + 1)] = cell;
        }
    }
    
    // TODO: write hold
    // std::vector<std::vector<unsigned char>> held_piece_shape = held_piece.getShape();
    // int shm_pos = (width + 1) * height;
    // shm_board[shm_pos++] = 'h';
    // shm_board[shm_pos++] = '\n';
    // for (int j = held_piece_shape.size() - 1 ; j >= 0 ; --j) {
    //     for (int i = 0 ; i < held_piece_shape[j].size() ; ++i) {
    //         shm_board[shm_pos++] = held_piece.isMino(i, j) ? SHM_CELL_FULL : SHM_CELL_EMPTY;
    //     }
    //     shm_board[shm_pos++] = '\n';
    // }
}

void Board::initSharedMemory() {
    shm_enabled = true;

    // Create a shared memory object
    shmfd = shm_open(SHM_PATH, O_CREAT | O_RDWR, 0666);
    if (shmfd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    const int size = SHM_SIZE;

    // Set the size of the shared memory object
    if (ftruncate(shmfd, size) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    // Map the shared memory object into the process's address space
    shm_board = (unsigned char*) mmap(NULL, size, PROT_WRITE, MAP_SHARED, shmfd, 0);
    if (shm_board == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    memset(shm_board, 0, size);
}

bool Board::isMino(unsigned int x, unsigned int y){
    // use the msb of cell to represent a tile with something other than a mino (empty, something else)
    return ( board[x + y * width] & empty_cell ) == 0;
}

bool Board::isMino(unsigned char mino){
    return (mino & empty_cell) == 0;
}

bool Board::inBounds(int x, int y){
    return x >= 0 && x < width && y >= 0 && y < height;
}

void Board::drawHeld(sf::RenderWindow* window, unsigned int x, unsigned int y){
    //TODO: center around point x and y
    held_piece.drawOffBoard(window, x, y);
}

void Board::draw(sf::RenderWindow* window, unsigned int board_x, unsigned int board_y){
    // draw board cells
    sf::RectangleShape rect(sf::Vector2f(tile_size, tile_size));
    rect.setOutlineColor(sf::Color::Black);
    rect.setOutlineThickness(1);
    for (int x = 0 ; x < width ; ++x){
        for (int y = 0 ; y < height ; ++y){
            rect.setPosition(x * tile_size + board_x, (height - y - 1) * tile_size + board_y);

            rect.setFillColor(isMino(x, y) ?
                mino::colors[board[x + y * width]] :
                empty_cell_color);

            window->draw(rect);
        }
    }

    // draw current piece
    cur_piece.draw(window, board_x, board_y);

    // draw held piece
    // std::cout << "hi: " << board_x - tile_size * 5 << std::endl;
    drawHeld(window, board_x - tile_size * 5, board_y);
    // std::cout << "hio: " << board_x - tile_size * 5 << std::endl;

    // draw next queue
    next_queue.draw(window, board_x, board_y);
}

void Board::print() {
    for (int j = height - 1 ; j >= 0 ; --j) {
        for (int i = 0 ; i < width; ++i) {
            unsigned char cell = board[i + j * width];
            if (cell == empty_cell){
                std::cout << 'x';
            } else {
                std::cout << (unsigned int) cell;
            }
        }
        std::cout << '\n';
    }
    std::cout << std::flush;
}

void Board::resetLockTimer() {
    cur_lock_timer = lock_time;
}

void Board::resetHardLockTimer() {
    hard_lock_timer = lock_time * hard_lock_factor;
}

void Board::setMino(unsigned int x, unsigned int y, char mino){
    board[x + y * width] = mino;
}

unsigned char Board::getMino(unsigned int x, unsigned int y){
    return board[x + y * width];
}

unsigned int Board::getHeight(){
    return height;
}

unsigned int Board::getWidth(){
    return width;
}