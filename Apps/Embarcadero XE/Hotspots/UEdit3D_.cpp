//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UEdit3D.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm3 *Form3;
//---------------------------------------------------------------------------
__fastcall TForm3::TForm3(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm3::FormCreate(TObject *Sender)
{
	StringGrid1->Cells[0][0]  = "X";
	StringGrid1->Cells[1][0]  = "Y";
	StringGrid1->Cells[2][0]  = "Z";
}
//---------------------------------------------------------------------------
void __fastcall TForm3::ButtonDoneClick(TObject *Sender)
{
	ModalResult = mrOk;
	TPoint3D * p3D = new TPoint3D(
		StringGrid1->Cells[0][1].ToInt(),
		StringGrid1->Cells[1][1].ToInt(),
		StringGrid1->Cells[2][1].ToInt()
		);
	
	StringGrid1->Objects[0][0] = (TObject *) p3D;
}
//---------------------------------------------------------------------------

TPoint3D p30(100,58,0); // 30 deg
TPoint3D p45(100,100,0);// 45 deg
TPoint3D p60(100,173,0); // 60 deg
TPoint3D p90(0,100,0);     // 90
TPoint3D p120(-100,173,0); // 120 deg
TPoint3D p135(-100,100,0); // 135 deg
TPoint3D p150(-100,58,0);     // 150
TPoint3D p180(-100,0,0);     // 180

TPoint3D p30m(100,-58,0); // 30 deg
TPoint3D p45m(100,-100,0);// 45 deg
TPoint3D p60m(100,-173,0); // 60 deg
TPoint3D p90m(0,-100,0);     // 90
TPoint3D p120m(-100,-173,0); // 120 deg
TPoint3D p135m(-100,-100,0); // 135 deg
TPoint3D p150m(-100,-58,0);     // 150




void __fastcall TForm3::StringGrid1SelectCell(TObject *Sender, int ACol,
	  int ARow, bool &CanSelect)
{
	LabeledEdit1->Text = StringGrid1->Cells[ACol][ARow];
	LabeledEdit1->Left = StringGrid1->Left+ StringGrid1->CellRect(ACol,ARow).Left;
	LabeledEdit1->Top = StringGrid1->Top+ StringGrid1->CellRect(ACol,ARow).Top;
	LabeledEdit1->Width = StringGrid1->CellRect(ACol,ARow).Width();
	LabeledEdit1->Height = StringGrid1->CellRect(ACol,ARow).Height();
	LabeledEdit1->Visible = true;
	LabeledEdit1->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TForm3::LabeledEdit1Exit(TObject *Sender)
{
		StringGrid1->Cells[StringGrid1->Col][StringGrid1->Row] = LabeledEdit1->Text;
		LabeledEdit1->Visible = false;
}
//---------------------------------------------------------------------------
#include "DataArrays.h"

extern TMaxArray MaxArrayCapture;
//extern bool Capture;
extern HANDLE hMutex;
extern HANDLE hCapture;
extern HANDLE hCaptureDone;

#include "U3DHotspots.h"
#include <vcl.h>
void __fastcall TForm3::Button2Click(TObject *Sender)
{
	int res = WaitForSingleObject(hMutex,100);
	if (res!=WAIT_OBJECT_0) return;
	__try
	{
		SetEvent(hCapture);
	}
	__finally
	{
		ReleaseMutex(hMutex);
	}
}
//---------------------------------------------------------------------------



void __fastcall TForm3::Timer1Timer(TObject *Sender)
{
	int res = WaitForSingleObject(hCaptureDone,0);
	if (res==WAIT_OBJECT_0) // signalled
	{
		ResetEvent(hCaptureDone);
		int SensorNmb = UpDown1->Position;
		int Sensor1 =	0;
		StringGrid1->Cells[0][1]  = MaxArrayCapture[Sensor1+1 + SensorNmb*3];
		StringGrid1->Cells[1][1]  = MaxArrayCapture[Sensor1+1 + SensorNmb*3+1];
		StringGrid1->Cells[2][1]  = MaxArrayCapture[Sensor1+1 + SensorNmb*3+2];
	}
}
//---------------------------------------------------------------------------

