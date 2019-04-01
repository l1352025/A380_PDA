#TITLE=    数据库类
#INFO
EditPlus Cliptext Library v1.0 written by ES-Computing.
This file is provided as a sample Clitext library of EditPlus.
#SORT=n

#T=_Select       选定一个工作区为当前工作区
_Select(
#T=_Go           定位记录
_Go(
#T=_Skip         相对移动记录
_Skip(
#T=_Use          打开数据库（参数为空时为关闭数据库)
_Use( 
#T=_ReadField    读字段
_ReadField(
#T=_Recno        返回当前记录号
_Recno(
#T=_Reccount     数据库总记录
_Reccount(
#T=_Bof          检测是否到第一条记录
_Bof(
#T=_Eof          检测是否到最后一条记录
_Eof(
#T=_Replace      字段更新
_Replace(
#T=_App          追加一条空记录
_App(
#T=_Del          标记当前记录已经被删除
_Del(
#T=_Deleted      判断当前记录是否被标记为删除
_Deleted(
#T=_Pack         物理删除已标记删除记录
_Pack(
#T=_Zap          清空数据库
_Zap(
#T=_Locate       查找记录
_Locate(
#T=_Field        字段属性结构体
_Field
#T=_GetFieldStr  获取字段属性
_GetFieldStr(
#T=_ReadFieldEx  读取字段,不会省略内容中的空格且末尾无字符串结束符
_ReadFieldEx(

#