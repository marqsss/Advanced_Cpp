#include <fstream>
#include "CARecrystallizer.h"

void CARecrystallizer::calculateToFile(std::string path)
{
	std::ofstream file(path, std::ios::trunc);
	if (file.is_open())
	{
		file << "# Initial data:\n";
		file << "# A =\n";
		file << A << "\n";
		file << "# B =\n";
		file << B << "\n";
		file << "# temperature =\n";
		file << temperature << "\n";
		file << "# total time =\n";
		file << total_time << "\n";

		auto i = 0;
		for (auto t = 0; t < total_time; t += total_time / 1000)
		{
			file << "\n# Iteration " << i++ << "\n";
			file << "# time =\n";
			file << t << "\n";
			file << "# ro =\n";
			file << getRo(t) << "\n";
			file << "# sigma =\n";
			file << getSigma(getRo(t)) << "\n";
		}
	}
	file.close();
}

void CARecrystallizer::calculateToCSV(std::string path)
{
	std::ofstream file(path, std::ios::trunc);
	if (file.is_open())
	{
		file << A << ", " << B << ", " << temperature << ", " << total_time << "\n";
		for (auto t = 0; t < total_time; t += total_time / 1000)
			file << t << ", " << getRo(t) << ", " << getSigma(getRo(t)) << "\n";
	}
	file.close();
}

void CARecrystallizer::recrystallize(double x, bool save, std::string path)
{
	if (save)
	{

	}
	auto turn = 0;
	for (auto t = 0; t < total_time; t += total_time / 1000)
	{
		// calculate deltaRo and distribute the uniform part
		auto deltaRo = getRo(t + total_time / 1000) - getRo(t);
		auto deltaRoUniform = deltaRo * x / 100.;
		deltaRo -= deltaRoUniform;
		deltaRoUniform = deltaRoUniform / cellMap.size() / cellMap.at(0).size();
		for (auto& row : cellMap)
			for (auto& cell : row)
				cell.dislocation_density = deltaRoUniform;
		// calculate packets and distribute them
		auto n = gatherEdges();
		auto packetNo = cellMap.size() * cellMap.at(0).size() * 100;
		auto deltaRoPacket = deltaRo / packetNo;
		for (auto i = 0; i < packetNo; ++i)
		{
			if (dice() % 10 < 2) // on edge
			{
				auto[x, y, b] = edges.at(dice() % edges.size());
				cellMap.at(x).at(y).dislocation_density += deltaRoPacket;
			}
			else
			{
				cellMap.at(dice() % cellMap.size()).at(dice() % cellMap.at(0).size()).dislocation_density += deltaRoPacket;
			}
		}
		// check for new nuclei
		for (auto&[a, b, e] : edges)
			if (cellMap.at(a).at(b).dislocation_density > critical)
			{
				cellMap.at(a).at(b).recrystallized = turn;
				cellMap.at(a).at(b).dislocation_density = 0;
				updateCell(a, b, sf::Color(palette == 0 ? dice() % 255 : 0, palette == 1 ? dice() % 255 : 0, palette == 2 ? dice() % 255 : 0));
			}
		// grow existing nuclei
		auto check = [&](auto k, auto l, auto x, auto y) -> bool {
			return cellMap.at(k).at(l).dislocation_density < cellMap.at(x).at(y).dislocation_density;
		};
		for (auto i = 0; i < cellMap.size(); ++i)
			for (auto j = 0; j < cellMap.at(i).size(); ++j)
			{
				// do not check those already recrystallized
				if (cellMap.at(i).at(j).recrystallized)
					continue;
				bool highest = true;
				auto recrystNeighb_x = 0, recrystNeighb_y = 0;
				if (boundaryCondition)
				{
					// left
					if ((n != Pentagonal_Right))
					{
						if (check(i, j, (i + cellMap.size() - 1) % cellMap.size(), j))
							highest = false;
						if (cellMap.at((i - 1) % cellMap.size()).at(j).recrystallized == turn - 1)
						{
							recrystNeighb_x = (i - 1) % cellMap.size();
							recrystNeighb_y = j;
						}
					}
					// top
					if ((n != Pentagonal_Bottom) && check(i, j, i, (j + cellMap.at(i).size() - 1) % cellMap.at(i).size()))
					{
						edges.emplace_back(i, j, false);
						continue;
					}
					// right
					if ((n != Pentagonal_Left) && check(i, j, (i + 1) % cellMap.size(), j))
					{
						edges.emplace_back(i, j, false);
						continue;
					}
					// bottom
					if ((n != Pentagonal_Top) && check(i, j, i, (j + 1) % cellMap.at(i).size()))
					{
						edges.emplace_back(i, j, false);
						continue;
					}
					// top-right
					if (n == Moore || n == Hexagonal_Right || n == Pentagonal_Top || n == Pentagonal_Right &&
						check(i, j, (i + 1) % cellMap.size(), (j + cellMap.at(i).size() - 1) % cellMap.at(i).size()))
					{
						edges.emplace_back(i, j, false);
						continue;
					}
					// bottom-right
					if (n == Moore || n == Hexagonal_Left || n == Pentagonal_Right || n == Pentagonal_Bottom &&
						check(i, j, (i + 1) % cellMap.size(), (j + 1) % cellMap.at(i).size()))
					{
						edges.emplace_back(i, j, false);
						continue;
					}
					// bottom-left
					if (n == Moore || n == Hexagonal_Right || n == Pentagonal_Bottom || n == Pentagonal_Left &&
						check(i, j, (i + cellMap.size() - 1) % cellMap.size(), (j + 1) % cellMap.at(i).size()))
					{
						edges.emplace_back(i, j, false);
						continue;
					}
					// top-left
					if (n == Moore || n == Hexagonal_Left || n == Pentagonal_Top || n == Pentagonal_Left &&
						check(i, j, (i + cellMap.size() - 1) % cellMap.size(), (j + cellMap.at(i).size() - 1) % cellMap.at(i).size()))
					{
						edges.emplace_back(i, j, false);
						continue;
					}
				}
				else
				{
					// left
					if (i > 0 && n != Pentagonal_Right && check(i, j, i - 1, j))
					{
						edges.emplace_back(i, j, false);
						continue;
					}
					// top
					if (j > 0 && n != Pentagonal_Bottom && check(i, j, i, j - 1))
					{
						edges.emplace_back(i, j, false);
						continue;
					}
					// right
					if (i < cellMap.size() - 1 && n != Pentagonal_Left && check(i, j, i + 1, j))
					{
						edges.emplace_back(i, j, false);
						continue;
					}
					// bottom
					if (j < cellMap.at(i).size() - 1 && n != Pentagonal_Top && check(i, j, i, j + 1))
					{
						edges.emplace_back(i, j, false);
						continue;
					}
					// top-right
					if (j > 0 && i < cellMap.size() - 1 && (n == Moore || n == Hexagonal_Right || n == Pentagonal_Top || n == Pentagonal_Right) && check(i, j, i + 1, j - 1))
					{
						edges.emplace_back(i, j, false);
						continue;
					}
					// bottom-right
					if (i < cellMap.size() - 1 && j < cellMap.at(i).size() - 1 && n == Moore || n == Hexagonal_Left || n == Pentagonal_Right || n == Pentagonal_Bottom && check(i, j, i + 1, j + 1))
					{
						edges.emplace_back(i, j, false);
						continue;
					}
					// bottom-left
					if (i > 0 && j < cellMap.at(i).size() - 1 && n == Moore || n == Hexagonal_Right || n == Pentagonal_Bottom || n == Pentagonal_Left && check(i, j, i - 1, j + 1))
					{
						edges.emplace_back(i, j, false);
						continue;
					}
					// top-left
					if (i > 0 && j > 0 && n == Moore || n == Hexagonal_Left || n == Pentagonal_Top || n == Pentagonal_Left && check(i, j, i - 1, j - 1))
					{
						edges.emplace_back(i, j, false);
						continue;
					}
				}
				if (highest && recrystNeighb_x)
					updateCell(i, j, cellMap.at(recrystNeighb_x).at(recrystNeighb_y).color);
			}
		/*t
		getRo(t)
		getSigma(getRo(t))*/
		++turn;
	}
}

// PRIVATE ////////////////////////////////////////////////////////////////////

double CARecrystallizer::getRo(double t)
{
	return (A / B) + (1 - A / B)*exp(-B * t);
}

double CARecrystallizer::getSigma(double ro)
{
	return 0 + 1.9*2.57e-10*8e10*sqrt(ro);
}