#include "CDBCSFightEvent.h"
#include "CLogMeg.h"
#include "CCSFightData.h"
#include "CDBConnector.h"

CCDBCSFightEvent::CCDBCSFightEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector): 
CCGameMesEvent(pMesManager, id), m_dbConnector(dbConnector)
{
	RegistMessage(MSC_MAIN_DB_CORSS_FIGHT,MSC_SUB_DB_CS_REWARD);
	RegistMessage(MSC_MAIN_DB_CORSS_FIGHT,MSC_SUB_DB_CS_SUPPORT);
	RegistMessage(MSC_MAIN_DB_CORSS_FIGHT,MSC_SUB_DB_DELREWARD);
}

CCDBCSFightEvent::~CCDBCSFightEvent(void)
{
}

//处理用户消息
CT_VOID CCDBCSFightEvent::OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
							CT_CHAR* pBuf,		CT_DWORD len,
							CT_LONGLONG reserve)
{
	if(MSC_MAIN_DB_CORSS_FIGHT != mainID)
	{
		CCLogMeg::getSingleton().WriteError("CCDBCSFightEvent::OnUserEvent MSC_MAIN_DB_CORSS_FIGHT != mainID");
		return;
	}

	switch(subID)
	{
	case MSC_SUB_DB_CS_REWARD:
		{
			DisposeDBCSReward(pBuf,len);
			break;
		}
	case MSC_SUB_DB_CS_SUPPORT:
		{
			DisposeCSSupport(pBuf,len);
			break;
		}
	case MSC_SUB_DB_DELREWARD:
		{
			DisposeDelReward(pBuf,len);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCDBCSFightEvent::OnUserEvent subid:%d",subID);
			return;
		}
	}
	return;
}

CT_VOID CCDBCSFightEvent::DisposeDBCSReward(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_CS_REWARD) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCDBCSFightEvent::DisposeDBCSReward sizeof(MSC_DB_CS_REWARD) != iLen");
		return;
	}

	MSC_DB_CS_REWARD *pCSr=(MSC_DB_CS_REWARD*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_CrossServerArwardUpdate");

	m_dbConnector.AddParm("_uid", pCSr->dwUserID);
	m_dbConnector.AddParm("_restype", pCSr->iResType);
	m_dbConnector.AddParm("_resid", pCSr->iResID);
	m_dbConnector.AddParm("_resnum", pCSr->iResNum);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
	}
	return;
}

//个人支持
CT_VOID CCDBCSFightEvent::DisposeCSSupport(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_CS_SUPPORT) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCDBCSFightEvent::DisposeCSSupport sizeof(MSC_DB_CS_SUPPORT) != iLen");
		return;
	}

	MSC_DB_CS_SUPPORT *pCSS=(MSC_DB_CS_SUPPORT*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_CrossServerUserSupportUpdate");

	m_dbConnector.AddParm("_uid", pCSS->dwUserID);
	m_dbConnector.AddParm("_FTID", pCSS->dwFTID);
	m_dbConnector.AddParm("_GSID", pCSS->iGSID);
	m_dbConnector.AddParm("_resid", pCSS->iResID);
	m_dbConnector.AddParm("_num", pCSS->iNum);
	m_dbConnector.AddParm("_index", pCSS->iIndex);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
	}

	return;

}

//删除奖励
CT_VOID CCDBCSFightEvent::DisposeDelReward(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_CS_DelReward) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCDBCSFightEvent::DisposeDelReward sizeof(MSC_DB_CS_DelReward) != iLen");
		return;
	}

	MSC_DB_CS_DelReward *p=(MSC_DB_CS_DelReward*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_CrossServerArwardDelete");

	m_dbConnector.AddParm("_uid", p->dwUserID);
	m_dbConnector.AddParm("_FTID", p->iResType);
	m_dbConnector.AddParm("_GSID", p->iResID);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
	}
	return;
}

//读取奖励
CT_VOID CCDBCSFightEvent::ReadCSReward()
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_CrossServerArwardRead");

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while(pResult->MoveNextRow())
			{
				CT_DWORD dwUserID=0;
				CCCSFightData::CSUserReward csur={0};
				pResult->GetValue(0, dwUserID);
				pResult->GetValue(1, csur.iType);
				pResult->GetValue(2, csur.iResID);
				pResult->GetValue(3, csur.iNum);

				CCCSFightData::getSingleton().InsertReward(dwUserID,csur,CT_FALSE);
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
	}
	return;
}

//
CT_VOID CCDBCSFightEvent::ReadCSSupport()
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_CrossServerUserSupportRead");

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while(pResult->MoveNextRow())
			{
				CT_DWORD dwUserID=0;
				CCCSFightData::CSUserSupport csus={0};
				pResult->GetValue(0, csus.dwUserID);
				pResult->GetValue(1, csus.dwFTID);
				pResult->GetValue(2, csus.iGSID);
				pResult->GetValue(3, csus.iResID);
				pResult->GetValue(4, csus.iNum);
				pResult->GetValue(5, csus.iIndex);

				CCCSFightData::getSingleton().InsertSupport(csus);
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
	}
}
