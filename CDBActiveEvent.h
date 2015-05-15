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
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID);
private:
	CT_VOID UpdateActiveStatus(MSC_DB_ActiveEventStatus &activeStatus);
	//月卡活动需要领奖后可以继续增加完成量
	CT_VOID UpdateActiveStatusEx(MSC_DB_ActiveEventStatus &activeStatus);
	CT_VOID	UpdateServerActive();  //更新服务器活动
	CT_VOID	UpdateTurntable(MSC_DB_Turntable &turntable);  //更新转盘
	CT_VOID UpdateGMList();

	CT_VOID	GetCDKEY(MSC_DB_CDKEY &cdkey);
	CT_VOID	UpdateCDKEY(MSC_DB_CDKEY &cdkey);

	//同步数据数据 彭文整理
	CT_VOID SysServerActive(CT_CHAR* pBuf, CT_DWORD len);
	CT_VOID	DisposeResReward( MSC_DB_RESREWARD &resReward);
};

#endif