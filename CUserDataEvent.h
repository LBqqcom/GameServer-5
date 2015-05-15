/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CUserDataEvent.h
Version:		1.0
Description:    �û������¼���
Author:			wangchen
Create Date:	2014-1-8
History:             

******************************************************************/
#ifndef CCUSERDATAEVENT_H_
#define CCUSERDATAEVENT_H_
#include <map>
#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCUserDataEvent:public CCGameMesEvent
{
public:
	CCUserDataEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCUserDataEvent(void);

private:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID){};
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}

private:
	//��������
	CT_VOID DisposeInsertData(CT_CHAR *pBuf,CT_DWORD len);
	//���뱻������Ϣ
	CT_VOID DisposeSeekedInfo(CT_CHAR* pBuf, CT_INT32 iLen);
};
#endif
