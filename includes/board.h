#include <SFML/Graphics.hpp>
#include <queue>
#include "mino.h"
#include "tetramino.h"
#include "settings.h"
#include "next_queue.h"

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
    Tetramino held_piece;
    NextQueue next_queue;

    bool isRowFull(unsigned int row);

    void clearRows();
    void shiftDown(unsigned int start_row);
    void placePiece();
    void hold();

    void setMino(unsigned int x, unsigned int y, char mino);

    //input handling
    struct {
        bool left = false;
        bool right = false;
        bool down = false;
    } keys;

    const double DAS = 85; // delayed auto shift ms
    const double ARR = INT32_MAX; // auto repeat rate Hz
    const double ARP = 1000.0 / ARR; // auto repeat period ms
    const double SDF = -1; // insta drop
    double cur_das_time = DAS;
    double cur_arp_time = ARP;

    void incrementDAS();
    void incrementARR();
    void incrementGravity();
    void hardDrop();

    const int gravity = 60; // frames per line
    int gravity_counter = 0;

    // private draws
    void drawHeld(sf::RenderWindow& window, unsigned int x, unsigned int y);

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