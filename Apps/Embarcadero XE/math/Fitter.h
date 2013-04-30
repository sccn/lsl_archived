 /**
 Fitter class. Given a set of independent coordinates y[], and a set of dependent values
 x[][] with error sigma[], finds a fit according to FittableFunction f. Uses the
 Levenberg-Marquardt algorithm.

Written by Matthew Grivich, Swartz Center for Computational Neuroscience, UCSD
after Dumitru Petrusca, University of California, Santa Cruz
*/

#ifndef FitterH
#define FitterH


#include <vcl.h>


#pragma hdrstop

#include "stdio.h"
#include "math_util.h"
#include "FittableFunction.h"

//workaround for an obscure embacadero bug.
//When codeguard is enabled, a "Method called on illegaly casted object in process" can be thrown on new Fitter()
//This is caused by incorrect byte alignment.
//http://qc.embarcadero.com/wc/qcmain.aspx?d=31594
#pragma pack(push, 4)
//end workaround

class Fitter {
	private:

		boolean DEBUG;
		double lamda, chi2, maxLambda;
			int paramsCount, needAdjustCount, nData, nDimensions;
		boolean *needAdjust;
		double *y, *sigma, *params, *initialParams, *beta, *da, *derivative, *coords;
		double **x, **oneDa, **covar;
		FittableFunction *function;

	public:
		double deltaChi2;
		int maxIterations;
		Fitter();
		~Fitter();
	private:
	/**
	 * The Gauss Jordan linear equations solver
	 * Borrows heavily from numerical recipies, section 2.1
	 */
	void gaussj(double** a, int n, double** b, int m);


	/**
	 * This method fills the arrays Alpha and Beta and calculates and returns
	 * the Chi-Square. Alpha and Beta can then be used to solve the
	 * Levenberg-Marquardt set of linear equations w.r.t to da.
	 */
	double prepareMatrices(double** alpha, double* beta);

	// the initialization step
	void initializeFit(double** alpha);

	// the finalization step
	void finishFit(double** covar, double** alpha);


	/**
	 * Levenberg-Marquardt solver. Borrows heavily from Numerical Recipies, section 15.5
	 */
	void mrqmin(double** covar, double** alpha);


	// this rearanges the covariance matrix at the end of the fitting process
	void covsrt(double** covar, boolean* needAdjust,
							   int needAdjustCount);




	/**
	 * Returns the resulting covarince matrix of the parameters as two dimesional array
	 * in row-major format.
	 */
	public:
	double** getCovarianceMatrix();



	 double* getParameters();

	/**
	 * This method allows to set the precision of the Chi-Squared at which the fitting
	 * is considered done.
	 *
	 * @param deltaChi2 the minimum varation between two consecutive iterations at
	 *        which the fit stops
	 */
	void setLastChiSquaredVariation(double deltaChi2);


	/**
	 * This method allows to set the total number of iterations made by the fit
	 * funtion. If the number of iterations is exceeded a FitFailedException gets
	 * thrown by the fit() methods. Default value is 1000.
	 *
	 * @param maxIterations the minimum number of iterations
	 */
	void setMaxIterations(int maxIterations);



	/**
	 * Performs a Levenberg-Marquardt multiparameter fit of the given multidimensional
	 * function to the data set described by the n-dimensional coordinates <code>x</code>
	 * and the measured values <code>y</code> with errors <code>sigma</code>. The
	 * results of the fit (Chi-Squared, parameters, covariance matrix) are set
	 * into the model funtion.
	 *
	 * @param function The model function to be fitted
	 * @param x The n-dimensional coordinates
	 * @param y The measured values
	 * @param sigma The errors of the measurements
	 * @param nData the number of actual data points contained in <b>y</b> and <b>x</b>.
	 * @throws FitFailedException In the case the fitting process fails due to
	 *         various reasons (is a checked exception).
	 * @throws InvalidFitDataException In the case the provided data are invalid
	 *         (is NOT a checked exception).
	 */
	double fit(
		FittableFunction* function, double** x, double* y, double* sigma, int nDimensions, int nData);

	/**
	 * Utility function to fit a conventional x (dependent) vs y (independent) function.
	 */
	double fit1D(
		FittableFunction* function, double* x, double* y, double* sigma, int nData);


	private:
	 double _fit(
		FittableFunction* function, double** x, double* y, double* sigma, int nDimensions, int nData);

};
//see above for explanation
#pragma pack(pop)

#endif



