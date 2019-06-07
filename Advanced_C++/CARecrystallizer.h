#ifndef CARECRYSTALLIZER_H
#define CARECRYSTALLIZER_H

#include "CellularAutomaton.h"

class CARecrystallizer : public CellularAutomaton
{
public:
	CARecrystallizer() {}
	CARecrystallizer(double a, double b, double temp, double time, double crit) :
		A(a), B(b), temperature(temp), total_time(time), critical(crit) {}
	void calculateToFile(std::string path);
	void calculateToCSV(std::string path);
	void recrystallize(double uniform_percent, bool saveToFile, std::string path = "");
	void drawing_mode(int mode = 0);
	void visualize(bool MC = false) { texMap.update(imgMap); setTexture(texMap); }

	void setA(double a) { A = a; }
	void setB(double b) { B = b; }
	void setTemperature(double t) { temperature = t; }
	void setTotalTime(double tt) { total_time = tt; }
	void setCritical(double cr) { critical = cr; }

private:
	double getRo(double t);
	double getSigma(double ro);

	double A;
	double B;
	double temperature;
	double total_time;
	double critical;
};

#endif