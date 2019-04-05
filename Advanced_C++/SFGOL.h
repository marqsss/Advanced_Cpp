#ifndef SFGOL_H
#define SFGOL_H

#include "GameOfLife.h"
#include <SFML/Graphics.hpp>

class SFGOL :public GameOfLife, public sf::Drawable
{
public:
	SFGOL();
	sf::Vector2u texOffset;
	bool pause();
	bool getPause();
private:
	void resizeMap(unsigned int, unsigned int);
	void resizeMap(sf::Vector2u);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	bool update(unsigned int, unsigned int);
	sf::Texture texMap;
	sf::VertexArray vertices;
	sf::Uint8* dead_cell;
	sf::Uint8* live_cell;
	bool paused;
};

#endif