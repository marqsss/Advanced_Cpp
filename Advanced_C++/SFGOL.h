#ifndef SFGOL_H
#define SFGOL_H

#include "GameOfLife.h"
#include <SFML/Graphics.hpp>

class SFGOL :public GameOfLife, public sf::Drawable
{
public:
	SFGOL();
private:
	void resizeMap(unsigned int, unsigned int);
	void resizeMap(sf::Vector2u);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states);
	bool update(unsigned int, unsigned int);
	sf::Texture texMap;
	sf::Sprite sprite;
	sf::Uint8* dead_cell;
	sf::Uint8* live_cell;
};

#endif