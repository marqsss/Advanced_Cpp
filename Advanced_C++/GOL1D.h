#ifndef GOL1D_H
#define GOL1D_H

#include <vector>
#include <sfml/Graphics.hpp>

class GOL1D: public sf::Drawable
{
public:
	GOL1D();
	GOL1D(unsigned int, unsigned int, unsigned int);
	void create(unsigned int, unsigned int, unsigned int);
	void run();
	void visualize();
	void setCell(unsigned int);
	void pause(bool);
	bool isPaused();
	void restart();
	void setScale(float x, float y) { visualization.setScale(x, y); }

	unsigned int getWidth() { return width; }

private:
	bool satisfies(unsigned int);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	class Cell1D
	{
	public:
		bool status;
		unsigned int neighbours;
	};
	std::vector<Cell1D> cellVec;
	unsigned int width;
	unsigned int iterations;
	unsigned int rule;
	unsigned int iterCounter;
	bool paused;
	sf::Texture tex;
	sf::Sprite visualization;
	sf::Uint8* dead_cell;
	sf::Uint8* live_cell;
};

#endif