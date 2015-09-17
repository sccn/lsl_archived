/**
  Class to define and map an outline in an image.

Written by Matthew Grivich, Swartz Center for Computational Neuroscience, UCSD
*/


#include <vcl.h>
#pragma hdrstop

#ifndef TOutlineH
#define TOutlineH
//---------------------------------------------------------------------------
#endif

#include "stdio.h"
#include "math_util.h"

enum THeading {UP,LEFT,DOWN, RIGHT} ;

class TOutline {
	private:
		int nPoints;

		int nAvailable;
		int * xValues, * yValues;
		int width, height;
		THeading heading;
	public:
		double x0, y0, r, eccentricity;
		int nPointsMinusCR;
	public:
		TOutline(int width, int height) {
			this->width = width;
			this->height = height;
			nPoints = 0;
			nPointsMinusCR = 0;
			nAvailable = 1024;
			xValues = (int *) malloc(nAvailable * sizeof(int));
			yValues = (int *) malloc(nAvailable * sizeof(int));
			x0 = NAN;
			y0 = NAN;
			r = NAN;
			eccentricity = NAN;

		};

		~TOutline(){
			if(xValues) free(xValues);
			if(yValues) free(yValues);
		};

		/**
		 * if the outline is a circle, this will give a good estimate of the parameters
		 * to feed into the fit.
		 * if it is not a circle, estimated eccentricity will typically be close to 1.
		 */
		void estimateParams();

		/**
		 * Returns the data to be fed into a fit. Does not include data within
		 * crRadius of (crX0,crY0). cr is typically cornea reflection.
		 * Also sets nPointsMinusCR, for public use.
		 *
		 */
		double** createDataForFit(double crX0, double crY0, double crRadius);

		ublas::matrix<double> createMatrixForFit(double crX0, double crY0, double crRadius);
		/**
		 * Get the number of points in the outline.
		 */
		int getNumberOfPoints();

		/**
		 * Draws a 2-d series of points on an image. Mainly for debugging.
		 */
		void drawOutline(unsigned char** image, double **points, int nPoints);

		/**
		 * Draws the outline on an image. Adds color coding for the mask and threshold data.
		 * Mainly for debugging.
		 */
		void drawSpot(unsigned char ** image, signed char ** mask, const unsigned char ** data);

		/**
		 * Draws a point on an image. Mainly for debugging.
		 */
		void drawPoint(unsigned char ** image, int x, int y);

		/**
		 * Add a point to an outline.
		 */
		void addPoint(int newX, int newY);

		/**
		 * Given a starting point for an outline, finds the rest of the outline.
		 * Mask stores information about where outlines have already been found,
		 * so they will not be found again.
		 */
		int findOutline(const unsigned char **data, signed char **mask,
			int startingX, int startingY, unsigned char **image);


};