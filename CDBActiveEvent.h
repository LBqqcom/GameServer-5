#ifndef CDBActiveEvent_H_
#define CDBActiveEvent_H_

#include "CGameMesEvent.h"

class CCGameMesManager;
class CCDBConnector;
class CCDBActiveEvent : public CCGameMesEvent
{
private:
	CCDBConnector &m_dbConnector;
public:
	CCDBActiveEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCDBActiveEvent();
private:
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID, CT_CHAR* pBuf,		CT_DWORD len, CT_LONGLONG reserve);
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID);
private:
	CT_VOID UpdateActiveStatus(MSC_DB_ActiveEventStatus &activeStatus);
	//�¿����Ҫ�콱����Լ������������
	CT_VOID UpdateActiveStatusEx(MSC_DB_ActiveEventStatus &activeStatus);
	CT_VOID	UpdateServerActive();  //���·������
	CT_VOID	UpdateTurntable(MSC_DB_Turntable &turntable);  //����ת��
	CT_VOID UpdateGMList();

	CT_VOID	GetCDKEY(MSC_DB_CDKEY &cdkey);
	CT_VOID	UpdateCDKEY(MSC_DB_CDKEY &cdkey);

	//ͬ���������� ��������
	CT_VOID SysServerActive(CT_CHAR* pBuf, CT_DWORD len);
	CT_VOID	DisposeResReward( MSC_DB_RESREWARD &resReward);
};

#endif