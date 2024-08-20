#pragma once

#include "tetramino.h"

class HeldTetramino : public Tetramino {
private:
    typedef Tetramino super;
public:
    // (dx, dy) is its top-right corner
    void draw(sf::RenderWindow* window, int dx, int dy, unsigned int tile_size);

    HeldTetramino& operator=(const Tetramino& other);
};