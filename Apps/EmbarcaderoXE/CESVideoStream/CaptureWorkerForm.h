//---------------------------------------------------------------------------
#ifndef CaptureWorkerFormH
#define CaptureWorkerFormH
//---------------------------------------------------------------------------
#include "useallegro4.h"
#include "winalleg.h"
#include "queue.h"


#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "VidGrab.hpp"
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include <Graphics.hpp>
#include <Menus.hpp>
#include <jpeg.hpp>
#include "Math.hpp"

#ifdef _DEBUG
	#undef _DEBUG  //prevent loading of debug version of library.
	#include "lsl_c.h" //must be "", not <>, otherwise get data corruption in console apps when using dll functions
	#define _DEBUG
#else
	#include "lsl_c.h"
#endif
//---------------------------------------------------------------------------

const CFreeHandMouseMax = 1000;

class TCaptureWorkerForm : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label9;
        TMemo *mmoLog;
        TEdit *edtStoragePath;
        TEdit *edtFrameCount;
		TEdit *edtDVDateTime;
        TVideoGrabber *VideoGrabber;
        TPageControl *pctMain;
		TTabSheet *tbsVideoDevice;
        TTabSheet *tbsAudio;
        TTabSheet *tbsRecording;
        TTabSheet *tbsFrameGrabber;
        TTabSheet *tbsDisplay;
        TLabel *Label42;
        TLabel *Label43;
        TRadioGroup *rgdFrameGrabber;
        TRadioGroup *rgdFrameGrabberFormat;
        TLabel *Label21;
        TGroupBox *GroupBox2;
        TLabel *Label30;
        TButton *btnStartAudioRendering;
        TButton *btnStartWAVRecording;
        TButton *btnStartMP3Recording;
        TButton *btnStopAudio;
        TCheckBox *chkUseAudioOfVideoCaptureDevice;
        TButton *btnStartASFRecording;
        TLabel *Label2;
        TRadioGroup *rdgVUMeters;
        TPanel *pnLeftVUMeter;
        TPanel *pnRightVUMeter;
        TGroupBox *grbVideoDeviceDialogs;
        TButton *btnDeviceDialog;
        TCheckBox *chk2ndVideoWindowActive;
        TCheckBox *chk1stVideoWindowActive;
        TGroupBox *grbMainVideoWindow;
        TCheckBox *chk1stEmbedded;
        TCheckBox *chk1stFullScreen;
        TCheckBox *chk1stStayOnTop;
        TCheckBox *chk1stUseVideoPort;
        TRadioGroup *rdg1stDisplayedOn;
        TCheckBox *chk1stMouseMovesWindow;
        TCheckBox *chk1stAutoSize;
        TButton *btnEG1stNewLocation;
        TButton *btnEG1stNewLocation2;
        TRadioGroup *rdg1AspectRatio;
        TCheckBox *chk1stVisible;
        TGroupBox *grb2ndVideoWindow;
        TLabel *Label76;
        TCheckBox *chk2ndEmbedded;
        TCheckBox *chk2ndFullScreen;
        TCheckBox *chk2ndStayOnTop;
        TCheckBox *chk2ndUseVideoPort;
        TRadioGroup *rdg2ndDisplayedOn;
        TCheckBox *chk2ndMouseMovesWindow;
        TCheckBox *chk2ndAutoSize;
        TButton *btnEG2ndNewLocation;
        TButton *btnEG2ndNewLocation2;
        TCheckBox *chk2ndVisible;
        TRadioGroup *rdg2AspectRatio;
        TTrackBar *tbr2PanScan;
        TLabel *Label74;
        TTrackBar *tbr1PanScan;
        TButton *btnTestPanScan;
        TMainMenu *MainMenu1;
        TMenuItem *mnuFile;
        TMenuItem *mnuExit;
        TMenuItem *mnuSendDVCommands;
        TMenuItem *mnuDVPlay;
        TMenuItem *mnuDVStop;
        TMenuItem *mnuDVFreeze;
        TMenuItem *mnuDVThaw;
        TMenuItem *mnuDVFF;
        TMenuItem *mnuDVRew;
        TMenuItem *mnuDVRecord;
        TMenuItem *mnuDVRecordfreeze;
        TMenuItem *mnuDVRecordStrobe;
        TMenuItem *mnuDVStepFwd;
        TMenuItem *mnuDVStepRev;
        TMenuItem *mnuDVModeShuttle;
        TMenuItem *mnuDVPlayfastestfwd;
        TMenuItem *mnuDVPlayslowestfwd;
        TMenuItem *mnuDVPlayfastestrev;
        TMenuItem *mnuDVPlayslowestrev;
        TMenuItem *mnuAbout;
        TGroupBox *grbAudioRendering;
        TLabel *Label17;
        TLabel *Label35;
        TTrackBar *tbrAudioVolume;
        TTrackBar *tbrAudioBalance;
        TCheckBox *chkMuteAudioRendering;
        TComboBox *cboAudioRenderers;
        TGroupBox *grbVideoSource;
        TComboBox *cboVideoSource;
        TGroupBox *grbVsVideoCaptureDevice;
        TLabel *Label4;
        TLabel *Label8;
        TLabel *Label18;
        TLabel *Label19;
        TComboBox *cboVideoInputs;
        TComboBox *cboVideoSizes;
        TComboBox *cboVideoSubtypes;
        TComboBox *cboAnalogVideoStandard;
        TComboBox *cboVideoDevices;
        TGroupBox *GroupBox1;
        TCheckBox *chkMultiplexedAutoSwitch;
        TCheckBox *chkMultiplexedInputEmulation;
        TRadioGroup *rdgPreferredVideoSize;
        TCheckBox *chkStretchPreferredVideoSize;
        TCheckBox *chkRenderAudioDevice;
        TGroupBox *grbAudioCaptureDevice;
        TLabel *Label26;
        TLabel *Label49;
        TLabel *Label36;
        TComboBox *cboAudioDevices;
        TComboBox *cboAudioInputs;
        TTrackBar *tbrAudioInputLevel;
        TTrackBar *tbrAudioInputBalance;
        TCheckBox *chkAudioInputMono;
        TButton *btnDlgAudioDevice;
        TGroupBox *grbAudioSynchronization;
        TCheckBox *chkAudioSyncAdjust;
        TTrackBar *tbrAudioSyncAdjust;
        TEdit *edtAudioSyncAdjust;
        TCheckBox *chkPreserveNativeFormat;
        TButton *btnNativeFormat;
        TCheckBox *chkAudioRecording;
        TLabel *Label5;
        TComboBox *cboAudioFormats;
        TCheckBox *chkAVISaveToThisFile;
        TCheckBox *chkPreallocFile;
        TEdit *edtRecordingFileName;
        TButton *btnCreatePreallocFileNow;
        TLabel *Label11;
        TEdit *edtPreallocSize;
        TGroupBox *GroupBox5;
        TLabel *Label1;
        TRadioGroup *grbRecordingTimer;
        TEdit *edtRecordingTimerInterval;
        TGroupBox *grbPreview;
        TBitBtn *btnStartPreview;
        TButton *btnStopPreview;
        TButton *btnPausePreview;
        TButton *btnResumePreview;
        TGroupBox *grbFrameRate;
        TEdit *edtFrameRate;
        TButton *btnFrameRate;
        TButton *btnAboutFrameRate;
        TCheckBox *btnAutoRefreshPreview;
        TCheckBox *chkReduceDVFrameRate;
        TMenuItem *Options1;
        TMenuItem *Resetvideocapturedevicesettings1;
        TBitBtn *btnStartRecordingImmediately;
        TBitBtn *btnStartRecordingControlled;
        TButton *btnStopRecording;
        TButton *btnRenewRecordingFile;
        TGroupBox *grbAVIPauseResume;
        TCheckBox *chkRecordingCanPause;
        TButton *btnPauseRecording;
        TButton *btnResumeRecording;
        TCheckBox *chkRecordingPauseCreatesNewFile;
        TCheckBox *chkRecordingOnMotion;
        TGroupBox *grbCompression;
        TLabel *Label6;
        TLabel *Label16;
        TComboBox *cboVideoCompressors;
        TComboBox *cboAudioCompressors;
        TRadioGroup *rdgCompressMode;
        TRadioGroup *rdgCompressType;
        TRadioGroup *rdgRecordingSize;
        TLabel *Label37;
        TButton *btnVideoCompressorSettings;
        TButton *btnSaveVideoCompressorSettings;
        TButton *btnReloadVideoCompressorSettings;
        TButton *btnAudioCompressorSettings;
        TButton *btnSaveAudioCompressorSettings;
        TButton *btnReloadAudioCompressorSettings;
        TRadioGroup *rdgVideoRenderer;
        TCheckBox *chkBlackMagicDecklink;
        TCheckBox *chkVideoVisibleWhenStopped;
        TCheckBox *chkAdjustPixelAspectRatio;
        TCheckBox *chkBorder;
        TCheckBox *chkChangeCursor;
        TCheckBox *chkTransparency;
        TCheckBox *chkSetLogo;
        TImage *imgLogo;
        TRadioGroup *rdgRecordingMethod;
        TGroupBox *grbVsFileOrUrl;
        TSpeedButton *btnSelectFileOrUrl;
        TLabel *Label100;
        TLabel *Label101;
        TEdit *edtVideoSourceFileOrUrl;
        TEdit *VideoSourceFileOrUrl_UserName;
        TEdit *VideoSourceFileOrUrl_Password;
        TGroupBox *grbVsScreenRecording;
        TLabel *Label29;
        TLabel *Label90;
        TEdit *edtScreenRecordingWindow;
        TButton *btnScreenRecordingWindow;
        TEdit *edtMonitorNumber;
        TButton *btnMonitorNumberInfo;
        TCheckBox *chkScreenRecordingWithCursor;
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall cboVideoDevicesChange(TObject *Sender);
        void __fastcall btnStartPreviewClick(TObject *Sender);
        void __fastcall btnStopPreviewClick(TObject *Sender);
        void __fastcall btnPausePreviewClick(TObject *Sender);
        void __fastcall btnResumePreviewClick(TObject *Sender);
        void __fastcall btnAutoRefreshPreviewClick(TObject *Sender);
        void __fastcall cboVideoInputsChange(TObject *Sender);
        void __fastcall cboVideoSizesChange(TObject *Sender);
        void __fastcall cboAnalogVideoStandardChange(TObject *Sender);
        void __fastcall cboVideoSubtypesChange(TObject *Sender);
        void __fastcall btnDeviceDialogClick(TObject *Sender);
		void __fastcall btnStreamDialogClick(TObject *Sender);
		void __fastcall btnFrameRateClick(TObject *Sender);
        void __fastcall chkReduceDVFrameRateClick(TObject *Sender);
        void __fastcall VideoGrabberVideoDeviceSelected(TObject *Sender);
        void __fastcall VideoGrabberPreviewStarted(TObject *Sender);
        void __fastcall cboAudioInputsChange(TObject *Sender);
        void __fastcall cboAudioDevicesChange(TObject *Sender);
        void __fastcall tbrAudioInputLevelChange(TObject *Sender);
        void __fastcall cboAudioFormatsChange(TObject *Sender);
		void __fastcall VideoGrabberAudioDeviceSelected(TObject *Sender);
		void __fastcall btnStopRecordingClick(TObject *Sender);
        void __fastcall rdgRecordingMethodClick(TObject *Sender);
        void __fastcall chkRecordingCanPauseClick(TObject *Sender);
        void __fastcall btnPauseRecordingClick(TObject *Sender);
        void __fastcall btnResumeRecordingClick(TObject *Sender);
        void __fastcall chkPreallocFileClick(TObject *Sender);
        void __fastcall btnCreatePreallocFileNowClick(TObject *Sender);
        void __fastcall edtPreallocSizeChange(TObject *Sender);
        void __fastcall cboVideoCompressorsChange(TObject *Sender);
        void __fastcall cboAudioCompressorsChange(TObject *Sender);
        void __fastcall btnVideoCompressorSettingsClick(TObject *Sender);
        void __fastcall btnAudioCompressorSettingsClick(TObject *Sender);
        void __fastcall rdgCompressModeClick(TObject *Sender);
        void __fastcall rdgCompressTypeClick(TObject *Sender);
        void __fastcall rgdFrameGrabberClick(TObject *Sender);
		void __fastcall rgdFrameGrabberFormatClick(TObject *Sender);
		void __fastcall rdgPreferredVideoSizeClick(TObject *Sender);
		void __fastcall chkRenderAudioDeviceClick(TObject *Sender);
        void __fastcall chkBorderClick(TObject *Sender);
        void __fastcall chkTransparencyClick(TObject *Sender);
        void __fastcall rdgVideoRendererClick(TObject *Sender);
        void __fastcall chk1stVideoWindowActiveClick(TObject *Sender);
        void __fastcall chk1stEmbeddedClick(TObject *Sender);
        void __fastcall chk1stAutoSizeClick(TObject *Sender);
        void __fastcall chk1stFullScreenClick(TObject *Sender);
        void __fastcall chk1stStayOnTopClick(TObject *Sender);
        void __fastcall chk1stUseVideoPortClick(TObject *Sender);
        void __fastcall rdg1stDisplayedOnClick(TObject *Sender);
        void __fastcall btnEG1stNewLocationClick(TObject *Sender);
        void __fastcall btnEG1stNewLocation2Click(TObject *Sender);
        void __fastcall chk1stMouseMovesWindowClick(TObject *Sender);
        void __fastcall chk2ndVideoWindowActiveClick(TObject *Sender);
        void __fastcall chk2ndEmbeddedClick(TObject *Sender);
        void __fastcall chk2ndFullScreenClick(TObject *Sender);
        void __fastcall chk2ndAutoSizeClick(TObject *Sender);
        void __fastcall chk2ndStayOnTopClick(TObject *Sender);
        void __fastcall chk2ndUseVideoPortClick(TObject *Sender);
        void __fastcall chk2ndMouseMovesWindowClick(TObject *Sender);
        void __fastcall rdg2ndDisplayedOnClick(TObject *Sender);
        void __fastcall btnEG2ndNewLocationClick(TObject *Sender);
        void __fastcall btnEG2ndNewLocation2Click(TObject *Sender);
        void __fastcall edtStoragePathChange(TObject *Sender);
        void __fastcall VideoGrabberLog(TObject *Sender, TLogType LogType,
          String Severity, String InfoMsg);
        void __fastcall VideoGrabberDVCommandCompleted(TObject *Sender,
          int NewStateValue, String NewStateLabel);
        void __fastcall mnuAboutClick(TObject *Sender);
        void __fastcall SendVCRCommand (TObject *Sender);
        void __fastcall VideoGrabberResize(TObject *Sender);
		void __fastcall VideoGrabberDeviceLost(TObject *Sender);
        void __fastcall VideoGrabberDiskFull(TObject *Sender);
        void __fastcall VideoGrabberDeviceArrivalOrRemoval(TObject *Sender,
          bool IsDeviceArrival, bool IsVideoDevice, String DeviceName,
          int DeviceIndex);
		void __fastcall mnuExitClick(TObject *Sender);
		void __fastcall tbrAudioInputBalanceChange(TObject *Sender);
        void __fastcall chkAudioInputMonoClick(TObject *Sender);
        void __fastcall VideoGrabberReinitializing(TObject *Sender);
        void __fastcall VideoGrabberCreatePreallocFileStarted(
          TObject *Sender, String FileName);
        void __fastcall VideoGrabberCreatePreallocFileCompleted(
          TObject *Sender, String FileName, bool Success);
        void __fastcall VideoGrabberCopyPreallocDataStarted(
          TObject *Sender, String SourceFile, String DestFile);
		void __fastcall VideoGrabberCopyPreallocDataCompleted(
          TObject *Sender, String SourceFile, String DestFile,
		  bool Success);
        void __fastcall VideoGrabberReencodingStarted(TObject *Sender,
          String SourceFile, String DestFile);
        void __fastcall VideoGrabberReencodingCompleted(TObject *Sender,
		  String SourceFile, String DestFile, bool Success);
		void __fastcall VideoGrabberInactive(TObject *Sender);
		void __fastcall btnStartRecordingImmediatelyClick(TObject *Sender);
		void __fastcall btnStartRecordingControlledClick(TObject *Sender);
		void __fastcall VideoGrabberRecordingReadyToStart(TObject *Sender);
		void __fastcall chkPreserveNativeFormatClick(TObject *Sender);
		void __fastcall chkSetLogoClick(TObject *Sender);
        void __fastcall cboVideoSourceChange(TObject *Sender);
		void __fastcall chkScreenRecordingWithCursorClick(TObject *Sender);
        void __fastcall chkChangeCursorClick(TObject *Sender);
        void __fastcall chkAudioRecordingClick(TObject *Sender);
        void __fastcall tbrAudioVolumeChange(TObject *Sender);
        void __fastcall tbrAudioBalanceChange(TObject *Sender);
        void __fastcall chkMuteAudioRenderingClick(TObject *Sender);
        void __fastcall btnRenewRecordingFileClick(TObject *Sender);
        void __fastcall VideoGrabberRecordingStarted(TObject *Sender,
          String FileName);
        void __fastcall VideoGrabberCopyPreallocDataProgress(
          TObject *Sender, int Percent, Int64 Position, Int64 Duration);
        void __fastcall VideoGrabberRecordingCompleted(TObject *Sender,
          String FileName, bool Success);
        void __fastcall VideoGrabberReencodingProgress(TObject *Sender,
          int Percent, Int64 Position, Int64 Duration);
        void __fastcall VideoGrabberCreatePreallocFileProgress(
		  TObject *Sender, int Percent, Int64 Position, Int64 Duration);
        void __fastcall edtVideoSourceFileOrUrlChange(TObject *Sender);
		void __fastcall btnNativeFormatClick(TObject *Sender);
        void __fastcall chkMultiplexedAutoSwitchClick(TObject *Sender);
		void __fastcall chkMultiplexedInputEmulationClick(TObject *Sender);
        void __fastcall chkRecordingPauseCreatesNewFileClick(
		  TObject *Sender);
		void __fastcall rdgVUMetersClick(TObject *Sender);
		void __fastcall btnDlgAudioDeviceClick(TObject *Sender);
        void __fastcall btnAboutFrameRateClick(TObject *Sender);
        void __fastcall btnStartAudioRenderingClick(TObject *Sender);
        void __fastcall btnStartWAVRecordingClick(TObject *Sender);
		void __fastcall btnStartMP3RecordingClick(TObject *Sender);
        void __fastcall btnStopAudioClick(TObject *Sender);
        void __fastcall chkUseAudioOfVideoCaptureDeviceClick(
          TObject *Sender);
		void __fastcall btnStartASFRecordingClick(TObject *Sender);
		void __fastcall chkRecordingOnMotionClick(TObject *Sender);
		void __fastcall btnUrlConfClick(TObject *Sender);
        void __fastcall btnSelectFileOrUrlClick(TObject *Sender);
        void __fastcall chkStretchPreferredVideoSizeClick(TObject *Sender);
        void __fastcall rdgRecordingSizeClick(TObject *Sender);
		void __fastcall btnScreenRecordingWindowClick(TObject *Sender);
		void __fastcall chkVideoVisibleWhenStoppedClick(TObject *Sender);
        void __fastcall VideoGrabberDVDiscontinuity(TObject *Sender,
          bool &DeliverNewFrame);
        void __fastcall rdg1AspectRatioClick(TObject *Sender);
		void __fastcall rdg2AspectRatioClick(TObject *Sender);
		void __fastcall cboAudioRenderersChange(TObject *Sender);
        void __fastcall chk1stVisibleClick(TObject *Sender);
		void __fastcall chk2ndVisibleClick(TObject *Sender);
        void __fastcall tbr1PanScanChange(TObject *Sender);
        void __fastcall tbr2PanScanChange(TObject *Sender);
		void __fastcall btnTestPanScanClick(TObject *Sender);
		void __fastcall edtMonitorNumberChange(TObject *Sender);
		void __fastcall btnMonitorNumberInfoClick(TObject *Sender);
		void __fastcall btnIPCamStartPreviewClick(TObject *Sender);
		void __fastcall btnIPCamAuthenticationInfoClick(TObject *Sender);
		void __fastcall btnCurrentIPCameraURLClick(TObject *Sender);
		void __fastcall VideoSourceFileOrUrl_UserNameChange(
          TObject *Sender);
        void __fastcall VideoSourceFileOrUrl_PasswordChange(
		  TObject *Sender);
        void __fastcall btnDemoURLClick(TObject *Sender);
		void __fastcall chkAdjustPixelAspectRatioClick(TObject *Sender);
        void __fastcall chkAudioSyncAdjustClick(TObject *Sender);
        void __fastcall tbrAudioSyncAdjustChange(TObject *Sender);
        void __fastcall chkAVISaveToThisFileClick(TObject *Sender);
        void __fastcall edtRecordingFileNameChange(TObject *Sender);
        void __fastcall edtRecordingTimerIntervalChange(TObject *Sender);
		void __fastcall grbRecordingTimerClick(TObject *Sender);
        void __fastcall VideoGrabberFrameProgress2(TObject *Sender,
          pFrameInfo FrameInfo);
        void __fastcall VideoGrabberFrameBitmap(TObject *Sender,
		  pFrameInfo FrameInfo, pFrameBitmapInfo BitmapInfo);
        void __fastcall btnPIPFromAnotherTVideoGrabberClick(
		  TObject *Sender);
        void __fastcall VideoGrabberRecordingPaused(TObject *Sender);
        void __fastcall Resetvideocapturedevicesettings1Click(
		  TObject *Sender);
		void __fastcall btnSaveVideoCompressorSettingsClick(
          TObject *Sender);
        void __fastcall btnReloadVideoCompressorSettingsClick(
          TObject *Sender);
        void __fastcall btnSaveAudioCompressorSettingsClick(
          TObject *Sender);
        void __fastcall btnReloadAudioCompressorSettingsClick(
		  TObject *Sender);
		void __fastcall chkBlackMagicDecklinkClick(TObject *Sender);
private:	// User declarations
		queue<BITMAP*>* bmpQueue;
		HANDLE hMutex;

		double goodTime;
		int goodFrames;
		double frameRate;
		double firstTimestamp, oldTimestamp, currentTimestamp;
		lsl_outlet outlet;




        int FStartupWidth;
        int FStartupHeight;

		TForm *FSnapForm;
        TImage *FSnapImage;
        TForm *FTransparentForm;
        TPanel *FTransparentPanel;
        int FThirdPartyFilterIndex;

        TRect FFreeHandMouseDrawing[CFreeHandMouseMax];
        int FiFreeHandMouseDrawing;
        bool FFreeHandMouseDrawingEnabled;

        Int64 FKBWritten_LastReportTime;

		bool FPickupTransparentColor_MustGetRGBPixel;
        int FPickupTransparentColor_LastRGBPixelValueAtMousePosition;
        int FPickupTransparentColor_MouseXVideoPosition;
        int FPickupTransparentColor_MouseYVideoPosition;

        int FOldVideoSignalState;


		void __fastcall RefreshDisplayOptions (void);
		void __fastcall RefreshDeviceControls (void);
		void __fastcall ShowFormAboveVideo (TForm *Form);
public:		// User declarations
		int xScene;
		int yScene;
		double dropped;
		int nFrames;
		double measuredFrameRate;
		double timestamp;
		int frameDivisor;

		__fastcall TCaptureWorkerForm(TComponent* Owner);
		void __fastcall TCaptureWorkerForm::InitVMR9Trackbar (TVMR9ImageAdjustment VMR9ImageAdjustment, TTrackBar *Trackbar);
		void SetQueue( queue<BITMAP*>& bmpQueue, HANDLE hMutex, lsl_outlet outlet, double requestedFrameRate);
};
//---------------------------------------------------------------------------
extern PACKAGE TCaptureWorkerForm *CaptureWorkerForm;
//---------------------------------------------------------------------------
#endif
