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
	paused = false;
	vertices.resize(4);
	vertices.setPrimitiveType(sf::Quads);
}

bool SFGOL::pause()
{
	paused = !paused;
	return paused;
}

bool SFGOL::getPause()
{
	return paused;
}

void SFGOL::resizeMap(unsigned int w, unsigned int h)
{
	cellMap.resize(w);
	for (auto &i : cellMap)
		i.resize(h);
	texMap.create(h, w);
	vertices[0].position = static_cast<sf::Vector2f>(sf::Vector2u(texOffset.x, texOffset.y));
	vertices[1].position = static_cast<sf::Vector2f>(sf::Vector2u(texOffset.x + texMap.getSize().x, texOffset.y));
	vertices[2].position = static_cast<sf::Vector2f>(sf::Vector2u(texOffset.x + texMap.getSize().x, texOffset.y + texMap.getSize().y));
	vertices[3].position = static_cast<sf::Vector2f>(sf::Vector2u(texOffset.x, texOffset.y + texMap.getSize().y));
	vertices[0].texCoords = sf::Vector2f(0, 0);
	vertices[1].texCoords = static_cast<sf::Vector2f>(sf::Vector2u(texMap.getSize().x, 0));
	vertices[2].texCoords = static_cast<sf::Vector2f>(sf::Vector2u(texMap.getSize().x, texMap.getSize().y));
	vertices[3].texCoords = static_cast<sf::Vector2f>(sf::Vector2u(0, texMap.getSize().y));
}

void SFGOL::resizeMap(sf::Vector2u size)
{
	resizeMap(size.x, size.y);
}

void SFGOL::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.texture = &texMap;
	target.draw(vertices, states);
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