/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CDBResourceGrabEvent.h
Version:		1.0
Description:    资源抢夺数据库
Author:			pengwen
Create Date:	2013-05-13
History:        处理DB好友事件   

******************************************************************/

#ifndef CCDBRESOURCEGRABEVENT_H_
#define CCDBRESOURCEGRABEVENT_H_

#include "CGameMesEvent.h"

class CCDBResourceGrabEvent :
	public CCGameMesEvent
{
private:
	CCDBConnector &m_dbConnector;
public:
	CCDBResourceGrabEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCDBResourceGrabEvent();
private:
	//用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID, CT_CHAR* pBuf,		CT_DWORD len, CT_LONGLONG reserve);
	//定时器
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID);
	//得到全部资源信息
	CT_VOID GetAllResSpot();
	//得到全部资源生产信息
	CT_VOID GetAllResSpotProduct();
	//保存资源争夺数据到数据库
	CT_VOID SaveResourceData(CT_CHAR* pBuf, CT_DWORD len);
	//读取资源点活动
	CT_VOID DisposeResActive();

	CT_VOID GetAllScene();
};

#endif