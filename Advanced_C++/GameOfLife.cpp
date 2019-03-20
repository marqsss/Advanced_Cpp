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
		cellMap.resize(width);
		file >> s;
		height = std::stoul(s);
		for (unsigned int i = 0; i < cellMap.size(); i++)
		{
			cellMap.at(i).resize(height);
			for (unsigned int j = 0; j < cellMap.at(i).size(); j++)
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
	else
		return false;
	// else throw BAD_FILEPATH
	file.close();
	return true;
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
		constant = true;
		if (safetycheck)
			for (unsigned int i = 0; i < cellMap.size(); i++)
				for (unsigned int j = 0; j < cellMap.at(i).size(); j++)
					cellMap.at(i).at(j).neighbours = checkNeighbours(i, j);
		for (unsigned int i = 0; i < cellMap.size(); i++)
			for (unsigned int j = 0; j < cellMap.at(0).size(); j++)
				if (update(cellMap.at(i).at(j)))
					constant = false;
		iterCounter++;
	}
	return !constant;
}

// PRIVATE --------------------------------------------------------------------

inline bool GameOfLife::update(Cell& cell)
{
	bool changed = false;
	if (cell.status)
	{
		bool newStatus = false;
		for (unsigned int i = 0; i < lifeMap.size(); i++)
			if (cell.neighbours == lifeMap.at(i))
				newStatus = true;
		if (!newStatus)
			changed = true;
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