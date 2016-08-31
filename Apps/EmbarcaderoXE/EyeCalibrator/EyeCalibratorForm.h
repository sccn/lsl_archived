 /*

 Application to allow the calibrating of a head-mounted, one scene camera, one eye camera
 eye tracker.

 Written by Matthew Grivich, Swartz Center for Computational Neuroscience, UCSD

 */

//---------------------------------------------------------------------------

#ifndef EyeCalibratorFormH
#define EyeCalibratorFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <msxmldom.hpp>
#include <XMLDoc.hpp>
#include <xmldom.hpp>
#include <XMLIntf.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
#include <ComCtrls.hpp>
#include "vector.h"
#include "HotspotTypes.h"

#ifdef _DEBUG
	#undef _DEBUG  //prevent loading of debug version of library.
	#include "lsl_c.h" //must be "", not <>, otherwise get data corruption in console apps when using dll functions
	#define _DEBUG
#else
	#include "lsl_c.h"
#endif



//---------------------------------------------------------------------------
class TForm4 : public TForm
{
__published:	// IDE-managed Components
	TXMLDocument *xd;
	TTimer *Timer1;
	TButton *CalculateCalibrationBtn;
	TButton *SaveCalibrationPointsBtn;
	TButton *LoadCalibrationPointsBtn;
	TButton *SaveCalibrationBtn;
	TLabeledEdit *CameraWidthEdit;
	TLabeledEdit *CameraHeightEdit;
	TLabeledEdit *MonitorWidthEdit;
	TLabeledEdit *MonitorHeightEdit;
	TLabel *Label1;
	TLabel *Label2;
	TPanel *eyeCalibPanel;
	TGridPanel *GridPanel1;
	TEdit *rInEdit;
	TEdit *rRangeEdit;
	TEdit *rOutEdit;
	TEdit *x0InEdit;
	TEdit *x0RangeEdit;
	TEdit *x0OutEdit;
	TEdit *y0InEdit;
	TEdit *y0RangeEdit;
	TEdit *y0OutEdit;
	TEdit *z0InEdit;
	TEdit *z0RangeEdit;
	TEdit *z0OutEdit;
	TEdit *aInEdit;
	TEdit *aRangeEdit;
	TEdit *aOutEdit;
	TEdit *bInEdit;
	TEdit *bRangeEdit;
	TEdit *bOutEdit;
	TEdit *gInEdit;
	TEdit *gRangeEdit;
	TEdit *gOutEdit;
	TEdit *bxInEdit;
	TEdit *bxRangeEdit;
	TEdit *bxOutEdit;
	TEdit *byInEdit;
	TEdit *byRangeEdit;
	TEdit *byOutEdit;
	TEdit *bzInEdit;
	TEdit *bzRangeEdit;
	TEdit *bzOutEdit;
	TEdit *fcInEdit;
	TEdit *fcRangeEdit;
	TEdit *fcOutEdit;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label9;
	TLabel *Label10;
	TLabel *Label11;
	TLabel *Label12;
	TLabel *Label13;
	TLabel *Label14;
	TXMLDocument *xdoc_out;
	TSaveDialog *SaveDialog1;
	TButton *ClearCalibrationPointsBtn;
	TPanel *sceneCalibPanel;
	TProgressBar *GazeProgressBar;
	TProgressBar *SceneProgressBar;
	TProgressBar *PhasespaceProgressBar;
	TLabeledEdit *HeadMarker0Edit;
	TLabeledEdit *HeadMarker1Edit;
	TLabeledEdit *HeadMarker2Edit;
	TLabeledEdit *HeadMarker3Edit;
	TButton *ReferenceHeadButton;
	TButton *testEye;
	TButton *eyeTestDone;
	TTimer *Timer2;
	TComboBox *GazeComboBox;
	TComboBox *SceneComboBox;
	TComboBox *PhaseComboBox;
	TOpenDialog *OpenDialog1;
	TSaveDialog *SaveDialog2;
	TMemo *StatusMemo;
	TButton *CalibrationWindowButton;
	TTimer *Timer3;
	TOpenDialog *OpenDialog2;
	TButton *phasespaceTestStart;
	TButton *phasespaceTestStop;
	TLabeledEdit *phasespaceMarker0Edit;
	TLabeledEdit *phasespaceMarker1Edit;
	TButton *MonitorPositionButton;
	TButton *AbortCalibrationButton;
	TButton *LoadIntrinsicButton;
	TXMLDocument *xdoc_in;
	TButton *RefreshStreamsButton;
	TLabeledEdit *BackgroundRedEdit;
	TLabeledEdit *BackgroundGreenEdit;
	TLabeledEdit *BackgroundBlueEdit;
	TLabeledEdit *CalibDistanceEdit;

	/**
	 * Main loop. Pulls all available data from input streams. Stores data
	 * for averaging. What exactly gets stored depends on the mode.
	 * In the default mode, the eye and marker position is stored.
	 * Can also be configured to track the head or wand in phasespace.
	 */
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall CalculateCalibrationBtnClick(TObject *Sender);
	void __fastcall SaveCalibrationBtnClick(TObject *Sender);

	/**
	 * Load calibration points from a csv file
	 */
	void __fastcall LoadCalibrationPointsBtnClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall drawCalibration();
	void __fastcall MonitorWidthEditChange(TObject *Sender);
	void __fastcall MonitorHeightEditChange(TObject *Sender);
	void __fastcall CameraWidthEditChange(TObject *Sender);
	void __fastcall CameraHeightEditChange(TObject *Sender);

	/**
	 * Save calibration parameters to xml file.
	 */
	void __fastcall SaveCalibrationPointsBtnClick(TObject *Sender);
	void __fastcall ClearCalibrationPointsBtnClick(TObject *Sender);

	/**
	 * Given a series of eye calibration points (position of the center of the pupil
	 * in the eye camera), find fit parameters.
	 *
	 */
	void __fastcall eyeCalibration();
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	/**
	 * Click when head is in head-fixed position.
	 */
	void __fastcall ReferenceHeadButtonClick(TObject *Sender);

	/**
	 * Starts an eye test, which will print out the location of the eye
	 * continuously.
	 */
	void __fastcall testEyeClick(TObject *Sender);

	/**
	 * Ends the eye test.
	 */
	void __fastcall eyeTestDoneClick(TObject *Sender);

	/**
	 * If Gaze combo box changes, load new stream.
	 */
	void __fastcall GazeComboBoxChange(TObject *Sender);

	/**
	 * If Scene combo box changes, load new stream.
	 */
	void __fastcall SceneComboBoxChange(TObject *Sender);

	/**
	 * If Phasespace combo box changes, load new stream.
	 */
	void __fastcall PhaseComboBoxChange(TObject *Sender);
	void __fastcall CalibrationWindowButtonClick(TObject *Sender);
	void __fastcall Timer3Timer(TObject *Sender);
	void __fastcall FormKeyPress(TObject *Sender, wchar_t &Key);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall phasespaceTestStartClick(TObject *Sender);
	void __fastcall phasespaceTestStopClick(TObject *Sender);
	void __fastcall MonitorPositionButtonClick(TObject *Sender);
	void __fastcall HeadMarker0EditChange(TObject *Sender);
	void __fastcall HeadMarker1EditChange(TObject *Sender);
	void __fastcall HeadMarker2EditChange(TObject *Sender);
	void __fastcall HeadMarker3EditChange(TObject *Sender);
	void __fastcall phasespaceMarker0EditChange(TObject *Sender);
	void __fastcall phasespaceMarker1EditChange(TObject *Sender);
	void __fastcall AbortCalibrationButtonClick(TObject *Sender);
	void __fastcall LoadIntrinsicButtonClick(TObject *Sender);
	void __fastcall RefreshStreamsButtonClick(TObject *Sender);
	void __fastcall BackgroundRedEditChange(TObject *Sender);
	void __fastcall BackgroundGreenEditChange(TObject *Sender);
	void __fastcall BackgroundBlueEditChange(TObject *Sender);
	void __fastcall UpdateMarkers();


private:	// User declarations
	void LoadHotspotsConfig(const System::UnicodeString FileName);
	void poseFinder(std::vector<double> xs, std::vector<double> ys);
public:		// User declarations
	__fastcall TForm4(TComponent* Owner);
	THotspotScreen * getHotspotScreen(int deviceNumber);


	/**
	 * transform data coordinates so that fit parameters match convention.
	 * marker position in mm
	 * eye position in pixels, centered at zero
	 */
	void scaleToPhysical();

};
//---------------------------------------------------------------------------
extern PACKAGE TForm4 *Form4;
//---------------------------------------------------------------------------
#endif
