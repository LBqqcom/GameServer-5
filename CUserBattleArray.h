/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CUserDuplicate.h
Version:		1.0
Description:    用户阵法数据
Author:			wangchen
Create Date:	2012-10-19
History:        用户阵法 

******************************************************************/
#ifndef CCUSERBATTLEARRAY_H_
#define CCUSERBATTLEARRAY_H_
#include <map>
#include "CGDefine.h"
#include "CMutex.h"
#include "CMD_Game.h"
#include "CUserBase.h"

////阵法信息
//struct MSC_DB_BattleArray
//{
//	CT_DWORD	dwUserID;				//用户ID
//	CT_INT32	iPosition[POST_COUNT/2];//位置
//};

class CCUser;

class CCUserBattleArray
{
    friend class CCUser;
private:
	CT_INT32	m_iPosition[POST_COUNT/2];		//阵法位置

private:
	CCUser*  m_pUser;
	CCMutex* m_pMutex;

public:
	CCUserBattleArray(void);
	~CCUserBattleArray(void);

public:
	//初始化
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex);
	//清理用户数据
	CT_VOID Clear();
public:
	//插入阵法
	CT_VOID InsertBufferData(CT_INT32 iPosition[POST_COUNT/2]);

public:
	//获取阵法
	CT_VOID GetBattleArray(CT_INT32 iPosition[POST_COUNT/2]);
	CT_BOOL GetBattleArray(CT_CHAR *pBuf,CT_INT32 &iLen);

public:
	//更新阵法
	CT_VOID	UpDataBattleArray(CT_INT32 iPosition[POST_COUNT/2]);

    // 更新阵法加成数据
    CT_VOID CalcArrayStatus(CT_INT32 iPosition);
	
	//判断兵种是否上阵
	CT_BOOL CheckArmyArray(CT_INT32 iArmyType);

public:
    static CT_VOID UpdateBattleStatus(MasterCombatStatus &mcs, CT_INT32 iClass, CT_INT32 *arrayBattle, CT_INT32 nCount);
};

#endif
