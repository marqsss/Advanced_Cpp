#include <thread>
#include <atomic>
#include "SFGOLConcurrent.h"

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
		std::vector<std::thread> workers(threadNo);
		auto size = cellMap.size() / threadNo;
		for (auto k = 0; k < threadNo; ++k)
		{
			for (auto i = k * size; i < (k + 1)*size; i++)
			{
				auto runPart = [&]() {for (auto j = 0; j < cellMap.at(0).size(); j++)
					if (cellMap.at(i).at(j).status)
						texMap.update(live_cell, 1, 1, j, i);
					else
						texMap.update(dead_cell, 1, 1, j, i); };
				workers.emplace_back(runPart);
			}
		}
		for (auto i = 0; i < threadNo; ++i)
			workers.at(i).join();
	}
	if (dir && !constant) // new iteration
	{
		updates.at(iterCounter % 1000).clear();
		std::atomic_bool _constant(true);
		std::vector<std::thread> workers(threadNo);
		auto size = cellMap.size() / threadNo;

		if (safetycheck)
		{
			for (auto k = 0; k < threadNo; ++k)
			{
				for (auto i = k * size; i < (k + 1)*size; i++)
				{
					auto runPart = [&]() {for (auto j = 0; j < cellMap.at(i).size(); j++)
						cellMap.at(i).at(j).neighbours = checkNeighbours(i, j);
					};
					workers.emplace_back(runPart);
				}
			}
			for (auto i = 0; i < threadNo; ++i)
				workers.at(i).join();
		}

		for (auto k = 0; k < threadNo; ++k)
		{
			for (auto i = k * size; i < (k + 1)*size; i++)
			{
				workers.emplace_back([&]() {for (auto j = 0; j < cellMap.at(i).size(); j++)
					if (update(i, j))
						_constant = false;
				});
			}
			for (auto i = 0; i < threadNo; ++i)
				workers.at(i).join();
		}
		if (totalIterCounter == iterCounter)
			++totalIterCounter;
		++iterCounter;
		lastValid = iterCounter;
		constant = _constant.load();
	}
	else if (!constant)
	{
		unrun(-1);
	}
	visualize();
	return !constant;
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE
///////////////////////////////////////////////////////////////////////////////

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