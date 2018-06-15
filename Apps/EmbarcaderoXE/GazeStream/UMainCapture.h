

//---------------------------------------------------------------------------

#ifndef UmainCaptureH
#define UmainCaptureH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <OleCtrls.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <queue.h>
#include <Dialogs.hpp>
#include <msxmldom.hpp>
#include <XMLDoc.hpp>
#include <xmldom.hpp>
#include <XMLIntf.hpp>
#include "GazeUtil.h"
#include "TOutline.h"
#include <boost/shared_ptr.hpp>

#ifdef _DEBUG
	#undef _DEBUG  //prevent loading of debug version of library.
	#include "lsl_c.h" //must be "", not <>, otherwise get data corruption in console apps when using dll functions
	#define _DEBUG
#else
	#include "lsl_c.h"
#endif

//---------------------------------------------------------------------------

typedef struct {
	double x0;
	double y0;
	double a;
	double b;
	double angle;
} ds_Ellipse;

class TMainCaptureForm : public TForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl1;
	TTabSheet *TabSheet2;
	TLabel *Label1;
	TComboBox *cbVideoInputDevice;
	TLabel *Label36;
	TComboBox *cbVideoInputFormat;
	TTimer *Timer1;
	TTabSheet *TabSheet1;
	TCheckBox *cbRecord;
	TComboBox *cbVideoCodecs;
	TLabel *Label2;
	TEdit *edOutput;
	TLabel *lbCodecs;
	TLabel *Label4;
	TEdit *edTimestamp;
	TLabel *Label3;
	TEdit *edFrame;
	TPageControl *PageControl2;
	TTabSheet *tsScene;
	TBitBtn *BitBtnPlay;
	TBitBtn *BitBtnStop;
	TRadioGroup *RadioGroup1;
	TTabSheet *tsVideo;
	TLabeledEdit *BacklogEdit;
	TLabeledEdit *droppedFramesEdit;
	TOpenDialog *OpenDialog1;
	TXMLDocument *xdoc_in;
	TTabSheet *tsEyeTracker;
	TTrackBar *tbUpper;
	TTrackBar *tbLower;
	TPanel *tPanel;
	TTrackBar *tbLeft;
	TTrackBar *tbRight;
	TTrackBar *tbThreshold;
	TLabeledEdit *SubsamplingEdit;
	TButton *LoadCalibration;
	TLabeledEdit *edtFrameRate;
	TCheckBox *autoThresholdBox;
	TLabeledEdit *crRadiusMultiplierEdit;
	TLabeledEdit *nOutlinesEdit;
	TTrackBar *crRoiTop;
	TTrackBar *crRoiBottom;
	TTrackBar *crRoiRight;
	TTrackBar *crRoiLeft;
	TTrackBar *crThreshold;
	TLabeledEdit *crRadiusMaxEdit;
	TLabeledEdit *FrameDivisorEdit;
	TLabeledEdit *SpatialDivisorEdit;
	TLabeledEdit *maxEccentricityEdit;
	TCheckBox *cbRecordAudio;
	TLabeledEdit *xParallaxCorrectionEdit;
	TLabeledEdit *yParallaxCorrectionEdit;
	TCheckBox *cbCompress;
	TComboBox *cbSceneCalibColor;
	TLabel *Label5;
	TComboBox *cbVideoInput;
	TLabel *Label6;
	TLabeledEdit *eyeRadiusMaxEdit;
	TCheckBox *flipVertCheckbox;
	TComboBox *cbReferenceCalibColor;
	TLabel *Label7;
	TLabeledEdit *IdentifierEdit;
	TComboBox *cbAudioInputDevice;
	TLabel *Label8;
	TLabeledEdit *numberOfMarkersEdit;
	TCheckBox *sendFrameCheckbox;
	TComboBox *FrameComboBox;
	TButton *RefreshStreamsButton;
	TCheckBox *enableFrameSending;
	TLabeledEdit *edtRequestedFrameRate;
	TLabeledEdit *measuredFrameRateEdit;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall cbVideoInputDeviceChange(TObject *Sender);
	void __fastcall cbVideoInputFormatChange(TObject *Sender);
	void __fastcall btStopClick(TObject *Sender);

	/**
	 * Start recording video.
	 */
	void __fastcall cbRecordClick(TObject *Sender);

	/**
	 * Start playing video.
	 */
	void __fastcall BitBtnPlayClick(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall CheckBox1Click(TObject *Sender);

	/**
	 * Determine which mode will be used.
	 */
	void __fastcall RadioGroup1Click(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall LoadCalibrationClick(TObject *Sender);
	void __fastcall crRadiusMultiplierEditChange(TObject *Sender);
	void __fastcall nOutlinesEditChange(TObject *Sender);
	void __fastcall eyeRadiusMaxEditChange(TObject *Sender);
	void __fastcall FrameDivisorEditChange(TObject *Sender);
	void __fastcall SpatialDivisorEditChange(TObject *Sender);
	void __fastcall maxEccentricityEditChange(TObject *Sender);
	void __fastcall xParallaxCorrectionEditChange(TObject *Sender);
	void __fastcall yParallaxCorrectionEditChange(TObject *Sender);
	void __fastcall cbSceneCalibColorChange(TObject *Sender);
	void __fastcall cbVideoInputChange(TObject *Sender);
	void __fastcall crRadiusMaxEditChange(TObject *Sender);
	void __fastcall cbReferenceCalibColorChange(TObject *Sender);
	void __fastcall IdentifierEditChange(TObject *Sender);
	void __fastcall cbAudioInputDeviceChange(TObject *Sender);
	void __fastcall numberOfMarkersEditChange(TObject *Sender);
	void __fastcall FrameComboBoxChange(TObject *Sender);
	void __fastcall RefreshStreamsButtonClick(TObject *Sender);
	void __fastcall enableFrameSendingClick(TObject *Sender);

private:	// User declarations
	void __fastcall TMainCaptureForm::Start();
	boost::shared_ptr<Graphics::TBitmap> pBmpRec, pBmpRecGr;

	queue<BITMAP*> bmpQueue;
	double crRadiusMultiplier;
	double crRadiusMax;
	double eyeRadiusMax;
	int nOutlinesDesired;
	double frameRate;

public:		// User declarations

boost::shared_ptr<TOutline> findLargestOutline(BITMAP *aBmp, boolean above,
	int tbLeftPosition, int tbRightPosition, int tbLowerPosition, int tbUpperPosition, bool paint);

list<boost::shared_ptr<TOutline> > findLargestOutlines(BITMAP *aBmp, boolean above,
	int tbLeftPosition, int tbRightPosition, int tbLowerPosition, int tbUpperPosition, bool paint, int count);

void fitCircle(BITMAP *aBmp, boost::shared_ptr<TOutline> largestOutline, double *x0, double *y0, double *radius, double crX0, double crY0, double crRadius, boolean above,
	int tbLeftPosition, int tbRightPosition, int tbLowerPosition, int tbUpperPosition, bool paint);

void fitEllipse(BITMAP *aBmp, boost::shared_ptr<TOutline> largestOutline, double *x0, double *y0, double *radiusA, double *radiusB, double *angle, double crX0, double crY0, double crRadius, boolean above,
	int tbLeftPosition, int tbRightPosition, int tbLowerPosition, int tbUpperPosition, bool paint);
	/**
	 * Find at fit a circle in the image. Used for tracking the eye, the
	 * calibration spot, or the cornea reflection.
	 */
 //	void findAndFitCircle(BITMAP *aBmp, double *xCenter, double *yCenter, double *radius,
 //		double crX0, double crY0, double crRadius, boolean above,
 //		int tbLeftPosition, int tbRightPosition, int tbLowerPosition, int tbUpperPosition, bool paint);
	/**
	 * Called from the VideoStream.exe project. Sets the application to basic
	 * Video acquisition.
	 */
	void SetToVideoMode();


	Graphics::TBitmap *bmpGaze;

	/**
	 * Process a video frame.
	 *
	 */
	void __fastcall DoFrame(BITMAP *aBmp);
	__fastcall TMainCaptureForm(TComponent* Owner);

	boost::shared_ptr<TGazeUtil> gu;
	
	std::list<ds_Ellipse> storedPupils;//x0,y0,a,b,angle
};
//---------------------------------------------------------------------------
extern PACKAGE TMainCaptureForm *MainCaptureForm;
//---------------------------------------------------------------------------
#endif
