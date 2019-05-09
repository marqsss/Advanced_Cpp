#ifndef GOL2D_H
#define GOL2D_H

#include <vector>
#include <filesystem>
#include <sfml/Graphics.hpp>

class GOL2D: public sf::Sprite {
public:
	GOL2D() {}
	bool createFromFile(std::filesystem::path);
	enum class Neighborhood;

private:
	Neighborhood neighborhood;
	unsigned int lifeRule;
	unsigned int reviveRule;
	sf::Texture texMap;

public:
	enum class Neighborhood {
		VonNeumann = 0,
		Moore = 1,
		FiveLeft = 2,
		FiveRight = 3,
		FiveAlternating = 4,
		FiveRandom = 5,
		SixLeft = 6,
		SixRight = 7,
		SixAlternating = 8,
		SixRandom = 9
	};
};

#endif