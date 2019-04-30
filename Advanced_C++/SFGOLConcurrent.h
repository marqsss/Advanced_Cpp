#ifndef SFGOLConcurrent_H
#define SFGOLConcurrent_H

#include "SFGOL.h"

class SFGOLConcurrent : public SFGOL
{
public:
	SFGOLConcurrent();
	bool run(unsigned int iterations = 1, bool safetycheck = true);

private:
	//void runPart(unsigned int, unsigned int, bool);

	int threadNo;
};

#endif