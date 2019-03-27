#ifndef GAMEOFLIFE_H
#define GAMEOFLIFE_H

#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class GameOfLife
{
public:
	GameOfLife() {};
	GameOfLife(std::string);
	bool create(std::string);
	bool createFromRLE(fs::path);
	void print();
	void describe();
	bool run(unsigned int iterations = 1, bool safetycheck = true);
private:
	static unsigned long iterCounter;
	unsigned int width;
	unsigned int height;
	bool constant;
	class Cell
	{
	public:
		unsigned int neighbours;
		bool status;
	};
	std::vector<std::vector<Cell> > cellMap;
	std::vector<unsigned int> lifeMap;
	std::vector<unsigned int> reviveMap;
	inline bool update(Cell& cell);
	unsigned int checkNeighbours(unsigned int, unsigned int);
};

#endif