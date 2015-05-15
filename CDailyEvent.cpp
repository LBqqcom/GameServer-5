/********************************************************************
	created:	2012/11/14
	created:	14:11:2012   15:19
	file base:	CDailyEvent
	file ext:	cpp
	author:		xingweizhen
	
	purpose:	
*********************************************************************/
#include "CDailyEvent.h"
#include "CMD_Game.h"
#include "CUserManager.h"
#include "CMasterScriptSetting.h"
#include "CGameConfig.h"
#include "CThreadMes.h"
#include "CGlobeData.h"
#include "CGlobeFunc.h"
#include "CDBMasterEvent.h"

CCDailyEvent::CCDailyEvent(CCGameMesManager* pMesManager, CT_DWORD id) : CCGameMesEvent(pMesManager, id)
{
    RegistMessage(MAIN_G_DAILY_EVENT, SUB_G_SIGN_REWARD);
    RegistMessage(MAIN_G_DAILY_EVENT, SUB_G_DAILY_REWARD);

    // ��ʱ��
    RegistClock(IDI_DAILY_DATA_RESET_CLOCK);
    SetClock(IDI_DAILY_DATA_RESET_CLOCK, 0, 0, 0);
}

CCDailyEvent::~CCDailyEvent(void)
{
}

//�����û���Ϣ
CT_VOID CCDailyEvent::OnUserEvent(CT_DWORD mainID,	CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve)
{
    switch (mainID)
    {
    case MAIN_G_DAILY_EVENT:
        DisposeDailyEvent(subID,  pBuf, len);
        break;
    }
}

//�û���ʱ������
CT_VOID CCDailyEvent::OnTime(CT_DWORD timeID)
{

}

//�û�ʱ�Ӵ���
CT_VOID CCDailyEvent::OnClock(CT_DWORD timeID)
{
    switch (timeID)
    {
    case IDI_DAILY_DATA_RESET_CLOCK:
        // �����¼����TODO ����������Ҳ��Ҫ�������
        MSC_DB_D_DailyInfo stDb = {0};
        G_SendThreadInfo(GAME_THREAD_DATABASE, MSC_MAIN_DB_DEL_DATA, MSC_SUB_DB_D_DAILY_DATA, (CT_CHAR*)&stDb, sizeof(stDb));
        CCUserManager::getSingleton().UpdateSignIn(m_dwThread - GAME_THREAD_CITY, ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum);
        break;
    }
}

// �����ճ��¼�
CT_VOID CCDailyEvent::DisposeDailyEvent(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len)
{
    switch (subID)
    {
    case SUB_G_SIGN_REWARD:
        if (sizeof(CMD_G_SignReward) == len) DisposeSignReward(*(CMD_G_SignReward *)pBuf);
        break;
    case SUB_G_DAILY_REWARD:
        if (sizeof(CMD_G_ActiveReward) == len) DisposeDailyReward(*(CMD_G_ActiveReward *)pBuf);
        break;
    }
}

// ������ȡǩ������
CT_VOID CCDailyEvent::DisposeSignReward(const CMD_G_SignReward &reward)
{
    CCUser* pUser = 0;
    ENUM_SignRewardFail enFail = ESR_DATA_ERROR;

    if(CCUserManager::getSingleton().Find(reward.dwUserID, &pUser))
    {
		if (reward.iDiamond>0 && pUser->m_userVip.VipGradeGet()<=0)
		{
			enFail = ESR_NOTVIP;
		}
        MasterData md = {0};
        pUser->m_userBase.GetMasterData(md);
        enFail = pUser->m_userDaily.RecvSignReward(m_dwThread, reward.iSigned, reward.iDiamond);
        if (enFail != ESR_SUCCESS)
        {
            CMD_G_SignRewardFail stFail = {0};
            stFail.dwUserID = pUser->GetUserID();
            stFail.enFail = enFail;
            pUser->GetNetInterface()->Send(MAIN_G_DAILY_EVENT, SUB_G_SIGN_REWARD_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);
        }
    }
}

// ������ȡ�ճ�����
CT_VOID CCDailyEvent::DisposeDailyReward(const CMD_G_ActiveReward &reward)
{
    CCUser* pUser = 0;
    ENUM_ActiveRewardFail enFail = EAR_DATA_ERROR;

    if(CCUserManager::getSingleton().Find(reward.dwUserID, &pUser))
    {
        MasterData md = {0};
        pUser->m_userBase.GetMasterData(md);
        enFail = pUser->m_userDaily.RecvActiveReward(m_dwThread, reward.iLevel);
        if (enFail != EAR_SUCCESS)
        {
            CMD_G_ActiveRewardFail stFail = {0};
            stFail.dwUserID = pUser->GetUserID();
            stFail.enFail = enFail;
            pUser->GetNetInterface()->Send(MAIN_G_DAILY_EVENT, SUB_G_DAILY_REWARD_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);
        }
    }
}