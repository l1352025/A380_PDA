#TITLE=       文件操作类
#INFO
EditPlus Cliptext Library v1.0 written by ES-Computing.
This file is provided as a sample Clitext library of EditPlus.
#SORT=n

#T=_Fopen     1打开文件	
_Fopen(
#T=_Fclose    2关闭文件
_Fclose(
#T=_Fread     3读文件字节
_Fread(
#T=_Fwrite    4写文件	
_Fwrite(
#T=_Lseek     5文件指针移动	
_Lseek(
#T=_Filelenth 6文件长度
_Filelenth(
#T=_Feof      7判定文件是否结尾	
_Feof(
#T=_Remove    8删除文件
_Remove(
#T=_Access    9检查文件
_Access(
#T=_MkDir     10建立目录
_MkDir(
#T=_RmDir     11删除目录
_RmDir(
#T=_GetFreeSpace 12获取硬盘空间
_GetFreeSpace(
#T=_OpenDir   13打开目录
_OpenDir(
#T=_ReadDir   14读取目录
_ReadDir(
#T=_CloseDir  15关闭目录 
_CloseDir(
#T=_Rename    16文件改名
_Rename(
#T=_GetFileAtt17获取指定(目录/文件)属性
_GetFileAtt(
#T=_GetFileList18列表方式选择文件
_GetFileList(
#T=_DIR       目录结构体
_dir
#T=_dirent    目录信息
_dirent
#T=_stat      文件属性
_stat
#T=S_IFDIR		目录标识
S_IFDIR
#T=S_IFREG		文件标识  
S_IFREG
#T=S_IFMT		  
S_IFMT
#