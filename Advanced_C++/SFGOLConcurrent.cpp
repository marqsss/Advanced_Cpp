#include <thread>
#include <atomic>
#include <execution> // for_each(std::execution::par_unseq, ...)
#include <mutex>
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

bool SFGOLConcurrent::alt_run(unsigned int iterations, bool safetycheck)
{
	std::mutex texMapMutex;
	if (dir && !totalIterCounter) // first iteration
	{
		std::for_each(std::execution::par_unseq, cellMap.begin(), cellMap.end(), [&](auto const& row) {
			auto i = std::distance(cellMap.begin(), std::find(cellMap.begin(), cellMap.end(), row));
			std::for_each(std::execution::par_unseq, row.begin(), row.end(), [&](auto const& cell) {
				auto j = std::distance(row.begin(), std::find(row.begin(), row.end(), cell));
				std::lock_guard<std::mutex> lock(texMapMutex);
				if (cell.status)
					texMap.update(live_cell, 1, 1, j, i);
				else
					texMap.update(dead_cell, 1, 1, j, i);
			});
		});

	}
	if (dir && !constant) // new iteration
	{
		updates.at(iterCounter % 1000).clear();
		constant = true;
		if (safetycheck)
			std::for_each(std::execution::par_unseq, cellMap.begin(), cellMap.end(), [&](auto const& row) {
			auto i = std::distance(cellMap.begin(), std::find(cellMap.begin(), cellMap.end(), row));
			std::for_each(std::execution::par_unseq, row.begin(), row.end(), [&](auto const& cell) {
				auto j = std::distance(row.begin(), std::find(row.begin(), row.end(), cell));
				std::lock_guard<std::mutex> lock(texMapMutex);
				checkNeighboursAndAssign(i, j);
			});
		});
		std::for_each(std::execution::par_unseq, cellMap.begin(), cellMap.end(), [&](auto const& row) {
			auto i = std::distance(cellMap.begin(), std::find(cellMap.begin(), cellMap.end(), row));
			std::for_each(std::execution::par_unseq, row.begin(), row.end(), [&](auto const& cell) {
				auto j = std::distance(row.begin(), std::find(row.begin(), row.end(), cell));
				std::lock_guard<std::mutex> lock(texMapMutex);
				if (update(i, j))
					constant = false;
			});
		});

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
}

double SFGOLConcurrent::getMaxHistories(uint64_t capacity, bool actual)
{
	if (actual && updates.size())
	{
		unsigned int updateSize = sizeof(sf::Vector2u);
		unsigned int res = sizeof(updates);
		for (auto v : updates)
		{
			res += sizeof(v);
			if (v.size())
				res += v.size()*updateSize;
		}
		return res;
	}

	unsigned int totalSize = sizeof(cellMap.at(0).at(0))*cellMap.size()*cellMap.at(0).size() +
		sizeof(cellMap) + sizeof(cellMap.at(0))*cellMap.size();
	printf("Expected (kB): %f\n", totalSize / 1024.);
	return 0;
}

void SFGOLConcurrent::resize(sf::Vector2u size)
{
	height = size.x;
	width = size.y;
	resizeMap(width, height);
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE
///////////////////////////////////////////////////////////////////////////////

void SFGOLConcurrent::checkNeighboursAndAssign(unsigned int x, unsigned int y)
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
	cellMap.at(x).at(y).neighbours = res;
}

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