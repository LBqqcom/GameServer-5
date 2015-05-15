/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CUserDuplicate.h
Version:		1.0
Description:    用户组队副本
Author:			wangchen
Create Date:	2012-10-19
History:        用户副本    

******************************************************************/
#ifndef CCUSERTEAMDUPLICATE_H_
#define CCUSERTEAMDUPLICATE_H_
#include "CGDefine.h"
#include "CMutex.h"
#include "CMD_Game.h"
#include<map>

class CCUser;
class CCMutex;

//组队副本
struct TeamDuplicateScene
{
	CT_INT32		iSceneID;
	CT_INT32		iDifficulty[TEAMDUPLICATE_DIFFICULTY];
	CT_INT32		iWin[TEAMDUPLICATE_DIFFICULTY];	
};

class CCUserTeamDuplicate
{
private:
	typedef std::map<CT_INT32,TeamDuplicateScene>				TeamDuplicateMap;
	typedef std::map<CT_INT32,TeamDuplicateScene>::iterator		TD_IT;

private: 
	CT_INT32			m_iFightingTime;			//战斗次数
	TeamDuplicateMap	m_TeamDuplicateMap;			//组队副本场景

private:
	CCUser*  m_pUser;
	CCMutex* m_pMutex;
public:
	CCUserTeamDuplicate(void);
	~CCUserTeamDuplicate(void);

public:
	//初始化
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex);
	//清理用户数据
	CT_VOID Clear();

public: 
	//插入数据
	CT_VOID InsertSceneData(const CT_INT32 iSceneID,const CT_INT32 iDifficulty,CT_INT32 iWin);


public:
	//设置场景
	CT_BOOL SetSceneData(CT_INT32 iSceneID,CT_INT32 iDifficulty);

public:
	//获取场景信息
	CT_VOID GetSceneData(CT_CHAR *pBuf,CT_INT32 &iLen);

public:
	//验证关卡
	CT_BOOL VaildateScene(CT_INT32 iSceneID,CT_INT32 iDifficulty,CT_BOOL bUpdata=CT_FALSE);
	//更新场景信息
	CT_VOID UpdataScene(CT_INT32 iSceneID,CT_INT32 iDifficulty,CT_INT32 iWin);
	//更新奖励次数
	CT_VOID UpdataFightingReward();
};

#endif