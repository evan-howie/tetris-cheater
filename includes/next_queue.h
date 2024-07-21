#ifndef NEXT_QUEUE_H
#define NEXT_QUEUE_H

#include <queue>
#include "tetramino.h"

class NextQueue {
private:
    std::queue<Tetramino> bag;
    std::queue<Tetramino> next_bag;
    std::queue<Tetramino> createBag();
    Board* board;
public:
    NextQueue();
    void init(Board* board);
    Tetramino pop();
    void draw(sf::RenderWindow& window, int board_x, int board_y);
};

#endif