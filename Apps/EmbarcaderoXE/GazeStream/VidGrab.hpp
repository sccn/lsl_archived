// CodeGear C++Builder
// Copyright (c) 1995, 2010 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'VidGrab.pas' rev: 22.00

#ifndef VidgrabHPP
#define VidgrabHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member functions
#pragma pack(push,8)
#include <System.hpp>	// Pascal unit
#include <SysInit.hpp>	// Pascal unit
#include <Windows.hpp>	// Pascal unit
#include <Messages.hpp>	// Pascal unit
#include <Classes.hpp>	// Pascal unit
#include <Controls.hpp>	// Pascal unit
#include <ComCtrls.hpp>	// Pascal unit
#include <Menus.hpp>	// Pascal unit
#include <Graphics.hpp>	// Pascal unit
#include <ExtCtrls.hpp>	// Pascal unit
#include <jpeg.hpp>	// Pascal unit
#include <ActiveX.hpp>	// Pascal unit
#include <Forms.hpp>	// Pascal unit
#include <Types.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------
typedef __int64 LargeInteger;
typedef __int64 Int64;

namespace Vidgrab
{
//-- type declarations -------------------------------------------------------
typedef __int64 LargeInteger;

typedef unsigned LongWord_;

#pragma option push -b
enum TFrameGrabber { fg_BothStreams, fg_PreviewStream, fg_CaptureStream, fg_Disabled };
#pragma option pop

#pragma option push -b
enum TFrameGrabberRGBFormat { fgf_Default, fgf_RGB32, fgf_RGB24, fgf_RGB565, fgf_RGB555, fgf_RGB8 };
#pragma option pop

#pragma option push -b
enum TAutoFileName { fn_Sequential, fn_DateTime, fn_Counter };
#pragma option pop

#pragma option push -b
enum TCompressionMode { cm_NoCompression, cm_CompressOnTheFly, cm_CompressAfterRecording };
#pragma option pop

#pragma option push -b
enum TCompressionType { ct_Video, ct_Audio, ct_AudioVideo };
#pragma option pop

#pragma option push -b
enum TCurrentState { cs_Down, cs_Preview, cs_Recording, cs_Playback, cs_Reencoding };
#pragma option pop

#pragma option push -b
enum TFrameCaptureDest { fc_TBitmap, fc_BmpFile, fc_JpegFile, fc_Clipboard };
#pragma option pop

#pragma option push -b
enum TDVSize { dv_Full, dv_Half, dv_Quarter, dv_DC };
#pragma option pop

#pragma option push -b
enum TTVChannelInfo { tci_Channel, tci_DefaultVideoFrequency, tci_OverriddenVideoFrequency, tci_TunerVideoFrequency, tci_TunerAudioFrequency, tci_Locked };
#pragma option pop

#pragma option push -b
enum TWebcamStillCaptureButton { wb_Disabled, wb_Enabled };
#pragma option pop

#pragma option push -b
enum TDVVideoStandard { dvs_Default, dvs_PAL, dvs_NTSC };
#pragma option pop

#pragma option push -b
enum TDVVideoFormat { dvf_Default, dvf_DVSD, dvf_DVHD, dvf_DVSL };
#pragma option pop

#pragma option push -b
enum TAVIInfoType { av_Duration, av_FrameCount, av_VideoWidth, av_VideoHeight, av_VideoFrameRateFps, av_VideoCodec, av_AudioCodec, av_AvgBitRate, av_AudioChannels, av_AudioSamplesPerSec, av_AudioBitsPerSample, av_FileSizeInKB, av_AudioStreams };
#pragma option pop

#pragma option push -b
enum TDVDInfoType { dvd_NumberOfVolumes, dvd_TotalDuration, dvd_NumberOfTitles, dvd_TitleDuration, dvd_TitleFrameRate, dvd_SourceResolutionX, dvd_SourceResolutionY, dvd_TitleFrameCount };
#pragma option pop

#pragma option push -b
enum THeaderAttribute { ha_Title, ha_Description, ha_Author, ha_Copyright, ha_AlbumArtist, ha_AlbumTitle, ha_Composer, ha_ContentDistributor, ha_Director, ha_EncodingTime, ha_Genre, ha_Language, ha_ParentalRating, ha_Producer, ha_Provider, ha_ToolName, ha_ToolVersion, ha_Writer, ha_IARL, ha_ICMS, ha_ICMT, ha_ICRD, ha_ICRP, ha_IDIM, ha_IDPI, ha_IENG, ha_IGNR, ha_IKEY, ha_ILGT, ha_IMED, ha_IPLT, ha_IPRD, ha_ISFT, ha_ISHP, ha_ISRC, ha_ISRF, ha_ITCH };
#pragma option pop

#pragma option push -b
enum TDVCommand { dv_Play, dv_Stop, dv_Freeze, dv_Thaw, dv_Ff, dv_Rew, dv_Record, dv_RecordFreeze, dv_RecordStrobe, dv_StepFwd, dv_StepRev, dv_ModeShuttle, dv_PlayFastestFwd, dv_PlaySlowestFwd, dv_PlayFastestRev, dv_PlaySlowestRev };
#pragma option pop

#pragma option push -b
enum TDialog { dlg_VideoDevice, dlg_VideoCompressor, dlg_AudioCompressor, dlg_StreamConfig, dlg_VfwFormat, dlg_VfwSource, dlg_vfwDisplay, dlg_VideoCrossbar, dlg_AudioCrossbar, dlg_TVTuner, dlg_TVAudio, dlg_AudioDevice, dlg_NetShowConfig, dlg_DScaler, dlg_FFDShowVideo, dlg_FFDShowAudio, dlg_Multiplexer, dlg_None };
#pragma option pop

#pragma option push -b
enum TTriState { ts_Undefined, ts_False, ts_True };
#pragma option pop

#pragma option push -b
enum TLogoLayout { lg_Centered, lg_Stretched, lg_Repeated, lg_TopLeft, lg_TopRight, lg_BottomLeft, lg_BottomRight, lg_Boxed };
#pragma option pop

#pragma option push -b
enum TNotificationMethod { nm_Timer, nm_Thread };
#pragma option pop

#pragma option push -b
enum TVideoControl { vc_FlipHorizontal, vc_FlipVertical, vc_ExternalTriggerEnable, vc_Trigger };
#pragma option pop

#pragma option push -b
enum TCardinalDirection { cd_North, cd_NorthEast, cd_East, cd_SouthEast, cd_South, cd_SouthWest, cd_West, cd_NorthWest, cd_Center };
#pragma option pop

#pragma option push -b
enum TVideoRotation { rt_0_deg, rt_90_deg, rt_180_deg, rt_270_deg, rt_0_deg_mirror, rt_90_deg_mirror, rt_180_deg_mirror, rt_270_deg_mirror, rt_custom_angle, rt_custom_angle_mirror };
#pragma option pop

#pragma option push -b
enum TVideoDeinterlacing { di_Disabled, di_HalfSize, di_FullSize, di_DScaler, di_AVISynth, di_FFDShow, di_ThirdPartyDeinterlacer };
#pragma option pop

#pragma option push -b
enum TThirdPartyFilterList { tpf_VideoSource, tpf_VideoPreview, tpf_VideoRecording, tpf_AudioSource, tpf_AudioRendering, tpf_AudioRecording, tpf_VideoRendering, tpf_VideoRenderer, tpf_AudioRenderer, tpf_ThirdPartyVideoSource, tpf_ThirdPartyAudioSource, tpf_AddToGraph };
#pragma option pop

#pragma option push -b
enum TRecordingSize { rs_Default, rs_HalfSize, rs_QuarterSize };
#pragma option pop

#pragma option push -b
enum TFormatType { ft_VideoInfo, ft_VideoInfo2, ft_DvInfo, ft_Mpeg1Video, ft_Mpeg2Video, ft_Mpeg1Stream, ft_Mpeg2Stream, ft_MpegStreamType, ft_MpegCustom, ft_WaveFormatEx, ft_Mpeg1Audio, ft_Mpeg2Audio, ft_Mpeg3Audio, ft_OGG, ft_Unknown };
#pragma option pop

typedef TFormatType *pFormatType;

#pragma option push -b
enum TAudioFormat { af_default, af_8000_8b_1ch, af_8000_8b_2ch, af_8000_16b_1ch, af_8000_16b_2ch, af_11025_8b_1ch, af_11025_8b_2ch, af_11025_16b_1ch, af_11025_16b_2ch, af_16000_8b_1ch, af_16000_8b_2ch, af_16000_16b_1ch, af_16000_16b_2ch, af_22050_8b_1ch, af_22050_8b_2ch, af_22050_16b_1ch, af_22050_16b_2ch, af_32000_8b_1ch, af_32000_8b_2ch, af_32000_16b_1ch, af_32000_16b_2ch, af_44100_8b_1ch, af_44100_8b_2ch, af_44100_16b_1ch, af_44100_16b_2ch, af_48000_8b_1ch, af_48000_8b_2ch, af_48000_16b_1ch, af_48000_16b_2ch };
#pragma option pop

#pragma option push -b
enum TAudioChannelRenderMode { acrm_Normal, acrm_RenderLeft, acrm_RenderRight, acrm_MuteLeft, acrm_MuteRight, acrm_Mute_All, acrm_MixLeftAndRight, acrm_PassThru };
#pragma option pop

#pragma option push -b
enum TVideoRenderer { vr_AutoSelect, vr_EVR, vr_VMR9, vr_VMR7, vr_StandardRenderer, vr_OverlayRenderer, vr_RecordingPriority, vr_None };
#pragma option pop

#pragma option push -b
enum TVideoRendererExternal { vre_None, vre_Matrox_PRO, vre_Decklink_SD, vre_Decklink_Extreme, vre_Pinnacle_MovieBoard, vre_BlackMagic_Decklink };
#pragma option pop

#pragma option push -b
enum TRawSampleCaptureLocation { rl_SourceFormat, rl_AfterCompression };
#pragma option pop

#pragma option push -b
enum TMainConceptFiltersVersion { mc_Version4, mc_Version8 };
#pragma option pop

#pragma option push -b
enum TPointGreyConfig { pgr_SetRegister, pgr_GetRegister, pgr_SetBufferSize, pgr_GetBufferSize, pgr_SetFormat, pgr_GetFormat };
#pragma option pop

#pragma option push -b
enum TAuthenticationType { at_PublishingPoint, at_IPCamera, at_StreamingUrl };
#pragma option pop

#pragma option push -b
enum TIPCameraSetting { ips_ConnectionTimeout, ips_ReceiveTimeout };
#pragma option pop

#pragma option push -b
enum TSynchronizationRole { sr_Master, sr_Slave };
#pragma option pop

#pragma option push -b
enum TTrackbarAction { tba_MouseDown, tba_MouseUp, tba_KeyDown, tba_KeyUp };
#pragma option pop

#pragma option push -b
enum TVideoSource { vs_VideoCaptureDevice, vs_ScreenRecording, vs_VideoFileOrURL, vs_JPEGsOrBitmaps, vs_IPCamera, vs_Mixer, vs_VideoFromImages, vs_ThirdPartyFilter };
#pragma option pop

#pragma option push -b
enum TAudioSource { as_Default, as_UseExternalAudio };
#pragma option pop

#pragma option push -b
enum TVuMeter { vu_Disabled, vu_Analog, vu_Bargraph };
#pragma option pop

#pragma option push -b
enum TASFDeinterlaceMode { adm_NotInterlaced, adm_DeinterlaceNormal, adm_DeinterlaceHalfSize, adm_DeinterlaceHalfSizeDoubleRate, adm_DeinterlaceInverseTelecine, adm_DeinterlaceVerticalHalfSizeDoubleRate };
#pragma option pop

#pragma option push -b
enum TASFProfileVersion { apv_ProfileVersion_8, apv_ProfileVersion_9 };
#pragma option pop

#pragma option push -b
enum TFileSort { fs_TimeAsc, fs_TimeDesc, fs_NameAsc, fs_NameDesc };
#pragma option pop

#pragma option push -b
enum TTunerInputType { TunerInputCable, TunerInputAntenna };
#pragma option pop

#pragma option push -b
enum TRGBSelector { rs_Red, rs_Green, rs_Blue };
#pragma option pop

#pragma option push -b
enum TCameraControl { cc_Pan, cc_Tilt, cc_Roll, cc_Zoom, cc_Exposure, cc_Iris, cc_Focus };
#pragma option pop

#pragma option push -b
enum TPlaylist { pl_Add, pl_Remove, pl_Clear, pl_Loop, pl_NoLoop, pl_Play, pl_Stop, pl_Next, pl_Previous, pl_SortAlpha, pl_SortRevAlpha, pl_Random, pl_Sequential, pl_SpecifyPositions, pl_Transition };
#pragma option pop

#pragma option push -b
enum TVideoQuality { vq_Brightness, vq_Contrast, vq_Hue, vq_Saturation, vq_Sharpness, vq_Gamma, vq_ColorEnable, vq_WhiteBalance, vq_BacklightCompensation, vq_Gain };
#pragma option pop

#pragma option push -b
enum TMiscDeviceControl { mdc_GPIO, mdc_VPD, mdc_VPD_Data };
#pragma option pop

#pragma option push -b
enum TAVIMuxConfig { avmx_SetInterleavingMode, avmx_SetInterleave, avmx_SetPreroll };
#pragma option pop

#pragma option push -b
enum TApplicationPriority { ap_default, ap_idle, ap_normal, ap_high, ap_realtime };
#pragma option pop

#pragma option push -b
enum TRecordingMethod { rm_AVI, rm_ASF, rm_SendToDV, rm_MKV, rm_FLV, rm_MP4, rm_WebM, rm_MPG, rm_Multiplexer };
#pragma option pop

#pragma option push -b
enum TNetworkStreaming { ns_Disabled, ns_ASFDirectNetworkStreaming, ns_ASFStreamingToPublishingPoint };
#pragma option pop

#pragma option push -b
enum TNetworkStreamingType { nst_AudioVideoStreaming, nst_VideoStreaming, nst_AudioStreaming };
#pragma option pop

#pragma option push -b
enum TSyncPreview { sp_Auto, sp_Disabled, sp_Enabled };
#pragma option pop

#pragma option push -b
enum TGraphState { gs_Stopped, gs_Paused, gs_Running };
#pragma option pop

#pragma option push -b
enum TPlayerState { ps_Closed, ps_Stopped, ps_Paused, ps_Playing, ps_PlayingBackward, ps_FastForwarding, ps_FastRewinding, ps_Downloading, ps_DownloadCompleted, ps_DownloadCancelled };
#pragma option pop

#pragma option push -b
enum TRecordingTimer { rt_Disabled, rt_RecordToNewFile, rt_StopRecording, rt_StartRecording };
#pragma option pop

#pragma option push -b
enum TAero { ae_Default, ae_AutoBestRenderingQuality, ae_ForceOnWhenStartingVideo, ae_ForceOffWhenStartingVideo, ae_ForceOnImmediately, ae_ForceOffImmediately };
#pragma option pop

#pragma option push -b
enum TMPEGProgramSetting { mps_Program_Number, mps_Program_PCR_PID, mps_VideoStream_PID, mps_AudioStream_PID, mps_VideoStream_Type, mps_AudioStream_Type, mps_VideoFormat_Width, mps_VideoFormat_Height, mps_VideoAspectRatio_X, mps_VideoAspectRatio_Y, mps_ReceiveTimeoutInSeconds };
#pragma option pop

#pragma option push -b
enum TFrameInfoId { fi_FrameNumber, fi_DroppedFrameCount, fi_SampleTime_Hour, fi_SampleTime_Min, fi_SampleTime_Sec, fi_SampleTime_Hs, fi_SampleTime_TotalMin, fi_DVTimeCode_IsAvailable, fi_DVTimeCode_Hour, fi_DVTimeCode_Min, fi_DVTimeCode_Sec, fi_DVTimeCode_Ff, fi_DVTimeCode_TrackNumber, fi_DVDateTime_IsAvailable, fi_DVDateTime_Year, fi_DVDateTime_Month, fi_DVDateTime_Day, fi_DVDateTime_Hour, fi_DVDateTime_Min, fi_DVDateTime_Sec };
#pragma option pop

#pragma option push -b
enum TFrameInfoStringId { fis_DVTimeCode, fis_DVDateTime, fis_TimeCode, fis_FrameTime, fis_FrameNumber, fis_FullInfo };
#pragma option pop

struct TFrameInfo;
typedef TFrameInfo *pFrameInfo;

struct DECLSPEC_DRECORD TFrameInfo
{
	
public:
	__int64 FrameTime;
	__int64 FrameTime_TotalMin;
	__int64 FrameTime_TotalSec;
	__int64 FrameTime_TotalHs;
	__int64 FrameNumber;
	int DroppedFrameCount;
	int FrameTime_Hour;
	int FrameTime_Min;
	int FrameTime_Sec;
	int FrameTime_Hs;
	int DVTimeCode_IsAvailable;
	int DVTimeCode_Hour;
	int DVTimeCode_Min;
	int DVTimeCode_Sec;
	int DVTimeCode_Ff;
	int DVTimeCode_TrackNumber;
	int DVDateTime_IsAvailable;
	int DVDateTime_Year;
	int DVDateTime_Month;
	int DVDateTime_Day;
	int DVDateTime_Hour;
	int DVDateTime_Min;
	int DVDateTime_Sec;
	TCurrentState CurrentState;
	TGraphState GraphState;
	TPlayerState PlayerState;
	int Reserved0;
	int Reserved1;
};


struct TFrameBitmapInfo;
typedef TFrameBitmapInfo *pFrameBitmapInfo;

struct DECLSPEC_DRECORD TFrameBitmapInfo
{
	
public:
	int BitmapWidth;
	int BitmapHeight;
	int BitmapBitsPerPixel;
	int BitmapLineSize;
	int BitmapSize;
	int BitmapPlanes;
	unsigned BitmapHandle;
	System::Byte *BitmapDataPtr;
	HDC BitmapDC;
	int CurrentXMouseLocation;
	int CurrentYMouseLocation;
	int LastXMouseDownLocation;
	int LastYMouseDownLocation;
	bool IsMouseDown;
	Controls::TMouseButton LastMouseButtonClicked;
	int Reserved0;
};


struct TVideoHdr;
typedef TVideoHdr *pVideoHdr;

struct DECLSPEC_DRECORD TVideoHdr
{
	
public:
	System::Byte *lpData;
	unsigned dwBufferLength;
	unsigned dwBytesUsed;
	unsigned dwTimeCaptured;
	unsigned dwUser;
	unsigned dwFlags;
	System::StaticArray<System::PLongWord, 4> dwReserved;
};


#pragma option push -b
enum TMultiplexedRole { mr_NotMultiplexed, mr_MultiplexedMosaic4, mr_MultiplexedMosaic16, mr_MultiplexedMaster, mr_MultiplexedSlave };
#pragma option pop

#pragma option push -b
enum TMpegStreamType { mpst_Default, mpst_Program, mpst_Program_DVD, mpst_Program_DVD_MC, mpst_Program_SVCD, mpst_MPEG1, mpst_MPEG1_VCD };
#pragma option pop

#pragma option push -b
enum TLogType { e_add_filter, e_add_source_filter, e_audio_compressor_not_suitable, e_bind_moniker_to_filter, e_compressor_possibly_not_suitable, e_create_instance, e_ddraw_caps_not_suitable, e_device_in_use_in_another_graph, e_disk_full, e_failed, e_failed_to_allocate_recording_file, e_failed_to_bind_codec, e_failed_to_connect_crossbar_pin, e_failed_to_connect_to_server, e_failed_to_create_directory, e_failed_to_create_file, e_failed_to_create_temp, e_failed_to_bind_frame_grabber, e_failed_to_load_ASF_profile, e_failed_to_load_ASF_profile_custom_file, e_failed_to_load_set_of_bitmaps, e_failed_to_set_image_overlay, e_failed_to_set_logo, e_failed_to_play_backwards, e_failed_to_render_file, e_failed_to_renew_recording_file, 
	e_failed_to_set_player_speed_ratio_with_audio, e_failed_to_setup_network_streaming, e_failed_to_start_preview, e_failed_to_start_recording, e_file_in_use, e_file_name_not_specified, e_file_not_found, e_get_audio_format, e_get_interface, e_get_video_format, e_graph_error, e_graph_cant_run, e_graph_must_be_restarted, e_hw_deinterlace_not_supported, e_incompatible_options, e_index_out_of_range, e_invalid_directory, e_library_not_found, e_load_filter, e_no_audio_In_device, e_no_device_available, e_no_dialog, e_no_stream_control, e_no_tv_tuner, e_no_device_selected, e_no_video_input_device, e_not_allowed_during_network_streaming, e_not_allowed_with_streaming_URL, e_not_assigned, e_not_multiplexed_master, e_not_previewing, e_not_recording, e_not_reencoding, 
	e_not_streaming, e_out_of_memory, e_pause_resume_disabled, e_pin_not_found, e_interface_not_assigned, e_query_config_avi_mux, e_reencoding, e_recording_cannot_pause, e_render_audio_stream, e_render_video_stream, e_must_restart_master, e_recording_on_motion_failed, e_sendtodv_device_index_out_of_bound, e_sendtodv_deviceindex_and_videodevice_have_same_value, e_sendtodv_failed_to_bind_dv_device, e_set_filter_graph, e_set_interleaving_mode, e_set_master_stream, e_set_output_compatibility_index, e_set_output_file_name, e_set_format, e_start_preview_first, e_stop_player_first, e_stop_preview_first, e_stop_recording_first, e_stop_reencoding_first, e_storage_path_read_only, e_streaming_type_not_specified, e_third_party_filter_already_inserted, 
	e_third_party_filter_error, e_trace_log, e_tv_command_not_allowed_during_tv_tuning, e_tuner_input_not_selected, e_TVideoGrabber_Filter_obsolete, e_value_out_of_range, e_video_compressor_not_suitable, e_window_transparency_failed, e_invalid_size, e_invalid_window_handle, e_tuner_mode_not_supported, e_publishing_point_connection_failed, e_speaker_control_disabled, i_audio_device_associated_to_video_device, i_begin_discovering_device, i_binding_device_or_compressor, i_discovering_device, i_end_discovering_device, i_preallocated_file_size_large_enough, i_preallocated_file_size_changed, i_preallocated_file_not_suitable, i_streaming_to_publishing_point, i_third_party_filter_inserted, i_using_ASF_Profile, i_recording_videosubtype, i_ismpegstream, 
	i_new_recording_filename, i_using_property_group, i_streaming_client_connected, i_streaming_client_disconnected, i_refreshing_preview, i_recording_on_motion, i_window_found, i_limiting_preview, i_codec_recommended, i_tuner_mode, i_DV_date_time_discontinuity, w_cannot_connect_thirdparty_filter, w_cannot_connect_thirdparty_renderer, w_cannot_instantiate_thirdparty_filter, w_cannot_route_crossbar, w_cannot_use_color_key, w_command_delayed, w_does_not_apply_to_dv, w_find_audio_device, w_filter_does_not_save_properties, w_frame_grabber_requires_CPU, w_hold_recording, w_information, w_not_playing, w_player_audio_should_be_disabled, w_recording_cancelled_by_user, w_can_pause_and_ASF_incompatible, w_set_audio_format, w_storage_path_on_network, w_tv_tuner, 
	w_using_nearest_video_size, w_divx_codec_not_installed, w_codec_does_not_support_debugger, w_divx_codec_profile, w_device_partially_supported, w_excessive_grid_size, w_grid_too_large_for_dialog, w_operation_may_lock, w_audio_streaming_needs_audiorecording_property_enabled, w_network_streaming_disabled, w_server_lost_next_retry, w_overlay_mixer_not_available, w_network_streaming_change_requires_application_to_be_restarted, w_standard_renderer_recommended, w_window_transparency_and_recording_not_recommended, w_clip_not_seekable, w_only_WMV_recording_during_network_streaming, w_check_analog_video_standard, w_recording_timer_set, w_stream_time_beyong_script_time, w_generate_new_file, w_hires_timer_not_available, w_applies_to_the_current_recording_method, 
	i_leaving_full_screen_mode };
#pragma option pop

#pragma option push -b
enum TRegistryRoot { RR_HKEY_CURRENT_USER, RR_HKEY_LOCAL_MACHINE };
#pragma option pop

#pragma option push -b
enum TAspectRatio { ar_Box, ar_NoResize, ar_Stretch, ar_PanScan };
#pragma option pop

#pragma option push -b
enum TTextOverlayAlign { tf_Left, tf_Center, tf_Right };
#pragma option pop

#pragma option push -b
enum TVUMeterSetting { vu_Handle, vu_WarningPercent, vu_PeakPercent, vu_BkgndColor, vu_NormalColor, vu_WarningColor, vu_PeakColor, vu_TickSize, vu_TickInterval, vu_NeedleThickness };
#pragma option pop

#pragma option push -b
enum TVMR9ImageAdjustment { vmr9_Brightness, vmr9_Contrast, vmr9_Hue, vmr9_Saturation, vmr9_Alpha };
#pragma option pop

#pragma option push -b
enum TVideoWindowNotify { vwActive, vwVisible, vwAutoSize, vwEmbedded, vwColorKeyEnabled, vwAlphaBlendEnabled, vwFullScreen, vwStayOnTop, vwMouseMovesWindow, vwVideoPortEnabled, vwMonitor, vwAspectRatio, vwVideoWidth, vwVideoHeight, vwPanScanRatio, vwColorKeyValue, vwAlphaBlendValue, vwLeft, vwTop, vwLocation };
#pragma option pop

#pragma option push -b
enum TTunerMode { tm_TVTuner, tm_FMRadioTuner, tm_AMRadioTuner, tm_DigitalSatelliteTuner };
#pragma option pop

typedef void __fastcall (__closure *TOnAudioPeak)(System::TObject* Sender, double Left_Percent, double Left_DB, double Right_Percent, double Right_DB);

typedef void __fastcall (__closure *TOnClientConnection)(System::TObject* Sender, bool Connected, System::UnicodeString ClientInfo);

typedef void __fastcall (__closure *TOnEventNotification)(System::TObject* Sender);

typedef void __fastcall (__closure *TOnDVDiscontinuity)(System::TObject* Sender, bool &DeliverNewFrame);

typedef void __fastcall (__closure *TOnLog)(System::TObject* Sender, TLogType LogType, System::UnicodeString Severity, System::UnicodeString InfoMsg);

typedef void __fastcall (__closure *TOnSourceFileToDestFileStarted)(System::TObject* Sender, System::UnicodeString SourceFile, System::UnicodeString DestFile);

typedef void __fastcall (__closure *TOnSourceFileToDestFileCompleted)(System::TObject* Sender, System::UnicodeString SourceFile, System::UnicodeString DestFile, bool Success);

typedef void __fastcall (__closure *TOnFileNotification)(System::TObject* Sender, System::UnicodeString FileName);

typedef void __fastcall (__closure *TOnCreatePreallocatedFileCompleted)(System::TObject* Sender, System::UnicodeString FileName, bool Success);

typedef void __fastcall (__closure *TOnResizeVideo)(System::TObject* Sender, int SourceWidth, int SourceHeight);

typedef void __fastcall (__closure *TOnRecordingCompleted)(System::TObject* Sender, System::UnicodeString FileName, bool Success);

typedef void __fastcall (__closure *TOnEnumerateWindows)(System::TObject* Sender, int WindowHandle, System::UnicodeString WindowName, System::UnicodeString WindowClass);

typedef void __fastcall (__closure *TOnVideoFromBitmapsNextFrameNeeded)(System::TObject* Sender, bool FirstSample);

typedef void __fastcall (__closure *TOnDVCommandCompleted)(System::TObject* Sender, int NewStateValue, System::UnicodeString NewStateLabel);

typedef void __fastcall (__closure *TOnDeviceArrivalOrRemoval)(System::TObject* Sender, bool IsDeviceArrival, bool IsVideoDevice, System::UnicodeString DeviceName, int DeviceIndex);

typedef void __fastcall (__closure *TOnPlayerBufferingData)(System::TObject* Sender, bool StartingToBuffer);

typedef void __fastcall (__closure *TOnDirectNetworkStreamingHostUrl)(System::TObject* Sender, System::UnicodeString HostUrl, System::UnicodeString HostName, int HostPort);

typedef void __fastcall (__closure *TOnAuthenticationNeeded)(System::TObject* Sender, TAuthenticationType AuthenticationType, System::UnicodeString Realm, System::UnicodeString Server, System::UnicodeString &Username, System::UnicodeString &Password);

typedef void __fastcall (__closure *TOnVideoCompressionSettings)(System::TObject* Sender, bool CanKeyFrameRate, bool CanPFramesPerKeyFrame, bool CanQuality, bool CanWindowSize, bool CanSpecifyDataRate, int DefaultFrameRate, int DefaultPFramesPerKey, double DefaultQuality, int DefaultWindowSize, int DefaultDataRate);

typedef void __fastcall (__closure *TOnColorKeyChange)(System::TObject* Sender, Graphics::TColor ColorKey);

typedef void __fastcall (__closure *TOnPlayerStateChanged)(System::TObject* Sender, TPlayerState OldPlayerState, TPlayerState NewPlayerState);

typedef void __fastcall (__closure *TOnTVChannelSelected)(System::TObject* Sender, int Channel, bool Locked, int DefaultVideoFrequency, int OverriddenVideoFrequency, int TunerVideoFrequency, int TunerAudioFrequency);

typedef void __fastcall (__closure *TOnTVChannelScanStarted)(System::TObject* Sender, int MinChannel, int MaxChannel);

typedef void __fastcall (__closure *TOnThirdPartyFilterConnected)(System::TObject* Sender, TThirdPartyFilterList Location, System::_di_IInterface Intf);

typedef void __fastcall (__closure *TOnFilterSelected)(System::TObject* Sender, System::UnicodeString FilterName, bool &RejectFilter);

typedef void __fastcall (__closure *TOnFrameCaptureCompleted)(System::TObject* Sender, void * FrameBitmap, int BitmapWidth, int BitmapHeight, unsigned FrameNumber, __int64 FrameTime, TFrameCaptureDest DestType, System::UnicodeString FileName, bool Success, int FrameId);

typedef void __fastcall (__closure *TOnFrameProgress)(System::TObject* Sender, unsigned FrameNumber, __int64 FrameTime, int FrameId);

typedef void __fastcall (__closure *TOnAVIDurationUpdated)(System::TObject* Sender, System::UnicodeString FileName, unsigned FrameCount, double &FrameRate, __int64 &Duration);

typedef void __fastcall (__closure *TOnMotionDetected)(System::TObject* Sender, double GlobalMotionRatio, int MaxMotionCellX, int MaxMotionCellY, void * FrameBitmap, unsigned FrameNumber, __int64 FrameTime, int FrameId, bool &CaptureFrame);

typedef void __fastcall (__closure *TOnMotionNotDetected)(System::TObject* Sender, void * FrameBitmap, unsigned FrameNumber, __int64 FrameTime, int FrameId, bool &CaptureFrame);

typedef void __fastcall (__closure *TOnProgress)(System::TObject* Sender, int Percent, __int64 Position, __int64 Duration);

typedef void __fastcall (__closure *TOnProgressCommented)(System::TObject* Sender, int Percent, __int64 Position, __int64 Duration, System::UnicodeString Comment);

typedef void __fastcall (__closure *TOnFrameOverlayUsingDC)(System::TObject* Sender, HDC Dc, unsigned FrameNumber, __int64 FrameTime, int FrameId);

typedef void __fastcall (__closure *TOnFrameOverlayUsingDIB)(System::TObject* Sender, HBITMAP FrameBitmapHandle, unsigned FrameNumber, __int64 FrameTime, int FrameId);

typedef void __fastcall (__closure *TOnFrameOverlayUsingVIDEOHDR)(System::TObject* Sender, unsigned FrameNumber, __int64 FrameTime, int FrameId, const TVideoHdr &VideoHdr);

typedef void __fastcall (__closure *TOnRawAudioSample)(System::TObject* Sender, void * pSampleBuffer, int SampleBufferSize, int SampleDataLength, TFormatType FormatType, void * pFormat, void * pWaveFormatEx, __int64 SampleStartTime, __int64 SampleStopTime);

typedef void __fastcall (__closure *TOnRawVideoSample)(System::TObject* Sender, void * pSampleBuffer, int SampleBufferSize, int SampleDataLength, TFormatType FormatType, void * pFormat, void * pBitmapInfoHeader, __int64 SampleStartTime, __int64 SampleStopTime);

typedef void __fastcall (__closure *TOnVideoKeyPress)(System::TObject* Sender, int VideoWindow, System::WideChar &Key, int PhysicalKey, Classes::TShiftState ShiftState);

typedef void __fastcall (__closure *TOnVideoMouseUpDown)(System::TObject* Sender, int VideoWindow, Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int Y);

typedef void __fastcall (__closure *TOnVideoMouseMove)(System::TObject* Sender, int VideoWindow, Classes::TShiftState Shift, int X, int Y);

typedef void __fastcall (__closure *TOnMouseWheel)(System::TObject* Sender, int VideoWindow, bool DirectionUp, int X, int Y);

typedef void __fastcall (__closure *TOnTextOverlayScrollingCompleted)(System::TObject* Sender, int TextOverlayIndex);

typedef void __fastcall (__closure *TOnFrameBitmap)(System::TObject* Sender, pFrameInfo FrameInfo, pFrameBitmapInfo BitmapInfo);

typedef void __fastcall (__closure *TOnFrameProgress2)(System::TObject* Sender, pFrameInfo FrameInfo);

class DELPHICLASS TVideoGrabber;
class PASCALIMPLEMENTATION TVideoGrabber : public Extctrls::TCustomPanel
{
	typedef Extctrls::TCustomPanel inherited;
	
private:
	void *_Vdgr;
	bool _VdgrInitialized;
	unsigned FCurThr;
	bool FSettingBounds;
	TOnEventNotification FOnAudioDeviceSelected;
	TOnAudioPeak FOnAudioPeak;
	TOnAuthenticationNeeded FOnAuthenticationNeeded;
	TOnAVIDurationUpdated FOnAVIDurationUpdated;
	TOnEventNotification FOnBacktimedFramesCountReached;
	TOnEventNotification FOnClick;
	TOnClientConnection FOnClientConnection;
	TOnColorKeyChange FOnColorKeyChange;
	TOnSourceFileToDestFileCompleted FOnCopyPreallocDataCompleted;
	TOnProgress FOnCopyPreallocDataProgress;
	TOnSourceFileToDestFileStarted FOnCopyPreallocDataStarted;
	TOnCreatePreallocatedFileCompleted FOnCreatePreallocFileCompleted;
	TOnProgress FOnCreatePreallocFileProgress;
	TOnFileNotification FOnCreatePreallocFileStarted;
	TOnEventNotification FOnDblClick;
	TOnDeviceArrivalOrRemoval FOnDeviceArrivalOrRemoval;
	TOnEventNotification FOnDeviceLost;
	TOnDirectNetworkStreamingHostUrl FOnDirectNetworkStreamingHostUrl;
	TOnEventNotification FOnDiskFull;
	TOnDVDiscontinuity FOnDVDiscontinuity;
	TOnEnumerateWindows FOnEnumerateWindows;
	TOnEventNotification FOnFirstFrameReceived;
	TOnFrameBitmap FOnFrameBitmap;
	TOnFrameOverlayUsingDC FOnFrameOverlayUsingDC;
	TOnFrameOverlayUsingDIB FOnFrameOverlayUsingDIB;
	TOnFrameProgress FOnFrameProgress;
	TOnFrameProgress2 FOnFrameProgress2;
	TOnFrameOverlayUsingVIDEOHDR FOnFrameOverlayUsingVIDEOHDR;
	TOnEventNotification FOnInactive;
	TOnEventNotification FOnLastCommandCompleted;
	TOnEventNotification FOnGraphBuilt;
	TOnLog FOnLog;
	TOnFilterSelected FOnFilterSelected;
	TOnFrameCaptureCompleted FOnFrameCaptureCompleted;
	TOnDVCommandCompleted FOnDVCommandCompleted;
	TOnVideoKeyPress FOnKeyPress;
	TOnVideoMouseUpDown FOnMouseDown;
	TOnVideoMouseMove FOnMouseMove;
	TOnVideoMouseUpDown FOnMouseUp;
	TOnMouseWheel FOnMouseWheel;
	TOnEventNotification FOnNoVideoDevices;
	TOnPlayerStateChanged FOnPlayerStateChanged;
	TOnEventNotification FOnPlayerEndOfStream;
	TOnEventNotification FOnPlayerEndOfPlaylist;
	TOnPlayerBufferingData FOnPlayerBufferingData;
	TOnEventNotification FOnPlayerOpened;
	TOnFrameProgress FOnPlayerUpdateTrackbarPosition;
	TOnEventNotification FOnPreviewStarted;
	TOnRawAudioSample FOnRawAudioSample;
	TOnRawVideoSample FOnRawVideoSample;
	TOnProgressCommented FOnBitmapsLoadingProgress;
	TOnRecordingCompleted FOnRecordingCompleted;
	TOnEventNotification FOnRecordingPaused;
	TOnEventNotification FOnRecordingReadyToStart;
	TOnFileNotification FOnRecordingStarted;
	TOnSourceFileToDestFileCompleted FOnReencodingCompleted;
	TOnProgress FOnReencodingProgress;
	TOnSourceFileToDestFileStarted FOnReencodingStarted;
	TOnResizeVideo FOnResizeVideo;
	TOnEventNotification FOnReinitializing;
	TOnTextOverlayScrollingCompleted FOnTextOverlayScrollingCompleted;
	TOnTVChannelSelected FOnTVChannelSelected;
	TOnEventNotification FOnTVChannelScanCompleted;
	TOnTVChannelScanStarted FOnTVChannelScanStarted;
	TOnThirdPartyFilterConnected FOnThirdPartyFilterConnected;
	TOnVideoCompressionSettings FOnVideoCompressionSettings;
	TOnEventNotification FOnVideoDeviceSelected;
	TOnVideoFromBitmapsNextFrameNeeded FOnVideoFromBitmapsNextFrameNeeded;
	TOnMotionDetected FOnMotionDetected;
	TOnMotionNotDetected FOnMotionNotDetected;
	void __fastcall UpdateBorderStyle(Forms::TBorderStyle Value);
	void __fastcall ClearEvents(void);
	bool __fastcall GetAdjustOverlayAspectRatio(void);
	bool __fastcall GetAdjustPixelAspectRatio(void);
	TApplicationPriority __fastcall GetApplicationPriority(void);
	int __fastcall GetASFAudioBitRate(void);
	int __fastcall GetASFAudioChannels(void);
	int __fastcall GetASFBufferWindow(void);
	TASFDeinterlaceMode __fastcall GetASFDeinterlaceMode(void);
	bool __fastcall GetASFFixedFrameRate(void);
	System::UnicodeString __fastcall GetASFMediaServerPublishingPoint(void);
	System::UnicodeString __fastcall GetASFMediaServerTemplatePublishingPoint(void);
	bool __fastcall GetASFMediaServerRemovePublishingPointAfterDisconnect(void);
	int __fastcall GetASFNetworkPort(void);
	int __fastcall GetASFNetworkMaxUsers(void);
	int __fastcall GetASFProfile(void);
	TASFProfileVersion __fastcall GetASFProfileVersion(void);
	System::UnicodeString __fastcall GetASFProfileFromCustomFile(void);
	int __fastcall GetASFVideoBitRate(void);
	double __fastcall GetASFVideoFrameRate(void);
	int __fastcall GetASFVideoHeight(void);
	int __fastcall GetASFVideoMaxKeyFrameSpacing(void);
	int __fastcall GetASFVideoQuality(void);
	int __fastcall GetASFVideoWidth(void);
	double __fastcall GetAspectRatioToUse(void);
	TAudioChannelRenderMode __fastcall GetAudioChannelRenderMode(void);
	bool __fastcall GetAVIDurationUpdated(void);
	TAero __fastcall GetAero(void);
	bool __fastcall GetAssociateAudioAndVideoDevices(void);
	int __fastcall GetAudioDevice(void);
	bool __fastcall GetAudioDeviceRendering(void);
	TAudioFormat __fastcall GetAudioFormat(void);
	int __fastcall GetAudioRenderer(void);
	int __fastcall GetAudioStreamNumber(void);
	bool __fastcall GetAudioSyncAdjustmentEnabled(void);
	int __fastcall GetAudioSyncAdjustment(void);
	int __fastcall GetMixAudioSamplesLevel(int Index);
	int __fastcall GetMixer_MosaicLines(void);
	int __fastcall GetMixer_MosaicColumns(void);
	bool __fastcall GetAudioPeakEvent(void);
	bool __fastcall GetAudioRecording(void);
	TAudioSource __fastcall GetAudioSource(void);
	TVuMeter __fastcall GetVuMeter(void);
	bool __fastcall GetAutoConnectRelatedPins(void);
	TAutoFileName __fastcall GetAutoFileName(void);
	int __fastcall GetAutoFileNameMinDigits(void);
	System::UnicodeString __fastcall GetAutoFilePrefix(void);
	bool __fastcall GetAutoRefreshPreview(void);
	bool __fastcall GetAutoStartPlayer(void);
	bool __fastcall GetAVIFormatOpenDML(void);
	bool __fastcall GetAVIFormatOpenDMLCompatibilityIndex(void);
	Graphics::TColor __fastcall GetBackgroundColor(void);
	Forms::TBorderStyle __fastcall GetBorderStyle(void);
	int __fastcall GetBurstCount(void);
	int __fastcall GetBurstInterval(void);
	bool __fastcall GetBurstMode(void);
	TFrameCaptureDest __fastcall GetBurstType(void);
	bool __fastcall GetCameraControlSettings(void);
	System::UnicodeString __fastcall GetCaptureFileExt(void);
	Graphics::TColor __fastcall GetColorKey(void);
	bool __fastcall GetColorKeyEnabled(void);
	TCompressionMode __fastcall GetCompressionMode(void);
	TCompressionType __fastcall GetCompressionType(void);
	TAspectRatio __fastcall GetDisplayAspectRatio(int Index);
	bool __fastcall GetDisplayBoolProperties(int Index);
	int __fastcall GetDisplayLongProperties(int Index);
	int __fastcall GetDisplay2LongProperties(int Index);
	bool __fastcall GetDisplay2BoolProperties(int Index);
	bool __fastcall GetVideoDoubleBuffered(void);
	int __fastcall GetDroppedFramesPollingInterval(void);
	bool __fastcall GetDVDateTimeEnabled(void);
	int __fastcall GetDVDiscontinuityMinimumInterval(void);
	int __fastcall GetDVDTitle(void);
	TDVVideoFormat __fastcall GetDVEncoder_VideoFormat(void);
	TDVSize __fastcall GetDVEncoder_VideoResolution(void);
	TDVVideoStandard __fastcall GetDVEncoder_VideoStandard(void);
	bool __fastcall GetDVRgb219(void);
	bool __fastcall GetDVReduceFrameRate(void);
	bool __fastcall GetDVRecordingInNativeFormatSeparatesStreams(void);
	bool __fastcall GetDVTimeCodeEnabled(void);
	bool __fastcall GetEventNotificationSynchrone(void);
	bool __fastcall GetCropping_Enabled(void);
	int __fastcall GetCropping_Height(void);
	bool __fastcall GetCropping_Outbounds(void);
	int __fastcall GetCropping_Width(void);
	int __fastcall GetCropping_X(void);
	int __fastcall GetCropping_Y(void);
	double __fastcall GetCropping_Zoom(void);
	bool __fastcall GetFixFlickerOrBlackCapture(void);
	bool __fastcall GetFrameCaptureWithoutOverlay(void);
	int __fastcall GetFrameCaptureHeight(void);
	int __fastcall GetFrameCaptureWidth(void);
	int __fastcall GetFrameCaptureZoomSize(void);
	TFrameGrabber __fastcall GetFrameGrabber(void);
	TFrameGrabberRGBFormat __fastcall GetFrameGrabberRGBFormat(void);
	bool __fastcall GetFrameNumberStartsFromZero(void);
	double __fastcall GetFrameRate(void);
	bool __fastcall GetImageOverlay_AlphaBlend(void);
	int __fastcall GetImageOverlay_AlphaBlendValue(void);
	bool __fastcall GetImageOverlay_ChromaKey(void);
	int __fastcall GetImageOverlay_ChromaKeyLeewayPercent(void);
	int __fastcall GetImageOverlay_ChromaKeyRGBColor(void);
	int __fastcall GetImageOverlay_Height(void);
	int __fastcall GetImageOverlay_LeftLocation(void);
	double __fastcall GetImageOverlay_RotationAngle(void);
	bool __fastcall GetImageOverlay_StretchToVideoSize(void);
	int __fastcall GetImageOverlay_TopLocation(void);
	bool __fastcall GetImageOverlay_Transparent(void);
	int __fastcall GetImageOverlay_TransparentColorValue(void);
	bool __fastcall GetImageOverlay_UseTransparentColor(void);
	int __fastcall GetImageOverlay_Width(void);
	bool __fastcall GetImageOverlayEnabled(void);
	System::UnicodeString __fastcall GetIPCameraURL(void);
	Jpeg::TJPEGPerformance __fastcall GetJPEGPerformance(void);
	bool __fastcall GetJPEGProgressiveDisplay(void);
	int __fastcall GetJPEGQuality(void);
	System::UnicodeString __fastcall GetLicenseString(void);
	bool __fastcall GetLogoDisplayed(void);
	TLogoLayout __fastcall GetLogoLayout(void);
	System::UnicodeString __fastcall GetMotionDetector_Grid(void);
	bool __fastcall GetMotionDetector_ReduceVideoNoise(void);
	bool __fastcall GetMotionDetector_CompareBlue(void);
	bool __fastcall GetMotionDetector_CompareGreen(void);
	bool __fastcall GetMotionDetector_CompareRed(void);
	bool __fastcall GetMotionDetector_GreyScale(void);
	bool __fastcall GetMotionDetector_Enabled(void);
	double __fastcall GetMotionDetector_MaxDetectionsPerSecond(void);
	int __fastcall GetMotionDetector_ReduceCPULoad(void);
	bool __fastcall GetMotionDetector_Triggered(void);
	bool __fastcall GetMouseWheelEventEnabled(void);
	bool __fastcall GetMuteAudioRendering(void);
	bool __fastcall GetMultiplexedInputEmulation(void);
	TMultiplexedRole __fastcall GetMultiplexedRole(void);
	int __fastcall GetMultiplexedSwitchDelay(void);
	int __fastcall GetMultiplexedStabilizationDelay(void);
	int __fastcall GetMultiplexer(void);
	TNetworkStreaming __fastcall GetNetworkStreaming(void);
	TNetworkStreamingType __fastcall GetNetworkStreamingType(void);
	TNotificationMethod __fastcall GetNotificationMethod(void);
	int __fastcall GetNotificationSleepTime(void);
	Classes::TThreadPriority __fastcall GetNotificationPriority(void);
	bool __fastcall GetOverlayAfterTransform(void);
	bool __fastcall GetPlayerAudioRendering(void);
	TDVSize __fastcall GetPlayerDVSize(void);
	int __fastcall GetPlayerFastSeekSpeedRatio(void);
	System::UnicodeString __fastcall GetPlayerFileName(void);
	System::UnicodeString __fastcall GetPlayerForcedCodec(void);
	bool __fastcall GetPlayerRefreshPausedDisplay(void);
	double __fastcall GetPlayerRefreshPausedDisplayFrameRate(void);
	double __fastcall GetPlayerSpeedRatio(void);
	Comctrls::TTrackBar* __fastcall GetPlayerTrackBar(void);
	double __fastcall GetPlayerTrackBarScale(void);
	Menus::TPopupMenu* __fastcall Get_PopupMenu(void);
	Classes::TComponent* __fastcall GetPropertyPageSaver(void);
	bool __fastcall GetPlayerTrackBarSynchrone(void);
	bool __fastcall GetPreallocCapFileCopiedAfterRecording(void);
	bool __fastcall GetPreallocCapFileEnabled(void);
	System::UnicodeString __fastcall GetPreallocCapFileName(void);
	int __fastcall GetPreallocCapFileSizeInMB(void);
	int __fastcall GetPreviewZoomSize(void);
	int __fastcall GetRecordingAudioBitRate(void);
	int __fastcall GetRecordingVideoBitRate(void);
	System::UnicodeString __fastcall GetRecordingFileName(void);
	bool __fastcall GetRecordingInNativeFormat(void);
	TRecordingMethod __fastcall GetRecordingMethod(void);
	bool __fastcall GetRecordingPauseCreatesNewFile(void);
	bool __fastcall GetRecordingOnMotion_Enabled(void);
	double __fastcall GetRecordingOnMotion_MotionThreshold(void);
	int __fastcall GetRecordingOnMotion_NoMotionPauseDelayMs(void);
	TRecordingTimer __fastcall GetRecordingTimer(void);
	int __fastcall GetRecordingTimerInterval(void);
	bool __fastcall GetReencodingIncludeAudioStream(void);
	bool __fastcall GetReencodingIncludeVideoStream(void);
	TRecordingMethod __fastcall GetReencodingMethod(void);
	System::UnicodeString __fastcall GetReencodingNewVideoClip(void);
	System::UnicodeString __fastcall GetReencodingSourceVideoClip(void);
	bool __fastcall GetReencodingUseAudioCompressor(void);
	bool __fastcall GetReencodingUseFrameGrabber(void);
	bool __fastcall GetReencodingUseVideoCompressor(void);
	bool __fastcall GetReencodingWMVOutput(void);
	bool __fastcall GetQuickDeviceInitialization(void);
	bool __fastcall GetRawAudioSampleCapture(void);
	bool __fastcall GetRawCaptureAsyncEvent(void);
	TRawSampleCaptureLocation __fastcall GetRawSampleCaptureLocation(void);
	bool __fastcall GetRawVideoSampleCapture(void);
	TRecordingSize __fastcall GetRecordingSize(void);
	bool __fastcall GetScreenRecordingLayeredWindows(void);
	int __fastcall GetScreenRecordingMonitor(void);
	bool __fastcall GetScreenRecordingNonVisibleWindows(void);
	bool __fastcall GetScreenRecordingThroughClipboard(void);
	bool __fastcall GetScreenRecordingWithCursor(void);
	Extctrls::TShape* __fastcall GetShapeOverlay(void);
	bool __fastcall GetShapeOverlayEnabled(void);
	bool __fastcall GetSpeakerControl(void);
	System::UnicodeString __fastcall GetStoragePath(void);
	bool __fastcall GetStoreDeviceSettingsInRegistry(void);
	Classes::TStream* __fastcall GetSourceStream(void);
	TSynchronizationRole __fastcall GetSynchronizationRole(void);
	bool __fastcall GetSynchronized(void);
	bool __fastcall GetSyncCommands(void);
	TSyncPreview __fastcall GetSyncPreview(void);
	TTextOverlayAlign __fastcall GetTextOverlayAlign(void);
	Graphics::TColor __fastcall GetTextOverlayBkColor(void);
	bool __fastcall GetTextOverlayEnabled(void);
	Graphics::TFont* __fastcall GetTextOverlayFont(void);
	int __fastcall GetTextOverlayLeft(void);
	int __fastcall GetTextOverlayRight(void);
	bool __fastcall GetTextOverlayScrolling(void);
	int __fastcall GetTextOverlayScrollingSpeed(void);
	bool __fastcall GetTextOverlayShadow(void);
	Graphics::TColor __fastcall GetTextOverlayShadowColor(void);
	TCardinalDirection __fastcall GetTextOverlayShadowDirection(void);
	System::UnicodeString __fastcall GetTextOverlayString(void);
	int __fastcall GetTextOverlayTop(void);
	bool __fastcall GetTextOverlayTransparent(void);
	System::UnicodeString __fastcall GetThirdPartyDeinterlacer(void);
	bool __fastcall GetTranslateMouseCoordinates(void);
	bool __fastcall GetTVUseFrequencyOverrides(void);
	int __fastcall GetTunerFrequency(void);
	bool __fastcall GetUseClock(void);
	System::UnicodeString __fastcall GetVersion(void);
	int __fastcall GetVideoCompression_DataRate(void);
	int __fastcall GetVideoCompression_KeyFrameRate(void);
	int __fastcall GetVideoCompression_PFramesPerKeyFrame(void);
	double __fastcall GetVideoCompression_Quality(void);
	int __fastcall GetVideoCompression_WindowSize(void);
	bool __fastcall GetVideoControlSettings(void);
	bool __fastcall GetVideoFromImages_RepeatIndefinitely(void);
	System::UnicodeString __fastcall GetVideoFromImages_SourceDirectory(void);
	TFileSort __fastcall GetVideoFromImages_BitmapsSortedBy(void);
	System::UnicodeString __fastcall GetVideoFromImages_TemporaryFile(void);
	int __fastcall GetVideoDevice(void);
	int __fastcall GetAnalogVideoStandard(void);
	int __fastcall GetAudioCompressor(void);
	int __fastcall GetAudioInput(void);
	int __fastcall GetVideoCompressor(void);
	int __fastcall GetVideoFormat(void);
	int __fastcall GetVideoInput(void);
	int __fastcall GetVideoSize(void);
	int __fastcall GetVideoSubtype(void);
	int __fastcall GetVideoProcessingBrightness(void);
	int __fastcall GetVideoProcessingContrast(void);
	TVideoDeinterlacing __fastcall GetVideoProcessingDeinterlacing(void);
	bool __fastcall GetVideoProcessingGrayScale(void);
	int __fastcall GetVideoProcessingHue(void);
	bool __fastcall GetVideoProcessingInvertColors(void);
	bool __fastcall GetVideoProcessingLeftRight(void);
	int __fastcall GetVideoProcessingPixellization(void);
	TVideoRotation __fastcall GetVideoProcessingRotation(void);
	double __fastcall GetVideoProcessingRotationCustomAngle(void);
	int __fastcall GetVideoProcessingSaturation(void);
	bool __fastcall GetVideoProcessingTopDown(void);
	bool __fastcall GetVideoQualitySettings(void);
	TVideoRenderer __fastcall GetVideoRenderer(void);
	TVideoRendererExternal __fastcall GetVideoRendererExternal(void);
	int __fastcall GetVideoRendererExternalIndex(void);
	TVideoSource __fastcall GetVideoSource(void);
	System::UnicodeString __fastcall GetVideoSource_FileOrURL(void);
	__int64 __fastcall GetVideoSource_FileOrURL_StartTime(void);
	__int64 __fastcall GetVideoSource_FileOrURL_StopTime(void);
	bool __fastcall GetVideoVisibleWhenStopped(void);
	int __fastcall GetVirtualVideoStreamControl(void);
	int __fastcall GetVirtualAudioStreamControl(void);
	TWebcamStillCaptureButton __fastcall GetWebcamStillCaptureButton(void);
	int __fastcall GetZoomCoeff(void);
	int __fastcall GetZoomXCenter(void);
	int __fastcall GetZoomYCenter(void);
	void __fastcall SetAdjustOverlayAspectRatio(bool Value);
	void __fastcall SetAdjustPixelAspectRatio(bool Value);
	void __fastcall SetApplicationPriority(TApplicationPriority Value);
	void __fastcall SetASFAudioBitRate(int Value);
	void __fastcall SetASFAudioChannels(int Value);
	void __fastcall SetASFBufferWindow(int Value);
	void __fastcall SetASFDeinterlaceMode(TASFDeinterlaceMode Value);
	void __fastcall SetASFFixedFrameRate(bool Value);
	void __fastcall SetASFMediaServerPublishingPoint(System::UnicodeString Value);
	void __fastcall SetASFMediaServerTemplatePublishingPoint(System::UnicodeString Value);
	void __fastcall SetASFMediaServerRemovePublishingPointAfterDisconnect(bool Value);
	void __fastcall SetASFNetworkPort(int Value);
	void __fastcall SetASFNetworkMaxUsers(int Value);
	void __fastcall SetASFProfile(int Value);
	void __fastcall SetASFProfileVersion(TASFProfileVersion Value);
	void __fastcall SetASFProfileFromCustomFile(System::UnicodeString Value);
	void __fastcall SetASFVideoBitRate(int Value);
	void __fastcall SetASFVideoFrameRate(double Value);
	void __fastcall SetASFVideoHeight(int Value);
	void __fastcall SetASFVideoMaxKeyFrameSpacing(int Value);
	void __fastcall SetASFVideoQuality(int Value);
	void __fastcall SetASFVideoWidth(int Value);
	void __fastcall SetAspectRatioToUse(double Value);
	void __fastcall SetAVIDurationUpdated(bool Value);
	void __fastcall SetAero(TAero Value);
	void __fastcall SetAssociateAudioAndVideoDevices(bool Value);
	void __fastcall SetAudioDevice(int Value);
	void __fastcall SetAudioRenderer(int Value);
	void __fastcall SetAudioStreamNumber(int Value);
	void __fastcall SetAudioSyncAdjustmentEnabled(bool Value);
	void __fastcall SetAudioSyncAdjustment(int Value);
	void __fastcall SetAudioDeviceRendering(bool Value);
	void __fastcall SetAudioFormat(TAudioFormat Value);
	void __fastcall SetMixAudioSamplesLevel(int Index, int Value);
	void __fastcall SetMixer_MosaicLines(int Value);
	void __fastcall SetMixer_MosaicColumns(int Value);
	void __fastcall SetAudioChannelRenderMode(TAudioChannelRenderMode Value);
	void __fastcall SetAudioPeakEvent(bool Value);
	void __fastcall SetAudioRecording(bool Value);
	void __fastcall SetAudioSource(TAudioSource Value);
	void __fastcall SetVuMeter(TVuMeter Value);
	void __fastcall SetAutoConnectRelatedPins(bool Value);
	void __fastcall SetAutoFileName(TAutoFileName Value);
	void __fastcall SetAutoFileNameMinDigits(int Value);
	void __fastcall SetAutoFilePrefix(System::UnicodeString Value);
	void __fastcall SetAutoRefreshPreview(bool Value);
	void __fastcall SetAutoStartPlayer(bool Value);
	void __fastcall SetAVIFormatOpenDML(bool Value);
	void __fastcall SetAVIFormatOpenDMLCompatibilityIndex(bool Value);
	void __fastcall SetBackgroundColor(Graphics::TColor Value);
	HIDESBASE void __fastcall SetBorderStyle(Forms::TBorderStyle Value);
	void __fastcall SetBurstCount(int Value);
	void __fastcall SetBurstInterval(int Value);
	void __fastcall SetBurstMode(bool Value);
	void __fastcall SetBurstType(TFrameCaptureDest Value);
	void __fastcall SetCameraControlSettings(bool Value);
	void __fastcall SetCaptureFileExt(System::UnicodeString Value);
	void __fastcall SetColorKey(Graphics::TColor Value);
	void __fastcall SetColorKeyEnabled(bool Value);
	void __fastcall SetCompressionMode(TCompressionMode Value);
	void __fastcall SetCompressionType(TCompressionType Value);
	void __fastcall SetDisplayAspectRatio(int Index, TAspectRatio Value);
	void __fastcall SetDisplayBoolProperties(int Index, bool Value);
	void __fastcall SetDisplayLongProperties(int Index, int Value);
	void __fastcall SetDisplay2BoolProperties(int Index, bool Value);
	void __fastcall SetDisplay2LongProperties(int Index, int Value);
	void __fastcall SetVideoDoubleBuffered(bool Value);
	void __fastcall SetDroppedFramesPollingInterval(int Value);
	void __fastcall SetDVDateTimeEnabled(bool Value);
	void __fastcall SetDVDiscontinuityMinimumInterval(int Value);
	void __fastcall SetDVDTitle(int Value);
	void __fastcall SetDVEncoder_VideoFormat(TDVVideoFormat Value);
	void __fastcall SetDVEncoder_VideoResolution(TDVSize Value);
	void __fastcall SetDVEncoder_VideoStandard(TDVVideoStandard Value);
	void __fastcall SetDVReduceFrameRate(bool Value);
	void __fastcall SetDVRecordingInNativeFormatSeparatesStreams(bool Value);
	void __fastcall SetDVTimeCodeEnabled(bool Value);
	void __fastcall SetDVRgb219(bool Value);
	void __fastcall SetEventNotificationSynchrone(bool Value);
	void __fastcall SetCropping_Enabled(bool Value);
	void __fastcall SetCropping_Height(int Value);
	void __fastcall SetCropping_Outbounds(bool Value);
	void __fastcall SetCropping_Width(int Value);
	void __fastcall SetCropping_X(int Value);
	void __fastcall SetCropping_Y(int Value);
	void __fastcall SetCropping_Zoom(double Value);
	void __fastcall SetFixFlickerOrBlackCapture(bool Value);
	void __fastcall SetFrameCaptureWithoutOverlay(bool Value);
	void __fastcall SetFrameCaptureHeight(int Value);
	void __fastcall SetFrameCaptureWidth(int Value);
	void __fastcall SetFrameCaptureZoomSize(int Value);
	void __fastcall SetFrameGrabber(TFrameGrabber Value);
	void __fastcall SetFrameGrabberRGBFormat(TFrameGrabberRGBFormat Value);
	void __fastcall SetFrameNumberStartsFromZero(bool Value);
	void __fastcall SetFrameRate(double Value);
	void __fastcall SetImageOverlay_AlphaBlend(bool Value);
	void __fastcall SetImageOverlay_AlphaBlendValue(int Value);
	void __fastcall SetImageOverlay_ChromaKey(bool Value);
	void __fastcall SetImageOverlay_ChromaKeyLeewayPercent(int Value);
	void __fastcall SetImageOverlay_ChromaKeyRGBColor(int Value);
	void __fastcall SetImageOverlay_Height(int Value);
	void __fastcall SetImageOverlay_LeftLocation(int Value);
	void __fastcall SetImageOverlay_RotationAngle(double Value);
	void __fastcall SetImageOverlay_StretchToVideoSize(bool Value);
	void __fastcall SetImageOverlay_TopLocation(int Value);
	void __fastcall SetImageOverlay_Transparent(bool Value);
	void __fastcall SetImageOverlay_TransparentColorValue(int Value);
	void __fastcall SetImageOverlay_UseTransparentColor(bool Value);
	void __fastcall SetImageOverlay_Width(int Value);
	void __fastcall SetImageOverlayEnabled(bool Value);
	void __fastcall SetIPCameraURL(System::UnicodeString Value);
	void __fastcall SetJPEGPerformance(Jpeg::TJPEGPerformance Value);
	void __fastcall SetJPEGProgressiveDisplay(bool Value);
	void __fastcall SetJPEGQuality(int Value);
	void __fastcall SetLicenseString(System::UnicodeString Value);
	void __fastcall SetLogoDisplayed(bool Value);
	void __fastcall SetLogoLayout(TLogoLayout Value);
	void __fastcall SetMotionDetector_Grid(System::UnicodeString Value);
	void __fastcall SetMotionDetector_CompareBlue(bool Value);
	void __fastcall SetMotionDetector_CompareGreen(bool Value);
	void __fastcall SetMotionDetector_GreyScale(bool Value);
	void __fastcall SetMotionDetector_CompareRed(bool Value);
	void __fastcall SetMotionDetector_Enabled(bool Value);
	void __fastcall SetMotionDetector_MaxDetectionsPerSecond(double Value);
	void __fastcall SetMotionDetector_ReduceVideoNoise(bool Value);
	void __fastcall SetMotionDetector_ReduceCPULoad(int Value);
	void __fastcall SetMotionDetector_Triggered(bool Value);
	void __fastcall SetMouseWheelEventEnabled(bool Value);
	void __fastcall SetMultiplexedInputEmulation(bool Value);
	void __fastcall SetMultiplexedRole(TMultiplexedRole Value);
	void __fastcall SetMultiplexedSwitchDelay(int Value);
	void __fastcall SetMultiplexedStabilizationDelay(int Value);
	void __fastcall SetMultiplexer(int Value);
	void __fastcall SetMuteAudioRendering(bool Value);
	void __fastcall SetNetworkStreaming(TNetworkStreaming Value);
	void __fastcall SetNetworkStreamingType(TNetworkStreamingType Value);
	void __fastcall SetNotificationMethod(TNotificationMethod Value);
	void __fastcall SetNotificationSleepTime(int Value);
	void __fastcall SetNotificationPriority(Classes::TThreadPriority Value);
	void __fastcall Set_OnDeviceArrivalOrRemoval(TOnDeviceArrivalOrRemoval Value);
	void __fastcall SetOverlayAfterTransform(bool Value);
	void __fastcall SetPlayerAudioRendering(bool Value);
	void __fastcall SetPlayerDVSize(TDVSize Value);
	void __fastcall SetPlayerFastSeekSpeedRatio(int Value);
	void __fastcall SetPlayerFileName(System::UnicodeString Value);
	void __fastcall SetPlayerForcedCodec(System::UnicodeString Value);
	void __fastcall SetPlayerRefreshPausedDisplay(bool Value);
	void __fastcall SetPlayerRefreshPausedDisplayFrameRate(double Value);
	void __fastcall SetPlayerSpeedRatio(double Value);
	void __fastcall SetPlayerTrackBar(Comctrls::TTrackBar* Value);
	HIDESBASE void __fastcall SetPopupMenu(Menus::TPopupMenu* Value);
	void __fastcall SetPropertyPageSaver(Classes::TComponent* Value);
	void __fastcall SetPlayerTrackBarSynchrone(bool Value);
	void __fastcall SetPreallocCapFileCopiedAfterRecording(bool Value);
	void __fastcall SetPreallocCapFileEnabled(bool Value);
	void __fastcall SetPreallocCapFileName(System::UnicodeString Value);
	void __fastcall SetPreallocCapFileSizeInMB(int Value);
	void __fastcall SetPreviewZoomSize(int Value);
	void __fastcall SetRecordingAudioBitRate(int Value);
	void __fastcall SetRecordingVideoBitRate(int Value);
	void __fastcall SetRecordingFileName(System::UnicodeString Value);
	void __fastcall SetRecordingInNativeFormat(bool Value);
	void __fastcall SetRecordingMethod(TRecordingMethod Value);
	void __fastcall SetRecordingPauseCreatesNewFile(bool Value);
	void __fastcall SetRecordingOnMotion_Enabled(bool Value);
	void __fastcall SetRecordingOnMotion_MotionThreshold(double Value);
	void __fastcall SetRecordingOnMotion_NoMotionPauseDelayMs(int Value);
	void __fastcall SetRecordingTimer(TRecordingTimer Value);
	void __fastcall SetRecordingTimerInterval(int Value);
	void __fastcall SetReencodingNewVideoClip(System::UnicodeString Value);
	void __fastcall SetReencodingSourceVideoClip(System::UnicodeString Value);
	void __fastcall SetReencodingIncludeAudioStream(bool Value);
	void __fastcall SetReencodingIncludeVideoStream(bool Value);
	void __fastcall SetReencodingMethod(TRecordingMethod Value);
	void __fastcall SetReencodingUseAudioCompressor(bool Value);
	void __fastcall SetReencodingUseFrameGrabber(bool Value);
	void __fastcall SetReencodingUseVideoCompressor(bool Value);
	void __fastcall SetReencodingWMVOutput(bool Value);
	void __fastcall SetQuickDeviceInitialization(bool Value);
	void __fastcall SetRawAudioSampleCapture(bool Value);
	void __fastcall SetRawCaptureAsyncEvent(bool Value);
	void __fastcall SetRawSampleCaptureLocation(TRawSampleCaptureLocation Value);
	void __fastcall SetRawVideoSampleCapture(bool Value);
	void __fastcall SetRecordingSize(TRecordingSize Value);
	void __fastcall SetScreenRecordingLayeredWindows(bool Value);
	void __fastcall SetScreenRecordingMonitor(int Value);
	void __fastcall SetScreenRecordingNonVisibleWindows(bool Value);
	void __fastcall SetScreenRecordingThroughClipboard(bool Value);
	void __fastcall SetScreenRecordingWithCursor(bool Value);
	void __fastcall SetShapeOverlay(Extctrls::TShape* Value);
	void __fastcall SetShapeOverlayEnabled(bool Value);
	void __fastcall SetSpeakerControl(bool Value);
	void __fastcall SetStoragePath(System::UnicodeString Value);
	void __fastcall SetStoreDeviceSettingsInRegistry(bool Value);
	void __fastcall SetSourceStream(Classes::TStream* Value);
	void __fastcall SetSynchronizationRole(TSynchronizationRole Value);
	void __fastcall SetSynchronized(bool Value);
	void __fastcall SetSyncCommands(bool Value);
	void __fastcall SetSyncPreview(TSyncPreview Value);
	void __fastcall SetTextOverlayAlign(TTextOverlayAlign Value);
	void __fastcall SetTextOverlayBkColor(Graphics::TColor Value);
	void __fastcall SetTextOverlayEnabled(bool Value);
	void __fastcall SetTextOverlayFont(Graphics::TFont* Value);
	void __fastcall SetTextOverlayLeft(int Value);
	void __fastcall SetTextOverlayRight(int Value);
	void __fastcall SetTextOverlayScrolling(bool Value);
	void __fastcall SetTextOverlayScrollingSpeed(int Value);
	void __fastcall SetTextOverlayShadow(bool Value);
	void __fastcall SetTextOverlayShadowColor(Graphics::TColor Value);
	void __fastcall SetTextOverlayShadowDirection(TCardinalDirection Value);
	void __fastcall SetTextOverlayString(System::UnicodeString Value);
	void __fastcall SetTextOverlayTop(int Value);
	void __fastcall SetTextOverlayTransparent(bool Value);
	void __fastcall SetThirdPartyDeinterlacer(System::UnicodeString Value);
	void __fastcall SetTranslateMouseCoordinates(bool Value);
	void __fastcall SetTVUseFrequencyOverrides(bool Value);
	void __fastcall SetTunerFrequency(int Value);
	void __fastcall SetUseClock(bool Value);
	void __fastcall SetVersion(System::UnicodeString Value);
	void __fastcall SetVideoCompression_DataRate(int Value);
	void __fastcall SetVideoCompression_KeyFrameRate(int Value);
	void __fastcall SetVideoCompression_PFramesPerKeyFrame(int Value);
	void __fastcall SetVideoCompression_Quality(double Value);
	void __fastcall SetVideoCompression_WindowSize(int Value);
	void __fastcall SetVideoControlSettings(bool Value);
	void __fastcall SetVideoDevice(int Value);
	void __fastcall SetAnalogVideoStandard(int Value);
	void __fastcall SetAudioCompressor(int Value);
	void __fastcall SetAudioInput(int Value);
	void __fastcall SetVideoCompressor(int Value);
	void __fastcall SetVideoFormat(int Value);
	void __fastcall SetVideoInput(int Value);
	void __fastcall SetVideoSize(int Value);
	void __fastcall SetVideoSubtype(int Value);
	void __fastcall SetVideoFromImages_SourceDirectory(System::UnicodeString Value);
	void __fastcall SetVideoFromImages_TemporaryFile(System::UnicodeString Value);
	void __fastcall SetVideoFromImages_BitmapsSortedBy(TFileSort Value);
	void __fastcall SetVideoFromImages_RepeatIndefinitely(bool Value);
	void __fastcall SetVideoProcessingBrightness(int Value);
	void __fastcall SetVideoProcessingContrast(int Value);
	void __fastcall SetVideoProcessingDeinterlacing(TVideoDeinterlacing Value);
	void __fastcall SetVideoProcessingGrayScale(bool Value);
	void __fastcall SetVideoProcessingHue(int Value);
	void __fastcall SetVideoProcessingInvertColors(bool Value);
	void __fastcall SetVideoProcessingLeftRight(bool Value);
	void __fastcall SetVideoProcessingPixellization(int Value);
	void __fastcall SetVideoProcessingRotationCustomAngle(double Value);
	void __fastcall SetVideoProcessingRotation(TVideoRotation Value);
	void __fastcall SetVideoProcessingSaturation(int Value);
	void __fastcall SetVideoProcessingTopDown(bool Value);
	void __fastcall SetVideoQualitySettings(bool Value);
	void __fastcall SetVideoRenderer(TVideoRenderer Value);
	void __fastcall SetVideoRendererExternal(TVideoRendererExternal Value);
	void __fastcall SetVideoRendererExternalIndex(int Value);
	void __fastcall SetVideoSource(TVideoSource Value);
	void __fastcall SetVideoSource_FileOrURL(System::UnicodeString Value);
	void __fastcall SetVideoSource_FileOrURL_StartTime(__int64 Value);
	void __fastcall SetVideoSource_FileOrURL_StopTime(__int64 Value);
	void __fastcall SetVideoVisibleWhenStopped(bool Value);
	void __fastcall SetVirtualVideoStreamControl(int Value);
	void __fastcall SetVirtualAudioStreamControl(int Value);
	void __fastcall SetWebcamStillCaptureButton(TWebcamStillCaptureButton Value);
	void __fastcall SetZoomXCenter(int Value);
	void __fastcall SetZoomYCenter(int Value);
	void __fastcall SetZoomCoeff(int Value);
	Controls::TCursor __fastcall GetBusyCursor(void);
	void __fastcall SetBusyCursor(Controls::TCursor Value);
	Controls::TCursor __fastcall GetVideoCursor(void);
	void __fastcall SetVideoCursor(Controls::TCursor Value);
	Controls::TCursor __fastcall GetNormalCursor(void);
	void __fastcall SetNormalCursor(Controls::TCursor Value);
	System::UnicodeString __fastcall GetAnalogVideoStandards(void);
	int __fastcall GetAnalogVideoStandardsCount(void);
	int __fastcall GetBufferCount(void);
	System::UnicodeString __fastcall GetASFProfiles(void);
	int __fastcall GetASFProfilesCount(void);
	int __fastcall GetAudioBalance(void);
	System::UnicodeString __fastcall GetAudioCompressorName(void);
	int __fastcall GetAudioInputBalance(void);
	int __fastcall GetAudioInputLevel(void);
	bool __fastcall GetAudioInputMono(void);
	System::UnicodeString __fastcall GetAudioInputs(void);
	int __fastcall GetAudioInputsCount(void);
	System::UnicodeString __fastcall GetAudioDeviceName(void);
	System::UnicodeString __fastcall GetAudioFormats(void);
	System::UnicodeString __fastcall GetAudioRendererName(void);
	int __fastcall GetAudioVolume(void);
	bool __fastcall GetBusy(void);
	int __fastcall GetCropping_YMax(void);
	int __fastcall GetCropping_XMax(void);
	double __fastcall GetCurrentFrameRate(void);
	TCurrentState __fastcall GetCurrentState(void);
	__int64 __fastcall GetDeliveredFrames(void);
	System::UnicodeString __fastcall GetDirectShowFilters(void);
	int __fastcall GetDirectShowFiltersCount(void);
	int __fastcall GetDisplay_VideoHeight(void);
	int __fastcall GetDisplay_VideoWidth(void);
	int __fastcall GetDroppedFrameCount(void);
	HWND __fastcall GetDisplay_VideoWindowHandle(void);
	HWND __fastcall GetDualDisplay_VideoWindowHandle(void);
	System::UnicodeString __fastcall GetAudioCompressors(void);
	int __fastcall GetAudioCompressorsCount(void);
	System::UnicodeString __fastcall GetAudioDevices(void);
	int __fastcall GetAudioDevicesCount(void);
	System::UnicodeString __fastcall GetAudioRenderers(void);
	int __fastcall GetAudioRenderersCount(void);
	int __fastcall GetDualDisplay_VideoHeight(void);
	int __fastcall GetDualDisplay_VideoWidth(void);
	TFrameGrabberRGBFormat __fastcall GetFrameGrabberCurrentRGBFormat(void);
	bool __fastcall GetHoldRecording(void);
	int __fastcall GetImageOverlaySelector(void);
	double __fastcall GetImageRatio(void);
	bool __fastcall GetInFrameProgressEvent(void);
	bool __fastcall GetIsAnalogVideoDecoderAvailable(void);
	bool __fastcall GetIsAudioCrossbarAvailable(void);
	TTriState __fastcall GetIsAudioInputBalanceAvailable(void);
	bool __fastcall GetIsCameraControlAvailable(void);
	bool __fastcall GetIsDigitalVideoIn(void);
	bool __fastcall GetIsDVCommandAvailable(void);
	bool __fastcall GetIsHorizontalSyncLocked(void);
	bool __fastcall GetIsMpegStream(void);
	TTriState __fastcall GetIsPlayerAudioStreamAvailable(void);
	TTriState __fastcall GetIsPlayerVideoStreamAvailable(void);
	bool __fastcall GetIsRecordingPaused(void);
	bool __fastcall GetTimeCodeReaderAvailable(void);
	bool __fastcall GetIsTVAudioAvailable(void);
	bool __fastcall GetIsTVAutoTuneRunning(void);
	bool __fastcall GetIsTVTunerAvailable(void);
	bool __fastcall GetIsVideoControlAvailable(void);
	bool __fastcall GetIsVideoCrossbarAvailable(void);
	bool __fastcall GetIsVideoQualityAvailable(void);
	bool __fastcall GetIsVideoPortAvailable(void);
	bool __fastcall GetIsVideoInterlaced(void);
	bool __fastcall GetIsWDMVideoDriver(void);
	System::UnicodeString __fastcall GetLast_BurstFrameCapture_FileName(void);
	System::UnicodeString __fastcall GetLast_CaptureFrameTo_FileName(void);
	System::UnicodeString __fastcall GetLast_Recording_FileName(void);
	System::UnicodeString __fastcall GetLast_Clip_Played(void);
	double __fastcall GetMotionDetector_GlobalMotionRatio(void);
	int __fastcall GetMotionDetector_GridXCount(void);
	int __fastcall GetMotionDetector_GridYCount(void);
	bool __fastcall GetMotionDetector_IsGridValid(void);
	TMpegStreamType __fastcall GetMpegStreamType(void);
	System::UnicodeString __fastcall GetMultiplexerName(void);
	System::UnicodeString __fastcall GetMultiplexers(void);
	int __fastcall GetMultiplexersCount(void);
	int __fastcall GetPlaylistIndex(void);
	__int64 __fastcall GetPlayerDuration(void);
	__int64 __fastcall GetPlayerFrameCount(void);
	__int64 __fastcall GetPlayerFramePosition(void);
	__int64 __fastcall GetPlayerTimePosition(void);
	double __fastcall GetPlayerFrameRate(void);
	TPlayerState __fastcall GetPlayerState(void);
	System::UnicodeString __fastcall GetPropertyPageSaverGroup(void);
	int __fastcall GetRecordingBacktimedFramesCount(void);
	bool __fastcall GetRecordingCanPause(void);
	double __fastcall GetRecordingDuration(void);
	System::UnicodeString __fastcall GetRecordingFourCC(void);
	int __fastcall GetRecordingHeight(void);
	int __fastcall GetRecordingWidth(void);
	__int64 __fastcall GetReencodingStartFrame(void);
	__int64 __fastcall GetReencodingStartTime(void);
	__int64 __fastcall GetReencodingStopFrame(void);
	__int64 __fastcall GetReencodingStopTime(void);
	int __fastcall GetSendToDV_DeviceIndex(void);
	int __fastcall GetSpeakerBalance(void);
	int __fastcall GetSpeakerVolume(void);
	System::UnicodeString __fastcall GetStreamingURL(void);
	System::UnicodeString __fastcall GetSystemTempPath(void);
	int __fastcall GetTextOverlaySelector(void);
	int __fastcall GetTVChannel(void);
	int __fastcall GetTVCountryCode(void);
	TTunerInputType __fastcall GetTVTunerInputType(void);
	TTunerMode __fastcall GetTunerMode(void);
	int __fastcall GetUniqueID(void);
	bool __fastcall GetVCRHorizontalLocking(void);
	System::UnicodeString __fastcall GetVideoCompressorName(void);
	System::UnicodeString __fastcall GetVideoCompressors(void);
	int __fastcall GetVideoCompressorsCount(void);
	System::UnicodeString __fastcall GetVideoDevices(void);
	System::UnicodeString __fastcall GetVideoDevicesId(void);
	int __fastcall GetVideoDevicesCount(void);
	System::UnicodeString __fastcall GetVideoFormats(void);
	int __fastcall GetVideoFormatsCount(void);
	System::UnicodeString __fastcall GetVideoInputs(void);
	int __fastcall GetVideoInputsCount(void);
	System::UnicodeString __fastcall GetVideoDeviceName(void);
	System::UnicodeString __fastcall GetVideoSizes(void);
	int __fastcall GetVideoSizesCount(void);
	int __fastcall GetVideoHeight(void);
	int __fastcall GetVideoHeight_PreferredAspectRatio(void);
	int __fastcall GetVideoWidth(void);
	int __fastcall GetVideoWidth_PreferredAspectRatio(void);
	System::UnicodeString __fastcall GetVideoSources(void);
	int __fastcall GetVideoSourcesCount(void);
	System::UnicodeString __fastcall GetVideoSubtypes(void);
	int __fastcall GetVideoSubtypesCount(void);
	void __fastcall SetAudioBalance(int Value);
	void __fastcall SetAudioInputBalance(int Value);
	void __fastcall SetAudioInputLevel(int Value);
	void __fastcall SetAudioInputMono(bool Value);
	void __fastcall SetAudioVolume(int Value);
	void __fastcall SetBufferCount(int Value);
	void __fastcall SetHoldRecording(bool Value);
	void __fastcall SetImageOverlaySelector(int Value);
	void __fastcall SetMpegStreamType(TMpegStreamType Value);
	void __fastcall SetPlaylistIndex(int Value);
	void __fastcall SetPlayerDuration(__int64 Value);
	void __fastcall SetPlayerFramePosition(__int64 Value);
	void __fastcall SetPlayerTimePosition(__int64 Value);
	void __fastcall SetPropertyPageSaverGroup(System::UnicodeString Value);
	void __fastcall SetRecordingBacktimedFramesCount(int Value);
	void __fastcall SetRecordingCanPause(bool Value);
	void __fastcall SetReencodingStartFrame(__int64 Value);
	void __fastcall SetReencodingStartTime(__int64 Value);
	void __fastcall SetReencodingStopFrame(__int64 Value);
	void __fastcall SetReencodingStopTime(__int64 Value);
	void __fastcall SetSendToDV_DeviceIndex(int Value);
	void __fastcall SetSpeakerBalance(int Value);
	void __fastcall SetSpeakerVolume(int Value);
	void __fastcall SetTextOverlaySelector(int Value);
	void __fastcall SetTVChannel(int Value);
	void __fastcall SetTVCountryCode(int Value);
	void __fastcall SetTVTunerInputType(TTunerInputType Value);
	void __fastcall SetTunerMode(TTunerMode Value);
	void __fastcall SetVCRHorizontalLocking(bool Value);
	
public:
	virtual void __fastcall BeforeDestruction(void);
	virtual void __fastcall SetBounds(int ALeft, int ATop, int AWidth, int AHeight);
	void __fastcall About(void);
	int __fastcall AnalogVideoStandardIndex(System::UnicodeString Value);
	System::UnicodeString __fastcall ASFStreaming_GetAuthorizationList(void);
	System::UnicodeString __fastcall ASFStreaming_GetConnectedClients(void);
	int __fastcall ASFStreaming_GetConnectedClientsCount(void);
	bool __fastcall ASFStreaming_ResetAuthorizations(void);
	bool __fastcall ASFStreaming_SetAuthorization(bool Allowed, System::UnicodeString IP, System::UnicodeString Mask);
	bool __fastcall AssociateMultiplexedSlave(int InputNumber, int SlaveUniqueID);
	int __fastcall AudioCompressorIndex(System::UnicodeString Value);
	int __fastcall AudioDeviceIndex(System::UnicodeString Value);
	int __fastcall AudioRendererIndex(System::UnicodeString Value);
	int __fastcall AudioInputIndex(System::UnicodeString Value);
	System::UnicodeString __fastcall AVIInfo2(System::UnicodeString AVIFile, TAVIInfoType AVIInfoType);
	System::UnicodeString __fastcall AVIHeaderInfo(System::UnicodeString AVIFile, THeaderAttribute HeaderAttribute);
	int __fastcall CameraControlValue(TCameraControl Setting);
	bool __fastcall CameraControlAuto(TCameraControl Setting);
	int __fastcall CameraControlDefault(TCameraControl Setting);
	int __fastcall CameraControlMin(TCameraControl Setting);
	int __fastcall CameraControlMax(TCameraControl Setting);
	int __fastcall CameraControlStep(TCameraControl Setting);
	bool __fastcall Cancel(void);
	bool __fastcall CanProcessMessages(void);
	bool __fastcall CaptureFrameTo(TFrameCaptureDest Dest, System::UnicodeString FileName);
	bool __fastcall CaptureFrameSyncTo(TFrameCaptureDest Dest, System::UnicodeString FileName);
	void __fastcall ClearHeaderAttributes(void);
	void __fastcall ClosePlayer(void);
	void __fastcall ContinueProcessing(void);
	HBITMAP __fastcall GetLastFrameAsHBITMAP(int BufferIndex, bool WithOverlays, int SrcLeftLocation, int SrcTopLocation, int SrcWidth, int SrcHeight, int DestWidth, int DestHeight, int BitmapColorBitCount);
	Graphics::TBitmap* __fastcall GetLastFrameAsTBitmap(int BufferIndex, bool WithOverlays, int SrcLeftLocation, int SrcTopLocation, int SrcWidth, int SrcHeight, int DestWidth, int DestHeight, int BitmapColorBitCount);
	bool __fastcall CreatePreallocCapFile(void);
	double __fastcall DVDInfo(System::UnicodeString DVDRootDirectory, TDVDInfoType DVDInfoType, int TitleNumber);
	void __fastcall Display_SetLocation(int WindowLeft, int WindowTop, int WindowWidth, int WindowHeight);
	void __fastcall DualDisplay_SetLocation(int WindowLeft, int WindowTop, int WindowWidth, int WindowHeight);
	bool __fastcall DrawBitmapOverFrame(Graphics::TBitmap* Bitmap, bool Stretched, int LeftLocation, int TopLocation, int bmWidth, int bmHeight, bool Transparent, bool UseTransparentColor, Graphics::TColor TransparentColorValue, bool AlphaBlend, int AlphaBlendValue, bool ChromaKey, int ChromaKeyRGBColor, int ChromaKeyLeewayPercent);
	bool __fastcall EnumerateWindows(void);
	bool __fastcall EnableMultiplexedInput(int InputNumber, bool Enable);
	void __fastcall FastForwardPlayer(void);
	int __fastcall FindIndexInListByName(System::UnicodeString List, System::UnicodeString SearchedString, bool IsSubString, bool IgnoreCase);
	System::UnicodeString __fastcall GetAudioCodec(void);
	unsigned __fastcall GetFrameInfo(int FrameId, TFrameInfoId FrameInfoId);
	System::UnicodeString __fastcall GetFrameInfoString(TFrameInfoStringId FrameInfoStringId);
	System::UnicodeString __fastcall GetItemNameFromList(System::UnicodeString List, int ItemIndex);
	int __fastcall GetMiscDeviceControl(TMiscDeviceControl MiscDeviceControl, int Index);
	System::UnicodeString __fastcall GetLogString(TLogType Value);
	int __fastcall GetNearestVideoWidth(int PreferredVideoWidth, int PreferredVideoHeight);
	int __fastcall GetNearestVideoHeight(int PreferredVideoWidth, int PreferredVideoHeight);
	int __fastcall GetPlayerOpenProgressPercent(void);
	System::UnicodeString __fastcall GetPlaylist(void);
	bool __fastcall GetTranslatedCoordinates(int DisplayIndex, int NativeX, int NativeY, int &TranslatedX, int &TranslatedY);
	int __fastcall GetTVChannelInfo(TTVChannelInfo Value);
	System::UnicodeString __fastcall GetVideoCodec(void);
	int __fastcall GetVideoWidthFromIndex(int Index);
	int __fastcall GetVideoHeightFromIndex(int Index);
	void __fastcall GetNearestVideoSize(int PreferredVideoWidth, int PreferredVideoHeight, /* out */ int &NearestVideoWidth, /* out */ int &NearestVideoHeight);
	bool __fastcall GetVMR9ImageAdjustmentBounds(bool MainDisplay, TVMR9ImageAdjustment VMR9ControlSetting, /* out */ int &MinValue, /* out */ int &MaxValue, /* out */ int &StepSize, /* out */ int &DefaultValue, /* out */ int &CurrentValue);
	bool __fastcall GetVideoSizeFromIndex(int Index, /* out */ int &_VideoWidth, /* out */ int &_VideoHeight);
	bool __fastcall AVIDuration(System::UnicodeString AVIFile, /* out */ __int64 &Duration, /* out */ __int64 &FrameCount);
	bool __fastcall AVIInfo(System::UnicodeString AVIFile, /* out */ __int64 &Duration, /* out */ __int64 &FrameCount, /* out */ int &_VideoWidth, /* out */ int &_VideoHeight, /* out */ double &VideoFrameRateFps, /* out */ int &AvgBitRate, /* out */ int &AudioChannels, /* out */ int &AudioSamplesPerSec, /* out */ int &AudioBitsPerSample, /* out */ System::UnicodeString &VideoCodec, /* out */ System::UnicodeString &AudioCodec);
	bool __fastcall GetVideoCompressionSettings(/* out */ int &DataRate, /* out */ int &KeyFrameRate, /* out */ int &PFramesPerKeyFrame, /* out */ int &WindowSize, /* out */ double &Quality, /* out */ bool &CanQuality, /* out */ bool &CanCrunch, /* out */ bool &CanKeyFrame, /* out */ bool &CanBFrame, /* out */ bool &CanWindow);
	int __fastcall GetRGBPixelAt(int x, int y);
	double __fastcall GetPixelsDistance(int x1, int y1, int x2, int y2);
	unsigned __fastcall GetVUMeterSetting(unsigned ChannelIndex, TVUMeterSetting VUMeterSetting);
	TGraphState __fastcall GraphState(void);
	bool __fastcall IsAudioDeviceConnected(int DeviceIndex);
	bool __fastcall IsAudioDeviceASoundCard(int DeviceIndex);
	bool __fastcall IsAudioRendererConnected(int RendererIndex);
	bool __fastcall IsCameraControlSettingAvailable(TCameraControl Setting);
	bool __fastcall IsDialogAvailable(TDialog Dialog);
	bool __fastcall IsDirectX8OrHigherInstalled(void);
	bool __fastcall IsPlaylistActive(void);
	bool __fastcall IsVideoQualitySettingAvailable(TVideoQuality Setting);
	bool __fastcall IsVideoControlModeAvailable(TVideoControl Mode);
	bool __fastcall IsVMR9ImageAdjustmentAvailable(bool MainDisplay);
	bool __fastcall IsVideoDeviceConnected(int DeviceIndex);
	bool __fastcall IsVideoSignalDetected(bool DetectConnexantBlueScreen, bool DetectCustomRGB, int CustomR, int CustomG, int CustomB, bool UseAsMaxValues);
	bool __fastcall IsDVDevice(int Index);
	bool __fastcall LoadCompressorSettingsFromDataString(bool IsVideoCompressor, int CompressorIndex, System::UnicodeString DataString);
	bool __fastcall LoadCompressorSettingsFromTextFile(bool IsVideoCompressor, int CompressorIndex, System::UnicodeString FileName);
	bool __fastcall MixAudioSamples(void * pSampleBuffer, int SampleBufferSize, int SampleDataLength, TFormatType SampleFormatType, void * pFormat, __int64 SampleStartTime, __int64 SampleStopTime);
	int __fastcall MonitorsCount(void);
	double __fastcall MotionDetector_CellMotionRatio(int x, int y);
	int __fastcall MotionDetector_CellColorIntensity(TRGBSelector RGBSelector, int x, int y);
	System::UnicodeString __fastcall MotionDetector_Get2DTextGrid(void);
	System::UnicodeString __fastcall MotionDetector_Get2DTextMotion(void);
	int __fastcall MotionDetector_GlobalColorIntensity(TRGBSelector RGBSelector);
	bool __fastcall MotionDetector_GetCellLocation(int x, int y, int &XLocation, int &YLocation);
	bool __fastcall MotionDetector_GetCellSensitivity(int x, int y, int &Value);
	bool __fastcall MotionDetector_GetCellSize(int &XSize, int &YSize);
	bool __fastcall MotionDetector_SetCellSensitivity(int x, int y, int Value);
	void __fastcall MotionDetector_SetGridSize(int x, int y);
	void __fastcall MotionDetector_GloballyIncOrDecSensitivity(int Value);
	void __fastcall MotionDetector_ResetGlobalSensitivity(int Value);
	void __fastcall MotionDetector_Reset(void);
	void __fastcall MotionDetector_ShowGridDialog(void);
	void __fastcall MotionDetector_TriggerNow(void);
	Classes::TComponent* __fastcall MotionDetector_EnumGridDialogControls(bool FirstControl);
	bool __fastcall MotionDetector_UseThisReferenceSample(Graphics::TBitmap* Bitmap, System::UnicodeString BMPFile, System::UnicodeString JPEGFile);
	int __fastcall MultiplexerIndex(System::UnicodeString Value);
	void __fastcall NotifyPlayerTrackbarAction(TTrackbarAction TrackbarAction);
	BOOL __fastcall OpenPlayer(void);
	BOOL __fastcall OpenDVD(void);
	BOOL __fastcall OpenPlayerAtFramePositions(__int64 StartFrame, __int64 StopFrame, bool KeepBounds, bool CloseAndReopenIfAlreadyOpened);
	BOOL __fastcall OpenPlayerAtTimePositions(__int64 StartTime, __int64 StopTime, bool KeepBounds, bool CloseAndReopenIfAlreadyOpened);
	bool __fastcall PauseRecording(void);
	void __fastcall PausePlayer(void);
	bool __fastcall PlayerFrameStep(int FrameCount);
	bool __fastcall Playlist(TPlaylist PlaylistAction, System::UnicodeString VideoClip);
	unsigned __fastcall RecordingKBytesWrittenToDisk(void);
	bool __fastcall ReencodeVideoClip(System::UnicodeString SourceVideoClip, System::UnicodeString NewVideoClip, bool IncludeVideoStream, bool IncludeAudioStream, bool UseFrameGrabber, bool UseCurrentVideoCompressor, bool UseCurrentAudioCompressor);
	void __fastcall RefreshDevicesAndCompressorsLists(void);
	void __fastcall RefreshPlayerOverlays(void);
	bool __fastcall ResetVideoDeviceSettings(void);
	bool __fastcall ResumeRecording(void);
	void __fastcall RewindPlayer(void);
	void __fastcall RunPlayer(void);
	void __fastcall RunPlayerBackwards(void);
	bool __fastcall StartSynchronized(void);
	int __fastcall VideoCompressorIndex(System::UnicodeString Value);
	int __fastcall VideoDeviceIndex(System::UnicodeString Value);
	int __fastcall VideoFormatIndex(System::UnicodeString Value);
	int __fastcall VideoInputIndex(System::UnicodeString Value);
	int __fastcall VideoSizeIndex(System::UnicodeString Value);
	int __fastcall VideoSubtypeIndex(System::UnicodeString Value);
	bool __fastcall SendDVCommand(TDVCommand DVCommand);
	bool __fastcall SendIPCameraCommand(System::UnicodeString IPCameraCommand);
	void __fastcall SetAuthentication(TAuthenticationType AuthenticationType, System::UnicodeString UserName, System::UnicodeString Password);
	int __fastcall Mixer_AddToMixer(int SourceUniqueID, int SourceVideoInput, int MosaicLine, int MosaicColumn, int AlternatedGroup, int AlternatedTimeIntervalInMs, bool ReplacePreviouslyAdded, bool CanEraseBackground);
	int __fastcall Mixer_AddAudioToMixer(int SourceUniqueID);
	bool __fastcall Mixer_AudioActivation(int Id, bool Activate);
	bool __fastcall Mixer_RemoveAudioFromMixer(int Id);
	int __fastcall Mixer_SetupPIPFromSource(int SourceUniqueID, int Source_Left, int Source_Top, int Source_Width, int Source_Height, bool ActivatePIP, int PIP_Left, int PIP_Top, int PIP_Width, int PIP_Height, bool MoveToTop);
	bool __fastcall Mixer_Activation(int Id, bool Activate);
	bool __fastcall Mixer_RemoveFromMixer(int Id);
	int __fastcall MPEGProgramSetting(TMPEGProgramSetting MPEGProgramSetting, int Value);
	System::UnicodeString __fastcall SaveCompressorSettingsToDataString(bool IsVideoCompressor, int CompressorIndex);
	bool __fastcall SaveCompressorSettingsToTextFile(bool IsVideoCompressor, int CompressorIndex, System::UnicodeString FileName);
	void __fastcall SetAVIMuxConfig(TAVIMuxConfig AVIMuxSetting, int Value);
	bool __fastcall SetCameraControl(TCameraControl Setting, bool SetAuto, bool SetDefault, int SetValue);
	void __fastcall SetFrameCaptureBounds(int LeftPosition, int TopPosition, int RightPosition, int BottomPosition);
	void __fastcall SetMultiplexerFilterByName(System::UnicodeString Value);
	void __fastcall SetTextOverlayCustomVar(int TextOverlaySelector, int VarIndex, System::UnicodeString VarText);
	bool __fastcall SetVideoCompressionDefaults(void);
	bool __fastcall SetVideoCompressionSettings(int DataRate, int KeyFrameRate, int PFramesPerKeyFrame, int WindowSize, double Quality);
	bool __fastcall SetVideoControlMode(bool FlipHorizontal, bool FlipVertical, bool ExternalTriggerEnable, bool Trigger);
	bool __fastcall SetVideoQuality(TVideoQuality Setting, bool SetAuto, bool SetDefault, int SetValue);
	bool __fastcall SetVMR9ImageAdjustmentValue(bool MainDisplay, TVMR9ImageAdjustment VMR9ControlSetting, int Value, bool FixRange);
	bool __fastcall ShapeOverlayList(Extctrls::TShape* Shape, bool Add);
	bool __fastcall ShowDialog(TDialog Dialog);
	bool __fastcall PausePreview(void);
	int __fastcall PointGreyConfig(TPointGreyConfig ConfigType, unsigned &PGRActionValue, unsigned ulRegister, unsigned ulMode, unsigned ulLeft, unsigned ulTop, unsigned ulWidth, unsigned ulHeight, unsigned ulPercentage, unsigned ulFormat);
	bool __fastcall ResetPreview(void);
	bool __fastcall ResumePreview(void);
	bool __fastcall RecordToNewFileNow(System::UnicodeString NewRecordingFileName, bool ResetStreamTime);
	void __fastcall RetrieveInitialXYAfterRotation(int X, int Y, int &OriginalX, int &OriginalY);
	bool __fastcall SendImageToVideoFromBitmaps(System::UnicodeString ImageFilePath, unsigned BitmapHandle, bool CanFreeBitmapHandle, bool EndOfData);
	void __fastcall SetHeaderAttribute(THeaderAttribute HeaderAttribute, System::UnicodeString Value);
	void __fastcall SetImageOverlayAttributes(int LeftLocation, int TopLocation, int StretchedWidth, int StretchedHeight, bool Transparent, bool UseTransparentColor, int TransparentColorValue, bool AlphaBlend, int AlphaBlendValue);
	bool __fastcall SetImageOverlayFromImageFile(System::UnicodeString FileName);
	bool __fastcall SetImageOverlayFromBMPFile(System::UnicodeString FileName);
	bool __fastcall SetImageOverlayFromJPEGFile(System::UnicodeString FileName);
	bool __fastcall SetImageOverlayFromHBitmap(HBITMAP Bitmap);
	bool __fastcall SetImageOverlayFromTBitmap(Graphics::TBitmap* Bitmap);
	bool __fastcall SetImageOverlayFromTImage(Extctrls::TImage* Image);
	bool __fastcall SetIPCameraSetting(TIPCameraSetting Setting, int Value);
	bool __fastcall SetLogoFromBMPFile(System::UnicodeString FileName);
	bool __fastcall SetLogoFromJPEGFile(System::UnicodeString FileName);
	bool __fastcall SetLogoFromHBitmap(HBITMAP Bitmap);
	bool __fastcall SetLogoFromTBitmap(Graphics::TBitmap* Bitmap);
	bool __fastcall SetLogoFromTImage(Extctrls::TImage* Image);
	bool __fastcall ScreenRecordingUsingCoordinates(bool FunctionEnabled, int scLeft, int scTop, int scWidth, int scHeight);
	bool __fastcall SendCameraCommand(int Pan, int Tilt, bool Relative);
	bool __fastcall PutMiscDeviceControl(TMiscDeviceControl MiscDeviceControl, int Index, int Value);
	bool __fastcall SetWindowRecordingByHandle(int Window_Handle);
	bool __fastcall SetWindowRecordingByName(System::UnicodeString WindowName, bool ExactMatch);
	void __fastcall SetVUMeterSetting(unsigned ChannelIndex, TVUMeterSetting VUMeterSetting, unsigned Value);
	bool __fastcall SetWindowTransparency(unsigned WndHandle, bool UseColorKey, bool UseAlpha, int AlphaValue);
	BOOL __fastcall StartAudioRecording(void);
	BOOL __fastcall StartAudioRendering(void);
	BOOL __fastcall StartPreview(void);
	BOOL __fastcall StartRecording(void);
	BOOL __fastcall StartReencoding(void);
	bool __fastcall Stop(void);
	void __fastcall StopPreview(void);
	void __fastcall StopPlayer(void);
	void __fastcall StopRecording(void);
	bool __fastcall StopReencoding(void);
	bool __fastcall TextOverlay_CreateCustomFont(int fHeight, int fWidth, int fEscapement, int fOrientation, int fWeight, bool fItalic, bool fUnderline, bool fStrikeOut, unsigned fCharSet, unsigned fOutputPrecision, unsigned fClipPrecision, unsigned fQuality, unsigned fPitchAndFamily, System::UnicodeString FontFacename);
	bool __fastcall ThirdPartyFilter_AddToList(TThirdPartyFilterList Location, System::UnicodeString GUIDString, System::UnicodeString OptionalDLLFilePath, System::UnicodeString FilterName, bool Enable, bool CanSaveFilterState);
	bool __fastcall ThirdPartyFilter_ClearList(void);
	bool __fastcall ThirdPartyFilter_Enable(TThirdPartyFilterList Location, System::UnicodeString GUIDString, bool Enable);
	bool __fastcall ThirdPartyFilter_RemoveFromList(TThirdPartyFilterList Location, System::UnicodeString GUIDString);
	bool __fastcall ThirdPartyFilter_ShowDialog(TThirdPartyFilterList Location, System::UnicodeString GUIDString);
	bool __fastcall TVGetMinMaxChannels(int &MinChannel, int &MaxChannel);
	bool __fastcall TVClearFrequencyOverrides(void);
	int __fastcall TVSetChannelFrequencyOverride(int TVChannel, int FrequencyInHz);
	bool __fastcall TVStartAutoScan(void);
	bool __fastcall TVStartAutoScanChannels(int MinChannel, int MaxChannel, int Interval_ms);
	bool __fastcall TVStopAutoScan(void);
	void __fastcall UseNearestVideoSize(int PreferredWidth, int PreferredHeight, bool Stretch);
	bool __fastcall VDECGetHorizontalLocked(/* out */ int &plLocked);
	bool __fastcall VDECGetNumberOfLines(/* out */ int &plNumberOfLines);
	bool __fastcall VDECGetOutputEnable(/* out */ int &plOutputEnable);
	bool __fastcall VDECGetVCRHorizontalLocking(/* out */ int &plVCRHorizontalLocking);
	bool __fastcall VDECPutTVFormat(int lAnalogVideoStandard);
	bool __fastcall VDECPutOutputEnable(int lOutputEnable);
	bool __fastcall VDECPutVCRHorizontalLocking(int lVCRHorizontalLocking);
	bool __fastcall VideoFromImages_CreateSetOfBitmaps(void);
	int __fastcall VideoQualityValue(TVideoQuality Setting);
	bool __fastcall VideoQualityAuto(TVideoQuality Setting);
	int __fastcall VideoQualityDefault(TVideoQuality Setting);
	int __fastcall VideoQualityMin(TVideoQuality Setting);
	int __fastcall VideoQualityMax(TVideoQuality Setting);
	int __fastcall VideoQualityStep(TVideoQuality Setting);
	bool __fastcall WriteScriptCommand(System::UnicodeString ScriptType, System::UnicodeString ScriptArgument);
	bool __fastcall MonitorBounds(int MonitorNumber, Types::TRect &Bounds);
	
protected:
	virtual void __fastcall SetParent(Controls::TWinControl* AParent);
	virtual void __fastcall CreateHandle(void);
	virtual void __fastcall DestroyWnd(void);
	virtual void __fastcall Loaded(void);
	HIDESBASE MESSAGE void __fastcall WMERASEBKGND(Messages::TWMEraseBkgnd &Message);
	virtual void __fastcall Notification(Classes::TComponent* AComponent, Classes::TOperation Operation);
	virtual void __fastcall PaintWindow(HDC DC);
	
public:
	__fastcall virtual TVideoGrabber(Classes::TComponent* Aowner);
	__fastcall virtual ~TVideoGrabber(void);
	__property System::UnicodeString AnalogVideoStandards = {read=GetAnalogVideoStandards};
	__property int AnalogVideoStandardsCount = {read=GetAnalogVideoStandardsCount, nodefault};
	__property System::UnicodeString ASFProfiles = {read=GetASFProfiles};
	__property int ASFProfilesCount = {read=GetASFProfilesCount, nodefault};
	__property int AudioBalance = {read=GetAudioBalance, write=SetAudioBalance, nodefault};
	__property System::UnicodeString AudioCompressorName = {read=GetAudioCompressorName};
	__property System::UnicodeString AudioCompressors = {read=GetAudioCompressors};
	__property int AudioCompressorsCount = {read=GetAudioCompressorsCount, nodefault};
	__property int AudioInputBalance = {read=GetAudioInputBalance, write=SetAudioInputBalance, nodefault};
	__property int AudioInputLevel = {read=GetAudioInputLevel, write=SetAudioInputLevel, nodefault};
	__property bool AudioInputMono = {read=GetAudioInputMono, write=SetAudioInputMono, nodefault};
	__property System::UnicodeString AudioInputs = {read=GetAudioInputs};
	__property int AudioInputsCount = {read=GetAudioInputsCount, nodefault};
	__property System::UnicodeString AudioDeviceName = {read=GetAudioDeviceName};
	__property System::UnicodeString AudioDevices = {read=GetAudioDevices};
	__property int AudioDevicesCount = {read=GetAudioDevicesCount, nodefault};
	__property System::UnicodeString AudioFormats = {read=GetAudioFormats};
	__property System::UnicodeString AudioRendererName = {read=GetAudioRendererName};
	__property System::UnicodeString AudioRenderers = {read=GetAudioRenderers};
	__property int AudioRenderersCount = {read=GetAudioRenderersCount, nodefault};
	__property int AudioVolume = {read=GetAudioVolume, write=SetAudioVolume, nodefault};
	__property int BufferCount = {read=GetBufferCount, write=SetBufferCount, nodefault};
	__property bool Busy = {read=GetBusy, nodefault};
	__property double RecordingDuration = {read=GetRecordingDuration};
	__property System::UnicodeString RecordingFourCC = {read=GetRecordingFourCC};
	__property int Cropping_YMax = {read=GetCropping_YMax, nodefault};
	__property int Cropping_XMax = {read=GetCropping_XMax, nodefault};
	__property double CurrentFrameRate = {read=GetCurrentFrameRate};
	__property TCurrentState CurrentState = {read=GetCurrentState, nodefault};
	__property __int64 DeliveredFrames = {read=GetDeliveredFrames};
	__property System::UnicodeString DirectShowFilters = {read=GetDirectShowFilters};
	__property int DirectShowFiltersCount = {read=GetDirectShowFiltersCount, nodefault};
	__property int Display_VideoHeight = {read=GetDisplay_VideoHeight, nodefault};
	__property int Display_VideoWidth = {read=GetDisplay_VideoWidth, nodefault};
	__property HWND Display_VideoWindowHandle = {read=GetDisplay_VideoWindowHandle, nodefault};
	__property HWND DualDisplay_VideoWindowHandle = {read=GetDualDisplay_VideoWindowHandle, nodefault};
	__property int DroppedFrames = {read=GetDroppedFrameCount, nodefault};
	__property int DualDisplay_VideoHeight = {read=GetDualDisplay_VideoHeight, nodefault};
	__property int DualDisplay_VideoWidth = {read=GetDualDisplay_VideoWidth, nodefault};
	__property TFrameGrabberRGBFormat FrameGrabberCurrentRGBFormat = {read=GetFrameGrabberCurrentRGBFormat, nodefault};
	__property bool HoldRecording = {read=GetHoldRecording, write=SetHoldRecording, nodefault};
	__property int ImageOverlaySelector = {read=GetImageOverlaySelector, write=SetImageOverlaySelector, nodefault};
	__property double ImageRatio = {read=GetImageRatio};
	__property bool InFrameProgressEvent = {read=GetInFrameProgressEvent, nodefault};
	__property bool IsAnalogVideoDecoderAvailable = {read=GetIsAnalogVideoDecoderAvailable, nodefault};
	__property bool IsAudioCrossbarAvailable = {read=GetIsAudioCrossbarAvailable, nodefault};
	__property TTriState IsAudioInputBalanceAvailable = {read=GetIsAudioInputBalanceAvailable, nodefault};
	__property bool IsCameraControlAvailable = {read=GetIsCameraControlAvailable, nodefault};
	__property bool IsDigitalVideoIn = {read=GetIsDigitalVideoIn, nodefault};
	__property bool IsDVCommandAvailable = {read=GetIsDVCommandAvailable, nodefault};
	__property bool IsHorizontalSyncLocked = {read=GetIsHorizontalSyncLocked, nodefault};
	__property bool IsMpegStream = {read=GetIsMpegStream, nodefault};
	__property TTriState IsPlayerAudioStreamAvailable = {read=GetIsPlayerAudioStreamAvailable, nodefault};
	__property TTriState IsPlayerVideoStreamAvailable = {read=GetIsPlayerVideoStreamAvailable, nodefault};
	__property bool IsRecordingPaused = {read=GetIsRecordingPaused, nodefault};
	__property bool IsTimeCodeReaderAvailable = {read=GetTimeCodeReaderAvailable, nodefault};
	__property bool IsTVTunerAvailable = {read=GetIsTVTunerAvailable, nodefault};
	__property bool IsTVAutoTuneRunning = {read=GetIsTVAutoTuneRunning, nodefault};
	__property bool IsTVAudioAvailable = {read=GetIsTVAudioAvailable, nodefault};
	__property bool IsVideoControlAvailable = {read=GetIsVideoControlAvailable, nodefault};
	__property bool IsVideoCrossbarAvailable = {read=GetIsVideoCrossbarAvailable, nodefault};
	__property bool IsVideoInterlaced = {read=GetIsVideoInterlaced, nodefault};
	__property bool IsVideoQualityAvailable = {read=GetIsVideoQualityAvailable, nodefault};
	__property bool IsVideoPortAvailable = {read=GetIsVideoPortAvailable, nodefault};
	__property bool IsWDMVideoDriver = {read=GetIsWDMVideoDriver, nodefault};
	__property System::UnicodeString Last_BurstFrameCapture_FileName = {read=GetLast_BurstFrameCapture_FileName};
	__property System::UnicodeString Last_CaptureFrameTo_FileName = {read=GetLast_CaptureFrameTo_FileName};
	__property System::UnicodeString Last_Recording_FileName = {read=GetLast_Recording_FileName};
	__property System::UnicodeString Last_Clip_Played = {read=GetLast_Clip_Played};
	__property double MotionDetector_GlobalMotionRatio = {read=GetMotionDetector_GlobalMotionRatio};
	__property int MotionDetector_GridXCount = {read=GetMotionDetector_GridXCount, nodefault};
	__property int MotionDetector_GridYCount = {read=GetMotionDetector_GridYCount, nodefault};
	__property bool MotionDetector_IsGridValid = {read=GetMotionDetector_IsGridValid, nodefault};
	__property TMpegStreamType MpegStreamType = {read=GetMpegStreamType, write=SetMpegStreamType, nodefault};
	__property System::UnicodeString MultiplexerName = {read=GetMultiplexerName};
	__property System::UnicodeString Multiplexers = {read=GetMultiplexers};
	__property int MultiplexersCount = {read=GetMultiplexersCount, nodefault};
	__property int PlaylistIndex = {read=GetPlaylistIndex, write=SetPlaylistIndex, nodefault};
	__property System::UnicodeString PlayerAudioCodec = {read=GetAudioCodec};
	__property __int64 PlayerDuration = {read=GetPlayerDuration, write=SetPlayerDuration};
	__property __int64 PlayerFrameCount = {read=GetPlayerFrameCount};
	__property __int64 PlayerFramePosition = {read=GetPlayerFramePosition, write=SetPlayerFramePosition};
	__property int PlayerOpenProgressPercent = {read=GetPlayerOpenProgressPercent, nodefault};
	__property TPlayerState PlayerState = {read=GetPlayerState, nodefault};
	__property __int64 PlayerTimePosition = {read=GetPlayerTimePosition, write=SetPlayerTimePosition};
	__property double PlayerTrackBarScale = {read=GetPlayerTrackBarScale};
	__property double PlayerFrameRate = {read=GetPlayerFrameRate};
	__property System::UnicodeString PlayerVideoCodec = {read=GetVideoCodec};
	__property System::UnicodeString PropertyPageSaverGroup = {read=GetPropertyPageSaverGroup, write=SetPropertyPageSaverGroup};
	__property int RecordingBacktimedFramesCount = {read=GetRecordingBacktimedFramesCount, write=SetRecordingBacktimedFramesCount, nodefault};
	__property bool RecordingCanPause = {read=GetRecordingCanPause, write=SetRecordingCanPause, nodefault};
	__property int RecordingHeight = {read=GetRecordingHeight, nodefault};
	__property int RecordingWidth = {read=GetRecordingWidth, nodefault};
	__property int SendToDV_DeviceIndex = {read=GetSendToDV_DeviceIndex, write=SetSendToDV_DeviceIndex, nodefault};
	__property Classes::TStream* SourceStream = {read=GetSourceStream, write=SetSourceStream};
	__property int SpeakerBalance = {read=GetSpeakerBalance, write=SetSpeakerBalance, nodefault};
	__property int SpeakerVolume = {read=GetSpeakerVolume, write=SetSpeakerVolume, nodefault};
	__property System::UnicodeString StreamingURL = {read=GetStreamingURL};
	__property System::UnicodeString SystemTempPath = {read=GetSystemTempPath};
	__property int TextOverlay_Selector = {read=GetTextOverlaySelector, write=SetTextOverlaySelector, nodefault};
	__property int TVChannel = {read=GetTVChannel, write=SetTVChannel, nodefault};
	__property int TVCountryCode = {read=GetTVCountryCode, write=SetTVCountryCode, nodefault};
	__property TTunerInputType TVTunerInputType = {read=GetTVTunerInputType, write=SetTVTunerInputType, nodefault};
	__property TTunerMode TunerMode = {read=GetTunerMode, write=SetTunerMode, default=0};
	__property int UniqueID = {read=GetUniqueID, nodefault};
	__property bool VCRHorizontalLocking = {read=GetVCRHorizontalLocking, write=SetVCRHorizontalLocking, nodefault};
	__property System::UnicodeString VideoCompressorName = {read=GetVideoCompressorName};
	__property System::UnicodeString VideoCompressors = {read=GetVideoCompressors};
	__property int VideoCompressorsCount = {read=GetVideoCompressorsCount, nodefault};
	__property System::UnicodeString VideoFormats = {read=GetVideoFormats};
	__property int VideoFormatsCount = {read=GetVideoFormatsCount, nodefault};
	__property System::UnicodeString VideoInputs = {read=GetVideoInputs};
	__property int VideoInputsCount = {read=GetVideoInputsCount, nodefault};
	__property System::UnicodeString VideoDeviceName = {read=GetVideoDeviceName};
	__property System::UnicodeString VideoDevices = {read=GetVideoDevices};
	__property System::UnicodeString VideoDevicesId = {read=GetVideoDevicesId};
	__property int VideoDevicesCount = {read=GetVideoDevicesCount, nodefault};
	__property int VideoHeight = {read=GetVideoHeight, nodefault};
	__property int VideoHeight_PreferredAspectRatio = {read=GetVideoHeight_PreferredAspectRatio, nodefault};
	__property int VideoWidth = {read=GetVideoWidth, nodefault};
	__property int VideoWidth_PreferredAspectRatio = {read=GetVideoWidth_PreferredAspectRatio, nodefault};
	__property System::UnicodeString VideoSizes = {read=GetVideoSizes};
	__property int VideoSizesCount = {read=GetVideoSizesCount, nodefault};
	__property System::UnicodeString VideoSources = {read=GetVideoSources};
	__property int VideoSourcesCount = {read=GetVideoSourcesCount, nodefault};
	__property System::UnicodeString VideoSubtypes = {read=GetVideoSubtypes};
	__property int VideoSubtypesCount = {read=GetVideoSubtypesCount, nodefault};
	
__published:
	__property Align = {default=0};
	__property Alignment = {default=2};
	__property Caption;
	__property Color = {default=-16777201};
	__property Enabled = {default=1};
	__property FullRepaint = {default=1};
	__property Height;
	__property Left;
	__property ParentColor = {default=0};
	__property ParentShowHint = {default=1};
	__property Tag = {default=0};
	__property Top;
	__property Visible = {default=1};
	__property Width;
	__property OnResize;
	__property OnDragDrop;
	__property OnDragOver;
	__property OnEndDrag;
	__property OnStartDrag;
	__property Anchors = {default=3};
	__property ParentBiDiMode = {default=1};
	__property OnCanResize;
	__property OnConstrainedResize;
	__property OnContextPopup;
	__property bool AdjustOverlayAspectRatio = {read=GetAdjustOverlayAspectRatio, write=SetAdjustOverlayAspectRatio, default=1};
	__property bool AdjustPixelAspectRatio = {read=GetAdjustPixelAspectRatio, write=SetAdjustPixelAspectRatio, default=1};
	__property TAero Aero = {read=GetAero, write=SetAero, default=0};
	__property int AnalogVideoStandard = {read=GetAnalogVideoStandard, write=SetAnalogVideoStandard, default=-1};
	__property TApplicationPriority ApplicationPriority = {read=GetApplicationPriority, write=SetApplicationPriority, default=0};
	__property int ASFAudioBitRate = {read=GetASFAudioBitRate, write=SetASFAudioBitRate, default=-1};
	__property int ASFAudioChannels = {read=GetASFAudioChannels, write=SetASFAudioChannels, default=-1};
	__property int ASFBufferWindow = {read=GetASFBufferWindow, write=SetASFBufferWindow, default=-1};
	__property TASFDeinterlaceMode ASFDeinterlaceMode = {read=GetASFDeinterlaceMode, write=SetASFDeinterlaceMode, default=0};
	__property bool ASFFixedFrameRate = {read=GetASFFixedFrameRate, write=SetASFFixedFrameRate, default=1};
	__property System::UnicodeString ASFMediaServerPublishingPoint = {read=GetASFMediaServerPublishingPoint, write=SetASFMediaServerPublishingPoint};
	__property System::UnicodeString ASFMediaServerTemplatePublishingPoint = {read=GetASFMediaServerTemplatePublishingPoint, write=SetASFMediaServerTemplatePublishingPoint};
	__property bool ASFMediaServerRemovePublishingPointAfterDisconnect = {read=GetASFMediaServerRemovePublishingPointAfterDisconnect, write=SetASFMediaServerRemovePublishingPointAfterDisconnect, default=0};
	__property int ASFNetworkPort = {read=GetASFNetworkPort, write=SetASFNetworkPort, default=0};
	__property int ASFNetworkMaxUsers = {read=GetASFNetworkMaxUsers, write=SetASFNetworkMaxUsers, default=5};
	__property int ASFProfile = {read=GetASFProfile, write=SetASFProfile, default=-1};
	__property System::UnicodeString ASFProfileFromCustomFile = {read=GetASFProfileFromCustomFile, write=SetASFProfileFromCustomFile};
	__property TASFProfileVersion ASFProfileVersion = {read=GetASFProfileVersion, write=SetASFProfileVersion, default=0};
	__property int ASFVideoBitRate = {read=GetASFVideoBitRate, write=SetASFVideoBitRate, default=-1};
	__property double ASFVideoFrameRate = {read=GetASFVideoFrameRate, write=SetASFVideoFrameRate};
	__property int ASFVideoHeight = {read=GetASFVideoHeight, write=SetASFVideoHeight, default=-1};
	__property int ASFVideoMaxKeyFrameSpacing = {read=GetASFVideoMaxKeyFrameSpacing, write=SetASFVideoMaxKeyFrameSpacing, default=-1};
	__property int ASFVideoQuality = {read=GetASFVideoQuality, write=SetASFVideoQuality, default=80};
	__property int ASFVideoWidth = {read=GetASFVideoWidth, write=SetASFVideoWidth, default=80};
	__property double AspectRatioToUse = {read=GetAspectRatioToUse, write=SetAspectRatioToUse};
	__property bool AssociateAudioAndVideoDevices = {read=GetAssociateAudioAndVideoDevices, write=SetAssociateAudioAndVideoDevices, default=0};
	__property bool AVIDurationUpdated = {read=GetAVIDurationUpdated, write=SetAVIDurationUpdated, default=1};
	__property TAudioChannelRenderMode AudioChannelRenderMode = {read=GetAudioChannelRenderMode, write=SetAudioChannelRenderMode, default=0};
	__property int AudioCompressor = {read=GetAudioCompressor, write=SetAudioCompressor, nodefault};
	__property int AudioDevice = {read=GetAudioDevice, write=SetAudioDevice, default=-1};
	__property bool AudioDeviceRendering = {read=GetAudioDeviceRendering, write=SetAudioDeviceRendering, default=0};
	__property TAudioFormat AudioFormat = {read=GetAudioFormat, write=SetAudioFormat, default=0};
	__property int AudioInput = {read=GetAudioInput, write=SetAudioInput, default=-1};
	__property bool AudioPeakEvent = {read=GetAudioPeakEvent, write=SetAudioPeakEvent, default=0};
	__property bool AudioRecording = {read=GetAudioRecording, write=SetAudioRecording, default=0};
	__property int AudioRenderer = {read=GetAudioRenderer, write=SetAudioRenderer, default=-1};
	__property int AudioStreamNumber = {read=GetAudioStreamNumber, write=SetAudioStreamNumber, default=-1};
	__property bool AudioSyncAdjustmentEnabled = {read=GetAudioSyncAdjustmentEnabled, write=SetAudioSyncAdjustmentEnabled, default=0};
	__property int AudioSyncAdjustment = {read=GetAudioSyncAdjustment, write=SetAudioSyncAdjustment, default=0};
	__property TAudioSource AudioSource = {read=GetAudioSource, write=SetAudioSource, default=0};
	__property bool AutoConnectRelatedPins = {read=GetAutoConnectRelatedPins, write=SetAutoConnectRelatedPins, default=1};
	__property TAutoFileName AutoFileName = {read=GetAutoFileName, write=SetAutoFileName, default=0};
	__property int AutoFileNameMinDigits = {read=GetAutoFileNameMinDigits, write=SetAutoFileNameMinDigits, default=6};
	__property System::UnicodeString AutoFilePrefix = {read=GetAutoFilePrefix, write=SetAutoFilePrefix};
	__property bool AutoRefreshPreview = {read=GetAutoRefreshPreview, write=SetAutoRefreshPreview, default=0};
	__property bool AutoStartPlayer = {read=GetAutoStartPlayer, write=SetAutoStartPlayer, default=1};
	__property bool AVIFormatOpenDML = {read=GetAVIFormatOpenDML, write=SetAVIFormatOpenDML, default=1};
	__property bool AVIFormatOpenDMLCompatibilityIndex = {read=GetAVIFormatOpenDMLCompatibilityIndex, write=SetAVIFormatOpenDMLCompatibilityIndex, default=1};
	__property Graphics::TColor BackgroundColor = {read=GetBackgroundColor, write=SetBackgroundColor, default=0};
	__property Forms::TBorderStyle BorderStyle = {read=GetBorderStyle, write=SetBorderStyle, default=1};
	__property int BurstCount = {read=GetBurstCount, write=SetBurstCount, default=3};
	__property int BurstInterval = {read=GetBurstInterval, write=SetBurstInterval, default=0};
	__property bool BurstMode = {read=GetBurstMode, write=SetBurstMode, default=0};
	__property TFrameCaptureDest BurstType = {read=GetBurstType, write=SetBurstType, default=0};
	__property Controls::TCursor BusyCursor = {read=GetBusyCursor, write=SetBusyCursor, default=-11};
	__property Controls::TCursor VideoCursor = {read=GetVideoCursor, write=SetVideoCursor, default=0};
	__property Controls::TCursor NormalCursor = {read=GetNormalCursor, write=SetNormalCursor, default=0};
	__property bool CameraControlSettings = {read=GetCameraControlSettings, write=SetCameraControlSettings, default=1};
	__property System::UnicodeString CaptureFileExt = {read=GetCaptureFileExt, write=SetCaptureFileExt};
	__property Graphics::TColor ColorKey = {read=GetColorKey, write=SetColorKey, default=1048592};
	__property bool ColorKeyEnabled = {read=GetColorKeyEnabled, write=SetColorKeyEnabled, default=0};
	__property TCompressionMode CompressionMode = {read=GetCompressionMode, write=SetCompressionMode, default=0};
	__property TCompressionType CompressionType = {read=GetCompressionType, write=SetCompressionType, default=0};
	__property bool Display_Active = {read=GetDisplayBoolProperties, write=SetDisplayBoolProperties, index=0, default=1};
	__property bool Display_AlphaBlendEnabled = {read=GetDisplayBoolProperties, write=SetDisplayBoolProperties, index=5, default=0};
	__property int Display_AlphaBlendValue = {read=GetDisplayLongProperties, write=SetDisplayLongProperties, index=16, default=180};
	__property bool Display_AutoSize = {read=GetDisplayBoolProperties, write=SetDisplayBoolProperties, index=2, default=0};
	__property TAspectRatio Display_AspectRatio = {read=GetDisplayAspectRatio, write=SetDisplayAspectRatio, index=0, default=2};
	__property bool Display_Embedded = {read=GetDisplayBoolProperties, write=SetDisplayBoolProperties, index=3, default=1};
	__property bool Display_FullScreen = {read=GetDisplayBoolProperties, write=SetDisplayBoolProperties, index=6, default=0};
	__property int Display_Height = {read=GetDisplayLongProperties, write=SetDisplayLongProperties, index=13, default=240};
	__property int Display_Left = {read=GetDisplayLongProperties, write=SetDisplayLongProperties, index=17, default=10};
	__property int Display_Monitor = {read=GetDisplayLongProperties, write=SetDisplayLongProperties, index=10, default=0};
	__property bool Display_MouseMovesWindow = {read=GetDisplayBoolProperties, write=SetDisplayBoolProperties, index=8, default=1};
	__property int Display_PanScanRatio = {read=GetDisplayLongProperties, write=SetDisplayLongProperties, index=14, default=50};
	__property bool Display_StayOnTop = {read=GetDisplayBoolProperties, write=SetDisplayBoolProperties, index=7, default=0};
	__property int Display_Top = {read=GetDisplayLongProperties, write=SetDisplayLongProperties, index=18, default=10};
	__property bool Display_TransparentColorEnabled = {read=GetDisplayBoolProperties, write=SetDisplayBoolProperties, index=4, default=0};
	__property int Display_TransparentColorValue = {read=GetDisplayLongProperties, write=SetDisplayLongProperties, index=15, default=255};
	__property bool Display_VideoPortEnabled = {read=GetDisplayBoolProperties, write=SetDisplayBoolProperties, index=9, default=1};
	__property bool Display_Visible = {read=GetDisplayBoolProperties, write=SetDisplayBoolProperties, index=1, default=1};
	__property int Display_Width = {read=GetDisplayLongProperties, write=SetDisplayLongProperties, index=12, default=320};
	__property bool DoubleBuffered = {read=GetVideoDoubleBuffered, write=SetVideoDoubleBuffered, default=0};
	__property int DroppedFramesPollingInterval = {read=GetDroppedFramesPollingInterval, write=SetDroppedFramesPollingInterval, default=-1};
	__property bool DualDisplay_Active = {read=GetDisplay2BoolProperties, write=SetDisplay2BoolProperties, index=0, default=0};
	__property bool DualDisplay_AlphaBlendEnabled = {read=GetDisplay2BoolProperties, write=SetDisplay2BoolProperties, index=5, default=0};
	__property int DualDisplay_AlphaBlendValue = {read=GetDisplay2LongProperties, write=SetDisplay2LongProperties, index=16, default=180};
	__property bool DualDisplay_TransparentColorEnabled = {read=GetDisplay2BoolProperties, write=SetDisplay2BoolProperties, index=4, default=0};
	__property int DualDisplay_TransparentColorValue = {read=GetDisplay2LongProperties, write=SetDisplay2LongProperties, index=15, default=255};
	__property bool DualDisplay_AutoSize = {read=GetDisplay2BoolProperties, write=SetDisplay2BoolProperties, index=2, default=0};
	__property TAspectRatio DualDisplay_AspectRatio = {read=GetDisplayAspectRatio, write=SetDisplayAspectRatio, index=1, default=2};
	__property bool DualDisplay_Embedded = {read=GetDisplay2BoolProperties, write=SetDisplay2BoolProperties, index=3, default=0};
	__property bool DualDisplay_FullScreen = {read=GetDisplay2BoolProperties, write=SetDisplay2BoolProperties, index=6, default=0};
	__property int DualDisplay_Height = {read=GetDisplay2LongProperties, write=SetDisplay2LongProperties, index=13, default=240};
	__property int DualDisplay_Left = {read=GetDisplay2LongProperties, write=SetDisplay2LongProperties, index=17, default=20};
	__property int DualDisplay_Monitor = {read=GetDisplay2LongProperties, write=SetDisplay2LongProperties, index=10, default=0};
	__property bool DualDisplay_MouseMovesWindow = {read=GetDisplay2BoolProperties, write=SetDisplay2BoolProperties, index=8, default=1};
	__property int DualDisplay_PanScanRatio = {read=GetDisplay2LongProperties, write=SetDisplay2LongProperties, index=14, default=50};
	__property bool DualDisplay_StayOnTop = {read=GetDisplay2BoolProperties, write=SetDisplay2BoolProperties, index=7, default=0};
	__property int DualDisplay_Top = {read=GetDisplay2LongProperties, write=SetDisplay2LongProperties, index=18, default=400};
	__property bool DualDisplay_VideoPortEnabled = {read=GetDisplay2BoolProperties, write=SetDisplay2BoolProperties, index=9, default=0};
	__property bool DualDisplay_Visible = {read=GetDisplay2BoolProperties, write=SetDisplay2BoolProperties, index=1, default=1};
	__property int DualDisplay_Width = {read=GetDisplay2LongProperties, write=SetDisplay2LongProperties, index=12, default=320};
	__property bool DVDateTimeEnabled = {read=GetDVDateTimeEnabled, write=SetDVDateTimeEnabled, default=1};
	__property int DVDTitle = {read=GetDVDTitle, write=SetDVDTitle, default=0};
	__property int DVDiscontinuityMinimumInterval = {read=GetDVDiscontinuityMinimumInterval, write=SetDVDiscontinuityMinimumInterval, default=3};
	__property TDVVideoFormat DVEncoder_VideoFormat = {read=GetDVEncoder_VideoFormat, write=SetDVEncoder_VideoFormat, default=1};
	__property TDVSize DVEncoder_VideoResolution = {read=GetDVEncoder_VideoResolution, write=SetDVEncoder_VideoResolution, default=0};
	__property TDVVideoStandard DVEncoder_VideoStandard = {read=GetDVEncoder_VideoStandard, write=SetDVEncoder_VideoStandard, default=0};
	__property bool DVRgb219 = {read=GetDVRgb219, write=SetDVRgb219, default=0};
	__property bool DVReduceFrameRate = {read=GetDVReduceFrameRate, write=SetDVReduceFrameRate, default=0};
	__property bool DVRecordingInNativeFormatSeparatesStreams = {read=GetDVRecordingInNativeFormatSeparatesStreams, write=SetDVRecordingInNativeFormatSeparatesStreams, default=0};
	__property bool DVTimeCodeEnabled = {read=GetDVTimeCodeEnabled, write=SetDVTimeCodeEnabled, default=1};
	__property bool EventNotificationSynchrone = {read=GetEventNotificationSynchrone, write=SetEventNotificationSynchrone, default=1};
	__property bool Cropping_Enabled = {read=GetCropping_Enabled, write=SetCropping_Enabled, default=0};
	__property int Cropping_Height = {read=GetCropping_Height, write=SetCropping_Height, default=120};
	__property bool Cropping_Outbounds = {read=GetCropping_Outbounds, write=SetCropping_Outbounds, default=1};
	__property int Cropping_Width = {read=GetCropping_Width, write=SetCropping_Width, default=160};
	__property int Cropping_X = {read=GetCropping_X, write=SetCropping_X, default=0};
	__property int Cropping_Y = {read=GetCropping_Y, write=SetCropping_Y, default=0};
	__property double Cropping_Zoom = {read=GetCropping_Zoom, write=SetCropping_Zoom};
	__property bool FixFlickerOrBlackCapture = {read=GetFixFlickerOrBlackCapture, write=SetFixFlickerOrBlackCapture, default=0};
	__property bool FrameCaptureWithoutOverlay = {read=GetFrameCaptureWithoutOverlay, write=SetFrameCaptureWithoutOverlay, default=0};
	__property int FrameCaptureHeight = {read=GetFrameCaptureHeight, write=SetFrameCaptureHeight, default=-1};
	__property int FrameCaptureWidth = {read=GetFrameCaptureWidth, write=SetFrameCaptureWidth, default=-1};
	__property int FrameCaptureZoomSize = {read=GetFrameCaptureZoomSize, write=SetFrameCaptureZoomSize, default=100};
	__property TFrameGrabber FrameGrabber = {read=GetFrameGrabber, write=SetFrameGrabber, default=0};
	__property TFrameGrabberRGBFormat FrameGrabberRGBFormat = {read=GetFrameGrabberRGBFormat, write=SetFrameGrabberRGBFormat, default=0};
	__property bool FrameNumberStartsFromZero = {read=GetFrameNumberStartsFromZero, write=SetFrameNumberStartsFromZero, default=0};
	__property double FrameRate = {read=GetFrameRate, write=SetFrameRate};
	__property bool ImageOverlay_AlphaBlend = {read=GetImageOverlay_AlphaBlend, write=SetImageOverlay_AlphaBlend, default=0};
	__property int ImageOverlay_AlphaBlendValue = {read=GetImageOverlay_AlphaBlendValue, write=SetImageOverlay_AlphaBlendValue, default=180};
	__property bool ImageOverlay_ChromaKey = {read=GetImageOverlay_ChromaKey, write=SetImageOverlay_ChromaKey, default=0};
	__property int ImageOverlay_ChromaKeyLeewayPercent = {read=GetImageOverlay_ChromaKeyLeewayPercent, write=SetImageOverlay_ChromaKeyLeewayPercent, default=25};
	__property int ImageOverlay_ChromaKeyRGBColor = {read=GetImageOverlay_ChromaKeyRGBColor, write=SetImageOverlay_ChromaKeyRGBColor, default=0};
	__property int ImageOverlay_Height = {read=GetImageOverlay_Height, write=SetImageOverlay_Height, default=-1};
	__property int ImageOverlay_LeftLocation = {read=GetImageOverlay_LeftLocation, write=SetImageOverlay_LeftLocation, default=10};
	__property double ImageOverlay_RotationAngle = {read=GetImageOverlay_RotationAngle, write=SetImageOverlay_RotationAngle};
	__property bool ImageOverlay_StretchToVideoSize = {read=GetImageOverlay_StretchToVideoSize, write=SetImageOverlay_StretchToVideoSize, default=0};
	__property int ImageOverlay_TopLocation = {read=GetImageOverlay_TopLocation, write=SetImageOverlay_TopLocation, default=10};
	__property bool ImageOverlay_Transparent = {read=GetImageOverlay_Transparent, write=SetImageOverlay_Transparent, default=0};
	__property int ImageOverlay_TransparentColorValue = {read=GetImageOverlay_TransparentColorValue, write=SetImageOverlay_TransparentColorValue, default=0};
	__property bool ImageOverlay_UseTransparentColor = {read=GetImageOverlay_UseTransparentColor, write=SetImageOverlay_UseTransparentColor, default=0};
	__property int ImageOverlay_Width = {read=GetImageOverlay_Width, write=SetImageOverlay_Width, default=-1};
	__property bool ImageOverlayEnabled = {read=GetImageOverlayEnabled, write=SetImageOverlayEnabled, default=0};
	__property System::UnicodeString IPCameraURL = {read=GetIPCameraURL, write=SetIPCameraURL};
	__property Jpeg::TJPEGPerformance JPEGPerformance = {read=GetJPEGPerformance, write=SetJPEGPerformance, default=0};
	__property bool JPEGProgressiveDisplay = {read=GetJPEGProgressiveDisplay, write=SetJPEGProgressiveDisplay, default=0};
	__property int JPEGQuality = {read=GetJPEGQuality, write=SetJPEGQuality, default=100};
	__property System::UnicodeString LicenseString = {read=GetLicenseString, write=SetLicenseString};
	__property bool LogoDisplayed = {read=GetLogoDisplayed, write=SetLogoDisplayed, default=0};
	__property TLogoLayout LogoLayout = {read=GetLogoLayout, write=SetLogoLayout, default=1};
	__property int MixAudioSamples_CurrentSourceLevel = {read=GetMixAudioSamplesLevel, write=SetMixAudioSamplesLevel, index=0, default=100};
	__property int MixAudioSamples_ExternalSourceLevel = {read=GetMixAudioSamplesLevel, write=SetMixAudioSamplesLevel, index=1, default=100};
	__property int Mixer_MosaicColumns = {read=GetMixer_MosaicColumns, write=SetMixer_MosaicColumns, default=1};
	__property int Mixer_MosaicLines = {read=GetMixer_MosaicLines, write=SetMixer_MosaicLines, default=1};
	__property bool MotionDetector_CompareBlue = {read=GetMotionDetector_CompareBlue, write=SetMotionDetector_CompareBlue, default=1};
	__property bool MotionDetector_CompareGreen = {read=GetMotionDetector_CompareGreen, write=SetMotionDetector_CompareGreen, default=1};
	__property bool MotionDetector_GreyScale = {read=GetMotionDetector_GreyScale, write=SetMotionDetector_GreyScale, default=0};
	__property bool MotionDetector_CompareRed = {read=GetMotionDetector_CompareRed, write=SetMotionDetector_CompareRed, default=1};
	__property bool MotionDetector_Enabled = {read=GetMotionDetector_Enabled, write=SetMotionDetector_Enabled, default=0};
	__property System::UnicodeString MotionDetector_Grid = {read=GetMotionDetector_Grid, write=SetMotionDetector_Grid};
	__property double MotionDetector_MaxDetectionsPerSecond = {read=GetMotionDetector_MaxDetectionsPerSecond, write=SetMotionDetector_MaxDetectionsPerSecond};
	__property int MotionDetector_ReduceCPULoad = {read=GetMotionDetector_ReduceCPULoad, write=SetMotionDetector_ReduceCPULoad, default=1};
	__property bool MotionDetector_ReduceVideoNoise = {read=GetMotionDetector_ReduceVideoNoise, write=SetMotionDetector_ReduceVideoNoise, default=0};
	__property bool MotionDetector_Triggered = {read=GetMotionDetector_Triggered, write=SetMotionDetector_Triggered, default=0};
	__property bool MouseWheelEventEnabled = {read=GetMouseWheelEventEnabled, write=SetMouseWheelEventEnabled, default=0};
	__property bool MultiplexedInputEmulation = {read=GetMultiplexedInputEmulation, write=SetMultiplexedInputEmulation, default=1};
	__property TMultiplexedRole MultiplexedRole = {read=GetMultiplexedRole, write=SetMultiplexedRole, default=0};
	__property int MultiplexedSwitchDelay = {read=GetMultiplexedSwitchDelay, write=SetMultiplexedSwitchDelay, default=0};
	__property int MultiplexedStabilizationDelay = {read=GetMultiplexedStabilizationDelay, write=SetMultiplexedStabilizationDelay, default=100};
	__property int Multiplexer = {read=GetMultiplexer, write=SetMultiplexer, default=-1};
	__property bool MuteAudioRendering = {read=GetMuteAudioRendering, write=SetMuteAudioRendering, default=0};
	__property TNetworkStreaming NetworkStreaming = {read=GetNetworkStreaming, write=SetNetworkStreaming, default=0};
	__property TNetworkStreamingType NetworkStreamingType = {read=GetNetworkStreamingType, write=SetNetworkStreamingType, default=0};
	__property TNotificationMethod NotificationMethod = {read=GetNotificationMethod, write=SetNotificationMethod, default=1};
	__property int NotificationSleepTime = {read=GetNotificationSleepTime, write=SetNotificationSleepTime, default=-1};
	__property Classes::TThreadPriority NotificationPriority = {read=GetNotificationPriority, write=SetNotificationPriority, default=3};
	__property bool OverlayAfterTransform = {read=GetOverlayAfterTransform, write=SetOverlayAfterTransform, default=0};
	__property bool PlayerAudioRendering = {read=GetPlayerAudioRendering, write=SetPlayerAudioRendering, default=1};
	__property TDVSize PlayerDVSize = {read=GetPlayerDVSize, write=SetPlayerDVSize, default=0};
	__property int PlayerFastSeekSpeedRatio = {read=GetPlayerFastSeekSpeedRatio, write=SetPlayerFastSeekSpeedRatio, default=4};
	__property System::UnicodeString PlayerFileName = {read=GetPlayerFileName, write=SetPlayerFileName};
	__property System::UnicodeString PlayerForcedCodec = {read=GetPlayerForcedCodec, write=SetPlayerForcedCodec};
	__property bool PlayerRefreshPausedDisplay = {read=GetPlayerRefreshPausedDisplay, write=SetPlayerRefreshPausedDisplay, default=0};
	__property double PlayerRefreshPausedDisplayFrameRate = {read=GetPlayerRefreshPausedDisplayFrameRate, write=SetPlayerRefreshPausedDisplayFrameRate};
	__property double PlayerSpeedRatio = {read=GetPlayerSpeedRatio, write=SetPlayerSpeedRatio};
	__property Comctrls::TTrackBar* PlayerTrackBar = {read=GetPlayerTrackBar, write=SetPlayerTrackBar};
	__property Menus::TPopupMenu* PopupMenu = {read=Get_PopupMenu, write=SetPopupMenu};
	__property Classes::TComponent* PropertyPageSaver = {read=GetPropertyPageSaver, write=SetPropertyPageSaver};
	__property bool PlayerTrackBarSynchrone = {read=GetPlayerTrackBarSynchrone, write=SetPlayerTrackBarSynchrone, default=0};
	__property bool PreallocCapFileCopiedAfterRecording = {read=GetPreallocCapFileCopiedAfterRecording, write=SetPreallocCapFileCopiedAfterRecording, default=1};
	__property bool PreallocCapFileEnabled = {read=GetPreallocCapFileEnabled, write=SetPreallocCapFileEnabled, default=0};
	__property System::UnicodeString PreallocCapFileName = {read=GetPreallocCapFileName, write=SetPreallocCapFileName};
	__property int PreallocCapFileSizeInMB = {read=GetPreallocCapFileSizeInMB, write=SetPreallocCapFileSizeInMB, default=100};
	__property int PreviewZoomSize = {read=GetPreviewZoomSize, write=SetPreviewZoomSize, default=100};
	__property int RecordingAudioBitRate = {read=GetRecordingAudioBitRate, write=SetRecordingAudioBitRate, default=-1};
	__property int RecordingVideoBitRate = {read=GetRecordingVideoBitRate, write=SetRecordingVideoBitRate, default=-1};
	__property System::UnicodeString RecordingFileName = {read=GetRecordingFileName, write=SetRecordingFileName};
	__property bool RecordingInNativeFormat = {read=GetRecordingInNativeFormat, write=SetRecordingInNativeFormat, default=1};
	__property TRecordingMethod RecordingMethod = {read=GetRecordingMethod, write=SetRecordingMethod, default=0};
	__property bool RecordingPauseCreatesNewFile = {read=GetRecordingPauseCreatesNewFile, write=SetRecordingPauseCreatesNewFile, default=0};
	__property bool RecordingOnMotion_Enabled = {read=GetRecordingOnMotion_Enabled, write=SetRecordingOnMotion_Enabled, default=0};
	__property double RecordingOnMotion_MotionThreshold = {read=GetRecordingOnMotion_MotionThreshold, write=SetRecordingOnMotion_MotionThreshold};
	__property TRecordingTimer RecordingTimer = {read=GetRecordingTimer, write=SetRecordingTimer, default=0};
	__property int RecordingTimerInterval = {read=GetRecordingTimerInterval, write=SetRecordingTimerInterval, default=0};
	__property int RecordingOnMotion_NoMotionPauseDelayMs = {read=GetRecordingOnMotion_NoMotionPauseDelayMs, write=SetRecordingOnMotion_NoMotionPauseDelayMs, default=5000};
	__property System::UnicodeString Reencoding_NewVideoClip = {read=GetReencodingNewVideoClip, write=SetReencodingNewVideoClip};
	__property System::UnicodeString Reencoding_SourceVideoClip = {read=GetReencodingSourceVideoClip, write=SetReencodingSourceVideoClip};
	__property __int64 Reencoding_StartTime = {read=GetReencodingStartTime, write=SetReencodingStartTime, default=-1};
	__property __int64 Reencoding_StartFrame = {read=GetReencodingStartFrame, write=SetReencodingStartFrame, default=-1};
	__property __int64 Reencoding_StopTime = {read=GetReencodingStopTime, write=SetReencodingStopTime, default=-1};
	__property __int64 Reencoding_StopFrame = {read=GetReencodingStopFrame, write=SetReencodingStopFrame, default=-1};
	__property bool Reencoding_IncludeAudioStream = {read=GetReencodingIncludeAudioStream, write=SetReencodingIncludeAudioStream, default=1};
	__property bool Reencoding_IncludeVideoStream = {read=GetReencodingIncludeVideoStream, write=SetReencodingIncludeVideoStream, default=1};
	__property TRecordingMethod Reencoding_Method = {read=GetReencodingMethod, write=SetReencodingMethod, default=0};
	__property bool Reencoding_UseAudioCompressor = {read=GetReencodingUseAudioCompressor, write=SetReencodingUseAudioCompressor, default=0};
	__property bool Reencoding_UseFrameGrabber = {read=GetReencodingUseFrameGrabber, write=SetReencodingUseFrameGrabber, default=1};
	__property bool Reencoding_UseVideoCompressor = {read=GetReencodingUseVideoCompressor, write=SetReencodingUseVideoCompressor, default=0};
	__property bool Reencoding_WMVOutput = {read=GetReencodingWMVOutput, write=SetReencodingWMVOutput, default=0};
	__property bool QuickDeviceInitialization = {read=GetQuickDeviceInitialization, write=SetQuickDeviceInitialization, default=0};
	__property bool RawAudioSampleCapture = {read=GetRawAudioSampleCapture, write=SetRawAudioSampleCapture, default=0};
	__property bool RawCaptureAsyncEvent = {read=GetRawCaptureAsyncEvent, write=SetRawCaptureAsyncEvent, default=1};
	__property TRawSampleCaptureLocation RawSampleCaptureLocation = {read=GetRawSampleCaptureLocation, write=SetRawSampleCaptureLocation, default=0};
	__property bool RawVideoSampleCapture = {read=GetRawVideoSampleCapture, write=SetRawVideoSampleCapture, default=0};
	__property TRecordingSize RecordingSize = {read=GetRecordingSize, write=SetRecordingSize, default=0};
	__property bool ScreenRecordingLayeredWindows = {read=GetScreenRecordingLayeredWindows, write=SetScreenRecordingLayeredWindows, default=0};
	__property int ScreenRecordingMonitor = {read=GetScreenRecordingMonitor, write=SetScreenRecordingMonitor, default=0};
	__property bool ScreenRecordingNonVisibleWindows = {read=GetScreenRecordingNonVisibleWindows, write=SetScreenRecordingNonVisibleWindows, default=0};
	__property bool ScreenRecordingThroughClipboard = {read=GetScreenRecordingThroughClipboard, write=SetScreenRecordingThroughClipboard, default=0};
	__property bool ScreenRecordingWithCursor = {read=GetScreenRecordingWithCursor, write=SetScreenRecordingWithCursor, default=1};
	__property Extctrls::TShape* ShapeOverlay = {read=GetShapeOverlay, write=SetShapeOverlay};
	__property bool ShapeOverlayEnabled = {read=GetShapeOverlayEnabled, write=SetShapeOverlayEnabled, default=0};
	__property bool SpeakerControl = {read=GetSpeakerControl, write=SetSpeakerControl, default=0};
	__property System::UnicodeString StoragePath = {read=GetStoragePath, write=SetStoragePath, stored=false};
	__property bool StoreDeviceSettingsInRegistry = {read=GetStoreDeviceSettingsInRegistry, write=SetStoreDeviceSettingsInRegistry, default=1};
	__property TSynchronizationRole SynchronizationRole = {read=GetSynchronizationRole, write=SetSynchronizationRole, default=0};
	__property bool Synchronized = {read=GetSynchronized, write=SetSynchronized, default=0};
	__property bool SyncCommands = {read=GetSyncCommands, write=SetSyncCommands, default=1};
	__property TSyncPreview SyncPreview = {read=GetSyncPreview, write=SetSyncPreview, default=0};
	__property bool TextOverlay_Enabled = {read=GetTextOverlayEnabled, write=SetTextOverlayEnabled, default=0};
	__property int TextOverlay_Left = {read=GetTextOverlayLeft, write=SetTextOverlayLeft, default=0};
	__property int TextOverlay_Top = {read=GetTextOverlayTop, write=SetTextOverlayTop, default=0};
	__property int TextOverlay_Right = {read=GetTextOverlayRight, write=SetTextOverlayRight, default=-1};
	__property bool TextOverlay_Scrolling = {read=GetTextOverlayScrolling, write=SetTextOverlayScrolling, default=0};
	__property int TextOverlay_ScrollingSpeed = {read=GetTextOverlayScrollingSpeed, write=SetTextOverlayScrollingSpeed, default=1};
	__property bool TextOverlay_Shadow = {read=GetTextOverlayShadow, write=SetTextOverlayShadow, default=1};
	__property Graphics::TColor TextOverlay_ShadowColor = {read=GetTextOverlayShadowColor, write=SetTextOverlayShadowColor, default=0};
	__property TCardinalDirection TextOverlay_ShadowDirection = {read=GetTextOverlayShadowDirection, write=SetTextOverlayShadowDirection, default=7};
	__property Graphics::TColor TextOverlay_BkColor = {read=GetTextOverlayBkColor, write=SetTextOverlayBkColor, default=16777215};
	__property TTextOverlayAlign TextOverlay_Align = {read=GetTextOverlayAlign, write=SetTextOverlayAlign, default=0};
	__property Graphics::TFont* TextOverlay_Font = {read=GetTextOverlayFont, write=SetTextOverlayFont, default=0};
	__property System::UnicodeString TextOverlay_String = {read=GetTextOverlayString, write=SetTextOverlayString};
	__property bool TextOverlay_Transparent = {read=GetTextOverlayTransparent, write=SetTextOverlayTransparent, default=1};
	__property System::UnicodeString ThirdPartyDeinterlacer = {read=GetThirdPartyDeinterlacer, write=SetThirdPartyDeinterlacer};
	__property bool TranslateMouseCoordinates = {read=GetTranslateMouseCoordinates, write=SetTranslateMouseCoordinates, default=1};
	__property bool TVUseFrequencyOverrides = {read=GetTVUseFrequencyOverrides, write=SetTVUseFrequencyOverrides, default=0};
	__property int TunerFrequency = {read=GetTunerFrequency, write=SetTunerFrequency, default=-1};
	__property bool UseClock = {read=GetUseClock, write=SetUseClock, default=1};
	__property System::UnicodeString Version = {read=GetVersion, write=SetVersion, stored=false};
	__property int VideoDevice = {read=GetVideoDevice, write=SetVideoDevice, default=-1};
	__property int VideoCompression_DataRate = {read=GetVideoCompression_DataRate, write=SetVideoCompression_DataRate, default=-1};
	__property int VideoCompression_KeyFrameRate = {read=GetVideoCompression_KeyFrameRate, write=SetVideoCompression_KeyFrameRate, default=15};
	__property int VideoCompression_PFramesPerKeyFrame = {read=GetVideoCompression_PFramesPerKeyFrame, write=SetVideoCompression_PFramesPerKeyFrame, default=0};
	__property double VideoCompression_Quality = {read=GetVideoCompression_Quality, write=SetVideoCompression_Quality};
	__property int VideoCompression_WindowSize = {read=GetVideoCompression_WindowSize, write=SetVideoCompression_WindowSize, default=-1};
	__property int VideoCompressor = {read=GetVideoCompressor, write=SetVideoCompressor, nodefault};
	__property bool VideoControlSettings = {read=GetVideoControlSettings, write=SetVideoControlSettings, default=1};
	__property int VideoFormat = {read=GetVideoFormat, write=SetVideoFormat, default=-1};
	__property TFileSort VideoFromImages_BitmapsSortedBy = {read=GetVideoFromImages_BitmapsSortedBy, write=SetVideoFromImages_BitmapsSortedBy, default=0};
	__property bool VideoFromImages_RepeatIndefinitely = {read=GetVideoFromImages_RepeatIndefinitely, write=SetVideoFromImages_RepeatIndefinitely, default=0};
	__property System::UnicodeString VideoFromImages_SourceDirectory = {read=GetVideoFromImages_SourceDirectory, write=SetVideoFromImages_SourceDirectory, stored=false};
	__property System::UnicodeString VideoFromImages_TemporaryFile = {read=GetVideoFromImages_TemporaryFile, write=SetVideoFromImages_TemporaryFile};
	__property int VideoInput = {read=GetVideoInput, write=SetVideoInput, default=-1};
	__property int VideoProcessing_Brightness = {read=GetVideoProcessingBrightness, write=SetVideoProcessingBrightness, default=0};
	__property int VideoProcessing_Contrast = {read=GetVideoProcessingContrast, write=SetVideoProcessingContrast, default=0};
	__property TVideoDeinterlacing VideoProcessing_Deinterlacing = {read=GetVideoProcessingDeinterlacing, write=SetVideoProcessingDeinterlacing, default=0};
	__property bool VideoProcessing_FlipHorizontal = {read=GetVideoProcessingLeftRight, write=SetVideoProcessingLeftRight, default=0};
	__property bool VideoProcessing_FlipVertical = {read=GetVideoProcessingTopDown, write=SetVideoProcessingTopDown, default=0};
	__property bool VideoProcessing_GrayScale = {read=GetVideoProcessingGrayScale, write=SetVideoProcessingGrayScale, default=0};
	__property int VideoProcessing_Hue = {read=GetVideoProcessingHue, write=SetVideoProcessingHue, default=0};
	__property bool VideoProcessing_InvertColors = {read=GetVideoProcessingInvertColors, write=SetVideoProcessingInvertColors, default=0};
	__property int VideoProcessing_Pixellization = {read=GetVideoProcessingPixellization, write=SetVideoProcessingPixellization, default=1};
	__property int VideoProcessing_Saturation = {read=GetVideoProcessingSaturation, write=SetVideoProcessingSaturation, default=0};
	__property TVideoRotation VideoProcessing_Rotation = {read=GetVideoProcessingRotation, write=SetVideoProcessingRotation, default=0};
	__property double VideoProcessing_RotationCustomAngle = {read=GetVideoProcessingRotationCustomAngle, write=SetVideoProcessingRotationCustomAngle};
	__property bool VideoQualitySettings = {read=GetVideoQualitySettings, write=SetVideoQualitySettings, default=1};
	__property TVideoRenderer VideoRenderer = {read=GetVideoRenderer, write=SetVideoRenderer, default=0};
	__property TVideoRendererExternal VideoRendererExternal = {read=GetVideoRendererExternal, write=SetVideoRendererExternal, default=0};
	__property int VideoRendererExternalIndex = {read=GetVideoRendererExternalIndex, write=SetVideoRendererExternalIndex, default=-1};
	__property int VideoSize = {read=GetVideoSize, write=SetVideoSize, default=-1};
	__property TVideoSource VideoSource = {read=GetVideoSource, write=SetVideoSource, default=0};
	__property System::UnicodeString VideoSource_FileOrURL = {read=GetVideoSource_FileOrURL, write=SetVideoSource_FileOrURL};
	__property __int64 VideoSource_FileOrURL_StartTime = {read=GetVideoSource_FileOrURL_StartTime, write=SetVideoSource_FileOrURL_StartTime, default=-1};
	__property __int64 VideoSource_FileOrURL_StopTime = {read=GetVideoSource_FileOrURL_StopTime, write=SetVideoSource_FileOrURL_StopTime, default=-1};
	__property int VideoSubtype = {read=GetVideoSubtype, write=SetVideoSubtype, default=-1};
	__property bool VideoVisibleWhenStopped = {read=GetVideoVisibleWhenStopped, write=SetVideoVisibleWhenStopped, default=0};
	__property int VirtualVideoStreamControl = {read=GetVirtualVideoStreamControl, write=SetVirtualVideoStreamControl, default=-1};
	__property int VirtualAudioStreamControl = {read=GetVirtualAudioStreamControl, write=SetVirtualAudioStreamControl, default=-1};
	__property TVuMeter VuMeter = {read=GetVuMeter, write=SetVuMeter, default=0};
	__property TWebcamStillCaptureButton WebcamStillCaptureButton = {read=GetWebcamStillCaptureButton, write=SetWebcamStillCaptureButton, default=0};
	__property int ZoomCoeff = {read=GetZoomCoeff, write=SetZoomCoeff, default=1000};
	__property int ZoomXCenter = {read=GetZoomXCenter, write=SetZoomXCenter, default=0};
	__property int ZoomYCenter = {read=GetZoomYCenter, write=SetZoomYCenter, default=0};
	__property TOnAVIDurationUpdated OnAVIDurationUpdated = {read=FOnAVIDurationUpdated, write=FOnAVIDurationUpdated};
	__property TOnEventNotification OnAudioDeviceSelected = {read=FOnAudioDeviceSelected, write=FOnAudioDeviceSelected};
	__property TOnAudioPeak OnAudioPeak = {read=FOnAudioPeak, write=FOnAudioPeak};
	__property TOnEventNotification OnBacktimedFramesCountReached = {read=FOnBacktimedFramesCountReached, write=FOnBacktimedFramesCountReached};
	__property TOnProgressCommented OnBitmapsLoadingProgress = {read=FOnBitmapsLoadingProgress, write=FOnBitmapsLoadingProgress};
	__property TOnEventNotification OnClick = {read=FOnClick, write=FOnClick};
	__property TOnEventNotification OnDblClick = {read=FOnDblClick, write=FOnDblClick};
	__property TOnClientConnection OnClientConnection = {read=FOnClientConnection, write=FOnClientConnection};
	__property TOnColorKeyChange OnColorKeyChange = {read=FOnColorKeyChange, write=FOnColorKeyChange};
	__property TOnSourceFileToDestFileCompleted OnCopyPreallocDataCompleted = {read=FOnCopyPreallocDataCompleted, write=FOnCopyPreallocDataCompleted};
	__property TOnProgress OnCopyPreallocDataProgress = {read=FOnCopyPreallocDataProgress, write=FOnCopyPreallocDataProgress};
	__property TOnSourceFileToDestFileStarted OnCopyPreallocDataStarted = {read=FOnCopyPreallocDataStarted, write=FOnCopyPreallocDataStarted};
	__property TOnCreatePreallocatedFileCompleted OnCreatePreallocFileCompleted = {read=FOnCreatePreallocFileCompleted, write=FOnCreatePreallocFileCompleted};
	__property TOnProgress OnCreatePreallocFileProgress = {read=FOnCreatePreallocFileProgress, write=FOnCreatePreallocFileProgress};
	__property TOnFileNotification OnCreatePreallocFileStarted = {read=FOnCreatePreallocFileStarted, write=FOnCreatePreallocFileStarted};
	__property TOnDeviceArrivalOrRemoval OnDeviceArrivalOrRemoval = {read=FOnDeviceArrivalOrRemoval, write=Set_OnDeviceArrivalOrRemoval};
	__property TOnEventNotification OnDeviceLost = {read=FOnDeviceLost, write=FOnDeviceLost};
	__property TOnDirectNetworkStreamingHostUrl OnDirectNetworkStreamingHostUrl = {read=FOnDirectNetworkStreamingHostUrl, write=FOnDirectNetworkStreamingHostUrl};
	__property TOnEventNotification OnDiskFull = {read=FOnDiskFull, write=FOnDiskFull};
	__property TOnDVDiscontinuity OnDVDiscontinuity = {read=FOnDVDiscontinuity, write=FOnDVDiscontinuity};
	__property TOnDVCommandCompleted OnDVCommandCompleted = {read=FOnDVCommandCompleted, write=FOnDVCommandCompleted};
	__property TOnEnumerateWindows OnEnumerateWindows = {read=FOnEnumerateWindows, write=FOnEnumerateWindows};
	__property TOnFilterSelected OnFilterSelected = {read=FOnFilterSelected, write=FOnFilterSelected};
	__property TOnEventNotification OnFirstFrameReceived = {read=FOnFirstFrameReceived, write=FOnFirstFrameReceived};
	__property TOnFrameBitmap OnFrameBitmap = {read=FOnFrameBitmap, write=FOnFrameBitmap};
	__property TOnFrameCaptureCompleted OnFrameCaptureCompleted = {read=FOnFrameCaptureCompleted, write=FOnFrameCaptureCompleted};
	__property TOnFrameOverlayUsingDC OnFrameOverlayUsingDC = {read=FOnFrameOverlayUsingDC, write=FOnFrameOverlayUsingDC};
	__property TOnFrameOverlayUsingDIB OnFrameOverlayUsingDIB = {read=FOnFrameOverlayUsingDIB, write=FOnFrameOverlayUsingDIB};
	__property TOnFrameOverlayUsingVIDEOHDR OnFrameOverlayUsingVIDEOHDR = {read=FOnFrameOverlayUsingVIDEOHDR, write=FOnFrameOverlayUsingVIDEOHDR};
	__property TOnFrameProgress OnFrameProgress = {read=FOnFrameProgress, write=FOnFrameProgress};
	__property TOnFrameProgress2 OnFrameProgress2 = {read=FOnFrameProgress2, write=FOnFrameProgress2};
	__property TOnEventNotification OnGraphBuilt = {read=FOnGraphBuilt, write=FOnGraphBuilt};
	__property TOnEventNotification OnInactive = {read=FOnInactive, write=FOnInactive};
	__property TOnEventNotification OnLastCommandCompleted = {read=FOnLastCommandCompleted, write=FOnLastCommandCompleted};
	__property TOnVideoKeyPress OnKeyPress = {read=FOnKeyPress, write=FOnKeyPress};
	__property TOnLog OnLog = {read=FOnLog, write=FOnLog};
	__property TOnMotionDetected OnMotionDetected = {read=FOnMotionDetected, write=FOnMotionDetected};
	__property TOnMotionNotDetected OnMotionNotDetected = {read=FOnMotionNotDetected, write=FOnMotionNotDetected};
	__property TOnVideoMouseUpDown OnMouseDown = {read=FOnMouseDown, write=FOnMouseDown};
	__property TOnVideoMouseMove OnMouseMove = {read=FOnMouseMove, write=FOnMouseMove};
	__property TOnVideoMouseUpDown OnMouseUp = {read=FOnMouseUp, write=FOnMouseUp};
	__property TOnMouseWheel OnMouseWheel = {read=FOnMouseWheel, write=FOnMouseWheel};
	__property TOnEventNotification OnNoVideoDevices = {read=FOnNoVideoDevices, write=FOnNoVideoDevices};
	__property TOnPlayerBufferingData OnPlayerBufferingData = {read=FOnPlayerBufferingData, write=FOnPlayerBufferingData};
	__property TOnEventNotification OnPlayerEndOfPlaylist = {read=FOnPlayerEndOfPlaylist, write=FOnPlayerEndOfPlaylist};
	__property TOnEventNotification OnPlayerEndOfStream = {read=FOnPlayerEndOfStream, write=FOnPlayerEndOfStream};
	__property TOnEventNotification OnPlayerOpened = {read=FOnPlayerOpened, write=FOnPlayerOpened};
	__property TOnFrameProgress OnPlayerUpdateTrackbarPosition = {read=FOnPlayerUpdateTrackbarPosition, write=FOnPlayerUpdateTrackbarPosition};
	__property TOnEventNotification OnPreviewStarted = {read=FOnPreviewStarted, write=FOnPreviewStarted};
	__property TOnPlayerStateChanged OnPlayerStateChanged = {read=FOnPlayerStateChanged, write=FOnPlayerStateChanged};
	__property TOnAuthenticationNeeded OnAuthenticationNeeded = {read=FOnAuthenticationNeeded, write=FOnAuthenticationNeeded};
	__property TOnRawAudioSample OnRawAudioSample = {read=FOnRawAudioSample, write=FOnRawAudioSample};
	__property TOnRawVideoSample OnRawVideoSample = {read=FOnRawVideoSample, write=FOnRawVideoSample};
	__property TOnRecordingCompleted OnRecordingCompleted = {read=FOnRecordingCompleted, write=FOnRecordingCompleted};
	__property TOnEventNotification OnRecordingPaused = {read=FOnRecordingPaused, write=FOnRecordingPaused};
	__property TOnEventNotification OnRecordingReadyToStart = {read=FOnRecordingReadyToStart, write=FOnRecordingReadyToStart};
	__property TOnFileNotification OnRecordingStarted = {read=FOnRecordingStarted, write=FOnRecordingStarted};
	__property TOnSourceFileToDestFileCompleted OnReencodingCompleted = {read=FOnReencodingCompleted, write=FOnReencodingCompleted};
	__property TOnProgress OnReencodingProgress = {read=FOnReencodingProgress, write=FOnReencodingProgress};
	__property TOnSourceFileToDestFileStarted OnReencodingStarted = {read=FOnReencodingStarted, write=FOnReencodingStarted};
	__property TOnEventNotification OnReinitializing = {read=FOnReinitializing, write=FOnReinitializing};
	__property TOnResizeVideo OnResizeVideo = {read=FOnResizeVideo, write=FOnResizeVideo};
	__property TOnTextOverlayScrollingCompleted OnTextOverlayScrollingCompleted = {read=FOnTextOverlayScrollingCompleted, write=FOnTextOverlayScrollingCompleted};
	__property TOnThirdPartyFilterConnected OnThirdPartyFilterConnected = {read=FOnThirdPartyFilterConnected, write=FOnThirdPartyFilterConnected};
	__property TOnEventNotification OnTVChannelScanCompleted = {read=FOnTVChannelScanCompleted, write=FOnTVChannelScanCompleted};
	__property TOnTVChannelScanStarted OnTVChannelScanStarted = {read=FOnTVChannelScanStarted, write=FOnTVChannelScanStarted};
	__property TOnTVChannelSelected OnTVChannelSelected = {read=FOnTVChannelSelected, write=FOnTVChannelSelected};
	__property TOnVideoCompressionSettings OnVideoCompressionSettings = {read=FOnVideoCompressionSettings, write=FOnVideoCompressionSettings};
	__property TOnEventNotification OnVideoDeviceSelected = {read=FOnVideoDeviceSelected, write=FOnVideoDeviceSelected};
	__property TOnVideoFromBitmapsNextFrameNeeded OnVideoFromBitmapsNextFrameNeeded = {read=FOnVideoFromBitmapsNextFrameNeeded, write=FOnVideoFromBitmapsNextFrameNeeded};
public:
	/* TWinControl.CreateParented */ inline __fastcall TVideoGrabber(HWND ParentWindow) : Extctrls::TCustomPanel(ParentWindow) { }
	
};


//-- var, const, procedure ---------------------------------------------------
#define BuildID L"4359"
#define TVideoGrabberVersionID L"8.5"
#define TVideoGrabberVersionNumber L"8.5.2.5"
#define TVideoGrabberVersion L"v8.5 build 8.5.2.5 (build 4359) - August 24, 2011 - Copyri"\
	L"ght \u00a92002-2011 Datastead"
#define APPNAME L"TVideoGrabber"
static const bool DEF_AdjustOverlayAspectRatio = true;
static const bool DEF_AdjustPixelAspectRatio = true;
static const TAero DEF_Aero = (TAero)(0);
static const TApplicationPriority DEF_ApplicationPriority = (TApplicationPriority)(0);
static const System::ShortInt DEF_ASFAudioBitRate = -1;
static const System::ShortInt DEF_ASFAudioChannels = -1;
static const System::ShortInt DEF_ASFBufferWindow = -1;
static const TASFDeinterlaceMode DEF_ASFDeinterlaceMode = (TASFDeinterlaceMode)(0);
static const bool DEF_ASFFixedFrameRate = true;
static const bool DEF_ASFMediaServerRemovePublishingPointAfterDisconnect = false;
static const System::ShortInt DEF_ASFNetworkPort = 0x0;
static const System::ShortInt DEF_ASFNetworkMaxUsers = 0x5;
#define DEF_AspectRatioToUse  (-1.000000E+00)
static const System::ShortInt DEF_ASFProfile = -1;
static const TASFProfileVersion DEF_ASFProfileVersion = (TASFProfileVersion)(0);
static const System::ShortInt DEF_ASFVideoBitRate = -1;
static const System::ShortInt DEF_ASFVideoHeight = -1;
static const System::ShortInt DEF_ASFVideoMaxKeyFrameSpacing = -1;
static const System::ShortInt DEF_ASFVideoQuality = 0x50;
static const System::ShortInt DEF_ASFVideoWidth = -1;
static const bool DEF_AssociateAudioAndVideoDevices = false;
static const System::ShortInt DEF_AudioBalance = 0x0;
static const TAudioChannelRenderMode DEF_AudioChannelRenderMode = (TAudioChannelRenderMode)(0);
static const System::ShortInt DEF_AudioDevice = -1;
static const System::ShortInt DEF_AudioRenderer = -1;
static const System::Word DEF_AudioVolume = 0x7fff;
static const System::ShortInt DEF_AudioStreamNumber = -1;
static const TAudioFormat DEF_AudioFormat = (TAudioFormat)(0);
static const bool DEF_AudioDeviceRendering = false;
static const bool DEF_AudioSyncAdjustmentEnabled = false;
static const System::ShortInt DEF_AudioSyncAdjustment = 0x0;
static const System::ShortInt DEF_MixAudioSamplesLevel = 0x64;
static const System::ShortInt DEF_Mixer_MosaicColumns = 0x1;
static const System::ShortInt DEF_Mixer_MosaicLines = 0x1;
static const bool DEF_AudioPeakEvent = false;
static const bool DEF_AudioRecording = false;
static const TAudioSource DEF_AudioSource = (TAudioSource)(0);
static const TVuMeter DEF_VuMeter = (TVuMeter)(0);
static const bool DEF_AutoConnectRelatedPins = true;
static const TAutoFileName DEF_AutoFileName = (TAutoFileName)(0);
static const System::ShortInt DEF_AutoFileNameMinDigits = 0x6;
static const bool DEF_AutoRefreshPreview = false;
static const bool DEF_AutoStartPlayer = true;
static const bool DEF_AVIFormatOpenDML = true;
static const bool DEF_AVIFormatOpenDMLCompatibilityIndex = true;
static const bool DEF_AVIDurationUpdated = true;
static const int DEF_BackgroundColor = 0;
static const System::ShortInt DEF_BufferCount = -1;
static const System::ShortInt DEF_BurstCount = 0x3;
static const Forms::TFormBorderStyle DEF_BorderStyle = (Forms::TFormBorderStyle)(1);
static const System::ShortInt DEF_BurstInterval = 0x0;
static const bool DEF_BurstMode = false;
static const TFrameCaptureDest DEF_BurstType = (TFrameCaptureDest)(0);
static const short DEF_BusyCursor = -11;
static const short DEF_NormalCursor = 0;
static const bool DEF_CameraControlSettings = true;
static const int DEF_ColorKey = 0x100010;
static const bool DEF_ColorKeyEnabled = false;
static const TCompressionMode DEF_CompressionMode = (TCompressionMode)(0);
static const TCompressionType DEF_CompressionType = (TCompressionType)(0);
static const short DEF_Cursor = 0;
static const bool DEF_Display_Active = true;
static const bool DEF_Display_AlphaBlendEnabled = false;
static const System::Byte DEF_Display_AlphaBlendValue = 0xb4;
static const bool DEF_Display_TransparentColorEnabled = false;
static const System::Byte DEF_Display_TransparentColorValue = 0xff;
static const bool DEF_Display_AutoSize = false;
static const TAspectRatio DEF_Display_AspectRatio = (TAspectRatio)(2);
static const bool DEF_Display_Embedded = true;
static const System::ShortInt DEF_Display_ExternalRenderer = -1;
static const bool DEF_Display_FullScreen = false;
static const System::Byte DEF_Display_Height = 0xf0;
static const System::ShortInt DEF_Display_Left = 0xa;
static const bool DEF_Display_StayOnTop = false;
static const System::ShortInt DEF_Display_Monitor = 0x0;
static const bool DEF_Display_MouseMovesWindow = true;
static const System::ShortInt DEF_Display_PanScanRatio = 0x32;
static const System::ShortInt DEF_Display_Top = 0xa;
static const bool DEF_Display_VideoPortEnabled = true;
static const bool DEF_Display_Visible = true;
static const System::Word DEF_Display_Width = 0x140;
static const bool DEF_DoubleBuffered = false;
static const bool DEF_DualDisplay_AutoSize = false;
static const System::ShortInt DEF_DualDisplay_Left = 0x14;
static const System::Word DEF_DualDisplay_Top = 0x190;
static const System::ShortInt DEF_DroppedFramesPollingInterval = -1;
static const bool DEF_DualDisplay_Active = false;
static const bool DEF_DualDisplay_Embedded = false;
static const bool DEF_DualDisplay_VideoPortEnabled = false;
static const bool DEF_DVDateTimeEnabled = true;
static const System::ShortInt DEF_DVDTitle = 0x0;
static const System::ShortInt DEF_DVDiscontinuityMinimumInterval = 0x3;
static const bool DEF_DVRgb219 = false;
static const bool DEF_DVReduceFrameRate = false;
static const bool DEF_DVRecordingInNativeFormatSeparatesStreams = false;
static const bool DEF_DVTimeCodeEnabled = true;
static const bool DEF_EventNotificationSynchrone = true;
static const bool DEF_Cropping_Enabled = false;
static const System::ShortInt DEF_Cropping_Height = 0x78;
static const bool DEF_Cropping_Outbounds = true;
static const System::Byte DEF_Cropping_Width = 0xa0;
static const System::ShortInt DEF_Cropping_X = 0x0;
static const System::ShortInt DEF_Cropping_Y = 0x0;
#define DEF_Cropping_Zoom  (1.000000E+00)
static const TDVVideoFormat DEF_DVEncoder_VideoFormat = (TDVVideoFormat)(1);
static const TDVVideoStandard DEF_DVEncoder_VideoStandard = (TDVVideoStandard)(0);
static const TDVSize DEF_DVEncoder_VideoResolution = (TDVSize)(0);
static const bool DEF_FixFlickerOrBlackCapture = false;
static const bool DEF_FrameCaptureWithoutOverlay = false;
static const System::ShortInt DEF_FrameCaptureZoomSize = 0x64;
static const System::ShortInt DEF_FrameCaptureWidth = -1;
static const System::ShortInt DEF_FrameCaptureHeight = -1;
static const bool DEF_FrameNumberStartsFromZero = false;
static const TFrameGrabber DEF_FrameGrabber = (TFrameGrabber)(0);
static const TFrameGrabberRGBFormat DEF_FrameGrabberRGBFormat = (TFrameGrabberRGBFormat)(0);
static const System::Word DEF_IPCameraConnectionTimeout = 0xea60;
static const System::Word DEF_IPCameraReceiveTimeout = 0x1388;
static const bool DEF_HoldRecording = false;
static const bool DEF_ImageOverlay_AlphaBlend = false;
static const bool DEF_ImageOverlay_ChromaKey = false;
static const System::Byte DEF_ImageOverlay_AlphaBlendValue = 0xb4;
static const System::ShortInt DEF_ImageOverlay_ChromaKeyLeewayPercent = 0x19;
static const System::ShortInt DEF_ImageOverlay_ChromaKeyRGBColor = 0x0;
static const System::ShortInt DEF_ImageOverlay_Height = -1;
static const System::ShortInt DEF_ImageOverlay_LeftLocation = 0xa;
static const bool DEF_ImageOverlay_StretchToVideoSize = false;
static const System::ShortInt DEF_ImageOverlay_TopLocation = 0xa;
static const bool DEF_ImageOverlay_Transparent = false;
static const System::ShortInt DEF_ImageOverlay_TransparentColorValue = 0x0;
static const bool DEF_ImageOverlay_UseTransparentColor = false;
static const System::ShortInt DEF_ImageOverlay_Width = -1;
static const bool DEF_ImageOverlayEnabled = false;
static const Jpeg::TJPEGPerformance DEF_JPEGPerformance = (Jpeg::TJPEGPerformance)(0);
static const bool DEF_JPEGProgressiveDisplay = false;
static const System::ShortInt DEF_JPEGQuality = 0x64;
static const bool DEF_LogoDisplayed = false;
static const TLogoLayout DEF_LogoLayout = (TLogoLayout)(1);
static const bool DEF_MotionDetector_CompareBlue = true;
static const bool DEF_MotionDetector_CompareGreen = true;
static const bool DEF_MotionDetector_GreyScale = false;
static const bool DEF_MotionDetector_CompareRed = true;
static const bool DEF_MotionDetector_Enabled = false;
static const System::ShortInt DEF_MotionDetector_MaxDetectionsPerSecond = 0x0;
static const System::ShortInt DEF_MotionDetector_ReduceCPULoad = 0x1;
static const bool DEF_MotionDetector_ReduceVideoNoise = false;
static const bool DEF_MotionDetector_Triggered = false;
static const bool DEF_MouseWheelEventEnabled = false;
static const bool DEF_MultiplexedInputEmulation = true;
static const TMultiplexedRole DEF_MultiplexedRole = (TMultiplexedRole)(0);
static const System::ShortInt DEF_MultiplexedSwitchDelay = 0x0;
static const System::ShortInt DEF_MultiplexedStabilizationDelay = 0x64;
static const System::ShortInt DEF_Multiplexer = -1;
static const bool DEF_MuteAudioRendering = false;
static const TNetworkStreaming DEF_NetworkStreaming = (TNetworkStreaming)(0);
static const TNetworkStreamingType DEF_NetworkStreamingType = (TNetworkStreamingType)(0);
static const TNotificationMethod DEF_NotificationMethod = (TNotificationMethod)(1);
static const System::ShortInt DEF_NotificationSleepTime = -1;
static const Classes::TThreadPriority DEF_NotificationPriority = (Classes::TThreadPriority)(3);
static const bool DEF_OverlayAfterTransform = false;
static const bool DEF_OverlayEventCallbackAllowed = false;
static const bool DEF_PlayerAudioRendering = true;
static const TDVSize DEF_PlayerDVSize = (TDVSize)(0);
static const System::ShortInt DEF_PlayerFastSeekSpeedRatio = 0x4;
static const bool DEF_PlayerRefreshPausedDisplay = false;
#define DEF_PlayerRefreshPausedDisplayFrameRate  (0.000000E+00)
static const System::ShortInt DEF_PlayerSpeedRatio = 0x1;
static const bool DEF_PreallocCapFileCopiedAfterRecording = true;
static const bool DEF_PlayerTrackBarSynchrone = false;
static const bool DEF_PreallocCapFileEnabled = false;
static const System::ShortInt DEF_PreallocCapFileSizeInMB = 0x64;
static const System::ShortInt DEF_PreviewZoomSize = 0x64;
static const bool DEF_QuickDeviceInitialization = false;
static const bool DEF_RawAudioSampleCapture = false;
static const bool DEF_RawCaptureAsyncEvent = true;
static const TRawSampleCaptureLocation DEF_RawSampleCaptureLocation = (TRawSampleCaptureLocation)(0);
static const bool DEF_RawVideoSampleCapture = false;
static const System::ShortInt DEF_RecordingAudioBitRate = -1;
static const System::ShortInt DEF_RecordingVideoBitRate = -1;
static const bool DEF_RecordingInNativeFormat = true;
static const TRecordingMethod DEF_RecordingMethod = (TRecordingMethod)(0);
static const bool DEF_RecordingPauseCreatesNewFile = false;
static const bool DEF_RecordingOnMotion_Enabled = false;
static const System::ShortInt DEF_RecordingOnMotion_MotionThreshold = 0x0;
static const System::Word DEF_RecordingOnMotion_NoMotionPauseDelayMs = 0x1388;
static const TRecordingTimer DEF_RecordingTimer = (TRecordingTimer)(0);
static const System::ShortInt DEF_RecordingTimerInterval = 0x0;
static const System::ShortInt DEF_NoTimeSpecified = -1;
static const bool DEF_Reencoding_IncludeAudioStream = true;
static const bool DEF_Reencoding_IncludeVideoStream = true;
static const TRecordingMethod DEF_Reencoding_Method = (TRecordingMethod)(0);
static const bool DEF_Reencoding_UseAudioCompressor = false;
static const bool DEF_Reencoding_UseFrameGrabber = true;
static const bool DEF_Reencoding_UseVideoCompressor = false;
static const bool DEF_Reencoding_WMVOutput = false;
static const TRecordingSize DEF_RecordingSize = (TRecordingSize)(0);
static const System::ShortInt DEF_ScreenRecordingMonitor = 0x0;
static const bool DEF_ScreenRecordingLayeredWindows = false;
static const bool DEF_ScreenRecordingNonVisibleWindows = false;
static const bool DEF_ScreenRecordingThroughClipboard = false;
static const bool DEF_ScreenRecordingWithCursor = true;
static const System::ShortInt DEF_ServerLostRetryCount = 0x0;
static const System::ShortInt DEF_ServerLostRetryInterval = 0x5;
static const System::ShortInt DEF_SendToDV_DeviceIndex = -1;
static const bool DEF_ShapeOverlayEnabled = false;
static const bool DEF_SpeakerControl = false;
static const bool DEF_StoreDeviceSettingsInRegistry = true;
static const bool DEF_Synchronized = false;
static const TSynchronizationRole DEF_SynchronizationRole = (TSynchronizationRole)(0);
static const bool DEF_SyncCommands = true;
static const TSyncPreview DEF_SyncPreview = (TSyncPreview)(0);
static const bool DEF_TextOverlay_Enabled = false;
#define DEF_TextOverlay_Font (void*)(0)
static const int DEF_TextOverlay_FontColor = 0xffff00;
static const System::ShortInt DEF_TextOverlay_Left = 0x0;
static const System::ShortInt DEF_TextOverlay_Top = 0x0;
static const System::ShortInt DEF_TextOverlay_Right = -1;
static const bool DEF_TextOverlay_Scrolling = false;
static const System::ShortInt DEF_TextOverlay_ScrollingSpeed = 0x1;
static const bool DEF_TextOverlay_Shadow = true;
static const int DEF_TextOverlay_ShadowColor = 0;
static const TCardinalDirection DEF_TextOverlay_ShadowDirection = (TCardinalDirection)(7);
static const System::ShortInt DEF_TextOverlay_Selector = 0x0;
static const int DEF_TextOverlay_BkColor = 16777215;
static const TTextOverlayAlign DEF_TextOverlay_Align = (TTextOverlayAlign)(0);
static const bool DEF_TextOverlay_Transparent = true;
static const bool DEF_TranslateMouseCoordinates = true;
static const bool DEF_TVUseFrequencyOverrides = false;
static const System::ShortInt DEF_TunerFrequency = -1;
static const TTunerMode DEF_TunerMode = (TTunerMode)(0);
static const bool DEF_UseClock = true;
static const System::ShortInt DEF_VideoDevice = -1;
static const System::ShortInt DEF_VideoCompression_DataRate = -1;
static const System::ShortInt DEF_VideoCompression_KeyFrameRate = 0xf;
static const System::ShortInt DEF_VideoCompression_PFramesPerKeyFrame = 0x0;
static const System::ShortInt DEF_VideoCompression_Quality = 0x1;
static const System::ShortInt DEF_VideoCompression_WindowSize = -1;
static const System::ShortInt DEF_StoredValueDefaults = -1;
static const bool DEF_VideoControlSettings = true;
static const TFileSort DEF_VideoFromImages_BitmapsSortedBy = (TFileSort)(0);
static const bool DEF_VideoFromImages_RepeatIndefinitely = false;
static const System::ShortInt DEF_VideoProcessing_Brightness = 0x0;
static const System::ShortInt DEF_VideoProcessing_Contrast = 0x0;
static const TVideoDeinterlacing DEF_VideoProcessing_Deinterlacing = (TVideoDeinterlacing)(0);
static const bool DEF_VideoProcessing_FlipHorizontal = false;
static const bool DEF_VideoProcessing_FlipVertical = false;
static const bool DEF_VideoProcessing_GrayScale = false;
static const System::ShortInt DEF_VideoProcessing_Hue = 0x0;
static const bool DEF_VideoProcessing_InvertColors = false;
static const System::ShortInt DEF_VideoProcessing_Pixellization = 0x1;
static const System::ShortInt DEF_VideoProcessing_Saturation = 0x0;
static const TVideoRotation DEF_VideoProcessing_Rotation = (TVideoRotation)(0);
#define DEF_VideoProcessing_RotationCustomAngle  (4.550000E+01)
static const bool DEF_VideoQualitySettings = true;
static const TVideoRenderer DEF_VideoRenderer = (TVideoRenderer)(0);
static const TVideoRendererExternal DEF_VideoRendererExternal = (TVideoRendererExternal)(0);
static const System::ShortInt DEF_VideoRendererExternalIndex = -1;
static const TVideoSource DEF_VideoSource = (TVideoSource)(0);
static const bool DEF_VideoVisibleWhenStopped = false;
static const System::ShortInt DEF_VirtualVideoStreamControl = -1;
static const System::ShortInt DEF_VirtualAudioStreamControl = -1;
static const TWebcamStillCaptureButton DEF_WebcamStillCaptureButton = (TWebcamStillCaptureButton)(0);
static const System::ShortInt DEF_VUNotificationFps = 0xa;
static const System::ShortInt DEF_ZoomYCenter = 0x0;
static const System::ShortInt DEF_ZoomXCenter = 0x0;
static const System::Word DEF_ZoomCoeff = 0x3e8;
#define DATASTEADREGENTRY L"\\Software\\Datastead\\"
#define USERMODEID L"UserMode"
#define FILTERPARAM1ID L"FilterParam1="
#define VCLID L"VCLId:"
#define VALID L"VALId:"
#define DESIGNMODEID L"DesignMode"
#define IDEXCEPTIONSENABLED L"ExceptionsEnabled"
#define IDEXCEPTIONSDISABLED L"ExceptionsDisabled"
#define DEFAULTREGIDNAME L"\\Software\\Datastead\\TVideoGrabber\\"
#define IDRESETCOUNTER L"RESETCOUNTER"
extern PACKAGE bool CleanTVideoGrabberRegistryOnExit;
extern PACKAGE bool CodecsDisabledFromIDE;
extern PACKAGE TRegistryRoot RegRootKey;
extern PACKAGE System::UnicodeString RegKeyId;
extern PACKAGE bool VfwDriversEnabled;
extern PACKAGE GUID *CustomEnClsid;
extern PACKAGE void __fastcall Register(void);

}	/* namespace Vidgrab */
#if !defined(DELPHIHEADER_NO_IMPLICIT_NAMESPACE_USE)
using namespace Vidgrab;
#endif
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// VidgrabHPP
