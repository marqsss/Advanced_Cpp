#ifndef GAMEOFLIFE_H
#define GAMEOFLIFE_H

#include <vector>
#include <string>

class GameOfLife
{
public:
	GameOfLife() {};
	GameOfLife(std::string);
	bool create(std::string);
	void print();
	void describe();
	bool run(unsigned int iterations = 1, bool safetycheck = true);
private:
	static unsigned long iterCounter;
	class Cell
	{
	public:
		unsigned int neighbours;
		bool status;
	};
	std::vector<std::vector<Cell> > cellMap;
	std::vector<unsigned int> lifeMap;
	std::vector<unsigned int> reviveMap;
	inline void update(Cell& cell);
	unsigned int checkNeighbours(unsigned int, unsigned int);
};

#endif