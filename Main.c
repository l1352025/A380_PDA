#include <HJLIB.H>
#include "string.h"
//#include "dbf.h"
#include "stdio.h"

// --------------------------------  全局变量  -----------------------------------------
char Screenbuff[160*(160/3+1)*2]; 


// --------------------------------  电力主节点通信  -----------------------------------------

void ElectricMainNodeFunc(void)
{
	
}


// --------------------------------  电力子节点通信  -----------------------------------------

void ElectricSubNodeFunc(void)
{
	
}

// --------------------------------  水力子节点通信  -----------------------------------------

void WaterMainNodeFunc(void)
{
	
}

// --------------------------------  透传模块设置  -----------------------------------------
void TransParentModuleFunc(void)
{
	uint8 key, menuItemNo;
	_ProGressBar progBar;
	_GuiLisStruEx menuList;
	char *fileName;
	char tmp[70];

	_ClearScreen();

	// 菜单
	menuList.title = ">> 透传模块升级 ";
	menuList.no = 3;
	menuList.MaxNum = 3;
	menuList.isRt = 0;
	menuList.x = 0;
	menuList.y = 0;
	menuList.with = 10 * 16;
	menuList.str[0] = "  1. 查看当前版本";
	menuList.str[1] = "  2. 打开升级文件";
	menuList.str[2] = "  3. 开始升级";
	menuList.defbar = 1;
	_GUIHLine(0, 4*16 + 8, 160, 1);

	// 进度条
	progBar.caption = "升级进度";
	progBar.left = 0;
	progBar.top = 7*16;
	progBar.width = 10*16;
	progBar.hight = 16;

	while(1){

		menuItemNo = _ListEx(&menuList);

		if (menuItemNo == 0) 
			return;

		switch(menuItemNo){
		case 1:	// " 查看当前版本 ";
			_GUIRectangleFill(0, 5 * 16, 160, 9 * 16, 0);
			sprintf(tmp, "当前版本: %s\0", "SRWF-4E88M-SG-BXD17-20190401-Vsp0.33");
			_Printfxy(0, 5*16, &tmp[0], 0);
			_Printfxy(0, 6*16, &tmp[20], 0);
			_Printfxy(0, 7*16, &tmp[40], 0);
			break;

		case 2:	// " 打开升级文件 "
			// 选择升级文件
			_GUIRectangleFill(0, 5 * 16, 160, 9 * 16, 0);
			sprintf(tmp, "文件: %s\0", "SRWF-4E88-APP-20190401-Vsp0.33.bin");
			_Printfxy(0, 5*16, &tmp[0], 0);
			_Printfxy(0, 6*16, &tmp[20], 0);
			sprintf(tmp, "大小:%dK,总包数:%d\0", 30, 300);
			_Printfxy(0, 7*16, &tmp[0], 0);
			// 保存路径
			//fileName = 

			break;
			
		case 3:	// " 开始升级 ";
		
			// 初始化
			//progBar.step = 1;	
			progBar.min = 0;
			progBar.max = 300; 	// 总包数
			_GUIRectangleFill(0, 5 * 16, 160, 9 * 16, 0);
			sprintf(tmp, "总包数：%d\0", 300);
			_Printfxy(0, 5*16, &tmp[0], 0);
			sprintf(tmp, "已发送：%d\0",233);
			_Printfxy(0, 6*16, &tmp[0], 0);

			_Printfxy(0, 9*16, "状态: 升级中...", 0);

			_GUIRectangle(0, 7*16, 160, 8*16, 1);

			// 更新进度
			//_CreateProgressBar(&progBar);
			_ReadKey();

			progBar.value = 30;
			//_IncrementProgressBar(&progBar);
			_GUIRectangleFill(0, 7 * 16, 30, 8 * 16, 1);
			_ReadKey();

			progBar.value = 150;
			//_IncrementProgressBar(&progBar);
			_GUIRectangleFill(0, 7 * 16, 80, 8 * 16, 1);
			_ReadKey();

			progBar.value = 250;
			//_IncrementProgressBar(&progBar);
			_GUIRectangleFill(0, 7 * 16, 130, 8 * 16, 1);
			_ReadKey();

			progBar.value = 300 - 10;
			//_IncrementProgressBar(&progBar);
			_GUIRectangleFill(0, 7 * 16, 160, 8 * 16, 1);

			// 升级完成
			_Printfxy(0, 9*16, "状态: 升级完成  ", 0);
			break;
		}
		
		key = _ReadKey();
		
		switch(key){
			
		case KEY_CANCEL:
			return;
			
		case KEY_1:
		case KEY_2:
		case KEY_3:
			menuList.defbar = (key - '0');
			break;

		case KEY_LEFT:
		case KEY_UP:
			menuList.defbar = (menuItemNo <= 1 ? 3 : menuItemNo - 1);
			break;

		case KEY_RIGHT:
		case KEY_DOWN:
		default:
			menuList.defbar = (menuItemNo >= 3 ? 1 : menuItemNo + 1);
			break;
		}

	}
}


// --------------------------------   主函数   -----------------------------------------------
int main(void)
{
	_GuiMenuStru MainMenu;
	MainMenu.left=0;
	MainMenu.top=0;
	MainMenu.no=4;
	MainMenu.title= "  桑锐手持机  ";
	MainMenu.str[0]=" 电力主节点通信 ";
	MainMenu.str[1]=" 电力子节点通信 ";
	MainMenu.str[2]=" 水力子节点通信 ";
	MainMenu.str[3]=" 透传模块升级 ";
	MainMenu.key[0]="1";
	MainMenu.key[1]="2";
	MainMenu.key[2]="3";
	MainMenu.key[3]="4";
	MainMenu.Function[0]=ElectricMainNodeFunc;
	MainMenu.Function[1]=ElectricSubNodeFunc;
	MainMenu.Function[2]=WaterMainNodeFunc;
	MainMenu.Function[3]=TransParentModuleFunc;
	MainMenu.FunctionEx=0;

	_Menu(&MainMenu);
}

