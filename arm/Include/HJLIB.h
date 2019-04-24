#ifndef _HJLIB_H
#define _HJLIB_H

typedef unsigned char  uint8;                
typedef signed   char  int8;                     
typedef unsigned short uint16;                  
typedef signed   short int16;                   /* defined for signed 16-bits integer variable 		有符号16位整型变量 */
typedef unsigned int   uint32;                  /* defined for unsigned 32-bits integer variable 	无符号32位整型变量 */
typedef signed   int   int32;                   /* defined for signed 32-bits integer variable 		有符号32位整型变量 */
typedef float          fp32;                    /* single precision floating point variable (32bits) 单精度浮点数（32位长度） */
typedef double         fp64;                    /* double precision floating point variable (64bits) 双精度浮点数（64位长度） */
typedef unsigned char  UINT8;                   /* defined for unsigned 8-bits integer variable 	无符号8位整型变量  */
typedef signed   char  INT8;                    /* defined for signed 8-bits integer variable		有符号8位整型变量  */
typedef unsigned short UINT16;                  /* defined for unsigned 16-bits integer variable 	无符号16位整型变量 */
typedef signed   short INT16;                   /* defined for signed 16-bits integer variable 		有符号16位整型变量 */
typedef unsigned int   UINT32;                  /* defined for unsigned 32-bits integer variable 	无符号32位整型变量 */
typedef signed   int   INT32;                   /* defined for signed 32-bits integer variable 		有符号32位整型变量 */
typedef unsigned char BOOLEAN;
typedef unsigned int size_t;

typedef struct 
{
unsigned int  bit0_1 : 2;
unsigned int   bit2 : 1;
unsigned int bit3 : 1;
unsigned int bit4 : 1;
unsigned int bit5 : 1;
unsigned int bit6 : 1;
unsigned int bit7 : 1;
unsigned int bit8_16 ;
}ChBit;


//按键定义
#define KEY_0            '0'   //  0键
#define KEY_1            '1'   //  1键
#define KEY_2            '2'   //  2键
#define KEY_3            '3'   //  3键
#define KEY_4            '4'   //  4键
#define KEY_5            '5'   //  5键
#define KEY_6            '6'   //  6键
#define KEY_7            '7'   //  7键
#define KEY_8            '8'   //  8键
#define KEY_9            '9'   //  9键
#define KEY_DOT          '.'   //  #键
#define KEY_KJ           '*'   //  *键   
#define KEY_UP           'U'   //向上导航 
#define KEY_DOWN         'D'   //向下导航
#define KEY_LEFT         'L'   //左向导航
#define KEY_RIGHT        'R'   //右向导航
#define KEY_CANCEL       'C'   //取消键
#define KEY_DELETE       'T'   //删除键
#define KEY_ENTER        'E'   //确认键
#define KEY_LIGHT        'G'   //背光灯
#define KEY_FUNC         'F'   //快捷键
#define KEY_ONOFF        'O'   //开关机按键
#define KEY_NOHOOK       0xFF  //没有按键 

#define KEY_SCAN         'S'   //
#define KEY_TAB          'B'   // 



#define S_IFMT		0170000
#define S_IFDIR		0040000 
#define S_IFREG		0100000

 struct _Field{
  
    uint16  offset;            
    uint16  len;                
    uint8   type;              
    uint8   adix  ;                  
  };
   
   

typedef  struct
{  uint32  left;			// 窗口位置(左上角的x坐标)
   uint32  top;			// 窗口位置(左上角的y坐标)
    
   uint32  width;		// 窗口宽度
   uint32  hight;		// 窗口高度
   
   uint8   *title;		// 定义标题栏指针 (标题字符为ASCII字符串，最大个数受窗口限制)
   uint8   *state;		// 定义状态栏指针 (若为空时则不显示状态栏)   
} WINDOWS;


typedef struct { 
  int  x;
  int  y;
  int  x1;
  int  y1;
  uint8 color;
  char *str;
  char *lpCaption;
  uint8 itype;
}_GuiStrStru;

#define MMENU_NO 8
#define  SMENU_NO					8

typedef  struct
{  WINDOWS  *win;		
   uint32  left;			// 窗口位置(左上角的x坐标)
   uint32   top;			// 窗口位置(左上角的y坐标);			
   char     *title;
   uint8    no;						// 主菜单个数   注意，在350里面最大数值是8
   char     *str[MMENU_NO];			// 主菜单字符串  注意，在350里面最大数值是8
   char     *key[MMENU_NO];			// 主菜单字符串  注意，在350里面最大数值是8
   void    (*Function[SMENU_NO])(void);		// 子菜单对应的服务程序  注意，在350里面最大数值是8
   void    (*FunctionEx)(void);	    //非按键回调函数 
   } _GuiMenuStru;
   
typedef  struct
{  WINDOWS  *win;					// 所属窗口
   uint32   x;                      //开始X坐标
   uint32   y;                      //开始Y坐标
   uint8    isRt;                  //是否带窗体
   uint8    with;                   //宽度
   uint8    MaxNum;                 //一屏幕多少个
   uint8    no;						//总共有多少个
   char     *title;                    //主题
   char     *str[255] ;			        // 主菜单字符串
  // uint8    defbar;                  //默认选项
   } _GuiLisStru;

 typedef  struct
{  WINDOWS  *win;					// 所属窗口
   uint32   x;                      //开始X坐标
   uint32   y;                      //开始Y坐标
   uint8    isRt;                  //是否带窗体
   uint8    with;                   //宽度
   uint8    MaxNum;                 //一屏幕多少个
   uint32   no;						//总共有多少个
   char     *title;                    //主题
   char     *str[255] ;			        // 主菜单字符串
   uint32    defbar;                  //默认选项
   } _GuiLisStruEx;
   
   
typedef  struct
{     
   uint32   x;                      //开始X坐标
   uint32   y;                      //开始Y坐标   
   uint8    isRt;                  //是否带窗体
   uint8    with;                   //宽度
   uint8    MaxNum;                 //一屏幕多少个
   uint32   no;						//总共有多少个
   char     *title;                 //主题
   char     **str;			        // 主菜单字符串
   uint32   defbar;                //默认选项
   void    (*Function)(uint32 *Key,uint32 *Serial);	    
   //如果不是数据库模式 嵌入服务服务程序 key ,按键, Serial 当前号  回调函数里面可以修改关键字和序号
   //如果是数据库模式 数据库方式,key低位8 是按键数值,高24位是当期数组记录地址+0x81000000 Serial: 是序号8位,高24位是第几屏幕
   
   uint32   type;                   //0 代表之前的风格,1,带左右按键返回, 2代表数据库方式
   uint8   fieldname;  //字段名称
   uint8   condition;  //条件 '>'、 '<'、 '='、 '#'不等于、 '@' 包括 、'>'+'=' 大于等于 、'<'+'='小于等于
   char    *Contect;//条件内容
   
   } _DBListStr;
  
   

//  返回0 按下取消按键, 
//      -1 KEYUP 
//      -2 KEYDOWN 
typedef  struct
{   
	 
   	uint32  top;		// 位置(左上角的x坐标)
    uint32  left;		// 位置(左上角的y坐标)
    uint32  width;		// 宽度
    uint32  hight;		// 高度
    uint8   *caption;		// 定义显示内容 (最大个数受窗口限制) 
    uint8   *context;
    uint8   type;
    uint8   tabindex;
    uint8   datelen;
    uint8   keyUpDown;  //上下键盘是否有效 0 上键=左键，下键=右键
    uint8   IsClear;    //是否输入后清除显示
}_GuiInputBoxStru;

 
typedef  struct
{  
	WINDOWS  *win;					// 所属窗口
   	uint32  top;			// 位置(左上角的x坐标)
    uint32  left;			// 位置(左上角的y坐标)
    uint32  width;		// 宽度
    uint32  hight;		// 高度
    uint32  max;  	// 最大值
    uint32  min;  	// 最小值
    uint32  value;  	// 最小值
    uint8   *caption;		// 定义显示内容 (最大个数受窗口限制) 
    uint8   tabindex;
    void    (*Function)(void);		// 对应的服务程序
    uint32   stepPt;
    uint8	step;
} _ProGressBar;
  
 




typedef struct __opaque _dir;
struct dirent{
    long d_ino;              
    long d_off;             
    unsigned short d_reclen;  
    char d_name [256]; 
};


struct  stat{
    int		  yst_dev;       
    int           yst_ino;      
    uint32        yst_mode;      
    int           yst_nlink;    
    int           yst_uid;       
    int           yst_gid;      
    unsigned      yst_rdev;      
    int32         yst_size;     
    unsigned long yst_blksize;   
    unsigned long yst_blocks;   
    unsigned long yst_atime;    
    unsigned long yst_mtime;     
    unsigned long yst_ctime;    
};
 
typedef struct {
uint32 x;
uint32 y;
uint8 *dat;
uint32 hno ;
uint32 lno;
 uint8 diff;
} _PicStru;

struct t9PY_index
{
    char  *t9PY_T9;
    char  *PY;
    const char  *PY_mb;
};


typedef struct 
{
    char uSSID[32];
    uint8 uMode;
    uint8 uRSSI;   
}Access_Points;




typedef struct dirent _dirent; 
typedef struct stat _stat; 
//file
extern int8   _Fopen(char *filename, char *mode);
extern int8   _Fclose(int hdl);
extern uint32 _Fread(void *ptr, uint32 size, int hdl);
extern  int32 _Fwrite(void *ptr, uint32 size, int hdl);
extern uint32 _Lseek(int hdl,uint32 offset,uint8 fromwhere);
extern uint32 _Filelenth(int hdl);
extern uint8  _Feof(int hdl);
extern int   _Remove(char *filename);
extern int8   _Access(const char *filenpath, int mode);
extern int8   _MkDir(const char *path);
extern int8   _RmDir(const char *path);
extern _dir * _OpenDir(const char *dirname);
extern _dirent * _ReadDir(_dir *dirp);
extern int    _CloseDir(_dir *dirp);
extern  int   _Rename(const char *oldPath, const char *newPath) ;
extern int32  _GetFreeSpace(void);
extern  int32 _GetFileAtt(const char *,_stat *);
extern  char *  _GetFileList(char * ,char *,char *); 
extern   int32 _Tell(int8);



//dbf	
extern void   _Select(int dbfno );
extern void   _Go(int recno);
extern void   _Skip(int recno);
extern int    _Use(char *dbffile);
extern char * _ReadField(uint8 dbfoffset,char *field);
extern uint32 _Recno(void);
extern uint32 _Reccount(void);
extern uint8  _Bof(void);
extern uint8  _Eof(void);
extern void   _Replace(uint8 dbfoffset,char *field);
extern void   _App(void); 
extern void   _Del(void);
extern uint8  _Deleted(void);
extern void   _Pack(void);
extern void   _Zap(void);
extern void   _GetFieldStr(uint8 fieldname, struct _Field *dbfstr);
extern char * _ReadFieldEx(uint8 dbfoffset,char *field);
extern int    _Locate(uint8 FieldName,char condition,char Contect[],int32 RecStart,int32 RecEnd,int fn); //Fn=0 精确,1 模糊,2,前模糊,3后模糊
extern int    _LocateEx(uint8 FieldName,char condition,char Contect[],int32 RecStart,int32 RecEnd,int fn); //Fn=0 精确,1 模糊,2,前模糊,3后模糊



//time
extern void    _GetTime(char *time,char div);
extern void    _GetDate(char *date,char div);
extern void    _SetTime(char *time);
extern void    _SetDate(char *date);
extern uint8   _GetWeek(void);
extern uint8   _GetDay(void);
extern uint8   _GetMonth(void);
extern uint32  _GetYear(void);
extern void    _GetDateTime(char *time,char time1,char time2); 
extern void    _SetDateTime(char *datetime);
extern uint8   _GetSec(void);             
extern uint8   _GetMin(void);   
extern uint8   _GetHour(void);
extern  volatile uint32 _GetTickCount(void); 
extern void  _Sleep(uint32);
extern uint32 _GetStartTime(void);
 
 
//disp 
 
extern void _ClearScreen(void);
extern void _Printfxy(int x,int y, char *str,int);
extern void _LCMXorDataXY(uint8 x, uint8 y,uint8 x1,uint8 y1);
extern void _GUIHLine(uint8 x0, uint8 y0, uint8 x1, uint8 color);
extern void _GUIVLine(uint8 x0, uint8 y0, uint8 y1, uint8 color);
extern void _GUILine(uint32 x0, uint32 y0, uint32 x1, uint32 y1, uint8 color);
extern void _GUIRectangle(uint32 x0 , uint32 y0, uint32 x1, uint32 y1 , uint8 color);
extern void _GUIRectangleFill(uint32 x, uint32 y, uint32 x1, uint32 y1, uint8 color);
extern void _GUIPoint(uint8 x, uint8 y, uint8 color);
 
extern void _SaveScreenToBuff(uint8 *buff);
extern void _RestoreBuffToScreen(uint8 *buff);
extern void _Printfxy12(int x,int y, char *str,int); //显示12点阵0－9 
extern void _GUILoadPic(_PicStru *); //显示图片
extern char _T9PY_Get_Match_PY_MB(char *p_PadInput,const struct t9PY_index   ** List_match_PY_Mb);  
extern void _toxy(unsigned char X, unsigned char Y);
extern void _HideCur(void);
extern void _ShowCur(void);
extern void _GetCursor(uint8 *,uint8 *);



#define MSG_OK 0     
#define MSG_OKCANCEL 1		
#define IDOK 1
#define IDCANCEL 2

#define DIDOK 0
#define DCANCEL 2

extern uint8  _MessageBox(_GuiStrStru *stru);
extern uint8  _MessageBoxEx(char *,char *,int8);
extern uint8  _Menu(_GuiMenuStru *);
extern uint8  _MenuEx(_GuiMenuStru *);
extern uint8  _List(_GuiLisStru *);
extern uint32 _ListEx(_GuiLisStruEx *);//与上面的_LIST的区别多了默认选项
 
extern uint32  _DBList(_DBListStr *,char * ,int );
extern uint8  _InputBox(_GuiInputBoxStru *); 
extern uint8  _GetStr(_GuiInputBoxStru *); 
extern uint8  _CreateProgressBar(_ProGressBar *);
extern uint8  _IncrementProgressBar(_ProGressBar *); 


extern void   _SetInputMode(uint32); //设置输入方式  默认输入为数值 1数值，2小写英文，3大写英文
extern void   _DisInputMode(uint32); //输入法是否允许切换

extern void   _SetFrontSize(uint8 size);//设置点阵 12 16;2012 5 11
extern uint8  _GetFronSize(void);//获取当前系统电子

 
 //orterh       
extern uint32 _ReadKey(void);  
extern uint32 _GetKeyExt(void);
//控制类
extern void   _OpenRedLight(void);
extern void   _CloseRedLight(void);
extern void   _OpenGreenLight(void);
extern void   _CloseGreenLight(void);
extern uint32 _GetShutDonwTime(void);
extern void   _SetShutDonwTime(uint32 time);
extern void   _SetBeepOnOff(uint8 onoff);
extern uint32 _GetMbt(void);
extern uint32 _GetSbt(void);
extern void   _Beep(void);//发声音
extern void   _OpenLcdBackLight(void);//开背光灯
extern void   _CloseLcdBackLight(void);//关背光灯
extern void   _OpenKeyBackLight(void);//开键盘灯
extern void   _CloseKeyBackLight(void);//关键盘灯
extern void   _OpenFlashLight(void);//打开手电筒
extern void   _CloseFlashLight(void);//关闭手电筒 
extern void   _EnableArmIrqInt(void);//打开时钟中断
extern void   _DisableArmIrqInt(void);//关闭时钟中断 
//调用系统类
 
 
 
 
 
   
  
extern  uint32  _SetBacklightSys(void);//系统设置对比读 380,版本在 1.00.08以后才有
extern  void  _SetLCDContrastSys (void);//系统设置液晶亮度
extern void   _ShutDownPower(void); //关机函数
extern void   _CalendarSys(void); //系统万年历
extern void   _BatteryDisplaySys(void); //系统电池显示
extern void   _CalculatorSys(void);//系统计算器
extern void   _FormatSys(void);//系统格式化
extern uint32   _SoundSwitchSys(void);//系统声音管理  380,版本在 1.00.08以后才有
extern void   _FileTransSys(void);//文件通信 
extern uint32   _AutoShutDonwMenuSys(void);//自动关机菜单  380,版本在 1.00.08以后才有
extern uint32   _TimeSettingSys(void);//系统时间 380,版本在 1.00.08以后才有
 
extern void   _DispTimeSys(void);//主菜单中的时间显示
extern void   _GetMachineSn(char *);//获取机器编号 
extern void   _InfoSys(void);//调用系统通讯设置菜单 380,版本在 1.00.08以后才有
extern void   _Reset(void); //复位机器
extern void   _SetFileTran(char,char);//设置默认文件通讯方式
extern void   _SoundOn(void);//开声音 
extern void   _SoundOff(void); //关声音 
extern void   _RunHex(char *filename);//运行程序
extern void   _GetTransCommPort(uint32 *,uint32 *);//获取系统文件通讯端口和速度
extern void   _SetShutDownTimeEx(uint32);//设置定时关机，但复位后无效
extern void   _SetKey(uint8 *);//自定义按键
extern void   _IsPowerKey(uint8);//运行二次开发后，关机按键是否生效 0 失效，非0生效 
extern void   _SetPowOffPro(uint32  (* pro)(void));//关机前函数
    
//其他类
extern char *  _DoubleToStr(   char *s1 ,  double s2 ,int n );
extern void   _GetSysVer(char * ver,char *UpdateTime);//获取系统版本更新日期



//串口
extern void   _ComSet(uint8 *Setting,uint8 comport);
extern void   _ComSetTran(uint8 Tran);
extern void   _SendComStr(uint8 *SendStr,uint32 SendSize);
extern uint32 _GetComStr(uint8* Getstr,uint32 GetSize,uint32 GetMs);
extern void   _CloseCom(void);
extern void _ComChannel(uint8 val);

//标准

extern void    _strcat(char *s1 ,  const char *s2);
extern void    _strncat(char *s1 ,  const char *s2, size_t n);
extern int     _memcmp(const char *s1 ,  const char *s2, size_t n);
extern int     _strcmp(const char *s1 ,  const char *s2 );
extern int     _strncmp(const char *s1 ,  const char *s2, size_t n);
extern size_t  _strxfrm(char *s1 ,  const char *s2, size_t n);
extern char *  _strchr(const char *s1 ,   char  s2);
extern size_t  _strcspn(const char *s1 ,  const char *s2 );
extern char *  _strpbrk(const char *s1 ,  const char *s2 );
extern char *  _strrchr(const char *s1 , int n );
extern char *  _strstr(const char *s1 ,  const char *s2 );
extern void *   memset(void *s1 , int c,size_t n );
extern size_t  _strlen(const char *s1  );
extern char *  _strcpy(  char *s1 ,  const char *s2 );
extern char *  _strncpy(   char *s1 ,  const char *s2 ,size_t n );
extern void    _free(void *ptr);
extern void *  _malloc(size_t len );
extern void *  _realloc(void *mem_address, unsigned int newsize);
extern unsigned long _strtoul(const char *,char **,int );
extern char *  _strtok (char *s , const char *s1 );

	
extern char *  _ltrim(char *dest);
extern char *  _rtrim(char *dest); 
extern char *  _trim(char *dest); 
extern void _leftspace(char * ,uint16 ,char );//左边补字符,源字符,数据长度,要补字符


extern size_t strnlen(const char * s, size_t count) ; //20120616
extern uint32    _sprintfEx(char *buff,const char *fmt, ...) ; //20110902
extern uint32    _sprintfxy(int x,int y,int color,const char *fmt, ... );//20110902 只开放到400个字节

extern double  _atof(char *Sour);
extern void *  _memcpy(void *s1, const void *s2, size_t n);
extern int32  _abs(int32 );
//RFIC
 
#define RFID_TIMEOUT    0xFF
#define RFID_ERROR      0xEE
#define RFID_OK      	0x00
#define RFID_ALL_CARD	0x52    
    
extern void   _RfidInit(void);
extern uint16 _RfidFind(uint8 active);
extern uint16 _RfidReadCardId(uint8 cardid[]);
extern uint16 _RfidSelectCard(uint8 cardid[]);


extern uint16 _RfidCheckPwd(uint8 pwd[],uint8 cardid[],uint8 address,uint8 type);
extern uint16 _RfidWriteData(uint8 data[],uint8 address);
extern uint16 _RfidReadData(uint8 data[],uint8 address);
extern uint16 _RfidCharge(uint32 value,uint8 address,uint8 type);
extern uint16 _RfidBackup(uint8 Saddress,uint8 Taddress);
extern uint16 _RfidDormancy(void);
extern uint16 _RfidReset(void);
extern uint16 _RfidOpenAntenna(void);
extern uint16 _RfidCloseAntenna(void);
extern void   _RfidClose(void);
extern int16  _RfidInitWallet(uint32 value,uint8 address);
extern uint32 _RfidReadWallet(uint8 address);
extern int8   _Rfid125Read(uint8 *id,int32 len,int32 ms);
extern int    _Rfid125Start(void);
extern void   _Rfid125End(void);


extern uint8 _ScanBarcodeKey(uint8 *barcode,uint8 evkey); //只用于带条码机器
extern uint8 _ScanBarcode(uint8 *barcode);  //只用于带条码机器
extern int   _ScanStart(void);//A380机器函数 20121123
extern void  _ScanEnd(void);//A380机器函数 20121123
extern void _ShockOpen(void);
extern void _ShockClose(void);
extern void	_Shock(int ms);
extern uint8	_ScanBarcodeEx(uint8 *barcode,uint8 *type);//获取条码，并获取种类,返回来是长度
		
 
extern uint8 _BtOpen(void); //A380机器函数
extern void _BtClose(void);  //A380机器函数
extern void _BtSetModel(uint8 atmodel); //A380机器函数
extern uint8 _BtGetModel (void); //A380机器函数
extern int8 _BtScan(uint8 num,char *cClass,char *cIac); //A380机器函数
extern int8 _BtName (char *name,uint8 op); //A380机器函数
extern int8 _BtRName(char*name ,char *addr); //A380机器函数
extern int8 _BtAddr(char *addr); //A380机器函数
extern int8 _BtPair(char *addr,uint16 ms); //A380机器函数
extern int8 _BtLink(char *addr); //A380机器函数
extern int8 _BtPswd(char *pswd,uint8 op); //A380机器函数
extern int8 _BtDefault(void); //A380机器函数
extern int16 _BtSendData(uint8 * Buff,uint32 Len); //A380机器函数
extern uint32 _BtGetData(uint8 *RecBuff,uint32 len,uint16 ms); //A380机器函数
extern int8 _BtMrad(char *addr); //A380机器函数
extern int8 _BtFsad(char *addr); //A380机器函数

extern void _BtGetList(void); //A380机器函数

extern int8 _WifiAbrd(void); //A380机器函数
extern int8 _WifiUpgrade(void); //A380机器函数
extern int8 _WifiLoadDone(void); //A380机器函数
extern int8 _WifiBand(uint8 band_val); //A380机器函数
extern int8 _WifiInit(void); //A380机器函数
extern int8 _WifiPassscan(uint16 bit_map); //A380机器函数
extern int8 _WifiSetNetworkType(char *Type); //A380机器函数
extern int8 _WifiPreSharedKey(char *psk); //A380机器函数
extern int8 _WifiScan(uint8 chan_num,char *ssid,Access_Points *ap,uint8 ap_num); //A380机器函数
extern int8 _WifiAuthmode(uint8 value); //A380机器函数
extern int8 _WifiSetIp(uint8 dhcp,char *ip,char *subnet,char *gateway); //A380机器函数
extern int8 _WifiCreateSockets(int8 type ,char *ipaddr,char *portno,char *lport); //A380机器函数
extern int8 _WifiCloseSocket(int8 socketno); //A380机器函数
extern int16 _WifiSendData(int8 sockno,uint16 slength,char *sdata,char *dip,char *dport); //A380机器函数
extern int8 _WifiClose(void); //A380机器函数
extern int8 _WifiOpen(void); //A380机器函数
extern int8 _WifiJoin(char *ssid_name,uint8 TxRate,uint8 TxPower);//A380机器函数
extern int16 _WifiReceiveData(int8 *sockno,char *sdata,uint16 ms);//A380机器函数

//GPRS
extern int16 _GprsGetSignal(char *cResult);
extern int16 _GprsGetRegistration(char *cResult);
extern int16 _GprsGetCurrentSelection(char *cResult);
extern int16 _GprsGetAllSelection(char *cResult);
extern int16 _GprsGetSimPinStatus(char *cResult);
extern int16 _GprsSetSimPin(char *cPin,char *cNewPin);
extern int16 _GprsSetService(uint8 uOperate);
extern int16 _GprsGetService(char *cResult);
extern int16 _GprsSetMultiIp(uint8 uOperate);
extern int16 _GprsGetMultiIp(void);
extern int16 _GprsStartTask(char *cApn,char *cName,char *cPassword);
extern int16 _GprsActivatePDP(void);
extern int16 _GprsGetLocalIp(char *ip);
extern int16 _GprsSetConnection(char*cMode,char *cIp, char * cPort,uint16 ms);
extern int16 _GprsGetIpStatus(int8 iMultiIp,char *cStatus);
extern int16 _GprsSendData(uint8 *cData,uint16 iLength);
extern int16 _GprsIpClose(void);
extern int16 _GprsDeactivatePDP(void);
extern int16 _GprsSetTcpipMode(uint8 uMode);
extern int16 _GprsGetTcpipMode(void);
extern int16 _GprsGetAtStatus(void);
extern int16 _GprsSetDefault(void);
extern int8  _GprsOpen(uint32 iBaud);
extern int8  _GprsClose(void);
extern int16 _GprsSetSingleConnection(char *cApn,char*cMode,char *cIp, char *iPort);


/*
功能 设置A380 PORT0 PORT1 的IO J2(USB口IO) 输出值
pin 管脚
val  1为输出。0为输入
正确设置为1
错误设置为0 */
extern int  _SetIo(int pin,int val);
/****
_GetIo 
功能 获取A380 PORT0 PORT1 的IO J2(USB口IO) 的值
正确设置为读出的值
错误设置为-1 
*****/
extern int  _GetIo(int pin);

/****
_SetIoDir 
功能 设置A380 PORT0 PORT1  J2(USB口IO) 的IO方向, 
参数
PIN  管脚
DIR  1 为输出，0为输入

正确设置为1
错误设置为0 
*****/
extern int _SetIoDir(int pin,int dir);

#endif
