#include "CUserFightTeamNember.h"
#include "CLogMeg.h"
#include "CGameConfig.h"
#include "CFightTeamEvent.h"
//#include "GlobeScriptInterface.h"
#include "CUserManager.h"
#include "CDatabaseMes.h"

CCUserFightTeamNember::CCUserFightTeamNember(void)
{
	m_DataMap.clear();
}

CCUserFightTeamNember::~CCUserFightTeamNember(void)
{
}

CT_INT32 CCUserFightTeamNember::transData(const NemberScience &src, CT_CHAR *pDst) const
{
	NemberScience *pst = (NemberScience *)pDst;
	pst->dwUserID = m_pUser->GetUserID();
	pst->iScienceGrade = src.iScienceGrade;
	pst->iScienceID = src.iScienceID;
	return sizeof(*pst);
}

//初始化
CT_VOID CCUserFightTeamNember::Init(CCUser* pUser, CCMutex* pMutex)
{
	m_pUser=pUser;
	m_pMutex=pMutex;

	m_DataMap.clear();
	return;
}

//清理用户数据
CT_VOID CCUserFightTeamNember::Clear()
{
	_AUTOMUTEX(m_pMutex);
	return;
}

//插入成员科技
CT_VOID	CCUserFightTeamNember::InsertMemberScience(NemberScience &Ftns)
{
	_AUTOMUTEX(m_pMutex);

	m_DataMap.insert(ScienceInfoMap::value_type(Ftns.iScienceID,Ftns));
	return;
}


//获取成员科技
CT_BOOL	CCUserFightTeamNember::GetNemberScience(CT_CHAR *pBuf,CT_INT32 &iLen)
{
	_AUTOMUTEX(m_pMutex);
	if(iLen < m_DataMap.size()*sizeof(NemberScience))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeam::CCUserFightTeamNember iLen < m_DataMap.size()*sizeof(FightTeamNemberScience)");
		return CT_FALSE;
	}

	CT_INT32 iSize=0;
	ScienceInfoMap::iterator itend=m_DataMap.end();
	for(ScienceInfoMap::iterator it=m_DataMap.begin();it != itend;++it)
	{
		memcpy(pBuf+iSize,&((*it).second),sizeof((*it).second));
		iSize+=sizeof((*it).second);
	}
	iLen=iSize;
	return CT_TRUE;
}

//获取科技信息
CT_BOOL CCUserFightTeamNember::GetScienceInfo(CT_INT32 iScienceID,NemberScience &ScienceInfo)
{
	_AUTOMUTEX(m_pMutex);

	ScienceInfoMap::iterator siit=m_DataMap.find(iScienceID);
	if(siit != m_DataMap.end())
	{
		memcpy(&ScienceInfo,&((*siit).second),sizeof((*siit).second));
	}
	return CT_TRUE;
}

//获取加成效果
CT_VOID CCUserFightTeamNember::GetAddAffect(CT_DWORD dwFTID,CT_INT32 iOccupation,MasterCombatStatus &mcs)
{
	_AUTOMUTEX(m_pMutex);
    UpdateBattleStatus(mcs, dwFTID,m_pUser->GetUserID(),&iOccupation, 1, m_DataMap);
#if 0
	CT_INT32 iDst=(iOccupation/100 <= 0? iOccupation:iOccupation/100);

	ScienceInfoMap::iterator itend=m_DataMap.end();
	for(ScienceInfoMap::iterator it=m_DataMap.begin();it != itend;++it)
	{
		G_FightTeamBless ftb={0};
		if(G_GetFightTeamBless((*it).second.iScienceID,&ftb))
		{
			if(ftb.iAdditionType != FT_SCIENCE_FIGHTING)continue;
			if(iDst == ftb.iAdditionObj || (ftb.iAdditionObj == UNIT_ALL_ARMY && iDst >= 100) || 
				ftb.iAdditionObj == UNIT_ALL_UNIT || ftb.iAdditionObj == UNIT_TYPE_PLAYER)
			{
				AddFightingType(ftb.iAdditionType,mcs,ftb);
			}
		}
	}
	return;
#endif
}

#if 0
//类型加成
CT_VOID CCUserFightTeamNember::AddFightingType(CT_INT32 iType,MasterCombatStatus &mcs,G_FightTeamBless &ftb)
{
	_AUTOMUTEX(m_pMutex);
	switch(iType)
	{
	case VALUE_TYPE_HEALTH:
		{
			mcs.iCurrentHP+=(CT_INT32)ftb.fAttrVal;
			mcs.iMaxHP+=(CT_INT32)ftb.fAttrVal;
			break;
		}
	case VALUE_TYPE_HPPENCENT:
		{
			mcs.fHPPersent+=ftb.fAttrVal;
			break;
		}
	case VALUE_TYPE_MANA:
		{
			mcs.iCurrentMP+=(CT_INT32)ftb.fAttrVal;
			mcs.iMaxMP+=(CT_INT32)ftb.fAttrVal;
		}
	case VALUE_TYPE_ATTACK:
		{
			mcs.iAttack+=(CT_INT32)ftb.fAttrVal;
			break;
		}
	case VALUE_TYPE_DEFENCE:
		{
			mcs.iDefence+=(CT_INT32)ftb.fAttrVal;
			break;
		}
	case VALUE_TYPE_STRENTH:
		{
			mcs.iStrength+=(CT_INT32)ftb.fAttrVal;
			break;
		}
	case VALUE_TYPE_AGILITY:
		{
			mcs.iAgility+=(CT_INT32)ftb.fAttrVal;
			break;
		}
	case VALUE_TYPE_INTELLENT:
		{
			mcs.iIntellect+=(CT_INT32)ftb.fAttrVal;
			break;
		}
	case VALUE_TYPE_SPIRIT:
		{
			mcs.iSpirit+=(CT_INT32)ftb.fAttrVal;
			break;
		}
	case VALUE_TYPE_HITRATE:
		{
			mcs.fHitRate+=ftb.fAttrVal;
			break;
		}
	case VALUE_TYPE_DODGERATE:
		{
			mcs.fDodgeRate+=ftb.fAttrVal;
			break;
		}
	case VALUE_TYPE_CRITRATE:
		{
			mcs.fCritRate+=ftb.fAttrVal;
			break;
		}
	case VALUE_TYPE_RESILIENT:
		{
			mcs.fResilient+=ftb.fAttrVal;
			break;
		}
	case VALUE_TYPE_DMGINCPERCENT:
		{
			mcs.fDmgIncPercent+=ftb.fAttrVal;
			break;
		}
	case VALUE_TYPE_DMGDECPERCENT:
		{
			mcs.fDmgDecPercent+=ftb.fAttrVal;
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCUserFightTeamNember::GetAddAffect ftb.iAdditionType:%d",ftb.iAdditionType);
			break;
		}
	}
	return;
}
#endif

//科技升级
CT_INT32 CCUserFightTeamNember::ScienceUpGrade(CT_INT32 iScienceID,CT_INT32 iItemCount)
{
	_AUTOMUTEX(m_pMutex);

	CT_INT32 iGrade=0;
	ScienceInfoMap::iterator siit=m_DataMap.find(iScienceID);
	if(siit != m_DataMap.end())
	{
		iGrade=(*siit).second.iScienceGrade;
	}

	G_FightTeamBless ftb={0};
	if(!G_GetFightTeamBless(iScienceID,&ftb))
	{
		CCLogMeg::getSingleton().WriteError("CCUserFightTeamNember::ScienceUpGrade iScienceID:%d",iScienceID);
		return FT_DATA_ERROR;
	}

	G_BlessUpgrade bug={0};
	if(!G_GetBlessUpgrade(ftb.iUpGradeRule,iGrade+1,&bug))
	{
		CCLogMeg::getSingleton().WriteError("CCUserFightTeamNember::ScienceUpGrade UpGradeRule:%d iScienceGrade:%d",ftb.iUpGradeRule,iGrade+1);
		return FT_DATA_ERROR;
	}

	if(iItemCount != 0)
	{
		//CT_DWORD threadid = GAME_THREAD_CITY + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		//if(!m_pUser->m_userItem.AutoAddItemData(threadid,iGoods,iItemCount,1,ITEM_OP_MODE_CHECK))
		//{
		//	return FT_NO_ITEM;
		//}
	}

	if(bug.fSuccessPer*100 /*+道具几率*/ >= rand()%100)
	{
		if(siit == m_DataMap.end())
		{
			NemberScience ns={0};
			ns.dwUserID=m_pUser->GetUserID();
			ns.iScienceGrade=1;
			ns.iScienceID=iScienceID;
			m_DataMap.insert(ScienceInfoMap::value_type(ns.iScienceID,ns));
		}
		else
		{
			(*siit).second.iScienceGrade+=1;
		}

        if (ftb.iAdditionType == FT_SCIENCE_FIGHTING && ftb.iAdditionObj < UNIT_ALL_ARMY) 
		{
            m_pUser->UpdateAndSendMasterData(EMI_BATLLE);
        }

		MSC_DB_FightTeamScienceUpGrade ftsug={0};
		ftsug.dwUserID=m_pUser->GetUserID();
		ftsug.iScienceID=iScienceID;
		ftsug.iGrade=(siit != m_DataMap.end()?(*siit).second.iScienceGrade:1);
		CT_DWORD threadID = GAME_THREAD_DATABASE + ftsug.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_SCIENCE_UPGRADE,(CT_CHAR*)&ftsug,sizeof(ftsug));


		//科技升级，判断指定科技升级任务
		CMD_G_TaskSystemInfo taskSysInfo = {0};
		taskSysInfo.iTaskMainType = TASK_MAIN_TYPE_SYS;
		taskSysInfo.iTaskSubType = TASK_SUB_TYPE_SYSTEM_SCIENCE_UP;
		taskSysInfo.arrayParam[0] = ftb.iAdditionType;        //参数0：0不限定类型，1建筑，2战斗
		taskSysInfo.arrayParam[1] = ftb.iAdditionObj;         //参数1：建筑id/角色id
		taskSysInfo.arrayParam[2] = ftb.iAttrID;              //参数2：战斗属性id(建筑不用)
		if (siit == m_DataMap.end())
		{
			taskSysInfo.iProgress = 1;                        //刚开启旗帜时，等级为1
		}
		else
		{
			taskSysInfo.iProgress = (*siit).second.iScienceGrade;
		}
		m_pUser->m_userTask.DisposeTask(taskSysInfo);

		return FT_SCIENCE_UP_GRADE_SUC;
	}
	return FT_SCIENCE_UP_GRADE_FAIL;
}

//科技降级
CT_VOID CCUserFightTeamNember::ScienceDownGrade()
{
	_AUTOMUTEX(m_pMutex);
#if 0 //科技惩罚取消
	ScienceInfoMap::iterator itend=m_DataMap.end();
	for(ScienceInfoMap::iterator it=m_DataMap.begin();it != itend;++it)
	{
		if((*it).second.iScienceGrade > 0)
			(*it).second.iScienceGrade =(*it).second.iScienceGrade - 1;
	}

	//科技信息
	CT_CHAR cBuf[10240]={0};
	CT_INT32 iLen=10240;
	if(GetNemberScience(cBuf,iLen))
	{
		m_pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_SCIENCE_INFO,cBuf,iLen);
	}
#endif
	return;
}

//科技建筑加成效果
CT_INT32 CCUserFightTeamNember::ScienceBulidAdd(const CT_DWORD dwTeamID,const CT_INT32 iProduct,const CT_INT32 iBuildType)
{
	_AUTOMUTEX(m_pMutex);
	
	if(dwTeamID == 0)return 0;
	CT_INT32 iCount=0;
	ScienceInfoMap::iterator itend=m_DataMap.end();
	for(ScienceInfoMap::iterator it=m_DataMap.begin();it != itend;++it)
	{
		G_FightTeamBless FTB={0};
		if(!G_GetFightTeamBless((*it).second.iScienceID,&FTB))
		{
			CCLogMeg::getSingleton().WriteError("CCUserFightTeamNember::ScienceBulidAdd G_GetFightTeamBless id:%d",(*it).second.iScienceID);
			continue;
		}

		if(/*FTB.iTeamGrade <= FTBase.iGrade && */FTB.iAdditionType == FT_SCIENCE_BUILD)
		{
			if(FTB.iAdditionObj == iBuildType)
			{
				CT_INT32 iGrade=(*it).second.iScienceGrade;//((*it).second.iScienceGrade > FTBase.iGrade?FTBase.iGrade:(*it).second.iScienceGrade);
				if(FTB.fAttrVal != 0)
					iCount+=G_GetFightTeamBuildAddScale(iProduct,FTB.fAttrVal,iGrade);
				else
					iCount+=FTB.iAttrID*iGrade;
			}
		}
	}

	return iCount;
}

CT_VOID CCUserFightTeamNember::UpdateBattleStatus( MasterCombatStatus &mcs,CT_DWORD dwTeamID,CT_DWORD dwUserID, const CT_INT32 arrayTargets[], const CT_INT32 nTarget, const ScienceInfoMap &scienceMap )
{
	if(dwTeamID == 0)return;
    for (ScienceInfoMap::const_iterator it = scienceMap.begin(); it != scienceMap.end(); ++it) {
        G_FightTeamBless FTB={0};
        if(!G_GetFightTeamBless((*it).second.iScienceID,&FTB))
        {
            CCLogMeg::getSingleton().WriteError("CCUserFightTeamNember::ScienceBulidAdd G_GetFightTeamBless id:%d",(*it).second.iScienceID);
            continue;
        }

        if(/*FTB.iTeamGrade <= FTBase.iGrade && */FTB.iAdditionType == FT_SCIENCE_FIGHTING)
        {
            for (CT_INT32 i = 0; i < nTarget; ++i)
            {
                if (FTB.iAdditionObj == arrayTargets[i]) 
                {
                    CT_FLOAT fVal = 0;
					CT_INT32 iGrade=it->second.iScienceGrade;//(it->second.iScienceGrade > FTBase.iGrade?FTBase.iGrade:it->second.iScienceGrade);
                    for (CT_INT32 j = 0; j < iGrade; ++j) {
                        fVal += FTB.fAttrVal * (j + 1);
                    }
                    CCUserBase::UpdateBattleStatus(mcs, FTB.iAttrID, fVal);
                    break;
                }
            }
        }
    }
}