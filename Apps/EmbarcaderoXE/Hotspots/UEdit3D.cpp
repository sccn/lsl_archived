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
	p3D = NULL;
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
	bool ex = false;
	float x,y,z;
	try {
		x = StringGrid1->Cells[0][1].ToDouble();
		y = StringGrid1->Cells[1][1].ToDouble();
		z =  StringGrid1->Cells[2][1].ToDouble();

	} catch (...) {
		ex = true;
	}
	if(ex == false) {
		if(p3D) delete p3D;
		p3D = new TPoint3D(x,y,z);
		ModalResult = mrOk;
	}
}


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
//#include "DataArraysx.h"

//extern TMaxArray MaxArray;
extern float * phaseData;
extern HANDLE hMutex;
extern HANDLE hCapture;
extern HANDLE hCaptureDone;
extern channelsPerMarker;

#include "U3DHotspots.h"
#include <vcl.h>
#include <stdio.h>
void __fastcall TForm3::Button2Click(TObject *Sender)
{

	unsigned int res = WaitForSingleObject(hMutex,INFINITE);
	if (res==WAIT_OBJECT_0) // signalled
	{
		__try
		{
			int SensorNmb = UpDown1->Position;
			StringGrid1->Cells[0][1]  = phaseData[SensorNmb*channelsPerMarker];
			StringGrid1->Cells[1][1]  = phaseData[SensorNmb*channelsPerMarker+1];
			StringGrid1->Cells[2][1]  = phaseData[SensorNmb*channelsPerMarker+2];
	//		StringGrid1->Cells[0][1]  = MaxArray.Data[Sensor1 + SensorNmb*3  ];
	//		StringGrid1->Cells[1][1]  = MaxArray.Data[Sensor1 + SensorNmb*3+1];
	//		StringGrid1->Cells[2][1]  = MaxArray.Data[Sensor1 + SensorNmb*3+2];
		  //	printf("%d %d %d\n",MaxArray.Data[Sensor1 + SensorNmb*3  ],
		}

		__finally
		{

			ReleaseMutex(hMutex);
		}
	}

}
//---------------------------------------------------------------------------





inline __fastcall TForm3::~TForm3(void) {
	  if(p3D) delete p3D;
}
//---------------------------------------------------------------------------


