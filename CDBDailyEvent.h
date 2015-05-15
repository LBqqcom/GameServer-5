/********************************************************************
	created:	2012/11/14
	created:	14:11:2012   15:17
	file base:	CDBDailyEvent
	file ext:	h
	author:		xingweizhen
	
	purpose:	
*********************************************************************/
#ifndef CDBDAILYEVENT_H_
#define CDBDAILYEVENT_H_

#include "CGameMesEvent.h"

class CCDBConnector;
class CCGameMesManager;
struct MSC_DB_W_SignInfo;
struct MSC_DB_W_DailyQuest;
struct MSC_DB_W_ActiveInfo;
struct MSC_DB_W_SignReward;
struct MSC_DB_W_ActiveReward;
struct MSC_DB_D_SignInfo;
struct MSC_DB_D_DailyInfo;

class CCDBDailyEvent : public CCGameMesEvent
{
private:
    CCDBConnector& m_dbConnector;
public:
    CCDBDailyEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
    ~CCDBDailyEvent(void);
private:
    //�����û���Ϣ
    virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID, CT_CHAR* pBuf,		CT_DWORD len, CT_LONGLONG reserve);
    //�û���ʱ������
    virtual CT_VOID OnTime(CT_DWORD timeID){  }
    //�û�ʱ�Ӵ���
    virtual CT_VOID OnClock(CT_DWORD timeID){  }

    // ������Ϣ����
    CT_VOID DisposeSaveData(CT_DWORD subID, CT_CHAR* pBuf,	CT_DWORD len);
    // ������Ϣɾ��
    CT_VOID DisposeDelData(CT_DWORD subID, CT_CHAR* pBuf,	CT_DWORD len);
private:
    // ��¼ǩ����Ϣ
    CT_VOID DisposeSaveData(const MSC_DB_W_SignInfo &info);
    // ��¼�ճ�����
    CT_VOID DisposeSaveData(const MSC_DB_W_DailyQuest &daily);
    // ��¼��Ծ����Ϣ
    CT_VOID DisposeSaveData(const MSC_DB_W_ActiveInfo &info);

    // ��¼��ȡ��Ծ�Ƚ���
    CT_VOID DisposeSaveData(const MSC_DB_W_ActiveReward &reward);

    // ���������½��¼
    CT_VOID DisposeDelData(const MSC_DB_D_SignInfo &info);
    // ����ճ���¼
    CT_VOID DisposeDelData(const MSC_DB_D_DailyInfo &info);
};
#endif // CDBDAILYEVENT_H_