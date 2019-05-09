#ifndef BOOLEANCELL_H
#define BOOLEANCELL_H

#include <SFML/Graphics.hpp>

struct BooleanCell {
	void flip() { status = !status; }

	unsigned int neighbours;
	bool status;
};

#endif