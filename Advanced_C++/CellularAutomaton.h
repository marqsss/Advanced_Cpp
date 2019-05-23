#ifndef CELLULARAUTOMATON_H
#define CELLULARAUTOMATON_H

#include <vector>
#include <SFML/Graphics.hpp>
#include <random>
#include "ColorCell.h"

class CellularAutomaton : public sf::Sprite
{
public:
	enum Neighbourhood {
		VonNeumann = 0,
		Moore = 1,
		Hexagonal_Left,
		Hexagonal_Right,
		Hexagonal_Random,
		Pentagonal_Random
	};
	enum Palette {
		Green_Blue = 0,
		Red_Blue,
		Red_Green
	};

	CellularAutomaton() :paused(true) {}
	void resize(sf::Vector2u size);
	inline void pause() { paused = !paused; }
	inline bool is_paused() { return paused; }
	void clear();
	void run();
	//void runMC();
	void seedRandom(unsigned int seeds = 10);
	void seedUniform(unsigned int seeds = 10);
	// radius: in cell-units (pixel). Default 0 means 10% of the average field dimension
	void seedWithRadius(unsigned int seeds = 10, unsigned int radius = 0);
	//void seedByHand(unsigned int seeds = 10);
	sf::Vector2u getSize() { return texMap.getSize(); }
	void setNeighbourhood(Neighbourhood n = VonNeumann) { neighbourhood = n; }
	void setBC(bool bc) { borderCondition = bc; }
	void setPalette(Palette p) { palette = p; }
	
private:
	//void display();
	void visualize();
	void checkNeighbours(unsigned int, unsigned int);
	void updateCell(unsigned int, unsigned int, sf::Color = sf::Color::White);

	std::vector<std::vector<ColorCell>> cellMap;
	std::vector<sf::Color> seeds;
	sf::Texture texMap;
	sf::Image imgMap;
	Neighbourhood neighbourhood;
	bool borderCondition;
	Palette palette;
	bool paused;
	std::random_device dice;
};

#endif