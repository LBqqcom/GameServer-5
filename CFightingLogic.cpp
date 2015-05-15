#include "CFightingLogic.h"
#include "CFightingScriptSetting.h"
#include "GlobeScriptInterface.h"
#include "CLogMeg.h"
#include "CThreadMes.h"
#include "CUser.h"
#include "CUserManager.h"


CCFightingLogic::CCFightingLogic(void)
{
	m_cbRound=0;
	m_iWiner=WINER_NULL;
	m_iActionPos=0;
	m_iFightingScore=0;
	m_bOrder=CT_TRUE;

	memset(m_dwUserID,0,sizeof(m_dwUserID));
	memset(m_iActionSort,-1,sizeof(m_iActionSort));
	memset(m_iNoActionUser,-1,sizeof(m_iNoActionUser));
	memset(m_UserInfo,0,sizeof(m_UserInfo));
	memset(m_iMaxHp,0,sizeof(m_iMaxHp));
	m_FightingList.clear();

	G_ValueRange ValueRound={0};
	if(!G_GetValueRange(VALUE_TYPE_FIGHTING_ROUND,&ValueRound))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet::FigheingInfo()VALUE_TYPE_FIGHTING_ROUND");
	}
	m_iFightingRound=(CT_INT32)ValueRound.fMinVal;

	m_pRand=new CCRand;
}

CCFightingLogic::~CCFightingLogic(void)
{
	delete m_pRand;
	m_pRand=0;
}

//���ú���
CT_VOID CCFightingLogic::ResetVariable()
{
	if(m_iWiner == WINER_NULL)
	{
		m_cbRound=0;
		m_iWiner=WINER_NULL;
		m_iActionPos=0;
		m_iFightingScore=0;
		m_bOrder=CT_TRUE;

		memset(m_dwUserID,0,sizeof(m_dwUserID));
		memset(m_iActionSort,-1,sizeof(m_iActionSort));
		memset(m_iNoActionUser,-1,sizeof(m_iNoActionUser));
		memset(m_UserInfo,0,sizeof(m_UserInfo));
		memset(m_iMaxHp,0,sizeof(m_iMaxHp));
		m_FightingList.clear();
	}
	else if(m_iWiner == WINER_LIFT)
	{
		m_cbRound=0;
		m_iWiner=WINER_NULL;
		m_iActionPos=0;
		m_iFightingScore=0;
		m_dwUserID[1]=0;

		memset(m_iActionSort,-1,sizeof(m_iActionSort));
		memset(m_iNoActionUser,-1,sizeof(m_iNoActionUser));
		memset(m_UserInfo+POST_COUNT/2,0,sizeof(m_UserInfo[0])*POST_COUNT/2);

		//for(CT_INT32 i=0;i<POST_COUNT;++i)
		//{
		//	m_UserInfo[i].iMP=0;
		//}
		m_FightingList.clear();

	}
	else
	{
		m_cbRound=0;
		m_iWiner=WINER_NULL;
		m_iActionPos=0;
		m_iFightingScore=0;
		m_dwUserID[0]=0;

		memset(m_iActionSort,-1,sizeof(m_iActionSort));
		memset(m_iNoActionUser,-1,sizeof(m_iNoActionUser));
		memset(m_UserInfo,0,sizeof(m_UserInfo[0])*POST_COUNT/2);

	/*	for(CT_INT32 i=0;i<POST_COUNT;++i)
		{
			m_UserInfo[i].iMP=0;
		}*/
		m_FightingList.clear();
	}
}

//���ú���
CT_VOID CCFightingLogic::ResetVariable(CT_INT32 iWiner)
{
	if(iWiner == WINER_LIFT)
	{
		m_cbRound=0;
		m_iWiner=WINER_NULL;
		m_iActionPos=0;
		m_iFightingScore=0;
		m_dwUserID[1]=0;

		memset(m_iActionSort,-1,sizeof(m_iActionSort));
		memset(m_iNoActionUser,-1,sizeof(m_iNoActionUser));
		memset(m_UserInfo+POST_COUNT/2,0,sizeof(m_UserInfo[0])*POST_COUNT/2);
		memset(m_iMaxHp,0,sizeof(m_iMaxHp));
	}
	return;
}

//��ʼ��
CT_VOID	CCFightingLogic::Init(CT_INT32 iWiner)
{
	if(iWiner == WINER_NULL)
		m_iWiner=iWiner;

	ResetVariable();
	return;
}
//�����û���Ϣ
CT_VOID	CCFightingLogic::SetUserInfo(CT_INT32 iPosition,const FightingUserInfo &UserInfo)
{
	if(m_UserInfo[iPosition].dwUserID != 0)
	{
		CCLogMeg::getSingleton().WriteError("CCFightingLogic::SetUserInfo %s %d",__FILE__,__LINE__);
		return;
	}

	m_UserInfo[iPosition]=UserInfo;
	m_UserInfo[iPosition].iPosition=iPosition;
	m_iMaxHp[iPosition]=UserInfo.iHPMax;
	return;
}

//��ȡս������
CT_INT32 CCFightingLogic::GetFightingScore()
{
	//ս������
	if(m_iWiner == WINER_LIFT)
	{
		CT_FLOAT fHp=0.0f;
		CT_INT32 iMaxHp=0;
		CT_INT32 iHp=0;
		for(CT_INT32 i=0;i<POST_COUNT/2;++i)
		{
			if(m_iMaxHp[i] != 0)
			{
				iMaxHp+=m_iMaxHp[i];
				iHp+=m_UserInfo[i].iHP;
			}
		}

		fHp=(CT_FLOAT)(iHp)/iMaxHp;
		m_iFightingScore=G_GetFightingScore(m_cbRound,m_iFightingRound,fHp);
	}
	return m_iFightingScore;
}

//��������BOSS��Ϣ
CT_VOID CCFightingLogic::GetBossInfo(UserFightingStatus &FightingData)
{
	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		FightingData.units[i].mcs.iMaxHP=m_UserInfo[i+POST_COUNT/2].iHPMax;
		FightingData.units[i].mcs.iCurrentHP=m_UserInfo[i+POST_COUNT/2].iHP;
	}
	return;
}

//ս����Ϣ
CT_BOOL CCFightingLogic::FigheingInfo()
{
	//��������
	ActionSort(m_iActionSort,-1);

	while(CT_TRUE)
	{
		//��ȡ�����û�
		CT_INT32 iHurt=0;
		FightingSave FightingSaveInfo={0};
		FightingUserInfo AttackUserInfo={0};

		CT_INT32 iCurrentUser=GetActionUser();
		if(iCurrentUser == -1)
		{
			//�쳣���(ͬʱ����)
			FightingUserInfo fuinfo={0};
			m_iWiner=GetWiner(fuinfo,0);
			if(m_iWiner != WINER_NULL)
				return CT_TRUE;
			else
				return CT_FALSE;
		}
		memcpy(&AttackUserInfo,&m_UserInfo[iCurrentUser],sizeof(AttackUserInfo));

		FightingUserInfo fuinfo={0};
		m_iWiner=GetWiner(fuinfo,0);
		if(m_iWiner != WINER_NULL)
			return CT_TRUE;

		//�;��ж�
		if(m_iFightingRound < ROUNDD_MAX)
		{
			if(m_cbRound == ROUNDD_MAX)
			{
				m_iWiner=WINER_RIGHT;
				m_FightingList.push_back(FightingSaveInfo);
				return CT_TRUE;
			}
		}
		else
		{
			if(m_cbRound == m_iFightingRound)
			{
				m_iWiner=WINER_RIGHT;
				m_FightingList.push_back(FightingSaveInfo);
				return CT_TRUE;
			}
		}

		//���˼���״̬���޸����λ
		CT_INT32 iRound=0;
		for(CT_INT32 i=0;i<POST_COUNT;++i)
		{
			if(m_UserInfo[i].iHP != 0 && i < POST_COUNT/2)
			{
				iRound++;
				i=POST_COUNT/2;
			}
			if(m_UserInfo[i].iHP != 0 && i >= POST_COUNT/2)
			{
				iRound++;
				break;
			}
		}
		if(iRound < 2)
			continue;

		//������֤
		if(AttackUserInfo.iHP <= 0)
		{
			CT_ASSERT(CT_FALSE);
			printf("%s %d",__FILE__,__LINE__);
			return CT_FALSE;
		}

		//����״̬
		CT_INT32 iAttactUser=AttackUserInfo.iPosition;
#ifdef _SHOW_FIGHT_INFO
		printf("\n�غ�ǰ:%i λ��%i \t������HP:%i \t������MP:%i \n",m_cbRound,iAttactUser,AttackUserInfo.iHP,AttackUserInfo.iMP);
		printf("״̬1��%d\t״̬2��%d\t״̬3��%d\t״̬4��%d\t״̬5��%d\t״̬6��%d\t\n",
			AttackUserInfo.iSkillStatus[0],AttackUserInfo.iSkillStatus[1],AttackUserInfo.iSkillStatus[2],
			AttackUserInfo.iSkillStatus[3],AttackUserInfo.iSkillStatus[4],AttackUserInfo.iSkillStatus[5]);
		StatusCompute(m_UserInfo[iCurrentUser],AttackUserInfo,CT_TRUE);
		printf("�غϺ�:%i λ��%i \t������HP:%i \t������MP:%i \n",m_cbRound,iAttactUser,AttackUserInfo.iHP,AttackUserInfo.iMP);
#else
		StatusCompute(m_UserInfo[iCurrentUser],AttackUserInfo,CT_TRUE);
#endif



		//�ж�ǰ����
		if(AttackUserInfo.iHP == 0)
		{
			m_iWiner=GetWiner(m_UserInfo[AttackUserInfo.iPosition],-1);

			m_UserInfo[AttackUserInfo.iPosition].iFaint=0;
			m_UserInfo[AttackUserInfo.iPosition].iExpire=0;
			memset(&m_UserInfo[AttackUserInfo.iPosition].AddStatus[1],0,sizeof(m_UserInfo[AttackUserInfo.iPosition].AddStatus[0])*(STATUSEX-1));
			memset(&m_UserInfo[AttackUserInfo.iPosition].iHpEffect[1],0,sizeof(m_UserInfo[AttackUserInfo.iPosition].iHpEffect[0])*(STATUSEX-1));
			memset(&m_UserInfo[AttackUserInfo.iPosition].iMpEffect[1],0,sizeof(m_UserInfo[AttackUserInfo.iPosition].iMpEffect[0])*(STATUSEX-1));
			memset(&m_UserInfo[AttackUserInfo.iPosition].iSkillStatus[1],0,sizeof(m_UserInfo[AttackUserInfo.iPosition].iSkillStatus[0])*(STATUSEX-1));
			memset(&m_UserInfo[AttackUserInfo.iPosition].iStatusTime[1],0,sizeof(m_UserInfo[AttackUserInfo.iPosition].iStatusTime[0])*(STATUSEX-1));
			m_UserInfo[AttackUserInfo.iPosition].cbStatusCount=1;
			
			memset(&FightingSaveInfo,0,sizeof(FightingSaveInfo));
			FightingSaveInfo.cbRound=m_cbRound;
			FightingSaveInfo.cbAttackUser=AttackUserInfo.iPosition;
			FightingSaveInfo.cbTarget[FightingSaveInfo.cbAttackUser]=Dead;
			memcpy(FightingSaveInfo.iSkillStatusHP,AttackUserInfo.iHpEffect,sizeof(FightingSaveInfo.iSkillStatusHP));
			memcpy(FightingSaveInfo.iSkillStatusMP,AttackUserInfo.iMpEffect,sizeof(FightingSaveInfo.iSkillStatusMP));

			//��¼ս����Ϣ
			m_FightingList.push_back(FightingSaveInfo);

			SkillEquipAffect(Skill_Time_2,m_UserInfo[AttackUserInfo.iPosition]);

			//text
			//////////////////////////////////////////////////////////////////////////
#ifdef _SHOW_FIGHT_INFO
			TextInfo(FightingSaveInfo,AttackUserInfo.iPosition,0,m_iWiner);
			//CT_INT32 iAttactUser=AttackUserInfo.iPosition;
			//printf("�غ�:%i ������HP:%i ������MP:%i ������:%i Ŀ��:%i ����ID:%d �˺�:%i \n״̬1:%i ״̬2:%i ״̬3:%i ״̬4 :%i ״̬5:%i ״̬6:%i ʤ��id:%d\n",
			//	FightingSaveInfo.cbRound,m_UserInfo[iAttactUser].iHP,m_UserInfo[iAttactUser].iMP,AttackUserInfo.iPosition,0,FightingSaveInfo.cbAction,FightingSaveInfo.iReduceHP[iAttactUser],FightingSaveInfo.cbStatus[iAttactUser][0],
			//	FightingSaveInfo.cbStatus[iAttactUser][1],FightingSaveInfo.cbStatus[iAttactUser][2],FightingSaveInfo.cbStatus[iAttactUser][3],FightingSaveInfo.cbStatus[iAttactUser][4],
			//	FightingSaveInfo.cbStatus[iAttactUser][5],m_iWiner);
#endif
			//////////////////////////////////////////////////////////////////////////
		}

		memset(&fuinfo,0,sizeof(fuinfo));
		m_iWiner=GetWiner(fuinfo,0);
		if(m_iWiner != WINER_NULL)
			return CT_TRUE;

		//�����������
		if(AttackUserInfo.iHP <= 0)
			continue;

		//ѣ��
		if(AttackUserInfo.iFaint != 0)
		{
			FightingSaveInfo.cbRound=m_cbRound;
			FightingSaveInfo.cbAttackUser=AttackUserInfo.iPosition;
			memcpy(FightingSaveInfo.iSkillStatusHP,AttackUserInfo.iHpEffect,sizeof(FightingSaveInfo.iSkillStatusHP));
			memcpy(FightingSaveInfo.iSkillStatusMP,AttackUserInfo.iMpEffect,sizeof(FightingSaveInfo.iSkillStatusMP));

			for(CT_BYTE j=0;j<POST_COUNT;++j)
			{
				for(CT_BYTE z=0;z<STATUSEX;++z)
					FightingSaveInfo.cbStatus[j][z]=m_UserInfo[j].iSkillStatus[z];
			}

			//��¼ս����Ϣ
			m_FightingList.push_back(FightingSaveInfo);

			//text
			//////////////////////////////////////////////////////////////////////////
#ifdef _SHOW_FIGHT_INFO
			TextInfo(FightingSaveInfo,AttackUserInfo.iPosition,0,m_iWiner);
			//CT_INT32 iAttactUser=AttackUserInfo.iPosition;
			//printf("�غ�:%i ������HP:%i ������MP:%i ������:%i Ŀ��:%i ����ID:%d �˺�:%i ״̬1:%i ״̬2:%i ״̬3:%i ״̬4:%i ״̬5:%i ״̬6:%i ʤ��id:%d\n",
			//	FightingSaveInfo.cbRound,m_UserInfo[iAttactUser].iHP,m_UserInfo[iAttactUser].iMP,AttackUserInfo.iPosition,0,FightingSaveInfo.cbAction,FightingSaveInfo.iReduceHP[iAttactUser],FightingSaveInfo.cbStatus[iAttactUser][0],
			//	FightingSaveInfo.cbStatus[iAttactUser][1],FightingSaveInfo.cbStatus[iAttactUser][2],FightingSaveInfo.cbStatus[iAttactUser][3],FightingSaveInfo.cbStatus[iAttactUser][4],
			//	FightingSaveInfo.cbStatus[iAttactUser][5],m_iWiner);
#endif
			//////////////////////////////////////////////////////////////////////////

			continue;
		}

#ifdef _WIN32
		//printf("hp:%d pos:%d\n",AttackUserInfo.iHP,AttackUserInfo.iPosition);
#endif

		//����ʹ��
		CT_INT32 iSkillCount=UseSkillCount(AttackUserInfo);
		CT_INT32 iPosition[POST_COUNT]={0};
		CT_INT32 iDeadPos[POST_COUNT]={0};

		if(iSkillCount == 0)
		{
			G_SkillMain SkillMain={0}; 
			CT_INT32 iAffectID[SKILLAFFECT]={0};
			CCFightingScriptSetting::getSingleton().GetSkillMainData(AttackUserInfo.iSkill1,SkillMain);

			SkillAffectInfo(AttackUserInfo.iSkill1,AttackUserInfo,iPosition);

			//��¼����λ��
			for(CT_INT32 j=0;j<POST_COUNT;++j)
			{
				if((iPosition[j]&Dead) != 0)
				{
					iDeadPos[j]=iPosition[j];
				}
			}
			////////////////////////////////////////////////////////////////////////
		}
		else
		{
			//���ܹ���
			for(CT_BYTE i=0;i<iSkillCount;++i)
			{
				//��¼����Ч��
				switch(i)
				{
				case 0:
					{
						SkillAffectInfo(AttackUserInfo.iSkill2.iID,AttackUserInfo,iPosition);
						break;
					}
				case 1:
					{
						SkillAffectInfo(AttackUserInfo.iSkill3.iID,AttackUserInfo,iPosition);
						break;
					}
				case 2:
					{
						SkillAffectInfo(AttackUserInfo.iSkill4.iID,AttackUserInfo,iPosition);
						break;
					}
				default:
					{
						return CT_FALSE;
					}
				}

				//��¼����λ��
				for(CT_INT32 j=0;j<POST_COUNT;++j)
				{
					if((iPosition[j]&Dead) != 0)
					{
						iDeadPos[j]=iPosition[j];
					}
				}

				m_iWiner=GetWiner(fuinfo,0);
				if(m_iWiner != WINER_NULL)
					break;

				//����״̬
				CT_BOOL bRound=CT_TRUE;
				for(CT_INT32 j=0;j<POST_COUNT;++j)
				{
					if(j < POST_COUNT/2)
					{
						if(m_UserInfo[j].iHP > 0)
						{
							bRound=CT_FALSE;
						}
					}

					if(bRound && j == POST_COUNT/2)
					{
						break;
					}
					if(!bRound && j == POST_COUNT/2)
					{
						bRound=CT_TRUE;
					}

					if(j >= POST_COUNT/2)
					{
						if(m_UserInfo[j].iHP > 0)
						{
							bRound=CT_FALSE;
						}
					}
				}

				if(bRound)
					break;
				//////////////////////////////////////////////////////////////////////////
			}
		}

		//����ǰ����
		for(CT_INT32 i=0;i<POST_COUNT;++i)
		{
			if((iDeadPos[i]&Dead) != 0)
			{
				m_UserInfo[i].iFaint=0;
				m_UserInfo[i].iExpire=0;
				memset(&m_UserInfo[i].AddStatus[1],0,sizeof(m_UserInfo[i].AddStatus[0])*(STATUSEX-1));
				memset(&m_UserInfo[i].iHpEffect[1],0,sizeof(m_UserInfo[i].iHpEffect[0])*(STATUSEX-1));
				memset(&m_UserInfo[i].iMpEffect[1],0,sizeof(m_UserInfo[i].iMpEffect[0])*(STATUSEX-1));
				memset(&m_UserInfo[i].iSkillStatus[1],0,sizeof(m_UserInfo[i].iSkillStatus[0])*(STATUSEX-1));
				memset(&m_UserInfo[i].iStatusTime[1],0,sizeof(m_UserInfo[i].iStatusTime[0])*(STATUSEX-1));
				m_UserInfo[i].cbStatusCount=1;

				SkillEquipAffect(Skill_Time_2,m_UserInfo[i]);
			}
		}

		//�����ж�
		if(m_iWiner != WINER_NULL)
		{
			return CT_TRUE;
		}
	}



	return CT_TRUE;
}


//�����û�
CT_INT32 CCFightingLogic::GetActionUser()
{
	CT_INT32 iTempPos=m_iActionPos;
	CT_BOOL bRound=CT_TRUE;
	for(CT_INT32 i=0;i<POST_COUNT;++i)
	{
		if(i < POST_COUNT/2)
		{
			if(m_UserInfo[i].iHP > 0)
			{
				bRound=CT_FALSE;
			}
		}

		if(bRound && i == POST_COUNT/2)
		{
			break;
		}
		if(!bRound && i == POST_COUNT/2)
		{
			bRound=CT_TRUE;
		}

		if(i >= POST_COUNT/2)
		{
			if(m_UserInfo[i].iHP > 0)
			{
				bRound=CT_FALSE;
			}
		}
	}

	//�����غ��л�����������״̬
	for(CT_INT32 i=0;i<POST_COUNT;++i)
	{
		if(bRound)
		{
			FightingUserInfo fuinfo={0};
			StatusCompute(m_UserInfo[i],fuinfo,CT_TRUE);
		}
	}

	for(CT_INT32 i=0;i<GAME_PLAYER*2;++i)
	{
		//�غϼ���
		if((m_iActionPos == 0 && i == 0) || i == GAME_PLAYER || bRound)
		{
			m_cbRound++;
			m_iActionPos=0;
			iTempPos=0;
			bRound=CT_FALSE;
			memcpy(m_iNoActionUser,m_iActionSort,sizeof(m_iActionSort));

			//�غ�ǰ����
			for(CT_INT32 j=0;j<POST_COUNT;++j)
				SkillEquipAffect(Skill_Time_1,m_UserInfo[j]);
			//���＼��
			for(CT_INT32 j=0;j<POST_COUNT;++j)
			{
				CT_INT32 win=GetWiner(m_UserInfo[j],0);
				if(win != WINER_NULL)break;
				if(m_iActionSort[j] >= 0)
					PetSkillAct(m_UserInfo[m_iActionSort[j]]);
			}
		}

		if(m_iNoActionUser[iTempPos] == -1)
		{
			if(iTempPos+2 >= GAME_PLAYER)
			{
				iTempPos=(m_iActionPos+1)%GAME_PLAYER;
			}
			else
				iTempPos=(iTempPos+2)%GAME_PLAYER;
			continue;
		}

		if(m_UserInfo[m_iNoActionUser[iTempPos]].iHP > 0)
		{
			CT_INT32 iReturnID=m_iNoActionUser[iTempPos];
			m_iNoActionUser[iTempPos]=-1;
			m_iActionPos=(++m_iActionPos)%GAME_PLAYER;

			return iReturnID;
		}
	}


	//�쳣�������ʧ��
	return -1;
}

//Ŀ���û�
CT_INT32 CCFightingLogic::GetTargetUser(CT_INT32 *pDst,const FightingUserInfo &UserInfo,CT_INT32 iAffectID,CT_INT32 iPosStatus[POST_COUNT])
{
	//��ͨ����
	if(iAffectID == 0)
	{
		CT_INT32 iTargetUser=(UserInfo.iPosition+(POST_COUNT/2))%POST_COUNT;
		if(m_UserInfo[iTargetUser].iHP > 0 && UserInfo.iPosition != 2 && UserInfo.iPosition != 7)
		{
			pDst[iTargetUser]=Negative_Effect;
		}
		else
		{
			if(UserInfo.iPosition < POST_COUNT/2)
			{
				for(CT_BYTE i=0;i < POST_COUNT/2;++i)
				{
					if(m_UserInfo[POST_COUNT/2+i].iHP > 0)
					{
						pDst[POST_COUNT/2+i]=Negative_Effect;
						break;
					}
				}
			}
			else
			{
				for(CT_BYTE i=0;i < POST_COUNT/2;++i)
				{
					if(m_UserInfo[i].iHP > 0)
					{
						pDst[i]=Negative_Effect;
						break;
					}
				}
			}
		}
	}
	else
	{
		CT_INT32 iPosition[POST_COUNT];
		memset(iPosition,0,sizeof(iPosition));

		GetAttickPos(iAffectID,iPosition,UserInfo.iPosition,m_UserInfo);

		G_SkillAffect SkillAffect={0};
		CCFightingScriptSetting::getSingleton().GetSkillAffectData(iAffectID,SkillAffect);

		//��������
		if(GetPostCount(iPosition) <= 1)
		{
			//Ŀ����֤
			ValidatePost(iPosition,UserInfo.iPosition,CT_TRUE,iPosStatus,(SkillAffect.iRandomEnemy != 0?CT_TRUE:CT_FALSE));
		}
		else
		{
			//Ŀ����֤
			ValidatePost(iPosition,UserInfo.iPosition,CT_FALSE,iPosStatus,(SkillAffect.iRandomEnemy != 0?CT_TRUE:CT_FALSE));
		}

		//��������
		memcpy(pDst,&iPosition,sizeof(iPosition));
	}

	//������֤
	for(CT_BYTE i=0;i<POST_COUNT;++i)
	{
		if(pDst[i] != 0)
		{
			return 0;
		}
	}

	//CT_ASSERT(CT_FALSE);
	//CCLogMeg::getSingleton().WriteError("CCFightingLogic::GetTargetUser pDst == NULL");
	return 1;
}

//ʹ�ü�����
//����1 = x1 + y1 * ( a �C b - 50)/50 = 45 + 10 * (a �C b - 50)/50��
//����2 = x2 + y2 * ( b �C c - 50)/50 = 45 + 25 * (b �C c - 50)/50��
CT_INT32 CCFightingLogic::UseSkillCount(const FightingUserInfo &UserInfo)
{
	G_ValueRange ValueRangeX={0};
	G_ValueRange ValueRangeY={0};
	CT_FLOAT fSkill3=0.0f;
	CT_FLOAT fSkill4=0.0f;
	CT_INT32 iSkill2Cost=(UserInfo.iSkill2.iID != 0?-GetMpCost(UserInfo.iSkill2.iID):0);
	CT_INT32 iSkill3Cost=(UserInfo.iSkill3.iID != 0?-GetMpCost(UserInfo.iSkill3.iID):0);
	CT_INT32 iSkill4Cost=(UserInfo.iSkill4.iID != 0?-GetMpCost(UserInfo.iSkill4.iID):0);

	G_GetValueRange(VALUE_TYPE_SKILLX1,&ValueRangeX);
	G_GetValueRange(VALUE_TYPE_SKILLY1,&ValueRangeY);
	fSkill3=GetSkill3Probability(ValueRangeX.fMinVal,ValueRangeY.fMinVal,iSkill2Cost,iSkill3Cost);

	G_GetValueRange(VALUE_TYPE_SKILLX2,&ValueRangeX);
	G_GetValueRange(VALUE_TYPE_SKILLY2,&ValueRangeY);
	fSkill4=GetSkill4Probability(ValueRangeX.fMinVal,ValueRangeY.fMinVal,iSkill3Cost,iSkill4Cost);


	//#ifndef _SHOW_MP_INFO
#ifndef _SHOW_MP_INFO
	if(UserInfo.iSkill2.iID != 0 && UserInfo.iMP >= UserInfo.iMPMax)
	{
		CCUser *pUser = 0;
		if (!CCUserManager::getSingleton().Find(UserInfo.dwUserID,&pUser))
		{
			//CCLogMeg::getSingleton().WriteError("CCFightingLogic::UseSkillCount can't find user %d",UserInfo.dwUserID);
		}
		//add by caihan 
		//ս�������������
		CMD_G_TaskEventInfo taskEventInfo = {0};
		taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_FIGHT;
		taskEventInfo.iTaskSubType = TASK_SUB_TYPE_FIGHT_BATTER;
		
		CT_INT32 UserMP=-iSkill2Cost;
		if(UserInfo.iSkill3.iID != 0 /*&& m_pRand->rand()%100 <= fSkill3*100*/)
		{
			UserMP+=-iSkill3Cost;
			if(UserInfo.iSkill4.iID != 0 /*&& m_pRand->rand()%100 - fSkill4*100 <= 0.000001*/)
			{
				taskEventInfo.iSubType = 3;
				if(pUser != 0)
					pUser->m_userTask.DisposeTask(taskEventInfo);
				return 3;
			}
			taskEventInfo.iSubType = 2;
			if(pUser != 0)
				pUser->m_userTask.DisposeTask(taskEventInfo);
			return 2;
		}
		taskEventInfo.iSubType = 1;
		if(pUser != 0)
			pUser->m_userTask.DisposeTask(taskEventInfo);
		return 1;
	}
#else
	if(UserInfo.iSkill2.iID != 0 /*&& UserInfo.iMP >= iSkill2Cost*/)
	{
		CT_INT32 UserMP=-iSkill2Cost;
		if(UserInfo.iSkill3.iID != 0 /*&& m_pRand->rand()*100 <= fSkill3*100*/)
		{
			UserMP+=-iSkill3Cost;
			if(UserInfo.iSkill4.iID != 0 /*&& m_pRand->rand()*100 <= fSkill4*100*/)
			{
				return 3;
			}
			return 2;
		}
		return 1;
	}
#endif

	return 0;
}

//��ȡʤ����
CT_INT32 CCFightingLogic::GetWiner(FightingUserInfo &fuInfo,CT_INT32 iHurt)
{
	//�˺�����
	if(fuInfo.iHP + iHurt <= 0 && fuInfo.iHPMax != 0)
	{
		fuInfo.iHP=0;

		for(CT_BYTE i=0;i<GAME_PLAYER;++i)
		{
			if(m_iActionSort[i] == fuInfo.iPosition)
			{
				m_iNoActionUser[i]=-1;
				break;
			}
		}
	}
	else if(fuInfo.iHPMax != 0)
	{
		fuInfo.iHP += iHurt;

		if(fuInfo.iHP > fuInfo.iHPMax)
			fuInfo.iHP=fuInfo.iHPMax;

		return WINER_NULL;
	}

	//ʤ���ж�
	CT_INT32 m_iWiner=WINER_RIGHT;
	for(CT_BYTE i=0;i<POST_COUNT;++i)
	{
		if(m_UserInfo[i].iHPMax != 0 && i < POST_COUNT/2)
		{
			i=POST_COUNT/2;
			m_iWiner=WINER_LIFT;

		}

		if(m_iWiner == WINER_RIGHT && i >= POST_COUNT/2)
			return m_iWiner;

		if(m_UserInfo[i].iHPMax != 0 && i >= POST_COUNT/2)
		{
			m_iWiner=WINER_NULL;
			break;
		}
	}
	return m_iWiner;
}

//���õ�λ����
CT_VOID CCFightingLogic::SetDie(FightingUserInfo &fuInfo)
{
	if(fuInfo.iHP <= 0 && fuInfo.iHPMax != 0)
	{
		for(CT_BYTE i=0;i<GAME_PLAYER;++i)
		{
			if(m_iActionSort[i] == fuInfo.iPosition)
			{
				m_iActionSort[i]=-1;
				m_iNoActionUser[i]=-1;
				break;
			}
		}

		memset(&fuInfo,0,sizeof(fuInfo));
	}
	return;
}

//Ŀ����Ŀ
CT_INT32 CCFightingLogic::GetPostCount(const CT_INT32 *iPosition)
{
	CT_INT32 iPostCount=0;

	for(CT_INT32 i=0;i<POST_COUNT;++i)
	{
		if(iPosition[i] != 0)
			iPostCount++;
	}
	return iPostCount;
}
//����λ��
CT_BOOL CCFightingLogic::MonomerPos(CT_INT32 iPosition[POST_COUNT],CT_INT32 iOppositePos,CT_INT32 iDstPos,CT_INT32 iMePosition,CT_INT32 iStatus)
{
	switch(iDstPos)
	{
	case 0:
	case 1:
		{
			if(iOppositePos == 3 || iOppositePos == 0)
			{
				if(m_UserInfo[0].iHP > 0 && iMePosition != 2 && iMePosition != 7)
				{
					//���λ��
					memset(iPosition,0,sizeof(CT_INT32)*POST_COUNT);
					iPosition[0]=iStatus;
					return CT_TRUE;
				}
				else if(m_UserInfo[3].iHP > 0 && iMePosition != 2 && iMePosition != 7)
				{
					if(m_UserInfo[0].iHP > 0 || m_UserInfo[1].iHP > 0 || m_UserInfo[2].iHP > 0)
						return CT_FALSE;

					//���λ��
					memset(iPosition,0,sizeof(CT_INT32)*POST_COUNT);
					iPosition[3]=iStatus;
					return CT_TRUE;
				}
			}
			else if(iOppositePos == 1 || iOppositePos == 4)
			{
				if(m_UserInfo[1].iHP > 0 && iMePosition != 2 && iMePosition != 7)
				{
					//���λ��
					memset(iPosition,0,sizeof(CT_INT32)*POST_COUNT);
					iPosition[1]=iStatus;
					return CT_TRUE;
				}
				else if(m_UserInfo[4].iHP > 0 && iMePosition != 2 && iMePosition != 7)
				{
					if(m_UserInfo[0].iHP > 0 || m_UserInfo[1].iHP > 0 || m_UserInfo[2].iHP > 0)
						return CT_FALSE;

					//���λ��
					memset(iPosition,0,sizeof(CT_INT32)*POST_COUNT);
					iPosition[4]=iStatus;
					return CT_TRUE;
				}
			}
			break;
		}
	case 3:
	case 4:
		{
			if(iOppositePos == 3 || iOppositePos == 0)
			{
				if(m_UserInfo[3].iHP > 0 && iMePosition != 2 && iMePosition != 7)
				{
					//���λ��
					memset(iPosition,0,sizeof(CT_INT32)*POST_COUNT);
					iPosition[3]=iStatus;
					return CT_TRUE;
				}
				else if(m_UserInfo[0].iHP > 0 && iMePosition != 2 && iMePosition != 7)
				{
					//���λ��
					memset(iPosition,0,sizeof(CT_INT32)*POST_COUNT);
					iPosition[0]=iStatus;
					return CT_TRUE;
				}
			}
			else if(iOppositePos == 1 || iOppositePos == 4)
			{
				if(m_UserInfo[4].iHP > 0 && iMePosition != 2 && iMePosition != 7)
				{
					//���λ��
					memset(iPosition,0,sizeof(CT_INT32)*POST_COUNT);
					iPosition[4]=iStatus;
					return CT_TRUE;
				}
				else if(m_UserInfo[1].iHP > 0 && iMePosition != 2 && iMePosition != 7)
				{
					//���λ��
					memset(iPosition,0,sizeof(CT_INT32)*POST_COUNT);
					iPosition[1]=iStatus;
					return CT_TRUE;
				}
			}
			break;
		}
	case 2:
		{
			if(m_UserInfo[iDstPos].iHP > 0)
			{
				iPosition[iDstPos]=iStatus;
				return CT_TRUE;
			}

			if(iOppositePos == 3 || iOppositePos == 0)
			{
				if(m_UserInfo[0].iHP > 0 && iMePosition != 2 && iMePosition != 7)
				{
					//���λ��
					memset(iPosition,0,sizeof(CT_INT32)*POST_COUNT);
					iPosition[0]=iStatus;
					return CT_TRUE;
				}
				else if(m_UserInfo[3].iHP > 0 && iMePosition != 2 && iMePosition != 7)
				{
					if(m_UserInfo[0].iHP > 0 || m_UserInfo[1].iHP > 0 || m_UserInfo[2].iHP > 0)
						return CT_FALSE;

					//���λ��
					memset(iPosition,0,sizeof(CT_INT32)*POST_COUNT);
					iPosition[3]=iStatus;
					return CT_TRUE;
				}
			}
			else if(iOppositePos == 1 || iOppositePos == 4)
			{
				if(m_UserInfo[1].iHP > 0 && iMePosition != 2 && iMePosition != 7)
				{
					//���λ��
					memset(iPosition,0,sizeof(CT_INT32)*POST_COUNT);
					iPosition[1]=iStatus;
					return CT_TRUE;
				}
				else if(m_UserInfo[4].iHP > 0 && iMePosition != 2 && iMePosition != 7)
				{
					if(m_UserInfo[0].iHP > 0 || m_UserInfo[1].iHP > 0 || m_UserInfo[2].iHP > 0)
						return CT_FALSE;

					//���λ��
					memset(iPosition,0,sizeof(CT_INT32)*POST_COUNT);
					iPosition[4]=iStatus;
					return CT_TRUE;
				}
			}
			break;
		}
	case 5:
	case 6:
		{
			if(iOppositePos == 5 || iOppositePos == 8)
			{
				if(m_UserInfo[5].iHP > 0 && iMePosition != 2 && iMePosition != 7)
				{
					//���λ��
					memset(iPosition,0,sizeof(CT_INT32)*POST_COUNT);
					iPosition[5]=iStatus;
					return CT_TRUE;
				}
				else if(m_UserInfo[8].iHP > 0 && iMePosition != 2 && iMePosition != 7)
				{
					if(m_UserInfo[5].iHP > 0 || m_UserInfo[6].iHP > 0 || m_UserInfo[7].iHP > 0)
						return CT_FALSE;

					//���λ��
					memset(iPosition,0,sizeof(CT_INT32)*POST_COUNT);
					iPosition[8]=iStatus;
					return CT_TRUE;
				}
			}
			else if(iOppositePos == 6 || iOppositePos == 9)
			{
				if(m_UserInfo[6].iHP > 0 && iMePosition != 2 && iMePosition != 7)
				{
					//���λ��
					memset(iPosition,0,sizeof(CT_INT32)*POST_COUNT);
					iPosition[6]=iStatus;
					return CT_TRUE;
				}
				else if(m_UserInfo[9].iHP > 0 && iMePosition != 2 && iMePosition != 7)
				{
					if(m_UserInfo[5].iHP > 0 || m_UserInfo[6].iHP > 0 || m_UserInfo[7].iHP > 0)
						return CT_FALSE;

					//���λ��
					memset(iPosition,0,sizeof(CT_INT32)*POST_COUNT);
					iPosition[9]=iStatus;
					return CT_TRUE;
				}
			}
			break;
		}
	case 7:
		{
			if(m_UserInfo[iDstPos].iHP > 0)
			{
				iPosition[iDstPos]=iStatus;
				return CT_TRUE;
			}

			if(iOppositePos == 5 || iOppositePos == 8)
			{
				if(m_UserInfo[5].iHP > 0 && iMePosition != 2 && iMePosition != 7)
				{
					//���λ��
					memset(iPosition,0,sizeof(CT_INT32)*POST_COUNT);
					iPosition[5]=iStatus;
					return CT_TRUE;
				}
				else if(m_UserInfo[8].iHP > 0 && iMePosition != 2 && iMePosition != 7)
				{
					if(m_UserInfo[5].iHP > 0 || m_UserInfo[6].iHP > 0 || m_UserInfo[7].iHP > 0)
						return CT_FALSE;

					//���λ��
					memset(iPosition,0,sizeof(CT_INT32)*POST_COUNT);
					iPosition[8]=iStatus;
					return CT_TRUE;
				}
			}
			else if(iOppositePos == 6 || iOppositePos == 9)
			{
				if(m_UserInfo[6].iHP > 0 && iMePosition != 2 && iMePosition != 7)
				{
					//���λ��
					memset(iPosition,0,sizeof(CT_INT32)*POST_COUNT);
					iPosition[6]=iStatus;
					return CT_TRUE;
				}
				else if(m_UserInfo[9].iHP > 0 && iMePosition != 2 && iMePosition != 7)
				{
					if(m_UserInfo[5].iHP > 0 || m_UserInfo[6].iHP > 0 || m_UserInfo[7].iHP > 0)
						return CT_FALSE;

					//���λ��
					memset(iPosition,0,sizeof(CT_INT32)*POST_COUNT);
					iPosition[9]=iStatus;
					return CT_TRUE;
				}
			}
			break;
		}
	case 8:
	case 9:
		{
			if(iOppositePos == 5 || iOppositePos == 8)
			{
				if(m_UserInfo[8].iHP > 0 && iMePosition != 2 && iMePosition != 7)
				{
					//���λ��
					memset(iPosition,0,sizeof(CT_INT32)*POST_COUNT);
					iPosition[8]=iStatus;
					return CT_TRUE;
				}
				else if(m_UserInfo[5].iHP > 0 && iMePosition != 2 && iMePosition != 7)
				{
					//���λ��
					memset(iPosition,0,sizeof(CT_INT32)*POST_COUNT);
					iPosition[5]=iStatus;
					return CT_TRUE;
				}
			}
			else if(iOppositePos == 6 || iOppositePos == 9)
			{
				if(m_UserInfo[9].iHP > 0 && iMePosition != 2 && iMePosition != 7)
				{
					//���λ��
					memset(iPosition,0,sizeof(CT_INT32)*POST_COUNT);
					iPosition[9]=iStatus;
					return CT_TRUE;
				}
				else if(m_UserInfo[6].iHP > 0 && iMePosition != 2 && iMePosition != 7)
				{
					//���λ��
					memset(iPosition,0,sizeof(CT_INT32)*POST_COUNT);
					iPosition[6]=iStatus;
					return CT_TRUE;
				}
			}
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("����λ�ô���");
		}
	}
	return CT_FALSE;
}

//���＼�ܹ���
CT_VOID CCFightingLogic::PetSkillAct(FightingUserInfo &fuInfo)
{
	G_SkillMain sm={0};
	if(fuInfo.PetSkill.iID == 0 || !G_GetSkillMainData(fuInfo.PetSkill.iID,&sm) ||
		fuInfo.iHP == 0)
	{
		return;
	}

	if(m_cbRound%(sm.iMpCost == 0?1:sm.iMpCost) != 0 && m_cbRound != 1)
		return;

	CT_INT32 iPostion[POST_COUNT]={0};
	SkillAffectInfo(fuInfo.PetSkill.iID,fuInfo,iPostion,CT_TRUE);
	return;
}

//Ŀ����֤
CT_VOID	CCFightingLogic::ValidatePost(CT_INT32 *iPosition,CT_INT32 iMePosition,CT_BOOL bMonomer,CT_INT32 iPosStatus[POST_COUNT],CT_BOOL bRandPos)
{
	CT_INT32 iCamp=(iMePosition>=POST_COUNT/2)?1:0;//0:1-5 1:6-10
	CT_INT32 iDstPos=(iMePosition+POST_COUNT/2)%POST_COUNT;

	if(bMonomer == CT_TRUE)
	{
		//���幥��
		if(iCamp == 1)
		{
			for(CT_INT32 i=0;i<POST_COUNT;++i)
			{
				if(iPosition[i] == 0)continue;
				//if((iPosStatus[i]&Dead) != 0)continue;
				if(iPosition[i] == Negative_Effect  && i < POST_COUNT/2)		//����Ч��λ��
				{
					//�������λ��
					if(MonomerPos(iPosition,iDstPos,i,iMePosition,Negative_Effect) || bRandPos)
						return;

					//��1��λ��ʼ
					for(CT_INT32 j=0;j<POST_COUNT/2;++j)
					{
						//if((iPosStatus[j]&Dead) != 0)break;
						if(m_UserInfo[j].iHP > 0)
						{
							memset(iPosition,0,sizeof(CT_INT32)*POST_COUNT);
							iPosition[j]=Negative_Effect;
							return;
						}
					}
				}
				else																	//����Ч��λ��
				{
					//�������λ��
					if(MonomerPos(iPosition,iDstPos,i,iMePosition,Positive_Effect) || bRandPos)
						return;

					for(CT_INT32 j=POST_COUNT/2;j<POST_COUNT;++j)
					{
						if(iPosition[j] == Positive_Effect)
						{
							return;
						}
					}

					//��1��λ��ʼ
					for(CT_INT32 j=POST_COUNT/2;j<POST_COUNT;++j)
					{
						//if((iPosStatus[j]&Dead) != 0)break;
						if(m_UserInfo[j].iHP > 0)
						{
							memset(iPosition,0,sizeof(CT_INT32)*POST_COUNT);
							iPosition[j]=Positive_Effect;
							return;
						}
					}
				}
			}
		}
		else
		{
			for(CT_INT32 i=POST_COUNT/2;i<POST_COUNT;++i)
			{
				if(iPosition[i] == 0)continue;
				//if((iPosStatus[i]&Dead) != 0)continue;
				if(iPosition[i] == Negative_Effect && (i < POST_COUNT && i >= POST_COUNT/2))		//����Ч��λ��
				{
					//�������λ��
					if(MonomerPos(iPosition,iDstPos,i,iMePosition,Negative_Effect) || bRandPos)
						return;

					//��1��λ��ʼ
					for(CT_INT32 j=POST_COUNT/2;j<POST_COUNT;++j)
					{
						//if((iPosStatus[j]&Dead) != 0)break;
						if(m_UserInfo[j].iHP > 0)
						{
							memset(iPosition,0,sizeof(CT_INT32)*POST_COUNT);
							iPosition[j]=Negative_Effect;
							return;
						}
					}
				}
				else if(iPosition[i] == Positive_Effect)										//����Ч��λ��
				{
					//�������λ��
					if(MonomerPos(iPosition,iDstPos,i,iMePosition,Positive_Effect) || bRandPos)
						return;

					for(CT_INT32 j=POST_COUNT/2;j<POST_COUNT;++j)
					{
						if(iPosition[j] == Positive_Effect)
						{
							return;
						}
					}

					//��1��λ��ʼ
					for(CT_INT32 j=0;j<POST_COUNT/2;++j)
					{
						//if((iPosStatus[j]&Dead) != 0)break;
						if(m_UserInfo[j].iHP > 0)
						{
							memset(iPosition,0,sizeof(CT_INT32)*POST_COUNT);
							iPosition[j]=Positive_Effect;
							return;
						}
					}
				}
			}
		}
	}
	else
	{
		//Ⱥ�幥��
		ValidateRow(iPosition);
	}
	return;
}

//��λ����֤
CT_BOOL	CCFightingLogic::ValidateRow(CT_INT32 *iPosition)
{
	//�����
	CT_INT32 iRow=0;
	CT_INT32 iAffect=0;
	for(CT_INT32 i=0;i<POST_COUNT;++i)
	{
		if(iPosition[i] != 0)
		{
			if(i == 0 || i == 1)
				iRow=1;
			else if(i == 2)
				iRow=2;
			else if(i == 3 || i == 4)
				iRow=3;
			else if(i == 5 || i == 6)
				iRow=4;
			else if(i == 7)
				iRow=5;
			else if(i == 8 || i == 9)
				iRow=6;

			iAffect=iPosition[i];
		}
	}

	//Ŀ�����
	for(CT_INT32 i=0;i<POST_COUNT;++i)
	{
		if(m_UserInfo[i].iHP <= 0 && iPosition[i] != 0)
		{
			iPosition[i]=0;
		}
	}

	//����֤
	for(CT_INT32 i=0;i<3;++i)
	{
		switch(iRow)
		{
		case 1:
			{
				if(m_UserInfo[0].iHP > 0 || m_UserInfo[1].iHP > 0)
				{
					iPosition[0]=iAffect;
					iPosition[1]=iAffect;

					if(m_UserInfo[0].iHP <= 0)
						iPosition[0]=0;
					if(m_UserInfo[1].iHP <= 0)
						iPosition[1]=0;
					return CT_TRUE;
				}
				else
				{
					iPosition[0]=0;
					iPosition[1]=0;
				}
				break;
			}
		case 2:
			{
				if(m_UserInfo[2].iHP > 0 )
				{
					iPosition[2]=iAffect;

					if(m_UserInfo[2].iHP <= 0)
						iPosition[2]=0;
					return CT_TRUE;
				}
				else
				{
					iPosition[2]=0;
				}
				break;
			}
		case 3:
			{
				if(m_UserInfo[3].iHP > 0 || m_UserInfo[4].iHP > 0)
				{
					iPosition[3]=iAffect;
					iPosition[4]=iAffect;

					if(m_UserInfo[3].iHP <= 0)
						iPosition[3]=0;
					if(m_UserInfo[4].iHP <= 0)
						iPosition[4]=0;
					return CT_TRUE;
				}
				else
				{
					iPosition[3]=0;
					iPosition[4]=0;
				}
				break;
			}
		case 4:
			{
				if(m_UserInfo[5].iHP > 0 || m_UserInfo[6].iHP > 0)
				{
					iPosition[5]=iAffect;
					iPosition[6]=iAffect;

					if(m_UserInfo[5].iHP <= 0)
						iPosition[5]=0;
					if(m_UserInfo[6].iHP <= 0)
						iPosition[6]=0;
					return CT_TRUE;
				}
				else
				{
					iPosition[5]=0;
					iPosition[6]=0;
				}
				break;
			}
		case 5:
			{
				if(m_UserInfo[7].iHP > 0)
				{
					iPosition[7]=iAffect;

					if(m_UserInfo[7].iHP <= 0)
						iPosition[7]=0;
					return CT_TRUE;
				}
				else
				{
					iPosition[7]=0;
				}
				break;
			}
		case 6:
			{
				if(m_UserInfo[8].iHP > 0 || m_UserInfo[9].iHP > 0)
				{
					iPosition[8]=iAffect;
					iPosition[9]=iAffect;

					if(m_UserInfo[8].iHP <= 0)
						iPosition[8]=0;
					if(m_UserInfo[9].iHP <= 0)
						iPosition[9]=0;
					return CT_TRUE;
				}
				else
				{
					iPosition[8]=0;
					iPosition[9]=0;
				}
				break;
			}
		default:
			{
				CCLogMeg::getSingleton().WriteError("����֤!");
				return CT_FALSE;
			}
		}

		if(i == 0 && iRow != 1 && iRow < 4)
		{
			iRow=1;
		}
		else if(iRow < 4)
			iRow=(i+1+1);

		if(i == 0 && iRow != 4 && iRow > 4)
		{
			iRow=4;
		}
		else if(iRow >= 4)
			iRow=4+i+1;
	}

	return CT_TRUE;
}

//���üӳ�״̬
CT_VOID CCFightingLogic::SetAddStatus(const G_SkillAffect &Affect,SkillAffect &MeAffect)
{
	MeAffect.fAbsorbDamageIncrease=Affect.fAbsorbDamageIncrease;
	MeAffect.fAimIncrease=Affect.fAimIncrease;
	MeAffect.iAttackIncrease=Affect.iAttackIncrease;
	MeAffect.fConstitutionIncrease=Affect.fConstitutionIncrease;
	MeAffect.fCriticalHitIncrease=Affect.fCriticalHitIncrease;
	MeAffect.fDamageIncrease=Affect.fDamageIncrease;
	MeAffect.iDefenceIncrease=Affect.iDefenceIncrease;
	MeAffect.iDexterityIncrease=Affect.iDexterityIncrease;
	MeAffect.fDodgeIncrease=Affect.fDodgeIncrease;
	MeAffect.iExpire=Affect.iExpire;
	MeAffect.iFaint=Affect.iFaint;
	MeAffect.iIntelligenceIncrease=Affect.iIntelligenceIncrease;
	MeAffect.iSpiritIncrease=Affect.iSpiritIncrease;
	MeAffect.iStrengthIncrease=Affect.iStrengthIncrease;

	return;
}

//ս������
CT_VOID	CCFightingLogic::ActionSort(CT_INT32 *pActionSort,CT_INT32 iBegin)
{
	CT_INT32 iMaxSpeedPos=0;
	for(CT_INT32 i=0;i<POST_COUNT;++i)
	{
		if(m_UserInfo[iMaxSpeedPos].fSpeed < m_UserInfo[i].fSpeed)
		{
			iMaxSpeedPos=i;
			m_bOrder=(i >= POST_COUNT/2?CT_FALSE:CT_TRUE);
		}
	}

	CT_BYTE cbTemp=(m_bOrder?0:1);
	memset(pActionSort,-1,sizeof(CT_INT32)*GAME_PLAYER);
	for(CT_BYTE i=0;i<POST_COUNT;++i)
	{
		if(i == POST_COUNT/2)
			cbTemp=(m_bOrder?1:0);

		if(m_UserInfo[i].iHP > 0)
		{
			if(i<POST_COUNT/2)
			{
				pActionSort[cbTemp]=m_UserInfo[i].iPosition;
				cbTemp+=2;
			}
			else
			{
				pActionSort[cbTemp]=m_UserInfo[i].iPosition;
				cbTemp+=2;
			}
		}
	}

	memcpy(m_iNoActionUser,m_iActionSort,sizeof(m_iActionSort));
}

//����λ��
CT_INT32 CCFightingLogic::GetAttickPos(CT_INT32 iSkillID,CT_INT32 *iPosition,CT_INT32 iMePosition,FightingUserInfo *UserInfo)
{
	G_SkillMain SkillMain={0};
	G_SkillAffect SkillAffect={0};
	CT_INT32 iCamp=(iMePosition>=POST_COUNT/2)?1:0;//0:1-5 1:6-10

	CCFightingScriptSetting::getSingleton().GetSkillAffectData(iSkillID,SkillAffect);

	if(SkillAffect.iSelf != 0)							//Ŀ���Լ�
	{
		iPosition[iMePosition]=SkillAffect.iSelf;
	}

	if(iCamp == 1)										//�ѷ�ǰ��
	{
		//�ѷ�ǰ��
		if(SkillAffect.iAllyFront1 != 0)
			iPosition[5]=SkillAffect.iAllyFront1;
		if(SkillAffect.iAllyFront2 != 0)
			iPosition[6]=SkillAffect.iAllyFront2;

		//�з�ǰ��
		if(SkillAffect.iEnemyFront1 != 0)	
			iPosition[0]=SkillAffect.iEnemyFront1;
		if(SkillAffect.iEnemyFront2 != 0)	
			iPosition[1]=SkillAffect.iEnemyFront2;

		//�ѷ����
		if(SkillAffect.iAllyBack4 != 0)	
			iPosition[8]=SkillAffect.iAllyBack4;
		if(SkillAffect.iAllyBack5 != 0)	
			iPosition[9]=SkillAffect.iAllyBack5;

		//�з����
		if(SkillAffect.iEnemyBack4 != 0)
			iPosition[3]=SkillAffect.iEnemyBack4;
		if(SkillAffect.iEnemyBack5 != 0)
			iPosition[4]=SkillAffect.iEnemyBack5;
	}
	else
	{
		//�ѷ�ǰ��
		if(SkillAffect.iAllyFront1 != 0)
			iPosition[0]=SkillAffect.iAllyFront1;
		if(SkillAffect.iAllyFront2 != 0)
			iPosition[1]=SkillAffect.iAllyFront2;

		//�з�ǰ��
		if(SkillAffect.iEnemyFront1 != 0)	
			iPosition[5]=SkillAffect.iEnemyFront1;
		if(SkillAffect.iEnemyFront2 != 0)	
			iPosition[6]=SkillAffect.iEnemyFront2;

		//�ѷ����
		if(SkillAffect.iAllyBack4 != 0)	
			iPosition[3]=SkillAffect.iAllyBack4;
		if(SkillAffect.iAllyBack5 != 0)	
			iPosition[4]=SkillAffect.iAllyBack5;

		//�з����
		if(SkillAffect.iEnemyBack4 != 0)
			iPosition[8]=SkillAffect.iEnemyBack4;
		if(SkillAffect.iEnemyBack5 != 0)
			iPosition[9]=SkillAffect.iEnemyBack5;
	}

	if(SkillAffect.iAllyMiddle3 != 0)					//�ѷ��о�
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

	if(SkillAffect.iEnemyMiddle3 != 0)					//�з��о�
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


	if(SkillAffect.iFriendLeastHp != 0)					//ʣ�������ٷֱ����ٵ��Ѿ�
	{
		CT_INT32 iMinHp=100;
		CT_INT32 iUserPos=0;
		if(iCamp == 0)
		{
			iUserPos=0;
			for(CT_INT32 i=0;i<POST_COUNT/2;++i)
			{
				if(UserInfo[i].iHP == 0)continue;
				CT_FLOAT fMinHp=(CT_FLOAT)(UserInfo[i].iHP)/UserInfo[i].iHPMax;
				if((CT_INT32)(fMinHp*100) <= iMinHp)
				{
					iMinHp=(CT_INT32)((CT_FLOAT)UserInfo[i].iHP/UserInfo[i].iHPMax*100);
					iUserPos=i;
				}
			}

			iPosition[iUserPos]=SkillAffect.iFriendLeastHp;
		}
		else
		{
			iUserPos=5;
			for(CT_INT32 i=POST_COUNT/2;i<POST_COUNT;++i)
			{
				if(UserInfo[i].iHP == 0)continue;
				CT_FLOAT fMinHp=(CT_FLOAT)(UserInfo[i].iHP)/UserInfo[i].iHPMax;
				if((CT_INT32)(fMinHp*100) <= iMinHp)
				{
					iMinHp=(CT_INT32)((CT_FLOAT)UserInfo[i].iHP/UserInfo[i].iHPMax*100);
					iUserPos=i;
				}
			}

			iPosition[iUserPos]=SkillAffect.iFriendLeastHp;
		}
	}

	if(SkillAffect.iRandomEnemy != 0)					//���һ���о�
	{
		if(iCamp == 0)
		{
			CT_INT32 iNum=0;
			while(true)
			{
				CT_INT32 iRand=m_pRand->rand()%(POST_COUNT/2)+POST_COUNT/2;
				if(UserInfo[iRand].iHP > 0)
				{
					iPosition[iRand]=SkillAffect.iRandomEnemy;
					break;
				}
				iNum++;
				if(iNum > 20)
				{
					for(CT_INT32 i=POST_COUNT/2;i<POST_COUNT;++i)
					{
						if(UserInfo[i].iHP > 0)
						{
							iPosition[i]=SkillAffect.iRandomEnemy;
							break;
						}
					}
					break;
				}
			}
		}
		else
		{
			CT_INT32 iNum=0;
			while(true)
			{
				CT_INT32 iRand=m_pRand->rand()%(POST_COUNT/2);
				if(UserInfo[iRand].iHP != 0)
				{
					iPosition[iRand]=SkillAffect.iRandomEnemy;
					break;
				}
				iNum++;
				if(iNum > 20)
				{
					for(CT_INT32 i=0;i<POST_COUNT/2;++i)
					{
						if(UserInfo[i].iHP > 0)
						{
							iPosition[i]=SkillAffect.iRandomEnemy;
							break;
						}
					}
					break;
				}
			}
		}
	}

	return 0;
}

//��ȡ��������
CT_INT32 CCFightingLogic::GetMpCost(CT_INT32 iSkillID)
{
	G_SkillMain Skill;
	memset(&Skill,0,sizeof(Skill));

	CCFightingScriptSetting::getSingleton().GetSkillMainData(iSkillID,Skill);
	return -Skill.iMpCost < 0?0:Skill.iMpCost;
}

//��ȡս����Ϣ
CT_VOID CCFightingLogic::GetFightingInfo(const UserFightingStatus &FightingInfo,FightingUserInfo &UserInfo)
{
	CombatUnitData data={0};
	G_GetMasterFightingData(FightingInfo,data);
	MasterCombatStatus MasterInfo=data.mcs;
	UserInfo.iHPMax=MasterInfo.iMaxHP*MasterInfo.fHPPersent;
	UserInfo.iHP=MasterInfo.iMaxHP*MasterInfo.fHPPersent;
	UserInfo.iMPMax=MasterInfo.iMaxMP;
	UserInfo.iMP=MasterInfo.iMaxMP;

#if 0
	CT_INT32 iMpMax=(FightingInfo.iSkillID[0].iID != 0?-GetMpCost(FightingInfo.iSkillID[0].iID):0);
	if(iMpMax > MasterInfo.iMaxMP)
	{
		UserInfo.iMPMax=iMpMax;
		//UserInfo.iMP=iMpMax;
	}
	else
	{
		UserInfo.iMPMax=iMpMax;
		UserInfo.iMP=iMpMax;
	}
#endif

	CT_INT32 iMpMax=0;
	for(CT_INT32 j=0;j<EQUIPED_SKILL_COUNT;++j)
	{
		iMpMax+=(data.Skills[j].iID != 0?-GetMpCost(data.Skills[j].iID):0);
	}

	if(iMpMax > MasterInfo.iMaxMP)
	{
		UserInfo.iMPMax=iMpMax;
		//UserInfo.iMP=iMpMax;
	}
	else
	{
		UserInfo.iMPMax=iMpMax;
		//UserInfo.iMP=iMpMax;
	}

	UserInfo.iStrenth=MasterInfo.iStrength;
	UserInfo.iAgility=MasterInfo.iAgility;
	UserInfo.iIntellent=MasterInfo.iIntellect;
	UserInfo.iSpirit=MasterInfo.iSpirit;
	UserInfo.fHitRate=MasterInfo.fHitRate;
	UserInfo.fDodgeRate=MasterInfo.fDodgeRate;
	UserInfo.fCritRate=MasterInfo.fCritRate;
	UserInfo.fResilient=MasterInfo.fResilient;
	UserInfo.fDmgIncPercent=MasterInfo.fDmgIncPercent;
	UserInfo.fDmgDecPercent=MasterInfo.fDmgDecPercent;
	UserInfo.iAttack=MasterInfo.iAttack;
	UserInfo.iDefence=MasterInfo.iDefence;
	UserInfo.fHPPersent=MasterInfo.fHPPersent;
	UserInfo.fSpeed=MasterInfo.fSpeed;

	UserInfo.iHpReply=MasterInfo.iHPRecover;
	UserInfo.iMpReply=MasterInfo.iMPRecover;

	UserInfo.iHpEffect[0]	+= UserInfo.iHpReply;
	UserInfo.iMpEffect[0]	+= UserInfo.iMpReply;
	UserInfo.cbStatusCount  = 1;
	UserInfo.iSkillStatus[0]= 1;
	UserInfo.iStatusTime[0] = m_iFightingRound;

	//�û�����
	CT_INT32 iSkillID[EQUIPED_SKILL_COUNT]={0}; 
	UserInfo.iSkill1=data.Skills[0].iID;
	UserInfo.iSkill2=data.Skills[1];
	UserInfo.iSkill3=data.Skills[2];
	UserInfo.iSkill4=data.Skills[3];

	for(CT_INT32 i=0;i<SKILL_ENUIP_COUNT;++i)
	{
		G_SkillMain sm={0};
		if(!G_GetSkillMainData(data.SkillEnuip.iSkillEquip[i],&sm))
			continue;

		UserInfo.SkillEquip.iSkillEquip[i]=sm.iID;
		UserInfo.SkillEquip.iSkillCount[i]=sm.iRounds;
	}

	UserInfo.PetSkill.iID=data.PetSkill.iID;
	//UserInfo.PetSkill.iID=4;

	UserInfo.dwUserID=FightingInfo.dwUserID;
	UserInfo.iGrade=data.level;
	UserInfo.iPosition=data.position%(POST_COUNT/2);
	UserInfo.iOccupation=data.type;
}
//��ȡս����Ϣ����ȡʿ����
CT_VOID CCFightingLogic::GetFightingInfo(const UserFightingStatus &FightingInfo,FightingUserInfo &UserInfo,CT_INT32 iIndex)
{
	CT_INT32 iType=FightingInfo.units[iIndex].type;
	if(iType != CLASS_TYPE_WARRIOR && iType != CLASS_TYPE_MAGE && iType != CLASS_TYPE_ARCHER)
	{
		MasterCombatStatus MasterInfo=FightingInfo.units[iIndex].mcs;
		UserInfo.iHPMax=MasterInfo.iMaxHP;
		UserInfo.iHP=MasterInfo.iMaxHP;
		UserInfo.iMPMax=MasterInfo.iMaxMP;
		UserInfo.iMP=MasterInfo.iMaxMP;

#if 0
		CT_INT32 iMpMax=(FightingInfo.iSkillID[0].iID != 0?-GetMpCost(FightingInfo.iSkillID[0].iID):0);
		if(iMpMax > MasterInfo.iMaxMP)
		{
			UserInfo.iMPMax=iMpMax;
			//UserInfo.iMP=iMpMax;
		}
		else
		{
			UserInfo.iMPMax=iMpMax;
			UserInfo.iMP=iMpMax;
		}
#endif

		CT_INT32 iMpMax=0;
		for(CT_INT32 j=0;j<EQUIPED_SKILL_COUNT;++j)
		{
			iMpMax+=(FightingInfo.units[iIndex].Skills[j].iID != 0?-GetMpCost(FightingInfo.units[iIndex].Skills[j].iID):0);
		}

		if(iMpMax > MasterInfo.iMaxMP)
		{
			UserInfo.iMPMax=iMpMax;
			//UserInfo.iMP=iMpMax;
		}
		else
		{
			UserInfo.iMPMax=iMpMax;
			//UserInfo.iMP=iMpMax;
		}

		UserInfo.iStrenth=MasterInfo.iStrength;
		UserInfo.iAgility=MasterInfo.iAgility;
		UserInfo.iIntellent=MasterInfo.iIntellect;
		UserInfo.iSpirit=MasterInfo.iSpirit;
		UserInfo.fHitRate=MasterInfo.fHitRate;
		UserInfo.fDodgeRate=MasterInfo.fDodgeRate;
		UserInfo.fCritRate=MasterInfo.fCritRate;
		UserInfo.fResilient=MasterInfo.fResilient;
		UserInfo.fDmgIncPercent=MasterInfo.fDmgIncPercent;
		UserInfo.fDmgDecPercent=MasterInfo.fDmgDecPercent;
		UserInfo.iAttack=MasterInfo.iAttack;
		UserInfo.iDefence=MasterInfo.iDefence;
		UserInfo.fHPPersent=MasterInfo.fHPPersent;

		UserInfo.iHpReply=MasterInfo.iHPRecover;
		UserInfo.iMpReply=MasterInfo.iMPRecover;

		UserInfo.iHpEffect[0]	+= UserInfo.iHpReply;
		UserInfo.iMpEffect[0]	+= UserInfo.iMpReply;
		UserInfo.cbStatusCount  = 1;
		UserInfo.iSkillStatus[0]= 1;
		UserInfo.iStatusTime[0] = m_iFightingRound;

		//�û�����
		CT_INT32 iSkillID[EQUIPED_SKILL_COUNT]={0}; 
		UserInfo.iSkill1=FightingInfo.units[iIndex].Skills[0].iID;
		UserInfo.iSkill2=FightingInfo.units[iIndex].Skills[1];
		UserInfo.iSkill3=FightingInfo.units[iIndex].Skills[2];
		UserInfo.iSkill4=FightingInfo.units[iIndex].Skills[3];

		for(CT_INT32 i=0;i<SKILL_ENUIP_COUNT;++i)
		{
			G_SkillMain sm={0};
			if(!G_GetSkillMainData(FightingInfo.units[iIndex].SkillEnuip.iSkillEquip[i],&sm))
				continue;

			UserInfo.SkillEquip.iSkillEquip[i]=sm.iID;
			UserInfo.SkillEquip.iSkillCount[i]=sm.iRounds;
		}
		UserInfo.PetSkill.iID=FightingInfo.units[iIndex].PetSkill.iID;

		UserInfo.dwUserID=FightingInfo.dwUserID;
		UserInfo.iGrade=FightingInfo.units[iIndex].level;
		UserInfo.iPosition=FightingInfo.units[iIndex].position%(POST_COUNT/2);
		UserInfo.iOccupation=FightingInfo.units[iIndex].type;
	}
	return;
}

//��ȡս����Ϣ
CT_VOID CCFightingLogic::GetFightingInfo(G_MonsterBaseInfo &MosterInfo,FightingUserInfo &UserInfo)
{
	UserInfo.iHPMax= (CT_INT32)(MosterInfo.iBaseHP/*+MosterInfo.iBaseHP*RankStatus.fHealthPercent+RankStatus.iHealth*/);
	UserInfo.iMPMax=MosterInfo.iBaseMP;
	UserInfo.iHP=  (CT_INT32)(MosterInfo.iBaseHP/*+MosterInfo.iBaseHP*RankStatus.fHealthPercent+RankStatus.iHealth*/);
	UserInfo.iMP=MosterInfo.iBaseMP;

#if 0
	CT_INT32 iMpMax=(MosterInfo.iSkill2 != 0?-GetMpCost(MosterInfo.iSkill2):0);
	if(iMpMax > MosterInfo.iBaseMP)
	{
		UserInfo.iMPMax=iMpMax;
	}
#endif

	CT_INT32 iMpMax=0;
	iMpMax+=(MosterInfo.iSkill2 != 0?-GetMpCost(MosterInfo.iSkill2):0);
	iMpMax+=(MosterInfo.iSkill3 != 0?-GetMpCost(MosterInfo.iSkill3):0);
	iMpMax+=(MosterInfo.iSkill4 != 0?-GetMpCost(MosterInfo.iSkill4):0);

	if(iMpMax > UserInfo.iMPMax)
	{
		UserInfo.iMPMax=iMpMax;
	}
	else
	{
		UserInfo.iMPMax=iMpMax;
	}

	UserInfo.iStrenth=MosterInfo.iBasePower;
	UserInfo.iAgility=MosterInfo.iBaseAgile;
	UserInfo.iIntellent=MosterInfo.iBaseIntelligence;
	UserInfo.iSpirit=MosterInfo.iBaseSprite;
	UserInfo.fHitRate=MosterInfo.fHitRate;
	UserInfo.fDodgeRate=MosterInfo.fDodgeRate;
	UserInfo.fCritRate=MosterInfo.fCritRate;
	UserInfo.fResilient=MosterInfo.fToughness;
	UserInfo.iAttack=MosterInfo.iBaseAttack;
	UserInfo.iDefence=MosterInfo.iBaseDefense;
	UserInfo.fDmgDecPercent=MosterInfo.fDmgDecPercent;
	UserInfo.fDmgIncPercent=MosterInfo.fDmgIncPercent;


	UserInfo.iSkill1=MosterInfo.iSkill1;
	UserInfo.iSkill2.iID=MosterInfo.iSkill2;
	UserInfo.iSkill3.iID=MosterInfo.iSkill3;
	UserInfo.iSkill4.iID=MosterInfo.iSkill4;

	UserInfo.iGrade=MosterInfo.iGrade;
}

//���ܹ���Ч��
CT_VOID CCFightingLogic::SkillActAffect(CT_INT32 iPosition,CT_INT32 iAffectID,CT_INT32 iMePosition,CT_INT32 iHurt)
{
	G_SkillAffect Skillaffect={0};
	CCFightingScriptSetting::getSingleton().GetSkillAffectData(iAffectID,Skillaffect);

	if(m_UserInfo[iPosition].iHP <= 0) return;

	//for(CT_INT32 j=0;j<POST_COUNT;++j)
	{
		//if(iPosition[j] == 0 || m_UserInfo[iPosition].iHP <= 0)continue;

		//������֤
		CT_BOOL bRepeat=CT_FALSE;
		for(CT_INT32 z=1;z<STATUSEX;++z)
		{
			if(m_UserInfo[iPosition].iSkillStatus[z] == iAffectID)
			{
				//HP,MPЧ���ӳ�(�ж�ǰ��Ч)
				m_UserInfo[iPosition].iHpEffect[z]+=iHurt;
				m_UserInfo[iPosition].iMpEffect[z]+=Skillaffect.iMpIncrease;

				//״̬�ӳ�
				SetAddStatus(Skillaffect,m_UserInfo[iPosition].AddStatus[z]);
				m_UserInfo[iPosition].iStatusTime[z]=Skillaffect.iRounds;

				bRepeat=CT_TRUE;

				break;
			}
		}

		//����Ч��������֤
		if(!bRepeat && Skillaffect.iRounds != 0)
		{
			if(m_UserInfo[iPosition].cbStatusCount < STATUSEX)
			{
				//״̬�ӳ�
				SetAddStatus(Skillaffect,m_UserInfo[iPosition].AddStatus[m_UserInfo[iPosition].cbStatusCount]);

				if(Skillaffect.iExpire != 0)
					m_UserInfo[iPosition].iExpire=Skillaffect.iExpire;

				m_UserInfo[iPosition].iSkillStatus[m_UserInfo[iPosition].cbStatusCount]=iAffectID;
				m_UserInfo[iPosition].iStatusTime[m_UserInfo[iPosition].cbStatusCount]=Skillaffect.iRounds;

				//HP,MPЧ���ӳ�(�ж�ǰ��Ч)
				m_UserInfo[iPosition].iHpEffect[m_UserInfo[iPosition].cbStatusCount]+=iHurt;
				m_UserInfo[iPosition].iMpEffect[m_UserInfo[iPosition].cbStatusCount]+=Skillaffect.iMpIncrease;

				if(m_UserInfo[iPosition].cbStatusCount < STATUSEX)
					m_UserInfo[iPosition].cbStatusCount++;

				if(Skillaffect.iExpire != 0)
				{
					m_UserInfo[iPosition].iExpire=Negative_Effect;
				}
			}
			else
			{
				//����״̬
				for(CT_INT32 z=2;z<STATUSEX;++z)
				{
					m_UserInfo[iPosition].iSkillStatus[z-1]=m_UserInfo[iPosition].iSkillStatus[z];
					m_UserInfo[iPosition].iStatusTime[z-1]=m_UserInfo[iPosition].iStatusTime[z];

					//HP,MPЧ���ӳ�(�ж�ǰ��Ч)
					m_UserInfo[iPosition].iHpEffect[z-1]=m_UserInfo[iPosition].iHpEffect[z];
					m_UserInfo[iPosition].iMpEffect[z-1]=m_UserInfo[iPosition].iMpEffect[z];

					//����
					if(Skillaffect.iExpire != 0)
					{
						m_UserInfo[iPosition].iExpire=Negative_Effect;
					}

				}
				m_UserInfo[iPosition].iSkillStatus[STATUSEX-1]=iAffectID;
				m_UserInfo[iPosition].iStatusTime[STATUSEX-1]=Skillaffect.iRounds;

				//HP,MPЧ���ӳ�(�ж�ǰ��Ч)
				m_UserInfo[iPosition].iHpEffect[STATUSEX-1]+=iHurt;
				m_UserInfo[iPosition].iMpEffect[STATUSEX-1]+=Skillaffect.iMpIncrease;

				//����
				if(Skillaffect.iExpire != 0)
				{
					m_UserInfo[iPosition].iExpire=Negative_Effect;
				}
			}
		}
	}

	return ;
}
//װ������
CT_VOID CCFightingLogic::SkillEquipAffect(const CT_INT32 iSkillTime,FightingUserInfo &fuInfo)
{
	for(CT_INT32 i=0;i<SKILL_ENUIP_COUNT;++i)
	{
		G_SkillMain sm={0};
		if(fuInfo.SkillEquip.iSkillEquip[i] == 0 || !G_GetSkillMainData(fuInfo.SkillEquip.iSkillEquip[i],&sm) ||
			fuInfo.SkillEquip.iSkillCount[i] <= 0)
		{
			continue;
		}

		CT_FLOAT fPercent=fuInfo.iHP/(CT_FLOAT)fuInfo.iHPMax;
		if(sm.iSkillTime != iSkillTime || fPercent*100 > sm.fResidueHPPercent)
			continue;

		CT_INT32 iRand=m_pRand->rand()%100;
		if(sm.fTouchPro*100 < iRand)
		{
			FightingSave fs={0};
			fs.cbRound=m_cbRound;
			fs.iAction=fuInfo.SkillEquip.iSkillEquip[i];
			fs.cbAttackUser=fuInfo.iPosition;
			m_FightingList.push_back(fs);
			fuInfo.SkillEquip.iSkillCount[i]--;

			//��������
			if(iSkillTime == Skill_Time_1)
				SetDie(fuInfo);
			GetWiner(fuInfo,0);

#ifdef _WIN32
			if(iSkillTime == Skill_Time_1)
				printf("*******pos:%d\tresurgence lost********\n",fuInfo.iPosition);
			else
				printf("*******pos:%d\trevenge lost********\n",fuInfo.iPosition);
#endif
			continue;
		}

		if(m_iWiner != WINER_NULL)
			break;

		CT_INT32 iPostion[POST_COUNT]={0};
		switch(iSkillTime)
		{
		case Skill_Time_1:
			{
				SkillAffectInfo(fuInfo.SkillEquip.iSkillEquip[i],fuInfo,iPostion);
				//printf("*******pos:%d\tresurgence********%d\n",fuInfo.iPosition,fuInfo.SkillEquip.iSkillCount[i]);
				break;
			}
		case Skill_Time_2:
			{
				SkillAffectInfo(fuInfo.SkillEquip.iSkillEquip[i],fuInfo,iPostion);
				//printf("*******pos:%d\trevenge********%d\n",fuInfo.iPosition,fuInfo.SkillEquip.iSkillCount[i]);
				break;
			}
		default:
			{
				CCLogMeg::getSingleton().WriteError("CCFightingLogic::SkillEquipAffect iSkillTime:%d",iSkillTime);
				break;
			}
		}

		fuInfo.SkillEquip.iSkillCount[i]--;

		//
		for(CT_INT32 j=0;j<POST_COUNT;++j)
		{
			if((iPostion[j]&Dead)!=0)
			{
				SkillEquipAffect(Skill_Time_2,m_UserInfo[j]);
			}
		}
	}

	//��������
	if(iSkillTime == Skill_Time_1)
		SetDie(fuInfo);
	GetWiner(fuInfo,0);
	return; 
}

//��������
CT_BOOL CCFightingLogic::SkillAffectHit(const CT_INT32 iAffectID,FightingUserInfo &AttackUserInfo,FightingUserInfo &DstUser,CT_INT32 &iEffect,const CT_FLOAT fAddHurt,FightingSave &FightingSaveInfo)
{
	//���м���
	CT_INT32 iHurt=0;
	if(HitCompute(AttackUserInfo,DstUser,iAffectID) == CT_TRUE)
	{
		G_SkillAffect Skillaffect={0}; 
		CCFightingScriptSetting::getSingleton().GetSkillAffectData(iAffectID,Skillaffect);

		//��������
		CT_FLOAT fCrit=CritCompute(AttackUserInfo,DstUser,iAffectID);

		if((iEffect&Negative_Effect) != 0)
		{
			G_BossSkillRule bsr={0};
			G_GetBossSkillRule(FightingSaveInfo.iAction,DstUser.iOccupation,&bsr);
			iHurt=(CT_INT32)(HurtCompute(AttackUserInfo,DstUser,iAffectID)*fCrit*fAddHurt);

			//����boss�˺�����
			if(bsr.fGain != 0 && iHurt != 0)
			{
#ifdef _WIN32
				printf("***********hurt befor:%d\n",iHurt);
				iHurt+=(iHurt*bsr.fGain);
				printf("***********hurt after:%d\n",iHurt);
#else
				iHurt+=(iHurt*bsr.fGain);
#endif
			}

		}
		else if((iEffect&Positive_Effect) != 0)
		{
			//����
			iHurt=TreatmentCompute(AttackUserInfo,DstUser,iAffectID);
			iHurt=(CT_INT32)(iHurt*fCrit*fAddHurt);
		}

		//��ʱ��Ч(�ͻ��˸���״̬Ч���ж�)
		CT_INT32 iPos=DstUser.iPosition;
		if(iEffect != 0 && Skillaffect.iRounds == 0)
		{
			//HP,MPЧ���ӳ�(�ж�ǰ��Ч)
			if(m_UserInfo[iPos].iExpire != 0 && Skillaffect.iMpIncrease <= 0)
			{
				m_UserInfo[iPos].iMP+=Skillaffect.iMpIncrease;
				FightingSaveInfo.iReduceMP[iPos]+=Skillaffect.iMpIncrease;
			}
			else if(m_UserInfo[iPos].iExpire == 0)
			{
				m_UserInfo[iPos].iMP+=Skillaffect.iMpIncrease;
				FightingSaveInfo.iReduceMP[iPos]+=Skillaffect.iMpIncrease;
			}

			//�����ж�
			if(m_UserInfo[iPos].iMP < 0)
				m_UserInfo[iPos].iMP=0;

			FightingSaveInfo.iReduceHP[iPos]+=iHurt;


			//if(iEffect == 0)
			{
				if(iHurt > 0)
				{
					iEffect=Positive_Effect;
					if(fCrit > 1.0f)
					{
						iEffect=iEffect|Critical;
					}
				}
				else if(iHurt < 0)
				{
					iEffect=Negative_Effect;
					if(fCrit > 1.0f)
					{
						iEffect=iEffect|Critical;
					}
				}
			}

			m_iWiner=GetWiner(m_UserInfo[iPos],iHurt);

			//�����ж�
			if(m_UserInfo[iPos].iHP == 0)
			{
				iEffect=iEffect|Dead;
			}
		}
		else if(iEffect != 0 && Skillaffect.iRounds != 0)
		{
			//�ӳ�Ч��
			SkillActAffect(iPos,iAffectID,AttackUserInfo.iPosition,iHurt);
		}
		return CT_TRUE;
	}
	return CT_FALSE;
}

//����Ч��
CT_VOID CCFightingLogic::SkillAffectInfo(CT_INT32 iSkillID,FightingUserInfo &AttackUserInfo,CT_INT32 iPosition[POST_COUNT],CT_BOOL bPetSkill)
{
	G_SkillMain SkillMain={0}; 
	FightingSave FightingSaveInfo={0};
	CT_INT32 iAffectID[SKILLAFFECT]={0};
	FightingSaveInfo.iAction=iSkillID;
	CCFightingScriptSetting::getSingleton().GetSkillMainData(iSkillID,SkillMain);

	iAffectID[0]=SkillMain.iAffectID1;
	iAffectID[1]=SkillMain.iAffectID2;
	iAffectID[2]=SkillMain.iAffectID3;
	iAffectID[3]=SkillMain.iAffectID4;

	//�ͷż��ܿ۳�MP
	CT_FLOAT fAddHurt=1.0f;
	if(SkillMain.iMpCost != 0 && !bPetSkill)
	{
		fAddHurt=1.0f+(AttackUserInfo.iMP-AttackUserInfo.iMPMax)/(AttackUserInfo.iMP > 0?AttackUserInfo.iMP:1.0f)*2.0f;
		fAddHurt=(fAddHurt >1.0f?fAddHurt:1.0f);
		FightingSaveInfo.iReduceMP[AttackUserInfo.iPosition]+=-AttackUserInfo.iMP;
		AttackUserInfo.iMP=0;
		m_UserInfo[AttackUserInfo.iPosition].iMP=0;
	}

	//����Ч��1��Ψһ���˺�Ч����
	if(iAffectID[0] != 0)
	{
		//ѡ��Ŀ��
		if(GetTargetUser(iPosition,AttackUserInfo,iAffectID[0],iPosition) != 1)
		{
			for(CT_BYTE j=0;j<POST_COUNT;++j)
			{
				if(iPosition[j] == 0)
					continue;

				//Ŀ���û�
				FightingUserInfo DstUser={0};
				memcpy(&DstUser,&m_UserInfo[j],sizeof(DstUser));
				StatusCompute(m_UserInfo[j],DstUser,CT_FALSE);

				if(!SkillAffectHit(iAffectID[0],AttackUserInfo,DstUser,iPosition[j],fAddHurt,FightingSaveInfo))
				{
					//����
					//ս����¼
					FightingSaveInfo.iAction=iSkillID;
					FightingSaveInfo.cbRound=m_cbRound;
					FightingSaveInfo.cbAttackUser=(bPetSkill?AttackUserInfo.iPosition+10:AttackUserInfo.iPosition);
					memcpy(FightingSaveInfo.iSkillStatusHP,AttackUserInfo.iHpEffect,sizeof(FightingSaveInfo.iSkillStatusHP));
					memcpy(FightingSaveInfo.iSkillStatusMP,AttackUserInfo.iMpEffect,sizeof(FightingSaveInfo.iSkillStatusMP));

					for(CT_BYTE k=0;k<POST_COUNT;++k)
					{
						FightingSaveInfo.cbTarget[k]=iPosition[k];
						memcpy(FightingSaveInfo.cbStatus[k],m_UserInfo[k].iSkillStatus,sizeof(m_UserInfo[k].iSkillStatus));
					}
				}
				else
				{
					iPosition[j]=iPosition[j]|SkillAffect1;
				}
			}
		}
	}


	//����Ч��2
	for(CT_INT32 k=1;k<SKILLAFFECT;++k)
	{
		if(m_iWiner != WINER_NULL)
			break;

		if(iAffectID[k] != 0)
		{
			//ѡ��Ŀ��
			CT_INT32 iStatusPos[POST_COUNT];
			memset(iStatusPos,0,sizeof(iStatusPos));
			if(GetTargetUser(iStatusPos,AttackUserInfo,iAffectID[k],iPosition) == 1)
				break;

			for(CT_INT32 j=0;j<POST_COUNT;++j)
			{
				if(iStatusPos[j] == 0)continue;			//Ч��2-4�е�Ŀ��
				if(m_UserInfo[j].iHP <= 0)continue;		//Ŀ����������0
				if((iPosition[j]&Positive_Effect) != 0 && FightingSaveInfo.iReduceHP[j] == 0)continue;//Ч��1Ϊ����Ч��,�˺�Ϊ0
				if(iStatusPos[j] != Positive_Effect && (iPosition[j]&Positive_Effect) != 0)continue;	//Ч��2-4Ŀ��Ϊ����Ч��,Ч��1Ϊ����(�����˺�,��ֹЧ��1��Ч��2-4�˺��ص�)
				if(iStatusPos[j] != Positive_Effect && FightingSaveInfo.iReduceHP[j] == 0)continue;		//Ч��2-4Ŀ��Ϊ����Ч��,Ч��1�˺�Ϊ0

#ifdef _SHOW_FIGHT_INFO
				printf("����Ч��%d\n",k);
#endif

				//Ŀ���û�
				FightingUserInfo DstUser={0};
				memcpy(&DstUser,&m_UserInfo[j],sizeof(DstUser));
				StatusCompute(m_UserInfo[j],DstUser,CT_FALSE);

				//���м���
				if(SkillAffectHit(iAffectID[k],AttackUserInfo,DstUser,iStatusPos[j],fAddHurt,FightingSaveInfo))
				{
					if(k == 1)
					{
						iPosition[j]=iPosition[j]|SkillAffect2;
					}
					else if(k == 2)
					{
						iPosition[j]=iPosition[j]|SkillAffect3;
					}
					else
					{
						iPosition[j]=iPosition[j]|SkillAffect4;
					}
				}
			}
		}
	}

	//ս����¼
	FightingSaveInfo.cbRound=m_cbRound;
	FightingSaveInfo.cbAttackUser=(bPetSkill?AttackUserInfo.iPosition+10:AttackUserInfo.iPosition);
	memcpy(FightingSaveInfo.iSkillStatusHP,AttackUserInfo.iHpEffect,sizeof(FightingSaveInfo.iSkillStatusHP));
	memcpy(FightingSaveInfo.iSkillStatusMP,AttackUserInfo.iMpEffect,sizeof(FightingSaveInfo.iSkillStatusMP));

	for(CT_BYTE j=0;j<POST_COUNT;++j)
	{
		FightingSaveInfo.cbTarget[j]=iPosition[j];
		memcpy(FightingSaveInfo.cbStatus[j],m_UserInfo[j].iSkillStatus,sizeof(m_UserInfo[j].iSkillStatus));
	}

	m_FightingList.push_back(FightingSaveInfo);

	return;
}

//�˺�����
//�˺�ֵ = �� ����������ֵ + A�������� * [��1 + A������/100) * ������ϵ��1 + (1 + A������/100��* ������ϵ��2 ]  
//�C B�������� * [��1 + B������/100 �� * ������ϵ��1 + ��1 + B������/100�� * ������ϵ��2 ] + 
//[ A�������� * (1 + A������/100) �C B�������� * (1 + B������/100)] * ������ϵ��3 �� * 
//�� 1 + A���˺����Ӱٷֱ� �C B���˺����ٰٷֱ� �� * random��1 - ����ֵ��1 + ����ֵ�� * ��1 + ���ܼ���ֵЧ����������
CT_INT32 CCFightingLogic::HurtCompute(const FightingUserInfo& UserInfo,const FightingUserInfo &DstUser,CT_INT32 iSkillID)
{
	G_SkillAffect GSA={0};
	if(CCFightingScriptSetting::getSingleton().GetSkillAffectData(iSkillID,GSA) == CT_FALSE)
	{
		CT_ASSERT(CT_FALSE);
		CCLogMeg::getSingleton().WriteError("CCFightingLogic::HurtCompute GetSkillAffectData skillid:%d",iSkillID);
	}

	CT_INT32 iRange=(CT_INT32)((1.0+GSA.fRange - (1.0-GSA.fRange))*100);
	CT_FLOAT fRand=(CT_FLOAT)(((1.0f-GSA.fRange)*100.0+m_pRand->rand()%(iRange == 0?1:iRange))/100.0);
	CT_FLOAT fPercent=(CT_FLOAT)(1.0+UserInfo.fDmgIncPercent-DstUser.fDmgDecPercent)*(1- GSA.iIsAbsorbDamage);
	CT_INT32 iMin=(UserInfo.iHPMax*GSA.fOneSelfDamagepercent+DstUser.iHP*GSA.fTargetDamagepercent);
	iMin=(iMin>DstUser.iHP-1?DstUser.iHP-1:iMin);
	CT_INT32 iHurt=(CT_INT32)(GSA.iAttackBasic+UserInfo.iAttack*((1.0+UserInfo.iStrenth/500.0)*GSA.fAttackVariable1+(1.0+UserInfo.iIntellent/500.0)*
		GSA.fAttackVariable2)-DstUser.iDefence*((1.0+DstUser.iAgility/500.0)*GSA.fDefenceVariable1+(1.0+DstUser.iSpirit/500.0)*
		GSA.fDefenceVariable2)+(UserInfo.iAttack*(1.0+UserInfo.iIntellent/500.0)-DstUser.iDefence*(1.0+DstUser.iSpirit/500.0))*
		GSA.fAttackVariable3)+iMin;

	if(fRand == 0.0f)
		fRand=1.0f;

	if(GSA.fAttackVariable1 == 0 && GSA.fAttackVariable2 == 0 && GSA.fAttackVariable3 == 0 && GSA.fOneSelfDamagepercent == 0 && GSA.fTargetDamagepercent == 0)
		return 0;
	else if(fPercent > -5.0f && GSA.iIsAbsorbDamage == 0)
		iHurt=(CT_INT32)(iHurt > 3*UserInfo.iGrade?iHurt:3*UserInfo.iGrade)*fPercent*fRand*(1.0 /*+SS.iValue*/);
	else if(GSA.iIsAbsorbDamage == 1)
	{
		//
	}
	else
		iHurt=1;

	if(iHurt <= 0)iHurt=1;

	return -iHurt;
}

//���м���
//���и��� = ���л���ֵ + ����ϵ�� * [ 0.45 + 0.5 *��1 + A�����У�/��1 + B������) ]
CT_BOOL CCFightingLogic::HitCompute(const FightingUserInfo& UserInfo,const FightingUserInfo &DstUser,CT_INT32 iSkillID)
{
	CT_FLOAT fHit=0.0f;
	G_SkillAffect GSA={0};

	if(CCFightingScriptSetting::getSingleton().GetSkillAffectData(iSkillID,GSA) == CT_FALSE)
	{
		CT_ASSERT(CT_FALSE);
		printf("%s %d\n",__FILE__,__LINE__);
	}

	fHit=(CT_FLOAT)(GSA.fAimBasic+(((1.0+UserInfo.fHitRate)/(1.0f+DstUser.fDodgeRate))*0.5f+0.45f)*GSA.fAimPercentage);

	CT_INT32 iRand=m_pRand->rand()%100;
	if(iRand <= fHit*100)
	{
#ifdef _SHOW_FIGHT_INFO
		printf("Ŀ��λ��:%d����iRand:%d\n",DstUser.iPosition,iRand);
#endif
		return CT_TRUE;
	}
	else 
	{
#ifdef _SHOW_FIGHT_INFO
		printf("Ŀ��λ��:%d����iRand:%d\n",DstUser.iPosition,iRand);
#endif
		return CT_FALSE;
	}
}

//��������
//�������� = ��������ֵ + ����ϵ�� * 0.15 * ��1 + A��������/��1 + B�����ԣ�
CT_FLOAT CCFightingLogic::CritCompute(const FightingUserInfo& UserInfo,const FightingUserInfo &DstUser,CT_INT32 iSkillID)
{
	CT_FLOAT fCrit=0.0f;
	G_SkillAffect GSA={0};

	if(CCFightingScriptSetting::getSingleton().GetSkillAffectData(iSkillID,GSA) == CT_FALSE)
	{
		CT_ASSERT(CT_FALSE);
		printf("%s %d\n",__FILE__,__LINE__);
	}

	fCrit= (CT_FLOAT) (GSA.fCriticalHitBasic+(((UserInfo.fCritRate+1.0f)/(1.0f+DstUser.fResilient))*0.5f+-0.42f)*GSA.fCriticalHitPercentage);

	if(fCrit <= 0)return 1.0f;

	CT_INT32 iRand=m_pRand->rand()%100;
	if(iRand <= fCrit*100)
		fCrit=GSA.fCriticalHitValue;
	else 
		fCrit=1.0f;
#ifdef _SHOW_FIGHT_INFO
	printf("Ŀ��λ��:%d����iRand:%d\n",DstUser.iPosition,iRand);
#endif

	return fCrit;
}

//���Ƽ���
CT_INT32 CCFightingLogic::TreatmentCompute(const FightingUserInfo& UserInfo,const FightingUserInfo &DstUser,CT_INT32 iSkillID)
{
	CT_FLOAT fTreatment=0.0f;
	G_SkillAffect GSA={0};

	if(CCFightingScriptSetting::getSingleton().GetSkillAffectData(iSkillID,GSA) == CT_FALSE)
	{
		CT_ASSERT(CT_FALSE);
		printf("%s %d\n",__FILE__,__LINE__);
	}

	CT_FLOAT fDmgIncPercent=(UserInfo.fDmgIncPercent < 0?0.0f:UserInfo.fDmgIncPercent);
	fTreatment=(CT_FLOAT)((GSA.iHealBasic+UserInfo.iAttack*(1.0+UserInfo.iIntellent/500.0)*GSA.fHealVariable+
		DstUser.iHPMax*GSA.fEndureVariable)*1.0/*(1+GSS.fValue)*/)*(1.0f + fDmgIncPercent/2);

	return (CT_INT32)fTreatment;
}

//״̬����
CT_BOOL CCFightingLogic::StatusCompute(FightingUserInfo &UserInfo,FightingUserInfo &AddStaus,CT_BOOL bMeRound)
{
	FightingUserInfo TempInfo={0};
	CT_INT32 iSkillCount=UserInfo.cbStatusCount;
	memcpy(&TempInfo,&UserInfo,sizeof(TempInfo));

	for(CT_INT32 i=0;i<iSkillCount;++i)
	{
		if(TempInfo.iSkillStatus[i] != 0 && TempInfo.iStatusTime[i] > 0)
		{
			if(TempInfo.iStatusTime[i] != 0)
			{
				TempInfo.iAttack+=UserInfo.AddStatus[i].iAttackIncrease;
				TempInfo.iDefence+=UserInfo.AddStatus[i].iDefenceIncrease;
				TempInfo.iStrenth+=UserInfo.AddStatus[i].iStrengthIncrease;
				TempInfo.iDefence+=UserInfo.AddStatus[i].iDexterityIncrease;
				TempInfo.iIntellent+=UserInfo.AddStatus[i].iIntelligenceIncrease;
				TempInfo.iSpirit+=UserInfo.AddStatus[i].iSpiritIncrease;
				TempInfo.fHitRate+=UserInfo.AddStatus[i].fAimIncrease;
				TempInfo.fDodgeRate+=UserInfo.AddStatus[i].fDodgeIncrease;
				TempInfo.fCritRate+=UserInfo.AddStatus[i].fCriticalHitIncrease;
				TempInfo.fResilient+=UserInfo.AddStatus[i].fConstitutionIncrease;

				TempInfo.fDmgIncPercent+=UserInfo.AddStatus[i].fDamageIncrease;
				TempInfo.fDmgDecPercent+=UserInfo.AddStatus[i].fAbsorbDamageIncrease;

				if(UserInfo.AddStatus[i].iFaint != 0)
					TempInfo.iFaint=UserInfo.AddStatus[i].iFaint;
				if(UserInfo.AddStatus[i].iExpire != 0)
				{
					TempInfo.iExpire=Negative_Effect;
					UserInfo.iExpire=Negative_Effect;
				}

				//�Լ��غϺ��󷨼ӳ�
				if(TempInfo.iStatusTime[i] > 0 && bMeRound)
				{
					TempInfo.iHP+=TempInfo.iHpEffect[i];
					UserInfo.iHP+=TempInfo.iHpEffect[i];

					if(TempInfo.iExpire != 0)
					{
						for(CT_INT32 k=0;k<iSkillCount;++k)
						{
							if(TempInfo.iMpEffect[k] > 0)
								TempInfo.iMpEffect[k]=0;
						}
					}

					if(TempInfo.iExpire != 0 && TempInfo.iMpEffect[i] <= 0)
					{
						TempInfo.iMP+=TempInfo.iMpEffect[i];
						UserInfo.iMP+=TempInfo.iMpEffect[i];
					}
					else if(TempInfo.iExpire == 0)
					{
						TempInfo.iMP+=TempInfo.iMpEffect[i];
						UserInfo.iMP+=TempInfo.iMpEffect[i];
					}

					UserInfo.iStatusTime[i]--;

					//Ч����ʧ
					if(UserInfo.iStatusTime[i] <= 0)
					{
						UserInfo.cbStatusCount--;
						UserInfo.iSkillStatus[i]=0;
						if(UserInfo.AddStatus[i].iExpire != 0)
						{
							 UserInfo.iExpire=0;
							 TempInfo.iExpire=0;
						}
					}

					//�����ж�
					if(TempInfo.iHP <= 0)
					{
						TempInfo.iHP=0;
						UserInfo.iHP=0;
						break;
					}
					else if(TempInfo.iHP > TempInfo.iHPMax)
					{
						TempInfo.iHP=TempInfo.iHPMax;
						UserInfo.iHP=TempInfo.iHPMax;
					}

					if(TempInfo.iMP <= 0)
					{
						TempInfo.iMP=0;
						UserInfo.iMP=0;
					}
				}
			}
		}
	}

	//Ч����ʧ
	for(CT_INT32 i=1;i<iSkillCount;++i)
	{
		if(UserInfo.iSkillStatus[i] == 0 && UserInfo.iStatusTime[i] <= 0)
		{
			UserInfo.iHpEffect[i]=0;
			UserInfo.iMpEffect[i]=0;
			UserInfo.iSkillStatus[i]=0;
			UserInfo.iStatusTime[i]=0;
			memset(&UserInfo.AddStatus[i],0,sizeof(UserInfo.AddStatus[i]));

			//��������
			for(CT_INT32 k=1;k<STATUSEX;++k)
			{
				for(CT_INT32 z=k;z<STATUSEX;++z)
				{
					if(UserInfo.iSkillStatus[z] != 0 && UserInfo.iStatusTime[k] <= 0)
					{
						UserInfo.iSkillStatus[k]=UserInfo.iSkillStatus[z];
						UserInfo.iHpEffect[k]=UserInfo.iHpEffect[z];
						UserInfo.iMpEffect[k]=UserInfo.iMpEffect[z];
						UserInfo.iStatusTime[k]=UserInfo.iStatusTime[z];
						memcpy(&UserInfo.AddStatus[k],&UserInfo.AddStatus[z],sizeof(UserInfo.AddStatus[k]));

						UserInfo.iSkillStatus[z]=0;
						UserInfo.iHpEffect[z]=0;
						UserInfo.iMpEffect[z]=0;
						UserInfo.iStatusTime[z]=0;
						break;
					}
				}
			}
		}
	}
	memcpy(&AddStaus,&TempInfo,sizeof(TempInfo));

	return 0;
}


CT_VOID CCFightingLogic::TextInfo(FightingSave &FightingSaveInfo,CT_INT32 iPos,CT_INT32 iDstPos,CT_INT32 iWin)
{
	CT_INT32 iAttactUser=iPos;
	//printf("�غ�:%i ������HP:%i ������MP:%i ������:%i Ŀ��:%i ����ID:%d �˺�:%i \n״̬  1:%i ״̬  2:%i ״̬  3:%i ״̬  4 :%i ״̬  5:%i ״̬  6:%i\n״̬1HP:%i ״̬2HP:%i ״̬3HP:%i ״̬4HP:%i ״̬5HP:%i ״̬6HP:%i ʤ��id:%d\n",
	//	FightingSaveInfo.cbRound,m_UserInfo[iAttactUser].iHP,m_UserInfo[iAttactUser].iMP,iAttactUser,iDstPos,FightingSaveInfo.cbAction,FightingSaveInfo.iReduceHP[iDstPos],
	//	FightingSaveInfo.cbStatus[iAttactUser][0],FightingSaveInfo.cbStatus[iAttactUser][1],FightingSaveInfo.cbStatus[iAttactUser][2],FightingSaveInfo.cbStatus[iAttactUser][3],FightingSaveInfo.cbStatus[iAttactUser][4],
	//	FightingSaveInfo.cbStatus[iAttactUser][5],
	//	FightingSaveInfo.iSkillStatusHP[0],FightingSaveInfo.iSkillStatusHP[1],FightingSaveInfo.iSkillStatusHP[2],FightingSaveInfo.iSkillStatusHP[3],
	//	FightingSaveInfo.iSkillStatusHP[4],FightingSaveInfo.iSkillStatusHP[5],iWin);
	//printf("�غ�:%i ������HP:%i ������MP:%i ������:%i Ŀ��:%i ����ID:%d �˺�:%i \n",
		//FightingSaveInfo.cbRound,m_UserInfo[iAttactUser].iHP,m_UserInfo[iAttactUser].iMP,iAttactUser,iDstPos,FightingSaveInfo.cbAction,FightingSaveInfo.iReduceHP[iDstPos]);

	printf("�غ�:%i ������HP:%i ������MP:%i ������:%i Ŀ��:%i ����ID:%d �˺�:%i \n״̬  1:%i ״̬  2:%i ״̬  3:%i ״̬  4 :%i ״̬  5:%i ״̬  6:%i\n ʤ��id:%d\n",
		FightingSaveInfo.cbRound,m_UserInfo[iAttactUser].iHP,m_UserInfo[iAttactUser].iMP,iAttactUser,iDstPos,FightingSaveInfo.iAction,FightingSaveInfo.iReduceHP[iDstPos],
		FightingSaveInfo.cbStatus[iAttactUser][0],FightingSaveInfo.cbStatus[iAttactUser][1],FightingSaveInfo.cbStatus[iAttactUser][2],FightingSaveInfo.cbStatus[iAttactUser][3],FightingSaveInfo.cbStatus[iAttactUser][4],
		FightingSaveInfo.cbStatus[iAttactUser][5],iWin);
	return;
}