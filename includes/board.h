#include <SFML/Graphics.hpp>
#include "mino.h"
#include "tetramino.h"

#ifndef BOARD_H
#define BOARD_H

class Board {
private:
    // board cells are characters representing what the cell contains (for mino some num < N_MINOS)
    // bottom row is index 0
    const sf::Color empty_cell_color = sf::Color{0x1b1b1bff};

    unsigned char* board;
    unsigned int width;
    unsigned int height;

    // tetramino
    Tetramino cur_piece;

    bool isRowFull(unsigned int row);

    void clearRows();
    void shiftDown(unsigned int start_row);

    void setMino(unsigned int x, unsigned int y, char mino);

public:
    const unsigned int tile_size;
    static const unsigned char empty_cell = 0x80;

    Board(unsigned int w, unsigned int h, unsigned int tile_size);
    ~Board();

    void init();
    void update();
    void handleInput(sf::Event e);
    void draw(sf::RenderWindow& window, unsigned int x, unsigned int y);

    bool isMino(unsigned int x, unsigned int y);
    bool isMino(unsigned char mino);
    bool inBounds(int x, int y);
    char getMino(unsigned int x, unsigned int y);

    unsigned int getHeight();
};

#endif