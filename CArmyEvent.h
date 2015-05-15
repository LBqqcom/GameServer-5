#ifndef CCARMYEVNET_H_
#define CCARMYEVNET_H_
#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCArmyEvent:public CCGameMesEvent
{
public:
	CCArmyEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCArmyEvent(void);

private:
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}
	//网络消息
	CT_VOID DisposeNetMessage(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len);

private:
	//军队升级消息
	CT_VOID DisposeArmyGrade(CT_CHAR* pBuf,CT_DWORD len);
	//军队进阶消息
	CT_VOID DisposeArmyAdvanced(CT_CHAR* pBuf,CT_DWORD len);

private:
	//军队升级
	CT_BOOL	ArmyGradeValidate(CCUser *pUser,CT_INT32 iArmyID);
	//军队进阶
	CT_BOOL	ArmyAdvancedValidate(CCUser *pUser,CT_INT32 iArmyID);
};
#endif