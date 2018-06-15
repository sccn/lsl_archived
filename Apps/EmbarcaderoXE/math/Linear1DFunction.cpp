//---------------------------------------------------------------------------


#pragma hdrstop

#include "FittableFunction.h"
#include "Linear1DFunction.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------



	Linear1DFunction::Linear1DFunction(double k, double b) {
		int pLength = 2;
		double* p = new1D<double>(pLength, 0.0);
		p[0] = k;
		p[1] = b;
		setParameters(p, pLength);
		delete1D(p);
	}



	  double Linear1DFunction::getValueAndDerivatives(
		const double *x, const double *parameters, double *derivatives) {

		derivatives[0] = x[0];
		derivatives[1] = 1;

		return parameters[0] * x[0] + parameters[1];
		};

	double Linear1DFunction::getSlope() {
		return parameters[0];
	}


	double Linear1DFunction::getIntercept() {
		return parameters[1];
	}


	double Linear1DFunction::getValueAt(double x) {
		return parameters[0] * x + parameters[1];
	}


/*
	public String getDescription() {
		return "Linear 1D Function";
	}
*/

//This is the more efficient way.  Not yet ported to C++
 /*	public Linear1DFunction lineFit(double* x, double* y, double* sigma, int dataLength) {
		double s = 0, sx = 0, sy = 0, sxx = 0, sxy = 0;
		for (int i = 0; i < dataLength; i++) {
			s += 1 / (sigma[i] * sigma[i]);
			sx += x[i] / (sigma[i] * sigma[i]);
			sy += y[i] / (sigma[i] * sigma[i]);
			sxx += (x[i] * x[i]) / (sigma[i] * sigma[i]);
			sxy += (x[i] * y[i]) / (sigma[i] * sigma[i]);
		}
		double delta = s * sxx - sx * sx;
		double intercept = (sxx * sy - sx * sxy) / delta;
		double slope = (s * sxy - sx * sy) / delta;
		double sigmaIntercept2 = sxx / delta;
		double sigmaSlope2 = s / delta;

		double chi2 = 0;
		for (int i = 0; i < dataLength; i++) {
			double a = (y[i] - intercept - slope * x[i]) / sigma[i];
			chi2 += a * a;
		}

		Linear1DFunction l = new Linear1DFunction();
		l.setParameters(new double[] {slope, intercept});
		l.setChiSquared(chi2);
		l.setCovarianceMatrix(new double[][] { {sigmaSlope2, 0}
							  , {0, sigmaIntercept2}
		});
		return l;
	}
   */


