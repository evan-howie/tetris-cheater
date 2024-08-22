#include "../includes/tetramino.h"
#include "../includes/board.h"
#include "../includes/mino.h"
#include <iostream>
#include <algorithm>

Tetramino::Tetramino() :
    shape{},
    pos{0, 0},
    shadow_y{0},
    offsets{}
{}

Tetramino::Tetramino(
    std::vector<std::vector<unsigned char>> &_shape,
    int _x,
    int _y,
    std::vector<std::vector<std::pair<int, int>>> _offsets
) : 
    shape{_shape}, 
    pos{_x, _y}, 
    offsets{_offsets}
{}
// Tetramino::Tetramino(std::vector<std::vector<unsigned char>> &_shape, int ox, int oy) : shape{_shape}, origin_x{ox}, origin_y{oy} {}

bool Tetramino::moveLeft(Board* board){
    --pos.first;
    if(!testShape(board, shape, pos)) {
        ++pos.first;
        return false;
    }
    updateShadow(board);
    return true;
}

bool Tetramino::moveRight(Board* board){
    ++pos.first;
    if(!testShape(board, shape, pos)){
        --pos.first;
        return false;
    }
    updateShadow(board);
    return true;
}

void Tetramino::slideLeft(Board* board){
    while (moveLeft(board)){}
}

void Tetramino::slideRight(Board* board){
    while (moveRight(board)){}
}

void Tetramino::slideDown(Board* board){
    while (moveDown(board)){}
}

bool Tetramino::moveDown(Board* board){
    --pos.second;
    if(!testShape(board, shape, pos)){
        ++pos.second;
        return false;
    }
    return true;
}

void Tetramino::rotateCW(Board* board){
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
    if (applyOffset(board, new_shape, old_rotation, rotation)){
        shape = new_shape;
    }
    else{
        rotation = old_rotation;
    }
    updateShadow(board);
}

void Tetramino::rotateCCW(Board* board){
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
    if (applyOffset(board, new_shape, old_rotation, rotation)){
        shape = new_shape;
    }
    else{
        rotation = old_rotation;
    }
    updateShadow(board);
}

void Tetramino::reset(Board* board){
    init(board);
    pos = defaultPosition.pos;
    while(rotation != defaultPosition.rotation){
        rotateCCW(board);
    }
}

bool Tetramino::applyOffset(Board* board, std::vector<std::vector<unsigned char>>& new_shape, char old_rotation, char rotation){
    for (int i = 0 ; i < offsets[rotation].size() ; ++i){
        std::pair<int, int> offset = getOffset(new_shape, rotation, old_rotation, i);
        if(testShape(board, new_shape, std::pair{pos.first + offset.first, pos.second + offset.second})){
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

void Tetramino::updateShadow(Board* board) {
    int _shadow_y{pos.second + 1}; // start on the +1 because we pre-decrament 

    while(testShape(board, shape, {pos.first, --_shadow_y}));

    shadow_y = _shadow_y + 1;
}

bool Tetramino::testShape(Board* board, std::vector<std::vector<unsigned char>>& test_shape, std::pair<int, int> test_pos){
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

bool Tetramino::testShape(Board* board, std::pair<int, int> test_pos){
    for (int y = 0; y < shape.size(); ++y) {
        for (int x = 0; x < shape[0].size(); ++x) {
            int test_x = test_pos.first + x - origin.first;
            int test_y = test_pos.second + y - origin.second;
            if (isMino(shape, x, y) && (!board->inBounds(test_x, test_y) || board->isMino(test_x, test_y))) {
                return false;
            }
        }
    }
    return true;
}

bool Tetramino::isMino(std::vector<std::vector<unsigned char>>& test_shape, int x, int y){
    return test_shape[y][x] != mino::EMPTY;
}

bool Tetramino::isMino(int x, int y){
    return shape[y][x] != mino::EMPTY;
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

void Tetramino::setPos(int x, int y){
    pos.first = x;
    pos.second = y;
}


void Tetramino::init(Board* board) {
    updateShadow(board);
}

void Tetramino::drawShadow(Board* board, sf::RenderWindow* window, int board_x, int board_y, unsigned int tile_size){
    int x = (pos.first - origin.first) * tile_size + board_x;
    int y = (board->getHeight() - shadow_y - origin.second - 1) * tile_size + board_y;
    
    draw(window, x, y, tile_size, mino::color_map.at(mino::SHADOW));
}

void Tetramino::drawOnBoard(Board* board, sf::RenderWindow* window, int board_x, int board_y, unsigned int tile_size){
    int x = (pos.first - origin.first) * tile_size + board_x;
    int y = (board->getHeight() - pos.second - origin.second - 1) * tile_size + board_y;

    draw(window, x, y, tile_size);
}

void Tetramino::draw(sf::RenderWindow* window, int dx, int dy, unsigned int tile_size){
    sf::RectangleShape rect(sf::Vector2f(tile_size, tile_size));
    rect.setOutlineColor(mino::OutlineColor);
    rect.setOutlineThickness(1);

    for(int y = 0 ; y < shape.size() ; ++y){
        for (int x = 0 ; x < shape[y].size() ; ++x){
            if (!isMino(x, y)) continue;
            int cell_x = (x) * tile_size + dx;
            int cell_y = (shape.size() - y - 1) * tile_size + dy;
            rect.setPosition(cell_x, cell_y);

            rect.setFillColor(mino::color_map.at(shape[y][x]));

            window->draw(rect);
        }
    }
}

void Tetramino::draw(sf::RenderWindow* window, int dx, int dy, unsigned int tile_size, sf::Color color){
    sf::RectangleShape rect(sf::Vector2f(tile_size, tile_size));
    rect.setOutlineColor(mino::OutlineColor);
    rect.setOutlineThickness(1);

    for(int y = 0 ; y < shape.size() ; ++y){
        for (int x = 0 ; x < shape[y].size() ; ++x){
            if (!isMino(x, y)) continue;
            int cell_x = (x) * tile_size + dx;
            int cell_y = (shape.size() - y - 1) * tile_size + dy;
            rect.setPosition(cell_x, cell_y);

            rect.setFillColor(color);

            window->draw(rect);
        }
    }
}

std::tuple<int, int, int, int> Tetramino::getBounds(){
    if (!shape.size()) return {0, 0, 0, 0};
    int y0 = shape.size();
    int y1 = -1;
    int x0 = shape[0].size();
    int x1 = -1;

    for (int y = 0; y < shape.size(); ++y) {
        for (int x = 0; x < shape[y].size(); ++x) {
            if (isMino(x, y)) {
                y0 = std::min(y0, y);
                y1 = std::max(y1, y);
                x0 = std::min(x0, x);
                x1 = std::max(x1, x);
            }
        }
    }

    if (y0 > y1 || x0 > x1) {
        // No non-zero elements found
        return {0, 0, 0, 0};
    }

    int width = x1 - x0 + 1;
    int height = y1 - y0 + 1;

    return {x0, shape.size() - y1 - 1, width, height};

}

std::pair<double, double> Tetramino::getCenter(){
    auto [x, y, w, h] = getBounds();
    return {x + w / 2.0, y + h / 2.0};
}

// create functions

Tetramino createI(){
    unsigned char _ = mino::EMPTY;
    unsigned char x = mino::I;
    std::vector<std::vector<unsigned char>> I_minos{
        {_, _, _, _, _},
        {_, _, _, _, _},
        {_, x, x, x, x},
        {_, _, _, _, _},
        {_, _, _, _, _},
    };
    std::reverse(I_minos.begin(), I_minos.end());

    std::vector<std::vector<std::pair<int, int>>> offsets{
        {{0, 0}, {-1, 0}, {2, 0}, {-1, 0}, {2, 0}},
        {{-1, 0}, {0, 0}, {0, 0}, {0, 1}, {0, -2}},
        {{-1, 1}, {1, 1}, {-2, 1}, {1, 0}, {-2, 0}},
        {{0, 1}, {0, 1}, {0, 1}, {0, -1}, {0, 2}}
    };

    Tetramino piece{I_minos, 4, 21, offsets};
    piece.setOrigin(2, 2);

    return piece;
}


Tetramino createJ(){
    unsigned char _ = mino::EMPTY;
    unsigned char x = mino::J;
    std::vector<std::vector<unsigned char>> J_minos{
        {x, _, _},
        {x, x, x},
        {_, _, _},
    };
    std::reverse(J_minos.begin(), J_minos.end());

    std::vector<std::vector<std::pair<int, int>>> offsets{
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
        {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
        {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}
    };

    Tetramino piece{J_minos, 4, 21, offsets};
    piece.setOrigin(1, 1);

    return piece;
}

Tetramino createL(){
    unsigned char _ = mino::EMPTY;
    unsigned char x = mino::L;
    std::vector<std::vector<unsigned char>> L_minos{
        {_, _, x},
        {x, x, x},
        {_, _, _},
    };
    std::reverse(L_minos.begin(), L_minos.end());

    std::vector<std::vector<std::pair<int, int>>> offsets{
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
        {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
        {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}
    };

    Tetramino piece{L_minos, 4, 21, offsets};
    piece.setOrigin(1, 1);

    return piece;
}

Tetramino createO(){
    unsigned char _ = mino::EMPTY;
    unsigned char x = mino::O;
    std::vector<std::vector<unsigned char>> O_minos{
        {_, x, x},
        {_, x, x},
        {_, _, _},
    };
    std::reverse(O_minos.begin(), O_minos.end());

    std::vector<std::vector<std::pair<int, int>>> offsets{
        {{0, 0}},
        {{0, -1}},
        {{-1, -1}},
        {{-1, 0}}
    };

    Tetramino piece{O_minos, 4, 21, offsets};
    piece.setOrigin(1, 1);

    return piece;
}

Tetramino createS(){
    unsigned char _ = mino::EMPTY;
    unsigned char x = mino::S;
    std::vector<std::vector<unsigned char>> S_minos{
        {_, x, x},
        {x, x, _},
        {_, _, _},
    };
    std::reverse(S_minos.begin(), S_minos.end());

    std::vector<std::vector<std::pair<int, int>>> offsets{
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
        {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
        {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}
    };

    Tetramino piece{S_minos, 4, 21, offsets};
    piece.setOrigin(1, 1);

    return piece;
}

Tetramino createT(){
    unsigned char _ = mino::EMPTY;
    unsigned char x = mino::T;
    std::vector<std::vector<unsigned char>> T_minos{
        {_, x, _},
        {x, x, x},
        {_, _, _},
    };
    std::reverse(T_minos.begin(), T_minos.end());

    std::vector<std::vector<std::pair<int, int>>> offsets{
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
        {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
        {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}
    };

    Tetramino piece{T_minos, 4, 21, offsets};
    piece.setOrigin(1, 1);

    return piece;
}

Tetramino createZ(){
    unsigned char _ = mino::EMPTY;
    unsigned char x = mino::Z;
    std::vector<std::vector<unsigned char>> Z_minos{
        {x, x, _},
        {_, x, x},
        {_, _, _},
    };
    std::reverse(Z_minos.begin(), Z_minos.end());

    std::vector<std::vector<std::pair<int, int>>> offsets{
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
        {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},
        {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}
    };

    Tetramino piece{Z_minos, 4, 21, offsets};
    piece.setOrigin(1, 1);

    return piece;
}