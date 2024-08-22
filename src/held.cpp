#include "../includes/held.h"

void HeldTetramino::draw(sf::RenderWindow* window, int dx, int dy, unsigned int tile_size){
    auto [min_x, __, h_w, ___] = getBounds();
    super::draw(window, dx - (h_w + min_x) * tile_size, dy, tile_size);
}


HeldTetramino& HeldTetramino::operator=(const Tetramino& other) {
        if (this != &other) {
            // Use the assignment operator of the base class
            super::operator=(other);
            // Assign additional members of HeldTetramino here, if any
        }
        return *this;
}

void HeldTetramino::setHasHeld(bool _has_held){
    has_held = _has_held;
}

bool HeldTetramino::getHasHeld(){
    return has_held;
}

