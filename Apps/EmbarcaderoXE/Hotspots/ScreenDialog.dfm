object ScreenForm: TScreenForm
  Left = 0
  Top = 0
  Caption = 'Form5'
  ClientHeight = 186
  ClientWidth = 547
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object TopLeftEdit: TLabeledEdit
    Left = 24
    Top = 24
    Width = 121
    Height = 21
    EditLabel.Width = 54
    EditLabel.Height = 13
    EditLabel.Caption = 'Top Left ID'
    TabOrder = 0
  end
  object TopRightEdit: TLabeledEdit
    Left = 24
    Top = 64
    Width = 121
    Height = 21
    EditLabel.Width = 60
    EditLabel.Height = 13
    EditLabel.Caption = 'Top Right ID'
    TabOrder = 1
  end
  object BottomLeftEdit: TLabeledEdit
    Left = 24
    Top = 105
    Width = 121
    Height = 21
    EditLabel.Width = 70
    EditLabel.Height = 13
    EditLabel.Caption = 'Bottom Left ID'
    TabOrder = 2
  end
  object BottomRightEdit: TLabeledEdit
    Left = 24
    Top = 145
    Width = 121
    Height = 21
    EditLabel.Width = 76
    EditLabel.Height = 13
    EditLabel.Caption = 'Bottom Right ID'
    TabOrder = 3
  end
  object DoneButton: TButton
    Left = 446
    Top = 143
    Width = 75
    Height = 25
    Caption = 'Done'
    TabOrder = 4
    OnClick = DoneButtonClick
  end
  object Sensor0Edit: TLabeledEdit
    Left = 160
    Top = 24
    Width = 121
    Height = 21
    EditLabel.Width = 60
    EditLabel.Height = 13
    EditLabel.Caption = 'From Sensor'
    TabOrder = 5
  end
  object Sensor1Edit: TLabeledEdit
    Left = 160
    Top = 64
    Width = 121
    Height = 21
    EditLabel.Width = 48
    EditLabel.Height = 13
    EditLabel.Caption = 'To Sensor'
    TabOrder = 6
  end
  object DeviceEdit: TLabeledEdit
    Left = 160
    Top = 105
    Width = 121
    Height = 21
    EditLabel.Width = 32
    EditLabel.Height = 13
    EditLabel.Caption = 'Device'
    TabOrder = 7
  end
  object XEdit: TLabeledEdit
    Left = 305
    Top = 24
    Width = 121
    Height = 21
    EditLabel.Width = 44
    EditLabel.Height = 13
    EditLabel.Caption = 'X (pixels)'
    TabOrder = 8
  end
  object WidthEdit: TLabeledEdit
    Left = 305
    Top = 105
    Width = 121
    Height = 21
    EditLabel.Width = 66
    EditLabel.Height = 13
    EditLabel.Caption = 'Width (pixels)'
    TabOrder = 9
  end
  object YEdit: TLabeledEdit
    Left = 305
    Top = 65
    Width = 121
    Height = 21
    EditLabel.Width = 44
    EditLabel.Height = 13
    EditLabel.Caption = 'Y (pixels)'
    TabOrder = 10
  end
  object HeightEdit: TLabeledEdit
    Left = 305
    Top = 145
    Width = 121
    Height = 21
    EditLabel.Width = 69
    EditLabel.Height = 13
    EditLabel.Caption = 'Height (pixels)'
    TabOrder = 11
  end
  object MonitorDepthEdit: TLabeledEdit
    Left = 160
    Top = 147
    Width = 121
    Height = 21
    EditLabel.Width = 95
    EditLabel.Height = 13
    EditLabel.Caption = 'Monitor Depth (mm)'
    TabOrder = 12
  end
end
