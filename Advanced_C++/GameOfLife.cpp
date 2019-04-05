#include "GameOfLife.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>

unsigned long GameOfLife::iterCounter;

GameOfLife::GameOfLife(std::string filename)
{
	constant = false;
	create(filename);
}

bool GameOfLife::create(std::string filename)
{
	std::fstream file(filename);
	if (file.is_open())
	{
		unsigned int x;
		std::string s;

		// setting the simulation model
		std::getline(file, s, '/');
		for (int arr = std::stoi(s); arr > 0; arr /= 10)
			lifeMap.insert(lifeMap.begin(), arr % 10);
		std::getline(file, s);
		for (int arr = std::stoi(s); arr > 0; arr /= 10)
			reviveMap.insert(reviveMap.begin(), arr % 10);

		// setting matrix size
		file >> s;
		width = std::stoul(s);
		file >> s;
		height = std::stoul(s);
		resizeMap(width, height);


		// TODO - recognizing pre-made patterns
		// getting the coords of the starting state
		while (!file.eof())
		{
			file >> s;
			x = std::stoul(s);
			file >> s;

			try {
				cellMap.at(x).at(std::stoul(s)).status = true;
			}
			catch (const std::out_of_range& oor) {
				std::cerr << "Out of Range error: " << oor.what() << '\n';
			}
		}
	}
	else
		return false;
	// else throw BAD_FILEPATH
	file.close();
	return true;
}

bool GameOfLife::createFromRLE(std::filesystem::path p)
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
				if ((width < y)||(height < x))
				{
					width = y;
					height = x;
					resizeMap(width, height);
				}
				// encode
				unsigned int offset = 0, aliveQueue = 0, k = 0;
				for (unsigned int i = 0; i < cellMap.size(); i++)
					for (unsigned int j = 0; j < cellMap.at(i).size(); j++)
					{
						k = s.find_first_not_of("0123456789", offset);
						if (k == std::string::npos)
						{
							offset = 0;
							std::getline(f, s); // new line of data
							k = s.find_first_not_of("0123456789", offset);
						}
						if (s.at(k) == '$')
						{
							i += (k - offset) < 1 ? 0 : stoul(s.substr(offset, k - offset)) - 1;
							offset = k + 1;
							k = s.find_first_not_of("0123456789", offset);
						}
						if (s.at(k) == 'b')
						{
							j += (k - offset) < 1 ? 0 : stoul(s.substr(offset, k - offset)) - 1;
							offset = k + 1;
						}
						else if (s.at(k) == 'o')
						{
							aliveQueue = (k - offset) < 1 ? 1 : stoul(s.substr(offset, k - offset));
							offset = k + 1;
							for (; aliveQueue > 0; --aliveQueue)
							{
								// printf("%d, %d\t", i, j); // debug
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

bool GameOfLife::offsetFromRLE(std::filesystem::path p, unsigned int offsetX, unsigned int offsetY)
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
				for (unsigned int i = offsetY; i <= offsetY+y; i++)
					for (unsigned int j = offsetX; j <= offsetX+x; j++)
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
							offset = k + 1;
							continue;
						}
						if (s.at(k) == 'b') // jump horizontally
						{
							j += (k - offset) < 1 ? 0 : stoul(s.substr(offset, k - offset)) - 1;
							offset = k + 1;
						}
						else if (s.at(k) == 'o') // revive some cells
						{
							aliveQueue = (k - offset) < 1 ? 1 : stoul(s.substr(offset, k - offset));
							offset = k + 1;
							for (; aliveQueue > 0; --aliveQueue)
							{
								printf("%d, %d\t", i, j); // debug
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

bool GameOfLife::initialize(sf::Vector2u size, std::filesystem::path p, sf::Vector2u offset)
{
	height = size.x;
	width = size.y;
	resizeMap(width, height);
	return offsetFromRLE(p, offset.x, offset.y);
}

void GameOfLife::print()
{
	std::string s = "";
	//adjust viewport: center bounding box to console size 176 178
	// s+=describe()+="View centered on x, y"
	for (unsigned int i = 0; i < cellMap.size(); i++)
	{
		for (unsigned int j = 0; j < cellMap.at(i).size(); j++)
		{
			if (cellMap.at(i).at(j).status)
				s += char(178);
			else
				s += char(176);
		}
		s += "\n";
	}
	printf(s.c_str());
}

void GameOfLife::describe()
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

	printf("%i living cells. There were %i iterations so far.\n", living, iterCounter);
	if (constant)
		printf("The Game is in a constant state.\n");
}

bool GameOfLife::run(unsigned int iterations, bool safetycheck)
{
	if (!constant)
	{
		if (iterCounter < 1000)
			updates.emplace_back();
		else
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
		iterCounter++;
	}
	return !constant;
}

// PRIVATE --------------------------------------------------------------------

bool GameOfLife::update(unsigned int i, unsigned int j)
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
		if (cell.status != newStatus)
			updates.at(iterCounter % 1000).emplace_back(sf::Vector2u(i, j));
		cell.status = newStatus;
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
		if (cell.status != newStatus)
			updates.at(iterCounter % 1000).emplace_back(sf::Vector2u(i, j));
		cell.status = newStatus;
	}
	return changed;
}

unsigned int GameOfLife::checkNeighbours(unsigned int x, unsigned int y)
{
	unsigned int res = 0;
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
	return res;
}

void GameOfLife::resizeMap(unsigned int w, unsigned int h)
{
	cellMap.resize(h);
	for (auto &i : cellMap)
		i.resize(w);
}

void GameOfLife::resizeMap(sf::Vector2u size)
{
	resizeMap(size.x, size.y);
}