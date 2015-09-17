#ifndef REGRESSION_H
#define REGRESSION_H


#include <cstdlib>
#include "boost/multi_array.hpp"

class MovingRegression
{
public:
	MovingRegression():
	  a(1.0),
	  b(0.0),
	  sumX(0.0),
	  sumY(0.0),
	  sumXY(0.0),
	  sumXX(0.0),
	  interval(1000)
	  {
	  }


	void reset(long n)
	{
		interval = n;
		a = 1.0;
		b = 0.0;
		sumX = 0.0;
		sumY = 0.0;
		sumXY = 0.0;
		sumXX = 0.0;
		writePointer = 0;
		readPointer = 0;
		bufferX.resize(interval);
		bufferY.resize(interval);
	}

	double get(double x, double y)
	{
		double lastX, lastY, n;
		bufferX[writePointer] = x;
		bufferY[writePointer] = y;
		writePointer++;
		if(writePointer >= interval)
			writePointer = 0;

		sumX += x;
		sumY += y;
		sumXX += x*x;
		sumXY += x*y;
	
		if(readPointer < interval)
			readPointer++;
		else
		{
			lastX = bufferX[writePointer];
			lastY = bufferY[writePointer];
			sumX -= lastX;
			sumY -= lastY;
			sumXX -= lastX*lastX;
			sumXY -= lastX*lastY;
		}
		n = (double)readPointer;
		try
		{
			a = (sumXY - sumX*sumY/n) / (sumXX - sumX*sumX/n);
		}
		catch(...)
		{
			a = 1.0;
		}
		b = sumY/n - a*sumX/n;
		if(readPointer > 1)
			y = a * x + b;
		return y;
	}



private:
	double a, b;
	double sumX, sumY, sumXY, sumXX;
	long interval;
	long writePointer;
	long readPointer;
	std::vector<double> bufferX;
	std::vector<double> bufferY;
};




#endif // REGRESSION_H
