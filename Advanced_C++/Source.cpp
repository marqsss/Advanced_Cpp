#include <thread> // sleep
#include <chrono> // sleep
#include <fstream>
#include <iostream>
#include <filesystem>
#include <string>

#include <sfml/window.hpp>
#include <SFML/Graphics.hpp>
// Advanced C++
#include "GameOfLife.h"
#include "SFGOL.h"
#include "SFGOLConcurrent.h"
#include "RAMTest.h"
// Modelowanie Wieloskalowe
#include "GOL1D.h"
#include "GOL2D.h"
#include "CellularAutomaton.h"
#include "CARecrystallizer.h"
// Utility
#include "SimpleFPS.h"
#include "QuickButtons.h"

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
			sf::RenderWindow window(sf::VideoMode(900, 600), "Game of Life", sf::Style::Default, settings);
			//window.setView(sf::View(sf::FloatRect(0, 0, 100, 75)));
			sf::Vector2f viewScale = sf::Vector2f(window.getView().getSize().x / window.getSize().x, window.getView().getSize().y / window.getSize().y);
			sf::Vector2f mousePos;
			sf::Event event;

			// menu background
			sf::RectangleShape menuBackground(sf::Vector2f(300, 600));
			menuBackground.setFillColor(sf::Color(195, 195, 195, 255));
			menuBackground.setPosition(601, 0);
			menuBackground.setOutlineThickness(1);
			menuBackground.setOutlineColor(sf::Color::Black);

			// button setup
			unsigned int n_button = 15;
			sf::Texture buttonTex;
			buttonTex.loadFromFile("..\\Advanced_C++\\resources\\graphics\\buttons.bmp");
			std::vector<sf::Sprite> buttonSpr(n_button);
			std::vector<sf::Sprite> buttonNH(12);
			// rev - back - play/pause - next - ffw
			for (auto i = 0; i < 5; ++i)
			{
				sf::IntRect area(100 * i, 0, 100, 100);
				buttonSpr.at(i).setTexture(buttonTex, true);
				buttonSpr.at(i).setTextureRect(area);
				buttonSpr.at(i).setPosition(sf::Vector2f(625 + 50 * i, 10));
				buttonSpr.at(i).setScale(.5, .5);
			}
			// text field background 1
			buttonSpr.at(5).setTexture(buttonTex, true);
			buttonSpr.at(5).setTextureRect(sf::IntRect(0, 150, 200, 50));
			buttonSpr.at(5).setPosition(sf::Vector2f(650, 110));
			buttonSpr.at(5).setScale(.5, .5);
			// text field background 2
			buttonSpr.at(6).setTexture(buttonTex, true);
			buttonSpr.at(6).setTextureRect(sf::IntRect(0, 150, 200, 50));
			buttonSpr.at(6).setPosition(sf::Vector2f(770, 110));
			buttonSpr.at(6).setScale(.5, .5);
			// RESIZE
			buttonSpr.at(7).setTexture(buttonTex, true);
			buttonSpr.at(7).setTextureRect(sf::IntRect(0, 100, 200, 50));
			buttonSpr.at(7).setPosition(sf::Vector2f(650, 145));
			// patern loading
			for (auto i = 8; i < 10; ++i)
			{
				buttonSpr.at(i).setTexture(buttonTex);
				buttonSpr.at(i).setTextureRect(sf::IntRect(300 + 50 * (i % 2), 150, 50, 50));
				buttonSpr.at(i).setPosition(700 + 50 * (i - 8), 275);
			}
			// STRATEGY
			int strategy = 0;
			buttonSpr.at(10).setTexture(buttonTex, true);
			buttonSpr.at(10).setTextureRect(sf::IntRect(200, 100, 200, 50));
			buttonSpr.at(10).setPosition(sf::Vector2f(650, 460));
			// text field background 5
			int fpsLimit = 60;
			buttonSpr.at(11).setTexture(buttonTex, true);
			buttonSpr.at(11).setTextureRect(sf::IntRect(0, 150, 200, 50));
			buttonSpr.at(11).setPosition(sf::Vector2f(700, 345));
			buttonSpr.at(11).setScale(.5, .5);
			// text field background 3
			buttonSpr.at(12).setTexture(buttonTex, true);
			buttonSpr.at(12).setTextureRect(sf::IntRect(0, 150, 200, 50));
			buttonSpr.at(12).setPosition(sf::Vector2f(650, 240));
			buttonSpr.at(12).setScale(.5, .5);
			// text field background 4
			buttonSpr.at(13).setTexture(buttonTex, true);
			buttonSpr.at(13).setTextureRect(sf::IntRect(0, 150, 200, 50));
			buttonSpr.at(13).setPosition(sf::Vector2f(770, 240));
			buttonSpr.at(13).setScale(.5, .5);
			// DESCRIBE
			buttonSpr.at(14).setTexture(buttonTex, true);
			buttonSpr.at(14).setTextureRect(sf::IntRect(0, 200, 200, 50));
			buttonSpr.at(14).setPosition(sf::Vector2f(650, 410));
			for (auto& b : buttonSpr)
				b.setColor(sf::Color(195, 195, 195, 255));

			// text field setup
			sf::Font arial;
			arial.loadFromFile("../Advanced_C++/resources/fonts/arial.ttf");
			unsigned int text_no = 7;
			std::vector<bool>text_active(text_no);
			std::vector<sf::String> text(text_no);
			std::vector<sf::Text> text_field(text_no);
			for (auto i = 0; i < text_field.size(); ++i)
			{
				text_field.at(i).setString(text.at(i));
				text_field.at(i).setFont(arial);
				text_field.at(i).setCharacterSize(14);
				text_field.at(i).setFillColor(sf::Color::Black);
			}
			text_field.at(0).setPosition(655, 115);
			text_field.at(1).setPosition(775, 115);
			text_field.at(2).setPosition(655, 245);
			text_field.at(3).setPosition(775, 245);
			text_field.at(4).setOrigin(sf::Vector2f(text_field.at(4).getLocalBounds().width / 2, 0));
			text_field.at(4).setPosition(755, 515);
			text_field.at(5).setPosition(5, 580);
			text_field.at(6).setPosition(705, 350);
			auto updateText = [](sf::String &s, sf::Text & t) {
				t.setString(s);
				t.setOrigin(sf::Vector2f(t.getLocalBounds().width / 2, 0));
			};
			sf::Text size_tooltip("W:                           H:", arial, 14);
			size_tooltip.setFillColor(sf::Color::Black);
			size_tooltip.setPosition(630, 115);
			sf::Text size_tooltip2("X:                           Y:", arial, 14);
			size_tooltip2.setFillColor(sf::Color::Black);
			size_tooltip2.setPosition(635, 245);
			text.at(0) = std::to_string(GOL.getSize().x);
			text.at(1) = std::to_string(GOL.getSize().y);
			text.at(2) = "10";
			text.at(3) = "0";
			text.at(6) = std::to_string(fpsLimit);
			for (auto i = 0; i < text.size(); ++i)
				text_field.at(i).setString(text.at(i));
			updateText(text.at(4) = "sequential", text_field.at(4));

			while (window.isOpen())
			{
				while (window.pollEvent(event))
				{
					switch (event.type)
					{
					case sf::Event::KeyPressed:
						switch (event.key.code)
						{
						case sf::Keyboard::Backspace:
							for (auto i = 0; i < text_field.size(); ++i)
								if (text_active.at(i) && text.at(i).getSize())
								{
									text.at(i).erase(text.at(i).getSize() - 1, 1);
									text_field.at(i).setString(text.at(i));
								}
							break;
						case sf::Keyboard::Enter:
							for (auto i = 0; i < text_active.size(); ++i)
								text_active.at(i) = false;
							GOL.resize(sf::Vector2u(
								text.at(0).isEmpty() ? GOL.getSize().x : std::stoul(text.at(0).toAnsiString()),
								text.at(1).isEmpty() ? GOL.getSize().y : std::stoul(text.at(1).toAnsiString())));
							fpsLimit = text.at(6).isEmpty() ? 60 : std::stoul(text.at(6).toAnsiString());
							break;
						}
						break;
					case sf::Event::MouseButtonPressed:
						//deactivate text fields
						for (auto i = 0; i < text_active.size(); ++i)
							text_active.at(i) = false;

						mousePos = sf::Vector2f(static_cast<float>(sf::Mouse::getPosition(window).x)*viewScale.x,
							static_cast<float>(sf::Mouse::getPosition(window).y)*viewScale.y);
						//printf("Mouse raw pos: (%f:%f)\n", static_cast<float>(sf::Mouse::getPosition(window).x), static_cast<float>(sf::Mouse::getPosition(window).y));
						//printf("Mouse pos: (%f:%f)\n", mousePos.x, mousePos.y);
						// manual entry
						if (GOL.paused() && GOL.contains(mousePos))
							GOL.setCell(static_cast<unsigned int>(mousePos.x*GOL.getScale().x - GOL.getSpriteOffset().x),
								static_cast<unsigned int>(mousePos.y*GOL.getScale().y - GOL.getSpriteOffset().y), SFGOL::OPPOSITE);
						//playback
						if (buttonSpr.at(0).getGlobalBounds().contains(mousePos))
						{
							printf("playback\n");
							if (GOL.paused())
							{
								GOL.forwards(false);
								//GOL.describe();
							}
						}
						//back
						if (buttonSpr.at(1).getGlobalBounds().contains(mousePos))
						{
							printf("back\n");
							if (GOL.paused())
							{
								GOL.unrun();
								//GOL.describe();
								//GOL.print();
							}
						}
						//pause
						if (buttonSpr.at(2).getGlobalBounds().contains(mousePos))
						{
							printf("pause\n");
							GOL.pause();
							//GOL.describe();
						}
						//forward
						if (buttonSpr.at(3).getGlobalBounds().contains(mousePos))
						{
							printf("forward\n");
							if (GOL.paused())
							{
								GOL.run();
								//GOL.describe();
								//GOL.print();
							}
						}
						//play
						if (buttonSpr.at(4).getGlobalBounds().contains(mousePos))
						{
							printf("play\n");
							if (GOL.paused())
							{
								GOL.forwards();
								//GOL.describe();
							}
						}
						// TFB 1
						if (buttonSpr.at(5).getGlobalBounds().contains(mousePos))
						{
							text_active.at(0) = true;
							printf("text field 1 active\n");
						}
						// TFB 2
						if (buttonSpr.at(6).getGlobalBounds().contains(mousePos))
						{
							text_active.at(1) = true;
							printf("text field 2 active\n");
						}
						// RESIZE
						if (buttonSpr.at(7).getGlobalBounds().contains(mousePos))
						{
							GOL.resize(sf::Vector2u(
								text.at(0).isEmpty() ? GOL.getSize().x : std::stoul(text.at(0).toAnsiString()),
								text.at(1).isEmpty() ? GOL.getSize().y : std::stoul(text.at(1).toAnsiString())));
						}
						// pattern input 1
						if (buttonSpr.at(8).getGlobalBounds().contains(mousePos))
						{
							GOL.resize(sf::Vector2u(
								text.at(0).isEmpty() ? GOL.getSize().x : std::stoul(text.at(0).toAnsiString()),
								text.at(1).isEmpty() ? GOL.getSize().y : std::stoul(text.at(1).toAnsiString())));
							GOL.offsetFromRLE("../Advanced_C++/resources/GOL_files/Gosper_glider_gun.rle",
								text.at(2).isEmpty() ? 0 : std::stoul(text.at(2).toAnsiString()),
								text.at(3).isEmpty() ? 0 : std::stoul(text.at(3).toAnsiString()));
						}
						// pattern input 2
						if (buttonSpr.at(9).getGlobalBounds().contains(mousePos))
						{
							GOL.resize(sf::Vector2u(
								text.at(0).isEmpty() ? GOL.getSize().x : std::stoul(text.at(0).toAnsiString()),
								text.at(1).isEmpty() ? GOL.getSize().y : std::stoul(text.at(1).toAnsiString())));
							GOL.offsetFromRLE("../Advanced_C++/resources/GOL_files/Queen_Bee_Shuttle.rle",
								text.at(2).isEmpty() ? 0 : std::stoul(text.at(2).toAnsiString()),
								text.at(3).isEmpty() ? 0 : std::stoul(text.at(3).toAnsiString()));
						}
						// STRATEGY
						if (buttonSpr.at(10).getGlobalBounds().contains(mousePos))
						{
							strategy = (strategy + 1) % 3;
							switch (strategy)
							{
							case 0:
								updateText(text.at(4) = "sequential", text_field.at(4));
								break;
							case 1:
								updateText(text.at(4) = "thread parallel", text_field.at(4));
								break;
							case 2:
								updateText(text.at(4) = "for_each parallel", text_field.at(4));
								break;
							}
						}
						// fps limit
						if (buttonSpr.at(11).getGlobalBounds().contains(mousePos))
						{
							text_active.at(6) = true;
							printf("text field 5 active\n");
						}
						// TFB 3
						if (buttonSpr.at(12).getGlobalBounds().contains(mousePos))
						{
							text_active.at(2) = true;
							printf("text field 3 active\n");
						}
						// TFB 4
						if (buttonSpr.at(13).getGlobalBounds().contains(mousePos))
						{
							text_active.at(3) = true;
							printf("text field 4 active\n");
						}
						// DESCRIBE
						if (buttonSpr.at(14).getGlobalBounds().contains(mousePos))
						{
							GOL.describe();
							printf("Max safe RAM: %u\n", RAMTest::getSafeRAM());
							printf("Histories: %f\n", GOL.getMaxHistories(RAMTest::getSafeRAM(), true));
							printf("Max histories: %f\n", static_cast<long double>(RAMTest::getSafeRAM()) / GOL.getMaxHistories(RAMTest::getSafeRAM(), true));
						}
						// regardless of where the click was
						for (auto& b : buttonSpr)
						{ // button color changes
							if (b.getGlobalBounds().contains(mousePos))
							{
								if (b.getColor() != sf::Color::White)
									b.setColor(sf::Color::White);
							}
							else if (b.getColor() == sf::Color::White)
								b.setColor(sf::Color(195, 195, 195, 255));
						}
						break;
					case sf::Event::MouseButtonReleased:
						// dimming buttons after click
						for (auto i = 0; i < n_button; ++i)
							if (i != 5 && i != 6 && i != 12 && i != 13) // exclude text_fields
								buttonSpr.at(i).setColor(sf::Color(195, 195, 195, 255));
						break;
					case sf::Event::TextEntered:
						if (event.text.unicode > 0x1f && event.text.unicode < 0x3a) // numbers only
						{
							for (auto i = 0; i < text_field.size(); ++i)
								if (text_active.at(i))
								{
									text.at(i) += event.text.unicode;
									if (i != 4)
										text_field.at(i).setString(text.at(i));
									else
										updateText(text.at(i), text_field.at(i));
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
					if (GOL.direction())
					{
						switch (strategy)
						{
						case 0:
							GOL.seq_run();
							break;
						case 1:
							GOL.run();
							break;
						case 2:
							GOL.alt_run();
							break;
						}
					}
					else
						GOL.unrun();
				}

				text.at(5) = std::to_string(fps(fpsLimit));
				text_field.at(5).setString(text.at(5));

				window.clear(sf::Color(195, 195, 195, 255));
				window.draw(GOL);
				window.draw(menuBackground);
				for (auto& a : buttonSpr)
					window.draw(a);
				for (auto& t : text_field)
					window.draw(t);
				window.draw(size_tooltip);
				window.draw(size_tooltip2);
				window.display();

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
		sf::RenderWindow window(sf::VideoMode(900, 600), "Zarodkowanie", sf::Style::Default, settings);
		sf::Vector2f viewScale = sf::Vector2f(window.getView().getSize().x / window.getSize().x, window.getView().getSize().y / window.getSize().y);
		sf::Vector2f mousePos;
		sf::Event event;

		// CA setup
		CARecrystallizer ca;
		ca.setA(86710969050178.5);
		ca.setB(9.41268203527779);
		ca.setTemperature(900);
		ca.setTotalTime(0.2);
		ca.setCritical(4215840142323.42);
		ca.resize(sf::Vector2u(60, 60));
		ca.setPosition(0, 0);
		ca.setScale(5, 5);
		ca.setBC(true);


		// menu background
		sf::RectangleShape menuBackground(sf::Vector2f(300, 600));
		menuBackground.setFillColor(sf::Color(195, 195, 195, 255));
		menuBackground.setPosition(601, 0);
		menuBackground.setOutlineThickness(1);
		menuBackground.setOutlineColor(sf::Color::Black);

		// button setup
		unsigned int n_button = 15;
		sf::Texture buttonTex;
		buttonTex.loadFromFile("..\\Advanced_C++\\resources\\graphics\\buttons.bmp");
		std::vector<sf::Sprite> buttonSpr(n_button);
		std::vector<sf::Sprite> buttonNH(12);
		for (auto i = 0; i < 5; ++i)
		{
			sf::IntRect area(100 * i, 0, 100, 100);
			buttonSpr.at(i).setTexture(buttonTex, true);
			buttonSpr.at(i).setTextureRect(area);
			buttonSpr.at(i).setPosition(sf::Vector2f(640 + 50 * i, 10));
			buttonSpr.at(i).setScale(.5, .5);
		}
		buttonSpr.at(5).setTexture(buttonTex, true);
		buttonSpr.at(5).setTextureRect(sf::IntRect(0, 150, 200, 50));
		buttonSpr.at(5).setPosition(sf::Vector2f(660, 110));
		buttonSpr.at(5).setScale(.5, .5);
		buttonSpr.at(6).setTexture(buttonTex, true);
		buttonSpr.at(6).setTextureRect(sf::IntRect(0, 150, 200, 50));
		buttonSpr.at(6).setPosition(sf::Vector2f(780, 110));
		buttonSpr.at(6).setScale(.5, .5);
		buttonSpr.at(7).setTexture(buttonTex, true);
		buttonSpr.at(7).setTextureRect(sf::IntRect(0, 100, 200, 50));
		buttonSpr.at(7).setPosition(sf::Vector2f(670, 145));
		for (auto i = 8; i < 12; ++i)
		{
			buttonSpr.at(i).setTexture(buttonTex);
			buttonSpr.at(i).setTextureRect(sf::IntRect(400 + 50 * (i % 2), 100 + 50 * ((i - 8) / 2), 50, 50));
			buttonSpr.at(i).setPosition(685 + 50 * (i - 8), 225);
		}buttonSpr.at(12).setTexture(buttonTex, true);
		buttonSpr.at(12).setTextureRect(sf::IntRect(0, 150, 200, 50));
		buttonSpr.at(12).setPosition(sf::Vector2f(660, 510));
		buttonSpr.at(12).setScale(.5, .5);
		buttonSpr.at(13).setTexture(buttonTex, true);
		buttonSpr.at(13).setTextureRect(sf::IntRect(0, 150, 200, 50));
		buttonSpr.at(13).setPosition(sf::Vector2f(780, 510));
		buttonSpr.at(13).setScale(.5, .5);
		buttonSpr.at(14).setTexture(buttonTex, true);
		buttonSpr.at(14).setTextureRect(sf::IntRect(250, 150, 50, 50));
		buttonSpr.at(14).setPosition(sf::Vector2f(630, 360));
		for (auto& b : buttonSpr)
			b.setColor(sf::Color(195, 195, 195, 255));
		for (auto i = 0; i < 12; ++i)
		{
			buttonNH.at(i).setTexture(buttonTex);
			buttonNH.at(i).setTextureRect(sf::IntRect(200 + 50 * (i % 6), 200 + 50 * (i / 6), 50, 50));
			buttonNH.at(i).setPosition(685 + 50 * (i % 4), 295 + 50 * (i / 4));
		}
		for (auto& b : buttonNH)
			b.setColor(sf::Color(195, 195, 195, 255));
		buttonNH.at(0).setColor(sf::Color::White);

		// text field setup
		sf::Font arial;
		arial.loadFromFile("../Advanced_C++/resources/fonts/arial.ttf");
		unsigned int text_no = 4;
		std::vector<bool>text_active(text_no);
		std::vector<sf::String> text(text_no);
		std::vector<sf::Text> text_field(text_no);
		for (auto i = 0; i < text_field.size(); ++i)
		{
			text_field.at(i).setString(text.at(i));
			text_field.at(i).setFont(arial);
			text_field.at(i).setCharacterSize(14);
			text_field.at(i).setFillColor(sf::Color::Black);
		}
		text_field.at(0).setPosition(665, 115);
		text_field.at(1).setPosition(785, 115);
		text_field.at(2).setPosition(665, 515);
		text_field.at(3).setPosition(785, 515);
		sf::Text size_tooltip("W:                           H:", arial, 14);
		size_tooltip.setFillColor(sf::Color::Black);
		size_tooltip.setPosition(640, 115);
		text.at(0) = std::to_string(ca.getSize().x);
		text.at(1) = std::to_string(ca.getSize().y);
		text.at(2) = "10";
		text.at(3) = "0";
		for (auto i = 0; i < text.size(); ++i)
			text_field.at(i).setString(text.at(i));
		// text field: sprite {texture, text; string; active; backspace; write; isempty; isactive; getstring; makeactive(v<>)}

		int drawing_mode = 1;

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
					case sf::Keyboard::Backspace:
						for (auto i = 0; i < text_field.size(); ++i)
							if (text_active.at(i) && text.at(i).getSize())
							{
								text.at(i).erase(text.at(i).getSize() - 1, 1);
								text_field.at(i).setString(text.at(i));
							}
						break;
					case sf::Keyboard::Enter:
						for (auto i = 0; i < text_active.size(); ++i)
							text_active.at(i) = false;
						ca.resize(sf::Vector2u(
							text.at(0).isEmpty() ? ca.getLocalBounds().width : std::stoul(text.at(0).toAnsiString()),
							text.at(1).isEmpty() ? ca.getLocalBounds().height : std::stoul(text.at(1).toAnsiString())));
						break;
					}
				case sf::Event::MouseButtonPressed:
					if (event.mouseButton.button == sf::Mouse::Left) // sprite-buttons
					{
						//deactivate text fields
						for (auto i = 0; i < text_active.size(); ++i)
							text_active.at(i) = false;

						mousePos = sf::Vector2f(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
						if (buttonSpr.at(4).getGlobalBounds().contains(mousePos))
						{//Seed
							printf("seed\n");
							ca.seedRandom();
						}
						else if (buttonSpr.at(3).getGlobalBounds().contains(mousePos))
						{//Step
							printf("step\n");
							ca.run();
						}
						else if (buttonSpr.at(1).getGlobalBounds().contains(mousePos))
						{//Disabled
							printf("swap visual\n");
							ca.drawing_mode(drawing_mode);
							drawing_mode = (drawing_mode + 1) % 3;
						}
						else if (buttonSpr.at(0).getGlobalBounds().contains(mousePos))
						{//Reset
							printf("clear\n");
							ca.clear();
						}
						else if (buttonSpr.at(2).getGlobalBounds().contains(mousePos))
						{//Pause/Unpause
							ca.pause();
							printf(ca.is_paused() ? "paused\n" : "unpaused\n");
						}
						else if (buttonSpr.at(5).getGlobalBounds().contains(mousePos))
						{//Text field 1
							text_active.at(0) = true;
							printf("text field 1 active\n");
						}
						else if (buttonSpr.at(6).getGlobalBounds().contains(mousePos))
						{//Text field 2
							text_active.at(1) = true;
							printf("text field 2 active\n");
						}
						else if (buttonSpr.at(12).getGlobalBounds().contains(mousePos))
						{//Text field 3
							text_active.at(2) = true;
							printf("text field 3 active\n");
						}
						else if (buttonSpr.at(13).getGlobalBounds().contains(mousePos))
						{//Text field 4
							text_active.at(3) = true;
							printf("text field 4 active\n");
						}
						else if (buttonSpr.at(7).getGlobalBounds().contains(mousePos))
						{// RESIZE button
							ca.resize(sf::Vector2u(
								text.at(0).isEmpty() ? ca.getLocalBounds().width : std::stoul(text.at(0).toAnsiString()),
								text.at(1).isEmpty() ? ca.getLocalBounds().height : std::stoul(text.at(1).toAnsiString())));
						}
						else if (buttonSpr.at(8).getGlobalBounds().contains(mousePos))
						{// Seed uniform
							printf("Seeding uniform\n");
							ca.seedUniform(text.at(2).isEmpty() ? 10 : std::stoul(text.at(2).toAnsiString()));
						}
						else if (buttonSpr.at(9).getGlobalBounds().contains(mousePos))
						{// Seed random
							printf("Seeding random\n");
							ca.seedRandom(text.at(2).isEmpty() ? 10 : std::stoul(text.at(2).toAnsiString()));
						}
						else if (buttonSpr.at(10).getGlobalBounds().contains(mousePos))
						{// Seed radius
							printf("Seeding with radius\n");
							ca.seedWithRadius(text.at(2).isEmpty() ? 10 : std::stoul(text.at(2).toAnsiString()), text.at(3).isEmpty() ? 0 : std::stoul(text.at(3).toAnsiString()));
						}
						else if (buttonSpr.at(11).getGlobalBounds().contains(mousePos))
						{// Seed by hand
							printf("Seeding by hand (not implemented)\n");
						}
						else if (buttonSpr.at(14).getGlobalBounds().contains(mousePos))
						{// Recrystallize
							printf("Recrystallizing\n");
							ca.recrystallize(80, true, "../Advanced_C++/resources/recrystallization.txt");
						}

						// regardless of where the click was
						for (auto& b : buttonSpr)
						{ // button color changes
							if (b.getGlobalBounds().contains(mousePos))
							{
								if (b.getColor() != sf::Color::White)
									b.setColor(sf::Color::White);
							}
							else if (b.getColor() == sf::Color::White)
								b.setColor(sf::Color(195, 195, 195, 255));
						}
						// check neighbourhood change
						for (auto i = 0; i < buttonNH.size(); ++i)
						{
							if (buttonNH.at(i).getGlobalBounds().contains(mousePos))
								ca.setNeighbourhood(CellularAutomaton::Neighbourhood(i));
						}
						for (auto i = 0; i < buttonNH.size(); ++i)
						{
							if (ca.getNeighbourhood() == i)
								buttonNH.at(i).setColor(sf::Color::White);
							else
								buttonNH.at(i).setColor(sf::Color(195, 195, 195, 255));
						}
					} // left-click
					break;
				case sf::Event::MouseButtonReleased:
					// dimming buttons after click
					for (auto i = 0; i < n_button; ++i)
						if (i != 5 && i != 6 && i != 12 && i != 13) // exclude text_fields
							buttonSpr.at(i).setColor(sf::Color(195, 195, 195, 255));
					break;
				case sf::Event::TextEntered:
					if (event.text.unicode > 0x1f && event.text.unicode < 0x3a) // numbers only
					{
						for (auto i = 0; i < text_field.size(); ++i)
							if (text_active.at(i))
							{
								text.at(i) += event.text.unicode;
								text_field.at(i).setString(text.at(i));
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

			window.clear(sf::Color(195, 195, 195, 255));
			window.draw(ca);
			window.draw(menuBackground);
			for (auto& a : buttonSpr)
				window.draw(a);
			for (auto& a : buttonNH)
				window.draw(a);
			for (auto& t : text_field)
				window.draw(t);
			window.draw(size_tooltip);
			window.display();
			//printf("FPS: %f\n", fps(60));

		} // window closes
	} // two-dimensional GOL

	return 0;
}