 /*

 Linear Algebra library.

 Written by Matthew Grivich, Swartz Center for Computational Neuroscience, UCSD

 Mostly ported from other sources. See below for details.
 */
//---------------------------------------------------------------------------

#ifndef LinearAlgebraH
#define LinearAlgebraH


#include "IllegalArgumentException.h"
#include "CalculationFailedException.h"
#include "FitFailedException.h"
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>


//shorten namespace
namespace ublas = boost::numeric::ublas;

/**
 * Print the contents of a matrix to standard output.
 */
void print(char *name, ublas::matrix<double> a);

/**
 * Print the contents of a vector to standard output.
 */
void print(char * name, ublas::vector<double> v);

/**
 * Calculate the determinant of a matrix. Sizes larger than 3x3 not implemented.
 */
double determinate(ublas::matrix<double> a);

/**
 * Check to see if a matrix contains a NAN.
 */
bool hasNan(ublas::matrix<double> a);

/**
 * Check to see if a vector contains a NAN.
 */
bool hasNan(ublas::vector<double> v);

/**
 * Returns the length of a vector.
 */
double length(ublas::vector<double> v);

ublas::vector<double> cross(ublas::vector<double> a, ublas::vector<double> b);

double norm(ublas::vector<double> v);

/*
 * solves A(m,n).x(n)=b(m).  Borrows heavily from svbksb, numerical rescipies, section 2.6
 */

ublas::vector<double> linearSolve(ublas::matrix<double> A, ublas::vector<double> b);

/*
Given a matrix a[1..m][1..n], this routine computes its singular value
decomposition, A = U.W.VT.  U has the same dimensions as A.  The diagonal
matrix of singular values W is output as a vector w[1..n].  The matrix V (not
the transpose VT) is output as v[1..n][1..n].  Caller must initialize all matrices.
*/
void svdcmp(ublas::matrix<double> a, ublas::matrix<double> &u, ublas::vector<double> &w, ublas::matrix<double> &v);

/**
 * Find the transformation to get rigid body from dataOrig to dataCurrent.
 * Use moveRigidBody to move something from the old coordinates to the new coordinates.
 *
 * translationOrig is the translation from 0,0,0 to the original center of mass
 * rotation is the rotation matrix around the center of mass
 * translationCurrent is the translation from 0,0,0 to the current center of mass
 *
 * See
 * http://www.kwon3d.com/theory/jkinem/rotmat.html
 * http://www.mathworks.com/matlabcentral/answers/3862-fit-rigid-body-to-points
 *Challis, J.H. (1995). A procedure for determining rigid body transformation parameters. J. Biomechanics 28, 733-737.
 */
 void findRigidBody(ublas::matrix<double> dataOrig, ublas::matrix<double> dataCurrent,
	ublas::vector<double> &translationOrig, ublas::matrix<double> &rotation, ublas::vector<double> &translationCurrent);

/**
 * Moves a rigid body to a new location. First, the priorTranslation is subtracted.
 * Then the rotation is done, then the postTranslation is added.
 *
 */
ublas::matrix<double> moveRigidBody(ublas::matrix<double> orig, ublas::vector<double> priorTranslation,
	ublas::matrix<double> rotation, ublas::vector<double> postTranslation);


/**
 * Moves a rigid vector to a new location. First, the priorTranslation is subtracted.
 * Then the rotation is done, then the postTranslation is added.
 *
 */
ublas::vector<double> moveRigidVector(ublas::vector<double> orig, ublas::vector<double> priorTranslation,
	ublas::matrix<double> rotation, ublas::vector<double> postTranslation);

//See http://paulbourke.net/geometry/lineline3d/
/*
   Calculate the line segment PaPb that is the shortest route between
   two lines P1->P2 and P3->P4. Calculate also the values of mua and mub where
	  Pa = P1 + mua (P2 - P1)
	  Pb = P3 + mub (P4 - P3)

   Returns (Pa + Pb)/2, that is, the best approximation for the intersection of the lines
   even if they are not intersecting.  Returns NAN if the lines are parallel.
*/
ublas::vector<double> findMidpoint(ublas::vector<double> p1, ublas::vector<double> p2,
	ublas::vector<double> p3, ublas::vector<double> p4);


/**
 * Jacobi transormation of a symmetrix matrix, after Numerical Recipies, 11.1
 * Computers eigenvalues d and eigenvectors v of symmetric matrix a.
 *
 */
void jacobi(ublas::matrix<double> a, ublas::vector<double> &d , ublas::matrix<double> &v);


/**
 * Perform the Cholesky decomposition
 * Return the lower triangular L  such that L*L'=A
 *
 * Ported from Numerical recipies section 2.9.
 *
 */
ublas::matrix<double> choldc(ublas::matrix<double> a);

/**
 * Invert a matrix
 * from http://homepages.inf.ed.ac.uk/rbf/CVonline/LOCAL_COPIES/PILU1/ElliFit.java.
 * Follows numerical recipies style, but does not appear to be in the book.
 * Numerical recipies version is in section 2.3.
*/
ublas::matrix<double> inverse(ublas::matrix<double> TB);

/**
 * Fit an ellipse, using the method of "Direct Least Square Fitting of Ellipses" Andrew Fitzgibbon,
 * Maurizio Pilu, Robert Fisher. Pattern Analysis and Machine Intelligence, 1999.
 * Based on code samples (Matlab and Java) at http://homepages.inf.ed.ac.uk/cgi/rbf/CVONLINE/entries.pl?TAG384
 */
void FitEllipse(ublas::matrix<double> data, int nPoints, double *x0, double *y0, double *rA, double *rB, double *angle);

/**
 * Fit a circle, using Least Squares Method by R. Bullock.
 * http://www.dtcenter.org/met/users/docs/write_ups/circle_fit.pdf
 */
void FitCircle(ublas::matrix<double> data, int nPoints, double *x0, double *y0, double *radius);

ublas::matrix<double> removeColumn(ublas::matrix<double> inMatrix, int column);
#endif
