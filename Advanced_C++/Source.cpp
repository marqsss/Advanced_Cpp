#include <thread> // sleep
#include <chrono> // sleep
#include <fstream>
#include <iostream>
#include <filesystem>
#include <sfml/window.hpp>
#include <SFML/Graphics.hpp>
#include "GameOfLife.h"
#include "SFGOL.h"
#include "QuickButtons.h"

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
			GOL.insertFromFile("../Advanced_C++/resources/GOL_files/Gosper_glider_gun.rle", sf::Vector2u(10, 10));
			valid = true;
			break;
		case 2:
			GOL.initialize(sf::Vector2u(40, 80), sf::Vector2f(10, 10));
			GOL.insertFromFile("../Advanced_C++/resources/GOL_files/Queen_Bee_Shuttle.rle", sf::Vector2u(10, 10));
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
	GOL.setScale(4, 4);
	GOL.describe();
	//GOL.print();

	// buttons setup: playback, back, pause, forward, play
	std::vector<sf::Texture> butTex;
	butTex.resize(5);
	std::vector<sf::Sprite> butSpr;
	butSpr.resize(5);
	for (auto i = 0; i < 5; ++i)
	{
		if (butTex.at(i).loadFromFile("../Advanced_C++/resources/graphics/buttons.bmp", sf::IntRect(100 * i, 0, 100, 100)))
			butSpr.at(i).setTexture(butTex.at(i), true);
		butSpr.at(i).setScale(.5, .5);
		butSpr.at(i).setPosition(540 + 50 * i, 10);
	}

	if (choice)
	{
		sf::ContextSettings settings;
		settings.antialiasingLevel = 0;
		sf::RenderWindow window(sf::VideoMode(800, 600), "Game of Life", sf::Style::Default, settings);
		//window.setView(sf::View(sf::FloatRect(0, 0, 100, 75)));
		sf::Vector2f viewScale = sf::Vector2f(window.getView().getSize().x / window.getSize().x, window.getView().getSize().y / window.getSize().y);
		sf::Vector2f mousePos;
		sf::Event event;

		while (window.isOpen())
		{
			while (window.pollEvent(event))
			{
				switch (event.type)
				{
				case sf::Event::KeyPressed:
					switch (event.key.code)
					{
					case sf::Keyboard::Space: // pause/resume
						//fall-through
					case sf::Keyboard::C:
						GOL.pause();
						GOL.describe();
						break;
					case sf::Keyboard::Z: // set backwards
						if (GOL.paused())
						{
							GOL.forwards(false);
							GOL.describe();
						}
						else
							GOL.pause();
						break;
					case sf::Keyboard::B: // set forwards
						if (GOL.paused())
						{
							GOL.forwards();
							GOL.describe();
						}
						else
							GOL.pause();
						break;
					case sf::Keyboard::X: // prev (if paused)
						if (GOL.paused())
						{
							GOL.unrun();
							GOL.describe();
							//GOL.print();
						}
						break;
					case sf::Keyboard::V: // next (if paused)
						if (GOL.paused())
						{
							GOL.run();
							GOL.describe();
							//GOL.print();
						}
						break;
					}
					break;
				case sf::Event::MouseButtonPressed:
					mousePos = sf::Vector2f(static_cast<float>(sf::Mouse::getPosition(window).x)*viewScale.x,
						static_cast<float>(sf::Mouse::getPosition(window).y)*viewScale.y);
					//printf("Mouse raw pos: (%f:%f)\n", static_cast<float>(sf::Mouse::getPosition(window).x), static_cast<float>(sf::Mouse::getPosition(window).y));
					//printf("Mouse pos: (%f:%f)\n", mousePos.x, mousePos.y);
					// manual entry
					if (GOL.paused() && GOL.contains(mousePos))
						GOL.setCell(static_cast<unsigned int>(mousePos.x*GOL.getScale().x - GOL.getSpriteOffset().x),
							static_cast<unsigned int>(mousePos.y*GOL.getScale().y - GOL.getSpriteOffset().y), SFGOL::OPPOSITE);
					//pause
					if (butSpr.at(2).getGlobalBounds().contains(mousePos))
					{
						printf("pause\n");
						GOL.pause();
						GOL.describe();
					}
					//playback
					if (butSpr.at(0).getGlobalBounds().contains(mousePos))
					{
						printf("playback\n");
						if (GOL.paused())
						{
							GOL.forwards(false);
							GOL.describe();
						}
					}
					//back
					if (butSpr.at(1).getGlobalBounds().contains(mousePos))
					{
						printf("back\n");
						if (GOL.paused())
						{
							GOL.unrun();
							GOL.describe();
							//GOL.print();
						}
					}
					//forward
					if (butSpr.at(3).getGlobalBounds().contains(mousePos))
					{
						printf("forward\n");
						if (GOL.paused())
						{
							GOL.run();
							GOL.describe();
							//GOL.print();
						}
					}
					//play
					if (butSpr.at(4).getGlobalBounds().contains(mousePos))
					{
						printf("play\n");
						if (GOL.paused())
						{
							GOL.forwards();
							GOL.describe();
						}
					}
					break;
				case sf::Event::Closed:
					window.close();
					break;
				} // end event switch
			} // end event handling

			if (!GOL.paused())
			{
				GOL.direction() ? GOL.run() : GOL.unrun();
				//GOL.describe();
				//GOL.print();
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
			}

			window.clear(sf::Color(0, 0, 128, 255));
			window.draw(GOL);
			for (auto i = 0; i < 5; ++i)
				window.draw(butSpr.at(i));
			window.display();
		}
		GOL.describe();
	}

	return 0;
}