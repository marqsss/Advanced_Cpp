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
	std::ofstream file;
	if (save)
	{
		file.open(path);
		if (!file.is_open())
			printf("ERROR opening output file'n");
	}
	auto turn = 0;
	for (double t = 0; t < total_time; t += total_time / 100.)
	{
		// file output
		if (save)
		{
			double ro_a = 0;
			for (auto& row : cellMap)
				for (auto& cell : row)
					ro_a += cell.dislocation_density;
			file << "time = " << t << "\tro_c = " << getRo(t) << "\t, ro_a = " << ro_a << "\n";
		}
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
		auto packetNo = cellMap.size() * cellMap.at(0).size();
		auto deltaRoPacket = deltaRo / packetNo;
		for (auto i = 0; i < packetNo; ++i)
		{
			if (dice() % 10 > 1) // on edge
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
		auto criticalUnit = critical / cellMap.size() / cellMap.at(0).size()/100;
		for (auto&[a, b, e] : edges)
			if (cellMap.at(a).at(b).dislocation_density > criticalUnit)
			{
				cellMap.at(a).at(b).recrystallized = turn;
				cellMap.at(a).at(b).dislocation_density = 0;
				auto color = sf::Color(dice() % 255, 0, 0, 255);
				updateCell(a, b, color);
				//texMap.update(color., 1, 1, a, b);
			}
		// grow existing nuclei
		auto check = [&](auto k, auto l, auto x, auto y) -> bool {
			return cellMap.at(k).at(l).dislocation_density < cellMap.at(x).at(y).dislocation_density;
		};
		if(turn>5)
		for (auto i = 0; i < cellMap.size(); ++i)
			for (auto j = 0; j < cellMap.at(i).size(); ++j)
			{
				// do not check those already recrystallized
				if (cellMap.at(i).at(j).recrystallized)
					continue;
				// get ready and check neighbours
				unsigned int n = neighbourhood;
				if (n == Random)
					n = dice() % 8;
				else if (n == Hexagonal_Random)
					n = dice() % 2 + Hexagonal_Left;
				else if (n == Pentagonal_Random)
					n = dice() % 4 + Pentagonal_Left;
				bool highest = true;
				auto recrystNeighb_x = 0, recrystNeighb_y = 0;

				if (boundaryCondition)
				{
					// left
					if ((n != Pentagonal_Right))
					{
						if (check(i, j, (i - 1) % cellMap.size(), j))
							highest = false;
						if (cellMap.at((i - 1) % cellMap.size()).at(j).recrystallized == (turn - 1))
						{
							recrystNeighb_x = (i - 1) % cellMap.size();
							recrystNeighb_y = j;
						}
					}
					// top
					if ((n != Pentagonal_Bottom))
					{
						if (check(i, j, i, (j - 1) % cellMap.at(i).size()))
							highest = false;
						if (cellMap.at(i).at((j - 1) % cellMap.at(i).size()).recrystallized == (turn - 1))
						{
							recrystNeighb_x = i;
							recrystNeighb_y = (j - 1) % cellMap.at(i).size();
						}
					}
					// right
					if ((n != Pentagonal_Left))
					{
						if (check(i, j, (i + 1) % cellMap.size(), j))
							highest = false;
						if (cellMap.at((i + 1) % cellMap.size()).at(j).recrystallized == (turn - 1))
						{
							recrystNeighb_x = (i + 1) % cellMap.size();
							recrystNeighb_y = j;
						}
					}
					// bottom
					if ((n != Pentagonal_Top))
					{
						if (check(i, j, i, (j + 1) % cellMap.at(i).size()))
							highest = false;
						if (cellMap.at(i).at((j + 1) % cellMap.at(i).size()).recrystallized == (turn - 1))
						{
							recrystNeighb_x = i;
							recrystNeighb_y = (j + 1) % cellMap.at(i).size();
						}
					}
					// top-right
					if (n == Moore || n == Hexagonal_Right || n == Pentagonal_Top || n == Pentagonal_Right)
					{
						if (check(i, j, (i + 1) % cellMap.size(), (j - 1) % cellMap.at(i).size()))
							highest = false;
						if (cellMap.at((i + 1) % cellMap.size()).at((j - 1) % cellMap.at(i).size()).recrystallized == (turn - 1))
						{
							recrystNeighb_x = (i + 1) % cellMap.size();
							recrystNeighb_y = (j - 1) % cellMap.at(i).size();
						}
					}
					// bottom-right
					if (n == Moore || n == Hexagonal_Left || n == Pentagonal_Right || n == Pentagonal_Bottom)
					{
						if (check(i, j, (i + 1) % cellMap.size(), (j + 1) % cellMap.at(i).size()))
							highest = false;
						if (cellMap.at((i + 1) % cellMap.size()).at((j + 1) % cellMap.at(i).size()).recrystallized == (turn - 1))
						{
							recrystNeighb_x = (i + 1) % cellMap.size();
							recrystNeighb_y = (j + 1) % cellMap.at(i).size();
						}
					}
					// bottom-left
					if (n == Moore || n == Hexagonal_Right || n == Pentagonal_Bottom || n == Pentagonal_Left)
					{
						if (check(i, j, (i - 1) % cellMap.size(), (j + 1) % cellMap.at(i).size()))
							highest = false;
						if (cellMap.at((i - 1) % cellMap.size()).at((j + 1) % cellMap.at(i).size()).recrystallized == (turn - 1))
						{
							recrystNeighb_x = (i - 1) % cellMap.size();
							recrystNeighb_y = (j + 1) % cellMap.at(i).size();
						}
					}
					// top-left
					if (n == Moore || n == Hexagonal_Left || n == Pentagonal_Top || n == Pentagonal_Left)
					{
						if (check(i, j, (i - 1) % cellMap.size(), (j - 1) % cellMap.at(i).size()))
							highest = false;
						if (cellMap.at((i - 1) % cellMap.size()).at((j - 1) % cellMap.at(i).size()).recrystallized == (turn - 1))
						{
							recrystNeighb_x = (i - 1) % cellMap.size();
							recrystNeighb_y = (j - 1) % cellMap.at(i).size();
						}
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
			} // grow nuclei loop
		++turn;
	}
	if (save)
		file.close();
	visualize();
}

void CARecrystallizer::drawing_mode(int mode)
{
	if (mode == 1)
	{
		imgMap.create(cellMap.size(), cellMap.at(0).size(), sf::Color::White);
		gatherEdges();
		for (auto i = 0; i < edges.size(); ++i)
		{
			auto[x, y, d] = edges.at(i);
			imgMap.setPixel(x, y, cellMap.at(x).at(y).color);
		}
	}
	if (mode == 2)
	{
		imgMap.create(cellMap.size(), cellMap.at(0).size(), sf::Color::White);
		double maxDisloc = 0;
		for (auto& row : cellMap)
			for (auto& cell : row)
				if (cell.dislocation_density > maxDisloc)
					maxDisloc = cell.dislocation_density;
		for (auto i = 0; i < cellMap.size(); ++i)
			for (auto j = 0; j < cellMap.at(i).size(); ++j)
				imgMap.setPixel(i, j, sf::Color(cellMap.at(i).at(j).dislocation_density / maxDisloc * 255, 0, 0));
	}
	else
	{
		imgMap.create(cellMap.size(), cellMap.at(0).size(), sf::Color::White);
		for (auto i = 0; i < cellMap.size(); ++i)
			for (auto j = 0; j < cellMap.at(i).size(); ++j)
				imgMap.setPixel(i, j, cellMap.at(i).at(j).color);
	}
	texMap.create(cellMap.size(), cellMap.at(0).size());
	visualize();
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