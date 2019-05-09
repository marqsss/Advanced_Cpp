#include "GOL2D.h"
#include <fstream>
#include <string>

bool GOL2D::config(std::filesystem::path p, sf::Vector2u size)
{
	std::ifstream file(p.c_str());
	if (file.is_open())
	{
		printf("Config started...\n");
		resize(size);
		std::string s;
		while (!file.eof())
		{
			std::getline(file, s);
			if (s.at(0) != '#') // # are comments
			{
				if (s == "NEIGHBORS")
				{
					std::getline(file, s);
					neighborhood = Neighborhood(std::stoul(s));
				}
				else if (s == "LIFERULE")
				{
					std::getline(file, s);
					lifeRule = std::stoul(s);
				}
				else if (s == "REVIVERULE")
				{
					std::getline(file, s);
					reviveRule = std::stoul(s);
				}
			}
		}
		printf("Config finished.\n");
	}
	file.close();
	if (neighborhood == Neighborhood::Undefined || !cellMap.size() || !cellMap.at(0).size())
		return false;
	return true;
}

// PRIVATE ///////////////////////////////////////////////////////

void GOL2D::resetTexture()
{
	// assure correct texture size
	if (texMap.getSize() != sf::Vector2u(cellMap.size(), cellMap.at(0).size()))
		resize(sf::Vector2u(cellMap.size(), cellMap.at(0).size()));
	// update texture, pixel by pixel
	for (auto i = 0; i < cellMap.size(); ++i)
		for (auto j = 0; j < cellMap.at(i).size(); ++j)
			texMap.update(cellMap.at(i).at(j).status ? SFGOL::live_cell : SFGOL::dead_cell, 1, 1, i, j);
	visualize();
}

void GOL2D::visualize(bool reset)
{
	setTexture(texMap, reset);
}