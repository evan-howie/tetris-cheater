#ifndef MINO_COLORS_H
#define MINO_COLORS_H

#include <unordered_map>
#include <SFML/Graphics.hpp>

namespace mino {
    const unsigned char I = 0;
    const unsigned char J = 1;
    const unsigned char L = 2;
    const unsigned char O = 3;
    const unsigned char S = 4;
    const unsigned char T = 5;
    const unsigned char Z = 6;
    const unsigned char EMPTY=0x80;

    inline bool isMino(unsigned char m){
        return (m & mino::EMPTY) == 0;
    }

    // Create a map to match enum values to colors
    const std::unordered_map<unsigned char, sf::Color> color_map = {
        {I, sf::Color::Cyan},
        {J, sf::Color::Blue},
        {L, sf::Color{255, 165, 0}},
        {O, sf::Color::Yellow},
        {S, sf::Color::Green},
        {T, sf::Color::Magenta},
        {Z, sf::Color::Red},
        {EMPTY, sf::Color{0x1b1b1bff}}  // Empty cells might be black or transparent
    };
}

#endif // MINO_COLORS_H
