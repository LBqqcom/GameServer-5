/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCUserEnterEvent.h
Version:		1.0
Description:    登入服务
Author:			pengwen
Create Date:	2012-8-16
History:        处理登入事件   

******************************************************************/
#ifndef  CCUSERENTER_H_
#define  CCUSERENTER_H_
#include "CGameMesEvent.h"

class  CCNetInterface;
struct CMD_G_EnterGame;
class CCUserEnterEvent : public CCGameMesEvent
{
public:
	CCUserEnterEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCUserEnterEvent(void);

private:
	//处理用户消息  最后的2个参数为备用
	virtual CT_VOID OnUserEvent(CT_DWORD mainID, CT_DWORD subID,
		                        CT_CHAR* pBuf,   CT_DWORD len, 
								CT_LONGLONG reserve);
	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID)
	{
    
	}
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{

	}
private:
	//处理登陆
	CT_VOID DisposeEnter( CCNetInterface* pNetHandle, CT_DWORD subID,  CT_CHAR* pBuf,  CT_DWORD len);
	//正常登录信息
	CT_VOID UserEnter(CMD_G_EnterGame* pGeg, CCNetInterface* pNetHandle);


private:
	//数据库登录信息
	CT_VOID DisposeDBLogin(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);

private:
	//登录验证码
	CT_VOID DisposeValidate(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
	
private:
	//发送用户信息
	CT_VOID SendUserData(CT_DWORD userID, CCNetInterface* pNetHandle);
	//
	CT_VOID Decompress(CT_INT32 *InBuffer,CT_INT32 iInLen,CT_INT32 *OutBuffer);

	//普通活动增加
	CT_VOID DisposeActiveAdd(CT_CHAR* pBuf,  CT_DWORD len);
	//转盘活动添加
	CT_VOID DisposeTrunTableAdd(CT_CHAR* pBuf,  CT_DWORD len);
	//祭祀添加
	CT_VOID DisposeWorshipAdd(CT_CHAR* pBuf,  CT_DWORD len);
};

#endif