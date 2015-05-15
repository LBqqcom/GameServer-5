/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CDuplicateEvent.h
Version:		1.0
Description:    ���¼���
Author:			wangchen
Create Date:	2012-11-8
History:             

******************************************************************/
#ifndef CCBATTLEARRAYEVENT_H_
#define CCBATTLEARRAYEVENT_H_
#include <map>
#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCUser;

class CCBattleArrayEvent:public CCGameMesEvent
{
public:
	CCBattleArrayEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCBattleArrayEvent(void);

private:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}
	//������Ϣ
	CT_VOID DisposeNetMessage(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len);
	//�߳���Ϣ
	CT_VOID DisposeThreadMessage(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len);

private:
	//����
	CT_VOID DisposeOpenBattleArray(CT_CHAR* pBuf,CT_DWORD len);
	//����Ϣ
	CT_VOID DisposeBattleArrayInfo(CT_CHAR* pBuf,CT_DWORD len);

private:
	//����֤
	CT_BOOL	ArmyValidate(CCUser *pUser,CT_INT32 iPosition[POST_COUNT/2]);
};
#endif