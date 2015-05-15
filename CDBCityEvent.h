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
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
								CT_CHAR* pBuf,		CT_DWORD len,
								CT_LONGLONG reserve);

	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID){}
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}
private:
	//处理建筑事件
	CT_VOID	DisposeBuildingEvent(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
	//处理建筑奖励
	CT_VOID DisposeBuildingEventAward(const MSC_DB_EventAward& eventAward);
	//更新公告信息
	CT_VOID UpdataAnnouncement();

};

#endif
