object GridForm: TGridForm
  Left = 0
  Top = 0
  Caption = 'Grid Parameters'
  ClientHeight = 134
  ClientWidth = 634
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Location1Edit: TLabeledEdit
    Left = 32
    Top = 24
    Width = 121
    Height = 21
    EditLabel.Width = 20
    EditLabel.Height = 13
    EditLabel.Caption = 'ID 1'
    TabOrder = 0
  end
  object Location2Edit: TLabeledEdit
    Left = 32
    Top = 64
    Width = 121
    Height = 21
    EditLabel.Width = 20
    EditLabel.Height = 13
    EditLabel.Caption = 'ID 2'
    TabOrder = 1
  end
  object ColumnsEdit: TLabeledEdit
    Left = 176
    Top = 24
    Width = 121
    Height = 21
    EditLabel.Width = 93
    EditLabel.Height = 13
    EditLabel.Caption = 'Number of Columns'
    TabOrder = 2
  end
  object RowsEdit: TLabeledEdit
    Left = 176
    Top = 64
    Width = 121
    Height = 21
    EditLabel.Width = 79
    EditLabel.Height = 13
    EditLabel.Caption = 'Number of Rows'
    TabOrder = 3
  end
  object HeightThicknessEdit: TLabeledEdit
    Left = 320
    Top = 105
    Width = 121
    Height = 21
    EditLabel.Width = 80
    EditLabel.Height = 13
    EditLabel.Caption = 'Height Thickness'
    TabOrder = 4
  end
  object DoneButton: TButton
    Left = 488
    Top = 91
    Width = 75
    Height = 25
    Caption = 'Done'
    TabOrder = 5
    OnClick = DoneButtonClick
  end
  object SensorEdit: TLabeledEdit
    Left = 464
    Top = 24
    Width = 121
    Height = 21
    EditLabel.Width = 33
    EditLabel.Height = 13
    EditLabel.Caption = 'Sensor'
    TabOrder = 6
  end
  object DeviceEdit: TLabeledEdit
    Left = 464
    Top = 64
    Width = 121
    Height = 21
    EditLabel.Width = 67
    EditLabel.Height = 13
    EditLabel.Caption = 'First Device #'
    TabOrder = 7
  end
  object ColumnThicknessEdit: TLabeledEdit
    Left = 320
    Top = 24
    Width = 121
    Height = 21
    EditLabel.Width = 129
    EditLabel.Height = 13
    EditLabel.Caption = 'Maximum Column thickness'
    TabOrder = 8
  end
  object RowThicknessEdit: TLabeledEdit
    Left = 320
    Top = 64
    Width = 121
    Height = 21
    EditLabel.Width = 115
    EditLabel.Height = 13
    EditLabel.Caption = 'Maximum Row thickness'
    TabOrder = 9
  end
  object Location3Edit: TLabeledEdit
    Left = 32
    Top = 105
    Width = 121
    Height = 21
    EditLabel.Width = 20
    EditLabel.Height = 13
    EditLabel.Caption = 'ID 3'
    TabOrder = 10
  end
end
