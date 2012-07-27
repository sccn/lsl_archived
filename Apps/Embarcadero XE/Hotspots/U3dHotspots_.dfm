object Form11: TForm11
  Left = 0
  Top = 0
  Caption = 'Hotspots'
  ClientHeight = 700
  ClientWidth = 513
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
  object LabeledEdit1: TLabeledEdit
    Left = 120
    Top = 12
    Width = 91
    Height = 12
    BorderStyle = bsNone
    Color = clBtnFace
    EditLabel.Width = 101
    EditLabel.Height = 12
    EditLabel.Caption = 'Reading 3D data from:'
    LabelPosition = lpLeft
    ReadOnly = True
    TabOrder = 0
  end
  object Button1: TButton
    Left = 17
    Top = 545
    Width = 79
    Height = 19
    Caption = 'Apply'
    TabOrder = 1
    OnClick = Button1Click
  end
  object Memo1: TMemo
    Left = 17
    Top = 569
    Width = 206
    Height = 97
    Lines.Strings = (
      '')
    TabOrder = 2
  end
  object PageControl1: TPageControl
    Left = 17
    Top = 36
    Width = 479
    Height = 493
    ActivePage = TabSheet1
    TabOrder = 3
    Visible = False
    object TabSheet1: TTabSheet
      Caption = 'Hotspots'
      object Button5: TButton
        Left = 443
        Top = 2
        Width = 27
        Height = 19
        Caption = 'Add'
        TabOrder = 0
        OnClick = Button5Click
      end
      object Panel1: TPanel
        Left = 0
        Top = 0
        Width = 425
        Height = 466
        Align = alLeft
        TabOrder = 1
        object GridPanel1: TGridPanel
          Left = 1
          Top = 1
          Width = 423
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
              Value = 80.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 80.000000000000000000
            end
            item
              SizeStyle = ssAbsolute
              Value = 80.000000000000000000
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
            423
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
            Left = 90
            Top = 5
            Width = 6
            Height = 12
            Anchors = []
            Caption = 'X'
            ExplicitLeft = 80
            ExplicitTop = 2
          end
          object Label6: TLabel
            Left = 114
            Top = 5
            Width = 6
            Height = 12
            Anchors = []
            Caption = 'Y'
            ExplicitLeft = 125
            ExplicitTop = 2
          end
          object Label7: TLabel
            Left = 138
            Top = 5
            Width = 6
            Height = 12
            Anchors = []
            Caption = 'Z'
            ExplicitLeft = 171
            ExplicitTop = 2
          end
          object Label8: TLabel
            Left = 161
            Top = 5
            Width = 10
            Height = 12
            Anchors = []
            Caption = 'Id'
            ExplicitLeft = 215
            ExplicitTop = 2
          end
          object Label9: TLabel
            Left = 217
            Top = 5
            Width = 3
            Height = 12
            Anchors = []
            Caption = ' '
            ExplicitLeft = 271
            ExplicitTop = 2
          end
          object Label10: TLabel
            Left = 297
            Top = 5
            Width = 3
            Height = 12
            Anchors = []
            Caption = ' '
            ExplicitLeft = 331
            ExplicitTop = 2
          end
          object Label1: TLabel
            Left = 377
            Top = 5
            Width = 3
            Height = 12
            Anchors = []
            Caption = ' '
            ExplicitLeft = 391
            ExplicitTop = 2
          end
        end
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Touch'
      ImageIndex = 1
      object GridPanel2: TGridPanel
        Left = 0
        Top = 0
        Width = 471
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
          471
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
    object TabSheet3: TTabSheet
      Caption = 'Point to'
      ImageIndex = 2
      object GridPanel3: TGridPanel
        Left = 0
        Top = 0
        Width = 471
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
          471
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
    object TabSheet4: TTabSheet
      Caption = 'Direction'
      ImageIndex = 3
      object GridPanel4: TGridPanel
        Left = 0
        Top = 0
        Width = 471
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
          471
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
          ExplicitLeft = 425
        end
      end
    end
  end
  object ProgressBar1: TProgressBar
    Left = 126
    Top = 545
    Width = 247
    Height = 16
    Smooth = True
    TabOrder = 4
  end
  object Memo2: TMemo
    Left = 227
    Top = 569
    Width = 146
    Height = 97
    TabOrder = 5
  end
  object Button2: TButton
    Left = 379
    Top = 624
    Width = 56
    Height = 19
    Caption = 'Save'
    TabOrder = 6
    OnClick = Button2Click
  end
  object Button3: TButton
    Left = 377
    Top = 647
    Width = 57
    Height = 19
    Caption = 'Load'
    TabOrder = 7
    OnClick = Button3Click
  end
  object Button4: TButton
    Left = 377
    Top = 545
    Width = 57
    Height = 19
    Caption = 'Fill'
    TabOrder = 8
    OnClick = Button4Click
  end
  object UpDown1: TUpDown
    Left = 307
    Top = 10
    Width = 15
    Height = 20
    Associate = Edit1
    Min = 1
    Position = 1
    TabOrder = 9
  end
  object Edit1: TEdit
    Left = 267
    Top = 10
    Width = 40
    Height = 20
    TabOrder = 10
    Text = '1'
  end
  object ProgressBar2: TProgressBar
    Left = 451
    Top = 552
    Width = 12
    Height = 103
    Max = 17000
    Orientation = pbVertical
    Smooth = True
    TabOrder = 11
  end
  object ProgressBar3: TProgressBar
    Left = 467
    Top = 552
    Width = 12
    Height = 103
    Max = 17000
    Orientation = pbVertical
    Smooth = True
    TabOrder = 12
  end
  object ProgressBar4: TProgressBar
    Left = 484
    Top = 552
    Width = 12
    Height = 103
    Max = 17000
    Orientation = pbVertical
    Smooth = True
    TabOrder = 13
  end
  object ComboBox1: TComboBox
    Left = 17
    Top = 10
    Width = 125
    Height = 20
    TabOrder = 14
    Text = '<Select input stream>'
    OnChange = ComboBox1Change
  end
  object Timer1: TTimer
    Interval = 100
    OnTimer = Timer1Timer
    Left = 504
    Top = 16
  end
  object SaveDialog1: TSaveDialog
    Left = 480
    Top = 696
  end
  object OpenDialog1: TOpenDialog
    Left = 512
    Top = 696
  end
end
