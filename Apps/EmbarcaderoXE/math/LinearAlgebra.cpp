//General linear algebra functions.
//Written by Matthew Grivich, Swartz Center for Computational Neuroscience, UCSD

//---------------------------------------------------------------------------


#pragma hdrstop

#include "LinearAlgebra.h"
#include "math_util.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)


void print(char *name, ublas::matrix<double> a) {
	printf("%s:\n", name);
	for(unsigned row=0; row<a.size1(); ++row) {
		for(unsigned col=0; col<a.size2(); ++col) {
			printf("%g\t", a(row,col));
		}
		printf("\n");
	}
	printf("\n");
}


void print(char *name, ublas::vector<double> v) {
	printf("%s:\n", name);
	for(unsigned i=0; i<v.size(); ++i) {
		printf("%g\t", v(i));
	}
	printf("\n\n");
}


double determinate(ublas::matrix<double> m) {
	if(m.size1() != m.size2()) throw IllegalArgumentException("Determinate: Matrix must be square.\n");

	if(m.size1() == 1) return m(0,0);
	else if(m.size1() == 2) return m(0,0)*m(1,1) - m(0,1)*m(1,0);
	else if(m.size1() == 3) return (m(0,0)*(m(1,1)*m(2,2) - m(1,2)*m(2,1)) -
								m(0,1)*(m(1,0)*m(2,2) - m(1,2)*m(2,0)) +
								m(0,2)*(m(1,0)*m(2,1) - m(1,1)*m(2,0)));
	else throw IllegalArgumentException("Determinate: Sizes greater than 3x3 not implemented.\n");
	//see lapack.cpp from opencv for an open algorithm for matrices larger than 3x3.

	return NAN;
}


bool hasNan(ublas::matrix<double> a) {
	for(unsigned int i=0; i<a.size1(); i++) {
		for(unsigned int j=0; j<a.size2(); j++) {
			if(_isnan(a(i,j))) return true;
		}
	}

	return false;

}


bool hasNan(ublas::vector<double> v) {
	for(unsigned int i=0; i<v.size(); i++) {
		if(_isnan(v(i))) return true;
	}
	return false;
}

ublas::vector<double> cross(ublas::vector<double> a, ublas::vector<double> b) {
	if(a.size() != 3 || b.size() != 3) throw IllegalArgumentException("Cross products require vectors of length 3.\n");

	ublas::vector<double> out(3);
	out(0) = a(1)*b(2) - a(2)*b(1);
	out(1) = a(2)*b(0) - a(0)*b(2);
	out(2) = a(0)*b(1) - a(1)*b(0);
	return out;
}

double length(ublas::vector<double> v) {
	double val = 0.0;

	for(unsigned int i=0; i<v.size(); i++) {
		val += v(i)*v(i);
	}
	return sqrt(val);
}

/**
 * Utility function for linearSolve.
 */
inline double pythag(double a, double b) {
	double absa = fabs(a);
	double absb = fabs(b);
	if(absa > absb) return absa*sqrt(1.0+absb*absb/absa/absa);
	else return(absb == 0.0 ? 0.0 : absb*sqrt(1.0 + absa*absa/absb/absb));
}

/**
 * Utility function for linearSolve.
 */
inline double max(double a, double b) {
	return (a > b) ? a : b;
}

/**
 * Utility function for linearSolve.
 */
inline double min(double a, double b) {
	return (a < b) ? a : b;
}

/**
 * Utility function for linearSolve.
 */
inline double sign(double a,double b) {
	return ((b) >= 0.0 ? fabs(a) : -fabs(a));
}

ublas::vector<double> linearSolve(ublas::matrix<double> A, ublas::vector<double> b) {
	//prealloc U, w, V)
	int m = A.size1();
	int n = A.size2();
	ublas::matrix<double> U(m,n);
	ublas::vector<double> w(n);
	ublas::matrix<double> V(n,n);
	svdcmp(A, U, w, V);

	int jj,j,i;
	double s;

	ublas::vector<double> temp(n);
	ublas::vector<double> x(n);

	for(j=0; j<n;j++) {
		s = 0.0;
		if(w[j]) {
			for(i=0; i<m; i++) s+= U(i,j)*b(i);
			if(w(j) == 0) throw CalculationFailedException("linearSolve: Divide by zero.\n");
			s/= w(j);

		}
		temp[j] = s;
	}

	for(j=0; j<n; j++) {
		s = 0.0;
		for(jj=0; jj<n; jj++) s+= V(j,jj)*temp(jj);
		x(j) = s;
	}

	return x;
}


void svdcmp(ublas::matrix<double> a, ublas::matrix<double> &u, ublas::vector<double> &w, ublas::matrix<double> &v)
{

 //	if(hasNan(a)) throw IllegalArgumentException("svdcmp: Input matrix has NANs.\n");

	int m = a.size1();
	int n = a.size2();
	if((int) u.size1() != m || (int) u.size2() != n) throw IllegalArgumentException("svdcmp: U has incorrect dimensions.");
	if((int) w.size() != n) throw IllegalArgumentException("svdcmp: W has incorrect length.");
	if((int) v.size1() != n || (int) v.size2() != n) throw IllegalArgumentException("svdcmp: V has incorrect dimensions.");
	int flag,i,its,j,jj,k,l,nm;
	double anorm,c,f,g,h,s,scale,x,y,z;

	ublas::vector<double> rv1 (n);
	rv1*=0;

	g=scale=anorm=0.0; /* Householder reduction to bidiagonal form */
	for (i=0;i<n;i++) {
		l=i+1;
		rv1(i)=scale*g;
		g=s=scale=0.0;
		if (i < m) {
			for (k=i;k<m;k++) scale += fabs(a(k,i));
			if (scale) {
				for (k=i;k<m;k++) {
					a(k,i) /= scale;
					s += a(k,i)*a(k,i);
				}
				f=a(i,i);
				g = -sign(sqrt(s),f);
				h=f*g-s;
				a(i,i)=f-g;
				for (j=l;j<n;j++) {
					for (s=0.0,k=i;k<m;k++) s += a(k,i)*a(k,j);
					f=s/h;
					for (k=i;k<m;k++) a(k,j) += f*a(k,i);
				}
				for (k=i;k<m;k++) a(k,i) *= scale;
			}
		}
		w(i)=scale *g;
		g=s=scale=0.0;
		if (i < m && i != n) {
			for (k=l;k<n;k++) scale += fabs(a(i,k));
			if (scale) {
				for (k=l;k<n;k++) {
					a(i,k) /= scale;
					s += a(i,k)*a(i,k);
				}
				f=a(i,l);
				g = -sign(sqrt(s),f);
				h=f*g-s;
				a(i,l)=f-g;
				for (k=l;k<n;k++) rv1(k)=a(i,k)/h;
				for (j=l;j<m;j++) {
					for (s=0.0,k=l;k<n;k++) s += a(j,k)*a(i,k);
					for (k=l;k<n;k++) a(j,k) += s*rv1(k);
				}
				for (k=l;k<n;k++) a(i,k) *= scale;
			}
		}
		anorm = max(anorm,(fabs(w(i))+fabs(rv1(i))));
	}
	for (i=n-1;i>=0;i--) { /* Accumulation of right-hand transformations. */
		if (i < n) {
			if (g) {
				for (j=l;j<n;j++) /* Double division to avoid possible underflow. */
					v(j,i)=(a(i,j)/a(i,l))/g;
				for (j=l;j<n;j++) {
					for (s=0.0,k=l;k<n;k++) s += a(i,k)*v(k,j);
					for (k=l;k<n;k++) v(k,j) += s*v(k,i);
				}
			}
			for (j=l;j<n;j++) v(i,j)=v(j,i)=0.0;
		}
		v(i,i)=1.0;
		g=rv1(i);
		l=i;
	}
	for (i=min(m,n)-1;i>=0;i--) { /* Accumulation of left-hand transformations. */
		l=i+1;
		g=w[i];
		for (j=l;j<n;j++) a(i,j)=0.0;
		if (g) {
			g=1.0/g;
			for (j=l;j<n;j++) {
				for (s=0.0,k=l;k<m;k++) s += a(k,i)*a(k,j);
				f=(s/a(i,i))*g;
				for (k=i;k<m;k++) a(k,j) += f*a(k,i);
			}
			for (j=i;j<m;j++) a(j,i) *= g;
		} else for (j=i;j<m;j++) a(j,i)=0.0;
		++a(i,i);
	}
	for (k=n-1;k>=0;k--) { /* Diagonalization of the bidiagonal form. */
		for (its=1;its<=30;its++) {
			flag=1;
			for (l=k;l>=0;l--) { /* Test for splitting. */
				nm=l-1; /* Note that rv1[1] is always zero. */
				if ((double)(fabs(rv1(l))+anorm) == anorm) {
					flag=0;
					break;
				}
				if ((double)(fabs(w(nm))+anorm) == anorm) break;
			}
			if (flag) {
				c=0.0; /* Cancellation of rv1[l], if l > 1. */
				s=1.0;
				for (i=l;i<=k;i++) {
					f=s*rv1(i);
					rv1(i)=c*rv1(i);
					if ((double)(fabs(f)+anorm) == anorm) break;
					g=w(i);
					h=pythag(f,g);
					w(i)=h;
					h=1.0/h;
					c=g*h;
					s = -f*h;
					for (j=0;j<m;j++) {
						y=a(j,nm);
						z=a(j,i);
						a(j,nm)=y*c+z*s;
						a(j,i)=z*c-y*s;
					}
				}
			}
			z=w(k);
			if (l == k) { /* Convergence. */
				if (z < 0.0) { /* Singular value is made nonnegative. */
					w(k) = -z;
					for (j=0;j<n;j++) v(j,k) = -v(j,k);
				}
				break;
			}
			if (its == 30) throw FitFailedException("No convergence in 30 svdcmp iterations.");
			x=w(l); /* Shift from bottom 2-by-2 minor. */
			nm=k-1;
			y=w(nm);
			g=rv1(nm);
			h=rv1(k);
			f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
			g=pythag(f,1.0);
			f=((x-z)*(x+z)+h*((y/(f+sign(g,f)))-h))/x;
			c=s=1.0; /* Next QR transformation: */
			for (j=l;j<=nm;j++) {
				i=j+1;
				g=rv1(i);
				y=w(i);
				h=s*g;
				g=c*g;
				z=pythag(f,h);
				rv1(j)=z;
				c=f/z;
				s=h/z;
				f=x*c+g*s;
				g = g*c-x*s;
				h=y*s;
				y *= c;
				for (jj=0;jj<n;jj++) {
					x=v(jj,j);
					z=v(jj,i);
					v(jj,j)=x*c+z*s;
					v(jj,i)=z*c-x*s;
				}
				z=pythag(f,h);
				w(j)=z; /* Rotation can be arbitrary if z = 0. */
				if (z) {
					z=1.0/z;
					c=f*z;
					s=h*z;
				}
				f=c*g+s*y;
				x=c*y-s*g;
				for (jj=0;jj<m;jj++) {
					y=a(jj,j);
					z=a(jj,i);
					a(jj,j)=y*c+z*s;
					a(jj,i)=z*c-y*s;
				}
			}
			rv1(l)=0.0;
			rv1(k)=f;
			w(k)=x;
		}
	}
	u = a;
}

ublas::matrix<double> removeColumn(ublas::matrix<double> inMatrix, int column) {

	unsigned int nRows = inMatrix.size1();
	unsigned int nColumns = inMatrix.size2();
	ublas::matrix<double> outMatrix(nRows, nColumns-1);

	unsigned int inCol = 0;
	for(unsigned int outCol=0; outCol<nColumns-1; outCol++) {
		if(column==outCol) inCol++;
		for(int row=0; row<nRows; row++) {
			outMatrix(row,outCol) = inMatrix(row,inCol);
		}
		inCol++;
	}
	return outMatrix;
}

 void findRigidBody(ublas::matrix<double> dataOrig, ublas::matrix<double> dataCurrent,
		ublas::vector<double> &translationOrig, ublas::matrix<double> &rotation, ublas::vector<double> &translationCurrent) {
	int nPoints = dataOrig.size2();
	int nDimensions = dataOrig.size1();
	if(nDimensions != 3) throw IllegalArgumentException("findRigidBody: nDimensions should be 3.\n");
	if(hasNan(dataOrig)) throw IllegalArgumentException("findRigidBody: dataOrig has NANs.\n");
 //   if(hasNan(dataCurrent)) throw IllegalArgumentException("findRigidBody: dataCurrent has NANs.\n");

	while(hasNan(dataCurrent)) {
		for(unsigned int dim=0; dim<nDimensions; dim++) {
			for(unsigned int point=0; point<nPoints; point++) {
				if(_isnan(dataCurrent(dim,point))) {

					dataCurrent = removeColumn(dataCurrent, point);
					dataOrig = removeColumn(dataOrig, point);
					nPoints--;
					goto end;
				}
			}
		}
		end:

	}

	if(nPoints < 3) {
		for(int dim=0; dim<nDimensions; dim++) {
			translationOrig(dim) = NAN;
			translationCurrent(dim) = NAN;
			for(int point=0; point<nPoints; point++) {
				rotation(dim,point) = NAN;
			}
		}

		return;
	}

	translationOrig(0) = 0;
	translationOrig(1) = 0;
	translationOrig(2) = 0;

	for(int dim=0; dim<nDimensions; ++dim) {
		// find center of mass for each dimension
		for(int point=0; point<nPoints; ++point) {
			translationOrig(dim) += dataOrig(dim,point);
		}
		translationOrig(dim)/=nPoints;
		//subtract off center of mass for all original points for a given dimension
		for(int point=0; point<nPoints; ++point) {
			dataOrig(dim,point) -= translationOrig(dim);
		}
	}

	translationCurrent(0) = 0;
	translationCurrent(1) = 0;
	translationCurrent(2) = 0;

	for(int dim=0; dim<nDimensions; ++dim) {
		// find center of mass for each dimension
		for(int point=0; point<nPoints; ++point) {
			translationCurrent(dim) += dataCurrent(dim,point);
		}
		translationCurrent(dim)/=nPoints;
		//subtract off center of mass for all current points for a given dimension
		for(int point=0; point<nPoints; ++point) {
			dataCurrent(dim,point) -= translationCurrent(dim);
		}
		//change translation to be with respect to original position rather than 0,0;
		//translationCurrent(dim) -= translationOrig(dim);
	}

	//find rotation
	ublas::matrix<double> cc = prod(dataCurrent, trans(dataOrig))/nPoints;
	ublas::matrix<double> u(nDimensions,nDimensions);
	ublas::vector<double> w(nDimensions);
	ublas::matrix<double> v(nDimensions,nDimensions);
	u*=0;
	w*=0;
	v*=0;

	svdcmp(cc, u, w, v);

	ublas::matrix<double> flip(nDimensions, nDimensions);
	flip*=0;
	flip(0,0) = 1;
	flip(1,1) = 1;
	flip(2,2) = determinate(prod(v,trans(u))); //== 1 or -1
	rotation = prod(u,flip);
	rotation = prod(rotation,trans(v));
}

ublas::matrix<double> moveRigidBody(ublas::matrix<double> orig, ublas::vector<double> priorTranslation,
	ublas::matrix<double> rotation, ublas::vector<double> postTranslation) {
	int nDimensions = orig.size1();
	int nPoints = orig.size2();
	ublas::matrix<double> moved(nDimensions, nPoints);
	for(int dim = 0; dim < nDimensions; ++dim) {
		for(int point = 0; point < nPoints; ++point) {
			moved(dim,point) = orig(dim,point) - priorTranslation(dim);
		}
	}
	moved = ublas::prod(rotation, moved);

	for(int dim = 0; dim < nDimensions; ++dim) {
		for(int point = 0; point < nPoints; ++point) {
			moved(dim,point) = moved(dim,point) + postTranslation(dim);
		}
	}

	return moved;

}

ublas::vector<double> moveRigidVector(ublas::vector<double> orig, ublas::vector<double> priorTranslation,
	ublas::matrix<double> rotation, ublas::vector<double> postTranslation) {
	int nDimensions = orig.size();
	ublas::vector<double> moved(nDimensions);
	for(int dim = 0; dim < nDimensions; ++dim) {
			moved(dim) = orig(dim) - priorTranslation(dim);
	}
	moved = ublas::prod(rotation, moved);

	for(int dim = 0; dim < nDimensions; ++dim) {
			moved(dim) = moved(dim) + postTranslation(dim);
	}

	return moved;

}


ublas::vector<double> findMidpoint(ublas::vector<double> p1, ublas::vector<double> p2,
		ublas::vector<double> p3, ublas::vector<double> p4) {
	if(p1(0) == p2(0) && p1(1) == p2(1) && p1(2) == p2(2)) throw IllegalArgumentException("findMidpoint: p1->p2 has zero length");
	if(p3(0) == p4(0) && p3(1) == p4(1) && p3(2) == p4(2)) throw IllegalArgumentException("findMidpoint: p3->p4 has zero length");

	ublas::vector<double> p13(3), p43(3), p21(3), pa(3), pb(3);
   double d1343,d4321,d1321,d4343,d2121, mua, mub;
   double numer,denom;

   p13(0) = p1(0) - p3(0);
   p13(1) = p1(1) - p3(1);
   p13(2) = p1(2) - p3(2);
   p43(0) = p4(0) - p3(0);
   p43(1) = p4(1) - p3(1);
   p43(2) = p4(2) - p3(2);

   p21(0) = p2(0) - p1(0);
   p21(1) = p2(1) - p1(1);
   p21(2) = p2(2) - p1(2);


   d1343 = p13(0) * p43(0) + p13(1) * p43(1) + p13(2) * p43(2);
   d4321 = p43(0) * p21(0) + p43(1) * p21(1) + p43(2) * p21(2);
   d1321 = p13(0) * p21(0) + p13(1) * p21(1) + p13(2) * p21(2);
   d4343 = p43(0) * p43(0) + p43(1) * p43(1) + p43(2) * p43(2);
   d2121 = p21(0) * p21(0) + p21(1) * p21(1) + p21(2) * p21(2);

   denom = d2121 * d4343 - d4321 * d4321;
   if (denom == 0) throw IllegalArgumentException("findMidpoint: Divide by zero.");;
   numer = d1343 * d4321 - d1321 * d4343;

   mua = numer / denom;

   if(d4343 == 0) throw IllegalArgumentException("findMidpoint: Divide by zero.");
   mub = (d1343 + d4321 * (mua)) / d4343;

   pa(0) = p1(0) + mua * p21(0);
   pa(1) = p1(1) + mua * p21(1);
   pa(2) = p1(2) + mua * p21(2);
   pb(0) = p3(0) + mub * p43(0);
   pb(1) = p3(1) + mub * p43(1);
   pb(2) = p3(2) + mub * p43(2);

   ublas::vector<double> result(3);
   result(0) = (pa(0) + pb(0))/2;
   result(1) = (pa(1) + pb(1))/2;
   result(2) = (pa(2) + pb(2))/2;


   return result;

}

/**
 * Utility method for Jacobi
 */

void rotate(ublas::matrix<double> &a, int i, int j, int k, int l, double tau, double s) {
	double g,h;
	g=a(i,j);
	h=a(k,l);
	a(i,j)=g-s*(h+g*tau);
	a(k,l)=h+s*(g-h*tau);
}


void jacobi(ublas::matrix<double> a, ublas::vector<double> &d , ublas::matrix<double> &v) {
	double termination = 1e-20;  // a bit above the machine precision noise floor
	int maxIterations = 50;
	int n = a.size1();
	if(n != (signed) a.size2()) throw IllegalArgumentException("jacobi: Matrix 'a' must be square.\n");
	if(hasNan(a)) throw IllegalArgumentException("jacobi: Matrix 'a' has NANs.\n");
	if(n != (signed) d.size()) throw IllegalArgumentException("jacobi: Vector 'd' must be the same length as matrix 'a'.\n");
	if(n != (signed) v.size1() || n != v.size2()) throw IllegalArgumentException("jacobi: Matrix 'v' must be the same size as matrix 'a'.\n");

	d*=0;
	v*=0;
	int nrot = 0;

	int j=0,iq=0,ip=0,i=0;
	double tresh=0.0,theta=0.0,tau=0.0,t=0.0,sm=0.0,s=0.0,h=0.0,g=0.0,c=0.0;

	ublas::vector<double> b(n);
	ublas::vector<double> z(n);

	for (ip=0;ip<n;ip++) {
		for (iq=0;iq<n;iq++) v(ip,iq)=0.0;
		v(ip,ip)=1.0;
	}
	for (ip=0;ip<n;ip++) {
		b(ip)=d(ip)=a(ip,ip);
		z(ip)=0.0;
	}


	for (i=0;i<maxIterations;i++) {
		sm=0.0;
		for (ip=0;ip<n-1;ip++) {
			for (iq=ip+1;iq<n;iq++) sm += fabs(a(ip,iq));
		}
		//if done, terminate
		if (sm < termination) {
			return;
		}
		if (i < 3) tresh=0.2*sm/(n*n);
		else tresh=0.0;


		for (ip=0;ip<n-1;ip++) {
			for (iq=ip+1;iq<n;iq++) {

				g=100.0*fabs(a(ip,iq));

				if (i > 3 && fabs(d(ip))+g == fabs(d(ip))
					&& fabs(d(iq))+g == fabs(d(iq)))
						a(ip,iq)=0.0;
				else if (fabs(a(ip,iq)) > tresh) {
					h=d(iq)-d(ip);
					if (fabs(h)+g == fabs(h)) t=(a(ip,iq))/h;
					else {
						theta=0.5*h/(a(ip,iq));
						t=1.0/(fabs(theta)+sqrt(1.0+theta*theta));
						if (theta < 0.0) t = -t;
					}


					c=1.0/sqrt(1+t*t);
					s=t*c;
					tau=s/(1.0+c);
					h=t*a(ip,iq);
					z[ip] -= h;
					z[iq] += h;
					d[ip] -= h;
					d[iq] += h;
					a(ip,iq)=0.0;

					for (j=0;j<=ip-1;j++) {
						rotate(a,j,ip,j,iq,tau,s);
					}

					for (j=ip+1;j<=iq-1;j++) {
						rotate(a,ip,j,j,iq,tau,s);
					}

					for (j=iq+1;j<n;j++) {
						rotate(a,ip,j,iq,j,tau,s);
					}

					for (j=0;j<n;j++) {
						rotate(v,j,ip,j,iq,tau,s);
					}

					++nrot;
				}
			}
		}

		for (ip=0;ip<n;ip++) {
			b(ip) += z(ip);
			d(ip)=b(ip);
			z(ip)=0.0;
		}
	}
	throw CalculationFailedException("jacobi: too many iterations.\n");
}

/*ublas::matrix<double>*/ void choldc(ublas::matrix<double> a, ublas::matrix<double> &l) {
	int n = a.size1();


	int i,j,k;
	double sum;
	ublas::vector<double> p(n);

  //	ublas::matrix<double> l(n,n);
	for (i=0; i<n; i++)  {
		for (j=i; j<n; j++)  {
			for (sum=a(i,j),k=i-1;k>=0;k--) sum -= a(i,k)*a(j,k);
			if (i == j) {
				if (sum<=0.0) {

					throw CalculationFailedException("choldc: 'a' is not poitive definite.\n");
				} else
					p(i)=sqrt(sum);
			} else {
				a(j,i)=sum/p(i);
			}
		}
	}
	for (i=0; i<n; i++)
		for (j=i; j<n; j++)
			if (i==j)
				l(i,i) = p(i);
			else {
				l(j,i)=a(j,i);
				l(i,j)=0.0;
			}

	//return l;
}


ublas::matrix<double> inverse(ublas::matrix<double> TB) {
	if(TB.size1() != TB.size2()) throw IllegalArgumentException("inverse: input matrix must be square");
	int N = TB.size1();
	ublas::matrix<double> InvB(TB.size1(), TB.size2());
	int k,i,j,p,q;
	double mult;
	double D,temp;
	double maxpivot;
	int npivot;
	ublas::matrix<double> B(N,N+1);
	ublas::matrix<double> A(N,2*N+1);
	ublas::matrix<double> C(N,N);

	double eps = 1e-20;

	for(k=0;k<N;k++)
		for(j=0;j<N;j++)
			B(k,j)=TB(k,j);

	for (k=0;k<N;k++) {
		for (j=0;j<N+1;j++)
			A(k,j)=B(k,j);
		for (j=N+1;j<2*N+1;j++)
			A(k,j)= 0.0;
		A(k,k-1+N+2)=1.0;
	}
	for (k=0;k<N;k++) {
		maxpivot=fabs(A(k,k));
		npivot=k;
		for (i=k;i<N;i++)
			if (maxpivot<fabs(A(i,k))) {
				maxpivot=fabs(A(i,k));
				npivot=i;
			}
		if (maxpivot>=eps) {
			if (npivot!=k)
				for (j=k;j<2*N+1;j++) {
					 temp=A(npivot,j);
					 A(npivot,j)=A(k,j);
					 A(k,j)=temp;
				}
			D=A(k,k);
			for (j=2*N;j>=k;j--)
				A(k,j)=A(k,j)/D;
			for (i=0;i<N;i++) {
				if (i!=k) {
					mult=A(i,k);
					for (j=2*N;j>=k;j--)
						A(i,j)=A(i,j)-mult*A(k,j) ;
				}
			}
		} else {
			throw CalculationFailedException("inverse: The matrix is singular\n");
		}
	}

	for (k=0,p=0;k<N;k++,p++)
		for (j=N+1,q=0;j<2*N+1;j++,q++)
			InvB(p,q)=A(k,j);
	return InvB;
}

void FitCircle(ublas::matrix<double> data, int nPoints, double *x0, double *y0, double *radius) {
	double meanX=0.0, meanY=0.0;
	*x0 = NAN;
	*y0 = NAN;
	*radius = NAN;
	for (int i=0; i<nPoints; i++) {
		meanX += data(0, i);
		meanY += data(1, i);
	}
	meanX /= nPoints;
	meanY /= nPoints;
	ublas::vector<double> u(nPoints);
	ublas::vector<double> v(nPoints);
	for(int i=0; i<nPoints; i++) {
		u(i) = data(0,i) - meanX;
		v(i) = data(1,i) - meanY;
	}
 /*
	printf("meanX: %g\n", meanX);
	printf("meanY: %g\n", meanY);
   */
	double Suu = 0.0, Suv = 0.0, Svv = 0.0, Suuu = 0.0, Suvv = 0.0, Svvv = 0.0, Svuu = 0.0;

	for(int i=0; i<nPoints; i++) {
		Suu +=  u(i)*u(i);
		Suv += u(i)*v(i);
		Svv += v(i)*v(i);
		Suuu += u(i)*u(i)*u(i);
		Suvv += u(i)*v(i)*v(i);
		Svvv += v(i)*v(i)*v(i);
		Svuu += v(i)*u(i)*u(i);

	}

  /*	printf("Suu: %g\n", Suu);
	printf("Suv: %g\n", Suv);
	printf("Svv: %g\n", Svv);
	printf("Suuu: %g\n", Suuu);
	printf("Suvv: %g\n", Suvv);
	printf("Svvv: %g\n", Svvv);
	printf("Svuu: %g\n", Svuu);
	*/
	ublas::matrix<double> mat(2,2);
	mat(0,0) = Suu;
	mat(0,1) = Suv;
	mat(1,0) = Suv;
	mat(1,1) = Svv;

	ublas::vector<double> vec(2);
	vec(0) =  (Suuu +Suvv)/2.0;
	vec(1) = (Svvv + Svuu)/2.0;
	try {
	ublas::vector<double> uc_vc_Solved = linearSolve(mat, vec);

		*x0 = uc_vc_Solved(0) +meanX;
		*y0 = uc_vc_Solved(1) + meanY;
		*radius = sqrt(uc_vc_Solved(0)*uc_vc_Solved(0) + uc_vc_Solved(1)*uc_vc_Solved(1) +(Suu +Svv)/nPoints);
	} catch(exception &ex) {
		throw;
	}
}


void FitEllipse(ublas::matrix<double> data,int nPoints,	double *x0, double *y0, double *rA, double *rB, double *angle) {

   //	if(x.size() != y.size()) throw IllegalArgumentException("FitEllipse: x and y must be the same length\n");

	ublas::matrix<double> D(nPoints,6);

	for(int i=0; i<nPoints; i++) {
		D(i,0) = data(0,i)*data(0,i);
		D(i,1) = data(0,i)*data(1,i);
		D(i,2) = data(1,i)*data(1,i);
		D(i,3) = data(0,i);
		D(i,4) = data(1,i);
		D(i,5) = 1;
	}

	//print("Design", D);

	ublas::matrix<double> Const(6,6);

	Const*=0;
	Const(0,2) = Const(2,0) = -2;
	Const(1,1) = 1.0;

	//print("Const", Const);

	ublas::matrix<double> S = ublas::prod(ublas::trans(D),D);

   //	print("Scatter", S);
	ublas::matrix<double> L(6,6);

	try {
		/*ublas::matrix<double> L = */ choldc(S,L);

		//print("Cholesky", L);

		ublas::matrix<double> invL = inverse(L);
		//print("inverse", invL);

		ublas::matrix<double> C = ublas::prod(Const, ublas::trans(invL));
		C = ublas::prod(invL,C);
		//print("C", C);
		ublas::vector<double> d(6);
		ublas::matrix<double> V(6,6);
		d*=0;
		V*=0;


		jacobi(C, d , V);

		//print("V", V);
		//print("d", d);

		ublas::matrix<double> sol = ublas::prod(ublas::trans(invL), V);

		//print("The GEV solution unnormalized", sol);



		// Now normalize them
		for (int j=0;j<6;j++)  {
			double mod = 0.0;
			for (int i=0;i<6;i++) mod += sol(i,j)*sol(i,j);
			for (int i=0;i<6;i++) sol(i,j) /=  sqrt(mod);
		}

		//print("The GEV solution", sol);

		double minev=INF;
		int  solind=-1;

		for (int i=0; i<6; i++)
			if (d(i)<minev ) {
				solind = i;
				minev = d(i);
			}

		ublas::vector<double> solvect(6);
		for (int j=0;j<6;j++)
			solvect(j) = sol(j,solind);

		//print("the solution", solvect);

		//transform from eigenvector solution to geometric parameters.

		*angle = 0.5*atan(solvect(1)/(solvect(0) - solvect(2)));
		double cost = cos(*angle);
		double sint = sin(*angle);
		double sin_squared = sint*sint;
		double cos_squared = cost*cost;
		double cos_sin = sint*cost;

		double Ao = solvect(5);
		double Au =   solvect(3) * cost + solvect(4)*sint;
		double Av = - solvect(3)*sint + solvect(4)*cost;
		double Auu = solvect(0)*cos_squared + solvect(2)*sin_squared + solvect(1)*cos_sin;
		double Avv = solvect(0)*sin_squared + solvect(2)*cos_squared - solvect(1)*cos_sin;


		double tuCentre = - Au/(2*Auu);
		double tvCentre = - Av/(2*Avv);
		double wCentre = Ao - Auu*tuCentre*tuCentre - Avv*tvCentre*tvCentre;

		double uCentre = tuCentre*cost - tvCentre*sint;
		double vCentre = tuCentre*sint + tvCentre*cost;

		double Ru = -wCentre/Auu;
		double Rv = -wCentre/Avv;

		*x0 = uCentre;
		*y0 = vCentre;
		Ru = sqrt(fabs(Ru));//(Ru <= 0.0) ? 0.0 : sqrt(Ru);
		Rv = sqrt(fabs(Rv));//(Rv <= 0.0) ? 0.0 : sqrt (Rv);

	  //	if(Ru > Rv) {
			*rA = Ru;
			*rB = Rv;
	  //	} else {
	  //		*rB = Ru;
	  //		*rA = Rv;
	  //	}
	  /*	printf("x0: %g\n", *x0);
		printf("y0: %g\n", *y0);
		printf("rA: %g\n", *rA);
		printf("rB: %g\n", *rB);
		printf("angle: %g\n", *angle);
		*/
	} catch(exception &ex) {
//			printf("exception: %s\n", ex.what());

		throw;
	}

}
