#include "GOL2D.h"
#include <fstream>
#include <string>
#include <random>

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

void GOL2D::resize(sf::Vector2u size)
{
	texMap.create(size.x, size.y);
	cellMap.clear();
	cellMap.resize(size.x);
	for (auto& a : cellMap)
		a.resize(size.y);
	resetTexture();
}

void GOL2D::fillRandomly(unsigned int chance)
{
	std::random_device dice;
	for (auto i = 0; i < cellMap.size(); ++i)
		for (auto j = 0; j < cellMap.at(i).size(); ++j)
		{
			cellMap.at(i).at(j).status = !(dice() % chance);
			texMap.update(cellMap.at(i).at(j).status ? SFGOL::live_cell : SFGOL::dead_cell, 1, 1, i, j);
		}
	visualize();
}

void GOL2D::run()
{
	for (unsigned int i = 0; i < cellMap.size(); i++)
		for (unsigned int j = 0; j < cellMap.at(i).size(); j++)
			cellMap.at(i).at(j).neighbours = checkNeighbours(i, j);
	for (unsigned int i = 0; i < cellMap.size(); i++)
		for (unsigned int j = 0; j < cellMap.at(0).size(); j++)
			updateCell(i, j);
}

void GOL2D::clear()
{
	for (auto i = 0; i < cellMap.size(); ++i)
		for (auto j = 0; j < cellMap.at(i).size(); ++j)
			cellMap.at(i).at(j).status = false;
	resetTexture();
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

unsigned int GOL2D::checkNeighbours(unsigned int x, unsigned int y)
{
	unsigned int res = 0;
	auto test = (x - 1) >cellMap.size()-1 ? 0 : (x - 1);
	auto test2 = (x + 1) > (cellMap.size() - 1) ? (cellMap.size() - 1) : (x + 1);
	switch (neighborhood)
	{
	case VonNeumann:
		if (cellMap.at((x - 1) > cellMap.size() - 1 ? 0 : (x - 1)).at(y).status)
			++res;
		if (cellMap.at((x + 1) > (cellMap.size() - 1) ? (cellMap.size() - 1) : (x + 1)).at(y).status)
			++res;
		if (cellMap.at(x).at((y - 1) > cellMap.size() - 1 ? 0 : (y - 1)).status)
			++res;
		if (cellMap.at(x).at((y + 1) > (cellMap.at(x).size() - 1) ? (cellMap.at(x).size() - 1) : (y + 1)).status)
			++res;
		break;
	case Moore:
		for (int i = static_cast<int>(x) - 1; i < static_cast<int>(x) + 2; i++) // one col west to one col east
			for (int j = static_cast<int>(y) - 1; j < static_cast<int>(y) + 2; j++) // one row north to one row south
			{
				int tempi = 0, tempj = 0;
				if (i < 0)
				{
					tempi = i;
					i = cellMap.size() - 1;
				}
				if (i >= static_cast<int>(cellMap.size()))
				{
					tempi = i;
					i = 0;
				}
				if (j < 0)
				{
					tempj = j;
					j = cellMap.at(i).size() - 1;
				}
				if (j >= static_cast<int>(cellMap.at(i).size()))
				{
					tempj = j;
					j = 0;
				}
				try
				{
					if (cellMap.at(i).at(j).status && (x != i || y != j))
						res++;
				}
				catch (const std::out_of_range&) {/*if out-of-bouds, just ignore and check the rest*/ }
				if (tempi)
					i = tempi;
				if (tempj)
					j = tempj;
			}
		break;
	}
	return res;
}

void GOL2D::updateCell(unsigned int x, unsigned int y)
{
	auto& cell = cellMap.at(x).at(y);
	if (cell.status && !(lifeRule >> cell.neighbours) % 2)
		cell.status = false;
	else if (!cell.status && (reviveRule >> cell.neighbours) % 2)
		cell.status = true;
}