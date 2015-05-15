#ifndef CDBTaskEvent_H_
#define CDBTaskEvent_H_

#include "CGameMesEvent.h"
struct TaskFinish;

class CCGameMesManager;
class CCDBConnector;

class CCDBTaskEvent : public CCGameMesEvent
{
private:
	CCDBConnector &m_dbConnector;
public:
	CCDBTaskEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCDBTaskEvent();
private:
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID, CT_CHAR* pBuf,		CT_DWORD len, CT_LONGLONG reserve);
	virtual CT_VOID OnTime(CT_DWORD timeID){}
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{  }
private:
	CT_VOID	UpdateTaskProgressData(MSC_DB_TaskProgressData &taskProgressData);
	CT_VOID UpdateTaskFinishData(TaskFinish &taskReward);
};

#endif