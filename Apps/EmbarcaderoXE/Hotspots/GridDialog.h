//---------------------------------------------------------------------------

#ifndef GridDialogH
#define GridDialogH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TGridForm : public TForm
{
__published:	// IDE-managed Components
	TLabeledEdit *Location1Edit;
	TLabeledEdit *Location2Edit;
	TLabeledEdit *ColumnsEdit;
	TLabeledEdit *RowsEdit;
	TLabeledEdit *HeightThicknessEdit;
	TLabeledEdit *SensorEdit;
	TButton *DoneButton;
	TLabeledEdit *DeviceEdit;
	TLabeledEdit *ColumnThicknessEdit;
	TLabeledEdit *RowThicknessEdit;
	TLabeledEdit *Location3Edit;

	void __fastcall DoneButtonClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	int location1, location2, location3, nColumns, nRows, sensor, device;
	double columnThickness, rowThickness, heightThickness;
	__fastcall TGridForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TGridForm *GridForm;
//---------------------------------------------------------------------------
#endif
