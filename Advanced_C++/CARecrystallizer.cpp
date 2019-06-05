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
	auto i = 0;
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
				cellMap.at(a).at(b).recrystallized = i;
				cellMap.at(a).at(b).dislocation_density = 0;
				updateCell(a, b, sf::Color(palette == 0 ? dice() % 255 : 0, palette == 1 ? dice() % 255 : 0, palette == 2 ? dice() % 255 : 0));
			}
		// grow existing nuclei
		// double for with indexes, check neighbours for recrystallized==i-1 && this cell has max density of neighbours
		/*t
		getRo(t)
		getSigma(getRo(t))*/
		++i;
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