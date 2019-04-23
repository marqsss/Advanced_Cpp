#ifndef SFGOL_H
#define SFGOL_H

#include "GameOfLife.h"
#include <SFML/Graphics.hpp>

class SFGOL :public GameOfLife, public sf::Drawable
{
public:
	enum status {
		DEAD = 0,
		ALIVE,
		OPPOSITE
	};
	SFGOL();
	bool pause();
	bool paused();
	void visualize();
	void unrun(int iterations = 1, bool safetycheck = true);
	bool offsetFromRLE(std::filesystem::path, unsigned int, unsigned int);
	bool initialize(sf::Vector2u, sf::Vector2f);
	bool insertFromFile(std::filesystem::path, sf::Vector2u);
	bool run(unsigned int iterations = 1, bool safetycheck = true);
	void describe();
	bool setCell(unsigned int, unsigned int, status, bool overwriteUpdates = true);
	sf::Vector2f getSpriteOffset();
	sf::Texture& getTex();
private:
	void resizeMap(unsigned int, unsigned int);
	void resizeMap(sf::Vector2u);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	bool update(unsigned int, unsigned int);

	static unsigned long totalIterCounter;
	static unsigned long lastValid;
	std::vector<std::vector<sf::Vector2u> > updates;
	sf::Texture texMap;
	sf::Sprite visualization;
	sf::Uint8* dead_cell;
	sf::Uint8* live_cell;
	bool is_paused;
};

#endif