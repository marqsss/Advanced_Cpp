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
		Pentagonal_Left,
		Pentagonal_Top,
		Pentagonal_Right,
		Pentagonal_Bottom,
		Hexagonal_Random = 8,
		Pentagonal_Random,
		Random,
		Monte_Carlo
	};
	enum Palette {
		Green_Blue = 0,
		Red_Blue,
		Red_Green
	};

	CellularAutomaton() :paused(true), colorThreshold(50) {}
	void resize(sf::Vector2u size);
	inline void pause() { paused = !paused; }
	inline bool is_paused() { return paused; }
	void clear();
	void run();
	void seedRandom(unsigned int n_seeds = 10);
	void seedUniform(unsigned int n_seeds = 10);
	// radius: in cell-units (pixel). Default 0 means 10% of the average field dimension
	void seedWithRadius(unsigned int n_seeds = 10, unsigned int radius = 0);
	//void seedByHand(unsigned int seeds = 10);
	sf::Vector2u getSize() { return texMap.getSize(); }
	void setNeighbourhood(Neighbourhood n = VonNeumann) { neighbourhood = n; }
	Neighbourhood getNeighbourhood() { return neighbourhood; }
	void setBC(bool bc) { boundaryCondition = bc; }
	void setPalette(Palette p) { palette = p; }
	void setColorThreshold(unsigned int ct = 50) { colorThreshold = ct; }
	void runMC(double kt = 0);
	void swapVisualization(); // ziarna <-> energia

protected:
	//virtual void draw(sf::RenderTarget& target, sf::RenderStates states) ;

	//void display();
	void visualize(bool MC = false);
	void checkNeighbours(unsigned int, unsigned int);
	void updateCell(unsigned int, unsigned int, sf::Color = sf::Color::White);
	inline unsigned int colorCompare(const sf::Color& c1, const sf::Color& c2);
	void check_radius(std::vector<sf::Vector2u>& positions, unsigned int& radius, unsigned int& boredom);
	Neighbourhood gatherEdges();
	void gatherEdgesNaive();

	std::vector<std::vector<ColorCell>> cellMap;
	std::vector<std::tuple<unsigned int, unsigned int, bool>> edges;
	std::vector<sf::Color> seeds;
	sf::Texture texMap;
	sf::Image imgMap;
	sf::Image MCMap;
	Neighbourhood neighbourhood;
	bool boundaryCondition;
	Palette palette;
	bool paused;
	std::random_device dice;
	unsigned int colorThreshold;
	bool visual_energy;
};

#endif