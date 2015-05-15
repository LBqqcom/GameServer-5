#include "CUserArmy.h"
#include "CLogMeg.h"
#include "CGameConfig.h"
#include "CMutex.h"
#include "GlobeScriptInterface.h"
#include "CLogMeg.h"
#include "CMD_Game.h"
#include "CGlobeFunc.h"
#include "CDatabaseMes.h"
#include "CGlobeData.h"
#include "CNetInterface.h"
#include "CActive.h"

#define  MAX_MES_MAX_BUFF_LEN   (100) //

CCUserArmy::CCUserArmy(void)
{
	m_DataMap.clear();
}

CCUserArmy::~CCUserArmy(void)
{
}

CT_INT32 CCUserArmy::transData(const ArmyInfo &src, CT_CHAR *pDst) const
{
	ArmyInfo *pst = (ArmyInfo *)pDst;
	pst->iArmyGrade = src.iArmyGrade;
	pst->iArmyID = src.iArmyID;
	return sizeof(*pst);
}

//��ʼ��
CT_VOID CCUserArmy::Init(CCUser* pUser, CCMutex* pMutex)
{
	m_pUser=pUser;
	m_pMutex=pMutex;
	m_DataMap.clear();
}

CT_VOID CCUserArmy::Clear()
{
	_AUTOMUTEX(m_pMutex);
	m_DataMap.clear();
}

//���������Ϣ
CT_VOID CCUserArmy::InsertBufferData(CT_INT32 iArmyID,CT_INT32 iArmyGrade)
{
	_AUTOMUTEX(m_pMutex);

	std::list<CT_INT32> AiIDList;
	for(AI_IT it=m_DataMap.begin();it != m_DataMap.end();++it)
	{
		if(GET_TARGET_TYPE(UNIT_ALL_BOSS) <= GET_TARGET_TYPE((*it).second.iArmyID) && (*it).second.iArmyID > UNIT_ALL_ARMY)
		{
			AiIDList.push_back((*it).second.iArmyID);
		}
	}

	for(std::list<CT_INT32>::iterator it=AiIDList.begin();it != AiIDList.end();++it)
	{
		m_DataMap.erase((*it));
	}

	ArmyInfo Armyinfo={0};
	Armyinfo.iArmyID=iArmyID;
	Armyinfo.iArmyGrade=iArmyGrade;
	m_DataMap.insert(DataMap::value_type(iArmyID,Armyinfo));
	return;
}

//��þ�����Ϣ
CT_BOOL CCUserArmy::GetArmyInfo(CT_INT32 iArmyID,ArmyInfo &ArmyGrade)
{
	_AUTOMUTEX(m_pMutex);

	AI_IT it=m_DataMap.find(iArmyID);
	if(it != m_DataMap.end())
	{
		ArmyGrade.iArmyGrade=(*it).second.iArmyGrade;
		ArmyGrade.iArmyID=(*it).second.iArmyID;
		return CT_TRUE;
	}

	return CT_FALSE;
}

//��þ�����Ϣ
CT_BOOL CCUserArmy::GetArmyInfo(CT_CHAR *pBuf,CT_INT32 &iLen)
{
	if((CT_UINT32)iLen < m_DataMap.size()*sizeof(ArmyInfo))
	{
		return CT_FALSE;
	}

	CT_INT32 iCount=0;
	AI_IT ItEnd=m_DataMap.end();
	for(AI_IT i=m_DataMap.begin();i != ItEnd;++i)
	{
		memcpy(pBuf+iCount*sizeof(ArmyInfo),&((*i).second),sizeof(ArmyInfo));
		iCount++;
	}
	iLen=iCount*sizeof(ArmyInfo);

	return CT_TRUE;
}
//���½�����Ϣ
CT_BOOL CCUserArmy::UpDataArmyAdvanced(const CT_INT32 iArmyID,CT_INT32 iAdcancedSoldier)
{
	_AUTOMUTEX(m_pMutex);
	AI_IT it=m_DataMap.find(iArmyID);
	if(it != m_DataMap.end())
	{
		ArmyInfo Armyinfo={0};
		Armyinfo.iArmyID=iAdcancedSoldier;

		if(Armyinfo.iArmyID%10 > 3)
		{
			Armyinfo.iArmyGrade=(*it).second.iArmyGrade/2;
		}
		else
			Armyinfo.iArmyGrade=1;

		m_DataMap.insert(AtmyInfoMap::value_type(Armyinfo.iArmyID,Armyinfo));
		m_DataMap.erase(it);

		//�������ݿ�
		MSC_DB_R_ArmyAdvanced ArmyAdvanced={0};
		ArmyAdvanced.dwUserID=m_pUser->GetUserID();
		ArmyAdvanced.iOldArmyID=iArmyID;
		ArmyAdvanced.iNewArmyID=iAdcancedSoldier;
		ArmyAdvanced.iArmyGrade=Armyinfo.iArmyGrade;

		CT_DWORD threadid = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_DB_ARMY_DATA,MSC_SUB_DB_R_ARMY_ADVANCED,(CT_CHAR*)&ArmyAdvanced,sizeof(ArmyAdvanced));
		
		//���ӽ��ף��жϱ��ֽ�������
		CT_INT32 iNum = 0;
		CT_INT32 iType = 0;
		CT_CHAR strID[10] = {0};
		//iNum = atoi(itoa(iAdcancedSoldier,strID,10) + 3);����ɾ���������п�ƽ̨����
		sprintf(strID, "%i", iAdcancedSoldier);
		iNum = atoi(strID + 3);
		strID[3] = 0;
		iType = atoi(strID);
		CMD_G_TaskEventInfo taskEventInfo = {0};
		taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_UPGRADE;
		taskEventInfo.iTaskSubType = TASK_SUB_TYPE_UPGRADE_ARMADV;
		taskEventInfo.iSubType = iType;				//��������
		taskEventInfo.iLevel = iNum;	//���ֽ�λ
		m_pUser->m_userTask.DisposeTask(taskEventInfo);

		return CT_TRUE;
	}

	return CT_FALSE;
}

//����������Ϣ
CT_BOOL CCUserArmy::UpDataArmyData(CT_INT32 iArmyID)
{
	_AUTOMUTEX(m_pMutex);

	AI_IT it=m_DataMap.find(iArmyID);
	if(it != m_DataMap.end())
	{
		(*it).second.iArmyGrade++;

		//�������ݿ�
		MSC_DB_R_ArmyGrade DBArmyGrade={0};
		DBArmyGrade.dwUserID=m_pUser->GetUserID();
		DBArmyGrade.iArmyID=iArmyID;

		CT_DWORD threadid = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_DB_ARMY_DATA,MSC_SUB_DB_R_ARMY_UPGRADE,(CT_CHAR*)&DBArmyGrade,sizeof(DBArmyGrade));

		//add by caihan
		//�����������жϱ�����������
		CMD_G_TaskEventInfo taskEventInfo = {0};
		taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_UPGRADE;
		taskEventInfo.iTaskSubType = TASK_SUB_TYPE_UPGRADE_ARM;
		taskEventInfo.iSubType = iArmyID;
		taskEventInfo.iLevel = (*it).second.iArmyGrade;
		m_pUser->m_userTask.DisposeTask(taskEventInfo);
		//end
		//�ж��Ƿ���arm�����
		CCActive::getSingleton().ActiveProcess(ACTIVE_ARM_LEVEL,(*it).second.iArmyGrade,m_pUser);
		return CT_TRUE;
	}
	return CT_FALSE;
}

//��������
CT_VOID CCUserArmy::UpDataOpenArmy(CT_INT32 iGrade)
{
	_AUTOMUTEX(m_pMutex);

	//��ȡ��������
	CT_INT32 iArmyID=0;
	if(!G_GetOpenArmy(iArmyID,iGrade))
	{
		//CCLogMeg::getSingleton().WriteError("CCUserArmy::UpDataOpenArmy G_GetOpenArmy Grade:%d",iGrade);
		return;
	}

	ArmyInfo Armyinfo={0};
	Armyinfo.iArmyID=iArmyID;
	Armyinfo.iArmyGrade=1;

	m_DataMap.insert(AtmyInfoMap::value_type(iArmyID,Armyinfo));

	//�ж��Ƿ���ӵ�б��ֻ
	CCActive::getSingleton().ActiveProcess(ACTIVE_ARM_GET,iArmyID,m_pUser);

	//�������ݿ�
	MSC_DB_R_ArmyGrade DBArmyGrade={0};
	DBArmyGrade.dwUserID=m_pUser->GetUserID();
	DBArmyGrade.iArmyID=Armyinfo.iArmyID;

	CT_DWORD threadid = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_DB_ARMY_DATA,MSC_SUB_DB_R_ARMY_UPGRADE,(CT_CHAR*)&DBArmyGrade,sizeof(DBArmyGrade));

	//���¾�����Ϣ
	SendArmyInfo();
	return;
}

//���;�����Ϣ
CT_VOID CCUserArmy::SendArmyInfo(CT_DWORD dwMain/*=MAIN_G_MASTER_EVENT_CITY*/)
{
	_AUTOMUTEX(m_pMutex);
	CT_CHAR cBuf[MAX_MES_MAX_BUFF_LEN]={0};
	CT_INT32 ibufLen = MAX_MES_MAX_BUFF_LEN;
	if(!GetArmyInfo(cBuf, ibufLen))
	{
		CCLogMeg::getSingleton().WriteError("CCUserArmy::UpDataOpenArmy GetArmyInfo");
		return;
	}
	m_pUser->GetNetInterface()->Send(dwMain,SUB_G_ARMY_INFO,(CT_CHAR*)&cBuf,ibufLen);
	return;
}

//ɾ������boss
CT_VOID CCUserArmy::DeleteBoss(CT_BOOL bSeek/*=CT_FALSE*/)
{
	_AUTOMUTEX(m_pMutex);
	CT_INT32 iPos[POST_COUNT/2]={0};
	m_pUser->m_userBattleArray.GetBattleArray(iPos);
	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		if(GET_TARGET_TYPE(UNIT_ALL_BOSS) <= GET_TARGET_TYPE(iPos[i]) && iPos[i] > UNIT_ALL_ARMY)
		{
			AI_IT it=m_DataMap.find(iPos[i]);
			if(it != m_DataMap.end())
			{
				m_DataMap.erase(it);
			}
			iPos[i]=0;
		}
	}

	m_pUser->m_userBattleArray.UpDataBattleArray(iPos);
	//���¿ͻ���
	CMD_G_BattleArrayInfo BattleArrayInfo={0};
	BattleArrayInfo.dwUserID=m_pUser->GetUserID();
	memcpy(BattleArrayInfo.iPosition,iPos,sizeof(iPos));
	m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_BATTLE_ARRAY_INFO,(CT_CHAR*)&BattleArrayInfo,sizeof(BattleArrayInfo));

	//��ǽ�������
	memset(iPos,0,sizeof(iPos));
	m_pUser->m_userSeek.GetWallArmy(iPos);
	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		if(GET_TARGET_TYPE(UNIT_ALL_BOSS) <= GET_TARGET_TYPE(iPos[i]) && iPos[i] > UNIT_ALL_ARMY)
		{
			AI_IT it=m_DataMap.find(iPos[i]);
			if(it != m_DataMap.end())
			{
				m_DataMap.erase(it);
			}
			iPos[i]=0;
		}
	}


	m_pUser->m_userSeek.SetWallArmy(iPos);

	CMD_G_SeekArmyInfo sai={0};
	memcpy(sai.iPos,iPos,sizeof(sai.iPos));
	m_pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_ARMY_INFO,(CT_CHAR*)&sai,sizeof(sai));

	SendArmyInfo(MAIN_G_ENTER_ENV);
	return;
}

CT_INT32	CCUserArmy::OwnLevelArm(CT_INT32 iLevel)
{
	_AUTOMUTEX(m_pMutex);
	AI_IT it=m_DataMap.begin();
	CT_INT32 iNum=0;
	while (it!=m_DataMap.end())
	{
		if (it->second.iArmyGrade>=iLevel)
		{
			iNum++;
		}
		it++;
	}
	return iNum;
}

CT_BOOL CCUserArmy::OwnIDArm(CT_INT32 iArmID)
{
	_AUTOMUTEX(m_pMutex);
	AI_IT it=m_DataMap.find(iArmID);
	if (it==m_DataMap.end())
	{
		return CT_FALSE;
	}
	return CT_TRUE;
}


CT_BOOL CCUserArmy::OwnArmyAdvanced(CT_INT32 iArmID)
{
	_AUTOMUTEX(m_pMutex);
	AI_IT it = m_DataMap.begin();
	AI_IT it_end = m_DataMap.end();
	while(it != it_end)
	{
		CT_INT32 iID = 0;
		CT_CHAR strID[100] = {0};
		CT_CHAR strArmID[100] = {0};
		iID = (*it).second.iArmyID;

		//if (strncmp(itoa(iID,strID,10), itoa(iArmID,strArmID,10), 3) == 0) ;����ɾ���������п�ƽ̨����
		sprintf(strID, "%i", iID);
		sprintf(strArmID,"%i", iArmID);
		if (strncmp(strID, strArmID, 3) == 0)
		{
			if (iID > iArmID)
			{
				return CT_TRUE;
			}
		}
		it++;
	}
	return CT_FALSE;
}

CT_BOOL CCUserArmy::OwnArmyAdvanNum(CT_INT32 iAdvanNum, CT_INT32 iArmType)
{
	_AUTOMUTEX(m_pMutex);
	AI_IT it = m_DataMap.begin();
	AI_IT it_end = m_DataMap.end();
	while(it != it_end)
	{
		CT_INT32 iID = 0;
		CT_INT32 iNum = 0;
		CT_INT32 iType = 0;
		CT_CHAR strID[10] = {0};
		iID = (*it).second.iArmyID;
		sprintf(strID, "%i", iID);
		iNum = atoi(strID + 3);
		strID[3] = 0;
		iType = atoi(strID);

		if (iNum >= iAdvanNum && (iArmType==0||iArmType==iType))
		{
			return CT_TRUE;
		}
		it++;
	}
	return CT_FALSE;
}


CT_BOOL CCUserArmy::OwnArmyOfType( CT_INT32 armyType ) const
{
    _AUTOMUTEX(m_pMutex);
    
    if (armyType < UNIT_ALL_ARMY) return CT_TRUE; // ��������ֱ�ӷ���TRUE

    for (DataMap_CONSTIT it = m_DataMap.begin(); it != m_DataMap.end(); ++it)
    {
        if (ARMY_ID2TYPE(it->first) == armyType) {
            return CT_TRUE;
        }
    }
    return CT_FALSE;
}

CT_VOID CCUserArmy::GetEquipedSkill( CMD_G_SkillData *pBuf, CT_INT32 &iLen, const CT_INT32 iArmyID, const CT_INT32 iArmyGrade )
{
    G_ArmyFighting armyFighting = {0};
    G_GetArmyFighting(iArmyID, iArmyGrade, &armyFighting);

    CT_INT32 nCnt = iLen / sizeof(CMD_G_SkillData);
    CT_INT32 n = 0;
    if (n < nCnt) pBuf[n++].iID = armyFighting.iSkill1;
    if (n < nCnt) pBuf[n++].iID = armyFighting.iSkill2;
    if (n < nCnt) pBuf[n++].iID = armyFighting.iSkill3;
    if (n < nCnt) pBuf[n++].iID = armyFighting.iSkill4;
    for (CT_INT32 i = 0; i < n; ++i) {
        pBuf[i].Used = i;
    }
}

// ����ս����Ϣ
CT_VOID CCUserArmy::UpdateBattleStatus( MasterCombatStatus &mcs, const CT_INT32 iArmyID, const CT_INT32 iArmyGrade )
{
    // ��������
    G_ArmyFighting armyFighting = {0};
    G_GetArmyFighting(iArmyID, iArmyGrade, &armyFighting);
    mcs.iAttack        += armyFighting.iBaseAttack;
    mcs.iDefence       += armyFighting.iBaseDefense;
    mcs.iMaxHP         += armyFighting.iBaseHP;
    mcs.iMaxMP         += armyFighting.iBaseMP;
    mcs.iStrength      += armyFighting.iBasePower;
    mcs.iAgility       += armyFighting.iBaseAgile;
    mcs.iIntellect     += armyFighting.iBaseIntelligence;
    mcs.iSpirit        += armyFighting.iBaseSprite;
    mcs.fHitRate       += armyFighting.fHitRate;
    mcs.fDodgeRate     += armyFighting.fDodgeRate;
    mcs.fCritRate      += armyFighting.fCritRate;
    mcs.fResilient     += armyFighting.fToughness;
    mcs.fDmgIncPercent += armyFighting.fDmgIncPercent;
    mcs.fDmgDecPercent += armyFighting.fDmgDecPercent;
    mcs.fHPPersent     += armyFighting.fHpCeiling;
}