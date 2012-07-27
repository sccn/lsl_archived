//---------------------------------------------------------------------------

#ifndef U3dHotspotsH
#define U3dHotspotsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
#include "UEdit3D.h"
class TForm11 : public TForm
{
__published:	// IDE-managed Components
	TLabeledEdit *LabeledEdit1;
	TButton *Button1;
	TTimer *Timer1;
	TMemo *Memo1;
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TTabSheet *TabSheet3;
	TGridPanel *GridPanel2;
	TLabel *Label11;
	TLabel *Label12;
	TLabel *Label13;
	TLabel *Label14;
	TLabel *Label15;
	TButton *Button5;
	TGridPanel *GridPanel3;
	TLabel *Label17;
	TLabel *Label18;
	TLabel *Label19;
	TLabel *Label20;
	TLabel *Label16;
	TLabel *Label21;
	TProgressBar *ProgressBar1;
	TMemo *Memo2;
	TButton *Button2;
	TButton *Button3;
	TButton *Button4;
	TSaveDialog *SaveDialog1;
	TOpenDialog *OpenDialog1;
	TUpDown *UpDown1;
	TEdit *Edit1;
	TProgressBar *ProgressBar2;
	TProgressBar *ProgressBar3;
	TProgressBar *ProgressBar4;
	TTabSheet *TabSheet4;
	TGridPanel *GridPanel4;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label22;
	TLabel *Label23;
	TLabel *Label24;
	TLabel *Label25;
	TPanel *Panel1;
	TGridPanel *GridPanel1;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label9;
	TLabel *Label10;
	TLabel *Label1;
	TComboBox *ComboBox1;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall GridPanel1Click(TObject *Sender);
	void __fastcall Button5Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall ComboBox1Change(TObject *Sender);
private:	// User declarations
	int FindMmfBuffer();
public:		// User declarations
	void AddHotspot(TPoint3D * p3D);
	__fastcall TForm11(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm11 *Form11;
//---------------------------------------------------------------------------
#endif
