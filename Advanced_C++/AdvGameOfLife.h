#ifndef ADVGAMEOFLIFE_H
#define ADVGAMEOFLIFE_H

#include <vector>
#include <string>

class AdvGameOfLife
{
public:
	AdvGameOfLife() {};
	// Create a cell map from a text file consisting of living cell coordinate touples separated by newlines.
	AdvGameOfLife(std::string filename);
	// Create a new cell map from a text file. Destroys any existing cell map and update plan if successful.
	// Returns true if successful; false otherwise.
	bool create(std::string filename);
	// Run simulation over the cell map, applying changes after each iteration.
	// Returns true if any cell changed status; false otherwise.
	bool run(unsigned int iterations = 1);
	// Iterate over the cell map, but do not apply results; store them internally instead as an update plan.
	// Returns true if any cell changed status; false otherwise.
	bool plan();
	// Apply the update plan to the cell map. Does nothing if plan() wasn't called since the last map update.
	// Returns true if there was an update plan to apply; false otherwise.
	bool apply();
private:
	class GOLCell
	{
	public:
		GOLCell(int x, int y): x_pos(x), y_pos(y) {};
		int x_pos;
		int y_pos;
	};
	std::vector<GOLCell> cellMap;
	std::vector<GOLCell> updatePlan;
	bool insertCell(std::vector<GOLCell>, GOLCell);
	bool insertCell(std::vector<GOLCell>, int, int);
	unsigned int planFor(GOLCell);
	unsigned int planFor(int x, int y);

};

#endif