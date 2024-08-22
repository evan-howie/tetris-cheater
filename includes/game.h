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
#include "next_queue.h"

enum class GameStatus {
    PLAYING,
    TOP_OUT
};


namespace Game {
    struct WindowSettings {
        unsigned int window_width;
        unsigned int window_height;
    };

    struct BoardSettings {
        unsigned int board_width;
        unsigned int board_height;
        unsigned int tile_size;
    };

    struct SHMSettings {
        unsigned int shm_size;
        char* shm_path;
        unsigned char shm_cell_full;
        unsigned char shm_cell_empty;
    };

    struct GameSettings {
        float loop_period;
        double das;
        double arr;
        double arp;
        double sdf;
        double lock_time;
        double hard_lock_factor;
    };

    const sf::Color BackgroundColor{0x010101ff};

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
        float loop_period;
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

        const double DAS; // delayed auto shift ms
        const double ARR; // auto repeat rate Hz
        const double ARP; // auto repeat period ms
        const double SDF; // insta drop
        double cur_das_time = DAS;
        double cur_arp_time = ARP;

        void incrementDAS();
        void incrementARR();
        void incrementGravity();
        void hardDrop();

        const int gravity = 60; // frames per line
        int gravity_counter = 0;
        double lock_time;
        double hard_lock_factor;
        double cur_lock_timer;
        double hard_lock_timer;
        bool is_empty_space_below = false;
        void incrementLockTimer();

        void resetLockTimer();
        void resetHardLockTimer();

        bool shm_enabled = false;
        int shmfd;
        unsigned char* shm_game;
        unsigned int shm_size;
        char* shm_path;
        unsigned char shm_cell_full;
        unsigned char shm_cell_empty;
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
            WindowSettings window_settings,
            BoardSettings board_settings,
            GameSettings game_settings,
            SHMSettings shm_settings
        );
        ~Game();

        void play();
    };
}