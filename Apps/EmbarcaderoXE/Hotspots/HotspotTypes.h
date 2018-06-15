//---------------------------------------------------------------------------

#ifndef HotspotTypesH
#define HotspotTypesH
//#include <Classes.hpp>
//#include <Controls.hpp>
//#include <StdCtrls.hpp>
//#include <Forms.hpp>
//#include <Grids.hpp>
//#include <ValEdit.hpp>
//#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <DBGrids.hpp>
#include <DBCtrls.hpp>
#include <Mask.hpp>
#include <DB.hpp>
#include <DBClient.hpp>
#include "stdio.h"
#include<list>
#include "LinearAlgebra.h"
#include "map.h"
#include "math_util.h"


struct TPoint3D
{
public:
	AnsiString name;
	float x,y,z;
	TPoint3D(float X=0.0,float Y=0.0, float Z=0.0, AnsiString Name = ""): x(X),y(Y),z(Z) {name =Name;};
	TPoint3D(const TPoint3D & c): x(c.x),y(c.y),z(c.z),name(c.name) {};
	TPoint3D & operator =(const TPoint3D & o)
	{
		x = o.x;
		y = o.y;
		z = o.z;
		return * this ;
	};
	TPoint3D operator -(TPoint3D & o)
	{
		TPoint3D ret(x - o.x, y - o.y, z - o.z); return ret;
	};
	long double modulus()
	{
		return Sqrt(x*x + y*y + z*z);
	}

	ublas::vector<double> toDoubleVector()
	{
		ublas::vector<double> vect(3);
		vect(0) = (double) x;
		vect(1) = (double) y;
		vect(2) = (double) z;
		return vect;
	}
};
inline bool operator == (TPoint3D Op1,TPoint3D Op2)
{
	return ((Op1.x==Op2.x)
				&&
			(Op1.y==Op2.y)
				&&
			(Op1.z==Op2.z));
};



class TRectHotspot : public TPoint3D {

public:
	float Xthickness, Ythickness, Zthickness;
	int sensor, value;
	TEdit *xEdit, *yEdit, *zEdit, *xtEdit, *ytEdit, *ztEdit, *sensorEdit, *valueEdit;
	TShape *activeShape;
	bool isChanged, isIn;
	int unchangedCount;

	TRectHotspot(float X=0.0,float Y=0.0, float Z=0.0,
	float Xt=0.0, float Yt=0.0, float Zt=0.0, int SENSOR=0, int VALUE=0):
	TPoint3D(X,Y,Z,""),
	Xthickness(Xt),
	Ythickness(Yt),
	Zthickness(Zt),
	sensor(SENSOR),
	value(VALUE) {
		isChanged = isIn = false;
		unchangedCount = 0;
		};
	bool Includes(TPoint3D &pt)
	{
		return(pt.x > x - Xthickness/2 && pt.x < x + Xthickness/2 &&
			   pt.y > y - Ythickness/2 && pt.y < y + Ythickness/2 &&
			   pt.z > z - Zthickness/2 && pt.z < z + Zthickness/2);

	}

	virtual float Distance(TPoint3D &pt1)
	{
		TPoint3D ptDif;
		ptDif = *this - pt1;
		float dist = ptDif.modulus();
		return dist;
	}
};



class THotspot: public TPoint3D
{
public:
	float r;
	int value;
	int sensor[2];
	int dist_mm;
	bool isChanged, isIn;
	int unchangedCount;
	TEdit *nameEdit, *rEdit, *sensor0Edit, *sensor1Edit, *valueEdit;
	TShape *activeShape;
	THotspot(
			float X=0.0, float Y =0.0, float Z =0.0,
			AnsiString Name = "",
			float R = 0,
			int Value = 0,
			int S1 = -1,
			int S2 = -1,
			int dist = 0):
			TPoint3D(X,Y,Z,Name),
			value(Value),
			r(R)
	{
			sensor[0] = S1;
			sensor[1] = S2;
			dist_mm = dist;
			nameEdit = rEdit = sensor0Edit = sensor1Edit = valueEdit = 0;
			activeShape = 0;
			isChanged = isIn = false;
			unchangedCount = 0;
			//		name = Name;
	};

	bool Includes(TPoint3D &pt1)
	{
		TPoint3D ptDif;
		ptDif = *this - pt1;

		int dist = ptDif.modulus();
		return dist <= r;
	}
	virtual float Distance(TPoint3D &pt1)
	{
		TPoint3D ptDif;
		ptDif = *this - pt1;
		float dist = ptDif.modulus();
		return dist;
	}
};

class THotspotGrid {

public:
	int location1, location2, location3, Xdiv, Zdiv, nSquares;
	double Xthickness, Ythickness, Zthickness;
	std::list<TRectHotspot*> rectHotspots;

	THotspotGrid(int lcation1, int lcation2, int lcation3, int Xdv,
	 int Zdv, double Xthckness, double Ythckness, double Zthckness) :
		  location1(lcation1),
		  location2(lcation2),
		  location3(lcation3),
		  Xdiv(Xdv),
		  Zdiv(Zdv),
		  Xthickness(Xthckness),
		  Zthickness(Zthckness),
		  nSquares(Xdv*Zdv) {

	}

	~THotspotGrid() {
		while(!rectHotspots.empty()) {
		delete rectHotspots.back();
		rectHotspots.pop_back();
		}
	}

};

#define PI 3.1415926535897932385

#include <math.h>
#include "U3Dhotspots.h"

class TVectorToHotspot: public TPoint3D//public THotspot3D
{
public:
	int deg;
	TVectorToHotspot(TPoint3D PtOrigin, TPoint3D PtEnd, int Sz = 10):
		TPoint3D(),
		deg(Sz)
	{
		TPoint3D ptDif = PtEnd - PtOrigin;
		long double m = ptDif.modulus();
		if (m<1) m=1; // avoid div by 0
		x = ptDif.x * 1000.0 /m;
		y = ptDif.y * 1000.0 /m;
		z = ptDif.z * 1000.0 /m;
	}

	bool Includes(TVectorToHotspot &ptNorm)
	{
		TPoint3D ptDifDir;
		ptDifDir= *this -ptNorm;
		long double mNorm = ptDifDir.modulus()/2/1000;
		float f = acos(mNorm)*360/(2*PI);
		static int fc;
		int old = fc;
		fc = 180 - 2*f +0.1; // round off
		static bool in = false;
		bool old_in = in;
		in =   fc<=deg;
		return in;
	}
	float Distance(TPoint3D &ptNorm)
	{
		TPoint3D ptDifDir;
		ptDifDir= *this -ptNorm;
		long double mNorm = ptDifDir.modulus()/2/1000;
		float f = acos(mNorm)*360/(2*PI);
		float fc;
		fc = 180 - 2*f; // round off
		return fc;
	}

};

class THotspotScreen {

	public:
		int topLeftID, bottomLeftID, topRightID, bottomRightID, sensor0, sensor1, device;
		double monitorDepth;
		int x, y, width, height;
		ublas::vector<double> monitorTL, monitorTR, monitorBL, monitorBR;

		TEdit *nameEdit,  *topLeftEdit, *topRightEdit, *bottomLeftEdit, *bottomRightEdit;
		TEdit *sensor0Edit, *sensor1Edit, *deviceEdit;
		TShape *activeShape;
		bool isChanged, isIn;
		int unchangedCount;

		double positionX;
		double positionY;

		double monitorWidth;
		double monitorHeight;

	THotspotScreen(std::map<int, TPoint3D*> p3Ds, int topLft, int topRght, int bottomLft, int bottomRght,
		int sensr0, int sensr1, int devce, double monitrDepth, int x0, int y0, int wdth, int hght):
		topLeftID(topLft),
		topRightID(topRght),
		bottomLeftID(bottomLft),
		bottomRightID(bottomRght),
		sensor0(sensr0),
		sensor1(sensr1),
		device(devce),
		monitorDepth(monitrDepth),
		x(x0),
		y(y0),
		width(wdth),
		height(hght) {

			monitorTL = p3Ds[topLeftID]->toDoubleVector();
			monitorTR = p3Ds[topRightID]->toDoubleVector();
			monitorBL = p3Ds[bottomLeftID]->toDoubleVector();
			monitorBR = p3Ds[bottomRightID]->toDoubleVector();

			//given the depth, push the monitor back by the requested amount.
			ublas::vector<double> tot = cross(monitorTR-monitorTL, monitorBL-monitorTL) +
				 cross(monitorBR-monitorTR, monitorTL-monitorTR) +
				  cross(monitorBL-monitorBR, monitorTR-monitorBR) + cross(monitorTL-monitorBL, monitorBR-monitorBL);
			tot = monitorDepth*tot/length(tot);

			monitorTL += tot;
			monitorTR += tot;
			monitorBL += tot;
			monitorBR += tot;

			isChanged = isIn = false;
			unchangedCount = 0;
			positionX = -1.0;
			positionY = -1.0;

			monitorWidth = (length(monitorBR - monitorBL) + length(monitorTR - monitorTL))/2;
			monitorHeight = (length(monitorBL - monitorTL) + 	length(monitorBR - monitorTR))/2;

	};

	/**
	 * Given screen coordinates, returns the location in phasespace coordinates.
	 *
	 *
	 */

	ublas::vector<double> THotspotScreen::screenToPosition(double monitorPosX, double monitorPosY) {
		ublas::vector<double> phasespacePos;
		phasespacePos = (monitorBR - monitorBL)*monitorPosX + (monitorTL - monitorBL)*monitorPosY;
		return phasespacePos;
	}

	/**
	 *  Gives the coordinates on a 2-d screen of a vector pointing from fromPos to targetPos.
	 *  result is stored in monitorPosX and monitorPosY.
	 *
	 */

	void THotspotScreen::pointToScreen(ublas::vector<double> fromPos, ublas::vector<double> targetPos,
			double *monitorPosX, double *monitorPosY) {
        static int i=0;
		bool pr = ((i++)%1000 == 0);


		if(hasNan(fromPos) || hasNan(targetPos)) {
			*monitorPosX = NAN;
			*monitorPosY = NAN;
			if(pr) if(hasNan(fromPos)) printf("fromPos is Nan.\n");
			if(pr) if(hasNan(targetPos)) printf("targetPos is Nan.\n");
			return;
		}
		ublas::matrix<double> A(3,3);
		ublas::vector<double> b(3);

		if(pr) {
			print("fromPos", fromPos);
			print("targetPos", targetPos);
		  //	print("monitorBL", monitorBL);
		  //	print("monitorBR", monitorBR);
		  //	print("monitorTL", monitorTL);
		}

		for(int i=0; i<3; i++) {
			A(i,0) = targetPos(i) - fromPos(i);
			A(i,1) = monitorBL(i) - monitorBR(i);
			A(i,2) = monitorBL(i) - monitorTL(i);
			b(i) = monitorBL(i) - fromPos(i);
		}

		ublas::vector<double> scaleFactors = linearSolve(A,b);
		if(pr) printf("monitorPosX: %g   monitorPosY: %g   monitorPosZ: %g\n", scaleFactors(0), scaleFactors(1), scaleFactors(2));


		*monitorPosX = scaleFactors(1);
		*monitorPosY = scaleFactors(2);
		positionX = *monitorPosX;
		positionY = *monitorPosY;
		  if(pr) 	printf("monitorPosX: %g   monitorPosY: %g\n", *monitorPosX, *monitorPosY);
	}

	bool THotspotScreen::Includes(double monitorPosX, double monitorPosY) {
		if(_isnan(monitorPosX)) return false;
		if(_isnan(monitorPosY)) return false;
		return (monitorPosX >= 0 && monitorPosX <= 1 && monitorPosY >= 0 && monitorPosY <= 1);
	}

};
//---------------------------------------------------------------------------
#endif
