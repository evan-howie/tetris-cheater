cmake_minimum_required(VERSION 3.0)
project(tetris)

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

# add libraries
include_directories(include)

# Include your source files
add_executable(
    tetris
    src/main.cpp
    src/game.cpp 
    src/board.cpp 
    src/tetramino.cpp 
    src/next_queue.cpp
    src/held.cpp
)

# Link SFML libraries
target_link_libraries(tetris sfml-graphics sfml-audio)
