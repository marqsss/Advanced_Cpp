#ifndef COLORCELL_H
#define COLORCELL_H

#include <SFML/Graphics.hpp>

class ColorCell
{
public:

	ColorCell(sf::Color c = sf::Color::White) :color(c), future(sf::Color::White) {}
	bool is_empty() { return color == sf::Color::White ? true : false; }

	sf::Color color;
	sf::Color future;
};

#endif