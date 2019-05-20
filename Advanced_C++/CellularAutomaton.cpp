#include "CellularAutomaton.h"


void CellularAutomaton::resize(sf::Vector2u size)
{
	cellMap.clear();
	cellMap.resize(size.x);
	for (auto& a : cellMap)
		a.resize(size.y);
	/*texMap.create(size.x, size.y);
	texMap.clear(sf::Color::Yellow);*/
	tex2.create(size.x, size.y);
	//display();
	visualize();
}

void CellularAutomaton::clear()
{
	resize(tex2.getSize());
}

void CellularAutomaton::run()
{
	for (auto i = 0; i < cellMap.size(); ++i)
		for (auto j = 0; j < cellMap.at(i).size(); ++j)
			checkNeighbours(i, j);
	for (auto i = 0; i < cellMap.size(); ++i)
		for (auto j = 0; j < cellMap.at(i).size(); ++j)
			updateCell(i, j);
	//display();
	visualize();
}

void CellularAutomaton::seed(unsigned int seeds)
{
	std::vector<sf::Vector2u> positions;
	//first seed
	if (seeds)
	{
		positions.emplace_back(dice() % cellMap.size(), dice() % cellMap.size());
		updateCell(positions.at(0).x, positions.at(0).y, sf::Color(dice() % 255, dice() % 255, dice() % 255, 255));
	}
	// other seeds
	if (seeds > 1)
		for (auto s = 1; s < seeds; ++s)
		{
			positions.emplace_back(dice() % cellMap.size(), dice() % cellMap.size());
			bool fresh = true;
			unsigned int boredom = 0;
			do // check for repeated positions
			{
				++boredom;
				for (auto i = 0; i < positions.size() - 1; ++i)
					if (positions.at(i) == positions.back())
						fresh = false;
			} while (!fresh && boredom < 10000);
			updateCell(positions.at(s).x, positions.at(s).y, sf::Color(dice() % 255, dice() % 255, dice() % 255, 255));
		}
}


// PRIVATE ////////////////////////////////////////////////
/*
void CellularAutomaton::display()
{
	texMap.display();
}
*/
void CellularAutomaton::update(sf::Texture& tex)
{
	setTexture(tex, true);
}

void CellularAutomaton::checkNeighbours(unsigned int i, unsigned int j)
{
	if (cellMap.at(i).at(j).empty())
	{
		std::vector<sf::Color> colors;
		std::vector<unsigned int> nr;
		auto check = [&](unsigned int x, unsigned int y) {
			bool set = false;
			for (auto c = 0; c < colors.size(); c++)
			{
				if (colors.at(c) == cellMap.at(x).at(y).color)
				{
					nr.at(c)++;
					set = true;
				}
			}
			if (!set && cellMap.at(x).at(y).color != sf::Color::White)
			{
				colors.emplace_back(cellMap.at(x).at(y).color);
				nr.emplace_back(1);
			}
		};

		if (i > 0)
			check(i - 1, j);
		if (j > 0)
			check(i, j - 1);
		if (i < cellMap.size() - 1)
			check(i + 1, j);
		if (j < cellMap.at(i).size() - 1)
			check(i, j + 1);

		unsigned int index = 0;
		if (nr.size() > 1)
			for (auto k = 1; k < nr.size(); k++)
			{
				if (nr.at(k) > nr.at(index))
					index = k;
			}
		if (nr.size())
			cellMap.at(i).at(j).future = colors.at(index);
	}
}

void CellularAutomaton::updateCell(unsigned int i, unsigned int j, sf::Color color)
{
	auto& cell = cellMap.at(i).at(j);
	cell.color = cell.future = (color == sf::Color::White ? cell.future : color);
	sf::Vertex pixel[] = { sf::Vertex(sf::Vector2f(0,   0), cell.color, sf::Vector2f(0,  0)), };
	//texMap.draw(pixel, 1, sf::Points);
	sf::Uint8 temp[4];
	temp[3] = 255;
	temp[0] = cell.color.r;
	temp[1] = cell.color.g;
	temp[2] = cell.color.b;
	tex2.update(temp, 1, 1, i, j);
}