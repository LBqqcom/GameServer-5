#ifndef CUserWorship_H_
#define CUserWorship_H_

#include <map>
#include <vector>
#include "CUser.h"
#include "CMD_Game.h"
#include "GlobeScriptInterface.h"

struct WorshipStatus
{
	CT_INT32	iXP;			//已祭祀经验
	CT_INT32	iStatus;		//是否完成
};

class CCUserWorship
{
	friend class CCUser;
private:
	CCUser	*m_pUser;
	CCMutex *m_pMutex;

	typedef  std::map<CT_INT32,WorshipStatus>			WorshipStatusMap;
	typedef  std::map<CT_INT32,WorshipStatus>::iterator			WorshipStatusMap_IT;

	WorshipStatusMap		m_WorshipStatusMap;
public:
	//初始化
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex)
	{
		m_pUser = pUser;
		m_pMutex = pMutex;
		m_WorshipStatusMap.clear();
	}

	//清理用户数据
	CT_VOID Clear()
	{
		_AUTOMUTEX(m_pMutex);
		m_WorshipStatusMap.clear();
	}
public:
	CCUserWorship();
	~CCUserWorship();
public:
	CT_BOOL			CheckAbilityToWorship(CT_INT32 iType);     //检查某类型的祭祀是否可以进行
	CT_VOID			UpdateWorshipStatus(CT_INT32 iType,CT_INT32 iXp);  //完成献祭，增加经验
	CT_BOOL			CheckWorshipComplete(CT_INT32 iType);		//检查祭祀是否完成
	CT_VOID			WorshipGetReward(CT_INT32 iType);			//祭祀领奖
	CT_VOID			GetWorshipByMasterLevel(CT_INT32 iLevel);	//根据领主等级，获取开启的祭祀
	CT_VOID			GetWorshipStatus(CT_INT32 iType,CT_INT32 iStatus,CT_INT32 iXp);  //用户登录获取祭祀信息
	CT_VOID			SendUserWorshipInfo();		//发送用户祭祀信息
	CT_BOOL			CheckWorshipCount(CT_INT32 iType);	//zhoujun	检查钻石祭祀次数
	CT_INT32		GetWorshipCount(CT_INT32 iType);
	CT_VOID			WorshipCount(CT_INT32 iType);
	CT_VOID			UpdateWorshipData();
};

#endif