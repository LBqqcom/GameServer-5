/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCUserExitEvent.h
Version:		1.0
Description:    �û��˳�����
Author:			pengwen
Create Date:	2012-11-28
History:        �û��˳��¼�   

******************************************************************/

#ifndef CCUSEREXITEVENT_H_
#define CCUSEREXITEVENT_H_
#include "CGameMesEvent.h"

class CCUser;
class  CCNetInterface;

class CCUserExitEvent : public CCGameMesEvent
{
public:
	CCUserExitEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCUserExitEvent(void);
private:
	//�û��뿪
	CT_BOOL DisposeLeave(CCNetInterface* pNetHandle,CT_DWORD subID,  CT_CHAR* pBuf,  CT_DWORD len);

private:
	//�����û���Ϣ  ����2������Ϊ����
	virtual CT_VOID OnUserEvent(CT_DWORD mainID, CT_DWORD subID,
		CT_CHAR* pBuf,   CT_DWORD len, 
		CT_LONGLONG reserve);
	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID)
	{

	}
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{

	}
};

#endif