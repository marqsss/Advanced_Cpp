#include <thread> // sleep
#include <chrono> // sleep
#include "GameOfLife.h"

int main()
{
	GameOfLife Conway("test_input.txt");
	while (true)
	{
		Conway.describe();
		Conway.print();
		Conway.run();
		//std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	return 0;
}