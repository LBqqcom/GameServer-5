/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CDBPVPEvent.h
Version:		1.0
Description:    数据库竞技场服务
Author:			wangchen
Create Date:	2012-10-25
History:        处理DB竞技场事件   

******************************************************************/
#ifndef CCDBPVPEVENT_H_
#define CCDBPVPEVENT_H_

#include "CGameMesEvent.h"
class CCDBConnector;
class CCUser;

class CCDBPVPEvent:public CCGameMesEvent
{
private:
	CCDBConnector& m_dbConnector;
	CCUser			*m_pUser;
public:
	CCDBPVPEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCDBPVPEvent(void);

private:
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
		CT_CHAR* pBuf,		CT_DWORD len,
		CT_LONGLONG reserve);

	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID);

private:
	//竞技场信息
	CT_VOID DisposePVPInfo(CT_CHAR* pBuf,CT_DWORD len);
	//刷新对手
	CT_VOID DisposeRefresh(CT_CHAR* pBuf,CT_DWORD len);
	//战斗记录
	CT_VOID DisposeFightingEnd(CT_CHAR* pBuf,CT_DWORD len);
	//挑战状况
	CT_VOID DisposeChallengeInfo(CT_CHAR* pBuf,CT_DWORD len);
	//竞技场终结
	CT_VOID DisposePVPEnding(CT_CHAR *pBuf,CT_DWORD len);
	//保存对手信息
	CT_VOID DisposePVPChallgerSave(CT_CHAR* pBuf, CT_DWORD len);
};
#endif