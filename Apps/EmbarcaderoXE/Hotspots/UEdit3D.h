/**
 Modal dialog to acquire a phasespace point and save it as a location.

 Also includes a number of hotspot classs definitions.
 */

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
#include "stdio.h"
#include<list>
#include "LinearAlgebra.h"
#include "map.h"
#include "HotspotTypes.h"


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
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall ButtonDoneClick(TObject *Sender);
	void __fastcall StringGrid1SelectCell(TObject *Sender, int ACol, int ARow,
		  bool &CanSelect);
	void __fastcall LabeledEdit1Exit(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);

private:	// User declarations
public:		// User declarations
	TPoint3D *p3D;
	__fastcall TForm3(TComponent* Owner);
	inline __fastcall ~TForm3(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm3 *Form3;




//---------------------------------------------------------------------------
#endif
