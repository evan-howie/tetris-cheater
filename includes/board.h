#ifndef BOARD_H
#define BOARD_H

#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <unistd.h>
#include <cstring>           /* For memset */
#include <SFML/Graphics.hpp>
#include <queue>
#include "mino.h"
#include "tetramino.h"
#include "settings.h"
#include "next_queue.h"

enum class BoardStatus {
    PLAYING,
    TOP_OUT
};

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
    void newPiece();
    void hold();

    void setMino(unsigned int x, unsigned int y, char mino);

    //input handling
    struct {
        bool left = false;
        bool right = false;
        bool down = false;
    } keys;

    const double DAS = DEFAULT_DAS; // delayed auto shift ms
    const double ARR = DEFAULT_ARR; // auto repeat rate Hz
    const double ARP = DEFAULT_ARP; // auto repeat period ms
    const double SDF = DEFAULT_SDF; // insta drop
    double cur_das_time = DAS;
    double cur_arp_time = ARP;

    void incrementDAS();
    void incrementARR();
    void incrementGravity();
    void hardDrop();

    const int gravity = 60; // frames per line
    int gravity_counter = 0;
    double lock_time = DEFAULT_LOCK_TIME;
    double hard_lock_factor = DEFAULT_HARD_LOCK_FACTOR;
    double cur_lock_timer = lock_time; // lock if no input and there is no space below
    double hard_lock_timer = lock_time * DEFAULT_HARD_LOCK_FACTOR; // lock regardless of input
    bool is_empty_space_below = false;
    void incrementLockTimer();

    // private draws
    void drawHeld(sf::RenderWindow* window, unsigned int x, unsigned int y);

    bool shm_enabled = false;
    int shmfd;
    unsigned char* shm_board;
    void writeToSharedMem();

public:
    enum BoardStatus status = BoardStatus::PLAYING;

    unsigned int tile_size;
    static const unsigned char empty_cell = 0x80;

    Board(unsigned int w, unsigned int h, unsigned int tile_size);
    ~Board();

    void init();
    void update();
    void handleInput(sf::Event e);
    void draw(sf::RenderWindow* window, unsigned int x, unsigned int y);
    void print();

    void resetLockTimer();
    void resetHardLockTimer();

    void initSharedMemory();

    bool isMino(unsigned int x, unsigned int y);
    bool isMino(unsigned char mino);
    bool inBounds(int x, int y);
    unsigned char getMino(unsigned int x, unsigned int y);

    unsigned int getHeight();
    unsigned int getWidth();

};

#endif