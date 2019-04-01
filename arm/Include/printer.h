#ifndef PRINTER_H
#define PRINTER_H
extern void _PrintOpen(void);
extern void _PrintClose(void);
extern uint16 _PrintStr(char *,uint16);
extern uint16 _PrintCmd(char *,uint16);
extern uint16 _PrintMove(int);
extern uint16 _PrintMovePaper(int);
extern void _PrintTestPage(void);

/**********************************
 检测黑标
 uint8 s =0	 检测到黑标才结束
 	   >0 超过S秒后未检测到黑标则返回-1
 uint8 nNum = 0 检测到黑标即停止走纸
!=0 检测到黑标，越过黑标才 停止走纸

**********************************/

int8 _PrintCheckTag(uint8 s,uint8 nNum);
#endif