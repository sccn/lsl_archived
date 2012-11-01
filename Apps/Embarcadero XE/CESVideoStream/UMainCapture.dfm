object MainCaptureForm: TMainCaptureForm
  Left = 0
  Top = 0
  Caption = 'GazeStream'
  ClientHeight = 981
  ClientWidth = 1732
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -10
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 12
  object PageControl1: TPageControl
    Left = 6
    Top = 6
    Width = 427
    Height = 181
    ActivePage = TabSheet2
    TabOrder = 0
    TabPosition = tpBottom
    object TabSheet2: TTabSheet
      Caption = 'Video in'
      ImageIndex = 1
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
      object edtFrameRate: TLabeledEdit
        Left = 11
        Top = 130
        Width = 78
        Height = 20
        EditLabel.Width = 50
        EditLabel.Height = 12
        EditLabel.Caption = 'Frame Rate'
        TabOrder = 2
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
        TabOrder = 3
        OnChange = cbVideoInputChange
      end
    end
    object TabSheet1: TTabSheet
      Caption = 'Video out'
      ImageIndex = 2
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
      end
      object edFrame: TEdit
        Left = 107
        Top = 98
        Width = 80
        Height = 20
        TabOrder = 4
      end
      object cbRecordAudio: TCheckBox
        Left = 206
        Top = 78
        Width = 49
        Height = 12
        Alignment = taLeftJustify
        Caption = 'Audio'
        TabOrder = 5
        OnClick = cbRecordClick
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
        TabOrder = 6
        OnClick = cbRecordClick
      end
    end
  end
  object BitBtnPlay: TBitBtn
    Left = 8
    Top = 208
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
    TabOrder = 1
    OnClick = BitBtnPlayClick
  end
  object BitBtnStop: TBitBtn
    Left = 92
    Top = 208
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
    TabOrder = 2
    OnClick = btStopClick
  end
  object BacklogEdit: TLabeledEdit
    Left = 8
    Top = 296
    Width = 101
    Height = 20
    EditLabel.Width = 75
    EditLabel.Height = 12
    EditLabel.Caption = 'Frame Backlog 0'
    TabOrder = 3
  end
  object FrameDivisorEdit: TLabeledEdit
    Left = 8
    Top = 343
    Width = 101
    Height = 20
    EditLabel.Width = 97
    EditLabel.Height = 12
    EditLabel.Caption = 'Frame Display Divisor'
    TabOrder = 5
    Text = '5'
    OnChange = FrameDivisorEditChange
  end
  object droppedFramesEdit: TLabeledEdit
    Left = 8
    Top = 256
    Width = 101
    Height = 20
    EditLabel.Width = 102
    EditLabel.Height = 12
    EditLabel.Caption = 'Est. Dropped Frames 0'
    TabOrder = 4
  end
  object SpatialDivisorEdit: TLabeledEdit
    Left = 115
    Top = 343
    Width = 101
    Height = 20
    EditLabel.Width = 98
    EditLabel.Height = 12
    EditLabel.Caption = 'Spatial Display Divisor'
    TabOrder = 6
    Text = '1'
    OnChange = SpatialDivisorEditChange
  end
  object tPanel: TPanel
    Left = 440
    Top = 10
    Width = 640
    Height = 480
    BevelOuter = bvNone
    BorderStyle = bsSingle
    Color = clNone
    TabOrder = 7
  end
  object tPanel1: TPanel
    Left = 1080
    Top = 10
    Width = 640
    Height = 480
    BevelOuter = bvNone
    BorderStyle = bsSingle
    Color = clNone
    TabOrder = 8
  end
  object BacklogEdit1: TLabeledEdit
    Left = 115
    Top = 295
    Width = 101
    Height = 20
    EditLabel.Width = 75
    EditLabel.Height = 12
    EditLabel.Caption = 'Frame Backlog 1'
    TabOrder = 9
  end
  object droppedFramesEdit1: TLabeledEdit
    Left = 115
    Top = 256
    Width = 101
    Height = 20
    EditLabel.Width = 102
    EditLabel.Height = 12
    EditLabel.Caption = 'Est. Dropped Frames 1'
    TabOrder = 10
  end
  object droppedFramesEdit2: TLabeledEdit
    Left = 222
    Top = 256
    Width = 101
    Height = 20
    EditLabel.Width = 102
    EditLabel.Height = 12
    EditLabel.Caption = 'Est. Dropped Frames 2'
    TabOrder = 11
  end
  object droppedFramesEdit3: TLabeledEdit
    Left = 329
    Top = 256
    Width = 101
    Height = 20
    EditLabel.Width = 102
    EditLabel.Height = 12
    EditLabel.Caption = 'Est. Dropped Frames 3'
    TabOrder = 12
  end
  object BacklogEdit2: TLabeledEdit
    Left = 222
    Top = 292
    Width = 101
    Height = 20
    EditLabel.Width = 75
    EditLabel.Height = 12
    EditLabel.Caption = 'Frame Backlog 2'
    TabOrder = 13
  end
  object BacklogEdit3: TLabeledEdit
    Left = 329
    Top = 292
    Width = 101
    Height = 20
    EditLabel.Width = 75
    EditLabel.Height = 12
    EditLabel.Caption = 'Frame Backlog 3'
    TabOrder = 14
  end
  object tPanel2: TPanel
    Left = 440
    Top = 490
    Width = 640
    Height = 480
    BevelOuter = bvNone
    BorderStyle = bsSingle
    Color = clNone
    TabOrder = 15
  end
  object tPanel3: TPanel
    Left = 1080
    Top = 490
    Width = 640
    Height = 480
    BevelOuter = bvNone
    BorderStyle = bsSingle
    Color = clNone
    TabOrder = 16
  end
  object Timer1: TTimer
    Interval = 50
    OnTimer = Timer1Timer
    Left = 176
    Top = 728
  end
  object OpenDialog1: TOpenDialog
    Left = 136
    Top = 728
  end
  object xdoc_in: TXMLDocument
    Left = 200
    Top = 728
    DOMVendorDesc = 'MSXML'
  end
end
