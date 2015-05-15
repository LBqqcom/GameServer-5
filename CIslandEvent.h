/****************************************************
	created:	2014/07/10		10:06
	file base:	IslandEvent
	file ext:	h
	author:		zhouhuan

	purpose:
*****************************************************/

#ifndef CISLANDEVENT_H
#define CISLANDEVENT_H

#include "CGameMesEvent.h"

class CCIslandEvent : public CCGameMesEvent
{
public:
	CCIslandEvent(CCGameMesManager* pMsg, CT_DWORD iID);
	~CCIslandEvent();

private:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve);
	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID);

private:
	// ����Ͷ������
	CT_VOID DisposeThrow(CT_CHAR* pBuf, CT_INT32 iLen);
	// �����¼�
	CT_VOID DisposeFightEvent(CT_CHAR* pBuf, CT_INT32 iLen);
	// �����������
	CT_VOID DisposeCompleteImmediately(CT_CHAR* pBuf, CT_INT32 iLen);
	// ��������¼�
	CT_VOID DisposeEventGiveup(CT_CHAR* pBuf, CT_INT32 iLen);
	// ��������ȡ
	CT_VOID DisposeReward(CT_CHAR* pBuf, CT_INT32 iLen);
	// ����Ͷ����������
	CT_VOID DisposeGiveThrowTimes(CT_CHAR* pBuf, CT_INT32 iLen);
	// ����ս���¼����
	CT_VOID DisposeFightOver(CT_CHAR* pBuf, CT_INT32 iLen);
	// ������������ʧ��
	CT_VOID DisposeOutLineGiveFail(CT_CHAR* pBuf, CT_INT32 iLen);
	// ������������
	CT_VOID DisposeOpenIsland(CT_DWORD dwUserID);
	// �����������
	CT_VOID DisposeTaskComplete(CT_CHAR* pBuf, CT_INT32 iLen);

};

#endif 