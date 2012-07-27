//---------------------------------------------------------------------------

#ifndef UEdit3DH
#define UEdit3DH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <ValEdit.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <DBGrids.hpp>
#include <DBCtrls.hpp>
#include <Mask.hpp>
#include <DB.hpp>
#include <DBClient.hpp>
//---------------------------------------------------------------------------
class TForm3 : public TForm
{
__published:	// IDE-managed Components
	TStringGrid *StringGrid1;
	TButton *Button2;
	TButton *ButtonDone;
	TLabeledEdit *LabeledEdit2;
	TUpDown *UpDown1;
	TLabeledEdit *LabeledEdit1;
	TTimer *Timer1;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall ButtonDoneClick(TObject *Sender);
	void __fastcall StringGrid1SelectCell(TObject *Sender, int ACol, int ARow,
		  bool &CanSelect);
	void __fastcall LabeledEdit1Exit(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TForm3(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm3 *Form3;


struct TPoint3D
{
public:
	AnsiString name;
	int x,y,z;
	TPoint3D(int X=0,int Y=0, int Z=0, AnsiString Name = ""): x(X),y(Y),z(Z) {name =Name;};
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
		return Sqrt(float(x)*x + float(y)*y + float(z)*z);
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


class THotspot3D: public TPoint3D
{
public:
	int r;
	THotspot3D(int X=0,int Y=0, int Z=0, AnsiString Name = "",int R=0):
		TPoint3D(X,Y,Z,Name),
		r(R) {};
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

class THotspot: public THotspot3D
{
public:
	int value;
	int sensor[2];
	int dist_mm;
	THotspot(
			int X=0, int Y =0, int Z =0,
			AnsiString Name = "",
			int Size = 0,
			int Value = 0,
			int S1 = -1,
			int S2 = -1,
			int dist = 0):
			THotspot3D(X,Y,Z,Name,Size),
			value(Value)
	{
			sensor[0] = S1;
			sensor[1] = S2;
			dist_mm = dist;
//		name = Name;
	};

};


double const PI =3.1415926535897932385;

#include <math.h>
#include "U3Dhotspots.h"

class TVectorToHotspot: public THotspot3D
{
public:
	int deg;
	TVectorToHotspot(TPoint3D PtOrigin, TPoint3D PtEnd, int Sz = 10):
		THotspot3D(),
		deg(Sz)
	{
		TPoint3D ptDif = PtEnd - PtOrigin;
		long double m = ptDif.modulus();
		if (m<1) m=1; // avoid div by 0
		r = 1000;
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
		if (fc!=old)
		{
//			String s;
//			s.sprintf("%d: %3d",fc <=deg, fc );
//			Form11->Memo1->Lines->Add(s);
		}
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

//---------------------------------------------------------------------------
#endif
