object Form11: TForm11
  Left = 0
  Top = 0
  Caption = 'Form11'
  ClientHeight = 487
  ClientWidth = 587
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 120
  TextHeight = 16
  object LabeledEdit1: TLabeledEdit
    Left = 160
    Top = 16
    Width = 121
    Height = 16
    BorderStyle = bsNone
    Color = clBtnFace
    EditLabel.Width = 130
    EditLabel.Height = 16
    EditLabel.Caption = 'Reading 3D data from:'
    LabelPosition = lpLeft
    ReadOnly = True
    TabOrder = 0
  end
  object Button1: TButton
    Left = 23
    Top = 311
    Width = 105
    Height = 25
    Caption = 'Apply'
    TabOrder = 1
    OnClick = Button1Click
  end
  object Memo1: TMemo
    Left = 23
    Top = 342
    Width = 536
    Height = 130
    Lines.Strings = (
      '')
    TabOrder = 2
  end
  object PageControl1: TPageControl
    Left = 23
    Top = 48
    Width = 536
    Height = 257
    ActivePage = TabSheet3
    TabOrder = 3
    object TabSheet1: TTabSheet
      Caption = 'Hotspots'
      object GridPanel1: TGridPanel
        Left = 3
        Top = 3
        Width = 475
        Height = 30
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
          475
          30)
        object Label4: TLabel
          Left = 39
          Top = 3
          Width = 4
          Height = 16
          Anchors = []
          Caption = ' '
          ExplicitLeft = 1
          ExplicitTop = 1
        end
        object Label5: TLabel
          Left = 105
          Top = 3
          Width = 8
          Height = 16
          Anchors = []
          Caption = 'X'
          ExplicitLeft = 12
          ExplicitTop = 1
        end
        object Label6: TLabel
          Left = 163
          Top = 3
          Width = 7
          Height = 16
          Anchors = []
          Caption = 'Y'
          ExplicitLeft = 20
          ExplicitTop = 1
        end
        object Label7: TLabel
          Left = 221
          Top = 3
          Width = 7
          Height = 16
          Anchors = []
          Caption = 'Z'
          ExplicitLeft = 27
          ExplicitTop = 1
        end
        object Label8: TLabel
          Left = 277
          Top = 3
          Width = 11
          Height = 16
          Anchors = []
          Caption = 'Id'
          ExplicitLeft = 43
          ExplicitTop = 1
        end
        object Label9: TLabel
          Left = 350
          Top = 3
          Width = 4
          Height = 16
          Anchors = []
          Caption = ' '
          ExplicitLeft = 333
          ExplicitTop = 1
        end
        object Label10: TLabel
          Left = 430
          Top = 3
          Width = 4
          Height = 16
          Anchors = []
          Caption = ' '
          ExplicitLeft = 410
          ExplicitTop = 1
        end
      end
      object Button5: TButton
        Left = 484
        Top = 3
        Width = 37
        Height = 25
        Caption = 'Add'
        TabOrder = 1
        OnClick = Button5Click
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Touch'
      ImageIndex = 1
      object GridPanel2: TGridPanel
        Left = 4
        Top = 3
        Width = 501
        Height = 25
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
          end
          item
            SizeStyle = ssAbsolute
            Value = 20.000000000000000000
          end>
        TabOrder = 0
        DesignSize = (
          501
          25)
        object Label11: TLabel
          Left = 49
          Top = 3
          Width = 4
          Height = 16
          Anchors = []
          Caption = ' '
          ExplicitTop = 1
        end
        object Label12: TLabel
          Left = 124
          Top = 3
          Width = 53
          Height = 16
          Anchors = []
          Caption = 'Target Id'
          ExplicitLeft = 128
          ExplicitTop = 1
        end
        object Label13: TLabel
          Left = 239
          Top = 3
          Width = 24
          Height = 16
          Anchors = []
          Caption = 'Size'
          ExplicitLeft = 240
        end
        object Label14: TLabel
          Left = 331
          Top = 3
          Width = 40
          Height = 16
          Anchors = []
          Caption = 'Sensor'
          ExplicitTop = 1
        end
        object Label15: TLabel
          Left = 426
          Top = 3
          Width = 50
          Height = 16
          Anchors = []
          Caption = ' Device#'
          ExplicitLeft = 430
          ExplicitTop = 1
        end
      end
    end
    object TabSheet3: TTabSheet
      Caption = 'Point to'
      ImageIndex = 2
      object GridPanel3: TGridPanel
        Left = 3
        Top = 3
        Width = 502
        Height = 25
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
          end
          item
            SizeStyle = ssAbsolute
            Value = 20.000000000000000000
          end>
        TabOrder = 0
        DesignSize = (
          502
          25)
        object Label17: TLabel
          Left = 124
          Top = 3
          Width = 53
          Height = 16
          Anchors = []
          Caption = 'Target Id'
          ExplicitLeft = 104
          ExplicitTop = 1
        end
        object Label18: TLabel
          Left = 223
          Top = 3
          Width = 55
          Height = 16
          Anchors = []
          Caption = 'Size(deg)'
          ExplicitLeft = 218
        end
        object Label19: TLabel
          Left = 311
          Top = 3
          Width = 30
          Height = 16
          Anchors = []
          Caption = 'From'
          ExplicitLeft = 285
          ExplicitTop = 1
        end
        object Label20: TLabel
          Left = 368
          Top = 3
          Width = 15
          Height = 16
          Anchors = []
          Caption = 'To'
          ExplicitLeft = 376
        end
        object Label16: TLabel
          Left = 49
          Top = 3
          Width = 4
          Height = 16
          Anchors = []
          Caption = ' '
          ExplicitLeft = 14
          ExplicitTop = 1
        end
        object Label21: TLabel
          Left = 428
          Top = 3
          Width = 46
          Height = 16
          Anchors = []
          Caption = 'Device#'
          ExplicitLeft = 432
        end
      end
    end
  end
  object Timer1: TTimer
    Interval = 100
    OnTimer = Timer1Timer
    Left = 504
    Top = 16
  end
end
