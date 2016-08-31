//---------------------------------------------------------------------------


#pragma hdrstop

#include "TOutline.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

//if the outline is a circle, this will give a good estimate of the parameters
	//to feed into the fit.
	//if it is not a circle, estimated eccentricity will typically be close to 1.
	void TOutline::estimateParams() {
		if(nPoints < 1) {
			x0 = NAN;
			y0 = NAN;
			r = NAN;
			eccentricity = NAN;
			return;
		} else {
				x0 = 0;
				y0 = 0;
			for(int i=0; i<nPoints; i++) {
				x0 += xValues[i];
				y0 += yValues[i];
			}
			x0/=nPoints;
			y0/=nPoints;
		}
		ublas::matrix<double> cov = calculateCovariance2D(xValues, yValues, nPoints);
		//calculate eigenvalues of covariane matrix
		double a = cov(0,0);
		double b = cov(0,1);
		double c = cov(1,0);
		double d = cov(1,1);

		if(_isnan(a) || _isnan(b)  || _isnan(c)  || _isnan(d)) {
			r = NAN;
			eccentricity = NAN;
			return;
		}
		double root = 4 * b*c + (a - d) * (a-d);
		if(root < 0.0) root = 0;
		else root = sqrt(root);
		double e1 = (a+d + root)/2; //eigenvalue 1 = major radius squared /2
		double e2 = (a+d - root)/2; //eigenvalue 2 = minor radius squared /2


		r = (e1 + e2);
		if(r < 0.0) r = 0.0;
		else r = sqrt(r);

		if (e1<e2) {
			double term = 1-e1/e2;
			term = (term > 0) ? term : 0;
			eccentricity = sqrt(term);
		} else if(e2 < e1) {
			double term = 1-e2/e1;
			term = (term > 0) ? term : 0;
			eccentricity = sqrt(term);
		} else {
			eccentricity = 0.0;
		}
	}

	/**
	 * Returns the data to be fed into a fit. Does not include data within
	 * crRadius of (crX0,crY0). cr is typically cornea reflection.
	 * Also sets nPointsMinusCR, for public use.

	 * Delete after last reference is removed.
	 *
	 */
	double** TOutline::createDataForFit(double crX0, double crY0, double crRadius) {
		double ** data = new2D(2, nPoints, 0.0);
		nPointsMinusCR = 0;
		for (int i=0; i < nPoints; ++i) {
			//drop points from near cornea reflection
			if(_isnan(crRadius) || ((xValues[i] - crX0)*(xValues[i] - crX0) + (yValues[i] - crY0)*(yValues[i] - crY0)
				> crRadius*crRadius)) {

				data[0][nPointsMinusCR] = (double) xValues[i];
				data[1][nPointsMinusCR++] = (double) yValues[i];
			}
		}


		return data;
	}

	ublas::matrix<double> TOutline::createMatrixForFit(double crX0, double crY0, double crRadius) {
		ublas::matrix<double> data(2,nPoints);
		//double ** data = new2D(2, nPoints, 0.0);
		nPointsMinusCR = 0;
		for (int i=0; i < nPoints; ++i) {
			//drop points from near cornea reflection
			if(_isnan(crRadius) || ((xValues[i] - crX0)*(xValues[i] - crX0) + (yValues[i] - crY0)*(yValues[i] - crY0)
				> crRadius*crRadius)) {

				data(0,nPointsMinusCR) = (double) xValues[i];
				data(1,nPointsMinusCR++) = (double) yValues[i];
			}
		}


		return data;
	}

	int TOutline::getNumberOfPoints() {
		return nPoints;
	}

	void TOutline::drawOutline(unsigned char** image, double **points, int nPoints) {
		for(int i=0; i<nPoints; i++) {
			image[(int)points[1][i]][((int)points[0][i]+100)*3+1] = 255;
		}
	}

	void TOutline::drawSpot(unsigned char ** image, signed char ** mask, const unsigned char ** data) {
		for(int i=0; i<nPoints; i++) {
			image[yValues[i]][xValues[i]*(24/8)+2] = 255;

		}

		for(int x=0; x<width; x++) {
			for(int y=0; y<height; y++) {
				if(mask[x][y] == 1) {
					image[y][(x+100)*3+ 1] = 255;
				}
				if(mask[x][y] == -1) {
					image[y][(x+100)*3 + 2] = 255;
				}

				if(data[x][y] == 1) {
					image[y][(x+200)*3+ 1] = 255;
				}
				if(data[x][y] == 0) {
					image[y][(x+200)*3 + 2] = 255;
				}
			}
		}
	}
	void TOutline::drawPoint(unsigned char ** image, int x, int y) {
		 image[y][(x+100)*3] = 255;

	}


	void TOutline::addPoint(int newX, int newY) {
		++nPoints;
		if(nPoints == nAvailable) {
			nAvailable *= 2;
			xValues = (int *) realloc(xValues, nAvailable * sizeof(int));
			yValues = (int *) realloc(yValues, nAvailable * sizeof(int));
		}

		xValues[nPoints-1] = newX;
		yValues[nPoints-1] = newY;
	}

	int TOutline::findOutline(const unsigned char **data, signed char **mask,
		int startingX, int startingY, unsigned char **image) {

  /*
		mask[startingX][startingY] = 1;


		this->addPoint(startingX, startingY);

		//if single point particle
		if(
			(startingY==0 || data[startingX][startingY-1] == 0) &&
			(startingX==width-1 || data[startingX+1][startingY] == 0) &&
			(startingY==height-1 || data[startingX][startingY+1] == 0) &&
			(startingX==0 || data[startingX-1][startingY] ==0)
			) {

			if(startingX < width - 1) mask[startingX+1][startingY] = -1;
//				drawPoint(image,startingX, startingY);
		} else {
			int x = startingX;
			int y = startingY;

			heading = RIGHT;
			//while walking around the edge of the outline.
			while(true) {
				switch(heading) {
					case UP:
						//if new point left
						if(x > 0 && data[x-1][y] == 1) {
							--x;
							heading = LEFT;
						} else {
							mask[x][y] = 1;
							//if new point up
							if (y > 0 && data[x][y-1] == 1) {
								--y;
								heading = UP;
							} else {
								//if new point right
								if(x < width-1 && data[x+1][y] == 1) {
									++x;
									heading = RIGHT;
								//new point down
								} else {
									++y;
									if(x < width - 1) mask[x+1][y] = -1;
									heading = DOWN;
								}
							}
						}
						break;
					case RIGHT:
						//if new point up
						if(y > 0 && data[x][y-1] == 1) {
							--y;
							heading = UP;
						} else {
							//if new point right
							if (x < width-1 && data[x+1][y] == 1) {
								++x;
								heading = RIGHT;
							} else {
								if(x < width - 1) mask[x+1][y] = -1;
								//if new point down
								if(y < height-1 && data[x][y+1] == 1) {
									++y;
									heading = DOWN;
								//new point left
								} else {
									--x;
									heading = LEFT;
								}
							}
						}
						break;
					case DOWN:
						//if new point right
						if(x < width-1 && data[x+1][y] == 1) {
							++x;
							heading = RIGHT;
						} else {
							if(x < width - 1) mask[x+1][y] = -1;
							//if new point down
							if (y < height-1 && data[x][y+1] == 1) {
								++y;
								heading = DOWN;
							} else {
								//if new left
								if(x > 0 && data[x-1][y] == 1) {
									--x;
									heading = LEFT;
								//new point up
								} else {
									mask[x][y] = 1;
									--y;
									heading = UP;
								}
							}
						}
						break;
					case LEFT:
						//if new point down
						if(y < height-1 && data[x][y+1] == 1) {
							++y;
							heading = DOWN;
						} else {
							//if new point left
							if (x > 0 && data[x-1][y] == 1) {
								--x;
								heading = LEFT;
							} else {
								mask[x][y] = 1;
								//if new up
								if(y > 0 && data[x][y-1] == 1) {
									--y;
									heading = UP;
								//new point right;
								} else {
									++x;
									heading = RIGHT;
								}
							}
						}
						break;

				}

				if(x==startingX && y== startingY) break;
				this->addPoint(x, y);

			}

		}

	 //	drawSpot(image,mask, data);

		return nPoints;
	*/

		mask[startingX][startingY] = 1;


		this->addPoint(startingX, startingY);

		if(
				(startingY==0 || data[startingX][startingY-1] == 0) &&
				(startingX==width-1 || data[startingX+1][startingY] == 0) &&
				(startingY==height-1 || data[startingX][startingY+1] == 0) &&
				(startingX==0 || data[startingX-1][startingY] ==0)
		) {

			if(startingX < width - 1) mask[startingX+1][startingY] = -1;

			//					drawPoint(image,startingX, startingY);
		} else {
			int x = startingX;
			int y = startingY;

			heading = RIGHT;
			//while walking around the edge of the outline.
			while(true) {
				switch(heading) {
				case UP:
					//if new point left
					if(x > 0 && data[x-1][y] == 1) {
						--x;
						heading = LEFT;
					} else {
						mask[x][y] = 1;
						//if new point up
						if (y > 0 && data[x][y-1] == 1) {
							--y;
							heading = UP;
						} else {
							//if at original position and orientation, we are done
							if(x==startingX && y == startingY) return nPoints;
							//if new point right
							if(x < width-1 && data[x+1][y] == 1) {

								++x;
								heading = RIGHT;
								//new point down
							} else {
								if(x < width - 1) mask[x+1][y] = -1;
								++y;

								heading = DOWN;
							}
						}
					}
					break;
				case RIGHT:
					//if new point up
					if(y > 0 && data[x][y-1] == 1) {
						--y;
						heading = UP;
					} else {
						//if at original position and orientation, we are done
						if(x==startingX && y == startingY && nPoints > 1) return nPoints;
						//if new point right
						if (x < width-1 && data[x+1][y] == 1) {
							++x;
							heading = RIGHT;
						} else {
							if(x < width - 1) mask[x+1][y] = -1;
							//if new point down
							if(y < height-1 && data[x][y+1] == 1) {
								++y;
								heading = DOWN;
								//new point left
							} else {
								--x;
								heading = LEFT;
							}
						}
					}
					break;
				case DOWN:
					//if at original position and orientation, we are done
					if(x==startingX && y == startingY) return nPoints;
					//if new point right
					if(x < width-1 && data[x+1][y] == 1) {
						++x;
						heading = RIGHT;
					} else {
						if(x < width - 1) mask[x+1][y] = -1;
						//if new point down
						if (y < height-1 && data[x][y+1] == 1) {
							++y;
							heading = DOWN;
						} else {
							//if new left
							if(x > 0 && data[x-1][y] == 1) {
								--x;
								heading = LEFT;
								//new point up
							} else {
								mask[x][y] = 1;
								--y;
								heading = UP;
							}
						}
					}
					break;
				case LEFT:
					//if new point down
					if(y < height-1 && data[x][y+1] == 1) {
						++y;
						heading = DOWN;
					} else {
						//if new point left
						if (x > 0 && data[x-1][y] == 1) {
							--x;
							heading = LEFT;
						} else {
							mask[x][y] = 1;
							//if new up
							if(y > 0 && data[x][y-1] == 1) {
								--y;
								heading = UP;
								//new point right;
							} else {
								//if at original position and orientation, we are done
								if(x==startingX && y == startingY) return nPoints;
								++x;
								heading = RIGHT;

							}
						}
					}
					break;

				}

				//	if((xValues.size() == 10000) break; escape valve if infinite loop, for testing only,
				//set set maximum number of points above this value in worm finder to mark which object are in infinite loop.
				//set minimum number of points high to filter out objects that were found correctly.
				this->addPoint(x, y);

			}

		}

		return nPoints;
	}
