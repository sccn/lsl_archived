
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <math.h>
#include <algorithm>
#ifndef FittableFunctionH
#define FittableFunctionH

#include "math_util.h"



/**
 * This class represents the model function that needs to be passed to the
 * Fitter class along with the data to be fit. This is an abstract class
 * that implements some basic functionality needed for any function. The concrete
 * implementations must implement the virtual getValueAndDerivatives()
 * function to make the implementation useful.
 *
 * @author Dumitru Petrusca, University of California, Santa Cruz
 * @author Matthew Grivich, University of California, San Diego (ported from Java to C++)
 */
class FittableFunction {
	private:


	public:
		double *parameters, *sigmas;
		double chiSquared;
		double **covarianceMatrix;
		int nParameters;
		boolean *adjustParameters;

		FittableFunction();
		~FittableFunction();
	/**
	 * Used by the fitter to set the chi squared after a fit.
	 * @param chiSquared double
	 */
	void setChiSquared(double chiSquared);




	/**
	 * Returns the parameters indexed by "i".
	 * @param i int
	 * @return double
	 */
	double getParameter(int i);

	/**
	 * Returns the error of the parameter indexed by "i".
	 * @param i int
	 * @return double
	 */
	double getParameterError(int i);
	/**
	 * Sets the error of the parameter indexed by "i".
	 * @param i int
	 * @return double
	 */
	void setParameterError(int i, double err);

	/**
	 * Sets the value and error of the parameter indexed by "i".
	 * @param i int
	 * @return double
	 */
	void setParameter(int i, double value, double err);
	void setParameter(int i, double v);



	/**
	 * Returns the Chi-Squared the funtion got after the fit.
	 */
	double getChiSquared();
	/**
	 * used by the fitter to write back the error matrix.
	 * @param covarianceMatrix double[][]
	 */
	void setCovarianceMatrix(double **covarianceMatrix);

	/**
	 * Returns the covarince matrix of the parameters the funtion got after the fit.
	 * If called before the fit or if the fit failed this will return <b>null</b>.
	 */
	double** getCovarianceMatrix();

	/**
	 * Returns the parameters array.
	 */
	double* getParameters();

	double* getSigmas();

	int getParametersCount();

	/**
	 * If the state of a parameter in "true" it is considered for adjustment by the fitter.
	 * It is held contant if the state is "false".
	 * @param param int
	 * @param adjust boolean
	 */
	void setParameterState(int param, boolean adjust);


	/**
	 * Used to set the value of parameters. This method MUST be called before the fit
	 * happens to properly set the starting values of parameters. If the fit is
	 * successful the Fitter will set the final values using this function.
	 */

	void setParameters(double *p, int pLength);

	/**
	 * Returns a boolean array of the same size as the parameters array. If an array
	 * element contains <b>true</b> then the coresponding parameter will be changed
	 * during the fit, otherwise the value of the parameter will be considered fixed.
	 */

	boolean* getAdjustParameters();


	/**
	 * This method MUST be properly implemented by subclasses to provide the value
	 * and derivatives (w.r.t parameters) of the model function.
	 *
	 * @param x the vector of coordinates
	 * @param parameters the vector of current parameters than should be used.
	 *        Do NOT change the values of parameters
	 * @param derivatives the vector in which the calculated derivatives have to be placed
	 * @return the value of the function
	 */
	virtual double getValueAndDerivatives(
		const double *x, const double *parameters, double *derivatives)=0; 


};
#endif
