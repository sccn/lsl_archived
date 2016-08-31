

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Fitter.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------



#include "stdio.h"
#include "math_util.h"
#include "FittableFunction.h"
#include "FitFailedException.h"
#include "Math.hpp"




	Fitter::Fitter() {
		DEBUG = false;
		deltaChi2 = 1e-4;
		maxIterations = 1000;
		maxLambda = 1e100; //catch before it hits infinity, or thread silently dies.
		params = beta = da = derivative = coords = NULL;
		oneDa = covar = NULL;
		}

	Fitter::~Fitter() {
		delete1D(params);
		delete1D(beta);
		delete1D(da);
		delete1D(derivative);
		delete1D(coords);
		delete2D(oneDa, needAdjustCount);
		delete2D(covar, paramsCount);

	}


	void Fitter::gaussj(double** a, int n, double** b, int m) {

		int icol = 0, irow = 0;
		int* indxc = new1D<int>(n, 0);
		int* indxr = new1D<int>(n, 0);
		int* ipiv = new1D<int>(n, 0);
		try {
			for (int i = 0; i < n; i++) {

				double big = 0, abs;
				for (int j = 0; j < n; j++) {
					if (ipiv[j] != 1) {
						for (int k = 0; k < n; k++) {
							if (ipiv[k] == 0) {
								abs = a[j][k];
								if (abs < 0) {
									abs = -abs;
								}
								if (abs >= big) {
									big = abs;
									irow = j;
									icol = k;
								}
							} else if (ipiv[k] > 1) {
								throw FitFailedException("gaussj: Singular Matrix-1");
							}
						}
					}
				}



				++ (ipiv[icol]);
				if (irow != icol) {
					for (int l = 0; l < n; l++) {
						double t = a[irow][l];
						a[irow][l] = a[icol][l];
						a[icol][l] = t;
					}
					for (int l = 0; l < m; l++) {
						double t = b[irow][l];
						b[irow][l] = b[icol][l];
						b[icol][l] = t;
					}
				}
				indxr[i] = irow;
				indxc[i] = icol;
				if (a[icol][icol] == 0) {
					throw FitFailedException("gaussj: Singular Matrix-2\n");

				}
				double pivinv = 1 / a[icol][icol];
				a[icol][icol] = 1;

				for (int l = 0; l < n; l++) {
					a[icol][l] *= pivinv;
				}
				for (int l = 0; l < m; l++) {
					b[icol][l] *= pivinv;

				}
				for (int ll = 0; ll < n; ll++) {
					if (ll != icol) {
						double dum = a[ll][icol];
						a[ll][icol] = 0;
						for (int l = 0; l < n; l++) {
							a[ll][l] -= a[icol][l] * dum;
						}
						for (int l = 0; l < m; l++) {
							b[ll][l] -= b[icol][l] * dum;
						}
					}
				}
			}

			for (int l = n - 1; l >= 0; l--) {
				if (indxr[l] != indxc[l]) {
					for (int k = 0; k < n; k++) {
						double t = a[k][indxr[l]];
						a[k][indxr[l]] = a[k][indxc[l]];
						a[k][indxc[l]] = t;
					}
				}
			}
		} catch (FitFailedException& ex) {

			delete1D(indxc);
			delete1D(indxr);
			delete1D(ipiv);
			throw ex;
		}
		delete1D(indxc);
		delete1D(indxr);
		delete1D(ipiv);
	}



	double Fitter::prepareMatrices(double** alpha, double* beta) {
		int i, j, k, l, m;
		double ymod, wt, sig2i, dy, chi2 = 0;

		for (j = 0; j < needAdjustCount; j++) {
			for (k = 0; k <= j; k++) {
				alpha[j][k] = 0;
			}
			beta[j] = 0;
		}
		for (i = 0; i < nData; i++) {
			for (int n = 0; n < nDimensions; n++) {
				coords[n] = x[n][i];
			}
			ymod = function->getValueAndDerivatives(coords, params, derivative);
			sig2i = 1 / (sigma[i] * sigma[i]);
			dy = y[i] - ymod;
			chi2 += dy * dy * sig2i;
			for (j = -1, l = 0; l < paramsCount; l++) {
				if (needAdjust[l]) {
					wt = derivative[l] * sig2i;
					for (j++, k = 0, m = 0; m <= l; m++) {
						if (needAdjust[m]) {
							alpha[j][k] += wt * derivative[m];
							k++;
						}
					}
					beta[j] += dy * wt;
				}
			}
		}
		for (j = 1; j < needAdjustCount; j++) {
			for (k = 0; k < j; k++) {
				alpha[k][j] = alpha[j][k];

			}
		}

		return chi2;
	}



	void Fitter::initializeFit(double** alpha) {
		params = new1D<double>(paramsCount,0.0);
		for(int i=0; i<paramsCount; i++) params[i] = initialParams[i];

		needAdjustCount = 0;
		for (int j = 0; j < paramsCount; j++) {
			if (needAdjust[j]) {
				needAdjustCount++;

			}
		}
		oneDa = new2D<double>(needAdjustCount, 1, 0.0);
		beta = new1D<double>(paramsCount,0.0);
		da = new1D<double>(paramsCount,0.0);
		lamda = 0.001;
		chi2 = prepareMatrices(alpha, beta);
	}



	void Fitter::finishFit(double** covar, double** alpha) {
		int j, k, l, m;

		for (j = -1, l = 0; l < paramsCount; l++) {
			if (needAdjust[l]) {
				for (j++, k = 0, m = 0; m < paramsCount; m++) {
					if (needAdjust[m]) {
						covar[j][k] = alpha[j][k];
						k++;
					}
				}
				covar[j][j] = alpha[j][j] * (1.0 + lamda);
				oneDa[j][0] = beta[j];
			}
		}

		gaussj(covar, needAdjustCount, oneDa, 1);
		for (j = 0; j < needAdjustCount; j++) {
			da[j] = oneDa[j][0];

		}
		covsrt(covar, needAdjust, needAdjustCount);
	}


	void Fitter::mrqmin(double** covar, double** alpha) {

		int j, k, l, m;

		// Compute Alpha'[i,j] (the left hand side of the linear equation)
		// and fill the right hand side vector. These two will be passed to
		// the linear equations solver to get the parameter variations.

		for (j = -1, l = 0; l < paramsCount; l++) {
			if (needAdjust[l]) {
				for (j++, k = 0, m = 0; m < paramsCount; m++) {
					if (needAdjust[m]) {
						covar[j][k] = alpha[j][k];
						k++;
					}
				}

				covar[j][j] = alpha[j][j] * (1.0 + lamda);
				oneDa[j][0] = beta[j];

			}
		}

		// Run the linear equation solver to get the parameter variations.
		// Copy the parameter variations (oneDa) into the da array.

		gaussj(covar, needAdjustCount, oneDa, 1);

		for (j = 0; j < needAdjustCount; j++) {
			da[j] = oneDa[j][0];
		}
		// update the values of the running parameters according to da.
		for (j = 0, l = 0; l < paramsCount; l++) {
			if (needAdjust[l]) {
				params[l] = initialParams[l] + da[j];
				j++;
			}
		}

		//
		double newChi2 = prepareMatrices(covar, da);
		if (newChi2 < chi2) {
			lamda *= 0.1;
			chi2 = newChi2;
			for (j = -1, l = 0; l < paramsCount; l++) {
				if (needAdjust[l]) {
					for (j++, k = 0, m = 0; m < paramsCount; m++) {
						if (needAdjust[m]) {
							alpha[j][k] = covar[j][k];
							k++;
						}
					}
					beta[j] = da[j];
					initialParams[l] = params[l];
				}
			}
		} else {
			lamda *= 10.0;
		}
	}



	void Fitter::covsrt(double** covar, boolean* needAdjust,
							   int needAdjustCount) {
		for (int i = needAdjustCount + 1; i < paramsCount; i++) {
			for (int j = 1; j <= i; j++) {
				covar[i][j] = covar[j][i] = 0;

			}
		}
		int k = needAdjustCount - 1;
		for (int j = paramsCount - 1; j >= 0; j--) {
			if (needAdjust[j]) {
				for (int i = 0; i < paramsCount; i++) {
					double swap = covar[i][k];
					covar[i][k] = covar[i][j];
					covar[i][j] = swap;
				}
				for (int i = 0; i < paramsCount; i++) {
					double swap = covar[k][i];
					covar[k][i] = covar[j][i];
					covar[j][i] = swap;
				}
				k--;
			}
		}
	}


	double** Fitter::getCovarianceMatrix() {
		return covar;
	}

	double* Fitter::getParameters() {
		return params;
	}


	void Fitter::setLastChiSquaredVariation(double deltaChi2) {
		if (deltaChi2 < 0) {
			throw FitFailedException("The Chi2 variation must be positive");
		}

		this->deltaChi2 = deltaChi2;
	}


	void Fitter::setMaxIterations(int maxIterations) {
		if (maxIterations < 1) {
			throw FitFailedException("maxIterations must be at least 1");
		}

		this->maxIterations = maxIterations;
	}


	double Fitter::fit(
		FittableFunction* function, double** x, double* y, double* sigma, int nDimensions, int nData) {
		return _fit(function, x, y, sigma, nDimensions, nData);
	}



	double Fitter::fit1D(
		FittableFunction* function, double* x, double* y, double* sigma, int nData) {
		double** x2D = new2D<double>(1,nData, 0.0);
		for(int i =0; i<nData; i++) {
			x2D[0][i] = x[i];
		}
		return _fit(function, x2D, y, sigma, 1, nData);
	}


	 double Fitter::_fit(
		FittableFunction* function, double** x, double* y, double* sigma, int nDimensions, int nData) {


		this->x = x;
		this->y = y;
		this->sigma = sigma;
		this->nDimensions = nDimensions;
		this->nData = nData;
		this->function = function;
		this->initialParams = function->getParameters();
		this->paramsCount = function->nParameters;
		derivative = new1D<double>(paramsCount,0.0);
		coords = new1D<double>(nDimensions,0.0);
		covar = new2D<double>(paramsCount,paramsCount,0.0);
		double** alpha = new2D<double>(paramsCount,paramsCount,0.0);
		try {


		needAdjust = function->getAdjustParameters();
		if (needAdjust == NULL) {
			needAdjust = new1D<boolean>(paramsCount,true);
		}

		double oldChi2;
		initializeFit(alpha);

		for (int iteration = 1; ; iteration++) {
			if (iteration > maxIterations) {
				throw FitFailedException("Maximum number of iterations exceeded.");
			}

			if(lamda > maxLambda) {
				throw FitFailedException("Maximum lambda exceeded.");
			}

			if (DEBUG) {
				printf("Iteration %d, lamda = %d\n", iteration, lamda);
				printf("-----------------------------------------\n");
			}

			oldChi2 = chi2;

			mrqmin(covar, alpha);

			if (chi2 == oldChi2) {
				continue;
			}
			if (fabs(chi2 - oldChi2) < deltaChi2) {
				break;
			}
		}
		finishFit(covar, alpha);
		function->setChiSquared(chi2 / (nData - paramsCount));
		function->setCovarianceMatrix(covar);
		} catch (FitFailedException& e) {
			delete2D(alpha,paramsCount);
			throw e;

		}
		delete2D(alpha,paramsCount);
		return function->getChiSquared();
	}







