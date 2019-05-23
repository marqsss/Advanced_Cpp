#include "CellularAutomaton.h"


void CellularAutomaton::resize(sf::Vector2u size)
{
	cellMap.clear();
	cellMap.resize(size.x);
	for (auto& a : cellMap)
		a.resize(size.y);
	texMap.create(size.x, size.y);
	imgMap.create(size.x, size.y, sf::Color::White);
	visualize();
}

void CellularAutomaton::clear()
{
	cellMap.clear();
	cellMap.resize(texMap.getSize().x);
	for (auto& a : cellMap)
		a.resize(texMap.getSize().y);
	imgMap.create(texMap.getSize().x, texMap.getSize().y, sf::Color::White);
	visualize();
}

void CellularAutomaton::run()
{
	if (seeds.size())
	{
		for (auto i = 0; i < cellMap.size(); ++i)
			for (auto j = 0; j < cellMap.at(i).size(); ++j)
				if (cellMap.at(i).at(j).is_empty())
					checkNeighbours(i, j);
		for (auto i = 0; i < cellMap.size(); ++i)
			for (auto j = 0; j < cellMap.at(i).size(); ++j)
				if (cellMap.at(i).at(j).needsUpdate())
					updateCell(i, j);
		visualize();
	}
}

void CellularAutomaton::seedRandom(unsigned int n_seeds)
{
	unsigned int colorThreshold = 50;
	seeds.clear();
	auto colorCompare = [](const sf::Color& c1, const sf::Color& c2) ->unsigned int {
		return abs(c1.r - c2.r) + abs(c1.g - c2.g) + abs(c1.b - c2.b);
	}; // for not only same, but too-similar colors

	std::vector<sf::Vector2u> positions;
	//first seed
	if (n_seeds)
	{
		positions.emplace_back(dice() % cellMap.size(), dice() % cellMap.at(0).size());
		seeds.emplace_back(sf::Color(palette == 0 ? 0 : dice() % 255, palette == 1 ? 0 : dice() % 255, palette == 2 ? 0 : dice() % 255, 255));
		while (colorCompare(seeds.back(), sf::Color::White) < colorThreshold)
			seeds.back() = sf::Color(palette == 0 ? 0 : dice() % 255, palette == 1 ? 0 : dice() % 255, palette == 2 ? 0 : dice() % 255, 255);
		updateCell(positions.at(0).x, positions.at(0).y, seeds.at(0));
	}
	// other seeds
	if (n_seeds > 1)
		for (auto s = 1; s < n_seeds; ++s)
		{
			positions.emplace_back(dice() % cellMap.size(), dice() % cellMap.at(0).size());
			bool fresh = true;
			unsigned int boredom = 0;
			do // do not repeat positions
			{
				++boredom;
				for (auto i = 0; i < positions.size() - 1; ++i)
					if (positions.at(i) == positions.back())
						fresh = false;
			}
			while (!fresh && boredom < 10000);
			bool repeat = false;
			sf::Color newseed;
			do // do not repeat colors
			{
				repeat = false;
				newseed = sf::Color(palette == 0 ? 0 : dice() % 255, palette == 1 ? 0 : dice() % 255, palette == 2 ? 0 : dice() % 255, 255);
				for (auto c : seeds)
					if (colorCompare(c, newseed) < colorThreshold || newseed == sf::Color::White)
						repeat = true;
			}
			while (repeat);
			seeds.emplace_back(newseed);
			updateCell(positions.at(s).x, positions.at(s).y, seeds.at(s));
		}
	visualize();
}



// PRIVATE ////////////////////////////////////////////////

void CellularAutomaton::visualize()
{
	texMap.update(imgMap);
	setTexture(texMap, true);
}

void CellularAutomaton::checkNeighbours(unsigned int i, unsigned int j)
{
	if (cellMap.at(i).at(j).is_empty())
	{
		std::vector<unsigned int> nr(seeds.size());
		auto check = [&](unsigned int x, unsigned int y) {
			bool set = false;
			for (auto c = 0; c < seeds.size(); c++)
			{
				if (seeds.at(c) == cellMap.at(x).at(y).color)
				{
					nr.at(c)++;
					set = true;
				}
			}
		};
		if (borderCondition)
		{
			check((i +cellMap.size()-1) % cellMap.size(), j);
			check(i, (j +cellMap.at(i).size()-1) % cellMap.at(i).size());
			check((i + 1) % cellMap.size(), j);
			check(i, (j + 1) % cellMap.at(i).size());
		}
		else
		{
			if (i > 0)
				check(i - 1, j);
			if (j > 0)
				check(i, j - 1);
			if (i < cellMap.size() - 1)
				check(i + 1, j);
			if (j < cellMap.at(i).size() - 1)
				check(i, j + 1);
		}
		unsigned int index = 0;
		for (auto k = 1; k < nr.size(); k++)
		{
			if (nr.at(k) > nr.at(index))
				index = k;
		}
		if (nr.size() && nr.at(index))
			cellMap.at(i).at(j).future = seeds.at(index);
	}
}

void CellularAutomaton::updateCell(unsigned int i, unsigned int j, sf::Color color)
{
	auto& cell = cellMap.at(i).at(j);
	cell.color = cell.future = (color == sf::Color::White ? cell.future : color);
	if (cell.future != sf::Color::White)
		imgMap.setPixel(i, j, cell.color);
	/*sf::Uint8 temp[4];
	temp[3] = 255;
	temp[0] = cell.color.r;
	temp[1] = cell.color.g;
	temp[2] = cell.color.b;
	texMap.update(temp, 1, 1, i, j);*/
}