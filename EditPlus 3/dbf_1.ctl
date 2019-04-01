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
 #T=CHAOBIAO_CBXH C  10
CHAOBIAO_CBXH
#T=CHAOBIAO_YHBH C  10
CHAOBIAO_YHBH
#T=CHAOBIAO_SBBH C  40
CHAOBIAO_SBBH
#T=CHAOBIAO_SBBJ N  16
CHAOBIAO_SBBJ
#T=CHAOBIAO_HM C  50
CHAOBIAO_HM
#T=CHAOBIAO_DZ C  -16
CHAOBIAO_DZ
#T=CHAOBIAO_SBDZ C  50
CHAOBIAO_SBDZ
#T=CHAOBIAO_YSXZ C  2
CHAOBIAO_YSXZ
#T=CHAOBIAO_SYSL N  16
CHAOBIAO_SYSL
#T=CHAOBIAO_BYSL N  16
CHAOBIAO_BYSL
#T=CHAOBIAO_SYDS N  16
CHAOBIAO_SYDS
#T=CHAOBIAO_BYDS N  16
CHAOBIAO_BYDS
#T=CHAOBIAO_CBBZ N  16
CHAOBIAO_CBBZ
#T=CHAOBIAO_CBRQ C  30
CHAOBIAO_CBRQ
#T=CHAOBIAO_YCBZ N  16
CHAOBIAO_YCBZ
#T=CHAOBIAO_DHHM C  12
CHAOBIAO_DHHM
#T=CHAOBIAO_SBMD N  16
CHAOBIAO_SBMD
#T=CHAOBIAO_YHZT N  16
CHAOBIAO_YHZT
#T=CHAOBIAO_HH C  10
CHAOBIAO_HH
#T=CHAOBIAO_CBQ C  5
CHAOBIAO_CBQ
#