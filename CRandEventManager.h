/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCRandManager.h
Version:		1.0
Description:    ����¼�����
Author:			pengwen
Create Date:	2012-8-16
History:        ��������¼�   

******************************************************************/
#ifndef CCRANDEVENTMANAGER_H_
#define CCRANDEVENTMANAGER_H_
#include <vector>
#include "CTType.h"
#include "CSingleton.h"
#include "CEventScriptSetting.h"
#include "CGDefine.h"

//����¼�
struct RandEvent
{
	int				 iProbabilityValue;   //���������ֵ
	ENUM_EventType   iEventType;          //�¼�����
	int				 iIndex;			  //�¼�����
	int				 iEventID;            //�¼�id
	float			 fProbability;        //�¼���������
};

class CCRandEventManager : public CCSingleton<CCRandEventManager>
{
	typedef std::vector<RandEvent>  RandEventVec;
	RandEventVec                    m_randEventVec;
public:
	CCRandEventManager(void);
	~CCRandEventManager(void);
public:
	//�õ�һ������¼�
	CT_BOOL GetOneEvent(RandEvent& re);

private:
	//�õ��ܵĸ���
	CT_VOID GetTotalProbability(CT_FLOAT& fNum);
	//������ʷֲ�
	CT_VOID CalProbability(CT_FLOAT& fNum);
	//�����������
	CT_VOID SaveProbability(CT_INT32& iBaseProbability, const G_NormalEventProbability& nep, float fNum, ENUM_EventType et);
	//�����������
	CT_VOID SaveProbability(CT_INT32& iBaseProbability, const G_BuildingEventProbability& nep, float fNum, ENUM_EventType et);
	//�����������
	CT_VOID SaveProbability(CT_INT32& iBaseProbability, const G_FightEventProbablity& nep, float fNum, ENUM_EventType et);

private:
	//���㽨������
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