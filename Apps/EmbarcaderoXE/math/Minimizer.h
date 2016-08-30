// ---------------------------------------------------------------------------

#ifndef MinimizerH
#define MinimizerH
// ---------------------------------------------------------------------------
#endif
#include "math_util.h"
#include "stdio.h"
#include "math.hpp"

//Follows numerical recepies, section 10.4, Downhill Simplex Method in Multidimensions.

class MinimizerFunction {
	protected:
		double *params;
		double *startingSpread;

	public:
		int nParams;
		MinimizerFunction(double *params, int nParams) {
			this->params = params;
			this->nParams = nParams;
			startingSpread = new1D<double>(nParams, 10.0);
		}
		~MinimizerFunction() {
			delete1D(startingSpread);
		}
		//test get value function, should cause minimizer to converge to params[0] = -4,params[1] = -8
		//Children should override this function.
		virtual double getValue(double *params) {
			return (params[0]+4.0)*(params[0]+4.0)*(params[1]+8.0)*(params[1]+8.0);
		}

	 double *getParameters() {
		return params;
	}

	double *getStartingSpread() {
		return startingSpread;
	}

	void setParameters(double *p) {
		for(int i=0; i<nParams;i++) {
			params[i] = p[i];
		}
	}


	//sets typical range of parameter.  If zero, parameter is fixed.
	void setStartingSpread(double *spread) {
		for(int i=0; i<nParams;i++) {
			startingSpread[i] = spread[i];
		}
	}

};

class Minimizer {
	public:
		double ftol; //fractional convergence tolerance
		int nfunk;  //number of iterations
		int nmax; //maximum number of function evaluations

	private:
		double **p;//[0..ndim][0..ndim-1] ndim+1 rows are ndim-dimenstional vectors which are vertices of starting simplex
		double *y;//[0..ndim] function evaluated at simplex verices.
		int ndim;
		MinimizerFunction *function; //Describes distance between modeled data and actual.  To be minimized.
		double *startingSpread;

	public:
		Minimizer(MinimizerFunction *function) {
			ftol = 1e-14;
			nfunk = 0;
			nmax = 100000;
			this->function = function;
			this->ndim = function->nParams;
			p = new2D(function->nParams+1, function->nParams, 0.0);
		}

		void DoMinimize() {
			initializeAmoeba();
			runAmoeba();
			function->setParameters(p[0]);

		}
		~Minimizer() {
			  delete2D(p, function->nParams+1);
			  delete1D(y);
		}


	private:
		void initializeAmoeba() {
			double *params = function->getParameters();
			startingSpread = function->getStartingSpread();
					//build starting amoeba, mean value is initial guess.
			for(int vect = 0; vect < function->nParams+1; vect++) {
				for(int dim = 0; dim < function->nParams; dim++) {
					p[vect][dim] = params[dim] - startingSpread[dim]/(function->nParams+1);
				}
				if(vect > 0) p[vect][vect-1] += startingSpread[vect-1];
			}

//			for(int vect = 0; vect < function->nParams+1; vect++) {
//				for(int dim = 0; dim < function->nParams; dim++) {
//							printf("p[vect][dim] %d %d %g\n", vect, dim, p[vect][dim]);
//				}
//			}

			y = new1D(function->nParams+1, 0.0);

			//find function value at each vertex.
			for(int vect = 0; vect < function->nParams+1; vect++) {
				y[vect] = function->getValue(p[vect]);
			}

		}


		void runAmoeba() {
			int i, ihi,ilo,inhi,j;
			int mpts = ndim+1;
			double rtol, sum, swap, ysave, ytry, *psum;
			psum = new1D(ndim, 0.0);
			nfunk = 0;
			printf("ndim: %d\n", ndim);
			//start psum
			for(j=0; j<ndim;j++) {
				for(sum=0.0, i=0;i<mpts;i++) sum+=p[i][j];
				psum[j]=sum;

			}
			//endpsum

			while(true) {
				ilo = 0;
				//first we must determine which point is the highest (worst),
				//next-highest, and lowest
				ihi = y[0]>y[1] ? (inhi=1,0) : (inhi=0,1);
				for(i=0; i<mpts;i++) {
					if(y[i] <=y[ilo]) ilo=i;
					if(y[i] > y[ihi]) {
						inhi=ihi;
						ihi=i;
					} else if (y[i] > y[inhi] && i != ihi) inhi=i;
				}


				if(y[ihi] == 0 && y[ilo] == 0) rtol = 0;
				else rtol = 2.0*fabs(y[ihi]-y[ilo])/(fabs(y[ihi])+fabs(y[ilo]));
			   //	printf("rtol: %g, ftol: %g\n", rtol, ftol);
				//compute the fractional range from highest to lowest and return if satisfactory
				if(rtol < ftol || nfunk >= nmax) { //if returning, put best point and value in slot 1

					swap = y[0];
					y[0] = y[ilo];
					y[ilo] = swap;
					for(i=0; i<ndim; i++) {
						swap = p[0][i];
						p[0][i] = p[ilo][i];
						p[ilo][i] = swap;
					}

					if(nfunk >= nmax) printf("nmax exceeded.\n");
					break;
				}

				nfunk += 2;
				//begin a new iteration.  First extrapolate factor by -1 through
				//the face of the simplex across from the high point. i.e. reflect the simplex
				//from the high point.
				ytry = amotry(psum,ihi,-1.0);
				if(ytry <= y[ilo])
					//gives a result better than the best point, so try an additional
					//extrapoloation by a factor 2
					ytry = amotry(psum, ihi, 2.0);
				else if(ytry >= y[inhi]) {
					//the reflected pont is worse than the second-highest, so look
					//for an intermediate lower point, i.e. do a one-dimenstional
					//contraction.
					ysave=y[ihi];
					ytry=amotry(psum,ihi,0.5);
					//can't seem to get rid of that high point.
					//better contract around the lowest (best) point.
					if(ytry >= ysave) {
						for(i=0;i<mpts;i++) {
							if(i != ilo) {
								for(j=0;j<ndim;j++) {
									if(startingSpread[j] != 0.0) //to prevent floating point error buildup
										p[i][j]=0.5*(p[i][j]+p[ilo][j]);

								}
								y[i]=function->getValue(p[i]);
							}
						}
						nfunk += ndim;

						//start psum
						for(j=0; j<ndim;j++) {
							for(sum=0.0, i=0;i<mpts;i++) sum+=p[i][j];
							psum[j]=sum;
						}
						//endpsum
					}
				} else (--nfunk);
			}
			delete1D(psum);
		}

		/* Extrapolates by a factor fac through the face of the simplex across
		 * from the high point, tries it, and replaces the high point if the
		 * new one is better.
		 */
		double amotry(double *psum, int ihi, double fac) {
			int j;
			double fac1, fac2, ytry, *ptry;

			ptry = new1D(ndim, 0.0);
			fac1 = (1.0-fac)/ndim;
			fac2 = fac1-fac;
			for(j=0;j<ndim;j++) ptry[j] = psum[j]*fac1-p[ihi][j]*fac2;
			//Evaluate the function at the trial point.
			//If it's better than the highest, then replace the highest.
			ytry=function->getValue(ptry);
			if(ytry<y[ihi]) {
				y[ihi]=ytry;
				for(j=0;j<ndim;j++) {
					psum[j] += ptry[j] - p[ihi][j];
					if(startingSpread[j] != 0.0) //to prevent floating point error buildup
						p[ihi][j] = ptry[j];
				}
			}
			delete1D(ptry);


			return ytry;
		}







};



// describes the distance between modeled and actual data, for an eye calibration.
class EyeDeltaFunction : public MinimizerFunction {
public:
	double** coords; // [points][dims] coordinates corresponding to data.
	double** data; // [points][dims] actual data
	double** sigma; //[points][dims] error of data
	double** modeled; // [points][dims] modeled data, filled by getDistance
 //	double* params;

 //	int nParams;
	int nPoints, nDims;


	EyeDeltaFunction(double **coords, double **data, double **sigma, int nPoints, int nDims, double* params,
		int nParams) : MinimizerFunction(params, nParams) {
		this->coords = coords; //ncoords not passed. Known to be 3.
		this->data = data;
		this->sigma = sigma;
		this->nPoints = nPoints;
		this->nDims = nDims;  //goes with data. Known to be 2.

		modeled = new2D(nPoints, nDims, 0.0);

	}

	~EyeDeltaFunction() {
		delete2D(modeled, nPoints);
	}


	// given params and monitor coordinates, fills eye camera modeledPoint
	void fromMonitorToCamera(double *params, double *coords,
		double *modeledPoint) {
		double R = params[0];  //eye radius, mm
		double x0 = params[1]; //monitor offset X (towards eye), mm
		double y0 = params[2]; //monitor offset Y (horizontal), mm
		double z0 = params[3]; //monitor offset Z (vertical), mm
		double a = params[4]; //yaw, radians
		double b = params[5]; //pitch, radians
		double g = params[6]; //roll, radians
		double dc = params[7]; //distance from eye center to camera, mm
		double by = params[8]; //camera offset, horizontal, mm
		double bz = params[9]; //camera offset, vertical, mm
		double fc = params[10]; //focal length of camera, camera pixels

		double x = coords[0];
		double y = coords[1];
		double z = coords[2];



		double sa = sin(a);
		double sb = sin(b);
		double sg = sin(g);
		double ca = cos(a);
		double cb = cos(b);
		double cg = cos(g);

		//rotation matrix
		double r00 = ca*cb;
		double r01 =  ca*sb*sg - sa*cg;
		double r02 =  ca*sb*cg + sa*sg;
		double r10 =  sa*cb;
		double r11 = sa*sb*sg + ca*cg;
		double r12 = sa*sb*cg - ca*sg;
		double r20 = -sb;
		double r21 = cb*sg;
		double r22 = cb*cg;


	   //	equivalent to below
	   /*	double x1 = x-x0;
		double y1 = y-y0;
		double z1 = z-z0;

		double d = sqrt(x1*x1+y1*y1+z1*z1);
		double x2 = R*x1/d;
		double y2 = R*y1/d;
		double z2 = R*z1/d;

		double x3 = r00*x2 + r01*y2 + r02*z2;
		double y3 = r10*x2 + r11*y2 + r12*z2;
		double z3 = r20*x2 + r21*y2 + r22*z2;

		double x4 = fc;
		double y4 = (y3-by)*fc/(dc-x3);
		double z4 = (z3-bz)*fc/(dc-x3);

		modeledPoint[0] = y4;
		modeledPoint[1] = z4;
         */


		double d =  sqrt((x-x0)*(x-x0) + (y - y0)*(y-y0) + (z - z0)*(z - z0));
		double yOut = -(fc*(by - (R*(r11)*(y - y0))/d + (R*(- r12)*(z - z0))/d -
		 (R*(x-x0)*r10)/d))/(dc + (R*(- r01)*(y - y0))/d -
		  (R*(r02)*(z - z0))/d - (R*(x-x0)*r00)/d);

		double zOut = -(fc*(bz + (R*(x-x0)*(-r20))/d - (R*r22*(z - z0))/d -
		 (R*r21*(y - y0))/d))/(dc + (R*(- r01)*(y - y0))/d -
		  (R*(r02)*(z - z0))/d - (R*(x-x0)*r00)/d);


		modeledPoint[0] = yOut;
		modeledPoint[1] = zOut;


	}

	int getNDims() {
		return nDims;
	}

	//calculates distance between modeled data and measured data
	double getValue(double *params) {
		double tot = 0.0;
		for (int point = 0; point < nPoints; point++) {
			fromMonitorToCamera(params, coords[point], modeled[point]);
			for (int dim = 0; dim < nDims; dim++) {
				tot += (modeled[point][dim] - data[point][dim]) *
					(modeled[point][dim] - data[point][dim])/sigma[point][dim]/sigma[point][dim];
			}
		}
		return sqrt(tot) / nPoints;
	}

};



// describes the distance between modeled and actual data, for a scene calibration.
class SceneDeltaFunction : public MinimizerFunction {
public:
	double** coords; // [points][dims] coordinates corresponding to data.
	double** data; // [points][dims] actual data
	double** sigma; //[points][dims] error of data
	double** modeled; // [points][dims] modeled data, filled by getDistance
 //	double* params;

 //	int nParams;
	int nPoints, nDims;


	SceneDeltaFunction(double **coords, double **data, double **sigma, int nPoints, int nDims, double* params,
		int nParams) : MinimizerFunction(params, nParams) {
		this->coords = coords;
		this->data = data;
		this->sigma = sigma;
		this->nPoints = nPoints;
		this->nDims = nDims;

		modeled = new2D(nPoints, nDims, 0.0);

	}

	~SceneDeltaFunction() {
		delete2D(modeled, nPoints);
	}


	// given params and monitor coordinates, fills scene camera modeledPoint
	void fromMonitorToCamera(double *params, double *coords,
		double *modeledPoint) {

		double a = params[0]; //yaw, radians
		double b = params[1]; //pitch, radians
		double g = params[2]; //roll, radians
		double bx = params[3]; //distance from camera to target, mm
		double by = params[4]; //camera offset, horizontal, mm
		double bz = params[5]; //camera offset, vertical, mm
		double fc = params[6]; //focal length of camera, camera pixels
		double K = params[7]; //radial^2 distortion
		double yc = params[8];//distortion center, y
		double zc = params[9];//distortion center, z

		double sa = sin(a);
		double sb = sin(b);
		double sg = sin(g);
		double ca = cos(a);
		double cb = cos(b);
		double cg = cos(g);

		//rotation matrix
		double r00 = ca*cb;
		double r01 =  ca*sb*sg - sa*cg;
		double r02 =  ca*sb*cg + sa*sg;
		double r10 =  sa*cb;
		double r11 = sa*sb*sg + ca*cg;
		double r12 = sa*sb*cg - ca*sg;
		double r20 = -sb;
		double r21 = cb*sg;
		double r22 = cb*cg;



		double x = 0;
		double y = coords[0];
		double z = coords[1];

		double tx = r00*x + r01*y + r02*z;
		double ty = r10*x + r11*y + r12*z;
		double tz = r20*x + r21*y + r22*z;
		x = tx;
		y = ty;
		z = tz;

		y = (y-by)*fc/(x-bx);
		z = (z-bz)*fc/(x-bx);
		x = fc;

		//radial distortion
		//K rd^3 + rd = ru
		//rd: distorted radius
		//ru: undistorted radius
		//inverse calculation.  Not currently needed.
		if(false) {
		//if(K!=0) {
			double ru = sqrt((y-yc)*(y-yc)+(z-zc)*(z-zc));
			double rd = 0.0;
			double a = K;
			double c = 1.0;
			double d = -ru;

			double det = -4*a*c*c*c - 27*a*a*d*d;
		   //	printf("det: %g\n", det);
			if(det < 0) { //only one real root http://en.wikipedia.org/wiki/Cubic_function
				double q = sqrt(27*27*a*a*a*a*d*d+4*3*3*3*a*a*a*c*c*c);

				double C = pow(fabs(.5*(q+27*a*a*d)) , 1.0/3.0);
				C = ( q+27*a*a*d < 0) ? -C : C;
				rd = -C/(3*a)+c/C;


			} else {   //three real roots    http://mathworld.wolfram.com/CubicFormula.html

				double a2 = 0;
				double a1 = c/a;
				double a0 = d/a;
				double Q = (3.0*a1-a2*a2)/9.0;
				double R = (9.0*a2*a1-27*a0-2*a2*a2*a2)/54.0;
				double theta = acos(R/sqrt(-Q*Q*Q));
				double rd1 = 2*sqrt(-Q)*cos(theta/3.0)-a2/3.0;
				double rd2 = 2*sqrt(-Q)*cos((theta+2*M_PI)/3.0)-a2/3.0;
				double rd3 = 2*sqrt(-Q)*cos((theta+4*M_PI)/3.0)-a2/3.0;
				//use the root with the smallest change.
				double d1 = fabs(rd1-ru);
				double d2 = fabs(rd2-ru);
				double d3 = fabs(rd3-ru);
				if(d1 < d2 && d1 < d3) rd = rd1;
				else if (d2 < d1 && d2 < d3) rd = rd2;
				else rd = rd3;
			}

			//convert from radial to cartisian
			y = rd * (y-yc) / ru + yc;
			z = rd * (z-zc) / ru + zc;

		   // This is the inverse of the distortion
		   //http://scitation.aip.org/getpdf/servlet/GetPDFServlet?filetype=pdf&id=PSISDG007266000001726611000001&idtype=cvips&doi=10.1117/12.804771&prog=normal
		   //	double d2 = (y-yc)*(y-yc) + (z-zc)*(z-zc);
		   //	ty = y+(y-yc)*K*d2;
		   //	tz = z+(z-zc)*K*d2;
		}

		//radial distortion
		double d2 = (y-yc)*(y-yc) + (z-zc)*(z-zc);
		y = y+(y-yc)*K*d2;
		z = z+(z-zc)*K*d2;

		modeledPoint[0] = y;
		modeledPoint[1] = z;
	}

	int getNDims() {
		return nDims;
	}

	//calculates distance between modeled data and measured data
	double getValue(double *params) {
		double tot = 0.0;
		for (int point = 0; point < nPoints; point++) {
			fromMonitorToCamera(params, coords[point], modeled[point]);
			for (int dim = 0; dim < nDims; dim++) {
				tot += (modeled[point][dim] - data[point][dim]) *
					(modeled[point][dim] - data[point][dim])/sigma[point][dim]/sigma[point][dim];
			}
		}
		return sqrt(tot) / nPoints;
	}

};



// describes the distance between modeled and measured rigid position data.
class RigidDeltaFunction : public MinimizerFunction {
public:
	double** orig; // [points][dims] original head position data (should be centered to 0,0,0.
	double** current; // [points][dims] current head position data
	double** sigma; //[points][dims] error of head position data
	double** modeled; // [points][dims] modeled current position data, filled by getDistance

	int nPoints, nDims;


	RigidDeltaFunction(double **orig, double **current, double **sigma, int nPoints, int nDims, double* params,
		int nParams) : MinimizerFunction(params, nParams) {
		this->orig = orig;
		this->current = current;
		this->sigma = sigma;
		this->nPoints = nPoints;
		this->nDims = nDims;

		modeled = new2D(nPoints, nDims, 0.0);
	}

	~RigidDeltaFunction() {
		delete2D(modeled, nPoints);
	}

	// given params and monitor coordinates, fills scene camera modeledPoint
	void fromOrigToCurrent(double *params, double *orig,
		double *modeledPoint) {

		double a = params[0]; //yaw, radians
		double b = params[1]; //pitch, radians
		double g = params[2]; //roll, radians
		double x1 = params[3]; //x offset, mm
		double y1 = params[4]; //y offset, mm
		double z1 = params[5]; //z offset, mm

		double sa = sin(a);
		double sb = sin(b);
		double sg = sin(g);
		double ca = cos(a);
		double cb = cos(b);
		double cg = cos(g);

		//rotation matrix
		double r00 = ca*cb;
		double r01 =  ca*sb*sg - sa*cg;
		double r02 =  ca*sb*cg + sa*sg;
		double r10 =  sa*cb;
		double r11 = sa*sb*sg + ca*cg;
		double r12 = sa*sb*cg - ca*sg;
		double r20 = -sb;
		double r21 = cb*sg;
		double r22 = cb*cg;

		double x = orig[0];
		double y = orig[1];
		double z = orig[2];

		double tx = r00*x + r01*y + r02*z;
		double ty = r10*x + r11*y + r12*z;
		double tz = r20*x + r21*y + r22*z;
		x = tx + x1;
		y = ty + y1;
		z = tz + z1;

		modeledPoint[0] = x;
		modeledPoint[1] = y;
		modeledPoint[2] = z;
	}

	int getNDims() {
		return nDims;
	}

	//calculates distance between modeled data and measured data
	double getValue(double *params) {
		double tot = 0.0;
		for (int point = 0; point < nPoints; point++) {
			fromOrigToCurrent(params, orig[point], modeled[point]);
			for (int dim = 0; dim < nDims; dim++) {
				tot += (modeled[point][dim] - current[point][dim]) *
					(modeled[point][dim] - current[point][dim])/sigma[point][dim]/sigma[point][dim];
			}
		}
		return sqrt(tot) / nPoints;
	}

};



