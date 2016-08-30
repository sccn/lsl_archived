object Form3: TForm3
  Left = 0
  Top = 0
  Caption = 'Capture'
  ClientHeight = 81
  ClientWidth = 360
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -10
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 12
  object StringGrid1: TStringGrid
    Left = 12
    Top = 18
    Width = 141
    Height = 48
    ColCount = 3
    FixedCols = 0
    RowCount = 2
    ScrollBars = ssNone
    TabOrder = 0
    OnSelectCell = StringGrid1SelectCell
    ColWidths = (
      48
      46
      64)
  end
  object Button2: TButton
    Left = 230
    Top = 38
    Width = 56
    Height = 19
    Caption = 'Capture'
    TabOrder = 1
    OnClick = Button2Click
  end
  object ButtonDone: TButton
    Left = 292
    Top = 38
    Width = 56
    Height = 19
    Caption = 'Done'
    TabOrder = 2
    OnClick = ButtonDoneClick
  end
  object LabeledEdit2: TLabeledEdit
    Left = 173
    Top = 38
    Width = 33
    Height = 18
    BorderStyle = bsNone
    Color = clBtnFace
    EditLabel.Width = 41
    EditLabel.Height = 12
    EditLabel.Caption = 'Sensor #'
    ReadOnly = True
    TabOrder = 3
    Text = '0'
  end
  object UpDown1: TUpDown
    Left = 206
    Top = 38
    Width = 15
    Height = 18
    Associate = LabeledEdit2
    Max = 72
    TabOrder = 4
  end
  object LabeledEdit1: TLabeledEdit
    Left = 287
    Top = 16
    Width = 49
    Height = 20
    EditLabel.Width = 56
    EditLabel.Height = 12
    EditLabel.Caption = 'LabeledEdit1'
    TabOrder = 5
    Visible = False
    OnExit = LabeledEdit1Exit
  end
end
