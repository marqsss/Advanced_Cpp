#include "RAMTest.h"

MEMORYSTATUSEX RAMTest::memStatus;


uint64_t RAMTest::getSafeRAM()
{
	return memStatus.ullAvailPhys;
}

uint64_t RAMTest::getPotentialRAM()
{
	return memStatus.ullTotalPhys;
}

double RAMTest::getPercentRAM()
{
	return memStatus.dwMemoryLoad;
}

uint64_t RAMTest::getInstalledRAM()
{
	uint64_t temp;
	GetPhysicallyInstalledSystemMemory(&temp);
	return temp;
}

void RAMTest::initialize()
{
	memStatus.dwLength = sizeof(memStatus);
	GlobalMemoryStatusEx(&memStatus);
}