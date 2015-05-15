/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		#include "CCustomerFeedBackEvent.h"
Version:		1.0
Description:    信息反馈
Author:			wangchen
Create Date:	2013-4-11
History:        信息反馈   

******************************************************************/

#ifndef CCCUSTOMERFEEDBACKEVENT_H_
#define CCCUSTOMERFEEDBACKEVENT_H_
#include "CGameMesEvent.h"

class CCCustomerFeedBackEvent : public CCGameMesEvent
{
public:
	CCCustomerFeedBackEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCCustomerFeedBackEvent(void);

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
	//信息反馈
	CT_VOID DisposeFeedBack(CT_CHAR *pBuf,const CT_INT32 iLen);

};

#endif