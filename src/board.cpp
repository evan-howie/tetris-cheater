#include "../includes/board.h"
#include "../includes/mino.h"
#include <iostream>
#include <random>
#include <cstring>

Board::Board(unsigned int w, unsigned int h, unsigned int _tile_size) : width{w}, height{h}, tile_size{_tile_size}{}

Board::~Board(){
    delete[] board;
}

void Board::init(){
    board = new unsigned char[width * height];
    for (unsigned int i = 0; i < width * height; ++i)
        board[i] = mino::EMPTY;

}

// called once per iteration of the game loop
void Board::update(){ }

// called every drop
void Board::clearRows(){
    // shift down starting from all full rows
    for (int row = 0 ; row < height ; ++row){
        while (isRowFull(row)){
            shiftDown(row);
        }
    }
}

void Board::shiftDown(unsigned int start_row){
    // shift all rows down 1 starting from start_row (inclusive)
    for (int y = start_row ; y < height - 1 ; ++y){
        for (int x = 0 ; x < width ; ++x){
            setMino(x, y, getMino(x, y + 1));
        }
    }

    //empty top row
    for (int x = 0 ; x < width ; ++x){
        setMino(x, height - 1, mino::EMPTY);
    }
}

bool Board::isRowFull(unsigned int row){
    for (int x = 0 ; x < width ; ++x){
        if (!isMino(x, row)) return false;
    }
    return true;
}

bool Board::isMino(unsigned int x, unsigned int y){
    // use the msb of cell to represent a tile with something other than a mino (empty, something else)
    return ( board[x + y * width] & mino::EMPTY ) == 0;
}

bool Board::isMino(unsigned char mino){
    return (mino & mino::EMPTY) == 0;
}

bool Board::inBounds(int x, int y){
    return x >= 0 && x < width && y >= 0 && y < height;
}

void Board::draw(sf::RenderWindow* window, unsigned int board_x, unsigned int board_y){
    // draw board cells
    sf::RectangleShape rect(sf::Vector2f(tile_size, tile_size));
    rect.setOutlineColor(mino::OutlineColor);
    rect.setOutlineThickness(1);
    for (int x = 0 ; x < width ; ++x){
        for (int y = 0 ; y < height ; ++y){
            rect.setPosition(x * tile_size + board_x, (height - y - 1) * tile_size + board_y);

            rect.setFillColor(
                mino::color_map.at(isMino(x, y) ? board[x + y * width] : mino::EMPTY)
            );

            window->draw(rect);
        }
    }
}

void Board::print() {
    for (int j = height - 1 ; j >= 0 ; --j) {
        for (int i = 0 ; i < width; ++i) {
            unsigned char cell = board[i + j * width];
            if (cell == mino::EMPTY){
                std::cout << 'x';
            } else {
                std::cout << (unsigned int) cell;
            }
        }
        std::cout << '\n';
    }
    std::cout << std::flush;
}


void Board::setMino(unsigned int x, unsigned int y, char mino){
    board[x + y * width] = mino;
}

unsigned char Board::getMino(unsigned int x, unsigned int y){
    return board[x + y * width];
}

unsigned int Board::getHeight(){
    return height;
}

unsigned int Board::getWidth(){
    return width;
}