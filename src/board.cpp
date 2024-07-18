#include "../includes/board.h"
#include <iostream>
#include <random>

Board::Board(unsigned int w, unsigned int h, unsigned int _tile_size) : width{w}, height{h}, tile_size{_tile_size}{
    board = new unsigned char[width * height];
    for (unsigned int i = 0; i < width * height; ++i)
        board[i] = empty_cell;
    cur_piece = Tetramino{this};
    held_piece = Tetramino{this};
}

Board::~Board(){
    delete[] board;
}

void Board::init(){
    //TODO: im sure theres something we need to init
    next_queue.init(this);
    cur_piece = next_queue.pop();
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
}

void Board::incrementDAS(){
    cur_das_time = std::max(0.0, cur_das_time - GAME_LOOP_PERIOD_ms);
    if (cur_das_time != 0) return;
    incrementARR();
}

void Board::incrementARR(){
    cur_arp_time = std::max(0.0, cur_arp_time - GAME_LOOP_PERIOD_ms);

    if (cur_arp_time != 0) return;

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

void Board::hardDrop(){
    while(cur_piece.moveDown());
    placePiece();
}

void Board::placePiece(){
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

void Board::drawHeld(sf::RenderWindow& window, unsigned int x, unsigned int y){
    //TODO: center around point x and y
    held_piece.drawOffBoard(window, x, y);
}

void Board::draw(sf::RenderWindow& window, unsigned int board_x, unsigned int board_y){
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

            window.draw(rect);
        }
    }

    // draw current piece
    cur_piece.draw(window, board_x, board_y);

    // draw held piece
    // std::cout << "hi: " << board_x - tile_size * 5 << std::endl;
    drawHeld(window, board_x - tile_size * 4, board_y);
    // std::cout << "hio: " << board_x - tile_size * 5 << std::endl;

    // draw next queue
    next_queue.draw(window, board_x, board_y);
}

void Board::setMino(unsigned int x, unsigned int y, char mino){
    board[x + y * width] = mino;
}

char Board::getMino(unsigned int x, unsigned int y){
    return board[x + y * width];
}

unsigned int Board::getHeight(){
    return height;
}

unsigned int Board::getWidth(){
    return width;
}