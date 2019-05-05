#TITLE=       DBF数据库类
#INFO
EditPlus Cliptext Library v1.0 written by ES-Computing.
This file is provided as a sample Clitext library of EditPlus.
#SORT=n
 
#T=_Select   选择工作区
_Select(
#T=_Go       定位记录	
_Go( 
#T=_Skip     相对移动记录
_Skip( 
#T=_Use      打开数据库
_Use(
#T=_Recno    获取当前记录	
_Recno()
#T=_Reccount 获取当前工作区总记录
_Reccount()
#T=_Bof      是否到第一条记录	
_Bof()
#T=_Eof      是否到最后一条记录	
_Eof();
#T=_App      追加一条空记录
_App();
#T=_Del      逻辑删除当前记录
_Del();
#T=_Pack     物理删除记录
_Pack();
#T=_Zap      清空所有记录	 
_Zap();
#T=_Replace  更新记录
_Replace(
#T=_ReadField读字段
_ReadField( 
 #T=TEST_ID N  2
TEST_ID
#T=TEST_Name C  10
TEST_Name
#