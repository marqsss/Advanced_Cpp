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
	bool paused();
	void visualize();
	void unrun(unsigned int iterations = 1, bool safetycheck = true);
	bool offsetFromRLE(std::filesystem::path, unsigned int, unsigned int);
	bool initialize(sf::Vector2u, std::filesystem::path, sf::Vector2u);
	sf::Texture& getTex();
private:
	void resizeMap(unsigned int, unsigned int);
	void resizeMap(sf::Vector2u);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	bool update(unsigned int, unsigned int);

	sf::Texture texMap;
	sf::VertexArray vertices;
	sf::Sprite visualization;
	sf::Uint8* dead_cell;
	sf::Uint8* live_cell;
	bool is_paused;
};

#endif