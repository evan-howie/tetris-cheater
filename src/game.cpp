#include "../includes/game.h"

Game::Game(
    unsigned int _window_width,
    unsigned int _window_height,
    float _loop_period_s,
    unsigned int _board_width,
    unsigned int _board_height,
    unsigned int _tile_size
):
    window_width{_window_width},
    window_height{_window_height},
    loop_period_s{_loop_period_s},
    board_width{_board_width},
    board_height{_board_height},
    tile_size{_tile_size}
{}

Game::~Game() {
    delete window;
    delete board;
}
// maybe move to structs with window settings and board settings
void Game::play() {
    // set up window
    window = new sf::RenderWindow{sf::VideoMode{window_width, window_height}, "tetris"};
    window->setPosition(sf::Vector2i{100, 100});

    //set up board
    board = createBoard(board_width, board_height, tile_size);

    // clock
    sf::Clock clock;
    const sf::Time loop_period = sf::seconds(loop_period_s);

    // main loop
    sf::Time time_since_last = sf::Time::Zero;
    while (window->isOpen()){
        sf::Time dt = clock.restart();
        time_since_last += dt;

        while (time_since_last > loop_period) {
            time_since_last -= loop_period;

            // process events
            sf::Event event;
            while (window->pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window->close();
                handleInput(event);
            }

            // update game logic here
            // one tick has passed
            update();
        }

        // display the game state
        draw();

    }
}

Board* Game::createBoard(unsigned int width, unsigned int height, unsigned int tile_size) {
    Board* board = new Board{BOARD_WIDTH, BOARD_HEIGHT, TILE_SIZE};
    board->init();
    return board;
}

void Game::handleInput(sf::Event event) {
    if (event.type == sf::Event::KeyPressed){
        switch(event.key.code){
            case sf::Keyboard::Escape:
                window->close();
                break;
            case sf::Keyboard::R:
                delete board;
                board = createBoard(board_width, board_height, tile_size);
                break;
        }
    }
    board->handleInput(event);
}

void Game::update() {
    board->update();
}

void Game::draw() {
    unsigned int bw = board->getWidth();
    unsigned int dx{window_width / 2 - bw * tile_size/ 2};
    unsigned int dy{0};

    window->clear(sf::Color::White);
    board->draw(window, dx, dy);
    window->display();
}