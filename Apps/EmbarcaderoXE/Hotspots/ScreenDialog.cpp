//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ScreenDialog.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//enum for virtual phasespace point, positive numbers are real phasespace points.
enum VIRTUAL_PS {IS_NONE=-1, IS_EYE=-2,IS_GAZE=-3};

TScreenForm *ScreenForm;
//---------------------------------------------------------------------------
__fastcall TScreenForm::TScreenForm(TComponent* Owner)
	: TForm(Owner)
{
	TopLeftEdit->Text = "1";
	TopRightEdit->Text = "2";
	BottomLeftEdit->Text = "3";
	BottomRightEdit->Text = "4";
	Sensor0Edit->Text = "e";
	Sensor1Edit->Text = "g";
	DeviceEdit->Text = "1";
	MonitorDepthEdit->Text = "16";
	XEdit->Text="0";
	YEdit->Text="0";
	WidthEdit->Text="1600";
	HeightEdit->Text="1200";

}
//---------------------------------------------------------------------------
void __fastcall TScreenForm::DoneButtonClick(TObject *Sender)
{
	topLeftID = TopLeftEdit->Text.ToInt();
	topRightID = TopRightEdit->Text.ToInt();
	bottomLeftID = BottomLeftEdit->Text.ToInt();
	bottomRightID = BottomRightEdit->Text.ToInt();

	if(Sensor0Edit->Text == "e") {
		sensor0 = IS_EYE;
	} else if(Sensor0Edit->Text == "g") {
		sensor0 = IS_GAZE;
	} else {
		sensor0 = Sensor0Edit->Text.ToInt();
	}

	if(Sensor1Edit->Text == "e") {
		sensor1 = IS_EYE;
	} else if(Sensor1Edit->Text == "g") {
		sensor1 = IS_GAZE;
	} else {
		sensor1 = Sensor1Edit->Text.ToInt();
	}

	device = DeviceEdit->Text.ToInt();

	monitorDepth = MonitorDepthEdit->Text.ToDouble();

	x = XEdit->Text.ToInt();
	y = YEdit->Text.ToInt();
	width = WidthEdit->Text.ToInt();
	height = HeightEdit->Text.ToInt();

	ModalResult = mrOk;
}
//---------------------------------------------------------------------------

