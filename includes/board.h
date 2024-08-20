#ifndef BOARD_H
#define BOARD_H

#include <SFML/Graphics.hpp>

class Board {
private:
    // board cells are characters representing what the cell contains (for mino some num < N_MINOS)
    // bottom row is index 0
    unsigned char* board;
    unsigned int width;
    unsigned int height;

    bool isRowFull(unsigned int row);

    void shiftDown(unsigned int start_row);

public:

    unsigned int tile_size;

    Board(unsigned int w, unsigned int h, unsigned int tile_size);
    ~Board();

    void init();
    void update();
    void draw(sf::RenderWindow* window, unsigned int x, unsigned int y);
    void print();

    void clearRows();
    bool isMino(unsigned int x, unsigned int y);
    bool isMino(unsigned char mino);
    bool inBounds(int x, int y);
    void setMino(unsigned int x, unsigned int y, char mino);
    unsigned char getMino(unsigned int x, unsigned int y);

    unsigned int getHeight();
    unsigned int getWidth();

};

#endif