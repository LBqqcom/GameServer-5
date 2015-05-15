/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CDBTop.h
Version:		1.0
Description:    数据库排行榜服务
Author:			wangchen
Create Date:	2013-5-24
History:        处理DB排行榜事件   

******************************************************************/
#ifndef CCDBTOP_H_
#define CCDBTOP_H_

#include "CGameMesEvent.h"
class CCDBConnector;

class CCDBTop:public CCGameMesEvent
{
private:
	CCDBConnector& m_dbConnector;
public:
	CCDBTop(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCDBTop(void);

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
	//处理主消息 彭文添加
	CT_VOID DisposeMain( CT_DWORD subID, CT_CHAR* pBuf,  CT_DWORD len);
	//得到排行榜
	CT_VOID DisposeTopGet();
	//保存排名数据岛数据库 彭文整理
	CT_VOID DisposeTopSaveDB(CT_CHAR* pBuf, CT_DWORD len);
	//更新排行榜奖励状态
	CT_VOID UpdateTopRewardStatus(CT_CHAR* pBuf, CT_DWORD len);
};

#endif
