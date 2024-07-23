#include <iostream>
#include <SFML/Graphics.hpp>
#include "../includes/board.h"
#include "../includes/settings.h"

#define WINDOW_WIDTH 700
#define WINDOW_HEIGHT 700
#define TILE_SIZE 25
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 24

void tetris();
void update(Board& board);
void draw(sf::RenderWindow& window, Board& board);
void reader(Board* board);

struct {
    unsigned int x{WINDOW_WIDTH / 2 - BOARD_WIDTH * TILE_SIZE / 2};
    unsigned int y{0};
} board_d;

void tetris(){
    // set up window
    sf::RenderWindow window{sf::VideoMode{WINDOW_WIDTH, WINDOW_HEIGHT}, "tetris"};
    window.setPosition(sf::Vector2i{100, 100});

    // clock
    sf::Clock clock;
    const sf::Time loop_period = sf::seconds(GAME_LOOP_PERIOD_s);

    // board
    Board* board{new Board{BOARD_WIDTH, BOARD_HEIGHT, TILE_SIZE}};
    board->init();

    // main loop
    sf::Time time_since_last = sf::Time::Zero;
    while (window.isOpen()){
        sf::Time dt = clock.restart();
        time_since_last += dt;

        while (time_since_last > loop_period) {
            time_since_last -= loop_period;

            // process events
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
                if (event.type == sf::Event::KeyPressed){
                    switch(event.key.code){
                        case sf::Keyboard::Escape:
                            window.close();
                            break;
                        case sf::Keyboard::R:
                        delete board;
                        board = new Board{BOARD_WIDTH, BOARD_HEIGHT, TILE_SIZE};
                        board->init();
                    }
                }

                board->handleInput(event);
            }

            // update game logic here
            // one tick has passed
            update(*board);
        }

        // display the game state
        draw(window, *board);

    }

    delete board;
}
void update(Board& board){
    board.update();
}

void draw(sf::RenderWindow& window, Board& board){
    window.clear(sf::Color::White);
    board.draw(window, board_d.x, board_d.y);
    window.display();
}

// sumulate a reader
void reader(Board* board){
    const char *shm_name = SHM_PATH;
    int shm_fd = shm_open(shm_name, O_RDONLY, 0666);
    if (shm_fd == -1) {
        std::cerr << "Failed to open shared memory object" << std::endl;
    }

    unsigned char *shm_ptr = (unsigned char*) mmap(0, SHM_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        std::cerr << "Failed to map shared memory object" << std::endl;
    }

    std::cout << shm_ptr << std::endl;

    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

    board->print();

    if (munmap(shm_ptr, SHM_SIZE) == -1) {
        std::cerr << "Failed to unmap shared memory" << std::endl;
    }

    if (close(shm_fd) == -1) {
        std::cerr << "Failed to close shared memory file descriptor" << std::endl;
    }
}

int main(int argc, char* argv[]){
    // handle command line arguments for future
    tetris();
    return 0;
}