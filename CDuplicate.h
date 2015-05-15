/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CSkill.h
Version:		1.0
Description:    ������
Author:			wangchen
Create Date:	2012-9-13
History:             

******************************************************************/
#ifndef CCDUPLICATE_H_
#define CCDUPLICATE_H_

#include "CTType.h"
#include "GlobeScriptInterface.h"

#define STARCOUNT					5			//�Ǽ���Ŀ

class CCMoster;

//������Ϣ
struct DuplicateInfo
{
	CT_INT32		iWorldSceneID;					//����ID
	CT_INT32		iSceneID;						//����ID
	CT_INT32		iEntranceResID;					//�����Դ
	CT_INT32		iMosterGroupID;					//������ID	
	CT_INT32		iOpenCondition;					//��������
	CT_INT32		iFood;							//ʳ������
	CT_INT32		iPphysical;						//��������
	CT_INT32		iGoods;							//��Ʒ����
	CT_INT32		iCount;							//��Ʒ����
	CT_INT32		iStarLevelID;					//�Ǽ�ID
	CT_INT32		iReward;						//����
};

class CCDuplicate
{
	//������Ϣ
	DuplicateInfo			m_DuplicateInfo;
	CT_INT32				m_iMosterGrop;			//���ﲨ��

public:
	CCDuplicate(void);
	~CCDuplicate(void);

public:
	//��ȡ��������
	CT_VOID GetSceneReward(G_SceneReward &SceneReward);
	//��ȡ��������Ϣ
	CT_INT32 GetMosterInfo(G_MonsterBaseInfo &MonsterInfo,CT_INT32 iMosterNum);
	//��ȡ�¼���������Ϣ
	CT_INT32 GetEventMosterInfo(G_MonsterBaseInfo &MonsterInfo,CT_INT32 iMosterNum,CT_INT32 iMosterGroupID);
	//���ظ���
	CT_VOID LoadDuplicate(CT_INT32 iWorldSceneID,CT_INT32 iSceneID);
	//����PVP
	CT_VOID LoadPVP(CT_INT32 iDuplicateID);
	//���ò���
	CT_VOID SetMosterGrop(){m_iMosterGrop++;};
	//��ȡ����
	CT_INT32 GetMosterGrop(){return 1;/*m_DuplicateInfo.iMonsterCount;*/};
};

#endif
