#pragma once

#include "board.h"
#include <SFML/Graphics.hpp>

// one day, move out class draw functions to have multiple UIs for same game logic
// parent class
class Game {
private:
    sf::RenderWindow *window;
    Board *board{};

    std::string top_out_text_string{"Press \"r\" to try again!"};
    sf::Font top_out_text_font;
    sf::Text top_out_text;

    unsigned int window_width;
    unsigned int window_height;
    float loop_period_s;
    unsigned int board_width;
    unsigned int board_height;
    unsigned int tile_size;

public:
    Game(
        unsigned int window_width,
        unsigned int window_height,
        float loop_period,
        unsigned int board_width,
        unsigned int board_height,
        unsigned int tile_size
    );
    ~Game();

    void play();
    void update();
    void draw();
    void drawTopOut();
    void setupText();
    Board* createBoard(unsigned int width, unsigned int height, unsigned int tile_size);
    void handleInput(sf::Event event);
};