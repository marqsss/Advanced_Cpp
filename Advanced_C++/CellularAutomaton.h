#ifndef CELLULARAUTOMATON_H
#define CELLULARAUTOMATON_H

#include <vector>
#include <SFML/Graphics.hpp>
#include <random>
#include "ColorCell.h"

class CellularAutomaton : public sf::Sprite
{
public:
	CellularAutomaton() :paused(true) {}
	void resize(sf::Vector2u size);
	inline void pause() { paused = !paused; }
	inline bool is_paused() { return paused; }
	void clear();
	void run();
	void seed(unsigned int seeds = 10);
	sf::Vector2u getSize() { return texMap.getSize(); }
	//const sf::Texture& getTexture() { return texMap.getTexture(); }
	void update(sf::Texture& tex);

private:
	//void display();
	void visualize() { setTexture(texMap, true); }
	void checkNeighbours(unsigned int, unsigned int);
	void updateCell(unsigned int, unsigned int, sf::Color = sf::Color::White);

	std::vector<std::vector<ColorCell>> cellMap;
	std::vector<sf::Color> seeds;
	sf::Texture texMap;
	bool paused;
	std::random_device dice;
};

#endif