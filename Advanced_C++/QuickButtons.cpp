#include "QuickButtons.h"

bool QuickButtons::newButton(std::filesystem::path p, sf::IntRect texRect, sf::Vector2f pos)
{
	textures.emplace_back();
	if (!textures.back().loadFromFile(p.string(), texRect))
	{
		textures.pop_back();
		return false;
	}
	else
	{
		buttons.emplace_back();
		buttons.back().setTexture(textures.back(), true);
		buttons.back().setPosition(pos);
	}
	return true;
}

void QuickButtons::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	for (auto b : buttons)
		target.draw(b, states);
}