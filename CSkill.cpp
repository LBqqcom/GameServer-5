#include "CSkill.h"
#include "CFightingScriptSetting.h"
#include "GlobeScriptInterface.h"
//#include "CGlobeData.h"
#include "CMD_Game.h"
#include "CFightingEvnet.h"

template<> CCSkill* CCSingleton<CCSkill>::ms_Singleton=0;
CCSkill::CCSkill(void)
{
}

CCSkill::~CCSkill(void)
{
}

//获取法力消耗
CT_INT32 CCSkill::GetMpCost(CT_INT32 iSkillID)
{
	G_SkillMain Skill;
	memset(&Skill,0,sizeof(Skill));

	CCFightingScriptSetting::getSingleton().GetSkillMainData(iSkillID,Skill);
	return Skill.iMpCost == 0?-1:Skill.iMpCost;
}

//攻击位置
CT_INT32 CCSkill::GetAttickPos(CT_INT32 iSkillID,CT_INT32 *iPosition,CT_INT32 iMePosition,FightingUserInfo *UserInfo)
{
	G_SkillMain SkillMain={0};
	G_SkillAffect SkillAffect={0};
	CT_INT32 iCamp=(iMePosition>=POST_COUNT/2)?1:0;//0:1-5 1:6-10

	CCFightingScriptSetting::getSingleton().GetSkillAffectData(iSkillID,SkillAffect);
	
	if(SkillAffect.iSelf != 0)							//目标自己
	{
		iPosition[iMePosition]=SkillAffect.iSelf;
	}

	if(iCamp == 1)										//友方前军
	{
		//友方前军
		if(SkillAffect.iAllyFront1 != 0)
			iPosition[5]=SkillAffect.iAllyFront1;
		if(SkillAffect.iAllyFront2 != 0)
			iPosition[6]=SkillAffect.iAllyFront2;

		//敌方前军
		if(SkillAffect.iEnemyFront1 != 0)	
			iPosition[0]=SkillAffect.iEnemyFront1;
		if(SkillAffect.iEnemyFront2 != 0)	
			iPosition[1]=SkillAffect.iEnemyFront2;

		//友方后军
		if(SkillAffect.iAllyBack4 != 0)	
			iPosition[8]=SkillAffect.iAllyBack4;
		if(SkillAffect.iAllyBack5 != 0)	
			iPosition[9]=SkillAffect.iAllyBack5;

		//敌方后军
		if(SkillAffect.iEnemyBack4 != 0)
			iPosition[3]=SkillAffect.iEnemyBack4;
		if(SkillAffect.iEnemyBack5 != 0)
			iPosition[4]=SkillAffect.iEnemyBack5;
	}
	else
	{
		//友方前军
		if(SkillAffect.iAllyFront1 != 0)
			iPosition[0]=SkillAffect.iAllyFront1;
		if(SkillAffect.iAllyFront2 != 0)
			iPosition[1]=SkillAffect.iAllyFront2;

		//敌方前军
		if(SkillAffect.iEnemyFront1 != 0)	
			iPosition[5]=SkillAffect.iEnemyFront1;
		if(SkillAffect.iEnemyFront2 != 0)	
			iPosition[6]=SkillAffect.iEnemyFront2;

		//友方后军
		if(SkillAffect.iAllyBack4 != 0)	
			iPosition[3]=SkillAffect.iAllyBack4;
		if(SkillAffect.iAllyBack5 != 0)	
			iPosition[4]=SkillAffect.iAllyBack5;

		//敌方后军
		if(SkillAffect.iEnemyBack4 != 0)
			iPosition[8]=SkillAffect.iEnemyBack4;
		if(SkillAffect.iEnemyBack5 != 0)
			iPosition[9]=SkillAffect.iEnemyBack5;
	}

	if(SkillAffect.iAllyMiddle3 != 0)					//友方中军
	{
		if(iCamp == 0)
		{
			iPosition[2]=SkillAffect.iAllyMiddle3;
		}
		else
		{
			iPosition[7]=SkillAffect.iAllyMiddle3;
		}
	}

	if(SkillAffect.iEnemyMiddle3 != 0)					//敌方中军
	{
		if(iCamp == 1)
		{
			iPosition[2]=SkillAffect.iEnemyMiddle3;
		}
		else
		{
			iPosition[7]=SkillAffect.iEnemyMiddle3;
		}
	}


	if(SkillAffect.iFriendLeastHp != 0)					//剩余生命百分比最少的友军
	{
		CT_INT32 iMinHp=0;
		CT_INT32 iUserPos=0;
		if(iCamp == 0)
		{
			iMinHp=UserInfo[0].iHP/UserInfo[0].iHPMax;
			iUserPos=0;
			for(CT_INT32 i=1;i<POST_COUNT;++i)
			{
				
				if(UserInfo[i].iHP/UserInfo[i].iHPMax > iMinHp)
				{
					iMinHp=UserInfo[i].iHP/UserInfo[i].iHPMax;
					iUserPos=i;
				}
			}

			iPosition[iUserPos]=SkillAffect.iFriendLeastHp;
		}
		else
		{
			iMinHp=UserInfo[5].iHP/UserInfo[5].iHPMax;
			iUserPos=5;
			for(CT_INT32 i=POST_COUNT/2+1;i<POST_COUNT;++i)
			{

				if(UserInfo[i].iHP/UserInfo[i].iHPMax > iMinHp)
				{
					iMinHp=UserInfo[i].iHP/UserInfo[i].iHPMax;
					iUserPos=i;
				}
			}

			iPosition[iUserPos]=SkillAffect.iFriendLeastHp;
		}
	}

	if(SkillAffect.iRandomEnemy != 0)					//随机一个敌军
	{
		if(iCamp == 0)
		{
			while(true)
			{
				CT_INT32 iRand=rand()%(POST_COUNT/2)+POST_COUNT/2;
				if(UserInfo[iRand].iPosition != 0)
				{
					iPosition[iRand]=SkillAffect.iRandomEnemy;
					break;
				}
			}
		}
		else
		{
			while(true)
			{
				CT_INT32 iRand=rand()%(POST_COUNT/2);
				if(UserInfo[iRand].iPosition != 0)
				{
					iPosition[iRand]=SkillAffect.iRandomEnemy;
					break;
				}
			}
		}
	}

	return 0;
}