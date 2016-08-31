object MainCaptureForm: TMainCaptureForm
  Left = 0
  Top = 0
  Caption = 'GazeStream'
  ClientHeight = 797
  ClientWidth = 924
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -10
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 12
  object PageControl1: TPageControl
    Left = 6
    Top = 6
    Width = 491
    Height = 181
    ActivePage = TabSheet2
    TabOrder = 0
    TabPosition = tpBottom
    object TabSheet2: TTabSheet
      Caption = 'Video in'
      ImageIndex = 1
      ExplicitWidth = 381
      object Label1: TLabel
        Left = 11
        Top = 0
        Width = 58
        Height = 12
        Caption = 'Video Device'
      end
      object Label36: TLabel
        Left = 11
        Top = 80
        Width = 57
        Height = 12
        Caption = 'Input format'
      end
      object Label6: TLabel
        Left = 11
        Top = 41
        Width = 53
        Height = 12
        Caption = 'Video Input'
      end
      object Label8: TLabel
        Left = 10
        Top = 117
        Width = 63
        Height = 12
        Caption = 'Audio  Device'
      end
      object cbVideoInputDevice: TComboBox
        Left = 11
        Top = 15
        Width = 189
        Height = 20
        Style = csDropDownList
        TabOrder = 0
        OnChange = cbVideoInputDeviceChange
      end
      object cbVideoInputFormat: TComboBox
        Left = 11
        Top = 95
        Width = 189
        Height = 20
        Style = csDropDownList
        TabOrder = 1
        OnChange = cbVideoInputFormatChange
      end
      object RadioGroup1: TRadioGroup
        Left = 206
        Top = 3
        Width = 150
        Height = 89
        Caption = 'Use as:'
        Items.Strings = (
          'Eye camera'
          'Scene camera (Calibrate)'
          'Scene camera')
        TabOrder = 2
        OnClick = RadioGroup1Click
      end
      object edtFrameRate: TLabeledEdit
        Left = 362
        Top = 55
        Width = 77
        Height = 20
        EditLabel.Width = 94
        EditLabel.Height = 12
        EditLabel.Caption = 'Reported Frame Rate'
        TabOrder = 3
      end
      object cbVideoInput: TComboBox
        Left = 11
        Top = 56
        Width = 189
        Height = 21
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 4
        OnChange = cbVideoInputChange
      end
      object cbRecordAudio: TCheckBox
        Left = 206
        Top = 98
        Width = 49
        Height = 12
        Alignment = taLeftJustify
        Caption = 'Audio'
        TabOrder = 5
      end
      object cbAudioInputDevice: TComboBox
        Left = 16
        Top = 133
        Width = 182
        Height = 20
        TabOrder = 6
        OnChange = cbAudioInputDeviceChange
      end
      object edtRequestedFrameRate: TLabeledEdit
        Left = 362
        Top = 15
        Width = 77
        Height = 20
        EditLabel.Width = 99
        EditLabel.Height = 12
        EditLabel.Caption = 'Requested Frame Rate'
        TabOrder = 7
        Text = '30'
      end
      object measuredFrameRateEdit: TLabeledEdit
        Left = 362
        Top = 98
        Width = 77
        Height = 20
        EditLabel.Width = 96
        EditLabel.Height = 12
        EditLabel.Caption = 'Measured Frame Rate'
        TabOrder = 8
      end
    end
    object TabSheet1: TTabSheet
      Caption = 'Video out'
      ImageIndex = 2
      ExplicitWidth = 372
      object Label2: TLabel
        Left = 14
        Top = -2
        Width = 69
        Height = 12
        Caption = 'File name (.avi)'
      end
      object lbCodecs: TLabel
        Left = 14
        Top = 42
        Width = 33
        Height = 12
        Caption = 'Codecs'
        Visible = False
      end
      object Label4: TLabel
        Left = 11
        Top = 81
        Width = 47
        Height = 12
        Caption = 'Timestamp'
      end
      object Label3: TLabel
        Left = 107
        Top = 81
        Width = 38
        Height = 12
        Caption = 'Frame #'
      end
      object cbRecord: TCheckBox
        Left = 206
        Top = 60
        Width = 49
        Height = 12
        Alignment = taLeftJustify
        Caption = 'Record'
        TabOrder = 0
        OnClick = cbRecordClick
      end
      object cbVideoCodecs: TComboBox
        Left = 11
        Top = 58
        Width = 176
        Height = 20
        TabOrder = 1
      end
      object edOutput: TEdit
        Left = 13
        Top = 16
        Width = 255
        Height = 20
        TabOrder = 2
        Text = 'c:\capture'
      end
      object edTimestamp: TEdit
        Left = 11
        Top = 98
        Width = 87
        Height = 20
        TabOrder = 3
        Text = '0'
      end
      object edFrame: TEdit
        Left = 107
        Top = 98
        Width = 80
        Height = 20
        TabOrder = 4
        Text = '0'
      end
      object cbCompress: TCheckBox
        Left = 278
        Top = 60
        Width = 65
        Height = 12
        Alignment = taLeftJustify
        Caption = 'Compress'
        Checked = True
        State = cbChecked
        TabOrder = 5
        OnClick = cbRecordClick
      end
      object flipVertCheckbox: TCheckBox
        Left = 268
        Top = 78
        Width = 75
        Height = 12
        Alignment = taLeftJustify
        Caption = 'Flip verrtical'
        TabOrder = 6
        OnClick = cbRecordClick
      end
    end
  end
  object PageControl2: TPageControl
    Left = 6
    Top = 193
    Width = 910
    Height = 584
    ActivePage = tsVideo
    TabOrder = 1
    object tsScene: TTabSheet
      Caption = 'Scene'
      ImageIndex = 2
      object xParallaxCorrectionEdit: TLabeledEdit
        Left = 14
        Top = 160
        Width = 75
        Height = 20
        EditLabel.Width = 57
        EditLabel.Height = 12
        EditLabel.Caption = 'X Correction'
        TabOrder = 0
        Text = '0.0'
        OnChange = xParallaxCorrectionEditChange
      end
      object yParallaxCorrectionEdit: TLabeledEdit
        Left = 13
        Top = 200
        Width = 76
        Height = 20
        EditLabel.Width = 57
        EditLabel.Height = 12
        EditLabel.Caption = 'Y Correction'
        TabOrder = 1
        Text = '0.0'
        OnChange = yParallaxCorrectionEditChange
      end
      object LoadCalibration: TButton
        Left = 14
        Top = 238
        Width = 84
        Height = 25
        Caption = 'Load Calibration'
        TabOrder = 2
        OnClick = LoadCalibrationClick
      end
    end
    object tsVideo: TTabSheet
      Caption = 'Video'
      ImageIndex = 2
    end
    object tsEyeTracker: TTabSheet
      Caption = 'Eyetracker'
      object Label5: TLabel
        Left = 3
        Top = 425
        Width = 97
        Height = 12
        Caption = 'Scene Calibrate Color'
      end
      object Label7: TLabel
        Left = 3
        Top = 476
        Width = 114
        Height = 12
        Caption = 'Reference Calibrate Color'
      end
      object tbUpper: TTrackBar
        Left = 123
        Top = 70
        Width = 24
        Height = 130
        Max = 480
        Orientation = trVertical
        Frequency = 0
        Position = 40
        TabOrder = 0
      end
      object tbLower: TTrackBar
        Left = 124
        Top = 206
        Width = 25
        Height = 137
        Max = 480
        Orientation = trVertical
        Frequency = 0
        Position = 438
        TabOrder = 1
      end
      object tbLeft: TTrackBar
        Left = 143
        Top = 29
        Width = 112
        Height = 19
        Max = 640
        Frequency = 0
        Position = 65
        TabOrder = 2
      end
      object tbRight: TTrackBar
        Left = 251
        Top = 29
        Width = 122
        Height = 19
        Max = 640
        Frequency = 0
        Position = 543
        TabOrder = 3
      end
      object tbThreshold: TTrackBar
        Left = 379
        Top = 29
        Width = 526
        Height = 19
        Max = 255
        Frequency = 0
        TabOrder = 4
      end
      object SubsamplingEdit: TLabeledEdit
        Left = 3
        Top = 142
        Width = 78
        Height = 20
        EditLabel.Width = 57
        EditLabel.Height = 12
        EditLabel.Caption = 'Subsampling'
        TabOrder = 5
        Text = '1'
      end
      object autoThresholdBox: TCheckBox
        Left = 3
        Top = 47
        Width = 97
        Height = 17
        Caption = 'Auto Threshold'
        TabOrder = 6
      end
      object crRadiusMultiplierEdit: TLabeledEdit
        Left = 3
        Top = 206
        Width = 78
        Height = 20
        EditLabel.Width = 88
        EditLabel.Height = 12
        EditLabel.Caption = 'CR Radius Multiplier'
        TabOrder = 7
        Text = '3'
        OnChange = crRadiusMultiplierEditChange
      end
      object nOutlinesEdit: TLabeledEdit
        Left = 3
        Top = 86
        Width = 80
        Height = 20
        EditLabel.Width = 87
        EditLabel.Height = 12
        EditLabel.Caption = 'Number of Outlines'
        TabOrder = 8
        Text = '125'
        OnChange = nOutlinesEditChange
      end
      object crRoiTop: TTrackBar
        Left = 98
        Top = 70
        Width = 25
        Height = 130
        Max = 480
        Orientation = trVertical
        Frequency = 0
        Position = 301
        TabOrder = 9
      end
      object crRoiBottom: TTrackBar
        Left = 98
        Top = 206
        Width = 25
        Height = 137
        Max = 480
        Orientation = trVertical
        Frequency = 0
        Position = 387
        TabOrder = 10
      end
      object crRoiRight: TTrackBar
        Left = 251
        Top = 6
        Width = 122
        Height = 19
        Max = 640
        Frequency = 0
        Position = 420
        TabOrder = 11
      end
      object crRoiLeft: TTrackBar
        Left = 143
        Top = 6
        Width = 112
        Height = 19
        Max = 640
        Frequency = 0
        Position = 295
        TabOrder = 12
      end
      object crThreshold: TTrackBar
        Left = 379
        Top = 6
        Width = 526
        Height = 19
        Max = 255
        Frequency = 0
        Position = 255
        TabOrder = 13
      end
      object crRadiusMaxEdit: TLabeledEdit
        Left = 3
        Top = 242
        Width = 78
        Height = 20
        EditLabel.Width = 66
        EditLabel.Height = 12
        EditLabel.Caption = 'CR Radius Max'
        TabOrder = 14
        Text = '40'
        OnChange = crRadiusMaxEditChange
      end
      object maxEccentricityEdit: TLabeledEdit
        Left = 3
        Top = 385
        Width = 80
        Height = 20
        EditLabel.Width = 73
        EditLabel.Height = 12
        EditLabel.Caption = 'Max Eccentricity'
        TabOrder = 15
        Text = '0.9'
        OnChange = maxEccentricityEditChange
      end
      object cbSceneCalibColor: TComboBox
        Left = 3
        Top = 443
        Width = 110
        Height = 20
        ItemIndex = 0
        TabOrder = 16
        Text = 'Red'
        OnChange = cbSceneCalibColorChange
        Items.Strings = (
          'Red'
          'Green'
          'Blue'
          'White')
      end
      object eyeRadiusMaxEdit: TLabeledEdit
        Left = 3
        Top = 282
        Width = 78
        Height = 20
        EditLabel.Width = 70
        EditLabel.Height = 12
        EditLabel.Caption = 'Eye Radius Max'
        TabOrder = 17
        Text = '80'
        OnChange = eyeRadiusMaxEditChange
      end
      object cbReferenceCalibColor: TComboBox
        Left = 3
        Top = 491
        Width = 110
        Height = 20
        ItemIndex = 2
        TabOrder = 18
        Text = 'Blue'
        OnChange = cbReferenceCalibColorChange
        Items.Strings = (
          'Red'
          'Green'
          'Blue'
          'White')
      end
      object numberOfMarkersEdit: TLabeledEdit
        Left = 3
        Top = 529
        Width = 110
        Height = 20
        EditLabel.Width = 87
        EditLabel.Height = 12
        EditLabel.Caption = 'Number of Markers'
        TabOrder = 19
        Text = '5'
        OnChange = numberOfMarkersEditChange
      end
    end
  end
  object BitBtnPlay: TBitBtn
    Left = 647
    Top = 25
    Width = 64
    Height = 20
    DoubleBuffered = True
    Glyph.Data = {
      36040000424D3604000000000000360000002800000010000000100000000100
      2000000000000004000074120000741200000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000004CB000FF4CB000FF000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000004CB000FF4CB000FF4CB000FF4CB000FF00000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000004CB000FF4CB000FF4CB000FF4CB000FF4CB000FF4CB000FF0000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000004CB000FF4CB000FF4CB000FF4CB000FF4CB000FF4CB000FF4CB0
      00FF4CB000FF0000000000000000000000000000000000000000000000000000
      0000000000004CB000FF4CB000FF4CB000FF4CB000FF4CB000FF4CB000FF4CB0
      00FF4CB000FF4CB000FF4CB000FF000000000000000000000000000000000000
      0000000000004CB000FF4CB000FF4CB000FF4CB000FF4CB000FF4CB000FF4CB0
      00FF4CB000FF4CB000FF4CB000FF4CB000FF4CB000FF00000000000000000000
      0000000000004CB000FF4CB000FF4CB000FF4CB000FF4CB000FF4CB000FF4CB0
      00FF4CB000FF4CB000FF4CB000FF000000000000000000000000000000000000
      0000000000004CB000FF4CB000FF4CB000FF4CB000FF4CB000FF4CB000FF4CB0
      00FF4CB000FF0000000000000000000000000000000000000000000000000000
      0000000000004CB000FF4CB000FF4CB000FF4CB000FF4CB000FF4CB000FF0000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000004CB000FF4CB000FF4CB000FF4CB000FF00000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000004CB000FF4CB000FF000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000}
    ParentDoubleBuffered = False
    TabOrder = 2
    OnClick = BitBtnPlayClick
  end
  object BitBtnStop: TBitBtn
    Left = 725
    Top = 25
    Width = 59
    Height = 20
    DoubleBuffered = True
    Enabled = False
    Glyph.Data = {
      36040000424D3604000000000000360000002800000010000000100000000100
      2000000000000004000074120000741200000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000FF000000FF000000FF000000FF000000FF000000FF0000
      00FF000000FF0000000000000000000000000000000000000000000000000000
      000000000000000000FF000000FF000000FF000000FF000000FF000000FF0000
      00FF000000FF0000000000000000000000000000000000000000000000000000
      000000000000000000FF000000FF000000FF000000FF000000FF000000FF0000
      00FF000000FF0000000000000000000000000000000000000000000000000000
      000000000000000000FF000000FF000000FF000000FF000000FF000000FF0000
      00FF000000FF0000000000000000000000000000000000000000000000000000
      000000000000000000FF000000FF000000FF000000FF000000FF000000FF0000
      00FF000000FF0000000000000000000000000000000000000000000000000000
      000000000000000000FF000000FF000000FF000000FF000000FF000000FF0000
      00FF000000FF0000000000000000000000000000000000000000000000000000
      000000000000000000FF000000FF000000FF000000FF000000FF000000FF0000
      00FF000000FF0000000000000000000000000000000000000000000000000000
      000000000000000000FF000000FF000000FF000000FF000000FF000000FF0000
      00FF000000FF0000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000}
    ParentDoubleBuffered = False
    TabOrder = 3
    OnClick = btStopClick
  end
  object BacklogEdit: TLabeledEdit
    Left = 512
    Top = 105
    Width = 101
    Height = 20
    EditLabel.Width = 67
    EditLabel.Height = 12
    EditLabel.Caption = 'Frame Backlog'
    TabOrder = 4
  end
  object FrameDivisorEdit: TLabeledEdit
    Left = 512
    Top = 152
    Width = 101
    Height = 20
    EditLabel.Width = 97
    EditLabel.Height = 12
    EditLabel.Caption = 'Frame Display Divisor'
    TabOrder = 6
    Text = '1'
    OnChange = FrameDivisorEditChange
  end
  object tPanel: TPanel
    Left = 163
    Top = 270
    Width = 631
    Height = 492
    BevelOuter = bvNone
    BorderStyle = bsSingle
    Color = clNone
    TabOrder = 7
  end
  object droppedFramesEdit: TLabeledEdit
    Left = 512
    Top = 65
    Width = 101
    Height = 20
    EditLabel.Width = 120
    EditLabel.Height = 12
    EditLabel.Caption = 'Estimated Dropped Frames'
    TabOrder = 5
  end
  object SpatialDivisorEdit: TLabeledEdit
    Left = 632
    Top = 152
    Width = 101
    Height = 20
    EditLabel.Width = 98
    EditLabel.Height = 12
    EditLabel.Caption = 'Spatial Display Divisor'
    TabOrder = 8
    Text = '1'
    OnChange = SpatialDivisorEditChange
  end
  object IdentifierEdit: TLabeledEdit
    Left = 512
    Top = 25
    Width = 121
    Height = 20
    EditLabel.Width = 74
    EditLabel.Height = 12
    EditLabel.Caption = 'Stream Identifier'
    TabOrder = 9
    OnChange = IdentifierEditChange
  end
  object sendFrameCheckbox: TCheckBox
    Left = 848
    Top = 149
    Width = 89
    Height = 17
    Caption = 'Send Frame'
    TabOrder = 10
  end
  object FrameComboBox: TComboBox
    Left = 848
    Top = 123
    Width = 177
    Height = 20
    TabOrder = 11
    Text = '<Select Frame Command Stream>'
    OnChange = FrameComboBoxChange
  end
  object RefreshStreamsButton: TButton
    Left = 848
    Top = 85
    Width = 97
    Height = 25
    Caption = 'Refresh Streams'
    TabOrder = 12
    OnClick = RefreshStreamsButtonClick
  end
  object enableFrameSending: TCheckBox
    Left = 848
    Top = 62
    Width = 129
    Height = 17
    BiDiMode = bdLeftToRight
    Caption = 'Enable Frame Sending'
    ParentBiDiMode = False
    TabOrder = 13
    OnClick = enableFrameSendingClick
  end
  object Timer1: TTimer
    Interval = 50
    OnTimer = Timer1Timer
    Left = 792
    Top = 72
  end
  object OpenDialog1: TOpenDialog
    Left = 672
    Top = 72
  end
  object xdoc_in: TXMLDocument
    Left = 736
    Top = 72
    DOMVendorDesc = 'MSXML'
  end
end
