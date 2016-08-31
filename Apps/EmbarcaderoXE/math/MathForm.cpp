

#include <vcl.h>
#pragma hdrstop

#include "MathForm.h"
#include "Linear1DFunction.h"
#include "CircleFunction.h"
#include "Fitter.h"
#include "FitFailedException.h"
#include "Minimizer.h"
#include <iostream>
#include <fstream>
#include "stdio.h"
//#include "Matrix.h"
#include "LinearAlgebra.h"
#include "epnp.h"
#include "cvWrapper.h"
#define DUB_PI 6.28318531
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm2 *Form2;
//---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void TForm2::testCircleFit() {
	ublas::matrix<double> data(2,7);
	data(0,0) = 0;  data(1,0) = 0;
	data(0,1) = .5;  data(1,1) = .25;
	data(0,2) = 1;  data(1,2) = 1;
	data(0,3) = 1.5;  data(1,3) = 2.25;
	data(0,4) = 2;  data(1,4) = 4;
	data(0,5) = 2.5;  data(1,5) = 6.25;
	data(0,6) = 3;  data(1,6) = 9;
	//x0: -11.8393
	//y0: 8.44643
	//radius: 14.6864
	double x0,y0,radius;
	FitCircle(data,7, &x0, &y0, &radius);
	printf("testCircleFit:\n");
	printf("x0: %g\n", x0);
	printf("y0: %g\n", y0);
	printf("radius: %g\n", radius);
}

void TForm2::testEllipseFit() {

	double Cx = 250;
	double Cy = 150;
	double Rx = 300;
	double Ry = 200;
	double Rotation = .4;

  int nPoints = 50;
  ublas::matrix<double> data(2,nPoints);
 //  ublas::vector<double> x(nPoints);
 //  ublas::vector<double> y(nPoints);

   /*	x(0) = 45;
	x(1) = 90;
	x(2) = 135;
	x(3) = 90;
	x(4) = 45;
	x(5) = 63;

	y(0) = 45;
	y(1) = 45;
	y(2) = 90;
	y(3) = 90;
	y(4) = 90;
	y(5) = 63;
	 */
 /*	for (int i=0; i<x.size(); i++) {
		double tx =  Rx*cos(2*PI*i/nPoints);
		double ty =  Ry*sin(2*PI*i/nPoints);
		x(i) = tx*cos(Rotation) - ty*sin(Rotation) + Cx;
		y(i) = tx*sin(Rotation) + ty*cos(Rotation) + Cy;
	}
   */
	for (int i=0; i<nPoints; i++) {
		double tx =  Rx*cos(2*PI*i/nPoints);
		double ty =  Ry*sin(2*PI*i/nPoints);
		data(0,i) = tx*cos(Rotation) - ty*sin(Rotation) + Cx;
		data(1,i) = tx*sin(Rotation) + ty*cos(Rotation) + Cy;
	}

	double x0=0.0,y0=0.0,rA=0.0,rB=0.0,angle=0.0;
	try {
		FitEllipse(data,nPoints,&x0,&y0,&rA,&rB,&angle);
	} catch(exception ex) {

		//printf("%s\n", ex.what());
	}
	printf("x0: %g\n", x0);
	printf("y0: %g\n", y0);
	printf("rA: %g\n", rA);
	printf("rB: %g\n", rB);
	printf("angle: %g\n", angle);
}



void TForm2::testCovariance2D() {
	double *xs = new1D(5,0.0);
	xs[0] = 3;
	xs[1] = 4;
	xs[2] = 5;
	xs[3] = 3;
	xs[4] = 3;

	double *ys = new1D(5,0.0);

	ys[0] = 3;
	ys[1] = 4;
	ys[2] = 1;
	ys[3] = -1;
	ys[4] = 0;

	ublas::matrix<double> cov = calculateCovariance2D(xs, ys, 5);

	print("cov", cov);
}

void TForm2::testMinimize() {
     		double *params = new1D(2, 0.0);
		MinimizerFunction *mf = new MinimizerFunction(params, 2);
		Minimizer *m = new Minimizer(mf);
		m->DoMinimize();
		double* pOut = mf->getParameters();

		p1Edit->Text = UnicodeString(pOut[0]);
		p2Edit->Text = UnicodeString(pOut[1]);

		delete m;
		delete mf;
		delete1D(params);
}

void TForm2::testRigidFit() {

	int nPoints = 4;

	double aIn = .3;
	double bIn = .8;
	double gIn = .7;

	double xIn = 0;
	double yIn = 0;
	double zIn = 0;
	double sa = sin(aIn);
	double sb = sin(bIn);
	double sg = sin(gIn);
	double ca = cos(aIn);
	double cb = cos(bIn);
	double cg = cos(gIn);

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




	double **orig = new2D(nPoints, 3, 0.0);
	orig[0][0] = 0.0;
	orig[0][1] = 0.0;
	orig[0][2] = 0.0;

	orig[1][0] = 1.0;
	orig[1][1] = 0.0;
	orig[1][2] = 0.0;

	orig[2][0] = 0.0;
	orig[2][1] = 1.0;
	orig[2][2] = 0.0;

	orig[3][0] = 0.0;
	orig[3][1] = 0.0;
	orig[3][2] = 1.0;

	double **current = new2D(nPoints, 3, 0.0);

	for(int i=0; i<nPoints; i++) {
		double x = orig[i][0];
		double y = orig[i][1];
		double z = orig[i][2];
		double tx = r00*x + r01*y + r02*z;
		double ty = r10*x + r11*y + r12*z;
		double tz = r20*x + r21*y + (r22)*z;
		current[i][0] = tx + xIn;
		current[i][1] = ty + yIn;
		current[i][2] = tz + zIn;
		printf("current[i][0]: %g\n", current[i][0]);
			printf("current[i][1]: %g\n", current[i][1]);
				printf("current[i][2]: %g\n\n", current[i][2]);
	}


	double **error = new2D(nPoints,3,1.0);

	double params[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	double *spread = new1D<double>(6, 0.0);

	spread[0] = 3.14/2.0;
	spread[1] = 3.14/2.0;
	spread[2] = 3.14/2.0;
	spread[3] = 1.0;
	spread[4] = 1.0;
	spread[5] = 1.0;
	RigidDeltaFunction *df = new RigidDeltaFunction(orig, current, error, nPoints, 3, params, 6);
	df->setStartingSpread(spread);

	Minimizer *m = new Minimizer(df);
	m->nmax = 1000;
	m->DoMinimize();

	double* pOut = df->getParameters();
	printf("a: %g\n", pOut[0]);
	printf("b: %g\n", pOut[1]);
	printf("g: %g\n", pOut[2]);
	printf("x0: %g\n", pOut[3]);
	printf("y0: %g\n", pOut[4]);
	printf("z0: %g\n", pOut[5]);

	delete2D(error, nPoints);
	delete2D(current, nPoints);
	delete2D(orig, nPoints);
	delete1D(spread);
	delete df;

 }


void TForm2::testEyeFit() {

	double monitorWidth = 326.0; //mm
	double monitorHeight = 260.0; //mm
	double cameraWidth = 352; //pixels
	double cameraHeight = 240; //pixels

	int nPoints = 0;
 /*	FILE *fp = fopen("C:\\Users\\mgrivich\\data.csv", "r");
	while(fgets(str,sizeof(str),fp) != NULL) {
		i++;
	}
   */


   //	rewind(fp);


	std::ifstream ifs;
	ifs.open ( "C:\\Users\\mgrivich\\data.csv"); // declare file stream: http://www.cplusplus.com/reference/iostream/ifstream/
	string value;

	while (ifs.good()) {
		getline(ifs, value);
		nPoints++;

	}
	nPoints--;

	double **coords = new2D(nPoints, 2, 0.0);
	double **data = new2D(nPoints, 2, 0.0);
	double **error = new2D(nPoints,2,0.0);
	ifs.clear();
	ifs.seekg(0);

	for(int i=0; i<nPoints; i++) {
		getline(ifs, value, ',');
		coords[i][0] =  monitorWidth*((1000.0 - atof(value.c_str()))/1000.0);

		getline(ifs, value, ',');
		coords[i][1] = monitorHeight*((1000.0 - atof(value.c_str()))/1000.0);

		getline(ifs, value, ',');
		data[i][0] = atof(value.c_str())-cameraWidth/2.0;

		getline(ifs, value, ',');
		data[i][1] = cameraHeight/2.0 - atof(value.c_str());

		getline(ifs, value, ',');
		error[i][0] = atof(value.c_str());

		getline(ifs, value);
		error[i][1] = atof(value.c_str());



	}

	ifs.close();

 /*	for(int k =0; k<nPoints; k++) {
			printf( "%g,%g,%g,%g,%g,%g\n", coords[k][0], coords[k][1], measured[k][0], measured[k][1], error[k][0], error[k][0]);

	} */
   //	double params[11] = {10.0, 570.0, 130.0, 185.0, -.4836443450476, -1.1132326618289, .8172934386507,
	//		-2.1796283945831, 8.0098006074300, 33.3526742746832, 563.0};

	double params[11] = {10.0, 570.0, 130.0, 185.0, 0, 0, 0,
			0, 0, 50, 563.0};
  double *spread = new1D<double>(11, 0.0);
//  spread[0] = 3;
//  spread[1] = 50;
//  spread[2] = 50;
//  spread[3] = 50;
  spread[4] = 3.14/2;
  spread[5] = 3.14/2;
  spread[6] = 3.14/2;
  spread[7] = 5;
  spread[8] = 5;
  spread[9] = 30;
  spread[10] = 0;
	EyeDeltaFunction *df = new EyeDeltaFunction(coords, data, error, nPoints, 2, params, 11);
	df->setStartingSpread(spread);
	Minimizer *m = new Minimizer(df);

	m->DoMinimize();


	double* pOut = df->getParameters();
	printf("R: %g\n", pOut[0]);
	printf("x0: %g\n", pOut[1]);
	printf("y0: %g\n", pOut[2]);
	printf("z0: %g\n", pOut[3]);
	printf("a: %g\n", pOut[4]);
	printf("b: %g\n", pOut[5]);
	printf("g: %g\n", pOut[6]);
	printf("bx: %g\n", pOut[7]);
	printf("by: %g\n", pOut[8]);
	printf("dc: %g\n", pOut[9]);
	printf("z: %g\n", pOut[10]);

	spread[1] = 50;
	spread[2] = 50;
	spread[3] = 50;
	df->setStartingSpread(spread);
	m->DoMinimize();
	pOut = df->getParameters();

		printf("R: %g\n", pOut[0]);
	printf("x0: %g\n", pOut[1]);
	printf("y0: %g\n", pOut[2]);
	printf("z0: %g\n", pOut[3]);
	printf("a: %g\n", pOut[4]);
	printf("b: %g\n", pOut[5]);
	printf("g: %g\n", pOut[6]);
	printf("bx: %g\n", pOut[7]);
	printf("by: %g\n", pOut[8]);
	printf("dc: %g\n", pOut[9]);
	printf("z: %g\n", pOut[10]);

	spread[0] = 3;
  //	spread[10] = 20;

		df->setStartingSpread(spread);
	m->DoMinimize();
	pOut = df->getParameters();

	printf("R: %g\n", pOut[0]);
	printf("x0: %g\n", pOut[1]);
	printf("y0: %g\n", pOut[2]);
	printf("z0: %g\n", pOut[3]);
	printf("a: %g\n", pOut[4]);
	printf("b: %g\n", pOut[5]);
	printf("g: %g\n", pOut[6]);
	printf("bx: %g\n", pOut[7]);
	printf("by: %g\n", pOut[8]);
	printf("dc: %g\n", pOut[9]);
	printf("z: %g\n", pOut[10]);
 }


//while ( file.good() )
//{
//	 getline ( file, value, ',' ); // read a string until next comma: http://www.cplusplus.com/reference/string/getline/
  //	 cout << string( value, 0, value.length()-1 ) << "\n"; // display value removing the first and the last character from it
//}
 /*	while(fgets(str,sizeof(str),fp) != NULL) {
	printf("%s\n", str);
		scanf(str,"%g,%g,%g,%g,%g,%g\n", &coords[i][0], &coords[i][1], &measured[i][0], &measured[i][1], &error[i][0], &error[i][0]);
		printf( "%g,%g,%g,%g,%g,%g\n", coords[i][0], coords[i][1], measured[i][0], measured[i][1], error[i][0], error[i][0]);
		i++;
	}*/
//}

void TForm2::testCircleFitOld() {
		int nData = 60;
		double **x = new2D<double>(2,nData,0.0);
		for(int i=0; i<nData; i++) {
			x[0][i] = 200*cos(DUB_PI*i/(double) nData) + 50;
			x[1][i] = 200*sin(DUB_PI*i/(double) nData) - 60;
		}


		double * y = new1D<double>(nData, 0.0);
		double * sig = new1D<double>(nData, 1.0);


		CircleFunction *f = new CircleFunction(0, 0, 100);

		Fitter *fit = new Fitter();
try {


		fit->fit(f, x, y, sig, 2, nData);
		double *pOut = f->getParameters();
		double *sOut = f->getSigmas();

		x0Edit->Text = UnicodeString(pOut[0]) + UnicodeString(" +/- ") + UnicodeString(sOut[0]);
		y0Edit->Text = UnicodeString(pOut[1]) + UnicodeString(" +/- ") + UnicodeString(sOut[1]);
		radiusEdit->Text = UnicodeString(pOut[2]) + UnicodeString(" +/- ") + UnicodeString(sOut[2]);


} catch (FitFailedException& e) {
	x0Edit->Text = UnicodeString(e.what());
}

	delete f;
	delete fit;
	delete2D(x, 2);
	delete1D(y);
	delete1D(sig);
}

void testLinearSolve() {
	//solves A(m,n).x(n)=b(m).  Borrows heavily from svbksb, numerical rescipies, section 2.6
	UnicodeString Astring = UnicodeString("3,4,2,2;2,3,5,1;1,2,2,1");
	UnicodeString Bstring = UnicodeString("3,5,2,1");
	ublas::matrix<double> A = unicodeStringToMatrix<double>(Astring);
	ublas::vector<double> b = unicodeStringToVector<double>(Bstring);

	ublas::vector<double> x = linearSolve(A,b);

	print("A", A);
	print("b", b);
	print("x", x);

	printf("x should equal: 1.16667, 0.166667, -.5\n");


}

void testSVD() {
  int m = 3;
  int n = 3;
	ublas::matrix<double> mat(m,n);
	ublas::matrix<double> u(m,n);
	ublas::matrix<double> v(n,n);
	ublas::vector<double> w(n);

   /*	mat(0,0) = 1;
	mat(0,1) = 1;
	mat(0,2) = 2;
	mat(1,0) = 3;
	mat(1,1) = 4;
	mat(1,2) = 5;
	mat(2,0) = 6;
	mat(2,1) = 7;
	mat(2,2) = 8;
	 */

	mat(0,0) = .1875;
	mat(0,1) = -.0625;
	mat(0,2) = -.0625;
	mat(1,0) = -.0625;
	mat(1,1) = .1875;
	mat(1,2) = -.0625;
	mat(2,0) = -.0625;
	mat(2,1) = -.0625;
	mat(2,2) = .1875;

	svdcmp(mat, u, w, v);
	print("mat", mat);
	print("u", u);
	print("w", w);
	print("v",v);

	ublas::matrix<double> wm(n,n);
	wm*=0;
	wm(0,0) = w(0);
	wm(1,1) = w(1);
	wm(2,2) = w(2);
	ublas::matrix<double> orig(m,n);

	orig = prod(wm, trans(v));
	orig = prod(u, orig);
	print("orig", orig);

}

void testFindRigidBody() {

	double x = 5.0;
	double y = 3.0;
	double z = 2.0;
	double a = .1;
	double b = .7;
	double g = .5;

	int nDims = 3;
	int nPoints = 4;
	ublas::matrix<double> dataOrig(3,4);
	ublas::matrix<double> dataTest(3,4);
	ublas::matrix<double> dataCurrent(3,4);
	ublas::vector<double> translationOrig(3);
	translationOrig*=0;
	ublas::vector<double> translationCurrent(3);
	translationCurrent*=0;
	ublas::matrix<double> rotation(3,3);
	dataOrig*=0.0;
	dataOrig(0,1) = 1;
	dataOrig(1,2) = 1;
	dataOrig(2,3) = 1;
	dataCurrent = dataOrig;

	print("dataOrig", dataOrig);

	for(int dim=0; dim<nDims; ++dim) {
		// find center of mass for each dimension
		for(int point=0; point<nPoints; ++point) {
			translationOrig(dim) += dataCurrent(dim,point);
		}
		translationOrig(dim)/=nPoints;
		//subtract off center of mass for all original points for a given dimension
		for(int point=0; point<nPoints; ++point) {
			dataCurrent(dim,point) -= translationOrig(dim);
		}
	}





	double sa = sin(a);
	double sb = sin(b);
	double sg = sin(g);
	double ca = cos(a);
	double cb = cos(b);
	double cg = cos(g);

	//rotation matrix
	ublas::matrix<double> r(3,3);
	r(0,0) = ca*cb;
	r(0,1) =  ca*sb*sg - sa*cg;
	r(0,2) =  ca*sb*cg + sa*sg;
	r(1,0) =  sa*cb;
	r(1,1) = sa*sb*sg + ca*cg;
	r(1,2) = sa*sb*cg - ca*sg;
	r(2,0) = -sb;
	r(2,1) = cb*sg;
	r(2,2) = cb*cg;

	dataCurrent = prod(r, dataCurrent);
	for(int point=0; point<nPoints;point++) {
		dataCurrent(0,point) += x;// + translationOrig(0);
		dataCurrent(1,point) += y;// + translationOrig(1);
		dataCurrent(2,point) += z;// + translationOrig(2);
	}

	print("Moved data", dataCurrent);

	findRigidBody(dataOrig, dataCurrent, translationOrig, rotation, translationCurrent);

	print("translationOrig", translationOrig);
	print("translationCurrent", translationCurrent);
	print("rotation", rotation);
	double B= -asin(rotation(2,0));
	double A = acos(rotation(0,0)/cos(B));
	double G = acos(rotation(2,2)/cos(B));

	printf("A: %g\n", A);
	printf("B: %g\n", B);
	printf("G: %g\n\n", G);

	//test to see if we can move dataOrig into dataCurrent
	dataTest = moveRigidBody(dataOrig, translationOrig, rotation, translationCurrent);
	print("dataTest, should equal moved data", dataTest);

	//test to see if we can move dataCurrent into dataOrig

	dataOrig = moveRigidBody(dataTest, translationCurrent, trans(rotation), translationOrig);
	print("dataOrig, should be the same as before", dataOrig);

}

void testFindMidpoint() {
	ublas::vector<double> p1(3,2.0), p2(3,2.0), p3(3,0.0), p4(3,0.0);
	p2(2) = 1;
	p4(0) = 1;
	ublas::vector<double> result = findMidpoint(p1,p2,p3, p4);
	print("result", result);//yields 2, 1, 0
}

void testEPNP() {

	ublas::matrix<double> boardPoints(3,12);
	int p=0;
	for(int w=0; w<2/*7*/; w++) {
				for(int h=0; h<6; h++) {
					boardPoints(0,p) = -20.0+w*5.0;
					boardPoints(1,p) = -20.0 + h*5.0;
					boardPoints(2,p) = 0.0;
					p++;
				}
	}

	ublas::matrix<double> cameraMatrix(3,3);

	cameraMatrix(0,0) = 654.8611202347574;
	cameraMatrix(0,1) = 0;
	cameraMatrix(0,2) = 319.5;
	cameraMatrix(1,0) = 0;
	cameraMatrix(1,1) = 654.8611202347574;
	cameraMatrix(1,2) = 239.5;
	cameraMatrix(2,0) = 0;
	cameraMatrix(2,1) = 0;
	cameraMatrix(2,2) = 1;

	ublas::vector<double> distCoeffVec(5);
	distCoeffVec(0) = -0.3669624087278113;
	distCoeffVec(1) = -0.07638290902180184;
	distCoeffVec(2) = 0;
	distCoeffVec(3) = 0;
	distCoeffVec(4) = 0.5764668364450144;

	ublas::matrix<double> foundPoints(2,12);


	foundPoints(0,0) = 122.56181;
	foundPoints(1,0) = 64.894775;
	foundPoints(0,1) = 163.85579;
	foundPoints(1,1) = 63.682297;
	foundPoints(0,2) = 204.5;
	foundPoints(1,2) = 62;
	foundPoints(0,2) = 245.62991;
	foundPoints(1,2) = 61.093784;
	foundPoints(0,3) = 283;
	foundPoints(1,3) = 61;
	foundPoints(0,4) = 319.81903;
	foundPoints(1,4) = 61.967384;
	foundPoints(0,5) = 354.08017;
	foundPoints(1,5) = 62.274704;
	foundPoints(0,6) = 123;
	foundPoints(1,6) = 104.5;
	foundPoints(0,7) = 164.5;
	foundPoints(1,7) = 102.5;
	foundPoints(0,8) = 204.5;
	foundPoints(1,8) = 100.5;
	foundPoints(0,9) = 244;
	foundPoints(1,9) = 99.5;
	foundPoints(0,10) = 281.5;
	foundPoints(1,10) = 99;
	foundPoints(0,11) = 318.5;
	foundPoints(1,11) = 99;
   /*	foundPoints(353.72653, 96.017204));
	foundPoints(124.62646,144.43448));
	foundPoints(165.5,142.5));
	foundPoints(206.03426,140.04895));
	foundPoints(245,138.5));
	foundPoints(283,137.5));
	foundPoints(319,136));
	foundPoints(354.5,134.5));
	foundPoints(127.50209,184.51553));
	foundPoints(167.5,181.5));
	foundPoints(207,179));
	foundPoints(245.5,177));
	foundPoints(283,175));
	foundPoints(318.88574,174.8522));
	foundPoints(353.5,170.5));
	foundPoints(128.28163,224.11998));
	foundPoints(169,220.5));
	foundPoints(210.13632,217.0213));
	foundPoints(247,214.5));
	foundPoints(282.64105,212.52209));
	foundPoints(319.19608,209.22551));
	foundPoints(343,206));
	foundPoints(134,260.5));
	foundPoints(172.92181,256.8718));
	foundPoints(211,255));
	foundPoints(248.5,250.5));
	foundPoints(285,248));
	foundPoints(319.5,243));
	foundPoints(353.30963,240.85687));  */

	 epnp PnP(cameraMatrix, boardPoints, foundPoints/*undistortedPoints*/);
	 ublas::matrix<double> R (3,3);
	 ublas::vector<double> tvec(3);
   	 PnP.compute_pose(R,tvec);
	 print("R", R);
	 print("tvec", tvec);
}

void testPose() {
	printf("openCV_version: %d\n", openCV_version());
	int nPoints = 12;
	double ** objectPoints = new2D<double>(nPoints, 3, 0.0);
	double ** imagePoints = new2D<double>(nPoints, 2, 0.0);
	double ** cameraMatrix = new2D<double>(3,3,0.0);
	double * distanceCoeffs = new1D<double>(5,0.0);
	double ** rMat = new2D<double>(3,3,0.0);
	double * tVec = new1D<double>(3,0.0);

    int p=0;
	for(int w=0; w<2/*7*/; w++) {
				for(int h=0; h<6; h++) {
					objectPoints[p][0] = -20.0+w*5.0;
					objectPoints[p][1] = -20.0 + h*5.0;
					objectPoints[p][2] = 0.0;
					p++;
				}
	}


	cameraMatrix[0][0] = 654.8611202347574;
	cameraMatrix[0][1] = 0;
	cameraMatrix[0][2] = 319.5;
	cameraMatrix[1][0] = 0;
	cameraMatrix[1][1] = 654.8611202347574;
	cameraMatrix[1][2] = 239.5;
	cameraMatrix[2][0] = 0;
	cameraMatrix[2][1] = 0;
	cameraMatrix[2][2] = 1;

	distanceCoeffs[0] = -0.3669624087278113;
	distanceCoeffs[1] = -0.07638290902180184;
	distanceCoeffs[2] = 0;
	distanceCoeffs[3] = 0;
	distanceCoeffs[4] = 0.5764668364450144;



	imagePoints[0][0] = 122.56181;
	imagePoints[0][1] = 64.894775;
	imagePoints[1][0] = 163.85579;
	imagePoints[1][1] = 63.682297;
	imagePoints[2][0] = 204.5;
	imagePoints[2][1] = 62;
	imagePoints[3][0] = 245.62991;
	imagePoints[3][1] = 61.093784;
	imagePoints[4][0] = 283;
	imagePoints[4][1] = 61;
	imagePoints[5][0] = 319.81903;
	imagePoints[5][1] = 61.967384;
	imagePoints[6][0] = 354.08017;
	imagePoints[6][1] = 62.274704;
	imagePoints[7][0] = 123;
	imagePoints[7][1] = 104.5;
	imagePoints[8][0] = 164.5;
	imagePoints[8][1] = 102.5;
	imagePoints[9][0] = 204.5;
	imagePoints[9][1] = 100.5;
	imagePoints[10][0] = 244;
	imagePoints[10][1] = 99.5;
	imagePoints[11][0] = 281.5;
	imagePoints[11][1] = 99;

	openCV_findPose(objectPoints, imagePoints, nPoints, cameraMatrix, distanceCoeffs, rMat, tVec);

	for(int i=0; i<3; i++) {
	for(int j=0; j<3; j++) {
	printf("rMat[%d][%d]: %g\n", i,j, rMat[i][j]);
	}
	}

	/* output:
openCV_version: 2
rMat[0][0]: 0.0440691
rMat[0][1]: 0.782867
rMat[0][2]: 0.620627
rMat[1][0]: 0.998206
rMat[1][1]: -0.0597012
rMat[1][2]: 0.00442775
rMat[2][0]: 0.0405185
rMat[2][1]: 0.619319
rMat[2][2]: -0.784093
tvec[0]: -13.5792
tvec[1]: -18.099
tvec[2]: 143.858
*/


	for(int i=0; i<3; i++) {
		printf("tvec[%d]: %g\n", i, tVec[i]);
	}

	delete2D(objectPoints, nPoints);
	delete2D(imagePoints, nPoints);
	delete2D(cameraMatrix, 3);
	delete1D(distanceCoeffs);
	delete2D(rMat, 3);
	delete1D(tVec);



}

void __fastcall TForm2::FormCreate(TObject *Sender)
{
	testCircleFit();
	//testEllipseFit();
	//testPose();
   //	testEPNP();
	//testCircleFit();
	//testCovariance2D();
	//testLinearSolve();
 /*	double **mat = newMatrix(3, 3);
	mat[0][0] = 1;
	mat[0][1] = 1;
	mat[0][2] = 2;
	mat[1][0] = 3;
	mat[1][1] = 4;
	mat[1][2] = 5;
	mat[2][0] = 6;
	mat[2][1] = 7;
	mat[2][2] = 8;
	printMatrix(mat, 3, 3);
	double d = determinate(mat, 3);
	printf("d: %g\n", d);
	printMatrix(mat, 3, 3);
	delete2D(mat, 3);
   */	/*	double params[11] = {10.0, 570.0, 130.0, 185.0, -.4836443450476, -1.1132326618289, .8172934386507,
			-2.1796283945831, 8.0098006074300, 33.3526742746832, 563.0};

		double *coords = new1D<double>(2, 0.0);
		coords[0] = 50.0;
		coords[1] = 40.0;
		double *modeledPoint = new1D<double>(2,0.0);
		DeltaFunction *df = new DeltaFunction(NULL, NULL, 0, 2, params, 11);

		df->fromMonitorToCamera(params, coords, modeledPoint);
		printf("modeled: %g %g\n", modeledPoint[0], modeledPoint[1]);
	  */
  //	  testEyeFit();
	   //	testMinimize();
	   //	testFitCircle();
//	testRigidFit();
	//testFindRigidBody();
   //	  testSVD();
   /*	ublas::matrix<double> m (3, 3);
	for (unsigned i = 0; i < m.size1 (); ++ i) {
		for (unsigned j = 0; j < m.size2 (); ++ j) {
			m (i, j) = 3 * i + j;
			m(0,0) = 1;
			printf("%g\t", m(i,j));

		}
		printf("\n");
	}
	printf("determinate:  %g\n", determinate(m));
	ublas::matrix<double> mt = ublas::trans(m);
	for(unsigned i=0; i<mt.size1(); ++i) {
		for(unsigned j=0; j<mt.size2(); ++j) {
			printf("%g\t", mt(i,j));
		}
		printf("\n");
	} */
 /*  Matrix m(5,5);
   m(1,3) = 106.15;
   m.print();
   Matrix t = m.transpose();
   t.print();
   printf("%g\n", m(1,3));
*/}


//---------------------------------------------------------------------------
/*
void __fastcall TForm2::FormCreate(TObject *Sender)
{
		double **x = malloc2D<double>(1,4,0.0);
		x[0][0] = 1;
		x[0][1] = 2;
		x[0][2] = 3;
		x[0][3] = 4;


		double y[4] = {2.99, 4.89, 5, 8};
		double sig[4] = {1, 1, 1, 1};

 //		Linear1DFunction l = Linear1DFunction.lineFit(x[0], y, sig);
//		System.out.println(l);

		Linear1DFunction *f = new Linear1DFunction(.5, .5);

		Fitter *fit = new Fitter();
		fit->fit(f, x, y, sig, 1, 4);
		double *pOut = f->getParameters();
		double *sOut = f->getSigmas();
//		printf("slope: %g +/- %g\n", pOut[0],sOut[0]);
//		printf("intercept: %g +/- %g\n", pOut[1],sOut[1]);
		slopeEdit->Text = UnicodeString(pOut[0]) + UnicodeString(" +/- ") + UnicodeString(sOut[0]);
		interceptEdit->Text = UnicodeString(pOut[1]) + UnicodeString(" +/- ") + UnicodeString(sOut[1]);
		free2D(x, 1);
		delete f;
		delete fit;
}
*/
