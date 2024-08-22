#include <iostream>
#include <SFML/Graphics.hpp>
#include "../includes/game.h"
#include "../includes/settings.h"

int main(int argc, char* argv[]){
    // handle command line arguments (or .env) for future
    Game::WindowSettings window_settings{
        WINDOW_WIDTH,
        WINDOW_HEIGHT
    };

    Game::GameSettings game_settings {
        GAME_LOOP_PERIOD_ms,
        DEFAULT_DAS,
        DEFAULT_ARR,
        DEFAULT_ARP,
        DEFAULT_SDF,
        DEFAULT_LOCK_TIME,
        DEFAULT_HARD_LOCK_FACTOR
    };

    Game::BoardSettings board_settings {
        BOARD_WIDTH,
        BOARD_HEIGHT,
        TILE_SIZE
    };

    Game::Game game{
        window_settings,
        board_settings,
        game_settings,
    };
    game.play();

    return 0;
}