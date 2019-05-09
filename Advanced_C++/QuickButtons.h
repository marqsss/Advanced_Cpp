#ifndef QUICKBUTTONS_H
#define QUICKBUTTONS_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <filesystem>

class QuickButtons
{
public:
	bool newButton(std::filesystem::path, sf::IntRect, sf::Vector2f);
	auto& getButton(unsigned int i) { return buttons.at(i); }
private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	std::vector<sf::Texture> textures;
	std::vector<sf::Sprite> buttons;
};

#endif