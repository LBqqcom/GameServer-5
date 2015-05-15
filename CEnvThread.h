/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CEnvThread.h
Version:		1.0
Description:    事件服务
Author:			pengwen
Create Date:	2012-8-8
History:        处理用户事件     

******************************************************************/

#ifndef CENVTREAD_H_
#define CENVTREAD_H_
#include "CGateThread.h"
#include "CGameMesEvent.h"

struct CMD_G_EnterGame;
struct MSC_DB_R_UserInfo;


//事件服务
class CCEnvThread :public CCGateThread
{
private:

public:
	CCEnvThread(CT_VOID);
	~CCEnvThread(CT_VOID);

public:
	//打开
	virtual CT_VOID OnOpen(); 
	
	//框架消息
public:
	//定时器
	virtual CT_VOID OnTimer(CT_DWORD timeID); 

	//网络连接进入
	virtual CT_VOID OnNetEventOpen(CCNetInterface* pNetHandle) ;
	//网络数据到来
	virtual CT_VOID OnNetEventRead(CCNetInterface* pNetHandle, CT_DWORD mainID, CT_DWORD subID,  CT_CHAR* pBuf,  CT_DWORD len) ;
	//网络关闭
	virtual CT_VOID OnNetEventClose(CCNetInterface* pNetHandle) ;
	//用户自定义网络数据
	virtual CT_VOID OnNetEventReadRow(CCNetInterface* pNetHandle, CT_CHAR* pBuf,  CT_DWORD len);


};
#endif