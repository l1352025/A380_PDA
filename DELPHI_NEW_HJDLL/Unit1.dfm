object Form1: TForm1
  Left = 316
  Top = 158
  Width = 583
  Height = 409
  Caption = #21160#24577#24211#20363#23376
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label2: TLabel
    Left = 48
    Top = 160
    Width = 57
    Height = 13
    AutoSize = False
    Caption = #25484#26426#36335#24452':'
  end
  object Label3: TLabel
    Left = 312
    Top = 32
    Width = 73
    Height = 13
    AutoSize = False
    Caption = #31471#21475
  end
  object Label4: TLabel
    Left = 416
    Top = 32
    Width = 57
    Height = 13
    AutoSize = False
    Caption = #27874#29305#29575
  end
  object downfile: TButton
    Left = 24
    Top = 24
    Width = 75
    Height = 25
    Caption = #19979#36733
    TabOrder = 0
    OnClick = downfileClick
  end
  object UPFILE: TButton
    Left = 24
    Top = 72
    Width = 75
    Height = 25
    Caption = #19978#20256
    TabOrder = 1
    OnClick = UPFILEClick
  end
  object Button1: TButton
    Left = 208
    Top = 72
    Width = 75
    Height = 25
    Caption = #33719#21462#26426#22120#32534#21495
    TabOrder = 2
    OnClick = Button1Click
  end
  object setdatetime: TButton
    Left = 112
    Top = 24
    Width = 75
    Height = 25
    Caption = #35774#26102#38388
    TabOrder = 3
    OnClick = setdatetimeClick
  end
  object Gdir: TButton
    Left = 208
    Top = 24
    Width = 75
    Height = 25
    Caption = #33719#21462#25991#20214
    TabOrder = 4
    OnClick = GdirClick
  end
  object delfile: TButton
    Left = 112
    Top = 72
    Width = 73
    Height = 25
    Caption = #21024#38500#25991#20214
    TabOrder = 5
    OnClick = delfileClick
  end
  object Edit1: TEdit
    Left = 144
    Top = 123
    Width = 257
    Height = 21
    TabOrder = 6
  end
  object Edit2: TEdit
    Left = 144
    Top = 155
    Width = 257
    Height = 21
    TabOrder = 7
  end
  object ComboBox1: TComboBox
    Left = 312
    Top = 64
    Width = 81
    Height = 21
    ItemHeight = 13
    TabOrder = 8
    Text = 'USB'
    Items.Strings = (
      'USB'
      'COM1'
      'COM2'
      'COM3'
      'COM4')
  end
  object ComboBox2: TComboBox
    Left = 408
    Top = 64
    Width = 81
    Height = 21
    ItemHeight = 13
    TabOrder = 9
    Text = '115200'
    Items.Strings = (
      '115200'
      '57600')
  end
  object BitBtn1: TBitBtn
    Left = 24
    Top = 120
    Width = 105
    Height = 25
    Caption = 'PC'#25991#20214#36873#25321
    TabOrder = 10
    OnClick = BitBtn1Click
    Layout = blGlyphTop
  end
  object OpenDialog1: TOpenDialog
    Left = 120
    Top = 208
  end
end
