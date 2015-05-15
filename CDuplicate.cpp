#include "CDuplicate.h"
#include "CEventScriptSetting.h"
#include "CMoster.h"
#include "CLogMeg.h"
#include "CFightingScriptSetting.h"

CCDuplicate::CCDuplicate(void)
{
	m_iMosterGrop=0;
	memset(&m_DuplicateInfo,0,sizeof(m_DuplicateInfo));
}

CCDuplicate::~CCDuplicate(void)
{
}

//���ظ���
CT_VOID CCDuplicate::LoadDuplicate(CT_INT32 iWorldSceneID,CT_INT32 iSceneID)
{
	//��ȡ�ű�
	G_FightingScene FightingScene;
	memset(&FightingScene,0,sizeof(FightingScene));
	if(CCFightingScriptSetting::getSingleton().GetFightingScene(iWorldSceneID,iSceneID,FightingScene) == false)
	{
		CCLogMeg::getSingleton().WriteError("��ȡ�����ű�ʧ�ܣ�");
		return;
	}



	//��������
	m_DuplicateInfo.iWorldSceneID=FightingScene.iWorldSceneID;
	m_DuplicateInfo.iSceneID=FightingScene.iSceneID;
	m_DuplicateInfo.iOpenCondition=FightingScene.iOpenCondition;
	m_DuplicateInfo.iMosterGroupID=FightingScene.iMosterGroupID;
	m_DuplicateInfo.iOpenCondition=FightingScene.iOpenCondition;
	m_DuplicateInfo.iFood=FightingScene.iFood;
	m_DuplicateInfo.iPphysical=FightingScene.iPphysical;
	m_DuplicateInfo.iGoods=FightingScene.iGoods;
	m_DuplicateInfo.iCount=FightingScene.iCount3;
	m_DuplicateInfo.iStarLevelID=FightingScene.iStarLevelID;
	m_DuplicateInfo.iReward=FightingScene.iReward;


	//���ñ���
	m_iMosterGrop=0;


	return;
}

//����PVP
CT_VOID CCDuplicate::LoadPVP(CT_INT32 iDuplicateID)
{
	return;
}

//��ȡ��������
CT_VOID CCDuplicate::GetSceneReward(G_SceneReward &SceneReward)
{
	CCFightingScriptSetting::getSingleton().GetSceneReward(m_DuplicateInfo.iWorldSceneID,m_DuplicateInfo.iSceneID,SceneReward);
	return;
}

//��ȡ��������Ϣ
CT_INT32 CCDuplicate::GetMosterInfo(G_MonsterBaseInfo &MonsterInfo,CT_INT32 iMosterNum)
{
	//��ȡ������ű�
	G_MonsterGroup MonsterGroup;
	memset(&MonsterGroup,0,sizeof(MonsterGroup));
	if(CCFightingScriptSetting::getSingleton().GetMonsterGroup(m_DuplicateInfo.iWorldSceneID,m_DuplicateInfo.iMosterGroupID,MonsterGroup) == false)
	{
		CCLogMeg::getSingleton().WriteError("��ȡ������ű�ʧ�ܣ�");
		return 0;
	}

	//��ȡ������ű�
	switch(iMosterNum)
	{
	case 0:
		{
			if(MonsterGroup.iBeforeMoster1 == 0)break;
			if(CCFightingScriptSetting::getSingleton().GetMosterData(MonsterGroup.iBeforeMoster1,MonsterInfo) == false)
			{
				CCLogMeg::getSingleton().WriteError("��ȡǰ����");
			}
			else
				return MonsterGroup.iBeforeMoster1;
			break;
		}
	case 1:
		{
			if(MonsterGroup.iBeforeMoster2 == 0)break;
			if(CCFightingScriptSetting::getSingleton().GetMosterData(MonsterGroup.iBeforeMoster2,MonsterInfo) == false)
			{
				CCLogMeg::getSingleton().WriteError("��ȡǰ����");
			}
			else
				return MonsterGroup.iBeforeMoster2;
			break;
		}
	case 2:
		{
			if(MonsterGroup.iMediumMoster == 0)break;
			if(CCFightingScriptSetting::getSingleton().GetMosterData(MonsterGroup.iMediumMoster,MonsterInfo) == false)
			{
				CCLogMeg::getSingleton().WriteError("��ȡ������");
			}
			else
				return MonsterGroup.iMediumMoster;
			break;
		}
	case 3:
		{
			if(MonsterGroup.iAfterMoster1 == 0)break;
			if(CCFightingScriptSetting::getSingleton().GetMosterData(MonsterGroup.iAfterMoster1,MonsterInfo) == false)
			{
				CCLogMeg::getSingleton().WriteError("��ȡ������");
			}
			else
				return MonsterGroup.iAfterMoster1;
			break;
		}
	case 4:
		{
			if(MonsterGroup.iAfterMoster2 == 0)break;
			if(CCFightingScriptSetting::getSingleton().GetMosterData(MonsterGroup.iAfterMoster2,MonsterInfo) == false)
			{
				CCLogMeg::getSingleton().WriteError("��ȡ������");
			}
			else
				return MonsterGroup.iAfterMoster2;
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("��ȡ����ʧ�ܣ�");
			return 0;
		}
	}

	return 0;
}

//��ȡ�¼���������Ϣ
CT_INT32 CCDuplicate::GetEventMosterInfo(G_MonsterBaseInfo &MonsterInfo,CT_INT32 iMosterNum,CT_INT32 iMosterGroupID)
{
	//��ȡ������ű�
	G_MonsterGroup MonsterGroup;
	memset(&MonsterGroup,0,sizeof(MonsterGroup));
	if(!CCEventScriptSetting::getSingleton().GetEventMonsterGroup(iMosterGroupID,MonsterGroup))
	{
		CCLogMeg::getSingleton().WriteError("��ȡ�¼�������ű�ʧ�ܣ�");
		return 0;
	}

	//��ȡ������ű�
	switch(iMosterNum)
	{
	case 0:
		{
			if(MonsterGroup.iBeforeMoster1 == 0)break;
			if(CCFightingScriptSetting::getSingleton().GetMosterData(MonsterGroup.iBeforeMoster1,MonsterInfo) == false)
			{
				CCLogMeg::getSingleton().WriteError("��ȡǰ����");
			}
			else
				return MonsterGroup.iBeforeMoster1;
			break;
		}
	case 1:
		{
			if(MonsterGroup.iBeforeMoster2 == 0)break;
			if(CCFightingScriptSetting::getSingleton().GetMosterData(MonsterGroup.iBeforeMoster2,MonsterInfo) == false)
			{
				CCLogMeg::getSingleton().WriteError("��ȡǰ����");
			}
			else
				return MonsterGroup.iBeforeMoster2;
			break;
		}
	case 2:
		{
			if(MonsterGroup.iMediumMoster == 0)break;
			if(CCFightingScriptSetting::getSingleton().GetMosterData(MonsterGroup.iMediumMoster,MonsterInfo) == false)
			{
				CCLogMeg::getSingleton().WriteError("��ȡ������");
			}
			else
				return MonsterGroup.iMediumMoster;
			break;
		}
	case 3:
		{
			if(MonsterGroup.iAfterMoster1 == 0)break;
			if(CCFightingScriptSetting::getSingleton().GetMosterData(MonsterGroup.iAfterMoster1,MonsterInfo) == false)
			{
				CCLogMeg::getSingleton().WriteError("��ȡ������");
			}
			else
				return MonsterGroup.iAfterMoster1;
			break;
		}
	case 4:
		{
			if(MonsterGroup.iAfterMoster2 == 0)break;
			if(CCFightingScriptSetting::getSingleton().GetMosterData(MonsterGroup.iAfterMoster2,MonsterInfo) == false)
			{
				CCLogMeg::getSingleton().WriteError("��ȡ������");
			}
			else
				return MonsterGroup.iAfterMoster2;
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("��ȡ����ʧ�ܣ�");
			return 0;
		}
	}
	return 0;
}