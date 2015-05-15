#ifndef CActiveEvent_H_
#define CActiveEvent_H_

#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCActiveEvent : public CCGameMesEvent
{
	std::map<CT_INT32,CT_INT32>		m_TimeActiveMap;	//TimeID, AEID
public:
	CCActiveEvent(CCGameMesManager *pMesManager,CT_WORD id);
	~CCActiveEvent();
private:
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID);
private:
	CT_VOID DisposeActiveReward(CT_CHAR* pBuf,CT_DWORD len);
	CT_VOID DisposeTurntableDraw(CT_CHAR* pBuf,CT_DWORD len);
	CT_INT32 GetPosByRandNum(CT_INT32 *iWeight,CT_INT32 begin,CT_INT32 end,CT_INT32 iRand);
	CT_VOID DisposeCDKEYReward(CT_CHAR* pBuf,CT_DWORD len);
	CT_VOID DisposeCDKEYGet(CT_CHAR* pBuf,CT_DWORD len);

	CT_VOID DisposePresent(CT_CHAR* pBuf,CT_DWORD len);
	CT_VOID DisposePresendAdd(CT_CHAR* pBuf,CT_DWORD len);

	CT_VOID DisposeTurntableRelation(CT_CHAR* pBuf,CT_DWORD len);
	CT_VOID DisposeRewardRelation(CT_CHAR* pBuf,CT_DWORD len);
	CT_VOID DisposeRewardComplete();
	CT_VOID DisposeDragonBall(CT_CHAR* pBuf, CT_DWORD len);
	CT_VOID DisposeDragonBallWish(CT_CHAR* pBuf, CT_DWORD len);
	//月卡续费
	CT_VOID DisposeMonthCardRenew(CT_CHAR* pBuf, CT_DWORD len);
	//月卡奖励
	CT_VOID DisposeMonthCardReward(CT_CHAR* pBuf, CT_DWORD len);

private:
	//得到活动
	CT_VOID DisposeActiveGet(CT_CHAR* pBuf,CT_DWORD len);
	//得到活动描述
	CT_VOID DisposeActiveDec(CT_CHAR* pBuf,CT_DWORD len);

	CT_VOID DisposeActiveOpen(CT_INT32 iActiveType);
	CT_INT64 NextActiveTime(CT_INT32 iAEType, CT_INT32 &iLastAEID);
	//年夜饭活动奖励
	CT_VOID YearDinnarReward(CT_INT32 iQuantity,G_ActiveEventReward &AER);

private:
	//月卡开启验证
	CT_INT32 ValidateMonthCard(CCUser *pUser);
	//月卡奖励
	CT_INT32 ValidateMonthCardReward(CCUser *pUser);
};

#endif