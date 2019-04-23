#ifndef GAMEOFLIFE_H
#define GAMEOFLIFE_H

#include <vector>
#include <string>
#include <filesystem>
#include <SFML/Window.hpp>

class GameOfLife
{
public:
	GameOfLife() {};
	GameOfLife(std::string);
	bool create(std::string);
	bool createFromRLE(std::filesystem::path);
	bool offsetFromRLE(std::filesystem::path, unsigned int, unsigned int);
	bool initialize(sf::Vector2u, std::filesystem::path, sf::Vector2u);
	void print();
	void describe();
	bool run(unsigned int iterations = 1, bool safetycheck = true);
protected:
	static unsigned long iterCounter;
	unsigned int width;
	unsigned int height;
	std::string author;
	std::string name;
	std::string description;
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
	bool update(unsigned int, unsigned int);
	unsigned int checkNeighbours(unsigned int, unsigned int);
	inline void resizeMap(unsigned int, unsigned int);
	inline void resizeMap(sf::Vector2u);
};

#endif