#include <thread> // sleep
#include <chrono> // sleep
#include <fstream>
#include <iostream>
#include <filesystem>
#include "GameOfLife.h"

int main()
{
	int choice = -1;
	bool valid = false;
	GameOfLife GOL;
	while (!valid)
		switch (choice)
		{
		case 0:
			std::cout << "Exiting..." << std::endl;
			valid = true;
			break;
		case 1:
			GOL.create("../Advanced_C++/input2.txt");
			valid = true;
			break;
		case 2:
			GOL.create("../Advanced_C++/test_input.txt");
			valid = true;
			break;
		case 3:
			GOL.create("../Advanced_C++/Gosper_glider_gun.txt");
			valid = true;
			break;
		case 4:
			GOL.create("../Advanced_C++/Diehard.txt");
			valid = true;
			break;
		case 5:
			namespace fs = std::filesystem;
			for (auto& p : fs::directory_iterator("."))
				std::cout << p.path() << '\n';
			choice = -1;
			break;
		default:
			printf("Choose input file:\n\
			1. Simple demonstration (block + blinker)\n\
			2. Smallest immortal arrangement\n\
			3. Gosper glider gun\n\
			4. Diehard (dies after 130 steps)\n");
			std::cin >> choice;
			break;
		}

	if (choice)
	{
		GOL.print();
		GOL.describe();
		while (GOL.run())
		{
			GOL.print();
			GOL.describe();
			//std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		GOL.describe();
	}

	return 0;
}