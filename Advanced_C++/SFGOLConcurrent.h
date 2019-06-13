#ifndef SFGOLConcurrent_H
#define SFGOLConcurrent_H

#include <atomic>
#include "SFGOL.h"

class SFGOLConcurrent : public SFGOL
{
public:
	SFGOLConcurrent();
	bool run(unsigned int iterations = 1, bool safetycheck = true);
	bool alt_run(unsigned int iterations = 1, bool safetycheck = true);
	bool seq_run(unsigned int iterations = 1, bool safetycheck = true) { return SFGOL::run(iterations, safetycheck); }
	double getMaxHistories(uint64_t, bool actual = false);
	sf::Vector2u getSize() { return sf::Vector2u(cellMap.size(), cellMap.size() ? cellMap.at(0).size() : 0); }
	void resize(sf::Vector2u size);

private:
	void checkNeighboursAndAssign(unsigned int i, unsigned int j);
	void taskA(unsigned int);
	void taskB(unsigned int);
	void taskC(unsigned int);

	int threadNo;
	static std::atomic_bool constant;
};

#endif