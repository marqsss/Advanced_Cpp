#include <thread>
#include <atomic>
#include "SFGOLConcurrent.h"

std::atomic_bool SFGOLConcurrent::constant;

SFGOLConcurrent::SFGOLConcurrent()
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
	threadNo = std::thread::hardware_concurrency();
}

bool SFGOLConcurrent::run(unsigned int iterations, bool safetycheck)
{
	if (dir && !totalIterCounter) // first iteration
	{
		std::vector<std::thread> workers;
		auto size = cellMap.size() / threadNo;
		for (auto k = 0; k < threadNo; ++k)
		{
			/*auto runPart = [&]() {
				for (int i = size * k + k % 1; i < size*(k + 1); ++i)
					for (auto j = 0; j < cellMap.at(i).size(); j++)
						if (cellMap.at(i).at(j).status)
							texMap.update(live_cell, 1, 1, j, i);
						else
							texMap.update(dead_cell, 1, 1, j, i); };*/
			workers.emplace_back(&SFGOLConcurrent::taskA, this, k);
		}
		for (auto i = 0; i < threadNo; ++i)
			workers.at(i).join();
	}
	if (dir && !constant.load()) // new iteration
	{
		updates.at(iterCounter % 1000).clear();
		std::vector<std::thread> workers;
		auto size = cellMap.size() / threadNo;

		if (safetycheck)
		{
			for (auto k = 0; k < threadNo; ++k)
			{
				/*auto runPart = [&]() {
					for (int i = size * k + k % 1; i < size*(k + 1); ++i)
						for (auto j = 0; j < cellMap.at(i).size(); j++)
							if (cellMap.at(i).at(j).status)
								texMap.update(live_cell, 1, 1, j, i);
							else
								texMap.update(dead_cell, 1, 1, j, i); };*/
				workers.emplace_back(&SFGOLConcurrent::taskC, this, k);
			}
			for (auto i = 0; i < threadNo; ++i)
				workers.at(i).join();
		}
		workers.clear();

		constant.store(true);
		for (auto k = 0; k < threadNo; ++k)
		{
			/*auto runPart = [&]() {
				for (int i = size * k + k % 1; i < size*(k + 1); ++i)
					for (auto j = 0; j < cellMap.at(i).size(); j++)
						if (update(i, j))
							_constant = false;
			};*/
			workers.emplace_back(&SFGOLConcurrent::taskB, this, k);
		}
		for (auto i = 0; i < threadNo; ++i)
			workers.at(i).join();

		if (totalIterCounter == iterCounter)
			++totalIterCounter;
		++iterCounter;
		lastValid = iterCounter;
	}
	else if (!constant.load())
	{
		unrun(-1);
	}
	visualize();
	//printf("_%d",constant.load());
	return !constant.load();
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE
///////////////////////////////////////////////////////////////////////////////

void SFGOLConcurrent::taskA(unsigned int k)
{
	//printf("_a");
	auto size = cellMap.size() / threadNo;
	for (int i = size * k + k % 1; i < size*(k + 1); ++i)
		for (auto j = 0; j < cellMap.at(i).size(); j++)
			if (cellMap.at(i).at(j).status)
				texMap.update(live_cell, 1, 1, j, i);
			else
				texMap.update(dead_cell, 1, 1, j, i);
}

void SFGOLConcurrent::taskB(unsigned int k)
{
	//printf("_b");
	auto size = cellMap.size() / threadNo;
	for (int i = size * k + k % 1; i < size*(k + 1); ++i)
		for (auto j = 0; j < cellMap.at(i).size(); j++)
			if (update(i, j))
				constant.store(false);
}

void SFGOLConcurrent::taskC(unsigned int k)
{
	//printf("_c");
	updates.at(iterCounter % 1000).clear();
	auto size = cellMap.size() / threadNo;
	for (int i = size * k + k % 1; i < size*(k + 1); ++i)
		for (unsigned int j = 0; j < cellMap.at(i).size(); j++)
				cellMap.at(i).at(j).neighbours = checkNeighbours(i, j);
}

/*void SFGOLConcurrent::runPart(unsigned int lower, unsigned int upper, bool safetycheck)
{
	if (dir && !totalIterCounter) // first iteration
	{
		for (unsigned int i = lower; i < upper; i++)
			for (unsigned int j = 0; j < cellMap.at(0).size(); j++)
				if (cellMap.at(i).at(j).status)
					texMap.update(live_cell, 1, 1, j, i);
				else
					texMap.update(dead_cell, 1, 1, j, i);
	}
	if (dir && !constant) // new iteration
	{
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
}*/