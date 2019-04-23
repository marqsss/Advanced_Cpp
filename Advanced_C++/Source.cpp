#include <thread> // sleep
#include <chrono> // sleep
#include <fstream>
#include <iostream>
#include <filesystem>
#include <sfml/window.hpp>
#include <SFML/Graphics.hpp>
#include "GameOfLife.h"
#include "SFGOL.h"

int main()
{
	int choice = -1;
	bool valid = false;
	SFGOL GOL;
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
			GOL.initialize(sf::Vector2u(30, 80), "../Advanced_C++/Gosper_glider_gun.rle", sf::Vector2u(10, 10));
			valid = true;
			break;
		case 6:
			GOL.initialize(sf::Vector2u(40, 80), "../Advanced_C++/Queen_Bee_Shuttle.rle", sf::Vector2u(10, 10));
			valid = true;
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

	// initial state
	GOL.describe();
	GOL.print();

	if (choice)
	{
		GOL.texOffset = sf::Vector2u(50, 50);
		sf::RenderWindow window(sf::VideoMode(800, 600), "Game of Life", sf::Style::Default);
		while (window.isOpen())
		{
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::KeyPressed)
					switch (event.key.code)
					{
					case sf::Keyboard::Space: // pause/resume
						GOL.pause();
						break;
					case sf::Keyboard::Z: // prev (if paused)
						if (GOL.paused())
						{
							GOL.unrun();
							GOL.describe();
							GOL.print();
						}
						break;
					case sf::Keyboard::B: // next (if paused)
						if (GOL.paused())
						{
							GOL.run();
							GOL.describe();
							GOL.print();
						}
						break;
					}
				if (event.type == sf::Event::Closed)
					window.close();
			}

			if (!GOL.paused())
			{
				GOL.run();
				GOL.describe();
				GOL.print();
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
			}

			GOL.visualize();
			sf::Sprite s(GOL.getTex());
			window.clear(sf::Color(0, 0, 128, 255));
			window.draw(s);
			window.display();
		}
		GOL.describe();
	}

	return 0;
}