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
	bool unrun(int iterations = 1, bool safetycheck = true);
	bool offsetFromRLE(std::filesystem::path, unsigned int, unsigned int);
	bool initialize(sf::Vector2u, sf::Vector2f);
	bool insertFromFile(std::filesystem::path, sf::Vector2u);
	bool run(unsigned int iterations = 1, bool safetycheck = true);
	void describe();
	bool setCell(unsigned int, unsigned int, status, bool overwriteUpdates = true);
	sf::Vector2f getSpriteOffset();
	bool contains(sf::Vector2i);
	bool contains(sf::Vector2f);
	bool contains(int, int);
	void forwards(bool b = true) { dir = b; }
	bool direction() { return dir; }
	void setScale(float x, float y) { visualization.setScale(x, y); }
	sf::Vector2f getScale() { return visualization.getScale(); }
	sf::Texture& getTex();

	static sf::Uint8* dead_cell;
	static sf::Uint8* live_cell;

protected:
	void resizeMap(unsigned int, unsigned int);
	void resizeMap(sf::Vector2u);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	bool update(unsigned int, unsigned int);

	static unsigned long totalIterCounter;
	static unsigned long lastValid;
	std::vector<std::vector<sf::Vector2u> > updates;
	sf::Texture texMap;
	sf::Sprite visualization;
	bool is_paused;
	bool dir;
};

#endif