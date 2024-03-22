#include "../includes/board.h"
#include <iostream>

Board::Board(unsigned int w, unsigned int h, unsigned int _tile_size) : width{w}, height{h}, tile_size{_tile_size}{
    board = new unsigned char[width * height];
    for (unsigned int i = 0; i < width * height; ++i)
        board[i] = empty_cell;
}

Board::~Board(){
    delete[] board;
}

void Board::init(){
    //TODO: im sure theres something we need to init
    cur_piece = createI(this);
}

// called once per iteration of the game loop
void Board::update(){
    // TODO: gravity
    // if (++gravity_counter == gravity){
    //     gravity_counter = 0;
    //     cur_piece.moveDown();
    // }

    clearRows();

}

void Board::handleInput(sf::Event e){
    if (e.type != sf::Event::KeyPressed && e.type != sf::Event::KeyReleased) return;
    
    if(e.type == sf::Event::KeyPressed){
        switch(e.key.code){
            case sf::Keyboard::Left:
                cur_piece.moveLeft();
                break;
            case sf::Keyboard::Right:
                cur_piece.moveRight();
                break;
            case sf::Keyboard::Down:
                cur_piece.moveDown();
                break;
            case sf::Keyboard::Space:
                break;
            case sf::Keyboard::Z:
                cur_piece.rotateCCW();
                break;
            case sf::Keyboard::X:
                cur_piece.rotateCW();
                break;
            default:
                break;
        }
    } else {

    }
}


// called every drop
void Board::clearRows(){
    // shift down starting from all full rows
    for (int row = 0 ; row < height ; ++row){
        if (isRowFull(row)){
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
        setMino(x, height - 1, empty_cell);
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
    return ( board[x + y * width] & empty_cell ) == 0;
}

bool Board::isMino(unsigned char mino){
    return (mino & empty_cell) == 0;
}

bool Board::inBounds(int x, int y){
    return x >= 0 && x < width && y >= 0 && y < height;
}

void Board::draw(sf::RenderWindow& window, unsigned int board_x, unsigned int board_y){
    // draw board cells
    sf::RectangleShape rect(sf::Vector2f(tile_size, tile_size));
    rect.setOutlineColor(sf::Color::Black);
    rect.setOutlineThickness(1);
    for (int x = 0 ; x < width ; ++x){
        for (int y = 0 ; y < height ; ++y){
            rect.setPosition(x * tile_size + board_x, (height - y - 1) * tile_size + board_y);

            rect.setFillColor(isMino(x, y) ?
                mino::colors[board[x + y * width]] :
                empty_cell_color);

            window.draw(rect);
        }
    }

    // draw current piece
    cur_piece.draw(window, board_x, board_y);
}

void Board::setMino(unsigned int x, unsigned int y, char mino){
    board[x + y * width] = mino;
}

char Board::getMino(unsigned int x, unsigned int y){
    return board[x + y * width];
}

unsigned int Board::getHeight(){
    return height;
}