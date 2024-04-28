#include "../includes/tetramino.h"
#include "../includes/board.h"
#include <iostream>
#include <algorithm>

Tetramino::Tetramino(){
    std::vector<std::vector<unsigned char>> empty_shape{};
    shape = empty_shape;
    pos = {0, 0};
    offsets = {};
}
Tetramino::Tetramino(Board* _board, std::vector<std::vector<unsigned char>> &_shape, int _x, int _y, std::vector<std::vector<std::pair<int, int>>> _offsets) : board{_board}, shape{_shape}, pos{_x, _y}, offsets{_offsets}{}
// Tetramino::Tetramino(std::vector<std::vector<unsigned char>> &_shape, int ox, int oy) : shape{_shape}, origin_x{ox}, origin_y{oy} {}

bool Tetramino::moveLeft(){
    --pos.first;
    if(!testShape(shape, pos)) {
        ++pos.first;
        return false;
    }
    return true;
}

bool Tetramino::moveRight(){
    ++pos.first;
    if(!testShape(shape, pos)){
        --pos.first;
        return false;
    }
    return true;
}

void Tetramino::slideLeft(){
    while (testShape(shape, std::pair{pos.first - 1, pos.second})){
        --pos.first;
    }
}

void Tetramino::slideRight(){
    while (testShape(shape, std::pair{pos.first + 1, pos.second})){
        ++pos.first;
    }
}

bool Tetramino::moveDown(){
    --pos.second;
    if(!testShape(shape, pos)){
        ++pos.second;
        return false;
    }
    return true;
}

void Tetramino::rotateCW(){
    // Transpose the matrix
    // if (can't rotate) return
    char old_rotation = rotation;
    rotation = (rotation + 1) % 4;

    std::vector<std::vector<unsigned char>> new_shape{shape};

    // Reverse each row
    for (auto& row : new_shape) {
        std::reverse(row.begin(), row.end());
    }
    // Transpose the matrix
    for (int i = 0; i < new_shape.size(); ++i) {
        for (int j = i; j < new_shape[i].size(); ++j) {
            std::swap(new_shape[i][j], new_shape[j][i]);
        }
    }

    // apply offsets
    if (applyOffset(new_shape, old_rotation, rotation)){
        shape = new_shape;
    }
    else{
        rotation = old_rotation;
    }
}

void Tetramino::rotateCCW(){
    // Reverse each row
    // if (can't rotate) return
    char old_rotation = rotation;
    rotation = rotation == 0 ? 3 : rotation - 1;

    std::vector<std::vector<unsigned char>> new_shape{shape};

    // Transpose the matrix
    for (int i = 0; i < new_shape.size(); ++i) {
        for (int j = i; j < new_shape[i].size(); ++j) {
            std::swap(new_shape[i][j], new_shape[j][i]);
        }
    }
    // Reverse each row
    for (auto& row : new_shape) {
        std::reverse(row.begin(), row.end());
    }

    // apply offsets
    if (applyOffset(new_shape, old_rotation, rotation)){
        shape = new_shape;
    }
    else{
        rotation = old_rotation;
    }
}

bool Tetramino::applyOffset(std::vector<std::vector<unsigned char>>& new_shape, char old_rotation, char rotation){
    for (int i = 0 ; i < offsets[rotation].size() ; ++i){
        std::pair<int, int> offset = getOffset(new_shape, rotation, old_rotation, i);
        if(testShape(new_shape, std::pair{pos.first + offset.first, pos.second + offset.second})){
            pos.first += offset.first;
            pos.second += offset.second;
            return true;
        }
    }
    return false;
}

std::pair<int, int> Tetramino::getOffset(std::vector<std::vector<unsigned char>>& test_shape, char rotation, char old_rotation, int test){
    std::pair<int, int> offset = offsets[rotation][test];
    std::pair<int, int> old_offset = offsets[old_rotation][test];
    return std::pair{  old_offset.first - offset.first, old_offset.second - offset.second };
}

bool Tetramino::testShape(std::vector<std::vector<unsigned char>>& test_shape, std::pair<int, int> test_pos){
    for (int y = 0; y < test_shape.size(); ++y) {
        for (int x = 0; x < test_shape[0].size(); ++x) {
            int test_x = test_pos.first + x - origin.first;
            int test_y = test_pos.second + y - origin.second;
            if (isMino(test_shape, x, y) && (!board->inBounds(test_x, test_y) || board->isMino(test_x, test_y))) {
                return false;
            }
        }
    }
    return true;
}

bool Tetramino::isMino(std::vector<std::vector<unsigned char>>& test_shape, int x, int y){
    return test_shape[y][x] != board->empty_cell;
}

bool Tetramino::isMino(int x, int y){
    return shape[y][x] != board->empty_cell;
}

bool Tetramino::isEmpty(){
    return shape.empty();
}

const unsigned char Tetramino::at(int x, int y){ return shape[y][x]; }
const std::vector<std::vector<unsigned char>>& Tetramino::getShape(){return shape;}
const std::pair<int, int>& Tetramino::getPos(){return pos;}
const std::pair<int, int>& Tetramino::getOrigin(){return origin;}

void Tetramino::setOrigin(int x, int y){
    origin.first = x;
    origin.second = y;
}

void Tetramino::draw(sf::RenderWindow& window, int board_x, int board_y){
    sf::RectangleShape rect(sf::Vector2f(board->tile_size, board->tile_size));
    rect.setOutlineColor(sf::Color::Black);
    rect.setOutlineThickness(1);

    for(int y = 0 ; y < shape.size() ; ++y){
        for (int x = 0 ; x < shape[y].size() ; ++x){
            if (!isMino(x, y)) continue;
            int cell_x = (pos.first + x - origin.first) * board->tile_size + board_x;
            int cell_y = (board->getHeight() - pos.second - y - 1 + origin.second) * board->tile_size + board_y;
            rect.setPosition(cell_x, cell_y);

            if(board->isMino(shape[y][x])){
                rect.setFillColor(mino::colors[shape[y][x]]);
            }
            // if(y == origin.second && x == origin.first){
            //     rect.setFillColor(sf::Color::White);
            // }

            window.draw(rect);
        }
    }
}

// create functions

Tetramino createI(Board* board){
    unsigned char x = Board::empty_cell;
    std::vector<std::vector<unsigned char>> I_minos{
        {x, x, x, x, x},
        {x, x, x, x, x},
        {x, 0, 0, 0, 0},
        {x, x, x, x, x},
        {x, x, x, x, x},
    };
    std::reverse(I_minos.begin(), I_minos.end());

    std::vector<std::vector<std::pair<int, int>>> offsets{
        {{0, 0}, {-1, 0}, {2, 0}, {-1, 0}, {2, 0}},
        {{-1, 0}, {0, 0}, {0, 0}, {0, 1}, {0, -2}},
        {{-1, 1}, {1, 1}, {-2, 1}, {1, 0}, {-2, 0}},
        {{0, 1}, {0, 1}, {0, 1}, {0, -1}, {0, 2}}
    };

    Tetramino piece{board, I_minos, 4, 21, offsets};
    piece.setOrigin(2, 2);

    return piece;
}


Tetramino createJ(Board* board){
    unsigned char x = Board::empty_cell;
    std::vector<std::vector<unsigned char>> J_minos{
        {1, x, x},
        {1, 1, 1},
        {x, x, x},
    };
    std::reverse(J_minos.begin(), J_minos.end());

    std::vector<std::vector<std::pair<int, int>>> offsets{
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
        {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
        {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}
    };

    Tetramino piece{board, J_minos, 4, 21, offsets};
    piece.setOrigin(1, 1);

    return piece;
}

Tetramino createL(Board* board){
    unsigned char x = Board::empty_cell;
    std::vector<std::vector<unsigned char>> L_minos{
        {x, x, 2},
        {2, 2, 2},
        {x, x, x},
    };
    std::reverse(L_minos.begin(), L_minos.end());

    std::vector<std::vector<std::pair<int, int>>> offsets{
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
        {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
        {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}
    };

    Tetramino piece{board, L_minos, 4, 21, offsets};
    piece.setOrigin(1, 1);

    return piece;
}

Tetramino createO(Board* board){
    unsigned char x = Board::empty_cell;
    std::vector<std::vector<unsigned char>> O_minos{
        {x, 3, 3},
        {x, 3, 3},
        {x, x, x},
    };
    std::reverse(O_minos.begin(), O_minos.end());

    std::vector<std::vector<std::pair<int, int>>> offsets{
        {{0, 0}},
        {{0, -1}},
        {{-1, -1}},
        {{-1, 0}}
    };

    Tetramino piece{board, O_minos, 4, 21, offsets};
    piece.setOrigin(1, 1);

    return piece;
}

Tetramino createS(Board* board){
    unsigned char x = Board::empty_cell;
    std::vector<std::vector<unsigned char>> S_minos{
        {x, 4, 4},
        {4, 4, x},
        {x, x, x},
    };
    std::reverse(S_minos.begin(), S_minos.end());

    std::vector<std::vector<std::pair<int, int>>> offsets{
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
        {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
        {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}
    };

    Tetramino piece{board, S_minos, 4, 21, offsets};
    piece.setOrigin(1, 1);

    return piece;
}

Tetramino createT(Board* board){
    unsigned char x = Board::empty_cell;
    std::vector<std::vector<unsigned char>> T_minos{
        {x, 5, x},
        {5, 5, 5},
        {x, x, x},
    };
    std::reverse(T_minos.begin(), T_minos.end());

    std::vector<std::vector<std::pair<int, int>>> offsets{
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
        {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
        {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}
    };

    Tetramino piece{board, T_minos, 4, 21, offsets};
    piece.setOrigin(1, 1);

    return piece;
}

Tetramino createZ(Board* board){
    unsigned char x = Board::empty_cell;
    std::vector<std::vector<unsigned char>> Z_minos{
        {6, 6, x},
        {x, 6, 6},
        {x, x, x},
    };
    std::reverse(Z_minos.begin(), Z_minos.end());

    std::vector<std::vector<std::pair<int, int>>> offsets{
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
        {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
        {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}
    };

    Tetramino piece{board, Z_minos, 4, 21, offsets};
    piece.setOrigin(1, 1);

    return piece;
}