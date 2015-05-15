#ifndef CCBOSSEVNET_H_
#define CCBOSSEVNET_H_
#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCBossEvent:public CCGameMesEvent
{
	std::map<CT_INT32,CT_INT32>		m_ActiveMap;
public:
	CCBossEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCBossEvent(void);

private:
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID);
	//计算下次活动时间
	CT_INT64 NextActiveTime(CT_INT32 AcviteList[100],  CT_INT32 &iLastAEID);

	CT_VOID DisposeMAIN(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
	CT_VOID DisposeMSC(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);

	CT_VOID DisposeInfoUpdate(CT_CHAR* pBuf, CT_DWORD len);		//第一次打开BOSS活动页面
	CT_VOID DisposeJoinActive(CT_CHAR* pBuf, CT_DWORD len);		//加入当前BOSS活动
	CT_VOID DisposeExitActive(CT_CHAR* pBuf, CT_DWORD len);		//离开活动页面
	CT_VOID DisposeEncourage(CT_CHAR* pBuf, CT_DWORD len);		//激励
	CT_VOID DisposeBattle(CT_CHAR* pBuf, CT_DWORD len);			//战斗/复活
	CT_VOID DisposeReward(CT_CHAR* pBuf, CT_DWORD len);			//领取奖励
	CT_VOID DisposeCurrInfo(CT_CHAR* pBuf, CT_DWORD len);		//请求血量和排行

	CT_VOID DisposeFighting(CT_CHAR* pBuf, CT_DWORD len);		//战斗返回

public:
	CT_VOID DisposeActiveOpen();

};
#endif