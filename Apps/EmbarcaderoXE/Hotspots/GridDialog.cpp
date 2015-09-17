//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "GridDialog.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TGridForm *GridForm;
//---------------------------------------------------------------------------
__fastcall TGridForm::TGridForm(TComponent* Owner)
	: TForm( Owner)
{
	Location1Edit->Text = "1";
	Location2Edit->Text = "2";
	Location3Edit->Text = "3";
	ColumnsEdit->Text = "2";
	RowsEdit->Text = "2";
	ColumnThicknessEdit->Text = "100";
	RowThicknessEdit->Text = "100";
	HeightThicknessEdit->Text = "100";
	SensorEdit->Text = "0";
	DeviceEdit->Text = "1";

}
//---------------------------------------------------------------------------
void __fastcall TGridForm::DoneButtonClick(TObject *Sender)
{

	location1 =  Location1Edit->Text.ToInt();
	location2 = Location2Edit->Text.ToInt();
	location3 = Location3Edit->Text.ToInt();
	nRows = RowsEdit->Text.ToInt();
	nColumns = ColumnsEdit->Text.ToInt();
	columnThickness = ColumnThicknessEdit->Text.ToDouble();
	rowThickness = RowThicknessEdit->Text.ToDouble();
	heightThickness = HeightThicknessEdit->Text.ToDouble();
	sensor = SensorEdit->Text.ToInt();
	device = DeviceEdit->Text.ToInt();
	ModalResult = mrOk;
}

