

#pragma hdrstop

#include "FittableFunction.h"

#pragma package(smart_init)


#include "math_util.h"
#include "stdio.h"


   FittableFunction::FittableFunction() {
		parameters = NULL;
		sigmas = NULL;
		covarianceMatrix = NULL;
		adjustParameters = NULL;
		chiSquared = NAN;
		nParameters = -1;
	}

	FittableFunction::~FittableFunction() {
		delete1D(parameters);
		delete1D(sigmas);
		delete1D(adjustParameters);
		delete2D(covarianceMatrix, nParameters);
       }

	void FittableFunction::setChiSquared(double chiSquared) {
		this->chiSquared = chiSquared;
	}



	double FittableFunction::getParameter(int i) {
		return parameters[i];
	}


	double FittableFunction::getParameterError(int i) {
		return sqrt(covarianceMatrix[i][i]);
	}



	void FittableFunction::setParameterError(int i, double err) {
		covarianceMatrix[i][i] = err * err;
	}


	void FittableFunction::setParameter(int i, double value, double err) {
		parameters[i] = value;
		if (covarianceMatrix == NULL) {
			covarianceMatrix = new2D<double>(nParameters,nParameters,0.0);
		}
		covarianceMatrix[i][i] = err * err;
	}


	void FittableFunction::setParameter(int i, double v) {
		parameters[i] = v;
	}



	double FittableFunction::getChiSquared() {
		return chiSquared;
	}



	void FittableFunction::setCovarianceMatrix(double **covarianceMatrix) {
		double **newMatrix = new2D<double>(nParameters,nParameters,0);
		for (int i = 0; i < nParameters; i++) {
			for (int j=0; j< nParameters; j++) {
				newMatrix[i][j] = covarianceMatrix[i][j];
			}
		}

		this->covarianceMatrix = newMatrix;
	}


	double** FittableFunction::getCovarianceMatrix() {
		if (covarianceMatrix == NULL) {
			return NULL;
		}

		double** newMatrix = new2D<double>(nParameters, nParameters, 0.0);
		for (int i = 0; i < nParameters; i++) {
			for(int j=0; j<nParameters; j++) {
				  newMatrix[i][j] = covarianceMatrix[i][j];
			}
		}

		return newMatrix;
	}



	double* FittableFunction::getParameters() {
		return parameters;
	}

	double* FittableFunction::getSigmas() {
		if(!sigmas) sigmas = new1D(nParameters,0.0);
		if(covarianceMatrix == NULL) return NULL;
		for(int i=0; i<nParameters; ++i) {
			sigmas[i] = sqrt(covarianceMatrix[i][i]);
		}
		return sigmas;
	}


	int FittableFunction::getParametersCount() {
		return nParameters;
	}


	void FittableFunction::setParameterState(int param, boolean adjust) {
		adjustParameters[param] = adjust;
	}



	void FittableFunction::setParameters(double *p, int pLength) {
		this->parameters = new1D<double>(pLength, 0.0);
		this->nParameters = pLength;
		adjustParameters = new1D<boolean>(pLength, true);
		for(int i=0; i<pLength; i++) {
			this->parameters[i] = p[i];
		}
	}


	boolean* FittableFunction::getAdjustParameters() {
		return adjustParameters;
	}



