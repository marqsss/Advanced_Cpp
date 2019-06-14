#include <cmath>
#include "CellularAutomaton.h"


void CellularAutomaton::resize(sf::Vector2u size)
{
	cellMap.clear();
	cellMap.resize(size.x);
	for (auto& a : cellMap)
		a.resize(size.y);
	texMap.create(size.x, size.y);
	imgMap.create(size.x, size.y, sf::Color::White);
	MCMap.create(size.x, size.y, sf::Color::Transparent);
	visualize();
}

void CellularAutomaton::clear()
{
	seeds.clear();
	edges.clear();
	cellMap.clear();
	cellMap.resize(texMap.getSize().x);
	for (auto& a : cellMap)
		a.resize(texMap.getSize().y);
	imgMap.create(texMap.getSize().x, texMap.getSize().y, sf::Color::White);
	MCMap.create(texMap.getSize().x, texMap.getSize().y, sf::Color::Transparent);
	visualize();
}

void CellularAutomaton::run()
{
	if (seeds.size())
	{
		if (neighbourhood == Monte_Carlo)
			alt_runMC();
		else
		{
			for (auto i = 0; i < cellMap.size(); ++i)
				for (auto j = 0; j < cellMap.at(i).size(); ++j)
					if (cellMap.at(i).at(j).is_empty())
						checkNeighbours(i, j);
			for (auto i = 0; i < cellMap.size(); ++i)
				for (auto j = 0; j < cellMap.at(i).size(); ++j)
					if (cellMap.at(i).at(j).needsUpdate())
						updateCell(i, j);
		}
		visualize();
	}
}

void CellularAutomaton::seedRandom(unsigned int n_seeds)
{
	clear();

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
			} while (!fresh && boredom < 10000);
			bool repeat = false;
			sf::Color newseed;
			do // do not repeat colors
			{
				repeat = false;
				newseed = sf::Color(palette == 0 ? 0 : dice() % 255, palette == 1 ? 0 : dice() % 255, palette == 2 ? 0 : dice() % 255, 255);
				for (auto c : seeds)
					if (colorCompare(c, newseed) < colorThreshold || newseed == sf::Color::White)
						repeat = true;
			} while (repeat);
			seeds.emplace_back(newseed);
			updateCell(positions.at(s).x, positions.at(s).y, seeds.at(s));
		}
	visualize();
}

void CellularAutomaton::seedUniform(unsigned int n_seeds)
{
	clear();
	// determine rows (h) x cols (w) dimensions of the "grid"
	auto sqrt = std::sqrt(n_seeds);
	unsigned int w = 0, h = 0;
	if (floor(sqrt)*floor(sqrt) == n_seeds)
		w = h = sqrt;
	else if (floor(sqrt)*ceil(sqrt) >= n_seeds)
	{
		w = ceil(sqrt);
		h = floor(sqrt);
	}
	else
		w = h = ceil(sqrt);
	double pos_h = cellMap.at(0).size() / 2. / h;
	double pos_w = cellMap.size() / 2. / w;
	// determine no. of "short" vs "long" rows
	unsigned int s_r = 0;
	s_r = h * w - n_seeds;
	// fill
	auto row_fill = s_r * 1. / h + .5;
	for (auto j = 0; j < h; ++j)
	{
		if (row_fill >= 1.)
		{ // long row
			--row_fill;
			for (auto i = 0; i < w; ++i)
			{
				bool repeat = false;
				sf::Color newseed;
				do // do not repeat colors
				{
					repeat = false;
					newseed = sf::Color(palette == 0 ? 0 : dice() % 255, palette == 1 ? 0 : dice() % 255, palette == 2 ? 0 : dice() % 255, 255);
					for (auto c : seeds)
						if (colorCompare(c, newseed) < colorThreshold || newseed == sf::Color::White)
							repeat = true;
				} while (repeat);
				seeds.emplace_back(newseed);
				updateCell(pos_w * 2 * i + pos_w, pos_h * 2 * j + pos_h, seeds.back());
			}
		}
		else
		{ // short row
			for (auto i = 0; i < w - 1; ++i)
			{
				bool repeat = false;
				sf::Color newseed;
				do // do not repeat colors
				{
					repeat = false;
					newseed = sf::Color(palette == 0 ? 0 : dice() % 255, palette == 1 ? 0 : dice() % 255, palette == 2 ? 0 : dice() % 255, 255);
					for (auto c : seeds)
						if (colorCompare(c, newseed) < colorThreshold || newseed == sf::Color::White)
							repeat = true;
				} while (repeat);
				seeds.emplace_back(newseed);
				updateCell(pos_w * 2 * (i + 1), pos_h * 2 * j + pos_h, seeds.back());
			}
		}
		row_fill += s_r * 1. / h;
	}
	printf("Seeded %u seeds.\n", seeds.size());
	visualize();
}

void CellularAutomaton::seedWithRadius(unsigned int n_seeds, unsigned int radius)
{
	clear();
	if (!radius)
		radius = std::sqrt(cellMap.size()*cellMap.size() + cellMap.at(0).size()*cellMap.at(0).size()) / 5;
	printf("Radius = %u\n", radius);

	std::vector<sf::Vector2u> positions;
	/*std::is_function<bool()> check_radius = [&]() -> bool {
	};*/

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

			unsigned int boredom = 0;
			check_radius(positions, radius, boredom);
			if (boredom >= 100000)
			{
				printf("The program failed to find a suitable place for a new seed. %u seeds were placed so far.\n", seeds.size());
				break;
			}
			else
				printf("Boredom: %u\n", boredom);

			bool repeat = false;
			sf::Color newseed;
			do // do not repeat colors
			{
				repeat = false;
				newseed = sf::Color(palette == 0 ? 0 : dice() % 255, palette == 1 ? 0 : dice() % 255, palette == 2 ? 0 : dice() % 255, 255);
				for (auto c : seeds)
					if (colorCompare(c, newseed) < colorThreshold || newseed == sf::Color::White)
						repeat = true;
			} while (repeat);
			seeds.emplace_back(newseed);
			updateCell(positions.at(s).x, positions.at(s).y, seeds.at(s));
		}
	visualize();
}

void CellularAutomaton::runMC(double kt)
{
	if (kt < .1 || kt >6)
		return;
	auto n = gatherEdges();
	/*gatherEdgesNaive();

	unsigned int n = neighbourhood;
	if (n == Random)
		n = dice() % 8;
	else if (n == Hexagonal_Random)
		n = dice() % 2 + Hexagonal_Left;
	else if (n == Pentagonal_Random)
		n = dice() % 4 + Pentagonal_Left;*/

	auto no_used = 0;
	auto check = [&](unsigned int i, unsigned int j, unsigned int k, unsigned int l)->bool {
		return cellMap.at(i).at(j).color != cellMap.at(k).at(l).color;
	};

	while (no_used < edges.size())
	{
		//printf("Progress: %f\%\n", 1.*no_used / edges.size());
		std::vector<unsigned int> nr(seeds.size());
		// retrieve random tuple and check if it's a first check on this MC-iteration
		auto&[i, j, used] = edges.at(dice() % edges.size());
		if (!used)
		{
			++no_used;
			used = true;
		}
		else
			continue;
		// fill >nr< according to what colors are around, taking care of neighbourhood & boundary condition
		if (boundaryCondition)
		{
			// left
			if (n != Pentagonal_Right && check(i, j, (i + cellMap.size() - 1) % cellMap.size(), j))
				++nr.at(std::distance(seeds.begin(), std::find(seeds.begin(), seeds.end(), cellMap.at((i - 1) % cellMap.size()).at(j).color)));
			// top
			if (n != Pentagonal_Bottom && check(i, j, i, (j - 1) % cellMap.at(i).size()))
				++nr.at(std::distance(seeds.begin(), std::find(seeds.begin(), seeds.end(), cellMap.at(i).at((j - 1) % cellMap.at(i).size()).color)));
			// right
			if (n != Pentagonal_Left && check(i, j, (i + 1) % cellMap.size(), j))
				++nr.at(std::distance(seeds.begin(), std::find(seeds.begin(), seeds.end(), cellMap.at((i + 1) % cellMap.size()).at(j).color)));
			// bottom
			if (n != Pentagonal_Top && check(i, j, i, (j + 1) % cellMap.at(i).size()))
				++nr.at(std::distance(seeds.begin(), std::find(seeds.begin(), seeds.end(), cellMap.at(i).at((j + 1) % cellMap.at(i).size()).color)));
			// top-right
			if ((n == Moore || n == Hexagonal_Right || n == Pentagonal_Top || n == Pentagonal_Right) &&
				check(i, j, (i + 1) % cellMap.size(), (j - 1) % cellMap.at(i).size()))
				++nr.at(std::distance(seeds.begin(), std::find(seeds.begin(), seeds.end(), cellMap.at((i + 1) % cellMap.size()).at((j - 1) % cellMap.at(i).size()).color)));
			// bottom-right
			if ((n == Moore || n == Hexagonal_Left || n == Pentagonal_Right || n == Pentagonal_Bottom) &&
				check(i, j, (i + 1) % cellMap.size(), (j + 1) % cellMap.at(i).size()))
				++nr.at(std::distance(seeds.begin(), std::find(seeds.begin(), seeds.end(), cellMap.at((i + 1) % cellMap.size()).at((j + 1) % cellMap.at(i).size()).color)));
			// bottom-left
			if ((n == Moore || n == Hexagonal_Right || n == Pentagonal_Bottom || n == Pentagonal_Left) &&
				check(i, j, (i - 1) % cellMap.size(), (j + 1) % cellMap.at(i).size()))
				++nr.at(std::distance(seeds.begin(), std::find(seeds.begin(), seeds.end(), cellMap.at((i - 1) % cellMap.size()).at((j + 1) % cellMap.at(i).size()).color)));
			// top-left
			if ((n == Moore || n == Hexagonal_Left || n == Pentagonal_Top || n == Pentagonal_Left) &&
				check(i, j, (i - 1) % cellMap.size(), (j - 1) % cellMap.at(i).size()))
				++nr.at(std::distance(seeds.begin(), std::find(seeds.begin(), seeds.end(), cellMap.at((i - 1) % cellMap.size()).at((j - 1) % cellMap.at(i).size()).color)));
		}
		else
		{
			// left
			if (i > 0 && n != Pentagonal_Right && check(i, j, i - 1, j))
				++nr.at(std::distance(seeds.begin(), std::find(seeds.begin(), seeds.end(), cellMap.at(i - 1).at(j).color)));
			// top
			if (j > 0 && n != Pentagonal_Bottom && check(i, j, i, j - 1))
				++nr.at(std::distance(seeds.begin(), std::find(seeds.begin(), seeds.end(), cellMap.at(i).at(j - 1).color)));
			// right
			if (i < cellMap.size() - 1 && n != Pentagonal_Left && check(i, j, i + 1, j))
				++nr.at(std::distance(seeds.begin(), std::find(seeds.begin(), seeds.end(), cellMap.at(i + 1).at(j).color)));
			// bottom
			if (j < cellMap.at(i).size() - 1 && n != Pentagonal_Top && check(i, j, i, j + 1))
				++nr.at(std::distance(seeds.begin(), std::find(seeds.begin(), seeds.end(), cellMap.at(i).at(j + 1).color)));
			// top-right
			if (j > 0 && i < cellMap.size() - 1 && (n == Moore || n == Hexagonal_Right || n == Pentagonal_Top || n == Pentagonal_Right) && check(i, j, i + 1, j - 1))
				++nr.at(std::distance(seeds.begin(), std::find(seeds.begin(), seeds.end(), cellMap.at(i + 1).at(j - 1).color)));
			// bottom-right
			if (i < cellMap.size() - 1 && j < cellMap.at(i).size() - 1 && n == Moore || n == Hexagonal_Left || n == Pentagonal_Right || n == Pentagonal_Bottom && check(i, j, i + 1, j + 1))
				++nr.at(std::distance(seeds.begin(), std::find(seeds.begin(), seeds.end(), cellMap.at(i + 1).at(j + 1).color)));
			// bottom-left
			if (i > 0 && j < cellMap.at(i).size() - 1 && n == Moore || n == Hexagonal_Right || n == Pentagonal_Bottom || n == Pentagonal_Left && check(i, j, i - 1, j + 1))
				++nr.at(std::distance(seeds.begin(), std::find(seeds.begin(), seeds.end(), cellMap.at(i - 1).at(j + 1).color)));
			// top-left
			if (i > 0 && j > 0 && n == Moore || n == Hexagonal_Left || n == Pentagonal_Top || n == Pentagonal_Left && check(i, j, i - 1, j - 1))
				++nr.at(std::distance(seeds.begin(), std::find(seeds.begin(), seeds.end(), cellMap.at(i - 1).at(j - 1).color)));
		}

		auto index = 0;
		for (auto i = 1; i < nr.size(); ++i)
			if (nr.at(i) > nr.at(index))
				index = i;
		// change to the most energy-efficient color
		if (nr.at(index) && cellMap.at(i).at(j).color != seeds.at(index))
			cellMap.at(i).at(j).future = seeds.at(index);
		// or try to change randomly, if there's no energy-positive option
		else if (exp(-(1.*nr.at(index) - nr.at(std::distance(seeds.begin(), std::find(seeds.begin(), seeds.end(), cellMap.at(i).at(j).color)))) / kt) * 1000 > dice() % 10000)
			cellMap.at(i).at(j).future = seeds.at(dice() % seeds.size());
		//MCMap.setPixel(i, j, cellMap.at(i).at(j).color);
	} // end of MC-iteration
	// update all futures
	for (auto i = 0; i < cellMap.size(); ++i)
		for (auto j = 0; j < cellMap.at(i).size(); ++j)
			updateCell(i, j);
}

void CellularAutomaton::alt_runMC(double kt)
{
	if (kt < .1 || kt >6)
		return;
	gatherEdges();
	int visited = 0;
	while (visited < edges.size())
	{
		auto &[x, y, used] = edges.at(dice() % edges.size());
		if (used)
			continue;
		used = true;
		visited++;
		int energy = 0, swapEnergy = 0;
		std::vector<sf::Color> colors;
		colors.emplace_back(cellMap.at(x).at(y).color);
		if (boundaryCondition)
		{
			for (int i = x - 1; i < x + 1; ++i)
				for (int j = y - 1; j < y + 1; ++j)
				{
					auto& color = cellMap.at(i%cellMap.size()).at(j%cellMap.at(x).size()).color;
					if (std::find(colors.begin(), colors.end(), color) == colors.end())
						colors.emplace_back(color);
				}
			int swapIndex = dice() % colors.size();

			for (int i = x - 1; i < x + 1; ++i)
				for (int j = y - 1; j < y + 1; ++j)
				{
					auto& color = cellMap.at(i%cellMap.size()).at(j%cellMap.at(x).size()).color;
					if (i != x && j != y && color != cellMap.at(x).at(y).color)
						energy++;
					if (i != x && j != y && color != colors.at(swapIndex))
						swapEnergy++;
				}
			auto deltaE = swapEnergy - energy;
			if (deltaE < 0)
				updateCell(x, y, colors.at(swapIndex));
			else if ((exp(-deltaE / kt) * 100) > (dice() % 100))
				updateCell(x, y, colors.at(swapIndex));
		}
		else
		{
		}
	}
}

void CellularAutomaton::swapVisualization()
{
	if (!edges.size() && !visual_energy)
		gatherEdges();
	visual_energy = !visual_energy;
	visualize(visual_energy);
}



// PRIVATE ////////////////////////////////////////////////

void CellularAutomaton::visualize(bool MC)
{
	if (!MC)
	{
		texMap.update(imgMap);
		/*if (edges.size())
			texMap.update(MCMap);*/
	}
	else if (edges.size())
	{
		sf::Image temp;
		temp.create(texMap.getSize().x, texMap.getSize().y, sf::Color::White);
		texMap.update(temp);
		texMap.update(MCMap);
	}
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
		// determine exact neighbourhood type n
		unsigned int n = neighbourhood;
		if (n == Random)
			n = dice() % 8;
		else if (n == Hexagonal_Random)
			n = dice() % 2 + Hexagonal_Left;
		else if (n == Pentagonal_Random)
			n = dice() % 4 + Pentagonal_Left;
		// get all neighbours in relation to n
		if (boundaryCondition)
		{
			// left
			if (n != Pentagonal_Right)
				check((i + cellMap.size() - 1) % cellMap.size(), j);
			// top
			if (n != Pentagonal_Bottom)
				check(i, (j + cellMap.at(i).size() - 1) % cellMap.at(i).size());
			// right
			if (n != Pentagonal_Left)
				check((i + 1) % cellMap.size(), j);
			// bottom
			if (n != Pentagonal_Top)
				check(i, (j + 1) % cellMap.at(i).size());
			// top-right
			if (n == Moore || n == Hexagonal_Right || n == Pentagonal_Top || n == Pentagonal_Right)
				check((i + 1) % cellMap.size(), (j + cellMap.at(i).size() - 1) % cellMap.at(i).size());
			// bottom-right
			if (n == Moore || n == Hexagonal_Left || n == Pentagonal_Right || n == Pentagonal_Bottom)
				check((i + 1) % cellMap.size(), (j + 1) % cellMap.at(i).size());
			// bottom-left
			if (n == Moore || n == Hexagonal_Right || n == Pentagonal_Bottom || n == Pentagonal_Left)
				check((i + cellMap.size() - 1) % cellMap.size(), (j + 1) % cellMap.at(i).size());
			// top-left
			if (n == Moore || n == Hexagonal_Left || n == Pentagonal_Top || n == Pentagonal_Left)
				check((i + cellMap.size() - 1) % cellMap.size(), (j + cellMap.at(i).size() - 1) % cellMap.at(i).size());
		}
		else
		{
			// left
			if (i > 0 && n != Pentagonal_Right)
				check(i - 1, j);
			// top
			if (j > 0 && n != Pentagonal_Bottom)
				check(i, j - 1);
			// right
			if (i < cellMap.size() - 1 && n != Pentagonal_Left)
				check(i + 1, j);
			// bottom
			if (j < cellMap.at(i).size() - 1 && n != Pentagonal_Top)
				check(i, j + 1);
			// top-right
			if (j > 0 && i < cellMap.size() - 1 && (n == Moore || n == Hexagonal_Right || n == Pentagonal_Top || n == Pentagonal_Right))
				check(i + 1, j - 1);
			// bottom-right
			if (i < cellMap.size() - 1 && j < cellMap.at(i).size() - 1 && n == Moore || n == Hexagonal_Left || n == Pentagonal_Right || n == Pentagonal_Bottom)
				check(i + 1, j + 1);
			// bottom-left
			if (i > 0 && j < cellMap.at(i).size() - 1 && n == Moore || n == Hexagonal_Right || n == Pentagonal_Bottom || n == Pentagonal_Left)
				check(i - 1, j + 1);
			// top-left
			if (i > 0 && j > 0 && n == Moore || n == Hexagonal_Left || n == Pentagonal_Top || n == Pentagonal_Left)
				check(i - 1, j - 1);
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
	if (cell.color != sf::Color::White)
		imgMap.setPixel(i, j, cell.color);
}

unsigned int CellularAutomaton::colorCompare(const sf::Color& c1, const sf::Color& c2)
{
	return abs(c1.r - c2.r) + abs(c1.g - c2.g) + abs(c1.b - c2.b);
}

void CellularAutomaton::check_radius(std::vector<sf::Vector2u>& positions, unsigned int& radius, unsigned int& boredom)
{
	++boredom;
	double min = 999;
	for (auto i = 0; i < positions.size() - 1; ++i)
	{
		auto sqrt = std::sqrt(pow(positions.at(i).x > positions.back().x ? positions.at(i).x - positions.back().x : positions.back().x - positions.at(i).x, 2) +
			pow(positions.at(i).y > positions.back().y ? positions.at(i).y - positions.back().y : positions.back().y - positions.at(i).y, 2));
		if (sqrt < radius)
		{
			positions.back() = sf::Vector2u(dice() % cellMap.size(), dice() % cellMap.at(0).size());
			check_radius(positions, radius, boredom);
		}
	}
}

CellularAutomaton::Neighbourhood CellularAutomaton::gatherEdges()
{
	if (edges.size())
		edges.clear();

	auto check = [&](unsigned int i, unsigned int j, unsigned int k, unsigned int l)->bool {
		return cellMap.at(i).at(j).color != cellMap.at(k).at(l).color;
	};

	/*unsigned int n = neighbourhood;
	if (n == Random)
		n = dice() % 8;
	else if (n == Hexagonal_Random)
		n = dice() % 2 + Hexagonal_Left;
	else if (n == Pentagonal_Random)
		n = dice() % 4 + Pentagonal_Left;*/
	auto n = Moore;

	for (auto i = 0; i < cellMap.size(); ++i)
		for (auto j = 0; j < cellMap.at(i).size(); ++j)
		{
			// get all neighbours in relation to n
			if (boundaryCondition)
			{
				// left
				if ((n != Pentagonal_Right) && check(i, j, (i + cellMap.size() - 1) % cellMap.size(), j))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
				// top
				if ((n != Pentagonal_Bottom) && check(i, j, i, (j + cellMap.at(i).size() - 1) % cellMap.at(i).size()))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
				// right
				if ((n != Pentagonal_Left) && check(i, j, (i + 1) % cellMap.size(), j))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
				// bottom
				if ((n != Pentagonal_Top) && check(i, j, i, (j + 1) % cellMap.at(i).size()))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
				// top-right
				if ((n == Moore || n == Hexagonal_Right || n == Pentagonal_Top || n == Pentagonal_Right) &&
					check(i, j, (i + 1) % cellMap.size(), (j + cellMap.at(i).size() - 1) % cellMap.at(i).size()))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
				// bottom-right
				if ((n == Moore || n == Hexagonal_Left || n == Pentagonal_Right || n == Pentagonal_Bottom) &&
					check(i, j, (i + 1) % cellMap.size(), (j + 1) % cellMap.at(i).size()))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
				// bottom-left
				if ((n == Moore || n == Hexagonal_Right || n == Pentagonal_Bottom || n == Pentagonal_Left) &&
					check(i, j, (i + cellMap.size() - 1) % cellMap.size(), (j + 1) % cellMap.at(i).size()))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
				// top-left
				if ((n == Moore || n == Hexagonal_Left || n == Pentagonal_Top || n == Pentagonal_Left) &&
					check(i, j, (i + cellMap.size() - 1) % cellMap.size(), (j + cellMap.at(i).size() - 1) % cellMap.at(i).size()))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
			}
			else
			{
				// left
				if (i > 0 && n != Pentagonal_Right && check(i, j, i - 1, j))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
				// top
				if (j > 0 && n != Pentagonal_Bottom && check(i, j, i, j - 1))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
				// right
				if (i < cellMap.size() - 1 && n != Pentagonal_Left && check(i, j, i + 1, j))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
				// bottom
				if (j < cellMap.at(i).size() - 1 && n != Pentagonal_Top && check(i, j, i, j + 1))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
				// top-right
				if (j > 0 && i < cellMap.size() - 1 && (n == Moore || n == Hexagonal_Right || n == Pentagonal_Top || n == Pentagonal_Right) && check(i, j, i + 1, j - 1))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
				// bottom-right
				if (i < cellMap.size() - 1 && j < cellMap.at(i).size() - 1 && n == Moore || n == Hexagonal_Left || n == Pentagonal_Right || n == Pentagonal_Bottom && check(i, j, i + 1, j + 1))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
				// bottom-left
				if (i > 0 && j < cellMap.at(i).size() - 1 && n == Moore || n == Hexagonal_Right || n == Pentagonal_Bottom || n == Pentagonal_Left && check(i, j, i - 1, j + 1))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
				// top-left
				if (i > 0 && j > 0 && n == Moore || n == Hexagonal_Left || n == Pentagonal_Top || n == Pentagonal_Left && check(i, j, i - 1, j - 1))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
			}
		}
	return Neighbourhood(n);
}

void CellularAutomaton::gatherEdgesNaive()
{
	if (edges.size())
		edges.clear();

	for (auto i = 0; i < cellMap.size(); ++i)
		for (auto j = 0; j < cellMap.at(i).size(); ++j)
		{
			// get all neighbours in Moore neighbourhood
			if (boundaryCondition)
			{
				// left
				if (colorCompare(cellMap.at(i).at(j).color, cellMap.at((i - 1) % cellMap.size()).at(j).color))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
				// top
				if (colorCompare(cellMap.at(i).at(j).color, cellMap.at(i).at((j - 1) % cellMap.at(i).size()).color))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
				// right
				if (colorCompare(cellMap.at(i).at(j).color, cellMap.at((i + 1) % cellMap.size()).at(j).color))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
				// bottom
				if (colorCompare(cellMap.at(i).at(j).color, cellMap.at(i).at((j + 1) % cellMap.at(i).size()).color))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
				// top-right
				if (colorCompare(cellMap.at(i).at(j).color, cellMap.at((i + 1) % cellMap.size()).at((j - 1) % cellMap.at(i).size()).color))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
				// bottom-right
				if (colorCompare(cellMap.at(i).at(j).color, cellMap.at((i + 1) % cellMap.size()).at((j + 1) % cellMap.at(i).size()).color))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
				// bottom-left
				if (colorCompare(cellMap.at(i).at(j).color, cellMap.at((i - 1) % cellMap.size()).at((j + 1) % cellMap.at(i).size()).color))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
				// top-left
				if (colorCompare(cellMap.at(i).at(j).color, cellMap.at((i - 1) % cellMap.size()).at((j - 1) % cellMap.at(i).size()).color))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
			}
			else
			{
				// left
				if (i > 0 && colorCompare(cellMap.at(i).at(j).color, cellMap.at(i - 1).at(j).color))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
				// top
				if (j > 0 && colorCompare(cellMap.at(i).at(j).color, cellMap.at(i).at(j - 1).color))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
				// right
				if (i < cellMap.size() - 1 && colorCompare(cellMap.at(i).at(j).color, cellMap.at(i + 1).at(j).color))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
				// bottom
				if (j < cellMap.at(i).size() - 1 && colorCompare(cellMap.at(i).at(j).color, cellMap.at(i).at(j + 1).color))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
				// top-right
				if (j > 0 && i < cellMap.size() - 1 && colorCompare(cellMap.at(i).at(j).color, cellMap.at(i + 1).at(j - 1).color))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
				// bottom-right
				if (i < cellMap.size() - 1 && j < cellMap.at(i).size() - 1 && colorCompare(cellMap.at(i).at(j).color, cellMap.at(i + 1).at(j + 1).color))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
				// bottom-left
				if (i > 0 && j < cellMap.at(i).size() - 1 && colorCompare(cellMap.at(i).at(j).color, cellMap.at(i - 1).at(j + 1).color))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
				// top-left
				if (i > 0 && j > 0 && colorCompare(cellMap.at(i).at(j).color, cellMap.at(i - 1).at(j - 1).color))
				{
					edges.emplace_back(i, j, false);
					continue;
				}
			}
		}
}