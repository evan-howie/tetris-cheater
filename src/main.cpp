#include <iostream>
#include <SFML/Graphics.hpp>
#include "../includes/game.h"
#include "../includes/settings.h"

void reader(Board* board);

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
    // handle command line arguments (or .env) for future
    Game game{
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        GAME_LOOP_PERIOD_s,
        BOARD_WIDTH,
        BOARD_HEIGHT,
        TILE_SIZE
    };
    game.play();

    return 0;
}