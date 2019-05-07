#ifndef SFGOLConcurrent_H
#define SFGOLConcurrent_H

#include <atomic>
#include "SFGOL.h"

class SFGOLConcurrent : public SFGOL
{
public:
	SFGOLConcurrent();
	bool run(unsigned int iterations = 1, bool safetycheck = true);

private:
	void taskA(unsigned int);
	void taskB(unsigned int);
	void taskC(unsigned int);

	int threadNo;
	static std::atomic_bool constant;
};

#endif