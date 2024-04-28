#include <vector>
#include <SFML/Graphics.hpp>

class Board;

#ifndef TETRAMINO_H
#define TETRAMINO_H
class Tetramino {
private:
    std::pair<int, int> offset{0, 0};
    std::pair<int, int> pos;
    std::pair<int, int> origin{1, 1};
    char rotation{0};

    std::vector<std::vector<unsigned char>> shape;
    std::vector<std::vector<std::pair<int, int>>> offsets;

    bool testShape(std::vector<std::vector<unsigned char>>& test_shape, std::pair<int, int> test_pos);
    bool applyOffset(std::vector<std::vector<unsigned char>>& test_shape, char old_rotation, char new_rotation);

    Board* board;
public:
    Tetramino();
    Tetramino(Board* _board, std::vector<std::vector<unsigned char>> &_shape, int _x, int _y, std::vector<std::vector<std::pair<int, int>>> _offsets);

    bool moveLeft();
    bool moveRight();
    void slideLeft();
    void slideRight();
    void slideDown();
    bool moveDown();
    void rotateCW();
    void rotateCCW();
    std::pair<int, int> getOffset(std::vector<std::vector<unsigned char>>& test_shape, char rotation, char old_rotation, int test);

    bool isMino(std::vector<std::vector<unsigned char>>& test_shape, int x, int y);
    bool isMino(int x, int y);
    bool isEmpty();

    const unsigned char at(int x, int y);
    const std::vector<std::vector<unsigned char>>& getShape();
    const std::pair<int, int>& getPos();
    const std::pair<int, int>& getOrigin();
    void setOrigin(int x, int y);

    void draw(sf::RenderWindow& window, int board_x, int board_y);
};

Tetramino createI(Board* board);
Tetramino createJ(Board* board);
Tetramino createL(Board* board);
Tetramino createO(Board* board);
Tetramino createS(Board* board);
Tetramino createT(Board* board);
Tetramino createZ(Board* board);

#endif