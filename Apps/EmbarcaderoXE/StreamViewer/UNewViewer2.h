//---------------------------------------------------------------------------

#ifndef UNewViewer2H
#define UNewViewer2H

#include "DrawView3.h"
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <msxmldom.hpp>
#include <StdCtrls.hpp>
#include <XMLDoc.hpp>
#include <xmldom.hpp>
#include <XMLIntf.hpp>  // for allegro

#define MAXITEMS_INT 640 //from drdata.h
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "TDDraw.h"
#include <syncobjs.hpp>
#include <Graphics.hpp>
#include <jpeg.hpp>
#include <Chart.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
#include <msxmldom.hpp>
#include <XMLDoc.hpp>
#include <xmldom.hpp>
#include <XMLIntf.hpp>
#include <math.h>

#ifdef _DEBUG
	#undef _DEBUG  //prevent loading of debug version of library.
	#include "lsl_c.h" //must be "", not <>, otherwise get data corruption in console apps when using dll functions
	#define _DEBUG
#else
	#include "lsl_c.h"
#endif


class TForm1 : public TForm
{
__published:	// IDE-managed Components
		TTimer *TimerDisp;
	TProgressBar *ProgressBar1;
	TMemo *Memo1;
	TBevel *Bevel2;
	TButton *Button2;
	TPageControl *PageControl2;
	TTabSheet *TabSheet1;
	TBevel *Bevel1;
	TLabel *Label5;
	TImage *Image3;
	TPageControl *PageControl1;
	TTabSheet *TabSheet3;
	TTabSheet *TabSheet4;
	TTabSheet *TabSheet5;
	TTabSheet *TabSheet6;
	TTabSheet *TabSheet7;
	TTabSheet *TabSheet8;
	TTabSheet *TabSheet9;
	TTabSheet *TabSheet10;
	TTabSheet *TabSheet11;
	TTabSheet *TabSheet12;
	TTabSheet *TabSheet13;
	TTabSheet *TabSheet14;
	TTabSheet *TabSheet15;
	TTabSheet *TabSheet16;
	TTabSheet *TabSheet17;
	TTabSheet *TabSheet18;
	TTabSheet *TabSheet19;
	TTabSheet *TabSheet20;
	TTabSheet *TabSheet21;
	TTabSheet *TabSheet22;
	TUpDown *UpDown1;
	TEdit *Edit1;
	TListBox *ListBox2;
	TListBox *ListBox1;
	TImage *Image4;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TTrackBar *TrackBarGain;
	TCheckBox *CheckBoxXYZ;
	TCheckBox *CheckBoxXY;
	TCheckBox *CheckBoxZY;
	TTrackBar *TrackBarRot;
	TTrackBar *TrackBarTilt;
	TPanel *Panel1;
	TLabel *Label4;
	TEdit *Edit2;
	TUpDown *UpDown2;
	TUpDown *UpDown3;
	TLabeledEdit *LabeledEdit4;
	TButton *Button3;
	TCheckBox *CheckBoxConn3;
	TLabeledEdit *LabeledEdit1;
	TTabSheet *TabSheet32;
	TEdit *Edit3;
	TShape *Shape1;
	TCheckBox *CheckBox1;
	TComboBox *ComboBox1;
	TLabel *Label6;
	TMemo *Memo2;
	TXMLDocument *xd;
	TCheckBox *cbPlay;
	TImage *ImageCode;
	TCheckBox *cbParseData;
	TLabeledEdit *sensor0Edit;
	TLabeledEdit *sensor1Edit;
	TLabeledEdit *sensor2Edit;
	TLabeledEdit *sensor3Edit;
	TEdit *x0Edit;
	TEdit *y0Edit;
	TEdit *z0Edit;
	TEdit *x1Edit;
	TEdit *y1Edit;
	TEdit *z1Edit;
	TEdit *x2Edit;
	TEdit *y2Edit;
	TEdit *z2Edit;
	TEdit *x3Edit;
	TEdit *y3Edit;
	TEdit *z3Edit;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label9;
	TButton *RefreshStreamsButton;
		void __fastcall UpDown1MouseUp(TObject *Sender,
		  TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall PageControl1Change(TObject *Sender);
	void __fastcall ListBox1Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall TrackBarRotChange(TObject *Sender);
	void __fastcall Panel1MouseEnter(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall Image4MouseDown(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y);
	void __fastcall Image4MouseUp(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y);
	void __fastcall Image4MouseMove(TObject *Sender, TShiftState Shift, int X,
		  int Y);
	void __fastcall ListBox2Click(TObject *Sender);
	void __fastcall ComboBox1Select(TObject *Sender);
	void __fastcall udMoChChangingEx(TObject *Sender, bool &AllowChange, short NewValue,
          TUpDownDirection Direction);
	void __fastcall cbPlayClick(TObject *Sender);
	void __fastcall sensor0EditChange(TObject *Sender);
	void __fastcall sensor1EditChange(TObject *Sender);
	void __fastcall sensor2EditChange(TObject *Sender);
	void __fastcall sensor3EditChange(TObject *Sender);
	void __fastcall RefreshStreamsButtonClick(TObject *Sender);
	void __fastcall PageControl2Change(TObject *Sender);
private:	// User declarations

public:		// User declarations
		TTabSheet * TabSheet[20];
		__fastcall TForm1(TComponent* Owner);


		ULONG ChanIsOn[MAXITEMS_INT / 32];  // 16*32 = 512

		double VScale(){return pow(2.0,(double)UpDown1->Position); };
		CChannelN * Display;
		TDisplay3D * Display3D;
		HWND getHWnd(){return WindowHandle;};
		void GetDDRect(TImage * pImg,RECT & rThis);

};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;


int GetAttribute(_di_IXMLNode Node, AnsiString attr);



#endif

