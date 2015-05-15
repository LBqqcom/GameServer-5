/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CSkill.h
Version:		1.0
Description:    副本类
Author:			wangchen
Create Date:	2012-9-13
History:             

******************************************************************/
#ifndef CCDUPLICATE_H_
#define CCDUPLICATE_H_

#include "CTType.h"
#include "GlobeScriptInterface.h"

#define STARCOUNT					5			//星级数目

class CCMoster;

//副本信息
struct DuplicateInfo
{
	CT_INT32		iWorldSceneID;					//场景ID
	CT_INT32		iSceneID;						//副本ID
	CT_INT32		iEntranceResID;					//入口资源
	CT_INT32		iMosterGroupID;					//怪物组ID	
	CT_INT32		iOpenCondition;					//开启条件
	CT_INT32		iFood;							//食物消耗
	CT_INT32		iPphysical;						//体力消耗
	CT_INT32		iGoods;							//物品消耗
	CT_INT32		iCount;							//物品数量
	CT_INT32		iStarLevelID;					//星级ID
	CT_INT32		iReward;						//奖励
};

class CCDuplicate
{
	//副本信息
	DuplicateInfo			m_DuplicateInfo;
	CT_INT32				m_iMosterGrop;			//怪物波数

public:
	CCDuplicate(void);
	~CCDuplicate(void);

public:
	//获取场景奖励
	CT_VOID GetSceneReward(G_SceneReward &SceneReward);
	//获取怪物组信息
	CT_INT32 GetMosterInfo(G_MonsterBaseInfo &MonsterInfo,CT_INT32 iMosterNum);
	//获取事件怪物组信息
	CT_INT32 GetEventMosterInfo(G_MonsterBaseInfo &MonsterInfo,CT_INT32 iMosterNum,CT_INT32 iMosterGroupID);
	//加载副本
	CT_VOID LoadDuplicate(CT_INT32 iWorldSceneID,CT_INT32 iSceneID);
	//加载PVP
	CT_VOID LoadPVP(CT_INT32 iDuplicateID);
	//设置波数
	CT_VOID SetMosterGrop(){m_iMosterGrop++;};
	//获取波数
	CT_INT32 GetMosterGrop(){return 1;/*m_DuplicateInfo.iMonsterCount;*/};
};

#endif
