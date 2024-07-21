#ifndef MINO_COLORS_H
#define MINO_COLORS_H

#include <SFML/Graphics.hpp>

namespace mino {
    inline sf::Color cyan{sf::Color::Cyan};
    inline sf::Color blue{sf::Color::Blue};
    inline sf::Color orange{sf::Color{255, 165, 0}};
    inline sf::Color yellow{sf::Color{255, 255, 0}};
    inline sf::Color green{sf::Color::Green};
    inline sf::Color purple{sf::Color::Magenta};
    inline sf::Color red{sf::Color::Red};
    
    inline sf::Color colors[] = {cyan, blue, orange, yellow, green, purple, red};
}

#endif // MINO_COLORS_H
