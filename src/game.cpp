#include "../includes/game.h"
#include <iostream>

Game::Game::Game(
    WindowSettings window_settings,
    BoardSettings board_settings,
    GameSettings game_settings
):
    window_width{window_settings.window_width},
    window_height{window_settings.window_width},
    loop_period{game_settings.loop_period},
    board_width{board_settings.board_width},
    board_height{board_settings.board_height},
    tile_size{board_settings.tile_size},
    DAS{game_settings.das},
    ARR{game_settings.arr},
    ARP{game_settings.arp},
    SDF{game_settings.sdf},
    lock_time{game_settings.lock_time},
    hard_lock_factor{game_settings.hard_lock_factor}
{}

Game::Game::~Game() {
    delete window;
    delete board;
}

/**
 * GAME LOGIC
 */

void Game::Game::incrementDAS(){
    cur_das_time = std::max(0.0, cur_das_time - loop_period);
    if (cur_das_time > 0.0) return;
    incrementARR();
}

void Game::Game::incrementARR(){
    cur_arp_time = std::max(0.0, cur_arp_time - loop_period);

    if (cur_arp_time > 0.0) return;

    if (keys.left){
        if(ARR >= 20) cur_piece.slideLeft(board);
        else cur_piece.moveLeft(board);
    } else if (keys.right){
        if(ARR >= 20) cur_piece.slideRight(board);
        else cur_piece.moveRight(board);
    }
    cur_arp_time = ARP;
}

void Game::Game::incrementGravity(){
    ++gravity_counter;
    if (gravity_counter != gravity) return;

    gravity_counter = 0;
    cur_piece.moveDown(board);
}

void Game::Game::incrementLockTimer() {
    std::pair<int, int> pos = cur_piece.getPos();
    std::vector<std::vector<unsigned char>> shape = cur_piece.getShape();
    is_empty_space_below = !cur_piece.testShape(board, shape, {pos.first, pos.second - 1});

    if (!is_empty_space_below){
        resetLockTimer();
        return;
    }

    cur_lock_timer = std::max(0.0, cur_lock_timer - loop_period);
    hard_lock_timer = std::max(0.0, hard_lock_timer - loop_period);

    if (cur_lock_timer > 0.0 && hard_lock_timer > 0.0) return;

    placePiece();
}

void Game::Game::hardDrop(){
    while(cur_piece.moveDown(board));
    placePiece();
}

void Game::Game::placePiece(){
    resetHardLockTimer();
    resetLockTimer();
    // place piece on board
    for (int y = 0 ; y < cur_piece.getShape().size() ; ++y){
        for (int x = 0 ; x < cur_piece.getShape()[y].size() ; ++x){
            if (!cur_piece.isMino(x, y)) continue;

            board->setMino(
                cur_piece.getPos().first - cur_piece.getOrigin().first + x, 
                cur_piece.getPos().second - cur_piece.getOrigin().second+ y, 
                cur_piece.at(x, y));
        }
    }

    board->clearRows();
    newPiece();
}

void Game::Game::newPiece() {
    cur_piece = next_queue.pop();
    held_piece.setHasHeld(false);
    
    cur_piece.init(board);
    if (!cur_piece.testShape(board, cur_piece.getPos())) {
        status = GameStatus::TOP_OUT;
    }
}

void Game::Game::hold(){
    if (held_piece.getHasHeld()) return;

    cur_piece.reset(board);
    if(held_piece.isEmpty()){
        held_piece = cur_piece;
        newPiece();
    } else {
        Tetramino temp_piece = cur_piece;
        cur_piece = held_piece;
        held_piece = temp_piece;
    }
    cur_piece.reset(board);

    held_piece.setHasHeld(true);
}

void Game::Game::initGameObjects() {
    //set up board
    board = createBoard(board_width, board_height, tile_size);

    //set up pieces
    next_queue.init();
    newPiece();
    held_piece = Tetramino{};
}

// maybe move to structs with window settings and board settings
void Game::Game::play() {
    setupText();
    // set up window
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    window = new sf::RenderWindow{
        sf::VideoMode{window_width, window_height},
        "tetris",
        sf::Style::Default,
        settings
    };
    window->setPosition(sf::Vector2i{100, 100});

    initGameObjects();

    // clock
    sf::Clock clock;
    const sf::Time loop_period_time = sf::milliseconds(loop_period);

    // main loop
    sf::Time time_since_last = sf::Time::Zero;
    while (window->isOpen()){
        sf::Time dt = clock.restart();
        time_since_last += dt;

        while (time_since_last > loop_period_time) {
            time_since_last -= loop_period_time;

            // process events
            sf::Event event;
            while (window->pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window->close();
                if (event.type == sf::Event::KeyPressed)
                    handleKeyPressed(event);
                if (event.type == sf::Event::KeyReleased)
                    handleKeyReleased(event);
            }

            // update game logic here
            // one tick has passed
            update();
        }

        // display the game state
        draw();

    }
}

Board* Game::Game::createBoard(unsigned int width, unsigned int height, unsigned int tile_size) {
    Board* board = new Board{board_width, board_height, tile_size};
    board->init();
    return board;
}

void Game::Game::reset() {
    delete board;
    board = createBoard(board_width, board_height, tile_size);

    initGameObjects();
    status = GameStatus::PLAYING;
}

void Game::Game::handleKeyPressed(sf::Event event) {
    if (event.type != sf::Event::KeyPressed) return;
    switch(event.key.code){
        case sf::Keyboard::Escape:
            window->close();
            break;
        case sf::Keyboard::R:
            reset();
            break;
        case sf::Keyboard::Left:
            resetLockTimer();
            if (!keys.left)
                cur_piece.moveLeft(board);
            keys.left = true;
            break;
        case sf::Keyboard::Right:
            resetLockTimer();
            if (!keys.right)
                cur_piece.moveRight(board);
            keys.right = true;
            break;
        case sf::Keyboard::C:
            hold();
            break;
        case sf::Keyboard::Down:
            resetLockTimer();
            cur_piece.slideDown(board);
            break;
        case sf::Keyboard::Space:
            hardDrop();
            break;
        case sf::Keyboard::Z:
            cur_piece.rotateCCW(board);
            break;
        case sf::Keyboard::X:
            cur_piece.rotateCW(board);
            break;
        case sf::Keyboard::A:
            // TODO: change to actual 180 spin
            cur_piece.rotateCW(board);
            cur_piece.rotateCW(board);
    }
}

void Game::Game::handleKeyReleased(sf::Event event) {
    if (event.type != sf::Event::KeyReleased) return;
    
    switch(event.key.code){
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

void Game::Game::resetLockTimer() {
    cur_lock_timer = lock_time;
}

void Game::Game::resetHardLockTimer() {
    hard_lock_timer = lock_time * hard_lock_factor;
}

void Game::Game::update() {
    if(keys.right || keys.left)
        incrementDAS();
    else {
        cur_das_time = DAS;
        cur_arp_time = ARP;
    }

    incrementGravity();
    incrementLockTimer();

    board->update();
}

void Game::Game::draw() {
    if (status == GameStatus::TOP_OUT) {
        // end screen
        drawTopOut();
        window->display();
        return;
    }
    unsigned int b_dx = window_width / 2 - board_width * tile_size/ 2;
    unsigned int b_dy = window_height / 2 - board_height * tile_size / 2;

    window->clear(BackgroundColor);

    board->draw(window, b_dx, b_dy);

    // draw current piece
    cur_piece.drawShadow(board, window, b_dx, b_dy, tile_size);
    cur_piece.drawOnBoard(board, window, b_dx, b_dy, tile_size);

    // draw held piece
    int h_dx = b_dx - 10;
    int h_dy = b_dy;
    held_piece.draw(window, h_dx, h_dy, tile_size);

    // draw next queue
    int nq_dx = b_dx + tile_size * (board_width) + 10;
    int nq_dy = b_dy;
    next_queue.draw(window, nq_dx, nq_dy, tile_size);
    window->display();
}

void Game::Game::drawTopOut() {
    window->draw(top_out_text);
}

void Game::Game::setupText() {
    if (!top_out_text_font.loadFromFile("../fonts/open-sans-variable.ttf")){ // maybe move to env something
        exit(1);
    }
    top_out_text.setFont(top_out_text_font);
    top_out_text.setScale(1, 1);
    top_out_text.setString(top_out_text_string);
    top_out_text.setCharacterSize(64);
    top_out_text.setFillColor(sf::Color::White);
    top_out_text.setOutlineColor(sf::Color::Black);
    top_out_text.setOutlineThickness(2);
    sf::FloatRect bounds = top_out_text.getGlobalBounds();
    top_out_text.setOrigin(bounds.left + bounds.width / 2.f , bounds.top + bounds.height / 2.f);
    top_out_text.setPosition(window_width / 2, window_height / 2);
    top_out_text.setStyle(sf::Text::Regular);
}
