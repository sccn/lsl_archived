object Form4: TForm4
  Left = 0
  Top = 0
  Caption = 'Form4'
  ClientHeight = 913
  ClientWidth = 887
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
    Width = 112
    Height = 13
    Caption = 'monitor position x (mm)'
  end
  object Label6: TLabel
    Left = 954
    Top = 126
    Width = 112
    Height = 13
    Caption = 'monitor position y (mm)'
  end
  object Label7: TLabel
    Left = 955
    Top = 152
    Width = 111
    Height = 13
    Caption = 'monitor position z (mm)'
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
  object Label15: TLabel
    Left = 1017
    Top = 417
    Width = 47
    Height = 13
    Caption = 'yaw (rad)'
  end
  object Label16: TLabel
    Left = 1014
    Top = 442
    Width = 50
    Height = 13
    Caption = 'pitch (rad)'
  end
  object Label17: TLabel
    Left = 1023
    Top = 467
    Width = 41
    Height = 13
    Caption = 'roll (rad)'
  end
  object Label18: TLabel
    Left = 953
    Top = 492
    Width = 111
    Height = 13
    Caption = 'camera position x (mm)'
  end
  object Label19: TLabel
    Left = 953
    Top = 517
    Width = 111
    Height = 13
    Caption = 'camera position y (mm)'
  end
  object Label20: TLabel
    Left = 955
    Top = 542
    Width = 110
    Height = 13
    Caption = 'camera position z (mm)'
  end
  object Label21: TLabel
    Left = 931
    Top = 567
    Width = 132
    Height = 13
    Caption = 'focal length (camera pixels)'
  end
  object Label22: TLabel
    Left = 924
    Top = 592
    Width = 139
    Height = 13
    Caption = 'distortion factor (1/pixels^2)'
  end
  object Label23: TLabel
    Left = 940
    Top = 617
    Width = 124
    Height = 13
    Caption = 'distortion offset y (pixels)'
  end
  object Label24: TLabel
    Left = 941
    Top = 642
    Width = 123
    Height = 13
    Caption = 'distortion offset z (pixels)'
  end
  object CalculateCalibrationBtn: TButton
    Left = 8
    Top = 140
    Width = 131
    Height = 25
    Caption = 'Calculate Calibration'
    TabOrder = 0
    OnClick = CalculateCalibrationBtnClick
  end
  object SaveCalibrationPointsBtn: TButton
    Left = 8
    Top = 77
    Width = 129
    Height = 26
    Caption = 'Save Calibration Points'
    TabOrder = 1
    OnClick = SaveCalibrationPointsBtnClick
  end
  object LoadCalibrationPointsBtn: TButton
    Left = 8
    Top = 109
    Width = 131
    Height = 25
    Caption = 'Load Calibration Points'
    TabOrder = 2
    OnClick = LoadCalibrationPointsBtnClick
  end
  object SaveCalibrationBtn: TButton
    Left = 8
    Top = 171
    Width = 131
    Height = 25
    Caption = 'Save Calibration'
    TabOrder = 3
    OnClick = SaveCalibrationBtnClick
  end
  object CameraWidthEdit: TLabeledEdit
    Left = 8
    Top = 310
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
    Left = 8
    Top = 351
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
    Left = 8
    Top = 220
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
    Left = 8
    Top = 263
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
    Left = 282
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
        Value = 33.333333333333340000
      end
      item
        Value = 33.333333333333340000
      end
      item
        Value = 33.333333333333340000
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
      Text = '-570.0'
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
      Text = '130.0'
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
      Text = '300.0'
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
    Left = 8
    Top = 46
    Width = 131
    Height = 25
    Caption = 'Clear Calibration Points'
    TabOrder = 10
    OnClick = ClearCalibrationPointsBtnClick
  end
  object sceneCalibPanel: TPanel
    Left = 282
    Top = 411
    Width = 481
    Height = 327
    TabOrder = 11
  end
  object GridPanel2: TGridPanel
    Left = 1070
    Top = 413
    Width = 321
    Height = 257
    Caption = 'GridPanel2'
    ColumnCollection = <
      item
        Value = 33.333333333333340000
      end
      item
        Value = 33.333333333333340000
      end
      item
        Value = 33.333333333333340000
      end>
    ControlCollection = <
      item
        Column = 0
        Control = aInSceneEdit
        Row = 0
      end
      item
        Column = 1
        Control = aRangeSceneEdit
        Row = 0
      end
      item
        Column = 2
        Control = aOutSceneEdit
        Row = 0
      end
      item
        Column = 0
        Control = bInSceneEdit
        Row = 1
      end
      item
        Column = 1
        Control = bRangeSceneEdit
        Row = 1
      end
      item
        Column = 2
        Control = bOutSceneEdit
        Row = 1
      end
      item
        Column = 0
        Control = gInSceneEdit
        Row = 2
      end
      item
        Column = 1
        Control = gRangeSceneEdit
        Row = 2
      end
      item
        Column = 2
        Control = gOutSceneEdit
        Row = 2
      end
      item
        Column = 0
        Control = bxInSceneEdit
        Row = 3
      end
      item
        Column = 1
        Control = bxRangeSceneEdit
        Row = 3
      end
      item
        Column = 2
        Control = bxOutSceneEdit
        Row = 3
      end
      item
        Column = 0
        Control = byInSceneEdit
        Row = 4
      end
      item
        Column = 1
        Control = byRangeSceneEdit
        Row = 4
      end
      item
        Column = 2
        Control = byOutSceneEdit
        Row = 4
      end
      item
        Column = 0
        Control = bzInSceneEdit
        Row = 5
      end
      item
        Column = 1
        Control = bzRangeSceneEdit
        Row = 5
      end
      item
        Column = 2
        Control = bzOutSceneEdit
        Row = 5
      end
      item
        Column = 0
        Control = fcInSceneEdit
        Row = 6
      end
      item
        Column = 1
        Control = fcRangeSceneEdit
        Row = 6
      end
      item
        Column = 2
        Control = fcOutSceneEdit
        Row = 6
      end
      item
        Column = 0
        Control = kInSceneEdit
        Row = 7
      end
      item
        Column = 1
        Control = kRangeSceneEdit
        Row = 7
      end
      item
        Column = 2
        Control = kOutSceneEdit
        Row = 7
      end
      item
        Column = 0
        Control = ydInSceneEdit
        Row = 8
      end
      item
        Column = 1
        Control = ydRangeSceneEdit
        Row = 8
      end
      item
        Column = 2
        Control = ydOutSceneEdit
        Row = 8
      end
      item
        Column = 0
        Control = zdInSceneEdit
        Row = 9
      end
      item
        Column = 1
        Control = zdRangeSceneEdit
        Row = 9
      end
      item
        Column = 2
        Control = zdOutSceneEdit
        Row = 9
      end>
    RowCollection = <
      item
        Value = 10.000000000000000000
      end
      item
        Value = 10.000000000000000000
      end
      item
        Value = 10.000000000000000000
      end
      item
        Value = 10.000000000000000000
      end
      item
        Value = 10.000000000000000000
      end
      item
        Value = 10.000000000000000000
      end
      item
        Value = 10.000000000000000000
      end
      item
        Value = 10.000000000000000000
      end
      item
        Value = 10.000000000000000000
      end
      item
        Value = 10.000000000000000000
      end>
    TabOrder = 12
    object aInSceneEdit: TEdit
      Left = 1
      Top = 1
      Width = 106
      Height = 25
      Align = alClient
      TabOrder = 0
      Text = '0.0'
      ExplicitHeight = 21
    end
    object aRangeSceneEdit: TEdit
      Left = 107
      Top = 1
      Width = 106
      Height = 25
      Align = alClient
      TabOrder = 1
      Text = '1.5'
      ExplicitHeight = 21
    end
    object aOutSceneEdit: TEdit
      Left = 213
      Top = 1
      Width = 107
      Height = 25
      Align = alClient
      TabOrder = 2
      Text = '0.0'
      ExplicitHeight = 21
    end
    object bInSceneEdit: TEdit
      Left = 1
      Top = 26
      Width = 106
      Height = 25
      Align = alClient
      TabOrder = 3
      Text = '0.0'
      ExplicitHeight = 21
    end
    object bRangeSceneEdit: TEdit
      Left = 107
      Top = 26
      Width = 106
      Height = 25
      Align = alClient
      TabOrder = 4
      Text = '1.5'
      ExplicitHeight = 21
    end
    object bOutSceneEdit: TEdit
      Left = 213
      Top = 26
      Width = 107
      Height = 25
      Align = alClient
      TabOrder = 5
      Text = '0.0'
      ExplicitHeight = 21
    end
    object gInSceneEdit: TEdit
      Left = 1
      Top = 51
      Width = 106
      Height = 25
      Align = alClient
      TabOrder = 6
      Text = '0.0'
      ExplicitHeight = 21
    end
    object gRangeSceneEdit: TEdit
      Left = 107
      Top = 51
      Width = 106
      Height = 25
      Align = alClient
      TabOrder = 7
      Text = '1.5'
      ExplicitHeight = 21
    end
    object gOutSceneEdit: TEdit
      Left = 213
      Top = 51
      Width = 107
      Height = 25
      Align = alClient
      TabOrder = 8
      Text = '0.0'
      ExplicitHeight = 21
    end
    object bxInSceneEdit: TEdit
      Left = 1
      Top = 76
      Width = 106
      Height = 25
      Align = alClient
      TabOrder = 9
      Text = '650.0'
      ExplicitHeight = 21
    end
    object bxRangeSceneEdit: TEdit
      Left = 107
      Top = 76
      Width = 106
      Height = 25
      Align = alClient
      TabOrder = 10
      Text = '50.0'
      ExplicitHeight = 21
    end
    object bxOutSceneEdit: TEdit
      Left = 213
      Top = 76
      Width = 107
      Height = 25
      Align = alClient
      TabOrder = 11
      Text = '0.0'
      ExplicitHeight = 21
    end
    object byInSceneEdit: TEdit
      Left = 1
      Top = 101
      Width = 106
      Height = 25
      Align = alClient
      TabOrder = 12
      Text = '350.0'
      ExplicitHeight = 21
    end
    object byRangeSceneEdit: TEdit
      Left = 107
      Top = 101
      Width = 106
      Height = 25
      Align = alClient
      TabOrder = 13
      Text = '50.0'
      ExplicitHeight = 21
    end
    object byOutSceneEdit: TEdit
      Left = 213
      Top = 101
      Width = 107
      Height = 25
      Align = alClient
      TabOrder = 14
      Text = '0.0'
      ExplicitHeight = 21
    end
    object bzInSceneEdit: TEdit
      Left = 1
      Top = 126
      Width = 106
      Height = 25
      Align = alClient
      TabOrder = 15
      Text = '310.0'
      ExplicitHeight = 21
    end
    object bzRangeSceneEdit: TEdit
      Left = 107
      Top = 126
      Width = 106
      Height = 25
      Align = alClient
      TabOrder = 16
      Text = '50.0'
      ExplicitHeight = 21
    end
    object bzOutSceneEdit: TEdit
      Left = 213
      Top = 126
      Width = 107
      Height = 25
      Align = alClient
      TabOrder = 17
      Text = '0.0'
      ExplicitHeight = 21
    end
    object fcInSceneEdit: TEdit
      Left = 1
      Top = 151
      Width = 106
      Height = 25
      Align = alClient
      TabOrder = 18
      Text = '419.0'
      ExplicitHeight = 21
    end
    object fcRangeSceneEdit: TEdit
      Left = 107
      Top = 151
      Width = 106
      Height = 25
      Align = alClient
      TabOrder = 19
      Text = '0.0'
      ExplicitHeight = 21
    end
    object fcOutSceneEdit: TEdit
      Left = 213
      Top = 151
      Width = 107
      Height = 25
      Align = alClient
      TabOrder = 20
      Text = '0.0'
      ExplicitHeight = 21
    end
    object kInSceneEdit: TEdit
      Left = 1
      Top = 176
      Width = 106
      Height = 25
      Align = alClient
      TabOrder = 21
      Text = '0.0'
      ExplicitHeight = 21
    end
    object kRangeSceneEdit: TEdit
      Left = 107
      Top = 176
      Width = 106
      Height = 25
      Align = alClient
      TabOrder = 22
      Text = '-1E-06'
      ExplicitHeight = 21
    end
    object kOutSceneEdit: TEdit
      Left = 213
      Top = 176
      Width = 107
      Height = 25
      Align = alClient
      TabOrder = 23
      Text = '0.0'
      ExplicitHeight = 21
    end
    object ydInSceneEdit: TEdit
      Left = 1
      Top = 201
      Width = 106
      Height = 25
      Align = alClient
      TabOrder = 24
      Text = '0.0'
      ExplicitHeight = 21
    end
    object ydRangeSceneEdit: TEdit
      Left = 107
      Top = 201
      Width = 106
      Height = 25
      Align = alClient
      TabOrder = 25
      Text = '1000.0'
      ExplicitHeight = 21
    end
    object ydOutSceneEdit: TEdit
      Left = 213
      Top = 201
      Width = 107
      Height = 25
      Align = alClient
      TabOrder = 26
      Text = '0.0'
      ExplicitHeight = 21
    end
    object zdInSceneEdit: TEdit
      Left = 1
      Top = 226
      Width = 106
      Height = 30
      Align = alClient
      TabOrder = 27
      Text = '0.0'
      ExplicitHeight = 21
    end
    object zdRangeSceneEdit: TEdit
      Left = 107
      Top = 226
      Width = 106
      Height = 30
      Align = alClient
      TabOrder = 28
      Text = '1000.0'
      ExplicitHeight = 21
    end
    object zdOutSceneEdit: TEdit
      Left = 213
      Top = 226
      Width = 107
      Height = 30
      Align = alClient
      TabOrder = 29
      Text = '0.0'
      ExplicitHeight = 21
    end
  end
  object SceneCameraWidthEdit: TLabeledEdit
    Left = 8
    Top = 403
    Width = 121
    Height = 21
    EditLabel.Width = 138
    EditLabel.Height = 13
    EditLabel.Caption = 'Scene Camera Width (pixels)'
    TabOrder = 13
    Text = '640'
    OnChange = SceneCameraWidthEditChange
  end
  object SceneCameraHeightEdit: TLabeledEdit
    Left = 8
    Top = 453
    Width = 121
    Height = 21
    EditLabel.Width = 141
    EditLabel.Height = 13
    EditLabel.Caption = 'Scene Camera Height (pixels)'
    TabOrder = 14
    Text = '480'
    OnChange = SceneCameraHeightEditChange
  end
  object GazeProgressBar: TProgressBar
    Left = 8
    Top = 552
    Width = 145
    Height = 17
    DoubleBuffered = False
    ParentDoubleBuffered = False
    TabOrder = 15
  end
  object SceneProgressBar: TProgressBar
    Left = 8
    Top = 610
    Width = 145
    Height = 21
    TabOrder = 16
  end
  object PhasespaceProgressBar: TProgressBar
    Left = 8
    Top = 675
    Width = 145
    Height = 17
    TabOrder = 17
  end
  object HeadMarker0Edit: TLabeledEdit
    Left = 32
    Top = 804
    Width = 59
    Height = 21
    EditLabel.Width = 70
    EditLabel.Height = 13
    EditLabel.Caption = 'Head Marker 0'
    TabOrder = 18
    Text = '3'
  end
  object HeadMarker1Edit: TLabeledEdit
    Left = 123
    Top = 804
    Width = 54
    Height = 21
    EditLabel.Width = 70
    EditLabel.Height = 13
    EditLabel.Caption = 'Head Marker 1'
    TabOrder = 19
    Text = '4'
  end
  object HeadMarker2Edit: TLabeledEdit
    Left = 232
    Top = 804
    Width = 59
    Height = 21
    EditLabel.Width = 70
    EditLabel.Height = 13
    EditLabel.Caption = 'Head Marker 2'
    TabOrder = 20
    Text = '5'
  end
  object HeadMarker3Edit: TLabeledEdit
    Left = 311
    Top = 804
    Width = 58
    Height = 21
    EditLabel.Width = 70
    EditLabel.Height = 13
    EditLabel.Caption = 'Head Marker 3'
    TabOrder = 21
    Text = '6'
  end
  object ReferenceHeadButton: TButton
    Left = 408
    Top = 771
    Width = 137
    Height = 25
    Caption = 'Reference Head Position'
    TabOrder = 22
    OnClick = ReferenceHeadButtonClick
  end
  object WandNear0Edit: TLabeledEdit
    Left = 16
    Top = 849
    Width = 59
    Height = 21
    EditLabel.Width = 99
    EditLabel.Height = 13
    EditLabel.Caption = 'Near Wand Marker 0'
    TabOrder = 23
    Text = '0'
  end
  object WandNear1Edit: TLabeledEdit
    Left = 123
    Top = 849
    Width = 54
    Height = 21
    EditLabel.Width = 99
    EditLabel.Height = 13
    EditLabel.Caption = 'Near Wand Marker 1'
    TabOrder = 24
    Text = '1'
  end
  object WandFarEdit: TLabeledEdit
    Left = 232
    Top = 849
    Width = 59
    Height = 21
    EditLabel.Width = 83
    EditLabel.Height = 13
    EditLabel.Caption = 'Far Wand Marker'
    TabOrder = 25
    Text = '2'
  end
  object WandPosition0Button: TButton
    Left = 408
    Top = 805
    Width = 137
    Height = 25
    Caption = 'Wand Position 0'
    TabOrder = 26
    OnClick = WandPosition0ButtonClick
  end
  object WandPosition1Button: TButton
    Left = 408
    Top = 836
    Width = 137
    Height = 25
    Caption = 'Wand Position 1'
    TabOrder = 27
    OnClick = WandPosition1ButtonClick
  end
  object DisplayMarkerEdit: TLabeledEdit
    Left = 328
    Top = 849
    Width = 59
    Height = 21
    EditLabel.Width = 70
    EditLabel.Height = 13
    EditLabel.Caption = 'Display Marker'
    TabOrder = 28
    Text = '7'
  end
  object testEye: TButton
    Left = 931
    Top = 729
    Width = 137
    Height = 25
    Caption = 'testEye'
    TabOrder = 29
    OnClick = testEyeClick
  end
  object eyeTestDone: TButton
    Left = 931
    Top = 760
    Width = 137
    Height = 25
    Caption = 'eyeTestDone'
    TabOrder = 30
    OnClick = eyeTestDoneClick
  end
  object GazeComboBox: TComboBox
    AlignWithMargins = True
    Left = 8
    Top = 525
    Width = 137
    Height = 21
    TabOrder = 31
    Text = '<Select gaze stream>'
    OnChange = GazeComboBoxChange
  end
  object SceneComboBox: TComboBox
    AlignWithMargins = True
    Left = 8
    Top = 583
    Width = 137
    Height = 21
    TabOrder = 32
    Text = '<Select scene stream>'
    OnChange = SceneComboBoxChange
  end
  object PhaseComboBox: TComboBox
    AlignWithMargins = True
    Left = 8
    Top = 648
    Width = 137
    Height = 21
    TabOrder = 33
    Text = '<Select phase stream>'
    OnChange = PhaseComboBoxChange
  end
  object StatusMemo: TMemo
    Left = 711
    Top = 773
    Width = 162
    Height = 116
    Lines.Strings = (
      '')
    TabOrder = 34
  end
  object CalibrationWindowButton: TButton
    Left = 408
    Top = 867
    Width = 137
    Height = 25
    Caption = 'Calibration Window'
    TabOrder = 35
    OnClick = CalibrationWindowButtonClick
  end
  object DisplayULButton: TButton
    Left = 551
    Top = 771
    Width = 130
    Height = 25
    Caption = 'Display Upper Left'
    TabOrder = 36
    OnClick = DisplayULButtonClick
  end
  object DisplayURButton: TButton
    Left = 551
    Top = 805
    Width = 130
    Height = 25
    Caption = 'Display Upper Right'
    TabOrder = 37
    OnClick = DisplayURButtonClick
  end
  object DisplayBLButton: TButton
    Left = 551
    Top = 836
    Width = 130
    Height = 25
    Caption = 'Display Bottom Left'
    TabOrder = 38
    OnClick = DisplayBLButtonClick
  end
  object DisplayBRButton: TButton
    Left = 551
    Top = 867
    Width = 130
    Height = 25
    Caption = 'Display Bottom Right'
    TabOrder = 39
    OnClick = DisplayBRButtonClick
  end
  object xd: TXMLDocument
    Left = 1202
    Top = 731
    DOMVendorDesc = 'MSXML'
  end
  object Timer1: TTimer
    Interval = 10
    OnTimer = Timer1Timer
    Left = 1354
    Top = 723
  end
  object xdoc_out: TXMLDocument
    Left = 1128
    Top = 731
    DOMVendorDesc = 'MSXML'
  end
  object SaveDialog1: TSaveDialog
    DefaultExt = 'xml'
    Left = 1296
    Top = 723
  end
  object Timer2: TTimer
    OnTimer = Timer2Timer
    Left = 1248
    Top = 779
  end
  object OpenDialog1: TOpenDialog
    DefaultExt = 'csv'
    Left = 1152
    Top = 779
  end
  object SaveDialog2: TSaveDialog
    DefaultExt = 'csv'
    Left = 1296
    Top = 779
  end
  object Timer3: TTimer
    Enabled = False
    Interval = 10
    OnTimer = Timer3Timer
    Left = 808
    Top = 560
  end
  object OpenDialog2: TOpenDialog
    Left = 832
    Top = 664
  end
end
