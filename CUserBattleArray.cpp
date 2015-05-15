#include "CUserBattleArray.h"
#include "GlobeScriptInterface.h"
#include "CUser.h"
#include "CBattleArray.h"

CCUserBattleArray::CCUserBattleArray(void)
{
	memset(m_iPosition,0,sizeof(m_iPosition));
}

CCUserBattleArray::~CCUserBattleArray(void)
{
}

//初始化
CT_VOID CCUserBattleArray::Init(CCUser* pUser, CCMutex* pMutex)
{
	m_pUser  = pUser;
	m_pMutex = pMutex;

	memset(m_iPosition,0,sizeof(m_iPosition));
}

//清理用户数据
CT_VOID CCUserBattleArray::Clear()
{
	_AUTOMUTEX(m_pMutex);
	memset(m_iPosition,0,sizeof(m_iPosition));
}

//插入阵法
CT_VOID CCUserBattleArray::InsertBufferData(CT_INT32 iPosition[POST_COUNT/2])
{
	_AUTOMUTEX(m_pMutex);
	memcpy(m_iPosition,iPosition,sizeof(m_iPosition));

	CT_INT32 iCount=0;
	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		if(m_iPosition[i] != 0)
		{
			iCount++;
		}
	}

	if(iCount > 3)
	{
		MasterData md={0};
		m_pUser->m_userBase.GetMasterData(md);
		for(CT_INT32 i=0;i<POST_COUNT/2;++i)
		{
			if(md.iOccupation == m_iPosition[i])continue;
			m_iPosition[i]=0;
		}
	}
	return;
}

//获取阵法
CT_VOID CCUserBattleArray::GetBattleArray(CT_INT32 iPosition[POST_COUNT/2])
{
	_AUTOMUTEX(m_pMutex);
	memcpy(iPosition,m_iPosition,sizeof(m_iPosition));
	return;
}

CT_BOOL CCUserBattleArray::GetBattleArray(CT_CHAR *pBuf,CT_INT32 &iLen)
{
	_AUTOMUTEX(m_pMutex);
	CT_DWORD dwUserID=m_pUser->GetUserID();
	memcpy(pBuf,&dwUserID,sizeof(dwUserID));
	memcpy(pBuf+sizeof(dwUserID),m_iPosition,sizeof(m_iPosition));
	iLen=sizeof(m_iPosition)+sizeof(dwUserID);
	return CT_TRUE;
}

//更新阵法
CT_VOID	CCUserBattleArray::UpDataBattleArray(CT_INT32 iPosition[POST_COUNT/2])
{
	_AUTOMUTEX(m_pMutex);
	memcpy(m_iPosition,iPosition,sizeof(m_iPosition));
	//add by caihan
	//设置队形，判断阵型任务
	CT_INT32 i =0;
	for (;i<POST_COUNT/2;i++)
	{
		if (iPosition[i]>0)
		{
			CMD_G_TaskEventInfo taskEventInfo = {0};
			taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_FIGHT;
			taskEventInfo.iTaskSubType = TASK_SUB_TYPE_FIGHT_FORMATION;
			//兵种id
			taskEventInfo.iSubType = iPosition[i]/10;
			m_pUser->m_userTask.DisposeTask(taskEventInfo);
		}
	}
	//end
	return;
}


CT_BOOL	CCUserBattleArray::CheckArmyArray(CT_INT32 iArmyType)
{
	_AUTOMUTEX(m_pMutex);
	for (CT_INT32 i =0; i<POST_COUNT/2;i++)
	{
		if (m_iPosition[i]/10 == iArmyType)
		{
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

CT_VOID CCUserBattleArray::UpdateBattleStatus(MasterCombatStatus &mcs, CT_INT32 iClass, CT_INT32 *arrayBattle, CT_INT32 nCount)
{
    for( CT_INT32 i = 0; i < nCount;++i)
    {
        if(arrayBattle[i] == iClass)
        {
            CCBattleArray::getSingleton().GetBattleArray(i, mcs);
        }
    }
}