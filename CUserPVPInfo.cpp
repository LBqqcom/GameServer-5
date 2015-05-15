#include "CUserPVPInfo.h"
#include "CLogMeg.h"
#include "CGameConfig.h"
#include "GlobeScriptInterface.h"
#include "ProtocolGame.h"
#include <time.h>
#include "CUser.h"
#include "CNetInterface.h"
#include "CThreadMes.h"
#include "CGlobeFunc.h"
#include "CCompress.h"
#include "CMD5.h"


#define MAX_BUFFER		1024*10
CCUserPVPInfo::CCUserPVPInfo(void)
{
	memset(&m_PVPInfo,0,sizeof(m_PVPInfo));
	memset(m_PVPRecord,0,sizeof(m_PVPRecord));
	memset(m_RecordFightData,0,sizeof(m_RecordFightData));
	memset(m_ChallengeFight,0,sizeof(m_ChallengeFight));
	m_iFightingCount=0;


}

CCUserPVPInfo::~CCUserPVPInfo(void)
{
}

//初始化
CT_VOID CCUserPVPInfo::Init(CCUser* pUser, CCMutex* pMutex)
{
	m_pUser=pUser;
	m_pMutex=pMutex;

	memset(&m_PVPInfo,0,sizeof(m_PVPInfo));
	memset(m_PVPRecord,0,sizeof(m_PVPRecord));
	memset(m_RecordFightData,0,sizeof(m_RecordFightData));
	memset(m_ChallengeFight,0,sizeof(m_ChallengeFight));
	m_iFightingCount=0;
	return;
}

//清理用户数据
CT_VOID CCUserPVPInfo::Clear()
{
	_AUTOMUTEX(m_pMutex);
	memset(&m_PVPInfo,0,sizeof(m_PVPInfo));
	memset(m_PVPRecord,0,sizeof(m_PVPRecord));
	memset(m_RecordFightData,0,sizeof(m_RecordFightData));
	memset(m_ChallengeFight,0,sizeof(m_ChallengeFight));
	m_iFightingCount=0;
}

//插入竞技场信息
void CCUserPVPInfo::InsertBufferData(const PVPInfo &PVPinfo)
{
	_AUTOMUTEX(m_pMutex); 

	m_PVPInfo.dwUserID=PVPinfo.dwUserID;
	m_PVPInfo.iFightingCount=PVPinfo.iFightingCount;
	m_PVPInfo.lBeginTime=PVPinfo.lBeginTime;
	m_PVPInfo.lRefreshTime=PVPinfo.lRefreshTime;
	m_PVPInfo.cbChestStatus=PVPinfo.cbChestStatus;
	m_PVPInfo.iPVPDegree=PVPinfo.iPVPDegree;
	return;
}

//插入对手状态
CT_VOID CCUserPVPInfo::InsertChallengUser(const PVPInfo &PVPinfo)
{
	_AUTOMUTEX(m_pMutex); 

	memcpy(m_PVPInfo.bChallengUser,PVPinfo.bChallengUser,sizeof(m_PVPInfo.bChallengUser));
	memcpy(m_PVPInfo.dwChallengUserID,PVPinfo.dwChallengUserID,sizeof(m_PVPInfo.dwChallengUserID));

	return;
}

//插入战报记录
CT_VOID CCUserPVPInfo::InsertRecordInfo(CT_INT32 iIndex,const PVPFightingInfo &FightingInfo)
{
	_AUTOMUTEX(m_pMutex); 
	if(iIndex > PVP_FIGHTING_COUNT)
	{
		return;
	}

	memcpy(&m_PVPRecord[iIndex],&FightingInfo,sizeof(m_PVPRecord[iIndex]));

	if(m_RecordFightData[iIndex].FightingData[0].dwUserID == FightingInfo.dwUserID)
	{
		CombatUnitData data={0};
		G_GetMasterFightingData(m_RecordFightData[iIndex].FightingData[0],data);
		memcpy(m_PVPRecord[iIndex].cUserName,data.name,sizeof(m_PVPRecord[iIndex].cUserName));
	}
	else
	{
		CombatUnitData data={0};
		G_GetMasterFightingData(m_RecordFightData[iIndex].FightingData[1],data);
		memcpy(m_PVPRecord[iIndex].cUserName,data.name,sizeof(m_PVPRecord[iIndex].cUserName));
	}

	return;
}

//插入战斗信息
CT_VOID	CCUserPVPInfo::InsertRecordFighting(MSC_PVP_Fighting_Info &fi)
{
	_AUTOMUTEX(m_pMutex);
	m_RecordFightData[m_iFightingCount]=fi;
	if(m_iFightingCount < PVP_FIGHTING_COUNT-1)
		m_iFightingCount++;
	return;
}

//插入高级组
CT_VOID CCUserPVPInfo::InsertHUserID(CT_INT32 iIndex,CT_DWORD dwUserID)
{
	_AUTOMUTEX(m_pMutex);

	if(iIndex >= PVP_CHALLENGE_COUNT/2)
		return;

	m_PVPInfo.dwChallengUserID[(PVP_CHALLENGE_COUNT/2+iIndex)%PVP_CHALLENGE_COUNT]=dwUserID;
	return;
}

//插入低级组
CT_VOID CCUserPVPInfo::InsertLUserID(CT_INT32 iIndex,CT_DWORD dwUserID)
{
	_AUTOMUTEX(m_pMutex);

	if(iIndex >= PVP_CHALLENGE_COUNT/2)
		return;

	m_PVPInfo.dwChallengUserID[iIndex%PVP_CHALLENGE_COUNT]=dwUserID;
	return;
}

//获取竞技场信息
CT_BOOL CCUserPVPInfo::GetPVPInfo(CT_CHAR *pBuf,CT_INT32 &iLen)
{
	_AUTOMUTEX(m_pMutex); 

	if(iLen < sizeof(CMD_G_PVPInfo))
	{
		return CT_FALSE;
	}

	if(m_ChallengeFight[0].dwUserID == 0 && m_ChallengeLook[4].dwUserID == 0)
		return CT_FALSE;

	G_ValueRange vr={0};
	if(!G_GetValueRange(VALUE_TYPE_PVPCHEST,&vr))
	{
		CCLogMeg::getSingleton().WriteError("CCUserPVPInfo::GetPVPInfo G_GetValueRange VALUE_TYPE_PVPCHEST");
	}

	G_ValueRange FightingValue={0};
	if(!G_GetValueRange(VALUE_TYPE_PVPFIGHTINGCOUNT,&FightingValue))
	{
		CCLogMeg::getSingleton().WriteError("CCUserPVPInfo::GetPVPInfo G_GetValueRange VALUE_TYPE_PVPFIGHTINGCOUNT");
	}

	MasterVariableData msc={0};
	m_pUser->m_userBase.GetMasterData(msc);

	CT_INT64 lTime=time(0);
	CMD_G_PVPInfo PVPInfo={0};
	CT_INT32 iFightingCount=(CT_INT32)(FightingValue.fMinVal-m_PVPInfo.iFightingCount+m_pUser->m_userVip.VipPriTimeGet(VIP_PRIVILEGE_ARENA));
	PVPInfo.dwUserID=m_PVPInfo.dwUserID;
	PVPInfo.iFightingCount=(iFightingCount > 0?iFightingCount:0);
	PVPInfo.lRefreshTime=(m_PVPInfo.lRefreshTime > 0?m_PVPInfo.lRefreshTime-(lTime-m_PVPInfo.lBeginTime):0);
	PVPInfo.lRefreshTime=PVPInfo.lRefreshTime>0?PVPInfo.lRefreshTime:0;
	//PVPInfo.cbChestStatus=m_PVPInfo.cbChestStatus;
	PVPInfo.iPVPDegree=m_PVPInfo.iPVPDegree;
	PVPInfo.iDiamod=msc.stRes.iDiamod;

	for(CT_INT32 i=0;i<PVP_CHALLENGE_COUNT;++i)
	{
		if(m_ChallengeLook[i].dwUserID == 0)continue;
		PVPInfo.ChallengUserInfo[i]=m_ChallengeLook[i];
	}

	memcpy(pBuf,&PVPInfo,sizeof(PVPInfo));
	iLen=sizeof(PVPInfo);

	return CT_TRUE;
}

//获取竞技场信息
CT_VOID CCUserPVPInfo::GetPVPInfo(PVPInfo &PVPinfo,CT_BOOL bIsServer)
{
	_AUTOMUTEX(m_pMutex); 
	memcpy(&PVPinfo,&m_PVPInfo,sizeof(PVPinfo));

	if(!bIsServer)
	{
		G_ValueRange FightingValue={0};
		if(!G_GetValueRange(VALUE_TYPE_PVPFIGHTINGCOUNT,&FightingValue))
		{
			CCLogMeg::getSingleton().WriteError("CCUserPVPInfo::GetPVPInfo G_GetValueRange VALUE_TYPE_PVPFIGHTINGCOUNT");
		}
		PVPinfo.iFightingCount=(CT_INT32)(FightingValue.fMinVal>m_PVPInfo.iFightingCount?(FightingValue.fMinVal- m_PVPInfo.iFightingCount):0);
	}

}

CT_BOOL CCUserPVPInfo::GetPVPRectord(CT_CHAR *pBuf,CT_INT32 &iLen)
{
	_AUTOMUTEX(m_pMutex); 

	if(sizeof(CMD_G_PVPFightingRecord) > iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCUserPVPInfo::GetPVPRectord sizeof(CMD_G_PVPFightingRecord) > iLen");
		return CT_FALSE;
	}

	if(m_PVPRecord[0].dwUserID == 0)
	{
		return CT_FALSE;
	}

	CMD_G_PVPFightingRecord PVPRecord={0};
	PVPRecord.dwUserID=m_pUser->GetUserID();
	for(CT_INT32 i=0;i<PVP_FIGHTING_COUNT;++i)
	{
		memcpy(&PVPRecord.PVPFightinginfo[i],&m_PVPRecord[i],sizeof(PVPFightingInfo));
		GetFightingRectordUser(i,m_PVPRecord[i].dwUserID,PVPRecord.UserData[i]);
	}

	iLen=sizeof(CMD_G_PVPFightingRecord);
	memcpy(pBuf,&PVPRecord,sizeof(CMD_G_PVPFightingRecord));
	return CT_TRUE;
}

//获得对手信息
CT_BOOL CCUserPVPInfo::GetFightingUser(CT_INT32 iIndex,CT_DWORD dwUserID,UserFightingStatus &FightingUser)
{
	_AUTOMUTEX(m_pMutex);

	if(iIndex >= PVP_CHALLENGE_COUNT)
		return CT_FALSE;

	FightingUser=m_ChallengeFight[iIndex];

	if(FightingUser.dwUserID != dwUserID)
		return CT_FALSE;

	return CT_TRUE;
}

CT_BOOL CCUserPVPInfo::GetFightingUser(CT_DWORD dwUserID,LookUserInfo &UserData)
{
	_AUTOMUTEX(m_pMutex);
	for(CT_INT32 i=0;i<PVP_CHALLENGE_COUNT;++i)
	{
		if(m_ChallengeLook[i].dwUserID == dwUserID)
		{
			UserData=m_ChallengeLook[i];
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

//设置竞技度
CT_INT32 CCUserPVPInfo::SetPVPDegree(const CT_INT32 iDegree,CT_BOOL bSend)
{
	_AUTOMUTEX(m_pMutex);

	G_ValueRange Degree={0};
	if(!G_GetValueRange(VALUE_TYPE_PVPDEGREEMAX,&Degree))
	{
		CCLogMeg::getSingleton().WriteError("CCUserPVPInfo::SetPVPDegree G_GetValueRange VALUE_TYPE_PVPDEGREEMAX");
		return 0;
	}

	if(iDegree == -1)
	{
		m_PVPInfo.iPVPDegree=0;
		m_PVPInfo.cbChestStatus=CHEST_STATUS_CLOSE;
	}
	else
	{
		m_PVPInfo.iPVPDegree+=iDegree;

		if(Degree.fMinVal <= m_PVPInfo.iPVPDegree)
		{
			m_PVPInfo.iPVPDegree=(CT_INT32)Degree.fMinVal;
			m_PVPInfo.cbChestStatus=CHEST_STATUS_OPEN;
		}	
	}

	if(bSend)
	{
		G_ValueRange vr={0};
		if(!G_GetValueRange(VALUE_TYPE_PVPCHEST,&vr))
		{
			CCLogMeg::getSingleton().WriteError("CCUserPVPInfo::GetPVPInfo G_GetValueRange VALUE_TYPE_PVPCHEST");
		}

		CMD_G_PVPDegree PVPDegree={0};
		PVPDegree.dwUserID=m_pUser->GetUserID();
		PVPDegree.iPVPDegree=m_PVPInfo.iPVPDegree;
		PVPDegree.iDiamod=G_GetPVPChestDiamod(PVPDegree.iPVPDegree,vr.fMinVal);
		m_pUser->GetNetInterface()->Send(MAIN_G_PVP_EVENT,SUB_G_PVP_DEGREE,(CT_CHAR*)&PVPDegree,sizeof(PVPDegree));

		//更新数据库
		MSC_DB_W_PVPInfo DBPVPinfo={0};
		DBPVPinfo.dwUserID=m_pUser->GetUserID();
		DBPVPinfo.iFightingCount=m_PVPInfo.iFightingCount;
		DBPVPinfo.lRefreshTime=m_PVPInfo.lRefreshTime;
		DBPVPinfo.cbChestStatus=m_PVPInfo.cbChestStatus;
		DBPVPinfo.iPVPDegree=m_PVPInfo.iPVPDegree;
		DBPVPinfo.iPVPChestNum=m_PVPInfo.iPVPChestNum;

		CT_DWORD threadid = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_DB_PVP_DATA,MSC_SUB_DB_R_PVP_INFO,(CT_CHAR*)&DBPVPinfo,sizeof(DBPVPinfo));

	}
	return m_PVPInfo.iPVPDegree;
}

//保存战报
CT_VOID CCUserPVPInfo::SaveFightingReacord()
{
	if(m_PVPRecord[0].dwUserID == 0)return;

	CT_CHAR cBuf[MAX_BUFFER]={0};
	CT_UCHAR ucIndex[PVPRECORD_INDEX]={0};
	MSC_PVP_Fighting_Info FightingData={0};
	MSC_DB_W_PVPFightingInfo FightingRecord={0};

	if(MAX_BUFFER <= sizeof(ucIndex)+sizeof(FightingData)+sizeof(FightingRecord))
	{
		CCLogMeg::getSingleton().WriteError("CCUserPVPInfo::SaveFightingReacord MAX_BUFFER <= sizeof(ucIndex)+sizeof(FightingData)+sizeof(FightingRecord)");
		return;
	}

	sprintf((CT_CHAR*)ucIndex, "%d%d%d",m_RecordFightData[0].FightingData[0].dwUserID,m_RecordFightData[0].FightingData[1].dwUserID,time(0));
	CCMD5::getSingleton().Encrypt(ucIndex,sizeof(ucIndex)-1,ucIndex,sizeof(ucIndex)-1);

	memcpy(&FightingData,&m_RecordFightData[0],sizeof(FightingData));
	memcpy(&FightingRecord.PVPFightinginfo,&m_PVPRecord[0],sizeof(m_PVPRecord[0]));
	memcpy(cBuf,ucIndex,sizeof(ucIndex));
	memcpy(cBuf+sizeof(ucIndex),&FightingData,sizeof(FightingData));
	memcpy(cBuf+sizeof(FightingData)+sizeof(ucIndex),&FightingRecord,sizeof(FightingRecord));

	CT_DWORD threadid = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_DB_PVP_DATA,MSC_SUB_DB_FIGHTING_END,cBuf,sizeof(MSC_PVP_Fighting_Info)+sizeof(MSC_DB_W_PVPFightingInfo)+sizeof(ucIndex));
	return;
}

//获得对手战报信息
CT_BOOL CCUserPVPInfo::GetFightingRectordUser(CT_INT32 iIndex,CT_DWORD dwUserID,LookUserInfo &UserData)
{
	_AUTOMUTEX(m_pMutex);

	if(iIndex >= PVP_CHALLENGE_COUNT)
		return CT_FALSE;

	if(m_RecordFightData[iIndex].LookData.dwUserID == dwUserID)
		UserData=m_RecordFightData[iIndex].LookData;

	return CT_TRUE;
}

//获取难度
CT_INT32 CCUserPVPInfo::GetDifficulty(CT_DWORD dwUserID)
{
	_AUTOMUTEX(m_pMutex);


	for(CT_INT32 i=0;i<PVP_CHALLENGE_COUNT;++i)
	{
		if(m_ChallengeFight[i].dwUserID == dwUserID)
		{
			if(i < PVP_CHALLENGE_COUNT/2)
				return LOW;
			else
				return HIGH;
		}
	}
	return 0;
}

//设置冷却时间
CT_VOID CCUserPVPInfo::SetBeginCoolTime(CT_INT64 lTime)
{
	_AUTOMUTEX(m_pMutex); 

	m_PVPInfo.lBeginTime=lTime;

	//清除时间
	m_PVPInfo.lRefreshTime=0;
}

//设置对手
CT_VOID CCUserPVPInfo::SetChallengInfo(CT_INT32 iIndex,const UserFightingStatus &FightData,LookUserInfo &LookData)
{
	_AUTOMUTEX(m_pMutex); 
	if(iIndex < PVP_CHALLENGE_COUNT)
	{
		m_ChallengeFight[iIndex]=FightData;
		m_ChallengeLook[iIndex]=LookData;
		m_ChallengeLook[iIndex].bFightingStatus=m_PVPInfo.bChallengUser[iIndex];
	}
	return;
}

//获得冷却时间
CT_BOOL CCUserPVPInfo::GetCoolTime(CT_INT64 lTime)
{
	_AUTOMUTEX(m_pMutex);

	if(m_PVPInfo.lBeginTime != 0)
	{
		if(m_PVPInfo.lRefreshTime <= lTime-m_PVPInfo.lBeginTime)
			return CT_TRUE;
		else
			return CT_FALSE;
	}
	else 
		return CT_TRUE; 
}

//获得挑战状况
CT_VOID CCUserPVPInfo::GetChallengeInfo(CT_INT32 iDifficulty,CT_BOOL bWiner[PVP_CHALLENGE_COUNT/2])
{
	_AUTOMUTEX(m_pMutex);

	if(iDifficulty == LOW)
	{
		memcpy(bWiner,m_PVPInfo.bChallengUser,sizeof(CT_BOOL)*(PVP_CHALLENGE_COUNT/2));
	}
	else
	{
		CT_INT32 iLen=sizeof(CT_BOOL)*(PVP_CHALLENGE_COUNT/2);
		memcpy(bWiner,m_PVPInfo.bChallengUser+iLen,iLen);
	}
	return;
}

//获得挑战ID(组)
CT_VOID  CCUserPVPInfo::GetChallengeID(CT_INT32 iDifficulty,CT_DWORD dwUserID[PVP_CHALLENGE_COUNT/2])
{
	if(iDifficulty == LOW)
	{
		for(CT_INT32 i=0;i<PVP_CHALLENGE_COUNT/2;++i)
		{
			dwUserID[i]=m_ChallengeFight[i].dwUserID;
		}
	}
	else
	{
		for(CT_INT32 i=0;i<PVP_CHALLENGE_COUNT/2;++i)
		{
			dwUserID[i]=m_ChallengeFight[(i+PVP_CHALLENGE_COUNT/2)%PVP_CHALLENGE_COUNT].dwUserID;
		}
	}
	return;
}
//获取战报
CT_BOOL CCUserPVPInfo::GetFightingInfo(CT_INT32 iIndex,MSC_PVP_Fighting_Info &FightingInfo)
{
	_AUTOMUTEX(m_pMutex);

	if(iIndex >= PVP_FIGHTING_COUNT)
	{
		return CT_FALSE;
	}

	if(m_RecordFightData[iIndex].FightingData[0].dwUserID == 0)
	{
		return CT_FALSE;
	}

	memcpy(&FightingInfo,&m_RecordFightData[iIndex],sizeof(MSC_PVP_Fighting_Info));
	return CT_TRUE;
}

//获得对手状况
CT_BOOL CCUserPVPInfo::GetChallengeStatce(CT_INT32 iIndex)
{
	_AUTOMUTEX(m_pMutex);
	if(iIndex >= PVP_CHALLENGE_COUNT)
		return 0;
	return m_PVPInfo.bChallengUser[iIndex] != STATUS_WIN;
}

//更新竞技场对手信息
CT_VOID CCUserPVPInfo::UpdataChallengUser(CT_DWORD dwUserID,CT_INT32 iDifficulty,CT_BYTE cbStatus)
{
	_AUTOMUTEX(m_pMutex); 
	if(cbStatus != STATUS_NULL)
	{
		if(iDifficulty == LOW)
		{
			for(CT_INT32 i=0;i<PVP_CHALLENGE_COUNT/2;++i)
			{
				if(m_PVPInfo.dwChallengUserID[i] == dwUserID)
				{
					m_PVPInfo.bChallengUser[i]=cbStatus;
					m_ChallengeLook[i].bFightingStatus=cbStatus;
					return;
				}
			}
		}
		else
		{
			for(CT_INT32 i=PVP_CHALLENGE_COUNT/2;i<PVP_CHALLENGE_COUNT;++i)
			{
				if(m_PVPInfo.dwChallengUserID[i] == dwUserID)
				{
					m_PVPInfo.bChallengUser[i]=cbStatus;
					m_ChallengeLook[i].bFightingStatus=cbStatus;
					return;
				}
			}
		}

		CCLogMeg::getSingleton().WriteError("CCUserPVPInfo::UpdataChallengUser 无此挑战对手!");
	}
	else
	{
		memset(m_PVPInfo.bChallengUser,cbStatus,sizeof(m_PVPInfo.bChallengUser));
	}
	return;
}

//更新竞技场记录
CT_VOID	CCUserPVPInfo::UpdataPVPRectord(const PVPFightingInfo &PVPFightingRecord)
{
	_AUTOMUTEX(m_pMutex); 

	//MSC_PVP_Fighting_Info FightingInfo[PVP_FIGHTING_COUNT]={0};
	PVPFightingInfo FightingRecord[PVP_FIGHTING_COUNT]={0};
	memcpy(FightingRecord,m_PVPRecord,sizeof(m_PVPRecord));
	memcpy(m_PVPRecord+1,FightingRecord,sizeof(m_PVPRecord[0])*(PVP_FIGHTING_COUNT-1));
	memcpy(&m_PVPRecord[0],&PVPFightingRecord,sizeof(PVPFightingRecord));

	//memcpy(FightingInfo,m_RecordFightData,sizeof(m_RecordFightData));
	//memcpy(m_RecordFightData+1,FightingInfo,sizeof(m_RecordFightData[0])*(PVP_FIGHTING_COUNT-1));

	//更新战报用户信息
	for(CT_INT32 i=0;i<PVP_CHALLENGE_COUNT;++i)
	{
		if(PVPFightingRecord.dwUserID == m_ChallengeFight[i].dwUserID)
		{
			//m_RecordFightData[0].FightingData=m_ChallengeFight[i];
			m_RecordFightData[0].LookData=m_ChallengeLook[i];
			break;
		}
	}
	return;
}

//更新战斗数据
CT_VOID CCUserPVPInfo::UpdataFighting(CT_CHAR *pBuf,CT_INT32 iLen)
{
	_AUTOMUTEX(m_pMutex); 
	if(iLen != sizeof(MSC_PVP_Fighting_Info))
	{
		CCLogMeg::getSingleton().WriteError("CCUserPVPInfo::UpdataFighting iLen != sizeof(MSC_PVP_Fighting_Info)");
		return;
	}

	MSC_PVP_Fighting_Info FightingInfo[PVP_FIGHTING_COUNT]={0};
	memcpy(FightingInfo,m_RecordFightData,sizeof(m_RecordFightData));
	memcpy(m_RecordFightData+1,FightingInfo,sizeof(MSC_PVP_Fighting_Info)*(PVP_FIGHTING_COUNT-1));
	memcpy(&m_RecordFightData[0],(MSC_PVP_Fighting_Info*)pBuf,sizeof(MSC_PVP_Fighting_Info));
	return;
}

//更新冷却时间
CT_INT64 CCUserPVPInfo::UpdataPVPCoolTime()
{
	_AUTOMUTEX(m_pMutex); 

	if(m_PVPInfo.lRefreshTime == 0)
		return 0;

	if(m_PVPInfo.lRefreshTime <= (CT_INT64)time(0)-m_PVPInfo.lBeginTime)
	{
		m_PVPInfo.lRefreshTime=0;
		m_PVPInfo.lBeginTime=0;
	}
	else
	{
		CT_INT64 lTime=(CT_INT64)time(0)-m_PVPInfo.lBeginTime;
		m_PVPInfo.lRefreshTime=(m_PVPInfo.lRefreshTime-lTime < 0?0:m_PVPInfo.lRefreshTime-lTime);
		m_PVPInfo.lBeginTime=(CT_INT64)time(0);
	}

	return m_PVPInfo.lRefreshTime;
}

//更新领奖次数
CT_BOOL CCUserPVPInfo::UpDataChestNum(CT_BOOL bReset)
{
	_AUTOMUTEX(m_pMutex); 
	if(bReset)
		m_PVPInfo.iPVPChestNum=0;
	else
	{
		G_ValueRange vr={0};
		if(!G_GetValueRange(VALUE_TYPE_PVPRECEIVE,&vr))
		{
			CCLogMeg::getSingleton().WriteError("CCUserPVPInfo::UpDataChestNum G_GetValueRange VALUE_TYPE_PVPRECEIVE");
			return CT_FALSE;
		}
		//vip add 增加宝箱次数
		if(vr.fMinVal > m_PVPInfo.iPVPChestNum + m_pUser->m_userVip.VipPriTimeGet(VIP_PRIVILEGE_CHEST))
		{
			m_PVPInfo.iPVPChestNum++;
			SetPVPDegree(-1);
		}
	}
	return CT_TRUE;
}
//12点更新竞技场
CT_VOID CCUserPVPInfo::UpdataPVPData(CT_CHAR *pBuf,CT_INT32 iLen)
{
	_AUTOMUTEX(m_pMutex); 
	m_PVPInfo.iFightingCount=0;
	m_PVPInfo.iPVPChestNum=0;

	//更新竞技场信息
	MasterVariableData msc={0};
	m_pUser->m_userBase.GetMasterData(msc);

	CMD_G_PVPInfo SendPVPInfo={0};
	CT_INT32 iTempLen=iLen;
	CT_INT32 iPVPLen=sizeof(SendPVPInfo);
	SendPVPInfo.dwUserID=m_pUser->GetUserID();
	if(GetPVPInfo((CT_CHAR*)&SendPVPInfo,iPVPLen))
	{
		SendPVPInfo.iDiamod=msc.stRes.iDiamod;
		if(CCCompress::getSingleton().Compress((CT_CHAR*)&SendPVPInfo,iPVPLen,pBuf,iTempLen ))
		{
			m_pUser->GetNetInterface()->Send(MAIN_G_PVP_EVENT,SUB_G_PVP_INFO,pBuf,iTempLen);
		}
	} 
	return;
}
