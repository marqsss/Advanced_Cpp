#include "GOL1D.h"

GOL1D::GOL1D()
{}
GOL1D::GOL1D(unsigned int w, unsigned int i, unsigned int r) : width(w), iterations(i), rule(r)
{
	dead_cell = new sf::Uint8[4];
	for (int i = 0; i < 3; i++)
		dead_cell[i] = 0;
	dead_cell[3] = 255;
	live_cell = new sf::Uint8[4];
	for (int i = 0; i < 4; i++)
		live_cell[i] = 255;
	cellVec.resize(w);
	tex.create(w, i);
}

void GOL1D::create(unsigned int w, unsigned int i, unsigned int r)
{
	width = w;
	iterations = i;
	rule = r;
	dead_cell = new sf::Uint8[4];
	for (int i = 0; i < 3; i++)
		dead_cell[i] = 0;
	dead_cell[3] = 255;
	live_cell = new sf::Uint8[4];
	for (int i = 0; i < 4; i++)
		live_cell[i] = 255;
	cellVec.resize(w);
	tex.create(w, i);
}

void GOL1D::run()
{
	if (iterCounter < iterations)
	{
		for (auto i = 0; i < cellVec.size(); ++i)
		{
			cellVec.at(i).neighbours = cellVec.at((i - 1 + cellVec.size()) % cellVec.size()).status * 4 +
				cellVec.at(i).status * 2 + cellVec.at((i + 1) % cellVec.size()).status;
		}
		for (auto i = 0; i < cellVec.size(); ++i)
		{
			cellVec.at(i).status = satisfies(cellVec.at(i).neighbours);
			if (cellVec.at(i).status)
				tex.update(live_cell, 1, 1, i, iterCounter);
			else
				tex.update(dead_cell, 1, 1, i, iterCounter);
		}
		++iterCounter;
	}
}

void GOL1D::visualize()
{
	visualization.setTexture(tex, true);
}

void GOL1D::setCell(unsigned int pos)
{
	cellVec.at(pos).status = !cellVec.at(pos).status;
}

void GOL1D::pause(bool p)
{
	paused = p;
}

bool GOL1D::isPaused()
{
	return paused;
}

void GOL1D::restart()
{
	iterCounter = 0;
	tex.update(dead_cell,width, iterations, 0, 0);
	cellVec.clear();
	cellVec.resize(width);
	setCell(width / 2);
	visualize();
}

// PRIVATE

bool GOL1D::satisfies(unsigned int n)
{
	auto t_rule = rule;
	if ((t_rule >> n) % 2)
		return true;
	return false;
}

void GOL1D::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(visualization, states);
}