#ifndef CCDBCITYEVENT_H_
#define CCDBCITYEVENT_H_
#include "CSYSType.h"
#include "CTType.h"
#include "CGameMesEvent.h"

class CCDBConnector;
struct MSC_DB_EventAward;
class CCDBCityEvent :
	public CCGameMesEvent
{
	CCDBConnector& m_dbConnector;
private:
	CCDBCityEvent();
public:
	CCDBCityEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCDBCityEvent(void);

private:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
								CT_CHAR* pBuf,		CT_DWORD len,
								CT_LONGLONG reserve);

	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID){}
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}
private:
	//�������¼�
	CT_VOID	DisposeBuildingEvent(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
	//����������
	CT_VOID DisposeBuildingEventAward(const MSC_DB_EventAward& eventAward);
	//���¹�����Ϣ
	CT_VOID UpdataAnnouncement();

};

#endif
