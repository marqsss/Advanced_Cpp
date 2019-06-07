#ifndef COLORCELL_H
#define COLORCELL_H

#include <SFML/Graphics.hpp>

class ColorCell
{
public:

	ColorCell(sf::Color c = sf::Color::White) :color(c), future(sf::Color::White), dislocation_density(0), recrystallized(0) {}
	bool is_empty() { return color == sf::Color::White ? true : false; }
	bool needsUpdate() { return color != future; };

	sf::Color color;
	sf::Color future;
	double dislocation_density;
	unsigned int recrystallized;
};

#endif