/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCRandManager.h
Version:		1.0
Description:    随机事件服务
Author:			pengwen
Create Date:	2012-8-16
History:        处理登入事件   

******************************************************************/
#ifndef CCRANDEVENTMANAGER_H_
#define CCRANDEVENTMANAGER_H_
#include <vector>
#include "CTType.h"
#include "CSingleton.h"
#include "CEventScriptSetting.h"
#include "CGDefine.h"

//随机事件
struct RandEvent
{
	int				 iProbabilityValue;   //修正后概率值
	ENUM_EventType   iEventType;          //事件类型
	int				 iIndex;			  //事件索引
	int				 iEventID;            //事件id
	float			 fProbability;        //事件发生概率
};

class CCRandEventManager : public CCSingleton<CCRandEventManager>
{
	typedef std::vector<RandEvent>  RandEventVec;
	RandEventVec                    m_randEventVec;
public:
	CCRandEventManager(void);
	~CCRandEventManager(void);
public:
	//得到一个随机事件
	CT_BOOL GetOneEvent(RandEvent& re);

private:
	//得到总的概率
	CT_VOID GetTotalProbability(CT_FLOAT& fNum);
	//计算概率分布
	CT_VOID CalProbability(CT_FLOAT& fNum);
	//保存随机数据
	CT_VOID SaveProbability(CT_INT32& iBaseProbability, const G_NormalEventProbability& nep, float fNum, ENUM_EventType et);
	//保存随机数据
	CT_VOID SaveProbability(CT_INT32& iBaseProbability, const G_BuildingEventProbability& nep, float fNum, ENUM_EventType et);
	//保存随机数据
	CT_VOID SaveProbability(CT_INT32& iBaseProbability, const G_FightEventProbablity& nep, float fNum, ENUM_EventType et);

private:
	//计算建筑概率
	CT_VOID GetBuildingProbability(CT_FLOAT& fNum, CT_INT32 iType)
	{
		CT_INT32 iMaxIndex = CCEventScriptSetting::getSingleton().GetBuildingEventNum();
		for (CT_INT32 i=1; i<=iMaxIndex; ++i)
		{
			G_BuildingEventProbability gNp = {0};
			if(CCEventScriptSetting::getSingleton().GetBuildingEvent(i, gNp))
			{
				fNum += gNp.fProbability;
			}
		}
	}
	
};


#endif