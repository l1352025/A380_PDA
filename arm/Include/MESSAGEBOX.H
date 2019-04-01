#ifndef  messagebox_H
#define  messagebox_H 
#define  gFont 16
#define  langueage 1
#define  CHI 1
#define  CaptionHight   (gFont + 4)  		//标题栏高度
#define  ButtonHight    (gFont + 4 + 4)   //按钮高度
#define  BlankHight		2     			//空白的像素
#define  CloseWidth		(gFont + 4)		//关闭按钮的宽
#define  BlankWidth    2  				//空白的像素
#define  ButtonWidth   (3 * gFont)       //按钮宽度
#define  Border        1				//边框
#define  GUI_LCM_XMAX		160							/* 定义液晶x轴的像素数 */
#define  GUI_LCM_YMAX		160							/* 定义液晶y轴的像素数 */ 
#define  _GUI_RectangleFill  _GUIRectangleFill
#define  _GUI_Rectangle  _GUIRectangle
#define  _GUI_Line _GUILine
#define  LCM_XorDataXY _LCMXorDataXY
#define  _Get_Key_Ext _GetKeyExt
#define  LEVE1COLOR			0x0 //白色
#define  LEVE2COLOR			0x0 //白色 
#define  LEVE3COLOR			0x01 //黑色 
#define  LEVE4COLOR			0x01 //黑色


#define MB_OK			0	 //消息框含有一个按钮：OK。这是缺省值。
#define MB_OKCANCEL		1	 //消息框含有两个按钮：OK和Cancel。

#define IDOK 		1 	    //返回值	　　
#define IDCANCEL 	2 		//返回值
#define NULL 0
typedef  struct
{  
	WINDOWS  *win;					// 所属窗口
   	uint32  top;		// 位置(左上角的x坐标)
    uint32  left;		// 位置(左上角的y坐标)
    uint32  width;		// 宽度
    uint32  hight;		// 高度
    uint8   *caption;		// 定义显示内容 (最大个数受窗口限制) 
    uint8   *context;
    uint8   type;       //0,为显示确定,1为显示确定,取消
    uint8   dtype;      //当type 为1的时候,0默认选择取消,1为确定
    uint8   tabindex;
    void    (*Function)(void);		// 按钮对应的服务程序
} MESSAGEBOX;

extern uint8 DMessageBoxEx(char *lpCaption,char *str1,int8 itype);
#endif 