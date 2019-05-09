#ifndef SIMPLEFPS_H
#define SIMPLEFPS_H

#include <SFML/System/Clock.hpp>

class SimpleFPS {
public:
	SimpleFPS() {}

	// call at each frame; optional argument functions as fps limit
	double operator()(float fps = 0) {
		if (fps)
			sf::sleep(sf::seconds(1) / fps - clock.getElapsedTime());
			return (sf::seconds(1) / clock.restart());
	}

private:
	sf::Clock clock;
};

#endif