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
    const unsigned char SHADOW=0x81;

    inline bool isMino(unsigned char m){
        return (m & mino::EMPTY) == 0;
    }

    // Create a map to match enum values to colors
    const std::unordered_map<unsigned char, sf::Color> color_map = {
        {I, sf::Color{0x66ccccff}},
        {J, sf::Color{0x6666ccff}},
        {L, sf::Color{0xffb266ff}},
        {O, sf::Color{0xffffccff}},
        {S, sf::Color{0x99cc99ff}},
        {T, sf::Color{0xb399ffff}},
        {Z, sf::Color{0xff6666ff}},
        {EMPTY, sf::Color{0x1b1b1bff}},  // Empty cells might be black or transparent
        {SHADOW, sf::Color{0x2b2b2bff}}
    };

    const sf::Color OutlineColor = sf::Color{0x00000000};
}

#endif // MINO_COLORS_H
