#include "SFGOL.h"
#include <string>
#include <fstream>

SFGOL::SFGOL()
{
	dead_cell = new sf::Uint8[4];
	for (int i = 0; i < 3; i++)
		dead_cell[i] = 0;
	dead_cell[3] = 255;
	live_cell = new sf::Uint8[4];
	for (int i = 0; i < 4; i++)
		live_cell[i] = 255;
}

void SFGOL::resizeMap(unsigned int w, unsigned int h)
{
	cellMap.resize(h);
	for (auto &i : cellMap)
		i.resize(w);
	texMap.create(w, h);
}

void SFGOL::resizeMap(sf::Vector2u size)
{
	resizeMap(size.x, size.y);
}

void SFGOL::draw(sf::RenderTarget& target, sf::RenderStates states)
{
	sprite.setTexture(texMap, true);
	target.draw(sprite, states);
}

bool SFGOL::update(unsigned int i, unsigned int j)
{
	bool changed = false;
	auto &cell = cellMap.at(i).at(j);
	if (cell.status)
	{
		bool newStatus = false;
		for (unsigned int i = 0; i < lifeMap.size(); i++)
			if (cell.neighbours == lifeMap.at(i))
				newStatus = true;
		if (!newStatus)
			changed = true;
		if (changed)
		{
			texMap.update(dead_cell, 1, 1, i, j);
			updates.at(iterCounter % 1000).emplace_back(sf::Vector2u(i, j));
			cell.status = newStatus;
		}
	}
	else
	{
		bool newStatus = false;
		for (unsigned int i = 0; i < reviveMap.size(); i++)
			if (cell.neighbours == reviveMap.at(i))
			{
				newStatus = true;
				changed = true;
			}
		if (changed)
		{
			texMap.update(live_cell, 1, 1, i, j);
			updates.at(iterCounter % 1000).emplace_back(sf::Vector2u(i, j));
			cell.status = newStatus;
		}
	}
	return changed;
}