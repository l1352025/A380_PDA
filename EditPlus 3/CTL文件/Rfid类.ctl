#TITLE=      RFID类
#INFO
EditPlus Cliptext Library v1.0 written by ES-Computing.
This file is provided as a sample Clitext library of EditPlus.
#SORT=n

#T=_RfidInit               卡的初始化 
_RfidInit(
#T=_RfidFind               寻卡并返回卡类型
_RfidFind(
#T=_RfidReadCardId         读卡序列号
_RfidReadCardId(
#T=RfidSelectCard          选定卡片
_RfidSelectCard(
#T=_RfidCheckPwd           验证卡片密码
_RfidCheckPwd(
#T=_RfidWriteData          写数据到M1卡某一块
_RfidWriteData(
#T=_RfidReadData           读M1卡某一块数据
_RfidReadData(
#T=_RfidCharge             扣款或充值
_RfidCharge(
#T=_RfidBackup             备份钱包
_RfidBackup(
#T=_RfidDormancy           命令卡进入休眠状态
_RfidDormancy(
#T=_RfidReset              复位RFID模块
_RfidReset(
#T=_RfidOpenAntenna        开启天线
_RfidOpenAntenna(
#T=_RfidClose              关闭天线
_RfidClose(
#T=_RfidInitWallet         初始化钱包
_RfidInitWallet(
#T=_RfidReadWallet         读取钱包
_RfidReadWallet(       
#T=_Rfid125Read            读125模块
_Rfid125Read(
#T=_Rfid125Start           打开125模块
_Rfid125Start(
#T=_Rfid125End             关闭125模块
_Rfid125End(
#