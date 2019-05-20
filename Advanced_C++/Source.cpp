#include <thread> // sleep
#include <chrono> // sleep
#include <fstream>
#include <iostream>
#include <filesystem>
#include <sfml/window.hpp>
#include <SFML/Graphics.hpp>
#include "GameOfLife.h"
#include "SFGOL.h"
#include "GOL1D.h"
#include "SFGOLConcurrent.h"
#include "SimpleFPS.h"
#include "QuickButtons.h"
#include "GOL2D.h"
#include "RAMTest.h"
#include "CellularAutomaton.h"

int main()
{
	SimpleFPS fps;
	int choice = -1;
	bool valid = false;
	unsigned int w, i, r;
	SFGOLConcurrent GOL;
	GOL1D oneD;
	GOL2D twoD;
	unsigned int twoDScale;
	RAMTest::initialize();
	CellularAutomaton ca;

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
		case 3:
			std::cout << "Width, iterations, rule:\n";
			std::cin >> w;
			std::cin >> i;
			std::cin >> r;
			oneD.create(w, i, r);
			oneD.setCell(oneD.getWidth() / 2);
			valid = true;
			break;
		case 4:
			std::cout << "Scale:\n";
			std::cin >> twoDScale;
			if (twoD.config("../Advanced_C++/resources/GOL2D/config.txt", sf::Vector2u(500 / twoDScale, 500 / twoDScale)))
				valid = true;
			else
				choice = 0;
			break;
		case 5:
			ca.resize(sf::Vector2u(20,20));
			valid = true;
			break;
		default:
			printf("Choose input file:\n\
			1. Gosper Glider Gun\n\
			2. Queen Bee Shuttle\n\
			3. One-dimensional GOL\n\
			4. Two-dimensional GOL\n\
			5. Zarodkowanie 1\n");
			std::cin >> choice;
			break;
		}

	if (choice == 1 || choice == 2)
	{
		printf("Max histories: %f\n", GOL.getMaxHistories(RAMTest::getSafeRAM()));
		// initial state
		GOL.setScale(4, 4);
		GOL.describe();
		//GOL.print();


		if (choice)
		{
			sf::ContextSettings settings;
			settings.antialiasingLevel = 0;
			sf::RenderWindow window(sf::VideoMode(800, 600), "Game of Life", sf::Style::Default, settings);
			//window.setView(sf::View(sf::FloatRect(0, 0, 100, 75)));
			sf::Vector2f viewScale = sf::Vector2f(window.getView().getSize().x / window.getSize().x, window.getView().getSize().y / window.getSize().y);
			sf::Vector2f mousePos;
			sf::Event event;

			// buttons setup: playback, back, pause, forward, play
			QuickButtons butSpr;
			for (auto i = 0; i < 5; ++i)
			{
				butSpr.newButton("../Advanced_C++/resources/graphics/buttons.bmp",
					sf::IntRect(100 * i, 0, 100, 100), sf::Vector2f(540 + 50 * i, 10));
				butSpr.getButton(i).setScale(.5, .5);
			}

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
						case sf::Keyboard::N: // histories
							if (GOL.paused())
							{
								printf("Max safe RAM: %u\n", RAMTest::getSafeRAM());
								printf("Histories: %f\n", GOL.getMaxHistories(RAMTest::getSafeRAM(), true));
								printf("Max histories: %f\n", static_cast<long double>(RAMTest::getSafeRAM()) / GOL.getMaxHistories(RAMTest::getSafeRAM(), true));
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
						if (butSpr.getButton(2).getGlobalBounds().contains(mousePos))
						{
							printf("pause\n");
							GOL.pause();
							GOL.describe();
						}
						//playback
						if (butSpr.getButton(0).getGlobalBounds().contains(mousePos))
						{
							printf("playback\n");
							if (GOL.paused())
							{
								GOL.forwards(false);
								GOL.describe();
							}
						}
						//back
						if (butSpr.getButton(1).getGlobalBounds().contains(mousePos))
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
						if (butSpr.getButton(3).getGlobalBounds().contains(mousePos))
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
						if (butSpr.getButton(4).getGlobalBounds().contains(mousePos))
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
					//std::this_thread::sleep_for(std::chrono::milliseconds(200));
				}

				window.clear(sf::Color(0, 0, 128, 255));
				window.draw(GOL);
				for (auto i = 0; i < 5; ++i)
					window.draw(butSpr.getButton(i));
				//window.draw(butSpr);
				window.display();

				//printf("FPS: %f\n", fps(60));
			} // while window.is_open

			GOL.describe();
		} // window closes
	} // standard GOL stuff
	else if (choice == 3) // one-dimensional stuff
	{
		// button setup
		QuickButtons butSpr;
		for (auto i = 0; i < 5; ++i)
		{
			butSpr.newButton("../Advanced_C++/resources/graphics/buttons.bmp",
				sf::IntRect(100 * i, 0, 100, 100), sf::Vector2f(540 + 50 * i, 10));
			butSpr.getButton(i).setScale(.5, .5);
		}
		oneD.setScale(2, 2);

		// window setup
		sf::ContextSettings settings;
		settings.antialiasingLevel = 0;
		sf::RenderWindow window(sf::VideoMode(800, 600), "Game of Life", sf::Style::Default, settings);
		//window.setView(sf::View(sf::FloatRect(0, 0, 100, 75)));
		sf::Vector2f viewScale = sf::Vector2f(window.getView().getSize().x / window.getSize().x, window.getView().getSize().y / window.getSize().y);
		sf::Vector2f mousePos;
		sf::Event event;

		// window launch
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
						oneD.pause(!oneD.isPaused());
						break;
					case sf::Keyboard::B: // set forwards
						oneD.pause(false);
						break;
					case sf::Keyboard::V: // next (if paused)
						if (oneD.isPaused())
							oneD.run();
						break;
					case sf::Keyboard::X: // restart
						oneD.restart();
					}
					break;
				case sf::Event::MouseButtonPressed:
					mousePos = sf::Vector2f(static_cast<float>(sf::Mouse::getPosition(window).x)*viewScale.x,
						static_cast<float>(sf::Mouse::getPosition(window).y)*viewScale.y);
					//printf("Mouse raw pos: (%f:%f)\n", static_cast<float>(sf::Mouse::getPosition(window).x), static_cast<float>(sf::Mouse::getPosition(window).y));
					//printf("Mouse pos: (%f:%f)\n", mousePos.x, mousePos.y);
					// manual entry
					/*if (GOL.paused() && GOL.contains(mousePos))
						GOL.setCell(static_cast<unsigned int>(mousePos.x*GOL.getScale().x - GOL.getSpriteOffset().x),
							static_cast<unsigned int>(mousePos.y*GOL.getScale().y - GOL.getSpriteOffset().y), SFGOL::OPPOSITE);
							*/
							//pause
					if (butSpr.getButton(2).getGlobalBounds().contains(mousePos))
					{
						printf("pause\n");
						oneD.pause(!oneD.isPaused());
					}
					//forward
					if (butSpr.getButton(3).getGlobalBounds().contains(mousePos))
					{
						printf("forward\n");
						if (oneD.isPaused())
							oneD.run();
					}
					//play
					if (butSpr.getButton(4).getGlobalBounds().contains(mousePos))
					{
						printf("play\n");
						oneD.pause(false);
					}
					break;
				case sf::Event::Closed:
					window.close();
					break;
				} // end event switch
			} // end event handling

			if (!oneD.isPaused())
			{
				oneD.run();
				oneD.visualize();
			}

			window.clear(sf::Color(0, 0, 128, 255));
			window.draw(oneD);
			for (auto i = 0; i < 5; ++i)
				window.draw(butSpr.getButton(i));
			window.display();
			printf("FPS: %f\n", fps());

		} // window closes
	} // one-dimensional stuff
	else if (choice == 4) // two-dimensional GOL
	{
		// window setup
		sf::ContextSettings settings;
		settings.antialiasingLevel = 0;
		sf::RenderWindow window(sf::VideoMode(600, 600), "Game of Life", sf::Style::Default, settings);
		sf::Vector2f viewScale = sf::Vector2f(window.getView().getSize().x / window.getSize().x, window.getView().getSize().y / window.getSize().y);
		sf::Vector2f mousePos;
		sf::Event event;
		twoD.setPosition(50, 50);
		twoD.setScale(twoDScale, twoDScale);

		// window launch
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
						twoD.pause();
						printf(twoD.is_paused() ? "paused\n" : "unpaused\n");
						break;
					case sf::Keyboard::R:
						twoD.fillRandomly();
						break;
					case sf::Keyboard::C:
						twoD.clear();
						break;
					}
					break;
				case sf::Event::MouseButtonPressed: // TODO: flip a bool, move cell entry lower
					if (twoD.is_paused())
					{
						mousePos = sf::Vector2f(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
						printf("mousePos:%f, %f\n", mousePos.x, mousePos.y);
						auto bounds = twoD.getLocalBounds();
						if (sf::FloatRect(bounds.left*twoDScale, bounds.top*twoDScale, bounds.width*twoDScale, bounds.height*twoDScale).contains(mousePos - twoD.getPosition()))
							twoD.flipCell((mousePos.x - twoD.getPosition().x) / twoDScale, (mousePos.y - twoD.getPosition().y) / twoDScale);
					}
					break;
				case sf::Event::Closed:
					window.close();
					break;
				} // end event switch
			} // end event handling

			if (!twoD.is_paused())
			{
				twoD.run();
			}

			window.clear(sf::Color(0, 0, 128, 255));
			window.draw(twoD);
			window.display();
			//printf("FPS: %f\n", fps(60));

		} // window closes
	} // two-dimensional GOL
	else if (choice == 5)
	{
		// window setup
		sf::ContextSettings settings;
		settings.antialiasingLevel = 0;
		sf::RenderWindow window(sf::VideoMode(800, 600), "Zarodkowanie", sf::Style::Default, settings);
		sf::Vector2f viewScale = sf::Vector2f(window.getView().getSize().x / window.getSize().x, window.getView().getSize().y / window.getSize().y);
		sf::Vector2f mousePos;
		sf::Event event;
		ca.setPosition(50, 50);
		ca.setScale(5, 5);

		// button setup
		std::vector<sf::Texture> buttonTex(5);
		std::vector<sf::Sprite> buttonSpr(5);
		for (auto i = 0; i < 5; ++i)
		{
			sf::IntRect area(100 * i, 0, 100, 100);
			buttonTex.at(i).loadFromFile("..\\Advanced_C++\\resources\\graphics\\buttons.bmp", area);
			buttonSpr.at(i).setTexture(buttonTex.at(i), true);
			buttonSpr.at(i).setPosition(sf::Vector2f(540 + 50 * i, 10));
			buttonSpr.at(i).setScale(.5, .5);
		}


		// window launch
		while (window.isOpen())
		{
			while (window.pollEvent(event))
			{
				switch (event.type)
				{
				case sf::Event::KeyPressed:
					switch (event.key.code)
					{
					case sf::Keyboard::Z:
						printf("clear\n");
						ca.clear();
						break;
					case sf::Keyboard::X:
						printf("size down\n");
						ca.resize(sf::Vector2u(ca.getSize().x - 1, ca.getSize().y - 1));
						break;			
					case sf::Keyboard::Space: // pause/resume
						// fall-through
					case sf::Keyboard::C:
						ca.pause();
						printf(ca.is_paused() ? "paused\n" : "unpaused\n");
						break;
					case sf::Keyboard::V:
						printf("size up\n");
						ca.resize(sf::Vector2u(ca.getSize().x + 1, ca.getSize().y + 1));
						break;
					case sf::Keyboard::B:
						printf("seed\n");
						ca.seed();
						break;
					case sf::Keyboard::R:
						printf("seed\n");
						ca.seed();
						break;
					}
					break;
				case sf::Event::MouseButtonPressed:
					if (event.mouseButton.button == sf::Mouse::Left) // sprite-buttons
					{
						mousePos = sf::Vector2f(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
						//Seed
						if (buttonSpr.at(4).getGlobalBounds().contains(mousePos))
						{
							printf("seed\n");
							ca.seed();
						}
						//Change Size Up
						if (buttonSpr.at(3).getGlobalBounds().contains(mousePos))
						{
							printf("size up\n");
							ca.resize(sf::Vector2u(ca.getSize().x + 1, ca.getSize().y + 1));
						}
						//Change Size Down
						if (buttonSpr.at(1).getGlobalBounds().contains(mousePos))
						{
							printf("size down\n");
							ca.resize(sf::Vector2u(ca.getSize().x - 1, ca.getSize().y - 1));
						}
						//Reset
						if (buttonSpr.at(0).getGlobalBounds().contains(mousePos))
						{
							printf("clear\n");
							ca.clear();
						}
						//Pause/Unpause
						if (buttonSpr.at(2).getGlobalBounds().contains(mousePos))
						{
							ca.pause();
							printf(ca.is_paused() ? "paused\n" : "unpaused\n");
						}
					}
					break;
				case sf::Event::Closed:
					window.close();
					break;
				} // end event switch
			} // end event handling

			if (!ca.is_paused())
			{
				ca.run();
			}

			window.clear(sf::Color(0, 0, 128, 255));
			auto tex = ca.getTexture();
			//ca.update(tex);
			window.draw(ca);
			for (auto a : buttonSpr)
				window.draw(a);
			window.display();
			//printf("FPS: %f\n", fps(60));

		} // window closes
	} // two-dimensional GOL

	return 0;
}