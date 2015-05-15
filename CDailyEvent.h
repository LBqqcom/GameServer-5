/********************************************************************
	created:	2012/11/14
	created:	14:11:2012   15:18
	file base:	CDailyEvent
	file ext:	h
	author:		xingweizhen
	
	purpose:	
*********************************************************************/
#ifndef CDAILYEVENT_H_
#define CDAILYEVENT_H_
#include "CGameMesEvent.h"
#include "CUserManager.h"

class CCDailyEvent : public CCGameMesEvent
{
public:
    CCDailyEvent(CCGameMesManager* pMesManager, CT_DWORD id);
    ~CCDailyEvent(void);
private:
    //�����û���Ϣ
    virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID, CT_CHAR* pBuf,		CT_DWORD len, CT_LONGLONG reserve);
    //�û���ʱ������
    virtual CT_VOID OnTime(CT_DWORD timeID);
    //�û�ʱ�Ӵ���
    virtual CT_VOID OnClock(CT_DWORD timeID);
private:
    //�����ճ��¼�
    CT_VOID DisposeDailyEvent(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
private:
    // ������ȡǩ������
    CT_VOID DisposeSignReward(const CMD_G_SignReward &reward);
    // ������ȡ�ճ�����
    CT_VOID DisposeDailyReward(const CMD_G_ActiveReward &reward);
};
#endif // CDAILYEVENT_H_