/********************************************************************
	created:	2012/11/14
	created:	14:11:2012   15:18
	file base:	CDBDailyEvent
	file ext:	cpp
	author:		xingweizhen
	
	purpose:	
*********************************************************************/
#include "CDBDailyEvent.h"
#include "CDatabaseMes.h"
#include "CGlobeFunc.h"
#include "CLogMeg.h"
#include "CDBConnector.h"
#include "CDBLogMes.h"

CCDBDailyEvent::CCDBDailyEvent(CCGameMesManager* pMesManager, CT_DWORD id,
                                 CCDBConnector& dbConnector) : CCGameMesEvent(pMesManager, id), m_dbConnector(dbConnector)
{
    RegistMessage(MSC_MAIN_DB_SAVE_DATA,     MSC_SUB_DB_W_SIGN_INFO);
    RegistMessage(MSC_MAIN_DB_SAVE_DATA,     MSC_SUB_DB_W_DAILY_QUEST);
    RegistMessage(MSC_MAIN_DB_SAVE_DATA,     MSC_SUB_DB_W_ACTIVE_INFO);
    RegistMessage(MSC_MAIN_DB_SAVE_DATA,     MSC_SUB_DB_W_SIGN_REWARD);
    RegistMessage(MSC_MAIN_DB_SAVE_DATA,     MSC_SUB_DB_W_ACTIVE_REWARD);
    RegistMessage(MSC_MAIN_DB_DEL_DATA,      MSC_SUB_DB_D_SIGN_DATA);
    RegistMessage(MSC_MAIN_DB_DEL_DATA,      MSC_SUB_DB_D_DAILY_DATA);
}

CCDBDailyEvent::~CCDBDailyEvent(void)
{
}

//处理领主消息
CT_VOID CCDBDailyEvent::OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
                                     CT_CHAR* pBuf,		CT_DWORD len,
                                     CT_LONGLONG reserve)
{
    switch (mainID)
    {
    case  MSC_MAIN_DB_SAVE_DATA:
        {
            DisposeSaveData(subID, pBuf, len);
            break;
        }
    case  MSC_MAIN_DB_DEL_DATA:
        {
            DisposeDelData(subID, pBuf, len);
            break;
        }
    default:
        {
            CCLogMeg::getSingleton().WriteError("CCDBMasterEvent::OnUserEvent : mainID=%i, subID=%i", mainID, subID);
            break;
        }
    }
}

// 处理信息保存
CT_VOID CCDBDailyEvent::DisposeSaveData(CT_DWORD subID, CT_CHAR* pBuf,	CT_DWORD len)
{
    switch (subID)
    {

    case MSC_SUB_DB_W_SIGN_INFO :
        if (len != sizeof(MSC_DB_W_SignInfo))
        {
            G_DisposeErrorMes(0, subID, "CCDBMasterEvent::OnMasterEvent: len == sizeof(MSC_DB_W_SignInfo)");
            return ;
        }
        DisposeSaveData(*(MSC_DB_W_SignInfo *)pBuf);
        break;
    case MSC_SUB_DB_W_DAILY_QUEST :
        if (len != sizeof(MSC_DB_W_DailyQuest))
        {
            G_DisposeErrorMes(0, subID, "CCDBMasterEvent::OnMasterEvent: len == sizeof(MSC_DB_W_DailyQuest)");
            return ;
        }
        DisposeSaveData(*(MSC_DB_W_DailyQuest*)pBuf);
        break;
    case MSC_SUB_DB_W_ACTIVE_INFO :
        if (len != sizeof(MSC_DB_W_ActiveInfo))
        {
            G_DisposeErrorMes(0, subID, "CCDBMasterEvent::OnMasterEvent: len == sizeof(MSC_DB_W_ActiveInfo)");
            return ;
        }
        DisposeSaveData(*(MSC_DB_W_ActiveInfo*)pBuf);
        break;
    /*case MSC_SUB_DB_W_SIGN_REWARD :
        if (len != sizeof(MSC_DB_W_SignReward))
        {
            G_DisposeErrorMes(0, subID, "CCDBMasterEvent::OnMasterEvent: len == sizeof(MSC_DB_W_SignReward)");
            return ;
        }
        DisposeSaveData(*(MSC_DB_W_SignReward*)pBuf);
        break;*/
    case MSC_SUB_DB_W_ACTIVE_REWARD :
        if (len != sizeof(MSC_DB_W_ActiveReward))
        {
            G_DisposeErrorMes(0, subID, "CCDBMasterEvent::OnMasterEvent: len == sizeof(MSC_DB_W_ActiveReward)");
            return ;
        }
        DisposeSaveData(*(MSC_DB_W_ActiveReward*)pBuf);
        break;
    }
}
// 处理信息删除
CT_VOID CCDBDailyEvent::DisposeDelData(CT_DWORD subID, CT_CHAR* pBuf,	CT_DWORD len)
{
    switch (subID)
    {
    case MSC_SUB_DB_D_SIGN_DATA: 
        if (len != sizeof(MSC_DB_D_SignInfo))
        {
            G_DisposeErrorMes(0, subID, "CCDBMasterEvent::OnMasterEvent: len == sizeof(MSC_DB_D_SignInfo)");
            return ;
        }
        DisposeDelData(*(MSC_DB_D_SignInfo *)pBuf);
        break;
    case MSC_SUB_DB_D_DAILY_DATA: 
        if (len != sizeof(MSC_DB_D_DailyInfo))
        {
            G_DisposeErrorMes(0, subID, "CCDBMasterEvent::OnMasterEvent: len == sizeof(MSC_DB_D_DailyInfo)");
            return ;
        }
        DisposeDelData(*(MSC_DB_D_DailyInfo *)pBuf);
        break;
    }
}
//////////////////////////////////////////////////////////////////////////
// 记录签到信息
CT_VOID CCDBDailyEvent::DisposeSaveData(const MSC_DB_W_SignInfo &info)
{
    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_EveryDayUpdateLoginIn");

    m_dbConnector.AddParm("_uid", info.dwUserID);
    m_dbConnector.AddParm("_lastingLoginInTime", info.nSigned);
    m_dbConnector.AddParm("_bGetAward ", info.bReward);

    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult)
        {
            //pResult->GetResult();
            pResult->Release();
        }
    }
    else
    {
		CCDBLogMes::getSingleton().WriteInfo(info.dwUserID,"SP_EveryDayUpdateLoginIn","_uid:%d _lastingLoginInTime:%d _bGetAward:%d",info.dwUserID,info.nSigned,info.bReward);
        G_DBError(m_dbConnector, m_dwThread);
        return;
    }

}
// 记录日常进度
CT_VOID CCDBDailyEvent::DisposeSaveData(const MSC_DB_W_DailyQuest &daily)
{
    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_EveryDayUpdateTask");

    m_dbConnector.AddParm("_uid", daily.dwUserID);
    m_dbConnector.AddParm("_taskID", daily.iQuestID);
    m_dbConnector.AddParm("_finishTimes", daily.iCurrent);

    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult)
        {
            //pResult->GetResult();
            pResult->Release();
        }
    }
    else
    {
		CCDBLogMes::getSingleton().WriteInfo(daily.dwUserID,"SP_EveryDayUpdateTask","_uid:%d _taskID:%d _finishTimes:%d",daily.dwUserID,daily.iQuestID,daily.iCurrent);
        G_DBError(m_dbConnector, m_dwThread);
        return;
    }    
}
// 记录活跃度信息
CT_VOID CCDBDailyEvent::DisposeSaveData(const MSC_DB_W_ActiveInfo &info)
{
    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_EveryDayUpdateVitality");

    m_dbConnector.AddParm("_uid", info.dwUserID);
    m_dbConnector.AddParm("_VitalityGrade", info.iLevel);
    m_dbConnector.AddParm("_VitalityXp", info.iActive);

    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult)
        {
            //pResult->GetResult();
            pResult->Release();
        }
    }
    else
    {
		CCDBLogMes::getSingleton().WriteInfo(info.dwUserID,"SP_EveryDayUpdateVitality","_uid:%d _VitalityGrade:%d _VitalityXp:%d",info.dwUserID,info.iLevel,info.iActive);
        G_DBError(m_dbConnector, m_dwThread);
        return;
    }    
}

// 记录领取活跃度奖励
CT_VOID CCDBDailyEvent::DisposeSaveData(const MSC_DB_W_ActiveReward &reward)
{
    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_EveryDayVitalityGradeUpdate");

    m_dbConnector.AddParm("_uid", reward.dwUserID);
    m_dbConnector.AddParm("_vitalityGrade", reward.iLevel);

    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult)
        {
            //pResult->GetResult();
            pResult->Release();
        }
    }
    else
    {
		CCDBLogMes::getSingleton().WriteInfo(reward.dwUserID,"SP_EveryDayVitalityGradeUpdate","_uid:%d _vitalityGrade:%d ",reward.dwUserID,reward.iLevel);
        G_DBError(m_dbConnector, m_dwThread);
        return;
    }
}

// 清除连续登陆记录
CT_VOID CCDBDailyEvent::DisposeDelData(const MSC_DB_D_SignInfo &info)
{
    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_EveryDayClearLoginInTimes");

    m_dbConnector.AddParm("_uid", info.dwUserID);

    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult)
        {
            //pResult->GetResult();
            pResult->Release();
        }
    }
    else
    {
		CCDBLogMes::getSingleton().WriteInfo(info.dwUserID,"SP_EveryDayClearLoginInTimes","_uid:%d",info.dwUserID);
        G_DBError(m_dbConnector, m_dwThread);
        return;
    }
}
// 清除日常记录
CT_VOID CCDBDailyEvent::DisposeDelData(const MSC_DB_D_DailyInfo &info)
{
    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_EveryDayClearTask");

    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult)
        {
            //pResult->GetResult();
            pResult->Release();
        }
    }
    else
    {
        G_DBError(m_dbConnector, m_dwThread);
        return;
    }
}