#include "GameOfLife.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>

unsigned long GameOfLife::iterCounter;

GameOfLife::GameOfLife(std::string filename)
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
		cellMap.resize(std::stoul(s));
		file >> s;
		for (int i = 0; i < cellMap.size(); i++)
		{
			cellMap.at(i).resize(stoul(s));
			for (int j = 0; j < cellMap.at(i).size(); j++)
				cellMap.at(i).at(j).status = false;
		}


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
	// else throw BAD_FILEPATH
	file.close();
}

bool GameOfLife::create(std::string filename)
{
	bool status = true;
	std::vector<std::vector<Cell> > tempMap;
	std::vector<unsigned int> tempLMap;
	std::vector<unsigned int> tempRMap;

	std::fstream file(filename);
	if (file.is_open())
	{
		tempMap.resize(1000);
		for (int i = 0; i < tempMap.size(); i++)
			tempMap.at(i).resize(1000);
		unsigned int x;
		std::string s;

		// setting the simulation model
		std::getline(file, s, '/');
		for (int arr = std::stoi(s); arr > 0; arr /= 10)
			tempLMap.insert(tempLMap.begin(), arr % 10);
		std::getline(file, s);
		for (int arr = std::stoi(s); arr > 0; arr /= 10)
			tempRMap.insert(tempRMap.begin(), arr % 10);

		// TODO - recognizing pre-made patterns
		// getting the coords of the starting state
		while (!file.eof())
		{
			file >> s;
			x = std::stoul(s);
			file >> s;

			try {
				tempMap.at(x).at(std::stoul(s)).status = true;
			}
			catch (const std::out_of_range& oor) {
				std::cerr << "Out of Range error: " << oor.what() << '\n';
				status = false;
			}
		}
	}
	// else throw BAD_FILEPATH
	file.close();
	if (status)
	{
		cellMap = tempMap;
		lifeMap = tempLMap;
		reviveMap = tempRMap;
	}
	return status;
}

void GameOfLife::print()
{
	std::string s = "\n";
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
	for (int i = 0; i < lifeMap.size(); i++)
		printf("%i", lifeMap.at(i));
	printf("/");
	for (int i = 0; i < reviveMap.size(); i++)
		printf("%i", reviveMap.at(i));
	printf(" rules, currently sporting ");
	unsigned int living = 0;
	for (unsigned int i = 0; i < cellMap.size(); i++)
		for (unsigned int j = 0; j < cellMap.at(0).size(); j++)
			if (cellMap.at(i).at(j).status)
				living++;

	printf("%i living cells. There were %i iterations so far.", living, iterCounter);
}

bool GameOfLife::run(unsigned int iterations, bool safetycheck)
{
	if (safetycheck)
		for (unsigned int i = 0; i < cellMap.size(); i++)
			for (unsigned int j = 0; j < cellMap.at(i).size(); j++)
				cellMap.at(i).at(j).neighbours = checkNeighbours(i, j);
	for (unsigned int i = 0; i < cellMap.size(); i++)
		for (unsigned int j = 0; j < cellMap.at(0).size(); j++)
			update(cellMap.at(i).at(j));
	iterCounter++;
	return true;
}

// PRIVATE --------------------------------------------------------------------

inline void GameOfLife::update(Cell& cell)
{
	if (cell.status)
	{
		bool newStatus = false;
		for (int i = 0; i < lifeMap.size(); i++)
			if (cell.neighbours == lifeMap.at(i))
				newStatus = true;
		cell.status = newStatus;
	}
	else
	{
		bool newStatus = false;
		for (int i = 0; i < reviveMap.size(); i++)
			if (cell.neighbours == reviveMap.at(i))
				newStatus = true;
		cell.status = newStatus;
	}
}

unsigned int GameOfLife::checkNeighbours(unsigned int x, unsigned int y)
{
	unsigned int res = 0;
	for (int i = static_cast<int>(x) - 1; i < static_cast<int>(x) + 2; i++) // one col west to one col east
		for (int j = static_cast<int>(y) - 1; j < static_cast<int>(y) + 2; j++) // one row north to one row south
		{
			if (i < 0 || j < 0 || i>=cellMap.size() || j>=cellMap.at(0).size())
				continue;
			try
			{
				if (cellMap.at(i).at(j).status && (x != i || y != j))
					res++;
			}
			catch (const std::out_of_range&) {/*if out-of-bouds, just ignore and check the rest*/ }
		}
	return res;
}