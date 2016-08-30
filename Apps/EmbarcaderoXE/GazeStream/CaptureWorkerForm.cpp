
//---------------------------------------------------------------------------
#include "useallegro4.h"
#include "winalleg.h"

#include <vcl.h>
#include <jpeg.hpp>
#include <shellapi.h>
#include <stdio.h>
#pragma hdrstop



#include "CaptureWorkerForm.h"
#include "UMainCapture.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "VidGrab"
#pragma resource "*.dfm"

#pragma warn -8057

TCaptureWorkerForm *CaptureWorkerForm;

   const String AUDIO_CLIPS = "*.mp3;*.wav;*.wma;*.mid";
   const String VIDEO_CLIPS = "*.mp*;*.vro;*.avi;*.asf;*.wmv;*.vob;*.avs;*.mov;*.divx;*.mp4;*.mts;*.3gp;*.swf;*.m2v;*.mkv;*.flv;*.wemb;*.ts;*.m4v;*.mp4v";
   const String IMAGE_FILES = "*.jpg;*.jpeg;*.jpe;*.bmp;*.gif;*.png";
   const String OPEN_MEDIA_FILES = "All media files|" + VIDEO_CLIPS + ";" + AUDIO_CLIPS + ";" + IMAGE_FILES
					+ "|Video clips|" + VIDEO_CLIPS
					+ "|Audio clips|" + AUDIO_CLIPS;
   const String OPEN_MEDIA_FILES_EXTENDED = OPEN_MEDIA_FILES
					+ "|Image files|" + IMAGE_FILES;
   const String OPEN_IMAGE_FILES = "Image files|*.bmp;*.jpg;*.gif;*.png;*.tif;*.tiff;*.wmf;*.emf;*.exf;*.jpe;*.jpeg";


//---------------------------------------------------------------------------
__fastcall TCaptureWorkerForm::TCaptureWorkerForm(TComponent* Owner)
		: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall AssignListToComboBox (TComboBox* ComboBox, String List, int Index)
{
   ComboBox->Items->Text = List;
   if (ComboBox->Items->Count > 0) {
	  ComboBox->ItemIndex = Index;
   }
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::FormCreate(TObject *Sender)
{
	xScene = -1000;
	yScene = -1000;
	VideoGrabber->Display_Visible = false;
   VideoGrabber->Display_AutoSize = true;    // the control will resize automatically
   VideoGrabber->AutoRefreshPreview = true;  // the preview will restart automatically when changing device properties (e.g. video size, analog video standard, etc...)

   FiFreeHandMouseDrawing = 0;                // used for free-hand mouse drawing
   FFreeHandMouseDrawingEnabled = false;      // used for free-hand mouse drawing

   FKBWritten_LastReportTime = 0;             // used to store the last time, to report periodically the KB written from the OnFrameProgress2 event (in this demo every 5 seconds)

   FStartupWidth = Width;
   FStartupHeight = Height;

   // - controls using global TVideoGrabber properties, e.g. like the VideoDevices list, are updated from the FormCreate event
   // - controls that depend of the current video device are set in the OnVideoDeviceSelected event
   // - controls that depend of the current audio device are set in the OnAudioDeviceSelected event


   ///////////////////////// Controls on main form
   edtStoragePath->Text = VideoGrabber->StoragePath;

   ///////////////////////// Video source tab
   AssignListToComboBox (cboVideoSource, VideoGrabber->VideoSources, VideoGrabber->VideoSource);
   btnAutoRefreshPreview->Checked = VideoGrabber->AutoRefreshPreview;
   cboVideoSource->ItemIndex = VideoGrabber->VideoSource;
   chkScreenRecordingWithCursor->Checked = VideoGrabber->ScreenRecordingWithCursor;
   edtMonitorNumber->Text = IntToStr (VideoGrabber->ScreenRecordingMonitor);
   chkMultiplexedInputEmulation->Checked = VideoGrabber->MultiplexedInputEmulation;
   VideoGrabber->VideoSource_FileOrURL = edtVideoSourceFileOrUrl->Text;

   ///////////////////////// Audio tab
   chkRenderAudioDevice->Checked = VideoGrabber->AudioDeviceRendering;
   chkMuteAudioRendering->Checked = VideoGrabber->MuteAudioRendering;
   chkAudioSyncAdjust->Checked = VideoGrabber->AudioSyncAdjustmentEnabled;
   tbrAudioSyncAdjust->Position = VideoGrabber->AudioSyncAdjustment;
   edtAudioSyncAdjust->Text = IntToStr (tbrAudioSyncAdjust->Position);

   ///////////////////////// RECORDING tab
   rdgRecordingMethod->ItemIndex = VideoGrabber->RecordingMethod;
   rdgRecordingSize->ItemIndex = VideoGrabber->RecordingSize;
   grbRecordingTimer->ItemIndex = VideoGrabber->RecordingTimer;
   edtPreallocSize->Text = IntToStr (VideoGrabber->PreallocCapFileSizeInMB);
   AssignListToComboBox (cboAudioFormats, VideoGrabber->AudioFormats, VideoGrabber->AudioFormat);
   chkRecordingCanPause->Checked = VideoGrabber->RecordingCanPause;
   if (VideoGrabber->RecordingFileName > "") {
      edtRecordingFileName->Text = VideoGrabber->RecordingFileName;
      chkAVISaveToThisFile->Checked = true;
   }
   chkRecordingPauseCreatesNewFile->Checked = VideoGrabber->RecordingPauseCreatesNewFile;
   chkPreallocFile->Checked = VideoGrabber->PreallocCapFileEnabled;
   chkPreserveNativeFormat->Checked = VideoGrabber->RecordingInNativeFormat;

   rdgCompressType->ItemIndex = VideoGrabber->CompressionType;
   rdgCompressMode->ItemIndex = VideoGrabber->CompressionMode;
   chkAudioRecording->Checked = VideoGrabber->AudioRecording;

   AssignListToComboBox (cboVideoCompressors, VideoGrabber->VideoCompressors, VideoGrabber->VideoCompressor);
   AssignListToComboBox(MainCaptureForm->cbVideoCodecs, VideoGrabber->VideoCompressors, VideoGrabber->VideoCompressor);//to MainCaputreForm
   AssignListToComboBox (cboAudioCompressors, VideoGrabber->AudioCompressors, VideoGrabber->AudioCompressor);

   ///////////////////////// Frame grabber tab
   rgdFrameGrabberFormat->ItemIndex = VideoGrabber->FrameGrabberRGBFormat;
   rgdFrameGrabber->ItemIndex = VideoGrabber->FrameGrabber;

   ///////////////////////// Display tab
   chkVideoVisibleWhenStopped->Checked = VideoGrabber->VideoVisibleWhenStopped;
   chkAdjustPixelAspectRatio->Checked = VideoGrabber->AdjustPixelAspectRatio;
   chkBorder->Checked = VideoGrabber->BorderStyle == bsSingle;
   chkTransparency->Checked = VideoGrabber->ColorKeyEnabled;
   rdgVideoRenderer->ItemIndex = VideoGrabber->VideoRenderer;
   RefreshDisplayOptions();
}
//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
//                            VIDEO DEVICE TAB
////////////////////////////////////////////////////////////////////////////////

void __fastcall TCaptureWorkerForm::VideoGrabberVideoDeviceSelected(
	  TObject *Sender)
{
   // from this event update the properties that depend of the current video capture device->
   // This event occurs when you assign a new value to the VideoDevice property->

   AssignListToComboBox (cboVideoSizes, VideoGrabber->VideoSizes, VideoGrabber->VideoSize);
   AssignListToComboBox (MainCaptureForm->cbVideoInputFormat, VideoGrabber->VideoSizes, VideoGrabber->VideoSize); //to UMainCapture
   AssignListToComboBox (cboVideoSubtypes, VideoGrabber->VideoSubtypes, VideoGrabber->VideoSubtype);
   AssignListToComboBox (cboAnalogVideoStandard, VideoGrabber->AnalogVideoStandards, VideoGrabber->AnalogVideoStandard);
   AssignListToComboBox (cboVideoInputs, VideoGrabber->VideoInputs, VideoGrabber->VideoInput);

   RefreshDeviceControls();
   RefreshDisplayOptions();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::cboVideoSourceChange(TObject *Sender)
{
  switch (cboVideoSource->ItemIndex) {
     case 0: VideoGrabber->VideoSource = vs_VideoCaptureDevice; break;
     case 1: VideoGrabber->VideoSource = vs_ScreenRecording;  break;
     case 2: VideoGrabber->VideoSource = vs_VideoFileOrURL; break;
	 case 3: VideoGrabber->VideoSource = vs_JPEGsOrBitmaps; break;
     case 4: VideoGrabber->VideoSource = vs_IPCamera; break;
     default:
		mmoLog->Lines->Add ("this option is not demonstrated in this demo");
  }
}

//---------------------------------------------------------------------------
void __fastcall TCaptureWorkerForm::cboVideoDevicesChange(TObject *Sender)
{
   VideoGrabber->VideoDevice = cboVideoDevices->ItemIndex;
   // when selecting another video device, the OnVideoDeviceSelected event occurs.
   // be sure to refresh all your device-dependent controls from this event.
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnStartPreviewClick(TObject *Sender)
{
   VideoGrabber->StartPreview();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnStopPreviewClick(TObject *Sender)
{
   VideoGrabber->StopPreview();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnPausePreviewClick(TObject *Sender)
{
   VideoGrabber->PausePreview();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnResumePreviewClick(TObject *Sender)
{
   VideoGrabber->ResumePreview();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnAutoRefreshPreviewClick(TObject *Sender)
{
   VideoGrabber->AutoRefreshPreview = btnAutoRefreshPreview->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::cboVideoInputsChange(TObject *Sender)
{
   VideoGrabber->VideoInput = cboVideoInputs->ItemIndex;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::cboVideoSizesChange(TObject *Sender)
{
   VideoGrabber->VideoSize = cboVideoSizes->ItemIndex;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::cboAnalogVideoStandardChange(TObject *Sender)
{
   VideoGrabber->AnalogVideoStandard = cboAnalogVideoStandard->ItemIndex;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::cboVideoSubtypesChange(TObject *Sender)
{
   VideoGrabber->VideoSubtype = cboVideoSubtypes->ItemIndex;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chkSetLogoClick(TObject *Sender)
{
   if (chkSetLogo->Checked) {
      imgLogo->Visible = false;
      VideoGrabber->SetLogoFromTImage (imgLogo);
      VideoGrabber->LogoLayout = lg_Stretched;
      VideoGrabber->LogoDisplayed = true;
   }
   else {
      VideoGrabber->LogoDisplayed = false;
   }
}
//---------------------------------------------------------------------------
void __fastcall TCaptureWorkerForm::btnDeviceDialogClick(TObject *Sender)
{
   VideoGrabber->ShowDialog (dlg_VideoDevice);
   RefreshDeviceControls();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnStreamDialogClick(TObject *Sender)
{
   // WARNING: USUALLY, THIS DIALOG ALLOWS TO CHANGE ONLY 1 PARAMETER AT THE SAME TIME
   VideoGrabber->ShowDialog (dlg_StreamConfig);
   VideoGrabberVideoDeviceSelected (this); // to refresh the controls, if (required
}
//---------------------------------------------------------------------------


void __fastcall TCaptureWorkerForm::rdgPreferredVideoSizeClick(TObject *Sender)
{
   switch (rdgPreferredVideoSize->ItemIndex) {
      case  0: VideoGrabber->UseNearestVideoSize (0, 0, chkStretchPreferredVideoSize->Checked); break; // disables the nearest size feature, uses the device"s VideoSize index in the VideoSizes list
      case  1: VideoGrabber->UseNearestVideoSize ( 88,  60, chkStretchPreferredVideoSize->Checked); break;
      case  2: VideoGrabber->UseNearestVideoSize ( 88,  72, chkStretchPreferredVideoSize->Checked); break;
      case  3: VideoGrabber->UseNearestVideoSize (180, 120, chkStretchPreferredVideoSize->Checked); break;
      case  4: VideoGrabber->UseNearestVideoSize (180, 144, chkStretchPreferredVideoSize->Checked); break;
      case  5: VideoGrabber->UseNearestVideoSize (320, 240, chkStretchPreferredVideoSize->Checked); break;
      case  6: VideoGrabber->UseNearestVideoSize (352, 288, chkStretchPreferredVideoSize->Checked); break;
      case  7: VideoGrabber->UseNearestVideoSize (640, 480, chkStretchPreferredVideoSize->Checked); break;
      case  8: VideoGrabber->UseNearestVideoSize (720, 480, chkStretchPreferredVideoSize->Checked); break;
      case  9: VideoGrabber->UseNearestVideoSize (720, 576, chkStretchPreferredVideoSize->Checked); break;
      case 10: VideoGrabber->UseNearestVideoSize (768, 576, chkStretchPreferredVideoSize->Checked); break;
      // you can add your own preferred formats
   }
   cboVideoSizes->Enabled = rdgPreferredVideoSize->ItemIndex == 0;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chkStretchPreferredVideoSizeClick(
      TObject *Sender)
{
   rdgPreferredVideoSizeClick (this);
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnSelectFileOrUrlClick(TObject *Sender)
{
   TOpenDialog *OpenDlg = new TOpenDialog(NULL);
   OpenDlg->Filter = OPEN_MEDIA_FILES;
   if (OpenDlg->Execute()) {
      edtVideoSourceFileOrUrl->Text = OpenDlg->FileName;
   }
   delete (OpenDlg);
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnUrlConfClick(TObject *Sender)
{
   VideoGrabber->ShowDialog (dlg_NetShowConfig);
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnFrameRateClick(TObject *Sender)
{
   float Value;

   Value = StrToFloatDef (edtFrameRate->Text, -1);
   VideoGrabber->FrameRate = Value;
   if (VideoGrabber->CurrentState == cs_Preview) {
	  mmoLog->Lines->Add ("current frame rate: " + FormatFloat ("0.00", VideoGrabber->CurrentFrameRate) + " fps.");
   }
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnAboutFrameRateClick(TObject *Sender)
{
   ShowMessage ("0 means that the default frame rate of the device will be used.\nAfter the OnPreviewStarted or OnRecordingStarted event occurs,\nits value is available in the CurrentFrameRate property.");
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chkMultiplexedAutoSwitchClick(
	  TObject *Sender)
{
   if (chkMultiplexedAutoSwitch->Checked) {
      VideoGrabber->MultiplexedRole = mr_MultiplexedMosaic4;
   }
   else {
      VideoGrabber->MultiplexedRole = mr_NotMultiplexed;
   }
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chkMultiplexedInputEmulationClick(
      TObject *Sender)
{
   VideoGrabber->MultiplexedInputEmulation = chkMultiplexedInputEmulation->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chkReduceDVFrameRateClick(TObject *Sender)
{
   VideoGrabber->DVReduceFrameRate = chkReduceDVFrameRate->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::edtVideoSourceFileOrUrlChange(
      TObject *Sender)
{
   VideoGrabber->VideoSource_FileOrURL = edtVideoSourceFileOrUrl->Text;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::VideoSourceFileOrUrl_UserNameChange(TObject *Sender)
{
   VideoGrabber->SetAuthentication (at_StreamingUrl, VideoSourceFileOrUrl_UserName->Text, VideoSourceFileOrUrl_Password->Text);
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::VideoSourceFileOrUrl_PasswordChange(TObject *Sender)
{
   VideoGrabber->SetAuthentication (at_StreamingUrl, VideoSourceFileOrUrl_UserName->Text, VideoSourceFileOrUrl_Password->Text);
}
//---------------------------------------------------------------------------


void __fastcall TCaptureWorkerForm::btnDemoURLClick(TObject *Sender)
{
   ShellExecute(0, "open", "http://www.datastead.com/demo/StreamingURL.htm", "", "", SW_SHOW);
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnScreenRecordingWindowClick(
      TObject *Sender)
{
   VideoGrabber->SetWindowRecordingByName (edtScreenRecordingWindow->Text, false); // set to "true" for an exact and case-sensitive match
}

//---------------------------------------------------------------------------
void __fastcall TCaptureWorkerForm::chkScreenRecordingWithCursorClick(
      TObject *Sender)
{
   VideoGrabber->ScreenRecordingWithCursor = chkScreenRecordingWithCursor->Checked;
}
//---------------------------------------------------------------------------


void __fastcall TCaptureWorkerForm::edtMonitorNumberChange(TObject *Sender)
{
   VideoGrabber->ScreenRecordingMonitor = StrToIntDef (edtMonitorNumber->Text, 0);
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnMonitorNumberInfoClick(TObject *Sender)
{
   ShowMessage ("-1 = whole desktop (across monitors)    0 = 1st monitor     1 = 2nd monitor     etc...");
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::VideoGrabberPreviewStarted(TObject *Sender)
{
   mmoLog->Lines->Add ("");
   mmoLog->Lines->Add ("video source size: " + IntToStr (VideoGrabber->VideoWidth) + "x" + IntToStr (VideoGrabber->VideoHeight));
   mmoLog->Lines->Add ("display aspect ratio: " + IntToStr (VideoGrabber->VideoWidth_PreferredAspectRatio) + "x" + IntToStr (VideoGrabber->VideoHeight_PreferredAspectRatio));
   mmoLog->Lines->Add ("current frame rate: " + FormatFloat ("0.00", VideoGrabber->CurrentFrameRate) + " fps.");
   mmoLog->Lines->Add ("");
}
//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
//                           AUDIO TAB
////////////////////////////////////////////////////////////////////////////////

void __fastcall TCaptureWorkerForm::VideoGrabberAudioDeviceSelected(
      TObject *Sender)
{
   // from this event set the parameters that change when the current audio capture device change

   cboAudioDevices->ItemIndex = VideoGrabber->AudioDevice; // be sure the listbox index matches the current index value

   AssignListToComboBox (cboAudioInputs, VideoGrabber->AudioInputs, VideoGrabber->AudioInput);
   tbrAudioInputLevel->Position = VideoGrabber->AudioInputLevel;
   tbrAudioInputBalance->Position = VideoGrabber->AudioInputBalance;
   chkAudioInputMono->Checked = VideoGrabber->AudioInputMono;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnStartAudioRenderingClick(TObject *Sender)
{
   VideoGrabber->StartAudioRendering();
   if (! VideoGrabber->AudioDeviceRendering) {
      if (btnStartAudioRendering->Tag == 0) {
         btnStartAudioRendering->Tag = 1; // to show the popup only one time
         ShowMessage ("StartAudioRendering acts as 'preview', e.g. for the VU-Meters, if AudioDeviceRendering (on the left) is not enabled, the audio will not be heard in the speakers");
      }
   }
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnStartWAVRecordingClick(TObject *Sender)
{
   VideoGrabber->RecordingMethod = rm_AVI;
   VideoGrabber->CompressionMode = cm_NoCompression;
   VideoGrabber->StartAudioRecording();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnStartMP3RecordingClick(TObject *Sender)
{
   VideoGrabber->RecordingMethod = rm_AVI;
   VideoGrabber->CompressionMode = cm_CompressOnTheFly;
   VideoGrabber->CompressionType = ct_Audio;
   VideoGrabber->AudioCompressor = VideoGrabber->FindIndexInListByName (VideoGrabber->AudioCompressors, "LAME Audio Encoder", false, true);
   if (VideoGrabber->AudioCompressor == -1) {
      ShowMessage ("The LAME Audio Encoder is required, download it from http://www.elecard.com/download/");
   }
   else {
	  VideoGrabber->StartAudioRecording();
   }
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnStartASFRecordingClick(TObject *Sender)
{
   VideoGrabber->CompressionMode = cm_NoCompression;
   VideoGrabber->RecordingMethod = rm_ASF;
   VideoGrabber->StartAudioRecording();
}
//---------------------------------------------------------------------------
void __fastcall TCaptureWorkerForm::btnStopAudioClick(TObject *Sender)
{
   VideoGrabber->Stop();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chkUseAudioOfVideoCaptureDeviceClick(
      TObject *Sender)
{
   if (chkUseAudioOfVideoCaptureDevice->Checked) {
      VideoGrabber->VideoDevice = 0; // in this demo, by default we choose the 1st
   }
   else {
      VideoGrabber->VideoDevice = -1; // this deactivates the video capture device
   }
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chkRenderAudioDeviceClick(TObject *Sender)
{
   VideoGrabber->AudioDeviceRendering = chkRenderAudioDevice->Checked;

}
//---------------------------------------------------------------------------
void __fastcall TCaptureWorkerForm::cboAudioDevicesChange(TObject *Sender)
{
   VideoGrabber->AudioDevice = cboAudioDevices->ItemIndex;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnDlgAudioDeviceClick(TObject *Sender)
{
   VideoGrabber->ShowDialog (dlg_AudioDevice);
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::cboAudioInputsChange(TObject *Sender)
{
   VideoGrabber->AudioInput = cboAudioInputs->ItemIndex;
}
//---------------------------------------------------------------------------


void __fastcall TCaptureWorkerForm::tbrAudioInputLevelChange(TObject *Sender)
{
   VideoGrabber->AudioInputLevel = tbrAudioInputLevel->Position;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::tbrAudioInputBalanceChange(TObject *Sender)
{
   VideoGrabber->AudioInputBalance = tbrAudioInputBalance->Position;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chkAudioInputMonoClick(TObject *Sender)
{
   VideoGrabber->AudioInputMono = chkAudioInputMono->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::cboAudioRenderersChange(TObject *Sender)
{
   VideoGrabber->AudioRenderer = cboAudioRenderers->ItemIndex;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::tbrAudioVolumeChange(TObject *Sender)
{
   VideoGrabber->AudioVolume = tbrAudioVolume->Position;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::tbrAudioBalanceChange(TObject *Sender)
{
   VideoGrabber->AudioBalance = tbrAudioBalance->Position;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chkMuteAudioRenderingClick(TObject *Sender)
{
   VideoGrabber->MuteAudioRendering = chkMuteAudioRendering->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chkAudioSyncAdjustClick(TObject *Sender)
{
   VideoGrabber->AudioSyncAdjustmentEnabled = chkAudioSyncAdjust->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::tbrAudioSyncAdjustChange(TObject *Sender)
{
   VideoGrabber->AudioSyncAdjustment = tbrAudioSyncAdjust->Position;
   edtAudioSyncAdjust->Text = IntToStr (tbrAudioSyncAdjust->Position);
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::rdgVUMetersClick(TObject *Sender)
{
   int VU_LEFT = 0;
   int VU_RIGHT = 1;

   switch (rdgVUMeters->ItemIndex) {
      case 0: {
         VideoGrabber->VuMeter = vu_Disabled;
      }; break;
      case 1: {
         VideoGrabber->VuMeter = vu_Analog;

         pnLeftVUMeter->Width = (pnLeftVUMeter->Height * 4) / 3;
         pnRightVUMeter->Width = (pnRightVUMeter->Height * 4) / 3;

         VideoGrabber->SetVUMeterSetting (VU_LEFT, vu_BkgndColor, clBlack);
         VideoGrabber->SetVUMeterSetting (VU_RIGHT, vu_BkgndColor, clBlack);
      }; break;
      case 2: {
         VideoGrabber->VuMeter = vu_Bargraph;

         pnLeftVUMeter->Width = (pnLeftVUMeter->Height * 2) / 10;
         pnRightVUMeter->Width = (pnRightVUMeter->Height * 2) / 10;

         VideoGrabber->SetVUMeterSetting (VU_LEFT, vu_BkgndColor, TColor (0x404040));
         VideoGrabber->SetVUMeterSetting (VU_RIGHT, vu_BkgndColor, TColor (0x404040));
      }; break;
   }
   if (VideoGrabber->VuMeter != vu_Disabled) {
      VideoGrabber->SetVUMeterSetting (VU_LEFT, vu_Handle, (unsigned int) pnLeftVUMeter->Handle);   // we use a TPanel to display the VU meters,
      VideoGrabber->SetVUMeterSetting (VU_RIGHT, vu_Handle, (unsigned int) pnRightVUMeter->Handle); // this is activated by assigning the TPanel's handle->->->
   }
   else {
      VideoGrabber->SetVUMeterSetting (VU_LEFT, vu_Handle, 0); // ->->->and the TPanel is freed by passing 0 as handle
      VideoGrabber->SetVUMeterSetting (VU_RIGHT, vu_Handle, 0);
   }

}
//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
//                             RECORDING TAB
////////////////////////////////////////////////////////////////////////////////

void __fastcall TCaptureWorkerForm::btnStartRecordingImmediatelyClick(
      TObject *Sender)
{
   VideoGrabber->HoldRecording = false; // let's start recording as soon as the graph is built.
   VideoGrabber->StartRecording();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnStartRecordingControlledClick(
      TObject *Sender)
{
   VideoGrabber->HoldRecording = true;
   VideoGrabber->StartRecording();

   // Enabling HoldRecording prevents the recording to start when the recording graph is built.
   // after the OnRecordingReadyToStart event occurs, you can accurately start the
   // recording by invoking ResumeRecording.
}

//---------------------------------------------------------------------------
void __fastcall TCaptureWorkerForm::btnStopRecordingClick(TObject *Sender)
{
   VideoGrabber->StopRecording();
}

//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnRenewRecordingFileClick(TObject *Sender)
{
   if (chkAVISaveToThisFile->Checked) {
      VideoGrabber->RecordToNewFileNow (edtRecordingFileName->Text, true);
   }
   else {
      VideoGrabber->RecordToNewFileNow ("", true);
   }
}

//---------------------------------------------------------------------------
void __fastcall TCaptureWorkerForm::rdgRecordingMethodClick(TObject *Sender)
{
   VideoGrabber->RecordingMethod = TRecordingMethod (rdgRecordingMethod->ItemIndex);
   RefreshDeviceControls();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::rdgRecordingSizeClick(TObject *Sender)
{
   VideoGrabber->RecordingSize = TRecordingSize (rdgRecordingSize->ItemIndex);
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chkRecordingCanPauseClick(TObject *Sender)
{
   VideoGrabber->RecordingCanPause = chkRecordingCanPause->Checked;
   RefreshDeviceControls();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chkRecordingPauseCreatesNewFileClick(
      TObject *Sender)
{
   VideoGrabber->RecordingPauseCreatesNewFile = chkRecordingPauseCreatesNewFile->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chkRecordingOnMotionClick(TObject *Sender)
{
   VideoGrabber->RecordingOnMotion_Enabled = chkRecordingOnMotion->Checked;
   chkRecordingCanPause->Checked = chkRecordingOnMotion->Checked;
}

//---------------------------------------------------------------------------
void __fastcall TCaptureWorkerForm::btnPauseRecordingClick(TObject *Sender)
{
   VideoGrabber->PauseRecording();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnResumeRecordingClick(TObject *Sender)
{
   VideoGrabber->ResumeRecording();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chkAVISaveToThisFileClick(TObject *Sender)
{
   if (chkAVISaveToThisFile->Checked) {
	  VideoGrabber->RecordingFileName = edtRecordingFileName->Text;
   }
   else {
      VideoGrabber->RecordingFileName = "";
   }
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::edtRecordingFileNameChange(TObject *Sender)
{
   if (chkAVISaveToThisFile->Checked) {
	  VideoGrabber->RecordingFileName = edtRecordingFileName->Text;
   }
}

void __fastcall TCaptureWorkerForm::cboAudioFormatsChange(TObject *Sender)
{
   VideoGrabber->AudioFormat = TAudioFormat (cboAudioFormats->ItemIndex);
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chkAudioRecordingClick(TObject *Sender)
{
   VideoGrabber->AudioRecording = chkAudioRecording->Checked;
   RefreshDeviceControls();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::grbRecordingTimerClick(TObject *Sender)
{
   VideoGrabber->RecordingTimer = TRecordingTimer (grbRecordingTimer->ItemIndex);
   VideoGrabber->RecordingTimerInterval = StrToIntDef (edtRecordingTimerInterval->Text, 0);
}

//---------------------------------------------------------------------------
void __fastcall TCaptureWorkerForm::edtRecordingTimerIntervalChange(
      TObject *Sender)
{
   VideoGrabber->RecordingTimerInterval = StrToIntDef (edtRecordingTimerInterval->Text, 0);
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chkPreserveNativeFormatClick(TObject *Sender)
{
   VideoGrabber->RecordingInNativeFormat = chkPreserveNativeFormat->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnNativeFormatClick(TObject *Sender)
{
   String Msg;

   Msg += "Means that the recording stream will not be uncompressed.\n";
   Msg += "\n";
   Msg += "MUST BE ENABLED:\n";
   Msg += "- to record hardware-encoded MPEG\n";
   Msg += "- to record or DV devices in native DV format\n";
   Msg += "- to record the DV date/time into the video clip and retrieve it during playback\n";
   Msg += "\n";
   Msg += "MUST BE DISABLED:\n";
   Msg += "- to record the text and graphics overlays into the video clip, and retrieve them during playback\n";
   ShowMessage (Msg);
}
//---------------------------------------------------------------------------
void __fastcall TCaptureWorkerForm::chkPreallocFileClick(TObject *Sender)
{
   VideoGrabber->PreallocCapFileEnabled = chkPreallocFile->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnCreatePreallocFileNowClick(
      TObject *Sender)
{
   VideoGrabber->CreatePreallocCapFile();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::edtPreallocSizeChange(TObject *Sender)
{
   VideoGrabber->PreallocCapFileSizeInMB = StrToIntDef (edtPreallocSize->Text, 100);
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::cboVideoCompressorsChange(TObject *Sender)
{
   VideoGrabber->VideoCompressor = cboVideoCompressors->ItemIndex;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::cboAudioCompressorsChange(TObject *Sender)
{
   VideoGrabber->AudioCompressor = cboAudioCompressors->ItemIndex;
}
//---------------------------------------------------------------------------

String GetRecordingMethodName (TRecordingMethod Value)
{
   switch (Value) {
      case rm_ASF         : return "rm_ASF";
      case rm_SendToDV    : return "rm_sDV";
      case rm_MKV         : return "rm_MKV";
      case rm_FLV         : return "rm_FLV";
	  case rm_MP4         : return "rm_MP4";
      case rm_Multiplexer : return "rm_Mux";
   default:
      return "rm_AVI";
   }
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnVideoCompressorSettingsClick(
      TObject *Sender)
{
   VideoGrabber->ShowDialog (dlg_VideoCompressor);
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnAudioCompressorSettingsClick(
      TObject *Sender)
{
   VideoGrabber->ShowDialog (dlg_AudioCompressor);
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnSaveVideoCompressorSettingsClick(
      TObject *Sender)
{
   TSaveDialog *SaveDlg = new TSaveDialog(NULL);

   SaveDlg->FileName = VideoGrabber->VideoCompressorName + " (" + GetRecordingMethodName (VideoGrabber->RecordingMethod) + ").txt";
   if (SaveDlg->Execute()) {

      // be sure to have selected the RecordingMethod and VideoCompressor before invoking SaveCompressorSettings...

      VideoGrabber->SaveCompressorSettingsToTextFile (TRUE, VideoGrabber->VideoCompressor, SaveDlg->FileName);

      // remark: you can use instead SaveCompressorSettingsToDataString (TRUE, VideoGrabber.VideoCompressor, YourDataString)
   }
   delete (SaveDlg);
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnSaveAudioCompressorSettingsClick(
      TObject *Sender)
{
   TSaveDialog *SaveDlg = new TSaveDialog(NULL);

   SaveDlg->FileName = VideoGrabber->AudioCompressorName + " (" + GetRecordingMethodName (VideoGrabber->RecordingMethod) + ").txt";
   if (SaveDlg->Execute()) {

      // be sure to have selected the RecordingMethod and AudioCompressor before invoking SaveCompressorSettings...

      VideoGrabber->SaveCompressorSettingsToTextFile (FALSE, VideoGrabber->AudioCompressor, SaveDlg->FileName);

      // remark: you can use instead SaveCompressorSettingsToDataString (FALSE, VideoGrabber.AudioCompressor, YourDataString)
   }
   delete (SaveDlg);
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnReloadVideoCompressorSettingsClick(
      TObject *Sender)
{
   TOpenDialog *OpenDlg = new TOpenDialog(NULL);
   OpenDlg->Filter = "Compressor saved settings (*.txt)|*.txt";
   if (OpenDlg->Execute()) {

      VideoGrabber->LoadCompressorSettingsFromTextFile (TRUE, VideoGrabber->VideoCompressor, OpenDlg->FileName);

      // remark: you can use instead YourDataString := LoadCompressorSettingsFromDataString (TRUE, VideoGrabber->VideoCompressor)
   }
   delete (OpenDlg);
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnReloadAudioCompressorSettingsClick(
      TObject *Sender)
{
   TOpenDialog *OpenDlg = new TOpenDialog(NULL);
   OpenDlg->Filter = "Compressor saved settings (*.txt)|*.txt";
   if (OpenDlg->Execute()) {

      VideoGrabber->LoadCompressorSettingsFromTextFile (FALSE, VideoGrabber->AudioCompressor, OpenDlg->FileName);

      // remark: you can use instead YourDataString := LoadCompressorSettingsFromDataString (FALSE, VideoGrabber->AudioCompressor)
   }
   delete (OpenDlg);
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::rdgCompressModeClick(TObject *Sender)
{
   VideoGrabber->CompressionMode = TCompressionMode (rdgCompressMode->ItemIndex);
   RefreshDeviceControls();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::rdgCompressTypeClick(TObject *Sender)
{
   VideoGrabber->CompressionType = TCompressionType (rdgCompressType->ItemIndex);
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::VideoGrabberRecordingReadyToStart(
      TObject *Sender)
{
   if (MessageDlg ("Ready to start recording!", mtInformation, TMsgDlgButtons() << mbOK << mbCancel, 0) == mrOk) {
      VideoGrabber->ResumeRecording();
   }
   else {
      VideoGrabber->StopRecording();
   }
}

void __fastcall TCaptureWorkerForm::VideoGrabberRecordingStarted(TObject *Sender,
	  String FileName)
{
   mmoLog->Lines->Add ("recording to " + FileName + ":");
   mmoLog->Lines->Add (IntToStr (VideoGrabber->RecordingWidth) + "x" + IntToStr (VideoGrabber->RecordingHeight));
   mmoLog->Lines->Add ("current frame rate: " + FormatFloat ("0.00", VideoGrabber->CurrentFrameRate) + " fps.");
   btnRenewRecordingFile->Enabled = true;
   FKBWritten_LastReportTime = 0;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::VideoGrabberRecordingPaused(TObject *Sender)
{
      mmoLog->Lines->Add ("recording paused...");

}

//---------------------------------------------------------------------------
void __fastcall TCaptureWorkerForm::VideoGrabberRecordingCompleted(
      TObject *Sender, String FileName, bool Success)
{
   if (Success) {
      mmoLog->Lines->Add (FileName + " successfully created.");
      mmoLog->Lines->Add ("size: " + IntToStr ((int) VideoGrabber->RecordingKBytesWrittenToDisk()) + " KB");
   }
   else {
      mmoLog->Lines->Add ("recording failed.");
   }
   btnRenewRecordingFile->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::VideoGrabberReencodingStarted(
      TObject *Sender, String SourceFile, String DestFile)
{
   mmoLog->Lines->Add ("reencoding " + SourceFile + " to " + DestFile + "...");
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::VideoGrabberReencodingProgress(
      TObject *Sender, int Percent, Int64 Position, Int64 Duration)
{
   mmoLog->Lines->Add (IntToStr (Percent) + "% completed...");
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::VideoGrabberReencodingCompleted(
      TObject *Sender, String SourceFile, String DestFile,
      bool Success)
{
   if (Success) {
      mmoLog->Lines->Add (SourceFile + "successfully reencoded to " + DestFile);
   }
   else {
      mmoLog->Lines->Add ("ERROR: failed to reencode " + SourceFile + " to " + DestFile);
   }
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::VideoGrabberCreatePreallocFileStarted(
      TObject *Sender, String FileName)
{
   mmoLog->Lines->Add ("preallocating file " + FileName + "...");
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::VideoGrabberCreatePreallocFileProgress(
      TObject *Sender, int Percent, Int64 Position, Int64 Duration)
{
   mmoLog->Lines->Add (IntToStr (Percent) + "% of preallocated file completed");
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::VideoGrabberCreatePreallocFileCompleted(
      TObject *Sender, String FileName, bool Success)
{
   if (Success) {
      mmoLog->Lines->Add (FileName + "successfully preallocated.");
   }
   else {
      mmoLog->Lines->Add ("Failed to allocate " + FileName);
   }
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::VideoGrabberCopyPreallocDataStarted(
      TObject *Sender, String SourceFile, String DestFile)
{
   mmoLog->Lines->Add ("copying data from " + SourceFile + " to " + DestFile + "...");
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::VideoGrabberCopyPreallocDataProgress(
      TObject *Sender, int Percent, Int64 Position, Int64 Duration)
{
   mmoLog->Lines->Add ("copying data: " + IntToStr (Percent) + "% completed.");
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::VideoGrabberCopyPreallocDataCompleted(
      TObject *Sender, String SourceFile, String DestFile,
      bool Success)
{
   if (Success) {
      mmoLog->Lines->Add (SourceFile + " successfully copied to " + DestFile);
   }
   else {
      mmoLog->Lines->Add ("ERROR: failed to copy " + SourceFile + " to " + DestFile);
   }

}
//---------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////////////
//                             FRAME GRABBER TAB
////////////////////////////////////////////////////////////////////////////////


void __fastcall TCaptureWorkerForm::rgdFrameGrabberClick(TObject *Sender)
{
   VideoGrabber->FrameGrabber = TFrameGrabber (rgdFrameGrabber->ItemIndex);
   RefreshDeviceControls();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::rgdFrameGrabberFormatClick(TObject *Sender)
{
   VideoGrabber->FrameGrabberRGBFormat = TFrameGrabberRGBFormat (rgdFrameGrabberFormat->ItemIndex);
}
//---------------------------------------------------------------------------


void __fastcall TCaptureWorkerForm::VideoGrabberFrameProgress2(TObject *Sender,
	  pFrameInfo FrameInfo)
{
   String DVTimeCode;
   String DVDateTime;

   TVarRec FrameTimeVars[] = {FrameInfo->FrameTime_Hour, FrameInfo->FrameTime_Min, FrameInfo->FrameTime_Sec, FrameInfo->FrameTime_Hs};
//	printf("dropped: %d\n", FrameInfo->DroppedFrameCount);
//		printf("frame: %d\n", FrameInfo->FrameNumber);
   if (FrameInfo->CurrentState == cs_Playback) {  // no dropped frames during playback
      edtFrameCount->Text = "frame: " + IntToStr (FrameInfo->FrameNumber) + " time:" + Format ("%.2d:%.2d:%.2d %.2d", FrameTimeVars, (ARRAYSIZE(FrameTimeVars) -1));
   }
   else {
      edtFrameCount->Text = "frame: " + IntToStr (FrameInfo->FrameNumber) + " dropped:" + IntToStr (FrameInfo->DroppedFrameCount) + " time:" + Format ("%.2d:%.2d:%.2d %.2d", FrameTimeVars, (ARRAYSIZE(FrameTimeVars) -1));
   }

   if (FrameInfo->DVTimeCode_IsAvailable) {
	  TVarRec TimeCodeVars[] = {FrameInfo->DVTimeCode_Hour, FrameInfo->DVTimeCode_Min, FrameInfo->DVTimeCode_Sec, FrameInfo->DVTimeCode_Ff};
      DVTimeCode = Format ("tc: %.2d:%.2d:%.2d %.2d", TimeCodeVars, (ARRAYSIZE(TimeCodeVars) -1));
   }
   else {
       DVTimeCode = "";
   }

   if (FrameInfo->DVDateTime_IsAvailable) {
      TVarRec DVDateTimeVars[] = {FrameInfo->DVDateTime_Day, FrameInfo->DVDateTime_Month, FrameInfo->DVDateTime_Year, FrameInfo->DVDateTime_Hour, FrameInfo->DVDateTime_Min, FrameInfo->DVDateTime_Sec};
       DVDateTime = Format ("dv date: %.2d/%.2d/%.2d %.2d:%.2d:%.2d", DVDateTimeVars, (ARRAYSIZE(DVDateTimeVars) -1));
   }
   else {
       DVDateTime = "";
   }
   edtDVDateTime->Text = DVTimeCode + " " + DVDateTime;

   if (VideoGrabber->CurrentState == cs_Recording) {
      if ((FrameInfo->FrameTime - FKBWritten_LastReportTime) > 50000000) { // 5 seconds
         FKBWritten_LastReportTime = FrameInfo->FrameTime;
         mmoLog->Lines->Add ("written to disk: " + IntToStr ((int) VideoGrabber->RecordingKBytesWrittenToDisk()) + " KB");
      }
   }
}
//---------------------------------------------------------------------------

void CopyFrameToBitmapDemo (pFrameBitmapInfo BitmapInfo)
{
   // this code shows how to make a copy of the current video frame to an Image, invoke it from the OnFrameBitmap event below

   Graphics::TBitmap *NewBitmap = new Graphics::TBitmap();
   NewBitmap->Canvas->Lock();
   NewBitmap->Width = BitmapInfo->BitmapWidth;
   NewBitmap->Height = BitmapInfo->BitmapHeight;
   switch (BitmapInfo->BitmapBitsPerPixel) {
      case 32: NewBitmap->PixelFormat = pf32bit; break;
      case 24: NewBitmap->PixelFormat = pf24bit; break;
      case 16: NewBitmap->PixelFormat = pf16bit; break;
      case 15: NewBitmap->PixelFormat = pf15bit; break;
      case  8: NewBitmap->PixelFormat =  pf8bit; break;
      default: NewBitmap->PixelFormat = pf24bit; break;
   }
   BitBlt (NewBitmap->Canvas->Handle, 0, 0, BitmapInfo->BitmapWidth, BitmapInfo->BitmapHeight, BitmapInfo->BitmapDC, 0, 0, SRCCOPY);
   NewBitmap->Canvas->Unlock();
   NewBitmap->SaveToFile ("c:\\myfolder\\myframecapture.bmp");
   delete (NewBitmap);
}
//---------------------------------------------------------------------------

void DrawCircle (TCanvas *Canvas, int xCenter, int yCenter, int Radius)
{
   int xCenterAndRadius = xCenter + Radius;
   Canvas->Arc (xCenter - Radius, yCenter - Radius, xCenterAndRadius, yCenter + Radius, xCenterAndRadius, xCenter, xCenterAndRadius, xCenter);
}
//---------------------------------------------------------------------------


void TCaptureWorkerForm::SetQueue( queue<BITMAP*>& bmpQueue, HANDLE hMutex) {
	this->bmpQueue= &bmpQueue;
	this->hMutex = hMutex;

}

void __fastcall TCaptureWorkerForm::VideoGrabberFrameBitmap(TObject *Sender,
	  pFrameInfo FrameInfo, pFrameBitmapInfo BitmapInfo)
{
	if(xScene > -1000 && yScene > -1000) {
		int TargetRadius = 10;
		 TCanvas *Canvas = new TCanvas();

		 Canvas->Handle = BitmapInfo->BitmapDC;
		 /*
		 Canvas->Pen->Color = clRed;
		 DrawCircle (Canvas, xScene, yScene, TargetRadius);
		 Canvas->MoveTo (xScene - TargetRadius, yScene);
		 Canvas->LineTo (xScene + TargetRadius, yScene);
		 Canvas->MoveTo (xScene, yScene - TargetRadius);
		 Canvas->LineTo (xScene, yScene + TargetRadius);
		 */

		 //draw cross
		 Canvas->Brush->Color = clBlack;
		 TRect *rect = new TRect(xScene-3,yScene-TargetRadius,xScene+3,yScene+TargetRadius);
		 Canvas->FillRect(*rect);

		 rect->left = xScene-TargetRadius;
		 rect->top =  yScene-3;
		 rect->right = xScene+TargetRadius;
		 rect->bottom = yScene+3;
		 Canvas->FillRect(*rect);


		 Canvas->Brush->Color = clWhite;

		 rect->left = xScene-1;
		 rect->top = yScene-TargetRadius;
		 rect->right = xScene+1;
		 rect->bottom = yScene+TargetRadius;

		 Canvas->FillRect(*rect);

		 rect->left = xScene-TargetRadius;
		 rect->top =  yScene-1;
		 rect->right = xScene+TargetRadius;
		 rect->bottom = yScene+1;
		 Canvas->FillRect(*rect);

		 delete (rect);
		 delete (Canvas);

	}
//    From this event you should not perform any actions with the potential to block:
//    1. do not hold a critical section or wait on another thread,
//    2. do not call any GDI or USER32.DLL APIs that might cause a window to move,
//    3. do not invoke component properties that send messages, e.g. do not read the ItemIndex property
//       of a ListBox component. If you need to read a such property, set an intermediary variable when
//       the property changes, and then read the intermediary variable from this event.



//	static int c = 0;
//	c++;

//	printf("%d\n",c);
//	Edit1->Text = c;

	int height = BitmapInfo->BitmapHeight;
	int width = BitmapInfo->BitmapWidth;
	BITMAP * aBmp = create_bitmap_ex(24, width, height);
	unsigned int BitmapLinePtr = (unsigned int) BitmapInfo->BitmapDataPtr;

	if (BitmapInfo->BitmapBitsPerPixel == 24) {   // case where FrameGrabberRGBFormat is set to fgf_RGB24 (you can select it in the "frame grabber" tab)
		for (int i = 0 ; i < BitmapInfo->BitmapHeight ; i++) {
			TRGBTriple *RGB24Line = (TRGBTriple*) BitmapLinePtr;
			for (int iCol = 0 ; iCol < BitmapInfo->BitmapWidth ; iCol ++) {
				aBmp->line[height-1-i][(iCol)*3] =   RGB24Line[iCol].rgbtBlue;
				aBmp->line[height-1-i][(iCol)*3+1] = RGB24Line[iCol].rgbtGreen;
				aBmp->line[height-1-i][(iCol)*3+2] = RGB24Line[iCol].rgbtRed;
			}
			BitmapLinePtr += BitmapInfo->BitmapLineSize;
		 }
	}
	else if (BitmapInfo->BitmapBitsPerPixel == 32) {   // case where FrameGrabberRGBFormat is set to fgf_RGB32 (default setting) (you can select it in the "frame grabber" tab)
		for (int i = 0 ; i < height ; i++) {
			TRGBQuad *RGB32Line = (TRGBQuad*) BitmapLinePtr;
			for (int iCol = 0 ; iCol < width ; iCol ++) {
				aBmp->line[height-1-i][(iCol)*3] =  RGB32Line[iCol].rgbBlue;
				aBmp->line[height-1-i][(iCol)*3+1] = RGB32Line[iCol].rgbGreen;
				aBmp->line[height-1-i][(iCol)*3+2] = RGB32Line[iCol].rgbRed;

			}
			BitmapLinePtr += BitmapInfo->BitmapLineSize;
		}
	}

	unsigned int res = WaitForSingleObject(hMutex,INFINITE);
	if (res!=WAIT_OBJECT_0) {ReleaseMutex(hMutex);return;}
	__try
	{
		bmpQueue->push(aBmp);
	}
	__finally
	{
		ReleaseMutex(hMutex);
	}
	Sleep(2);



   // DIRECT ACCESS TO THE RGB PIXELS
   // ===============================

   if (false) {
	  unsigned int BitmapLinePtr = (unsigned int) BitmapInfo->BitmapDataPtr;

	  if (BitmapInfo->BitmapBitsPerPixel == 24) {   // case where FrameGrabberRGBFormat is set to fgf_RGB24 (you can select it in the "frame grabber" tab)
		 for (int i = 0 ; i < BitmapInfo->BitmapHeight ; i++) {
			TRGBTriple *RGB24Line = (TRGBTriple*) BitmapLinePtr;
			for (int iCol = 0 ; iCol < BitmapInfo->BitmapWidth ; iCol ++) {
			   RGB24Line[iCol].rgbtGreen = 0; // e.g. we remove the green color -> purple image
			}
			BitmapLinePtr += BitmapInfo->BitmapLineSize;
		 }
	  }
	  else if (BitmapInfo->BitmapBitsPerPixel == 32) {   // case where FrameGrabberRGBFormat is set to fgf_RGB32 (default setting) (you can select it in the "frame grabber" tab)
		 for (int i = 0 ; i < BitmapInfo->BitmapHeight ; i++) {
			TRGBQuad *RGB32Line = (TRGBQuad*) BitmapLinePtr;
			for (int iCol = 0 ; iCol < BitmapInfo->BitmapWidth ; iCol ++) {
			   RGB32Line[iCol].rgbBlue = 0; // e.g. we remove the blue color -> yellow image
			}
			BitmapLinePtr += BitmapInfo->BitmapLineSize;
		 }
	  }
   }

   // GRID DRAWING BY USING THE CANVAS DC AND FILLRECT
   // ================================================

   if (false) {

      int xStep = BitmapInfo->BitmapWidth / 10;
      int yStep = BitmapInfo->BitmapHeight / 10;

      int xLocation = xStep;
	  int yLocation = yStep;

      TCanvas *Canvas = new TCanvas;
	  Canvas->Brush->Color = clBlue;
      Canvas->Brush->Style = bsSolid;
      Canvas->Handle = BitmapInfo->BitmapDC;

      while (xLocation < BitmapInfo->BitmapWidth) {
		 Canvas->FillRect(Rect(xLocation, 0, xLocation + 1, BitmapInfo->BitmapHeight));
         xLocation += xStep;
      }

      while (yLocation < BitmapInfo->BitmapHeight) {
         Canvas->FillRect(Rect(0, yLocation, BitmapInfo->BitmapWidth, yLocation + 1));
         yLocation += yStep;
      }

      Canvas->Free();
   }


}


////////////////////////////////////////////////////////////////////////////////
//                                DISPLAY TAB
////////////////////////////////////////////////////////////////////////////////

void __fastcall EnableOrDisableGroupBoxControls (TGroupBox *GroupBox, bool Value)
{
   int i;

   for (i = 0 ; i < GroupBox->ControlCount ; i ++) {
	  GroupBox->Controls[i]->Enabled = Value;
   }
}

void __fastcall TCaptureWorkerForm::RefreshDisplayOptions(void)
{
   if (ComponentState.Contains(csDestroying)) return; // do nothing if the app terminates

   // let's refresh the controls according to the TVideoGrabber's states
   AssignListToComboBox (MainCaptureForm->cbVideoInputDevice, VideoGrabber->VideoDevices, VideoGrabber->VideoDevice);//to MainCaptureForm
   AssignListToComboBox (MainCaptureForm->cbAudioInputDevice, VideoGrabber->AudioDevices, VideoGrabber->AudioDevice);

   AssignListToComboBox (cboVideoDevices, VideoGrabber->VideoDevices, VideoGrabber->VideoDevice);
   AssignListToComboBox (cboAudioDevices, VideoGrabber->AudioDevices, VideoGrabber->AudioDevice);
   AssignListToComboBox (cboAudioRenderers, VideoGrabber->AudioRenderers, VideoGrabber->AudioRenderer);

   chk1stVideoWindowActive->Checked = VideoGrabber->Display_Active;
   chk1stVisible->Checked           = VideoGrabber->Display_Visible;
   chk1stEmbedded->Checked          = VideoGrabber->Display_Embedded;
   chk1stFullScreen->Checked        = VideoGrabber->Display_FullScreen;
   chk1stStayOnTop->Checked         = VideoGrabber->Display_StayOnTop;
   chk1stUseVideoPort->Checked      = VideoGrabber->Display_VideoPortEnabled;
   chk1stMouseMovesWindow->Checked  = VideoGrabber->Display_MouseMovesWindow;
   chk1stAutoSize->Checked          = VideoGrabber->Display_AutoSize;
   rdg1AspectRatio->ItemIndex       = VideoGrabber->Display_AspectRatio;
   tbr1PanScan->Position            = VideoGrabber->Display_PanScanRatio;

   rdg1stDisplayedOn->ItemIndex     = VideoGrabber->Display_Monitor;

   EnableOrDisableGroupBoxControls (grbMainVideoWindow, VideoGrabber->Display_Active);

   if (VideoGrabber->Display_Active) {
	  chk1stStayOnTop->Enabled         = (! VideoGrabber->Display_Embedded) || VideoGrabber->Display_FullScreen;
	  rdg1stDisplayedOn->Enabled       = VideoGrabber->MonitorsCount() > 1;
	  chk1stMouseMovesWindow->Enabled  = ! VideoGrabber->Display_Embedded;
	  btnEG1stNewLocation->Enabled     = ! VideoGrabber->Display_Embedded;
	  btnEG1stNewLocation2->Enabled    = ! VideoGrabber->Display_Embedded;
	  chk1stUseVideoPort->Enabled      = VideoGrabber->IsVideoPortAvailable;
   }

   chk2ndVideoWindowActive->Checked = VideoGrabber->DualDisplay_Active;
   chk2ndVisible->Checked           = VideoGrabber->DualDisplay_Visible;
   chk2ndEmbedded->Checked          = VideoGrabber->DualDisplay_Embedded;
   chk2ndFullScreen->Checked        = VideoGrabber->DualDisplay_FullScreen;
   chk2ndStayOnTop->Checked         = VideoGrabber->DualDisplay_StayOnTop;
   chk2ndUseVideoPort->Checked      = VideoGrabber->DualDisplay_VideoPortEnabled;
   chk2ndMouseMovesWindow->Checked  = VideoGrabber->DualDisplay_MouseMovesWindow;
   chk2ndAutoSize->Checked          = VideoGrabber->DualDisplay_AutoSize;
   rdg2AspectRatio->ItemIndex       = VideoGrabber->DualDisplay_AspectRatio;
   tbr2PanScan->Position            = VideoGrabber->DualDisplay_PanScanRatio;
   rdg2ndDisplayedOn->ItemIndex     = VideoGrabber->DualDisplay_Monitor;

   EnableOrDisableGroupBoxControls (grb2ndVideoWindow, VideoGrabber->DualDisplay_Active);

   if (VideoGrabber->DualDisplay_Active) {
      chk2ndStayOnTop->Enabled         = (! VideoGrabber->DualDisplay_Embedded) || VideoGrabber->DualDisplay_FullScreen;
      rdg2ndDisplayedOn->Enabled       = VideoGrabber->MonitorsCount() > 1;
      chk2ndMouseMovesWindow->Enabled  = ! VideoGrabber->DualDisplay_Embedded;
      btnEG2ndNewLocation->Enabled     = ! VideoGrabber->DualDisplay_Embedded;
      btnEG2ndNewLocation->Enabled     = ! VideoGrabber->DualDisplay_Embedded;
      chk2ndUseVideoPort->Enabled      = VideoGrabber->IsVideoPortAvailable;
   }
}

//---------------------------------------------------------------------------
void __fastcall TCaptureWorkerForm::chkBlackMagicDecklinkClick(TObject *Sender)
{
   if (chkBlackMagicDecklink->Checked)
   {
      VideoGrabber->VideoRendererExternal = vre_BlackMagic_Decklink;
   }
   else
   {
      VideoGrabber->VideoRendererExternal = vre_None;
   }
}

//---------------------------------------------------------------------------
void __fastcall TCaptureWorkerForm::chkVideoVisibleWhenStoppedClick(
      TObject *Sender)
{
   VideoGrabber->VideoVisibleWhenStopped = chkVideoVisibleWhenStopped->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chkAdjustPixelAspectRatioClick(
      TObject *Sender)
{
   VideoGrabber->AdjustPixelAspectRatio = chkAdjustPixelAspectRatio->Checked;
}

//---------------------------------------------------------------------------
void __fastcall TCaptureWorkerForm::chkChangeCursorClick(TObject *Sender)
{
   if (chkChangeCursor->Checked) {
      VideoGrabber->VideoCursor = crCross;
   }
   else {
      VideoGrabber->VideoCursor = crDefault;
   }
}

//---------------------------------------------------------------------------
void __fastcall TCaptureWorkerForm::chkBorderClick(TObject *Sender)
{
   if (chkBorder->Checked) {
      VideoGrabber->BorderStyle = bsSingle;
   }
   else {
      VideoGrabber->BorderStyle = bsNone;
   }
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chkTransparencyClick(TObject *Sender)
{
   VideoGrabber->ColorKeyEnabled = chkTransparency->Checked;
   if (VideoGrabber->ColorKeyEnabled) {
      if (FTransparentForm == NULL) {
         Application->CreateForm (__classid (TForm), &FTransparentForm);
         FTransparentForm->Width = 300;
         FTransparentForm->Height = 100;
         FTransparentForm->Position = poScreenCenter;
         FTransparentForm->FormStyle = fsStayOnTop;
         FTransparentPanel = new TPanel (FTransparentForm);
         FTransparentPanel->Parent = FTransparentForm;
         FTransparentPanel->BevelInner = bvNone;
         FTransparentPanel->BevelOuter = bvNone;
         FTransparentPanel->Align = alClient;
         FTransparentPanel->Caption = "transparent window";
         FTransparentPanel->Font->Color = clRed;
         FTransparentPanel->Font->Size = 16;
      }

      FTransparentForm->Color = clBlue;
      FTransparentPanel->Color = VideoGrabber->ColorKey;
      VideoGrabber->SetWindowTransparency ((unsigned int) FTransparentForm->Handle, true, false, 0);
      FTransparentForm->Show();
   }
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::rdgVideoRendererClick(TObject *Sender)
{
   VideoGrabber->VideoRenderer = TVideoRenderer (rdgVideoRenderer->ItemIndex);
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chk1stVideoWindowActiveClick(TObject *Sender)
{
   VideoGrabber->Display_Active = chk1stVideoWindowActive->Checked;
   RefreshDisplayOptions();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chk1stVisibleClick(TObject *Sender)
{
   VideoGrabber->Display_Visible = chk1stVisible->Checked;
   RefreshDisplayOptions();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chk1stEmbeddedClick(TObject *Sender)
{
   VideoGrabber->Display_Embedded = chk1stEmbedded->Checked;
   RefreshDisplayOptions();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chk1stAutoSizeClick(TObject *Sender)
{
   VideoGrabber->Display_AutoSize = chk1stAutoSize->Checked;
   RefreshDisplayOptions();
}
//---------------------------------------------------------------------------


void __fastcall TCaptureWorkerForm::chk1stFullScreenClick(TObject *Sender)
{
   VideoGrabber->Display_FullScreen = chk1stFullScreen->Checked;
   RefreshDisplayOptions();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chk1stStayOnTopClick(TObject *Sender)
{
   VideoGrabber->Display_StayOnTop = chk1stStayOnTop->Checked;
   RefreshDisplayOptions();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chk1stUseVideoPortClick(TObject *Sender)
{
   VideoGrabber->Display_VideoPortEnabled = chk1stUseVideoPort->Checked;
   RefreshDisplayOptions();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::rdg1stDisplayedOnClick(TObject *Sender)
{
   VideoGrabber->Display_Monitor = rdg1stDisplayedOn->ItemIndex; // 0 or 1
   RefreshDisplayOptions();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnEG1stNewLocationClick(TObject *Sender)
{
   VideoGrabber->Display_AutoSize = true;  // the component"s video size will be used
   VideoGrabber->Display_Embedded = false; // detach from component
   VideoGrabber->Display_SetLocation (10, 10, 320, 240); // 320 and 240 are ! used because of AutoSize above
   RefreshDisplayOptions();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnEG1stNewLocation2Click(TObject *Sender)
{
   VideoGrabber->Display_AutoSize = false; // we will specify the size below
   VideoGrabber->Display_Embedded = false; // detach from component
   VideoGrabber->Display_SetLocation (20, 20, 480, 360); // new size: 480 x 360
   RefreshDisplayOptions();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chk1stMouseMovesWindowClick(TObject *Sender)
{
   VideoGrabber->Display_MouseMovesWindow = chk1stMouseMovesWindow->Checked;
   RefreshDisplayOptions();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::rdg1AspectRatioClick(TObject *Sender)
{
   VideoGrabber->Display_AspectRatio = (TAspectRatio) rdg1AspectRatio->ItemIndex;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::tbr2PanScanChange(TObject *Sender)
{
   VideoGrabber->DualDisplay_PanScanRatio = tbr2PanScan->Position;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::tbr1PanScanChange(TObject *Sender)
{
   VideoGrabber->Display_PanScanRatio = tbr1PanScan->Position;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnTestPanScanClick(TObject *Sender)
{
   rdg1AspectRatio->ItemIndex = ar_PanScan;
   VideoGrabber->Display_AspectRatio = ar_PanScan;
   VideoGrabber->Display_Embedded = true;
   VideoGrabber->Display_AutoSize = false;
   VideoGrabber->Width = 400;
   VideoGrabber->Height = VideoGrabber->Width * 9 / 16;
}

//---------------------------------------------------------------------------
void __fastcall TCaptureWorkerForm::chk2ndVideoWindowActiveClick(TObject *Sender)
{
   VideoGrabber->DualDisplay_Active = chk2ndVideoWindowActive->Checked;
   RefreshDisplayOptions();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chk2ndVisibleClick(TObject *Sender)
{
   VideoGrabber->DualDisplay_Visible = chk2ndVisible->Checked;
   RefreshDisplayOptions();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chk2ndEmbeddedClick(TObject *Sender)
{
   VideoGrabber->DualDisplay_Embedded = chk2ndEmbedded->Checked;
   RefreshDisplayOptions();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chk2ndFullScreenClick(TObject *Sender)
{
   VideoGrabber->DualDisplay_FullScreen = chk2ndFullScreen->Checked;
   RefreshDisplayOptions();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chk2ndAutoSizeClick(TObject *Sender)
{
   VideoGrabber->DualDisplay_AutoSize = chk2ndAutoSize->Checked;
   RefreshDisplayOptions();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chk2ndStayOnTopClick(TObject *Sender)
{
   VideoGrabber->DualDisplay_StayOnTop = chk2ndStayOnTop->Checked;
   RefreshDisplayOptions();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chk2ndUseVideoPortClick(TObject *Sender)
{
   VideoGrabber->DualDisplay_VideoPortEnabled = chk2ndUseVideoPort->Checked;
   RefreshDisplayOptions();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::chk2ndMouseMovesWindowClick(TObject *Sender)
{
   VideoGrabber->DualDisplay_MouseMovesWindow = chk2ndMouseMovesWindow->Checked;
   RefreshDisplayOptions();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::rdg2AspectRatioClick(TObject *Sender)
{
   VideoGrabber->DualDisplay_AspectRatio = (TAspectRatio) rdg2AspectRatio->ItemIndex;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::rdg2ndDisplayedOnClick(TObject *Sender)
{
   VideoGrabber->DualDisplay_Monitor = rdg2ndDisplayedOn->ItemIndex; // 0 or 1
   RefreshDisplayOptions();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnEG2ndNewLocationClick(TObject *Sender)
{
   VideoGrabber->DualDisplay_AutoSize = true;  // the component"s video size will be used
   VideoGrabber->DualDisplay_Embedded = false; // detach from component
   VideoGrabber->DualDisplay_SetLocation (510, 10, 320, 240); // 320 and 240 are ! used because of AutoSize above
   RefreshDisplayOptions();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnEG2ndNewLocation2Click(TObject *Sender)
{
   VideoGrabber->DualDisplay_AutoSize = false; // we will specify the size below
   VideoGrabber->DualDisplay_Embedded = false; // detach from component
   VideoGrabber->DualDisplay_SetLocation (520, 30, 480, 360); // new size: 480 x 360
   RefreshDisplayOptions();
}

//---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
//                                    MISC.
////////////////////////////////////////////////////////////////////////////////


void __fastcall TCaptureWorkerForm::edtStoragePathChange(TObject *Sender)
{
   VideoGrabber->StoragePath = edtStoragePath->Text;
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::RefreshDeviceControls(void)
{
   if (ComponentState == (TComponentState() << csDestroying)) {
      // we do nothing if the app terminates
      return;
   }

   // refresh the controls according to the TVideoGrabber's states

   chkReduceDVFrameRate->Checked = VideoGrabber->DVReduceFrameRate;
   chkReduceDVFrameRate->Enabled = VideoGrabber->IsDigitalVideoIn;

   btnPauseRecording->Enabled = VideoGrabber->RecordingCanPause;
   btnResumeRecording->Enabled = VideoGrabber->RecordingCanPause;


   cboAnalogVideoStandard->Enabled = VideoGrabber->IsAnalogVideoDecoderAvailable;
   cboVideoInputs->Enabled = VideoGrabber->IsVideoCrossbarAvailable;

   edtFrameRate->Text = FormatFloat ("0.00", VideoGrabber->FrameRate);


   bool CanUseCompressors = (VideoGrabber->RecordingMethod == rm_AVI) || (VideoGrabber->RecordingMethod == rm_MKV) || (VideoGrabber->RecordingMethod == rm_MP4);

   rdgCompressMode->Enabled = CanUseCompressors;
   rdgCompressType->Enabled = CanUseCompressors;
   cboVideoCompressors->Enabled = CanUseCompressors;
   btnVideoCompressorSettings->Enabled = CanUseCompressors;
   cboAudioCompressors->Enabled = CanUseCompressors;
   btnAudioCompressorSettings->Enabled = CanUseCompressors;
   chkPreallocFile->Enabled = CanUseCompressors;
}

void __fastcall TCaptureWorkerForm::SendVCRCommand (TObject *Sender)
{
   // THE "TAG" PROPERTY OF EACH MENU ITEM IS USED TO SELECT THE CORRESPONDING TVCRCommand value
   if (VideoGrabber->SendDVCommand (TDVCommand (((TMenuItem*) Sender)->Tag))) {
      mmoLog->Lines->Add ("DV command sent...");
   }
   else {
      mmoLog->Lines->Add ("DV command failed.");
   }
}
void __fastcall TCaptureWorkerForm::VideoGrabberDVCommandCompleted(
      TObject *Sender, int NewStateValue, String NewStateLabel)
{
   mmoLog->Lines->Add ("camcorder state: " + NewStateLabel);
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::Resetvideocapturedevicesettings1Click(
      TObject *Sender)
{
   VideoGrabber->ResetVideoDeviceSettings();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::VideoGrabberDVDiscontinuity(TObject *Sender,
      bool &DeliverNewFrame)
{
   // SAMPLE CODE 1: creates a new file during recording when a discontinuity in the DV date/time is detected (uncommend the 2 lines below)
   // DeliverNewFrame = false;
   // VideoGrabber->RecordToNewFileNow();

   // SAMPLE CODE 2: stops the recording when a discontinuity in the DV date/time is detected (uncommend the 2 lines below)
   // DeliverNewFrame = false;
   // VideoGrabber->StopRecording();

   // Important: from this event you should not perform any actions with the potential to block, such as holding
   //   a critical section or waiting on another thread. Also, do not call any GDI or USER32.DLL APIs.
   
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::VideoGrabberLog(TObject *Sender,
      TLogType LogType, String Severity, String InfoMsg)
{
   mmoLog->Lines->Add ("[" + Severity + "] " + InfoMsg);
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::VideoGrabberInactive(TObject *Sender)
{
   mmoLog->Lines->Add ("(done)");
}
//---------------------------------------------------------------------------
void __fastcall TCaptureWorkerForm::ShowFormAboveVideo (TForm *Form)
{
   if (Form == NULL) return;
   Form->Top = Top + VideoGrabber->Top - Form->Height;
   Form->Left = Left + VideoGrabber->Left - Form->Width;
   if (Form->Top < 0) {
      Form->Top = 0;
   }
   if (Form->Left < 0) {
      Form->Left = 0;
   }
   Form->Show();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::mnuAboutClick(TObject *Sender)
{
   VideoGrabber->About();
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::VideoGrabberResize(TObject *Sender)
{
   int NewWidth;
   int NewHeight;

   NewWidth = VideoGrabber->Left + VideoGrabber->Width + 30;
   NewHeight = VideoGrabber->Top + VideoGrabber->Height + 60;
   if (NewWidth < FStartupWidth) {
      NewWidth = FStartupWidth;
   }
   if (NewHeight < FStartupHeight) {
      NewHeight = FStartupHeight;
   }
   if ((NewWidth >= Width) && (NewHeight >= Height)) {
	  Width = NewWidth;
      Height = NewHeight;
   }
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::VideoGrabberDeviceLost(TObject *Sender)
{
   mmoLog->Lines->Add ("DEVICE LOST!");
   AssignListToComboBox (cboVideoDevices, VideoGrabber->VideoDevices, VideoGrabber->VideoDevice);
   AssignListToComboBox (cboAudioDevices, VideoGrabber->AudioDevices, VideoGrabber->AudioDevice);
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::VideoGrabberDiskFull(TObject *Sender)
{
   mmoLog->Lines->Add ("DISK FULL!");
}
//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::VideoGrabberReinitializing(TObject *Sender)
{
   mmoLog->Clear();
   FOldVideoSignalState = false;
}
//---------------------------------------------------------------------------
void __fastcall TCaptureWorkerForm::VideoGrabberDeviceArrivalOrRemoval(
      TObject *Sender, bool IsDeviceArrival, bool IsVideoDevice,
      String DeviceName, int DeviceIndex)
{
   String AudioOrVideoDevice;

   if (IsVideoDevice) {
      AudioOrVideoDevice= "video device ";
   }
   else {
      AudioOrVideoDevice= "audio device ";
   }
   if (IsDeviceArrival) {
      mmoLog->Lines->Add (AudioOrVideoDevice + "arrival:");
   }
   else {
      mmoLog->Lines->Add (AudioOrVideoDevice + "removal:");
   }
   mmoLog->Lines->Add (DeviceName);
   if (IsVideoDevice) {
	  AssignListToComboBox (cboVideoDevices, VideoGrabber->VideoDevices, VideoGrabber->VideoDevice);
   }
   else {
	  AssignListToComboBox (cboAudioDevices, VideoGrabber->AudioDevices, VideoGrabber->AudioDevice);
   }
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void __fastcall TCaptureWorkerForm::btnCurrentIPCameraURLClick(TObject *Sender)
{
      ShellExecute(0, "open", "http://www.datastead.com/demo/IPCam.htm", "", "", SW_SHOW);
}

//---------------------------------------------------------------------------



void __fastcall TCaptureWorkerForm::btnIPCamStartPreviewClick(TObject *Sender)
{
   if (VideoGrabber->VideoSource != vs_IPCamera) {
      if (VideoGrabber->CurrentState == cs_Down) {
         VideoGrabber->VideoSource = vs_IPCamera;
         mmoLog->Lines->Add ("VideoSource set to vs_IPCamera");
      }
      else {
		 ShowMessage ("stop the current action, then select 'IP camera' in the 'Video sources' list of the 'Video Source' tab");
      }
   }
   VideoGrabber->StartPreview();
}
//---------------------------------------------------------------------------


void __fastcall TCaptureWorkerForm::btnIPCamAuthenticationInfoClick(
      TObject *Sender)
{
   ShowMessage ("when an authentication is required to connect to the IP camera, the OnAuthenticationNeeded event occurs.\nThe user name and password must be passed to the UserName and Password parameters of this event.");

}
//---------------------------------------------------------------------------


TVideoGrabber *PIPVideoGrabber = NULL;

void __fastcall TCaptureWorkerForm::btnPIPFromAnotherTVideoGrabberClick(
      TObject *Sender)
{
   TOpenDialog *OpenDlg = new TOpenDialog(NULL);
   OpenDlg->Filter = OPEN_MEDIA_FILES;
   if (OpenDlg->Execute()) {
      if (PIPVideoGrabber == NULL) {
         PIPVideoGrabber = new TVideoGrabber(this);
         PIPVideoGrabber->VideoRenderer = vr_None;
         PIPVideoGrabber->Visible = false;
         VideoGrabber->Mixer_SetupPIPFromSource (PIPVideoGrabber->UniqueID, 0, 0, 0, 0, true, 10, 10, 200, 150, false);
      }
      PIPVideoGrabber->PlayerFileName = OpenDlg->FileName;
      PIPVideoGrabber->OpenPlayer();
   }
   delete (OpenDlg);
}
//---------------------------------------------------------------------------




void __fastcall TCaptureWorkerForm::mnuExitClick(TObject *Sender)
{
   Close();
}
//---------------------------------------------------------------------------


