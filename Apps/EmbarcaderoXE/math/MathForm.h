//---------------------------------------------------------------------------

#ifndef MathFormH
#define MathFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TForm2 : public TForm
{
__published:	// IDE-managed Components
	TLabeledEdit *x0Edit;
	TLabeledEdit *y0Edit;
	TLabeledEdit *radiusEdit;
	TEdit *p1Edit;
	TEdit *p2Edit;
	void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
	void testMinimize();
	void testEyeFit();
	void testRigidFit();
	void testCovariance2D();
	void testEllipseFit();
	void testCircleFit();
	void testCircleFitOld();
	__fastcall TForm2(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm2 *Form2;
//---------------------------------------------------------------------------
#endif
