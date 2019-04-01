unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Buttons;

type
  TForm1 = class(TForm)
    downfile: TButton;
    UPFILE: TButton;
    Button1: TButton;
    setdatetime: TButton;
    Gdir: TButton;
    delfile: TButton;
    Edit1: TEdit;
    Edit2: TEdit;
    Label2: TLabel;
    Label3: TLabel;
    ComboBox1: TComboBox;
    Label4: TLabel;
    ComboBox2: TComboBox;
    OpenDialog1: TOpenDialog;
    BitBtn1: TBitBtn;
    procedure downfileClick(Sender: TObject);
    procedure UPFILEClick(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure setdatetimeClick(Sender: TObject);
    procedure GdirClick(Sender: TObject);
    procedure delfileClick(Sender: TObject);
    procedure BitBtn1Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;
  function   SendFileToHc(PORT:string;BAUDRATEP:string; DATAFILE: string; APPFILE: string; PROMPT:string ): Integer;  stdcall;
  function   SendFileToPc(PORT:string;BAUDRATEP:string; DATAFILE: string; FILESAVEAS: string; PROMPT:string) : Integer;  stdcall;
  function   GetHcNum(   PORT:string; BAUDRATE:string;HCNUM:pchar): Integer;  stdcall;
  function   SetDateTimeEx(PORT:string; BAUDRATE:string;DATE1 :string;TIME1:string): Integer;  stdcall;
  function   GetHcDir(PORT:string; BAUDRATE:string;path:string;dir:pchar): Integer;  stdcall;
  function   DelHcFile (PORT:string; BAUDRATE:string;PDADIR:string) : Integer;  stdcall;
implementation
  function SendFileToHc; external 'hjdll.dll' name 'SendFileToHc';
  function SendFileToPc; external 'hjdll.dll' name 'SendFileToPc';
  function GetHcNum; external 'hjdll.dll' name 'GetHcNum';
  function SetDateTimeEx; external 'hjdll.dll' name 'SetDateTime';
   function GetHcDir; external 'hjdll.dll' name 'GetHcDir';
  function   DelHcFile  ; external 'hjdll.dll' name 'DelHcFile';

{$R *.dfm}

procedure TForm1.downfileClick(Sender: TObject);
var
  i : integer;
  port,btl:   string;
begin
   case  Combobox1.ItemIndex of
     -1:port:='0';
     0: port:='0';
     1: port:='1';
     2: port:='2';
     3: port:='3';
     4: port:='4';

     end;
   case  Combobox2.ItemIndex of
    -1: btl:='05';
     0: btl:='05';
     1: btl:='04';

     end;


  i:=SendFileToHc(port, btl,edit1.Text , '','1');
  ShowMessage(IntToStr(i));

end;

procedure TForm1.UPFILEClick(Sender: TObject);
var
 i : integer;
 port,btl:   string;
begin
   case  Combobox1.ItemIndex of
    -1:port:='0';
     0: port:='0';
     1: port:='1';
     2: port:='2';
     3: port:='3';
     4: port:='4';

     end;
   case  Combobox2.ItemIndex of
    -1: btl:='05';
     0: btl:='05';
     1: btl:='04';

     end;
 i:=SendFileToPc(port, btl, edit2.Text, edit1.Text, '1');
 ShowMessage(IntToStr(i));
end;

procedure TForm1.Button1Click(Sender: TObject);
var
  i : integer;
  Info1: pchar;    //机器编号     长度  8
 port,btl:   string;
begin
   case  Combobox1.ItemIndex of
    -1:port:='0';
     0: port:='0';
     1: port:='1';
     2: port:='2';
     3: port:='3';
     4: port:='4';

     end;
   case  Combobox2.ItemIndex of
    -1: btl:='05';
     0: btl:='05';
     1: btl:='04';

     end;
     getmem(Info1,256);
     i:=GetHcNum(port, btl,Info1);
     ShowMessage(Info1);
     ShowMessage(IntToStr(i));
end;

procedure TForm1.setdatetimeClick(Sender: TObject);
var
  i : integer;
   port,btl:   string;
begin
   case  Combobox1.ItemIndex of
    -1:port:='0';
     0: port:='0';
     1: port:='1';
     2: port:='2';
     3: port:='3';
     4: port:='4';

     end;
   case  Combobox2.ItemIndex of
    -1: btl:='05';
     0: btl:='05';
     1: btl:='04';

     end;
    // i:=SetDateTime('0', '05', '2008-01-01', '08:18:18');
    i:=SetDateTimeEx(port, btl,'2008-01-01','08:08:08');
    ShowMessage(IntToStr(i));
end;

procedure TForm1.GdirClick(Sender: TObject);
var
  i : integer;
  Info1: pchar;
   port,btl:   string;
begin
   case  Combobox1.ItemIndex of
    -1:port:='0';
     0: port:='0';
     1: port:='1';
     2: port:='2';
     3: port:='3';
     4: port:='4';

     end;
   case  Combobox2.ItemIndex of
    -1: btl:='05';
     0: btl:='05';
     1: btl:='04';

     end;
{
	char cName[256]; 文件名，目录名
	char cMtime[20]; 修改时间
	char cCtime[20]; 创建时间
	char cSize[20]; 文件大小
	int nType;//F 文件 ，其他目录
	DWORD iSize; 文件大小
	DWORD iMtime; 修改时间
	DWORD iCtime;创建时间
        FILE_DIR
}

     getmem(Info1,2048*20);
     i:=GetHcDir(port, btl,'/',Info1);
     ShowMessage(Info1);//需要分解
     ShowMessage(IntToStr(i));

end;

procedure TForm1.delfileClick(Sender: TObject);
var
  i : integer;

   port,btl:   string;
begin
   case  Combobox1.ItemIndex of
    -1:port:='0';
     0: port:='0';
     1: port:='1';
     2: port:='2';
     3: port:='3';
     4: port:='4';

     end;
   case  Combobox2.ItemIndex of
    -1: btl:='05';
     0: btl:='05';
     1: btl:='04';

     end;


     i:=DelHcFile(port, btl,'/cbsjk.dbf' );
     ShowMessage(IntToStr(i));

end;

procedure TForm1.BitBtn1Click(Sender: TObject);
begin
if OpenDialog1.Execute then
  begin
   edit1.Text:=OpenDialog1.FileName; 
  end;
end;

end.
