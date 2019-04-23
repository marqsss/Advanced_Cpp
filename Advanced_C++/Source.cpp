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
			GOL.initialize(sf::Vector2u(30, 80), sf::Vector2f(10, 10));
			GOL.insertFile("../Advanced_C++/Gosper_glider_gun.rle", sf::Vector2u(10, 10));
			valid = true;
			break;
		case 2:
			GOL.initialize(sf::Vector2u(40, 80), sf::Vector2f(10, 10));
			GOL.insertFile("../Advanced_C++/Queen_Bee_Shuttle.rle", sf::Vector2u(10, 10));
			valid = true;
			break;
		default:
			printf("Choose input file:\n\
			1. Gosper Glider Gun\n\
			2. Queen Bee Shuttle\n");
			std::cin >> choice;
			break;
		}

	// initial state
	GOL.describe();
	//GOL.print();

	if (choice)
	{
		sf::ContextSettings settings;
		settings.antialiasingLevel = 0;
		sf::RenderWindow window(sf::VideoMode(800, 600), "Game of Life", sf::Style::Default, settings);
		window.setView(sf::View(sf::FloatRect(0, 0, 200, 150)));
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
						GOL.describe();
						break;
					case sf::Keyboard::Z: // prev (if paused)
						if (GOL.paused())
						{
							GOL.unrun();
							GOL.describe();
							//GOL.print();
						}
						break;
					case sf::Keyboard::B: // next (if paused)
						if (GOL.paused())
						{
							GOL.unrun(-1);
							GOL.describe();
							//GOL.print();
						}
						break;
					}
				if (event.type == sf::Event::Closed)
					window.close();
			}

			if (!GOL.paused())
			{
				GOL.run();
				//GOL.describe();
				//GOL.print();
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
			}

			window.clear(sf::Color(0, 0, 128, 255));
			window.draw(GOL);
			window.display();
		}
		GOL.describe();
	}

	return 0;
}