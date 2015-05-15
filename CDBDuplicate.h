/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CDBFightingThread.h
Version:		1.0
Description:    数据库副本服务
Author:			wangchen
Create Date:	2012-10-25
History:        处理DB副本事件   

******************************************************************/
#ifndef CCDBDUPLICATETHREAD_H_
#define CCDBDUPLICATETHREAD_H_

#include "CGameMesEvent.h"
class CCDBConnector;

class CCDBDuplicate:public CCGameMesEvent
{
private:
	CCDBConnector& m_dbConnector;
public:
	CCDBDuplicate(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCDBDuplicate(void);

private:
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
		CT_CHAR* pBuf,		CT_DWORD len,
		CT_LONGLONG reserve);

	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}
private:
	//战斗信息
	CT_VOID DisposeFightingInfo(CT_CHAR* pBuf,CT_DWORD len);
	//世界信息
	CT_VOID DisposeWorldScene(CT_CHAR* pBuf,CT_DWORD len);
	//场景信息
	CT_VOID DisposeSceneInfo(CT_CHAR* pBuf,CT_DWORD len);
	//购买战斗次数
	CT_VOID DisposeBuyFightingTime(CT_CHAR* pBuf,CT_DWORD len);
	//扫荡信息
	CT_VOID DisposeRaidsInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//扫荡奖励
	CT_VOID DisposeRaidsReward(CT_CHAR *pBuf,CT_INT32 iLen);
	//组队副本场景开启
	CT_VOID DisposeTeamDuplicateScene(CT_CHAR *pBuf,CT_INT32 iLen);
	//组队副本自动匹配
	CT_VOID DisposeTeamDuplicateAutoMatch(CT_CHAR *pBuf,CT_INT32 iLen);


};
#endif