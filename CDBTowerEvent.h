/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CDBTowerEvent.h
Version:		1.0
Description:    元素之塔数据库
Author:			zhoujun
Create Date:	2013-08-13
History:        处理元素之塔DB事件   

******************************************************************/
#ifndef CCDBTOWEREVENT_H_
#define CCDBTOWEREVENT_H_

#include "CGameMesEvent.h"
class CCDBConnector;

class CCDBTowerEvent:public CCGameMesEvent
{
private:
	CCDBConnector& m_dbConnector;
public:
	CCDBTowerEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCDBTowerEvent(void);

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
	//
	CT_VOID DisposeTowerUpdate(CT_CHAR* pBuf,CT_DWORD len);	

	CT_VOID DisposeTowerSlotReward(CT_CHAR* pBuf,CT_DWORD len);	

	CT_VOID DisposeTowerSlotRewardDel(CT_CHAR* pBuf,CT_DWORD len);	

	CT_VOID DisposeTowerMSGAdd(CT_CHAR* pBuf,CT_DWORD len);	

	CT_VOID DisposeTowerMSGDel(CT_CHAR* pBuf,CT_DWORD len);	

	CT_VOID DisposeTowerFight(CT_CHAR* pBuf,CT_DWORD len);	

	CT_VOID DisposeTowerSlotFight(CT_CHAR* pBuf,CT_DWORD len);	

	CT_VOID DisposeTowerEvent(CT_CHAR* pBuf,CT_DWORD len);	
	CT_VOID DisposeTowerHelp(CT_CHAR* pBuf,CT_DWORD len);	


public:
	//保存塔的数据到数据库
	CT_VOID DisposeSaveTowerToDB();
	//从数据库读取数据
	CT_VOID DisposeLoadTowerFromDB();
};
#endif