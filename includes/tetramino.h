#ifndef TETRAMINO_H
#define TETRAMINO_H

class Board;

#include <vector>
#include <SFML/Graphics.hpp>


class Tetramino {
private:
    std::pair<int, int> offset{0, 0};
    std::pair<int, int> pos;
    std::pair<int, int> origin{1, 1};
    char rotation{0};

    std::vector<std::vector<unsigned char>> shape;
    std::vector<std::vector<std::pair<int, int>>> offsets;

    bool applyOffset(Board* board, std::vector<std::vector<unsigned char>>& test_shape, char old_rotation, char new_rotation);

    struct DefaultPosition {
        std::pair<int, int> pos{4, 21};
        int rotation{0};
    } defaultPosition;

public:
    Tetramino();
    Tetramino(std::vector<std::vector<unsigned char>> &_shape, int _x, int _y, std::vector<std::vector<std::pair<int, int>>> _offsets);

    bool moveLeft(Board* board);
    bool moveRight(Board* board);
    void slideLeft(Board* board);
    void slideRight(Board* board);
    void slideDown(Board* board);
    bool moveDown(Board* board);
    void rotateCW(Board* board);
    void rotateCCW(Board* board);
    void reset(Board* board);
    std::pair<int, int> getOffset(std::vector<std::vector<unsigned char>>& test_shape, char rotation, char old_rotation, int test);

    bool testShape(Board* board, std::vector<std::vector<unsigned char>>& test_shape, std::pair<int, int> test_pos);
    bool testShape(Board* board, std::pair<int, int> test_pos);
    bool isMino(std::vector<std::vector<unsigned char>>& test_shape, int x, int y);
    bool isMino(int x, int y);
    bool isEmpty();

    const unsigned char at(int x, int y);
    const std::vector<std::vector<unsigned char>>& getShape();
    const std::pair<int, int>& getPos();
    const std::pair<int, int>& getOrigin();
    void setOrigin(int x, int y);
    void setPos(int x, int y);
    std::tuple<int, int, int, int> getBounds();
    std::pair<double, double> getCenter();

    void drawOnBoard(Board* board, sf::RenderWindow* window, int board_x, int board_y, unsigned int tile_size);
    void draw(sf::RenderWindow* window, int board_x, int board_y, unsigned int tile_size);
};

Tetramino createI(Board* board);
Tetramino createJ(Board* board);
Tetramino createL(Board* board);
Tetramino createO(Board* board);
Tetramino createS(Board* board);
Tetramino createT(Board* board);
Tetramino createZ(Board* board);

#endif