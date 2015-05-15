/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCCityEvent.h
Version:		1.0
Description:    ��������¼�
Author:			pengwen
Create Date:	2012-9-13
History:        ��������¼�  �������¼��������¼����ȵ�   

******************************************************************/
#ifndef CCCITYEVENT_H_
#define CCCITYEVENT_H_
#include "CGameMesEvent.h"
#include "CGDefine.h"

class  CCUser;
struct CMD_G_ApplyAwardEvent;
struct CMD_G_EVENT;
struct MSC_Event_Fighting_End;
class CCCityEvent :
	public CCGameMesEvent
{
public:
	CCCityEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCCityEvent(void);
private:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve);
	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID);

private:
	//��������¼�
	CT_VOID  DisposeCityEvent(CT_DWORD subID,  CT_CHAR* pBuf, CT_DWORD len);
	//��������¼�
	CT_VOID  DisposeMscCityEvent(CT_DWORD subID,  CT_CHAR* pBuf, CT_DWORD len);
private:
	//�����û�����
	CT_VOID DisposeApplyAwardEvent(const CMD_G_ApplyAwardEvent& awardEvent);
	//�����û�����
	CT_VOID DisposeApplyAwardEvent(CCUser* pUser, CT_INT32 eventID,  CT_INT32 index,  CT_INT32 awardID, ENUM_EventType et);
	
private:
	//����ս������
	CT_VOID DisposeFightEventAward(CCUser* pUser, const CMD_G_ApplyAwardEvent& awardEvent);
	//����������
	CT_VOID DisposeBuildingEventAward(CCUser* pUser, const CMD_G_ApplyAwardEvent& awardEvent);
	//�����˽���
	CT_VOID DisposeWorkerEventAward(CCUser* pUser, const CMD_G_ApplyAwardEvent& awardEvent);
	//������ｱ��
	CT_VOID DisposePetEventAward(CCUser* pUser, const CMD_G_ApplyAwardEvent& awardEvent);
	//�����佱��
	CT_VOID DisposeTreasureEventAward(CCUser* pUser, const CMD_G_ApplyAwardEvent& awardEvent);
	//����
	CT_VOID DisposeHeartBeatEvent(CT_CHAR* pBuf, CT_DWORD len);
	//ϵͳ�㲥
	CT_VOID DisposeBroadcast(CT_CHAR* pBuf, CT_DWORD len);
	//��������
	CT_VOID DisposeBroadcastWorldMessage(CT_CHAR* pBuf, CT_DWORD len);
	//���¹���
	CT_VOID DisposeAnnouncementUpdata(CT_CHAR* pBuf, CT_DWORD len);


private:
	//����ս���¼�
	CT_VOID DisposeMscFightingEventEnd(const MSC_Event_Fighting_End &FightingEventEnd);
};

#endif
