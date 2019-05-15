#ifndef RAMTEST_H
#define RAMTEST_H

#include <windows.h>
#include <cstdint>

class RAMTest {
public:
	static uint64_t getSafeRAM();
	static uint64_t getPotentialRAM();
	static double getPercentRAM();
	static uint64_t getInstalledRAM();

	static void initialize();

private:

	static MEMORYSTATUSEX memStatus;

	RAMTest() = delete;
};

#endif