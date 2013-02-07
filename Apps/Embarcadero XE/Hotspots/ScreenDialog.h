//---------------------------------------------------------------------------

#ifndef ScreenDialogH
#define ScreenDialogH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TScreenForm : public TForm
{
__published:	// IDE-managed Components
	TLabeledEdit *TopLeftEdit;
	TLabeledEdit *TopRightEdit;
	TLabeledEdit *BottomLeftEdit;
	TLabeledEdit *BottomRightEdit;
	TButton *DoneButton;
	TLabeledEdit *Sensor0Edit;
	TLabeledEdit *Sensor1Edit;
	TLabeledEdit *DeviceEdit;
	TLabeledEdit *MonitorNumberEdit;
	void __fastcall DoneButtonClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	int topLeftID, topRightID, bottomLeftID, bottomRightID;
	int sensor0, sensor1, device, monitorNumber;
	__fastcall TScreenForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TScreenForm *ScreenForm;
//---------------------------------------------------------------------------
#endif
