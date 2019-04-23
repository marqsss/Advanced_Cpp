#include "SFGOL.h"
#include <string>
#include <fstream>

SFGOL::SFGOL()
{
	dead_cell = new sf::Uint8[4];
	for (int i = 0; i < 3; i++)
		dead_cell[i] = 0;
	dead_cell[3] = 255;
	live_cell = new sf::Uint8[4];
	for (int i = 0; i < 4; i++)
		live_cell[i] = 255;
	is_paused = false;
	vertices.resize(4);
	vertices.setPrimitiveType(sf::Quads);
}

bool SFGOL::pause()
{
	is_paused = !is_paused;
	return is_paused;
}

bool SFGOL::paused()
{
	return is_paused;
}

void SFGOL::resizeMap(unsigned int w, unsigned int h)
{
	cellMap.resize(h);
	for (auto &i : cellMap)
		i.resize(w);
	texMap.create(h, w);
	for (unsigned int i = 0; i < h; ++i)
		for (unsigned int j = 0; j < w; ++j)
			texMap.update(dead_cell, i, j, 1, 1);

	vertices[0].position = static_cast<sf::Vector2f>(sf::Vector2u(texOffset.x, texOffset.y));
	vertices[1].position = static_cast<sf::Vector2f>(sf::Vector2u(texOffset.x + texMap.getSize().x, texOffset.y));
	vertices[2].position = static_cast<sf::Vector2f>(sf::Vector2u(texOffset.x + texMap.getSize().x, texOffset.y + texMap.getSize().y));
	vertices[3].position = static_cast<sf::Vector2f>(sf::Vector2u(texOffset.x, texOffset.y + texMap.getSize().y));
	vertices[0].texCoords = sf::Vector2f(0, 0);
	vertices[1].texCoords = static_cast<sf::Vector2f>(sf::Vector2u(texMap.getSize().x, 0));
	vertices[2].texCoords = static_cast<sf::Vector2f>(sf::Vector2u(texMap.getSize().x, texMap.getSize().y));
	vertices[3].texCoords = static_cast<sf::Vector2f>(sf::Vector2u(0, texMap.getSize().y));
}

void SFGOL::resizeMap(sf::Vector2u size)
{
	resizeMap(size.x, size.y);
}

void SFGOL::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(visualization, states);
}

bool SFGOL::update(unsigned int i, unsigned int j)
{
	bool changed = false;
	auto &cell = cellMap.at(i).at(j);
	if (cell.status)
	{
		bool newStatus = false;
		for (unsigned int i = 0; i < lifeMap.size(); i++)
			if (cell.neighbours == lifeMap.at(i))
				newStatus = true;
		if (!newStatus)
			changed = true;
		if (changed)
		{
			texMap.update(dead_cell, 1, 1, i, j);
			updates.at(iterCounter % 1000).emplace_back(sf::Vector2u(i, j));
			cell.status = newStatus;
		}
	}
	else
	{
		bool newStatus = false;
		for (unsigned int i = 0; i < reviveMap.size(); i++)
			if (cell.neighbours == reviveMap.at(i))
			{
				newStatus = true;
				changed = true;
			}
		if (changed)
		{
			texMap.update(live_cell, 1, 1, i, j);
			updates.at(iterCounter % 1000).emplace_back(sf::Vector2u(i, j));
			cell.status = newStatus;
		}
	}
	return changed;
}

void SFGOL::visualize()
{
	visualization.setTexture(texMap, true);
}

void SFGOL::unrun(unsigned int iterations, bool safetycheck)
{
	printf("unrun() - to be implemented\n");
}

bool SFGOL::offsetFromRLE(std::filesystem::path p, unsigned int offsetX, unsigned int offsetY)
{
	std::ifstream f(p.c_str());
	if (f.is_open())
	{
		std::string s;
		unsigned int x, y;
		while (!f.eof())
		{
			std::getline(f, s);
			if (s.at(0) == '#')
			{
				if (s.at(1) == 'N')
					name += s.substr(3, s.size());
				else if (s.at(1) == 'O')
					author += s.substr(3, s.size());
				else if (s.at(1) == 'C' || s.at(1) == 'c')
					description += s.substr(3, s.size()) += '\n';
				// else ignore - invalid input
			}
			else if (s.at(0) == 'x')
			{
				x = std::stoul(s.substr(s.find_first_of("0123456789"),
					s.find_first_of(',') - s.find_first_of("0123456789")));
				y = std::stoul(s.substr(s.find_first_of("0123456789", s.find_first_of(',')),
					s.find_first_of(',', s.find_first_of(',') + 1) -
					s.find_first_of("0123456789", s.find_first_of(','))));
				std::string rule1 = s.substr(s.find_last_of('=') + 2,
					s.find_last_of('/') - s.find_last_of('=') - 2);
				std::string rule2 = s.substr(s.find_last_of('/') + 1, s.size());
				if (rule1.at(0) == 'B')
					for (int arr = std::stoi(rule1.substr(1)); arr > 0; arr /= 10)
						reviveMap.insert(reviveMap.begin(), arr % 10);
				else if (rule1.at(0) == 'S')
					for (int arr = std::stoi(rule1.substr(1)); arr > 0; arr /= 10)
						lifeMap.insert(lifeMap.begin(), arr % 10);
				if (rule2.at(0) == 'B')
					for (int arr = std::stoi(rule2.substr(1)); arr > 0; arr /= 10)
						reviveMap.insert(reviveMap.begin(), arr % 10);
				else if (rule2.at(0) == 'S')
					for (int arr = std::stoi(rule2.substr(1)); arr > 0; arr /= 10)
						lifeMap.insert(lifeMap.begin(), arr % 10);
				// else ignore - invalid input
				// print 'commented' data
				printf("%s\n%s\n%s", name.c_str(), author.c_str(), description.c_str());
			}
			else
			{
				// ensure map size
				if ((width < x) || (height < y))
				{
					width = y;
					height = x;
					resizeMap(width, height);
				}
				// encode
				unsigned int offset = 0, aliveQueue = 0;
				size_t k = 0;
				for (unsigned int i = offsetY; i <= offsetY + y; i++)
					for (unsigned int j = offsetX; j <= offsetX + x; j++)
					{
						k = s.find_first_not_of("0123456789", offset);
						if (k == std::string::npos) // read next line of data
						{
							offset = 0;
							std::getline(f, s); // new line of data
							k = s.find_first_not_of("0123456789", offset);
						}
						if (s.at(k) == '$') // jump vertically
						{
							i += (k - offset) < 1 ? 0 : stoul(s.substr(offset, k - offset)) - 1;
							//j = cellMap.at(i).size();
							offset = k + 1;
							continue;
						}
						if (s.at(k) == 'b') // jump horizontally
						{
							j += (k - offset) < 1 ? 0 : stoul(s.substr(offset, k - offset)) - 1;
							offset = k + 1;
						}
						else if (s.at(k) == 'o') // revive some cells
						{
							aliveQueue = (k - offset) < 1 ? 1 : stoul(s.substr(offset, k - offset));
							offset = k + 1;
							for (; aliveQueue > 0; --aliveQueue)
							{
								printf("%d, %d\t", i, j); // debug
								cellMap.at(i).at(j).status = true;
								if (aliveQueue - 1)
									++j;
							}
						}
					}
			}
		}
	}
	else
		return false;
	f.close();
	return true;
}

bool SFGOL::initialize(sf::Vector2u size, std::filesystem::path p, sf::Vector2u offset)
{
	height = size.x;
	width = size.y;
	resizeMap(width, height);
	return offsetFromRLE(p, offset.x, offset.y);
}

sf::Texture& SFGOL::getTex()
{
	return texMap;
}