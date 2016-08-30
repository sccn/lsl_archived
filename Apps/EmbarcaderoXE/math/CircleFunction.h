/**
 * Implemets a fittable circle.
 * Parameters: "x0", "y0", "r".
 *
 * @author Matthew Grivich, University of California, San Diego
 */

#ifndef CircleFunctionH
#define CircleFunctionH

#include "FittableFunction.h"



class CircleFunction: public FittableFunction {


	public:
		 CircleFunction(double x0, double y0, double r);


	virtual double getValueAndDerivatives(
		const double *x, const double *parameters, double *derivatives);

	double getX0();

	double getY0();

	double getR();


};


#endif

