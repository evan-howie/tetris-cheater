#pragma once

#include <SFML/Graphics.hpp>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <unistd.h>
#include <cstring>           /* For memset */
#include <queue>
#include "board.h"
#include "held.h"
#include "mino.h"
#include "tetramino.h"
#include "settings.h"
#include "next_queue.h"

enum class GameStatus {
    PLAYING,
    TOP_OUT
};


class Game {
private:
    enum GameStatus status = GameStatus::PLAYING;

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

    // game objects

    Tetramino cur_piece;
    HeldTetramino held_piece;
    NextQueue next_queue;

    // game logic functions

    void placePiece();
    void newPiece();
    void hold();

    // reset

    void reset();

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

    void resetLockTimer();
    void resetHardLockTimer();

    bool shm_enabled = false;
    int shmfd;
    unsigned char* shm_game;
    unsigned int shm_size;
    void writeToSharedMemory();
    void initSharedMemory(unsigned int _shm_size);
    void cleanupSharedMemory();

    void initGameObjects(bool should_init_shm=false);
    void update();
    void draw();
    void drawTopOut();
    void setupText();
    Board* createBoard(unsigned int width, unsigned int height, unsigned int tile_size);
    void handleKeyPressed(sf::Event event);
    void handleKeyReleased(sf::Event event);
public:
    Game(
        unsigned int window_width,
        unsigned int window_height,
        float loop_period,
        unsigned int board_width,
        unsigned int board_height,
        unsigned int tile_size,
        unsigned int shm_size
    );
    ~Game();

    void play();
};