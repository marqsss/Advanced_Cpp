#include "AdvGameOfLife.h"
#include <fstream>

AdvGameOfLife::AdvGameOfLife(std::string filename)
{
	std::fstream file(filename);
	if (file.is_open())
	{
		std::vector<AdvGameOfLife::GOLCell> tempMap;
		int x;
		std::string s;
		while (!file.eof())
		{
			file >> s;
			x = std::stoi(s);
			file >> s;
			if (!insertCell(tempMap, x, stoi(s)))
				;// throw BAD_ENTRY
		}
	}
	// else throw BAD_FILEPATH
	file.close();
}

bool AdvGameOfLife::create(std::string filename)
{
	// copy from above when rdy PLUS bool success;
}

bool AdvGameOfLife::run(unsigned int iterations)
{

}

bool AdvGameOfLife::plan()
{
	bool changes = false;
	for (unsigned int pos = 0; pos < cellMap.size(); pos++)
	{
		;
	}

	return changes;
}

bool AdvGameOfLife::apply()
{

}

bool AdvGameOfLife::insertCell(std::vector<GOLCell> vector, GOLCell cell)
{
	if (vector.size())
	{
		for (unsigned int pos = 0; pos < vector.size(); pos++)
			if (vector.at(pos).x_pos > cell.x_pos)
				if (vector.at(pos).y_pos > cell.y_pos)
					vector.insert(vector.begin() + pos, cell);
	}
	else
		vector.insert(vector.begin(), cell);
}

bool AdvGameOfLife::insertCell(std::vector<GOLCell> vector, int x, int y)
{
	if (vector.size())
	{
		for (unsigned int pos = 0; pos < vector.size(); pos++)
			if (vector.at(pos).x_pos > x)
				if (vector.at(pos).y_pos > y)
					vector.insert(vector.begin() + pos, GOLCell(x, y));
	}
	else
		vector.insert(vector.begin(), GOLCell(x, y));
}

unsigned int AdvGameOfLife::planFor(GOLCell)
{
	unsigned int neighbours = 0;

	for (unsigned int pos = 0; pos < cellMap.size(); pos++)
		;

	return neighbours;
}

unsigned int AdvGameOfLife::planFor(int x, int y)
{

}