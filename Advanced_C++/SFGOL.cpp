#include "SFGOL.h"
#include <iostream>
#include <string>
#include <fstream>

unsigned long SFGOL::totalIterCounter;
unsigned long SFGOL::lastValid;

SFGOL::SFGOL()
{
	dead_cell = new sf::Uint8[4];
	for (int i = 0; i < 3; i++)
		dead_cell[i] = 0;
	dead_cell[3] = 255;
	live_cell = new sf::Uint8[4];
	for (int i = 0; i < 4; i++)
		live_cell[i] = 255;
	is_paused = false;
	dir = true;
}

bool SFGOL::pause()
{
	is_paused = !is_paused;
	return is_paused;
}

bool SFGOL::paused()
{
	return is_paused;
}

void SFGOL::resizeMap(unsigned int w, unsigned int h)
{
	cellMap.resize(h);
	for (auto &i : cellMap)
		i.resize(w);
	texMap.create(w, h);
	for (unsigned int i = 0; i < h; ++i)
		for (unsigned int j = 0; j < w; ++j)
			texMap.update(dead_cell, 1, 1, j, i);
}

void SFGOL::resizeMap(sf::Vector2u size)
{
	resizeMap(size.x, size.y);
}

void SFGOL::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(visualization, states);
}

bool SFGOL::update(unsigned int i, unsigned int j)
{
	bool changed = false;
	auto &cell = cellMap.at(i).at(j);
	if (cell.status)
	{
		//should it die?
		bool newStatus = false;
		for (unsigned int i = 0; i < lifeMap.size(); i++)
			if (cell.neighbours == lifeMap.at(i))
				newStatus = true;
		if (!newStatus)
			changed = true;
		if (changed)
		{
			texMap.update(dead_cell, 1, 1, j, i);
			updates.at(totalIterCounter % 1000).emplace_back(sf::Vector2u(j, i));
			cell.status = newStatus;
		}
	}
	else
	{
		//should it be born?
		bool newStatus = false;
		for (unsigned int i = 0; i < reviveMap.size(); i++)
			if (cell.neighbours == reviveMap.at(i))
			{
				newStatus = true;
				changed = true;
			}
		if (changed)
		{
			texMap.update(live_cell, 1, 1, j, i);
			updates.at(iterCounter % 1000).emplace_back(sf::Vector2u(j, i));
			cell.status = newStatus;
		}
	}
	return changed;
}

bool SFGOL::run(unsigned int iterations, bool safetycheck)
{
	if (dir && !totalIterCounter) // first iteration
	{
		for (unsigned int i = 0; i < cellMap.size(); i++)
			for (unsigned int j = 0; j < cellMap.at(0).size(); j++)
				if (cellMap.at(i).at(j).status)
					texMap.update(live_cell, 1, 1, j, i);
				else
					texMap.update(dead_cell, 1, 1, j, i);
	}
	if (dir && !constant ) // new iteration
	{
		updates.at(iterCounter % 1000).clear();
		constant = true;
		if (safetycheck)
			for (unsigned int i = 0; i < cellMap.size(); i++)
				for (unsigned int j = 0; j < cellMap.at(i).size(); j++)
					cellMap.at(i).at(j).neighbours = checkNeighbours(i, j);
		for (unsigned int i = 0; i < cellMap.size(); i++)
			for (unsigned int j = 0; j < cellMap.at(0).size(); j++)
				if (update(i, j))
					constant = false;
		if (totalIterCounter == iterCounter)
			++totalIterCounter;
		++iterCounter;
		lastValid = iterCounter;
	}
	else if (!constant)
	{
		unrun(-1);
	}
	visualize();
	return !constant;
}

void SFGOL::describe()
{
	printf("Game of life with ");
	for (unsigned int i = 0; i < lifeMap.size(); i++)
		printf("%i", lifeMap.at(i));
	printf("/");
	for (unsigned int i = 0; i < reviveMap.size(); i++)
		printf("%i", reviveMap.at(i));
	printf(" rules, currently sporting ");
	unsigned int living = 0;
	for (unsigned int i = 0; i < cellMap.size(); i++)
		for (unsigned int j = 0; j < cellMap.at(0).size(); j++)
			if (cellMap.at(i).at(j).status)
				living++;
	if (iterCounter == totalIterCounter)
		printf("%i living cells. There were %i iterations so far.\n", living, totalIterCounter);
	else
		printf("%i living cells. Iteration %i out of %i total so far.\n", living, iterCounter, totalIterCounter);
	if (constant)
		printf("The Game is in a constant state.\n");
}

bool SFGOL::setCell(unsigned int i, unsigned int j, status newStatus, bool overwriteUpdates)
{
	if (j < height&& i < width)
		if (newStatus == OPPOSITE)
		{
			newStatus = status(static_cast<int>(!cellMap.at(j).at(i).status));
			texMap.update(newStatus ? live_cell : dead_cell, 1, 1, i, j);
			cellMap.at(j).at(i).status = newStatus;
			for (auto k = 0; k < updates.at(iterCounter).size(); ++k)
				if (updates.at(iterCounter).at(k).x == i && updates.at(iterCounter).at(k).y == j)
					updates.at(iterCounter).erase(updates.at(iterCounter).begin() + k);
			lastValid = iterCounter;
			return true;
		}
		else if (cellMap.at(j).at(i).status != static_cast<bool>(newStatus))
		{
			texMap.update(newStatus ? live_cell : dead_cell, 1, 1, i, j);
			cellMap.at(j).at(i).status = newStatus;
			for (auto k = 0; k < updates.at(iterCounter).size(); ++k)
				if (updates.at(iterCounter).at(k).x == i && updates.at(iterCounter).at(k).y == j)
					updates.at(iterCounter).erase(updates.at(iterCounter).begin() + k);
			lastValid = iterCounter;
			return true;
		}
	return false;
}

sf::Vector2f SFGOL::getSpriteOffset()
{
	return visualization.getPosition();
}

bool SFGOL::contains(sf::Vector2i point)
{
	return visualization.getGlobalBounds().contains(point.x, point.y);
}
bool SFGOL::contains(sf::Vector2f point)
{
	return visualization.getGlobalBounds().contains(point);
}
bool SFGOL::contains(int x, int y)
{
	return visualization.getGlobalBounds().contains(x, y);
}

void SFGOL::visualize()
{
	visualization.setTexture(texMap, true);
}

bool SFGOL::unrun(int iterations, bool safetycheck)
{
	if (!iterations)
		return false;
	if (iterations < 0 && iterCounter < totalIterCounter)
	{
		//forwards
		for (auto k = iterations; k < 0; ++k)
		{
			if (lastValid < totalIterCounter)
				run();
			else
			{
				auto changes = updates.at(iterCounter % 1000);
				for (auto ch : changes)
				{
					if (cellMap.at(ch.y).at(ch.x).status)
					{
						texMap.update(dead_cell, 1, 1, ch.x, ch.y);
						cellMap.at(ch.y).at(ch.x).status = false;
					}
					else
					{
						texMap.update(live_cell, 1, 1, ch.x, ch.y);
						cellMap.at(ch.y).at(ch.x).status = true;
					}
				}
				++iterCounter;
			}
		}
	}
	else
	{
		//backwards
		for (auto k = 0; k < iterations; ++k)
		{
			if (iterCounter + 999 == totalIterCounter || iterCounter == 0)
			{
				std::cerr << "You cannot go backwards anymore. Try moving forward." << std::endl;
				return false;
			}
			else
			{
				--iterCounter;
				auto changes = updates.at(iterCounter % 1000);
				for (auto ch : changes)
				{
					if (cellMap.at(ch.y).at(ch.x).status)
					{
						texMap.update(dead_cell, 1, 1, ch.x, ch.y);
						cellMap.at(ch.y).at(ch.x).status = false;
					}
					else
					{
						texMap.update(live_cell, 1, 1, ch.x, ch.y);
						cellMap.at(ch.y).at(ch.x).status = true;
					}
				}
			}
		}
	}
	visualize();
	return true;
}

bool SFGOL::offsetFromRLE(std::filesystem::path p, unsigned int offsetX, unsigned int offsetY)
{
	std::ifstream f(p.c_str());
	if (f.is_open())
	{
		std::string s;
		unsigned int x, y;
		while (!f.eof())
		{
			std::getline(f, s);
			if (s.at(0) == '#')
			{
				if (s.at(1) == 'N')
					name += s.substr(3, s.size());
				else if (s.at(1) == 'O')
					author += s.substr(3, s.size());
				else if (s.at(1) == 'C' || s.at(1) == 'c')
					description += s.substr(3, s.size()) += '\n';
				// else ignore - invalid input
			}
			else if (s.at(0) == 'x')
			{
				x = std::stoul(s.substr(s.find_first_of("0123456789"),
					s.find_first_of(',') - s.find_first_of("0123456789")));
				y = std::stoul(s.substr(s.find_first_of("0123456789", s.find_first_of(',')),
					s.find_first_of(',', s.find_first_of(',') + 1) -
					s.find_first_of("0123456789", s.find_first_of(','))));
				std::string rule1 = s.substr(s.find_last_of('=') + 2,
					s.find_last_of('/') - s.find_last_of('=') - 2);
				std::string rule2 = s.substr(s.find_last_of('/') + 1, s.size());
				if (rule1.at(0) == 'B')
					for (int arr = std::stoi(rule1.substr(1)); arr > 0; arr /= 10)
						reviveMap.insert(reviveMap.begin(), arr % 10);
				else if (rule1.at(0) == 'S')
					for (int arr = std::stoi(rule1.substr(1)); arr > 0; arr /= 10)
						lifeMap.insert(lifeMap.begin(), arr % 10);
				if (rule2.at(0) == 'B')
					for (int arr = std::stoi(rule2.substr(1)); arr > 0; arr /= 10)
						reviveMap.insert(reviveMap.begin(), arr % 10);
				else if (rule2.at(0) == 'S')
					for (int arr = std::stoi(rule2.substr(1)); arr > 0; arr /= 10)
						lifeMap.insert(lifeMap.begin(), arr % 10);
				// else ignore - invalid input
				// print 'commented' data
				printf("%s\n%s\n%s", name.c_str(), author.c_str(), description.c_str());
			}
			else
			{
				// ensure map size
				if ((width < x) || (height < y))
				{
					width = y;
					height = x;
					resizeMap(width, height);
				}
				// encode
				unsigned int offset = 0, aliveQueue = 0;
				size_t k = 0;
				for (unsigned int i = offsetY; i <= offsetY + y; i++)
					for (unsigned int j = offsetX; j <= offsetX + x; j++)
					{
						k = s.find_first_not_of("0123456789", offset);
						if (k == std::string::npos) // read next line of data
						{
							offset = 0;
							std::getline(f, s); // new line of data
							k = s.find_first_not_of("0123456789", offset);
						}
						if (s.at(k) == '$') // jump vertically
						{
							i += (k - offset) < 1 ? 0 : stoul(s.substr(offset, k - offset)) - 1;
							//j = cellMap.at(i).size();
							offset = static_cast<unsigned int>(k) + 1;
							continue;
						}
						if (s.at(k) == 'b') // jump horizontally
						{
							j += (k - offset) < 1 ? 0 : stoul(s.substr(offset, k - offset)) - 1;
							offset = static_cast<unsigned int>(k) + 1;
						}
						else if (s.at(k) == 'o') // revive some cells
						{
							aliveQueue = (k - offset) < 1 ? 1 : stoul(s.substr(offset, k - offset));
							offset = static_cast<unsigned int>(k) + 1;
							for (; aliveQueue > 0; --aliveQueue)
							{
								//printf("%d, %d\t", i, j); // debug
								cellMap.at(i).at(j).status = true;
								if (aliveQueue - 1)
									++j;
							}
						}
					}
			}
		}
	}
	else
		return false;
	f.close();
	return true;
}

bool SFGOL::initialize(sf::Vector2u size, sf::Vector2f spriteOffset)
{
	height = size.x;
	width = size.y;
	resizeMap(width, height);
	updates.resize(1000);
	visualization.setPosition(spriteOffset);
	return true;
}

bool SFGOL::insertFromFile(std::filesystem::path p, sf::Vector2u offset)
{
	return offsetFromRLE(p, offset.x, offset.y);
}

sf::Texture& SFGOL::getTex()
{
	return texMap;
}