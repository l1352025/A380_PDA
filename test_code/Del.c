#include <HJLIB.H>
#include "string.h"
#include "stdio.h"
#include "stdarg.h"

typedef unsigned char bool;
#define true 1
#define false 0

typedef enum{
    Color_White = 0,
    Color_Black = 1
}GUI_COLOR;

#define LogFileName "log.txt"
#define LogFileSize 1024 * 4
#define Log_On 1
unsigned char LogBuf[LogFileSize];

void LogWrite(void *buf, uint32 len)
{
#if Log_On
	int fp;

	if(_Access(LogFileName, 0) < 0){
		fp = _Fopen(LogFileName, "W");
	}else{
		fp = _Fopen(LogFileName, "RW");
	}
	
	_Lseek(fp, 0, 2);
	_Fwrite(buf, len, fp);
	_Fclose(fp);
#endif
}

void LogPrint(const char * format, ...)
{
#if Log_On
	uint32 len = 0; 
	va_list ap;
	uint8 *buf;
	char time[24];

	// buf = (uint8 *) _malloc(2048);	// 1.使用动态内存时
	buf = &LogBuf[0];					// 2.静态内存

	_GetDateTime(time, '-', ':');
	len += sprintf(&buf[len], "[%s] ", time);
	va_start(ap, format);
	len += vsprintf(&buf[len], format, ap);
	buf[len++] = '\n';
	va_end(ap);
	
	LogWrite(buf, len);

	//_free(buf);						// 使用动态内存时
#endif
}

/*
* 描  述：显示提示消息框，等待 n ms后返回
* 参  数：
*		x ，y - 消息框在屏幕中的坐标 x, y
*		str - 显示的字符串
*		waitMs - 等待的毫秒数
* 返回值：void
*/
void ShowMsg(uint8 x, uint8 y, char *str, uint16 waitMs)
{
	uint8 strTmp[20], i, j, rowCnt, colCnt;
	uint16 len = strlen(str);	

	if(x < 8) x = 8;
	if(y < 8) y = 8;
	
	colCnt = ((160 - x - x) / 16) * 2;
	rowCnt = (len + colCnt - 1) / colCnt;

	if(y + rowCnt * 16 > 160){
		rowCnt = (160 - y) / 16;
	}
	
	_GUIRectangleFill(x - 8, y - 8, x + colCnt * 8 + 8, y + rowCnt * 16 + 8, Color_White);
	_GUIRectangle(x - 8, y - 8, x + colCnt * 8 + 8, y + rowCnt * 16 + 8, Color_Black);
	
	for(i = 0, j = 0; i < rowCnt; i++){
		memcpy(strTmp, &str[j], colCnt);
		strTmp[colCnt] = 0x00;
		j += colCnt;
		_Printfxy(x, y + i * 16, strTmp, Color_White);
	}

	_Sleep(waitMs);
}


void FileDeleteFunc(void)
{
	char *fileName;
    char tmp[512];
    int8 fp;

	fp = _Fopen("deleted.txt", "W");


    while(1){

        _ClearScreen();
        
        fileName = _GetFileList("选|\n择|\n要|\n删|\n除|\n的|\n文|\n件|\n  |\n  |\n", "", "");

        if(fileName == 0){
            break;
        }

        if(_Remove(fileName) == -1){
			sprintf(tmp, "删除失败：%s", fileName);
			continue;
        }
		
		_Fwrite(fileName, strlen(fileName), fp);
		_Fwrite("\r\n", 2, fp);

		sprintf(tmp, "已删除：%s", fileName);
        ShowMsg(16, 3*16, tmp, 1500);
    }

	_Fclose(fp);
}

void DeleteAll(const char * dirname)
{
	_dir *d;
	_dirent *de;
	_stat s;
	char str[256];
	char fileName[256];
	char dname[256];
	char *ptr;
	char *logBuf = &LogBuf[0];
	int8 dirLevel = 0;
	bool isDir = false;
	int len = 0;
	int fp;
	int8 ret;

	_ClearScreen();

	strcpy(dname, dirname);

	while(1)
	{
		d = _OpenDir(dname);

		ptr = d != 0 ? "成功" : "失败";
		len += sprintf(&logBuf[len], "进入目录：%s    %s \r\n", dname, ptr);

		while ( d != 0 && ((de = _ReadDir(d)) != NULL))
		{
			_ClearScreen();

			sprintf(fileName, "%s/%s", dname, de->d_name);

			if(fileName[0] == '/' && fileName[1] == '/'){
				sprintf(fileName, "%s", &fileName[1]);
			}
			//if(strcmp("/DEL.HEX", fileName) == 0) continue;
			if(strncmp("/lost+", fileName, 6) == 0) continue;

			_GetFileAtt(fileName, &s);

			_Printfxy(0, 0, de->d_name, 0);
			sprintf(str, "长度:%d ", s.yst_size);
			_Printfxy(0, 16, str, 0);

			switch (s.yst_mode & S_IFMT)
			{
			case S_IFREG:
				_Printfxy(0, 32, "属性：文件", 0);
				isDir = false;
				break;
			case S_IFDIR:
				_Printfxy(0, 32, "属性：目录", 0);
				isDir = true;
				break;

			default:
				_Printfxy(0, 32, "unknown", 0);
				isDir = false;
				break;
			}

			if(isDir) break;

			_CloseDir(d);
			ret = _Remove(fileName);

			ptr = ret == 1 ? "成功" : "失败";
			sprintf(str, "删除文件：%s", fileName);
			ShowMsg(16, 4*16, str, 1000);
			len += sprintf(&logBuf[len], "删除文件: %s    %s\r\n", fileName, ptr);

			d = _OpenDir(dname);
		}

		// close curr dir
		_CloseDir(d);

		_ClearScreen();

		if(isDir) {
			// go to sub dir
			dirLevel++;
			sprintf(dname, "%s", fileName);

			isDir = false;
		}
		else{
			// delete curr dir
			ret = _RmDir(dname);

			ptr = ret == 0 ? "成功" : "失败";
			sprintf(str, "删除目录：%s", dname);
			ShowMsg(16, 4*16, str, 1500);
			len += sprintf(&logBuf[len], "删除目录: %s    %s \r\n", dname, ptr);
			

			// back to parent dir 
			dirLevel--;
			if(dirLevel < 0) break;

			ptr = strrchr(dname, '/');
			if(!ptr){
				dname[0] = '/';
				dname[1] = '\0';
			}
			else{
				*ptr = '\0';
			}

			if(dname[0] == '\0'){
				dname[0] = '/';
				dname[1] = '\0';
			}
		}
	}


	if(_Access("DEL.log", 0) < 0){
		fp = _Fopen("DEL.log", "W");
	}else{
		fp = _Fopen("DEL.log", "RW");
	}
	
	_Lseek(fp, 0, 2);
	_Fwrite(logBuf, len, fp);
	_Fclose(fp);

	_ClearScreen();
	_Printfxy(0, 16*4, "    删除完成！ ", 0);
	_Printfxy(0, 16*9, "<< 按任意键返回", 0);

    _ReadKey();
}

void FileDeleteAllFunc(void)
{
	uint8 key;

	_ClearScreen();

	_Printfxy(0, 16*3, "     确定要删除    ", 0);
	_Printfxy(0, 16*4, "  所有文件和目录吗? ", 0);

	_Printfxy(0, 16*6, "将生成日志：DEL.LOG ", 0);
	key = _ReadKey();

	if(key != KEY_ENTER) return;

	DeleteAll("/");
}

int main(void)
{
    FileDeleteAllFunc();
}