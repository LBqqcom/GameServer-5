#ifndef CPetEvent_H_
#define CPetEvent_H_

#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCPetEvent:public CCGameMesEvent
{
public:
	CCPetEvent(CCGameMesManager *pMesManager,CT_WORD id);
	~CCPetEvent();
private:
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}

private:
	//线程消息
	CT_VOID DisposeMecMessage(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len);
	//网络消息
	CT_VOID DisposeNetMessage(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len);

private:
	//使用宠物蛋
	CT_VOID DisposeUseEgg(CT_CHAR *pBuf,CT_INT32 iLen);

private:
	//宠物升级
	CT_VOID DisposePetUpGrade(CT_CHAR *pBuf,CT_INT32 iLen);
	//宠物技能
	CT_VOID DisposePetSkill(CT_CHAR *pBuf,CT_INT32 iLen);
	//宠物幻化
	CT_VOID DisposePetChange(CT_CHAR* pBuf,CT_DWORD iLen);
	//宠物技能升级
	CT_VOID DisposePetSkillUpGrade(CT_CHAR* pBuf,CT_DWORD iLen);

private:
	//升级验证
	CT_INT32 ValidateUpGrade(CCUser *pUser,CMD_G_PetUpGrade &pug);
	//技能验证
	CT_INT32 ValidateSkill(CCUser *pUser,CMD_G_PetSkillEquip &pse);
	//幻化验证
	CT_INT32 ValidateChange(CCUser *pUser,CMD_G_PetChange &pc);
	//主动技能升级验证
	CT_INT32 ValidateSkillUpGrade(CCUser *pUser,CMD_G_PetSkillUpGrade &psug,RewardCost &rc);
};

#endif