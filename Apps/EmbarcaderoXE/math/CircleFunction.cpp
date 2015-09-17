

//---------------------------------------------------------------------------


#pragma hdrstop

#include "FittableFunction.h"
#include "CircleFunction.h"
#include "stdio.h"
#include "FitFailedException.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------



	CircleFunction::CircleFunction(double x0, double y0, double r) {
		int pLength = 3;
		double* p = new1D<double>(pLength, 0.0);
		p[0] = x0;
		p[1] = y0;
		p[2] = r;
		setParameters(p, pLength);
		delete1D(p);
	}



	  double CircleFunction::getValueAndDerivatives(
		const double *x, const double *parameters, double *derivatives) {

		//sqrt((x-x0)^2 + (y-y0)^2) - r = 0 for perfect fit, independent parameter set to zero
		//to remove bias between x and y.
		double d = sqrt(pow((x[0]-parameters[0]),2) + pow((x[1]-parameters[1]),2))-parameters[2];
		 if(d+parameters[2] == 0) throw FitFailedException("Divide by zero error in CircleFunction fit.  Points are probably in a line.");
		derivatives[0] = (parameters[0]-x[0])/(d+parameters[2]);
		derivatives[1] = (parameters[1]-x[1])/(d+parameters[2]);
		derivatives[2] = -1;

		return d;
		}

	double CircleFunction::getX0() {
		return parameters[0];
	}


	double CircleFunction::getY0() {
		return parameters[1];
	}

	double CircleFunction::getR() {
		return parameters[2];
	}






