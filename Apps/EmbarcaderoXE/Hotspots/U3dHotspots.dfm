object Form11: TForm11
  Left = 0
  Top = 0
  Caption = 'Hotspots'
  ClientHeight = 754
  ClientWidth = 933
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -10
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 12
  object PageControl1: TPageControl
    Left = 0
    Top = 49
    Width = 933
    Height = 531
    ActivePage = TabSheet5
    Align = alClient
    TabOrder = 0
    Visible = False
    object TabSheet1: TTabSheet
      Caption = 'Locations'
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object Button5: TButton
        Left = 611
        Top = 3
        Width = 27
        Height = 19
        Caption = 'Add'
        TabOrder = 0
        OnClick = Button5Click
      end
      object Panel1: TPanel
        Left = 0
        Top = 0
        Width = 585
        Height = 504
        Align = alLeft
        TabOrder = 1
        object ScrollBox1: TScrollBox
          Left = 1
          Top = 1
          Width = 584
          Height = 502
          Align = alLeft
          TabOrder = 0
          object GridPanel1: TGridPanel
            Left = 0
            Top = 0
            Width = 580
            Height = 23
            Align = alTop
            ColumnCollection = <
              item
                SizeStyle = ssAbsolute
                Value = 80.000000000000000000
              end
              item
                Value = 24.724847319524010000
              end
              item
                Value = 24.724847319524010000
              end
              item
                Value = 25.605458319945430000
              end
              item
                Value = 24.944847041006560000
              end
              item
                SizeStyle = ssAbsolute
                Value = 64.000000000000000000
              end
              item
                SizeStyle = ssAbsolute
                Value = 64.000000000000000000
              end
              item
                SizeStyle = ssAbsolute
                Value = 64.000000000000000000
              end
              item
                SizeStyle = ssAbsolute
                Value = 64.000000000000000000
              end
              item
                SizeStyle = ssAbsolute
                Value = 64.000000000000000000
              end>
            ControlCollection = <
              item
                Column = 0
                Control = Label4
                Row = 0
              end
              item
                Column = 1
                Control = Label5
                Row = 0
              end
              item
                Column = 2
                Control = Label6
                Row = 0
              end
              item
                Column = 3
                Control = Label7
                Row = 0
              end
              item
                Column = 4
                Control = Label8
                Row = 0
              end
              item
                Column = 5
                Control = Label9
                Row = 0
              end
              item
                Column = 6
                Control = Label10
                Row = 0
              end
              item
                Column = 7
                Control = Label1
                Row = 0
              end
              item
                Column = 8
                Control = Label26
                Row = 0
              end
              item
                Column = 9
                Control = Label45
                Row = 0
              end>
            RowCollection = <
              item
                SizeStyle = ssAbsolute
                Value = 20.000000000000000000
              end
              item
                SizeStyle = ssAbsolute
                Value = 20.000000000000000000
              end>
            TabOrder = 0
            OnClick = GridPanel1Click
            DesignSize = (
              580
              23)
            object Label4: TLabel
              Left = 39
              Top = 5
              Width = 3
              Height = 12
              Anchors = []
              Caption = ' '
              ExplicitLeft = 29
              ExplicitTop = 2
            end
            object Label5: TLabel
              Left = 100
              Top = 5
              Width = 6
              Height = 12
              Anchors = []
              Caption = 'X'
              ExplicitLeft = 92
              ExplicitTop = 2
            end
            object Label6: TLabel
              Left = 144
              Top = 5
              Width = 6
              Height = 12
              Anchors = []
              Caption = 'Y'
              ExplicitLeft = 161
              ExplicitTop = 2
            end
            object Label7: TLabel
              Left = 188
              Top = 5
              Width = 6
              Height = 12
              Anchors = []
              Caption = 'Z'
              ExplicitLeft = 232
              ExplicitTop = 2
            end
            object Label8: TLabel
              Left = 231
              Top = 5
              Width = 10
              Height = 12
              Anchors = []
              Caption = 'Id'
              ExplicitLeft = 301
              ExplicitTop = 2
            end
            object Label9: TLabel
              Left = 288
              Top = 5
              Width = 3
              Height = 12
              Anchors = []
              Caption = ' '
              ExplicitLeft = 368
              ExplicitTop = 2
            end
            object Label10: TLabel
              Left = 352
              Top = 5
              Width = 3
              Height = 12
              Anchors = []
              Caption = ' '
              ExplicitLeft = 428
              ExplicitTop = 2
            end
            object Label1: TLabel
              Left = 416
              Top = 5
              Width = 3
              Height = 12
              Anchors = []
              Caption = ' '
              ExplicitLeft = 488
              ExplicitTop = 2
            end
            object Label26: TLabel
              Left = 480
              Top = 5
              Width = 3
              Height = 12
              Anchors = []
              ExplicitLeft = 548
              ExplicitTop = 2
            end
            object Label45: TLabel
              Left = 544
              Top = 5
              Width = 3
              Height = 12
              Anchors = []
              Caption = ' '
              ExplicitLeft = 29
              ExplicitTop = 2
            end
          end
        end
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Touch'
      ImageIndex = 1
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object ScrollBox2: TScrollBox
        Left = 0
        Top = 0
        Width = 600
        Height = 504
        Align = alLeft
        TabOrder = 0
        object GridPanel2: TGridPanel
          Left = 0
          Top = 0
          Width = 596
          Height = 19
          Align = alTop
          ColumnCollection = <
            item
              SizeStyle = ssAbsolute
              Value = 100.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 100.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 100.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 100.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 100.000000000000000000
            end>
          ControlCollection = <
            item
              Column = 0
              Control = Label11
              Row = 0
            end
            item
              Column = 1
              Control = Label12
              Row = 0
            end
            item
              Column = 2
              Control = Label13
              Row = 0
            end
            item
              Column = 3
              Control = Label14
              Row = 0
            end
            item
              Column = 4
              Control = Label15
              Row = 0
            end>
          RowCollection = <
            item
              SizeStyle = ssAbsolute
              Value = 20.000000000000000000
            end>
          TabOrder = 0
          DesignSize = (
            596
            19)
          object Label11: TLabel
            Left = 49
            Top = 5
            Width = 3
            Height = 12
            Anchors = []
            Caption = ' '
            ExplicitLeft = 37
            ExplicitTop = 2
          end
          object Label12: TLabel
            Left = 130
            Top = 5
            Width = 42
            Height = 12
            Anchors = []
            Caption = 'Target Id'
            ExplicitLeft = 93
            ExplicitTop = 2
          end
          object Label13: TLabel
            Left = 242
            Top = 5
            Width = 17
            Height = 12
            Anchors = []
            Caption = 'Size'
            ExplicitLeft = 179
            ExplicitTop = 2
          end
          object Label14: TLabel
            Left = 335
            Top = 5
            Width = 31
            Height = 12
            Anchors = []
            Caption = 'Sensor'
            ExplicitLeft = 248
            ExplicitTop = 2
          end
          object Label15: TLabel
            Left = 431
            Top = 5
            Width = 40
            Height = 12
            Anchors = []
            Caption = ' Device#'
            ExplicitLeft = 320
            ExplicitTop = 2
          end
        end
      end
    end
    object TabSheet3: TTabSheet
      Caption = 'Point to'
      ImageIndex = 2
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object ScrollBox3: TScrollBox
        Left = 0
        Top = 0
        Width = 600
        Height = 504
        Align = alLeft
        TabOrder = 0
        object GridPanel3: TGridPanel
          Left = 0
          Top = 0
          Width = 596
          Height = 19
          Align = alTop
          ColumnCollection = <
            item
              SizeStyle = ssAbsolute
              Value = 100.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 100.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 100.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 50.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 50.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 100.000000000000000000
            end>
          ControlCollection = <
            item
              Column = 1
              Control = Label17
              Row = 0
            end
            item
              Column = 2
              Control = Label18
              Row = 0
            end
            item
              Column = 3
              Control = Label19
              Row = 0
            end
            item
              Column = 4
              Control = Label20
              Row = 0
            end
            item
              Column = 0
              Control = Label16
              Row = 0
            end
            item
              Column = -1
              Row = 0
            end
            item
              Column = 5
              Row = 1
            end
            item
              Column = 5
              Control = Label21
              Row = 0
            end>
          RowCollection = <
            item
              SizeStyle = ssAbsolute
              Value = 20.000000000000000000
            end>
          TabOrder = 0
          DesignSize = (
            596
            19)
          object Label17: TLabel
            Left = 130
            Top = 5
            Width = 42
            Height = 12
            Anchors = []
            Caption = 'Target Id'
            ExplicitLeft = 93
            ExplicitTop = 2
          end
          object Label18: TLabel
            Left = 230
            Top = 5
            Width = 42
            Height = 12
            Anchors = []
            Caption = 'Size(deg)'
            ExplicitLeft = 167
            ExplicitTop = 2
          end
          object Label19: TLabel
            Left = 314
            Top = 5
            Width = 24
            Height = 12
            Anchors = []
            Caption = 'From'
            ExplicitLeft = 233
            ExplicitTop = 2
          end
          object Label20: TLabel
            Left = 370
            Top = 5
            Width = 12
            Height = 12
            Anchors = []
            Caption = 'To'
            ExplicitLeft = 276
            ExplicitTop = 2
          end
          object Label16: TLabel
            Left = 49
            Top = 5
            Width = 3
            Height = 12
            Anchors = []
            Caption = ' '
            ExplicitLeft = 37
            ExplicitTop = 2
          end
          object Label21: TLabel
            Left = 432
            Top = 5
            Width = 37
            Height = 12
            Anchors = []
            Caption = 'Device#'
            ExplicitLeft = 321
            ExplicitTop = 2
          end
        end
      end
    end
    object TabSheet4: TTabSheet
      Caption = 'Direction'
      ImageIndex = 3
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object ScrollBox4: TScrollBox
        Left = 0
        Top = 0
        Width = 600
        Height = 504
        Align = alLeft
        TabOrder = 0
        object GridPanel4: TGridPanel
          Left = 0
          Top = 0
          Width = 596
          Height = 19
          Align = alTop
          ColumnCollection = <
            item
              SizeStyle = ssAbsolute
              Value = 100.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 100.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 100.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 50.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 50.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 100.000000000000000000
            end>
          ControlCollection = <
            item
              Column = 1
              Control = Label2
              Row = 0
            end
            item
              Column = 2
              Control = Label3
              Row = 0
            end
            item
              Column = 3
              Control = Label22
              Row = 0
            end
            item
              Column = 4
              Control = Label23
              Row = 0
            end
            item
              Column = 0
              Control = Label24
              Row = 0
            end
            item
              Column = -1
              Row = 0
            end
            item
              Column = 5
              Row = 1
            end
            item
              Column = 5
              Control = Label25
              Row = 0
            end>
          RowCollection = <
            item
              SizeStyle = ssAbsolute
              Value = 20.000000000000000000
            end>
          TabOrder = 0
          DesignSize = (
            596
            19)
          object Label2: TLabel
            Left = 130
            Top = 5
            Width = 42
            Height = 12
            Anchors = []
            Caption = 'Target Id'
            ExplicitLeft = 93
            ExplicitTop = 2
          end
          object Label3: TLabel
            Left = 230
            Top = 5
            Width = 42
            Height = 12
            Anchors = []
            Caption = 'Size(deg)'
            ExplicitLeft = 167
            ExplicitTop = 2
          end
          object Label22: TLabel
            Left = 314
            Top = 5
            Width = 24
            Height = 12
            Anchors = []
            Caption = 'From'
            ExplicitLeft = 233
            ExplicitTop = 2
          end
          object Label23: TLabel
            Left = 370
            Top = 5
            Width = 12
            Height = 12
            Anchors = []
            Caption = 'To'
            ExplicitLeft = 276
            ExplicitTop = 2
          end
          object Label24: TLabel
            Left = 49
            Top = 5
            Width = 3
            Height = 12
            Anchors = []
            Caption = ' '
            ExplicitLeft = 37
            ExplicitTop = 2
          end
          object Label25: TLabel
            Left = 423
            Top = 5
            Width = 55
            Height = 12
            Anchors = []
            Caption = 'Device#      '
            ExplicitLeft = 312
            ExplicitTop = 2
          end
        end
      end
    end
    object TabSheet5: TTabSheet
      Caption = 'Grid'
      ImageIndex = 4
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object ScrollBox5: TScrollBox
        Left = 0
        Top = 0
        Width = 600
        Height = 504
        Align = alLeft
        TabOrder = 0
        object GridPanel5: TGridPanel
          Left = 0
          Top = 0
          Width = 596
          Height = 25
          Align = alTop
          ColumnCollection = <
            item
              SizeStyle = ssAbsolute
              Value = 66.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 66.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 66.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 66.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 66.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 66.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 66.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 66.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 66.000000000000000000
            end>
          ControlCollection = <
            item
              Column = -1
              Row = -1
            end
            item
              Column = -1
              Row = -1
            end
            item
              Column = -1
              Row = -1
            end
            item
              Column = 0
              Control = Label27
              Row = 0
            end
            item
              Column = 1
              Control = Label30
              Row = 0
            end
            item
              Column = 2
              Control = Label28
              Row = 0
            end
            item
              Column = 3
              Control = Label29
              Row = 0
            end
            item
              Column = 4
              Control = Label33
              Row = 0
            end
            item
              Column = 5
              Control = Label35
              Row = 0
            end
            item
              Column = 6
              Control = Label31
              Row = 0
            end
            item
              Column = 7
              Control = Label32
              Row = 0
            end
            item
              Column = 8
              Control = device
              Row = 0
            end>
          RowCollection = <
            item
              SizeStyle = ssAbsolute
              Value = 20.000000000000000000
            end>
          TabOrder = 0
          DesignSize = (
            596
            25)
          object Label27: TLabel
            Left = 32
            Top = 5
            Width = 3
            Height = 12
            Anchors = []
            ExplicitLeft = 24
            ExplicitTop = 2
          end
          object Label30: TLabel
            Left = 97
            Top = 5
            Width = 6
            Height = 12
            Anchors = []
            Caption = 'X'
            Visible = False
            ExplicitLeft = 80
          end
          object Label28: TLabel
            Left = 163
            Top = 5
            Width = 6
            Height = 12
            Anchors = []
            Caption = 'Y'
            Visible = False
            ExplicitLeft = 133
          end
          object Label29: TLabel
            Left = 229
            Top = 5
            Width = 6
            Height = 12
            Anchors = []
            Caption = 'Z'
            Visible = False
            ExplicitLeft = 171
            ExplicitTop = 2
          end
          object Label33: TLabel
            Left = 282
            Top = 5
            Width = 31
            Height = 12
            Anchors = []
            Caption = 'Delta X'
            Visible = False
            ExplicitLeft = 208
            ExplicitTop = 2
          end
          object Label35: TLabel
            Left = 350
            Top = 5
            Width = 28
            Height = 12
            Anchors = []
            Caption = 'DeltaY'
            Visible = False
            ExplicitLeft = 260
            ExplicitTop = 2
          end
          object Label31: TLabel
            Left = 414
            Top = 5
            Width = 31
            Height = 12
            Anchors = []
            Caption = 'Delta Z'
            Visible = False
            ExplicitLeft = 308
            ExplicitTop = 2
          end
          object Label32: TLabel
            Left = 480
            Top = 5
            Width = 31
            Height = 12
            Anchors = []
            Caption = 'Sensor'
            Visible = False
            ExplicitLeft = 357
            ExplicitTop = 2
          end
          object device: TLabel
            Left = 536
            Top = 5
            Width = 52
            Height = 12
            Anchors = []
            Caption = 'Device #    '
            Visible = False
            ExplicitLeft = 397
            ExplicitTop = 2
          end
        end
      end
    end
    object Screen: TTabSheet
      Caption = 'Screen'
      ImageIndex = 5
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object ScrollBox6: TScrollBox
        Left = 0
        Top = 0
        Width = 600
        Height = 504
        Align = alLeft
        TabOrder = 0
        object GridPanel6: TGridPanel
          Left = 0
          Top = 0
          Width = 596
          Height = 19
          Align = alTop
          ColumnCollection = <
            item
              SizeStyle = ssAbsolute
              Value = 74.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 74.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 74.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 74.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 66.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 74.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 74.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 74.000000000000000000
            end>
          ControlCollection = <
            item
              Column = -1
              Row = -1
            end
            item
              Column = -1
              Row = -1
            end
            item
              Column = -1
              Row = -1
            end
            item
              Column = 0
              Control = Label34
              Row = 0
            end
            item
              Column = 1
              Control = Label38
              Row = 0
            end
            item
              Column = 2
              Control = Label39
              Row = 0
            end
            item
              Column = 3
              Control = Label40
              Row = 0
            end
            item
              Column = 4
              Control = Label41
              Row = 0
            end
            item
              Column = 5
              Control = Label42
              Row = 0
            end
            item
              Column = 6
              Control = Label43
              Row = 0
            end
            item
              Column = 7
              Control = Label44
              Row = 0
            end>
          RowCollection = <
            item
              SizeStyle = ssAbsolute
              Value = 20.000000000000000000
            end>
          TabOrder = 0
          DesignSize = (
            596
            19)
          object Label34: TLabel
            Left = 36
            Top = 5
            Width = 3
            Height = 12
            Anchors = []
            ExplicitLeft = 24
            ExplicitTop = 2
          end
          object Label38: TLabel
            Left = 93
            Top = 5
            Width = 37
            Height = 12
            Anchors = []
            Caption = 'Top Left'
            ExplicitLeft = 83
          end
          object Label39: TLabel
            Left = 164
            Top = 5
            Width = 44
            Height = 12
            Anchors = []
            Caption = 'Top Right'
            ExplicitLeft = 145
          end
          object Label40: TLabel
            Left = 234
            Top = 5
            Width = 51
            Height = 12
            Anchors = []
            Caption = 'Bottom Left'
            ExplicitLeft = 208
          end
          object Label41: TLabel
            Left = 301
            Top = 5
            Width = 58
            Height = 12
            Anchors = []
            Caption = 'Bottom Right'
            ExplicitLeft = 272
          end
          object Label42: TLabel
            Left = 388
            Top = 5
            Width = 24
            Height = 12
            Anchors = []
            Caption = 'From'
            ExplicitLeft = 392
          end
          object Label43: TLabel
            Left = 468
            Top = 5
            Width = 12
            Height = 12
            Anchors = []
            Caption = 'To'
            ExplicitLeft = 471
          end
          object Label44: TLabel
            Left = 528
            Top = 5
            Width = 40
            Height = 12
            Anchors = []
            Caption = 'Device #'
            ExplicitLeft = 479
          end
        end
      end
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 580
    Width = 933
    Height = 174
    Align = alBottom
    TabOrder = 1
    object Button4: TButton
      Left = 720
      Top = 89
      Width = 57
      Height = 19
      Caption = 'Fill'
      TabOrder = 0
      OnClick = Button4Click
    end
    object Memo1: TMemo
      Left = 13
      Top = 8
      Width = 206
      Height = 97
      Lines.Strings = (
        '')
      TabOrder = 1
    end
    object Memo2: TMemo
      Left = 223
      Top = 8
      Width = 491
      Height = 97
      TabOrder = 2
    end
    object LoadLoc: TButton
      Left = 447
      Top = 150
      Width = 66
      Height = 19
      Caption = 'Load Loc'
      TabOrder = 3
      OnClick = LoadLocClick
    end
    object SaveLoc: TButton
      Left = 348
      Top = 150
      Width = 69
      Height = 19
      Caption = 'Save Loc'
      TabOrder = 4
      OnClick = SaveLocClick
    end
    object SaveConfigButton: TButton
      Left = 348
      Top = 125
      Width = 69
      Height = 19
      Caption = 'Save Config'
      TabOrder = 5
      OnClick = SaveConfigButtonClick
    end
    object LoadConfigButton: TButton
      Left = 447
      Top = 126
      Width = 66
      Height = 18
      Caption = 'Load Config'
      TabOrder = 6
      OnClick = LoadConfigButtonClick
    end
    object TrashPanel: TPanel
      Left = 714
      Top = 122
      Width = 58
      Height = 47
      Caption = 'TrashPanel'
      TabOrder = 7
      Visible = False
    end
    object GazeProgressBar: TProgressBar
      Left = 17
      Top = 143
      Width = 247
      Height = 16
      Smooth = True
      TabOrder = 8
    end
    object OpenDisplaysButton: TButton
      Left = 551
      Top = 125
      Width = 82
      Height = 19
      Caption = 'Open Displays'
      TabOrder = 9
      OnClick = OpenDisplaysButtonClick
    end
    object CloseDisplaysButton: TButton
      Left = 551
      Top = 150
      Width = 82
      Height = 19
      Caption = 'Close Displays'
      TabOrder = 10
      OnClick = CloseDisplaysButtonClick
    end
    object ProgressBar1: TProgressBar
      Left = 17
      Top = 120
      Width = 248
      Height = 17
      Max = 1000
      TabOrder = 11
    end
  end
  object Panel3x: TPanel
    Left = 0
    Top = 0
    Width = 933
    Height = 49
    Align = alTop
    Alignment = taLeftJustify
    TabOrder = 2
    object Label37: TLabel
      Left = 492
      Top = -3
      Width = 105
      Height = 12
      Caption = 'Reading eye data from:'
    end
    object Label36: TLabel
      Left = 138
      Top = 1
      Width = 140
      Height = 12
      Caption = 'Reading phasespace data from:'
    end
    object GazeComboBox: TComboBox
      Left = 486
      Top = 15
      Width = 144
      Height = 20
      TabOrder = 0
      Text = '<Select gaze stream>'
      OnChange = GazeComboBoxChange
    end
    object LoadCalibration: TButton
      Left = 654
      Top = 13
      Width = 84
      Height = 25
      Caption = 'Load Calibration'
      TabOrder = 1
      OnClick = LoadCalibrationClick
    end
    object PhaseComboBox: TComboBox
      Left = 138
      Top = 19
      Width = 144
      Height = 20
      TabOrder = 2
      Text = '<Select phasespace stream>'
      OnChange = PhaseComboBoxChange
    end
    object xOffsetEdit: TLabeledEdit
      Left = 766
      Top = 15
      Width = 66
      Height = 20
      EditLabel.Width = 62
      EditLabel.Height = 12
      EditLabel.Caption = 'X Offset (mm)'
      TabOrder = 3
      Text = '0'
      OnChange = xOffsetEditChange
    end
    object yOffsetEdit: TLabeledEdit
      Left = 855
      Top = 15
      Width = 74
      Height = 20
      EditLabel.Width = 62
      EditLabel.Height = 12
      EditLabel.Caption = 'Z Offset (mm)'
      TabOrder = 4
      Text = '0'
      OnChange = yOffsetEditChange
    end
  end
  object RefreshStreamsButton: TButton
    Left = 34
    Top = 13
    Width = 87
    Height = 25
    Caption = 'Refresh Streams'
    TabOrder = 3
    OnClick = RefreshStreamsButtonClick
  end
  object SaveDialog1: TSaveDialog
    DefaultExt = 'hs'
    Left = 712
    Top = 712
  end
  object OpenDialog1: TOpenDialog
    DefaultExt = 'hs'
    Left = 688
    Top = 704
  end
  object xdoc_out: TXMLDocument
    Active = True
    Left = 656
    Top = 712
    DOMVendorDesc = 'MSXML'
  end
  object SaveDialog2: TSaveDialog
    DefaultExt = 'xml'
    Left = 712
    Top = 704
  end
  object OpenDialog2: TOpenDialog
    DefaultExt = 'xml'
    Left = 696
    Top = 712
  end
  object GazeOpenDialog: TOpenDialog
    Left = 672
    Top = 704
  end
  object xdoc_in: TXMLDocument
    Left = 608
    Top = 152
    DOMVendorDesc = 'MSXML'
  end
end
