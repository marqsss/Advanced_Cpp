#ifndef GOL2D_H
#define GOL2D_H

#include <vector>
#include <filesystem>
#include <sfml/Graphics.hpp>
#include "BooleanCell.h"
#include "SFGOL.h" // for static live_cell and dead_cell

class GOL2D : public sf::Sprite {
public:
	GOL2D() {}
	bool config(std::filesystem::path, sf::Vector2u);
	inline void flipCell(sf::Vector2u);
	inline void flipCell(unsigned int, unsigned int);
	void resize(sf::Vector2u size);
	inline void pause();
	inline bool is_paused();
	void fillRandomly(unsigned int chance = 2);
	void run();
	void clear();

	enum Neighborhood {
		Undefined = 0,
		VonNeumann = 1,
		Moore = 2,
		FiveLeft = 3,
		FiveRight = 4,
		FiveAlternating = 5,
		FiveRandom = 6,
		SixLeft = 7,
		SixRight = 8,
		SixAlternating = 9,
		SixRandom = 10
	};

private:
	void resetTexture();
	inline void visualize(bool reset = true);
	unsigned int checkNeighbours(unsigned int, unsigned int);
	void updateCell(unsigned int, unsigned int);

	Neighborhood neighborhood;
	unsigned int lifeRule;
	unsigned int reviveRule;
	sf::Texture texMap;
	std::vector<std::vector<BooleanCell>> cellMap;
	bool paused;
	// border condition!
};

// INLINES
void GOL2D::flipCell(sf::Vector2u pos)
{
	cellMap.at(pos.x).at(pos.y).flip();
	texMap.update(cellMap.at(pos.x).at(pos.y).status ? SFGOL::live_cell : SFGOL::dead_cell, 1, 1, pos.x, pos.y);
}

void GOL2D::flipCell(unsigned int pos_x, unsigned int pos_y)
{
	cellMap.at(pos_x).at(pos_y).flip();
	texMap.update(cellMap.at(pos_x).at(pos_y).status ? SFGOL::live_cell : SFGOL::dead_cell, 1, 1, pos_x, pos_y);
}

void GOL2D::pause()
{
	paused = !paused;
}

bool GOL2D::is_paused()
{
	return paused;
}


#endif


/*
create
load data
update tex
update sprite
draw
pause

update pixels by hand
run
*/