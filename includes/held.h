#pragma once

#include "tetramino.h"

class HeldTetramino : public Tetramino {
private:
    typedef Tetramino super;
    bool has_held{false};
public:
    // (dx, dy) is its top-right corner
    void draw(sf::RenderWindow* window, int dx, int dy, unsigned int tile_size);
    void setHasHeld(bool _has_held);
    bool getHasHeld();

    HeldTetramino& operator=(const Tetramino& other);
};