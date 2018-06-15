object Form4: TForm4
  Left = 0
  Top = 0
  Caption = 'Form4'
  ClientHeight = 894
  ClientWidth = 796
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  OnKeyDown = FormKeyDown
  OnKeyPress = FormKeyPress
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 1077
    Top = 52
    Width = 96
    Height = 13
    Caption = 'Starting Parameters'
  end
  object Label2: TLabel
    Left = 1291
    Top = 52
    Width = 80
    Height = 13
    Caption = 'Final Parameters'
  end
  object Label3: TLabel
    Left = 1195
    Top = 52
    Width = 84
    Height = 13
    Caption = 'Parameter Range'
  end
  object Label4: TLabel
    Left = 990
    Top = 74
    Width = 77
    Height = 13
    Caption = 'Eye radius (mm)'
  end
  object Label5: TLabel
    Left = 954
    Top = 100
    Width = 94
    Height = 13
    Caption = 'eye position x (mm)'
  end
  object Label6: TLabel
    Left = 954
    Top = 126
    Width = 94
    Height = 13
    Caption = 'eye position y (mm)'
  end
  object Label7: TLabel
    Left = 955
    Top = 152
    Width = 93
    Height = 13
    Caption = 'eye position z (mm)'
  end
  object Label8: TLabel
    Left = 1019
    Top = 178
    Width = 47
    Height = 13
    Caption = 'yaw (rad)'
  end
  object Label9: TLabel
    Left = 1016
    Top = 204
    Width = 50
    Height = 13
    Caption = 'pitch (rad)'
  end
  object Label10: TLabel
    Left = 1025
    Top = 230
    Width = 41
    Height = 13
    Caption = 'roll (rad)'
  end
  object Label11: TLabel
    Left = 955
    Top = 256
    Width = 111
    Height = 13
    Caption = 'camera position x (mm)'
  end
  object Label12: TLabel
    Left = 955
    Top = 282
    Width = 111
    Height = 13
    Caption = 'camera position y (mm)'
  end
  object Label13: TLabel
    Left = 956
    Top = 308
    Width = 110
    Height = 13
    Caption = 'camera position z (mm)'
  end
  object Label14: TLabel
    Left = 934
    Top = 334
    Width = 132
    Height = 13
    Caption = 'focal length (camera pixels)'
  end
  object CalculateCalibrationBtn: TButton
    Left = 16
    Top = 469
    Width = 137
    Height = 25
    Caption = 'Calculate Calibration'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    OnClick = CalculateCalibrationBtnClick
  end
  object SaveCalibrationPointsBtn: TButton
    Left = 15
    Top = 818
    Width = 132
    Height = 26
    Caption = 'Save Calibration Points'
    TabOrder = 1
    OnClick = SaveCalibrationPointsBtnClick
  end
  object LoadCalibrationPointsBtn: TButton
    Left = 16
    Top = 850
    Width = 131
    Height = 25
    Caption = 'Load Calibration Points'
    TabOrder = 2
    OnClick = LoadCalibrationPointsBtnClick
  end
  object SaveCalibrationBtn: TButton
    Left = 16
    Top = 500
    Width = 137
    Height = 25
    Caption = 'Save Calibration'
    TabOrder = 3
    OnClick = SaveCalibrationBtnClick
  end
  object CameraWidthEdit: TLabeledEdit
    Left = 320
    Top = 773
    Width = 121
    Height = 21
    EditLabel.Width = 127
    EditLabel.Height = 13
    EditLabel.Caption = 'Eye Camera Width (pixels)'
    TabOrder = 4
    Text = '640'
    OnChange = CameraWidthEditChange
  end
  object CameraHeightEdit: TLabeledEdit
    Left = 320
    Top = 814
    Width = 121
    Height = 21
    EditLabel.Width = 130
    EditLabel.Height = 13
    EditLabel.Caption = 'Eye Camera Height (pixels)'
    TabOrder = 5
    Text = '480'
    OnChange = CameraHeightEditChange
  end
  object MonitorWidthEdit: TLabeledEdit
    Left = 186
    Top = 773
    Width = 121
    Height = 21
    EditLabel.Width = 94
    EditLabel.Height = 13
    EditLabel.Caption = 'Monitor Width (mm)'
    TabOrder = 6
    Text = '326'
    OnChange = MonitorWidthEditChange
  end
  object MonitorHeightEdit: TLabeledEdit
    Left = 186
    Top = 814
    Width = 121
    Height = 21
    EditLabel.Width = 97
    EditLabel.Height = 13
    EditLabel.Caption = 'Monitor Height (mm)'
    TabOrder = 7
    Text = '260'
    OnChange = MonitorHeightEditChange
  end
  object eyeCalibPanel: TPanel
    Left = 186
    Top = 53
    Width = 481
    Height = 327
    TabOrder = 8
  end
  object GridPanel1: TGridPanel
    Left = 1072
    Top = 70
    Width = 321
    Height = 294
    Caption = 'GridPanel1'
    ColumnCollection = <
      item
        Value = 33.333333333333330000
      end
      item
        Value = 33.333333333333330000
      end
      item
        Value = 33.333333333333330000
      end>
    ControlCollection = <
      item
        Column = 0
        Control = rInEdit
        Row = 0
      end
      item
        Column = 1
        Control = rRangeEdit
        Row = 0
      end
      item
        Column = 2
        Control = rOutEdit
        Row = 0
      end
      item
        Column = 0
        Control = x0InEdit
        Row = 1
      end
      item
        Column = 1
        Control = x0RangeEdit
        Row = 1
      end
      item
        Column = 2
        Control = x0OutEdit
        Row = 1
      end
      item
        Column = 0
        Control = y0InEdit
        Row = 2
      end
      item
        Column = 1
        Control = y0RangeEdit
        Row = 2
      end
      item
        Column = 2
        Control = y0OutEdit
        Row = 2
      end
      item
        Column = 0
        Control = z0InEdit
        Row = 3
      end
      item
        Column = 1
        Control = z0RangeEdit
        Row = 3
      end
      item
        Column = 2
        Control = z0OutEdit
        Row = 3
      end
      item
        Column = 0
        Control = aInEdit
        Row = 4
      end
      item
        Column = 1
        Control = aRangeEdit
        Row = 4
      end
      item
        Column = 2
        Control = aOutEdit
        Row = 4
      end
      item
        Column = 0
        Control = bInEdit
        Row = 5
      end
      item
        Column = 1
        Control = bRangeEdit
        Row = 5
      end
      item
        Column = 2
        Control = bOutEdit
        Row = 5
      end
      item
        Column = 0
        Control = gInEdit
        Row = 6
      end
      item
        Column = 1
        Control = gRangeEdit
        Row = 6
      end
      item
        Column = 2
        Control = gOutEdit
        Row = 6
      end
      item
        Column = 0
        Control = bxInEdit
        Row = 7
      end
      item
        Column = 1
        Control = bxRangeEdit
        Row = 7
      end
      item
        Column = 2
        Control = bxOutEdit
        Row = 7
      end
      item
        Column = 0
        Control = byInEdit
        Row = 8
      end
      item
        Column = 1
        Control = byRangeEdit
        Row = 8
      end
      item
        Column = 2
        Control = byOutEdit
        Row = 8
      end
      item
        Column = 0
        Control = bzInEdit
        Row = 9
      end
      item
        Column = 1
        Control = bzRangeEdit
        Row = 9
      end
      item
        Column = 2
        Control = bzOutEdit
        Row = 9
      end
      item
        Column = 0
        Control = fcInEdit
        Row = 10
      end
      item
        Column = 1
        Control = fcRangeEdit
        Row = 10
      end
      item
        Column = 2
        Control = fcOutEdit
        Row = 10
      end>
    RowCollection = <
      item
        Value = 9.090909090909085000
      end
      item
        Value = 9.090909090909085000
      end
      item
        Value = 9.090909090909092000
      end
      item
        Value = 9.090909090909092000
      end
      item
        Value = 9.090909090909092000
      end
      item
        Value = 9.090909090909092000
      end
      item
        Value = 9.090909090909092000
      end
      item
        Value = 9.090909090909092000
      end
      item
        Value = 9.090909090909092000
      end
      item
        Value = 9.090909090909092000
      end
      item
        Value = 9.090909090909092000
      end>
    TabOrder = 9
    object rInEdit: TEdit
      Left = 1
      Top = 1
      Width = 106
      Height = 26
      Align = alClient
      TabOrder = 0
      Text = '10.0'
      ExplicitHeight = 21
    end
    object rRangeEdit: TEdit
      Left = 107
      Top = 1
      Width = 106
      Height = 26
      Align = alClient
      TabOrder = 1
      Text = '3.0'
      ExplicitHeight = 21
    end
    object rOutEdit: TEdit
      Left = 213
      Top = 1
      Width = 107
      Height = 26
      Align = alClient
      TabOrder = 2
      Text = '0.0'
      ExplicitHeight = 21
    end
    object x0InEdit: TEdit
      Left = 1
      Top = 27
      Width = 106
      Height = 26
      Align = alClient
      TabOrder = 3
      Text = '0.0'
      ExplicitHeight = 21
    end
    object x0RangeEdit: TEdit
      Left = 107
      Top = 27
      Width = 106
      Height = 26
      Align = alClient
      TabOrder = 4
      Text = '50.0'
      ExplicitHeight = 21
    end
    object x0OutEdit: TEdit
      Left = 213
      Top = 27
      Width = 107
      Height = 26
      Align = alClient
      TabOrder = 5
      Text = '0.0'
      ExplicitHeight = 21
    end
    object y0InEdit: TEdit
      Left = 1
      Top = 53
      Width = 106
      Height = 26
      Align = alClient
      TabOrder = 6
      Text = '0.0'
      ExplicitHeight = 21
    end
    object y0RangeEdit: TEdit
      Left = 107
      Top = 53
      Width = 106
      Height = 26
      Align = alClient
      TabOrder = 7
      Text = '50.0'
      ExplicitHeight = 21
    end
    object y0OutEdit: TEdit
      Left = 213
      Top = 53
      Width = 107
      Height = 26
      Align = alClient
      TabOrder = 8
      Text = '0.0'
      ExplicitHeight = 21
    end
    object z0InEdit: TEdit
      Left = 1
      Top = 79
      Width = 106
      Height = 26
      Align = alClient
      TabOrder = 9
      Text = '0.0'
      ExplicitHeight = 21
    end
    object z0RangeEdit: TEdit
      Left = 107
      Top = 79
      Width = 106
      Height = 26
      Align = alClient
      TabOrder = 10
      Text = '50.0'
      ExplicitHeight = 21
    end
    object z0OutEdit: TEdit
      Left = 213
      Top = 79
      Width = 107
      Height = 26
      Align = alClient
      TabOrder = 11
      Text = '0.0'
      ExplicitHeight = 21
    end
    object aInEdit: TEdit
      Left = 1
      Top = 105
      Width = 106
      Height = 26
      Align = alClient
      TabOrder = 12
      Text = '-.83'
      ExplicitHeight = 21
    end
    object aRangeEdit: TEdit
      Left = 107
      Top = 105
      Width = 106
      Height = 26
      Align = alClient
      TabOrder = 13
      Text = '.788'
      ExplicitHeight = 21
    end
    object aOutEdit: TEdit
      Left = 213
      Top = 105
      Width = 107
      Height = 26
      Align = alClient
      TabOrder = 14
      Text = '0.0'
      ExplicitHeight = 21
    end
    object bInEdit: TEdit
      Left = 1
      Top = 131
      Width = 106
      Height = 26
      Align = alClient
      TabOrder = 15
      Text = '-1.03'
      ExplicitHeight = 21
    end
    object bRangeEdit: TEdit
      Left = 107
      Top = 131
      Width = 106
      Height = 26
      Align = alClient
      TabOrder = 16
      Text = '.788'
      ExplicitHeight = 21
    end
    object bOutEdit: TEdit
      Left = 213
      Top = 131
      Width = 107
      Height = 26
      Align = alClient
      TabOrder = 17
      Text = '0.0'
      ExplicitHeight = 21
    end
    object gInEdit: TEdit
      Left = 1
      Top = 157
      Width = 106
      Height = 26
      Align = alClient
      TabOrder = 18
      Text = '.53'
      ExplicitHeight = 21
    end
    object gRangeEdit: TEdit
      Left = 107
      Top = 157
      Width = 106
      Height = 26
      Align = alClient
      TabOrder = 19
      Text = '.788'
      ExplicitHeight = 21
    end
    object gOutEdit: TEdit
      Left = 213
      Top = 157
      Width = 107
      Height = 26
      Align = alClient
      TabOrder = 20
      Text = '0.0'
      ExplicitHeight = 21
    end
    object bxInEdit: TEdit
      Left = 1
      Top = 183
      Width = 106
      Height = 26
      Align = alClient
      TabOrder = 21
      Text = '20.0'
      ExplicitHeight = 21
    end
    object bxRangeEdit: TEdit
      Left = 107
      Top = 183
      Width = 106
      Height = 26
      Align = alClient
      TabOrder = 22
      Text = '10.0'
      ExplicitHeight = 21
    end
    object bxOutEdit: TEdit
      Left = 213
      Top = 183
      Width = 107
      Height = 26
      Align = alClient
      TabOrder = 23
      Text = '0.0'
      ExplicitHeight = 21
    end
    object byInEdit: TEdit
      Left = 1
      Top = 209
      Width = 106
      Height = 26
      Align = alClient
      TabOrder = 24
      Text = '0.0'
      ExplicitHeight = 21
    end
    object byRangeEdit: TEdit
      Left = 107
      Top = 209
      Width = 106
      Height = 26
      Align = alClient
      TabOrder = 25
      Text = '10.0'
      ExplicitHeight = 21
    end
    object byOutEdit: TEdit
      Left = 213
      Top = 209
      Width = 107
      Height = 26
      Align = alClient
      TabOrder = 26
      Text = '0.0'
      ExplicitHeight = 21
    end
    object bzInEdit: TEdit
      Left = 1
      Top = 235
      Width = 106
      Height = 26
      Align = alClient
      TabOrder = 27
      Text = '0.0'
      ExplicitHeight = 21
    end
    object bzRangeEdit: TEdit
      Left = 107
      Top = 235
      Width = 106
      Height = 26
      Align = alClient
      TabOrder = 28
      Text = '10.0'
      ExplicitHeight = 21
    end
    object bzOutEdit: TEdit
      Left = 213
      Top = 235
      Width = 107
      Height = 26
      Align = alClient
      TabOrder = 29
      Text = '0.0'
      ExplicitHeight = 21
    end
    object fcInEdit: TEdit
      Left = 1
      Top = 261
      Width = 106
      Height = 32
      Align = alClient
      TabOrder = 30
      Text = '563.0'
      ExplicitHeight = 21
    end
    object fcRangeEdit: TEdit
      Left = 107
      Top = 261
      Width = 106
      Height = 32
      Align = alClient
      TabOrder = 31
      Text = '0.0'
      ExplicitHeight = 21
    end
    object fcOutEdit: TEdit
      Left = 213
      Top = 261
      Width = 107
      Height = 32
      Align = alClient
      TabOrder = 32
      Text = '0.0'
      ExplicitHeight = 21
    end
  end
  object ClearCalibrationPointsBtn: TButton
    Left = 16
    Top = 787
    Width = 131
    Height = 25
    Caption = 'Clear Calibration Points'
    TabOrder = 10
    OnClick = ClearCalibrationPointsBtnClick
  end
  object sceneCalibPanel: TPanel
    Left = 186
    Top = 411
    Width = 481
    Height = 327
    TabOrder = 11
  end
  object GazeProgressBar: TProgressBar
    Left = 16
    Top = 76
    Width = 145
    Height = 17
    DoubleBuffered = False
    ParentDoubleBuffered = False
    TabOrder = 12
  end
  object SceneProgressBar: TProgressBar
    Left = 16
    Top = 136
    Width = 145
    Height = 21
    TabOrder = 13
  end
  object PhasespaceProgressBar: TProgressBar
    Left = 16
    Top = 193
    Width = 145
    Height = 17
    TabOrder = 14
  end
  object HeadMarker0Edit: TLabeledEdit
    Left = 16
    Top = 248
    Width = 59
    Height = 21
    EditLabel.Width = 70
    EditLabel.Height = 13
    EditLabel.Caption = 'Head Marker 0'
    TabOrder = 15
    Text = '0'
    OnChange = HeadMarker0EditChange
  end
  object HeadMarker1Edit: TLabeledEdit
    Left = 99
    Top = 248
    Width = 54
    Height = 21
    EditLabel.Width = 70
    EditLabel.Height = 13
    EditLabel.Caption = 'Head Marker 1'
    TabOrder = 16
    Text = '1'
    OnChange = HeadMarker1EditChange
  end
  object HeadMarker2Edit: TLabeledEdit
    Left = 16
    Top = 284
    Width = 59
    Height = 21
    EditLabel.Width = 70
    EditLabel.Height = 13
    EditLabel.Caption = 'Head Marker 2'
    TabOrder = 17
    Text = '2'
    OnChange = HeadMarker2EditChange
  end
  object HeadMarker3Edit: TLabeledEdit
    Left = 99
    Top = 284
    Width = 58
    Height = 21
    EditLabel.Width = 70
    EditLabel.Height = 13
    EditLabel.Caption = 'Head Marker 3'
    TabOrder = 18
    Text = '3'
    OnChange = HeadMarker3EditChange
  end
  object ReferenceHeadButton: TButton
    Left = 16
    Top = 407
    Width = 137
    Height = 25
    Caption = 'Reference Head Position'
    TabOrder = 19
    OnClick = ReferenceHeadButtonClick
  end
  object testEye: TButton
    Left = 931
    Top = 729
    Width = 137
    Height = 25
    Caption = 'testEye'
    TabOrder = 20
    OnClick = testEyeClick
  end
  object eyeTestDone: TButton
    Left = 931
    Top = 760
    Width = 137
    Height = 25
    Caption = 'eyeTestDone'
    TabOrder = 21
    OnClick = eyeTestDoneClick
  end
  object GazeComboBox: TComboBox
    AlignWithMargins = True
    Left = 16
    Top = 49
    Width = 137
    Height = 21
    TabOrder = 22
    Text = '<Select gaze stream>'
    OnChange = GazeComboBoxChange
  end
  object SceneComboBox: TComboBox
    AlignWithMargins = True
    Left = 20
    Top = 108
    Width = 137
    Height = 21
    TabOrder = 23
    Text = '<Select scene stream>'
    OnChange = SceneComboBoxChange
  end
  object PhaseComboBox: TComboBox
    AlignWithMargins = True
    Left = 16
    Top = 166
    Width = 137
    Height = 21
    TabOrder = 24
    Text = '<Select phase stream>'
    OnChange = PhaseComboBoxChange
  end
  object StatusMemo: TMemo
    Left = 8
    Top = 549
    Width = 162
    Height = 220
    Lines.Strings = (
      '')
    TabOrder = 25
  end
  object CalibrationWindowButton: TButton
    Left = 16
    Top = 376
    Width = 137
    Height = 25
    Caption = 'Calibration Window'
    TabOrder = 26
    OnClick = CalibrationWindowButtonClick
  end
  object phasespaceTestStart: TButton
    Left = 931
    Top = 837
    Width = 136
    Height = 25
    Caption = 'Phasespace Test Start'
    TabOrder = 27
    OnClick = phasespaceTestStartClick
  end
  object phasespaceTestStop: TButton
    Left = 931
    Top = 870
    Width = 136
    Height = 25
    Caption = 'Phasespace Test Stop'
    TabOrder = 28
    OnClick = phasespaceTestStopClick
  end
  object phasespaceMarker0Edit: TLabeledEdit
    Left = 624
    Top = 773
    Width = 65
    Height = 21
    EditLabel.Width = 102
    EditLabel.Height = 13
    EditLabel.Caption = 'Phasespace Marker 0'
    TabOrder = 29
    Text = '0'
    OnChange = phasespaceMarker0EditChange
  end
  object phasespaceMarker1Edit: TLabeledEdit
    Left = 624
    Top = 823
    Width = 65
    Height = 21
    EditLabel.Width = 102
    EditLabel.Height = 13
    EditLabel.Caption = 'Phasespace Marker 1'
    TabOrder = 30
    Text = '1'
    OnChange = phasespaceMarker1EditChange
  end
  object MonitorPositionButton: TButton
    Left = 16
    Top = 345
    Width = 137
    Height = 25
    Caption = 'Load Monitor Position'
    TabOrder = 31
    OnClick = MonitorPositionButtonClick
  end
  object AbortCalibrationButton: TButton
    Left = 16
    Top = 438
    Width = 137
    Height = 25
    Caption = 'Abort Calibration'
    TabOrder = 32
    OnClick = AbortCalibrationButtonClick
  end
  object LoadIntrinsicButton: TButton
    Left = 16
    Top = 314
    Width = 137
    Height = 25
    Caption = 'Load Intrinsic Parameters'
    TabOrder = 33
    OnClick = LoadIntrinsicButtonClick
  end
  object RefreshStreamsButton: TButton
    Left = 20
    Top = 16
    Width = 137
    Height = 25
    Caption = 'Refresh Streams'
    TabOrder = 34
    OnClick = RefreshStreamsButtonClick
  end
  object BackgroundRedEdit: TLabeledEdit
    Left = 480
    Top = 773
    Width = 65
    Height = 21
    EditLabel.Width = 112
    EditLabel.Height = 13
    EditLabel.Caption = 'Calib Background (Red)'
    TabOrder = 35
    Text = '0'
    OnChange = BackgroundRedEditChange
  end
  object BackgroundGreenEdit: TLabeledEdit
    Left = 480
    Top = 814
    Width = 65
    Height = 21
    EditLabel.Width = 122
    EditLabel.Height = 13
    EditLabel.Caption = 'Calib Background (Green)'
    TabOrder = 36
    Text = '0'
    OnChange = BackgroundGreenEditChange
  end
  object BackgroundBlueEdit: TLabeledEdit
    Left = 480
    Top = 865
    Width = 65
    Height = 21
    EditLabel.Width = 113
    EditLabel.Height = 13
    EditLabel.Caption = 'Calib Background (Blue)'
    TabOrder = 37
    Text = '0'
    OnChange = BackgroundBlueEditChange
  end
  object CalibDistanceEdit: TLabeledEdit
    Left = 186
    Top = 852
    Width = 121
    Height = 21
    EditLabel.Width = 162
    EditLabel.Height = 13
    EditLabel.Caption = 'Distance to Calibration Point (mm)'
    TabOrder = 38
  end
  object xd: TXMLDocument
    Left = 1322
    Top = 723
    DOMVendorDesc = 'MSXML'
  end
  object Timer1: TTimer
    Interval = 10
    OnTimer = Timer1Timer
    Left = 1474
    Top = 715
  end
  object xdoc_out: TXMLDocument
    Left = 1248
    Top = 723
    DOMVendorDesc = 'MSXML'
  end
  object SaveDialog1: TSaveDialog
    DefaultExt = 'xml'
    Left = 1416
    Top = 715
  end
  object Timer2: TTimer
    Left = 1368
    Top = 771
  end
  object OpenDialog1: TOpenDialog
    DefaultExt = 'csv'
    Left = 1272
    Top = 771
  end
  object SaveDialog2: TSaveDialog
    DefaultExt = 'csv'
    Left = 1416
    Top = 771
  end
  object Timer3: TTimer
    Enabled = False
    Interval = 10
    OnTimer = Timer3Timer
    Left = 112
    Top = 728
  end
  object OpenDialog2: TOpenDialog
    Left = 120
    Top = 675
  end
  object xdoc_in: TXMLDocument
    Left = 48
    Top = 672
    DOMVendorDesc = 'MSXML'
  end
end
