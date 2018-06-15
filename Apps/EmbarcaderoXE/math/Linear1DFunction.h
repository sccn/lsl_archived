

//---------------------------------------------------------------------------

#ifndef Linear1DFunctionH
#define Linear1DFunctionH
//---------------------------------------------------------------------------
#endif

#include "FittableFunction.h"


/**
 * Implemets a fittable linear function.
 * Parameters: "slope", "intercept".
 *
 * @author Dumitru Petrusca, University of California, Santa Cruz
 */
class Linear1DFunction: public FittableFunction {


	public:
		 Linear1DFunction(double k, double b);


	virtual double getValueAndDerivatives(
		const double *x, const double *parameters, double *derivatives);

	double getSlope();


	double getIntercept();


	double getValueAt(double x);



};

