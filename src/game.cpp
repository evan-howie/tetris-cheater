#include "../includes/game.h"
#include <iostream>

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
    if (board->status == BoardStatus::TOP_OUT) {
        // end screen
        drawTopOut();
        window->display();
        return;
    }
    unsigned int bw = board->getWidth();
    unsigned int dx{window_width / 2 - bw * tile_size/ 2};
    unsigned int dy{0};

    window->clear(sf::Color::White);
    board->draw(window, dx, dy);
    window->display();
}

void Game::drawTopOut() {
    window->draw(top_out_text);
}

void Game::setupText() {
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
