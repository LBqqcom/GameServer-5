#include "CDatabaseMes.h"
#include "CGlobeFunc.h"
#include "CLogMeg.h"
#include "CDBConnector.h"
#include "CDBTaskEvent.h"
#include "CDBLogMes.h"
#include "CDBActiveEvent.h"
#include <vector>
#include "CActive.h"
#include "CGameConfig.h"
#include "CUserManager.h"

CCDBActiveEvent::CCDBActiveEvent(CCGameMesManager* pMesManager, CT_DWORD id, 
								 CCDBConnector& dbConnector):CCGameMesEvent(pMesManager,id),m_dbConnector(dbConnector)
{
	RegistMessage(MSC_MAIN_DB_ACTIVE_DATA,MSC_SUB_ACTIVE_STATUS_UPDATE);
	RegistMessage(MSC_MAIN_DB_ACTIVE_DATA,MSC_SUB_ACTIVE_STATUS_UPDATEEX);
	RegistMessage(MSC_MAIN_DB_ACTIVE_DATA,MSC_SUB_TURNTABLE_UPDATE);
	RegistMessage(MSC_MAIN_DB_ACTIVE_DATA,MSC_SUB_CDKEY_GET);
	RegistMessage(MSC_MAIN_DB_ACTIVE_DATA,MSC_SUB_CDKEY_UPDATE);
	RegistMessage(MSC_MAIN_DB_ACTIVE_DATA,MSC_SUB_RES_PRODUCT_REWARD);

	if (GAME_THREAD_DATABASE == m_dwThread)
	{
		RegistMessage(MSC_MAIN_DB_ACTIVE_DATA,MSC_SUB_ACTIVE_DB_SYS);
	}

	G_ValueRange vr={0};
	if (G_GetValueRange(VALUE_TYPE_ACTIVE_EVENT_CLOCK,&vr))
	{
		RegistClock(IDI_ACTIVE_EVENT_CLOCK);
		SetClock(IDI_ACTIVE_EVENT_CLOCK, (CT_BYTE)vr.fMaxVal, 0, 0);
	}
	memset(&vr,0,sizeof(vr));
	if (G_GetValueRange(VALUE_TYPE_ACTIVE_GET_TIMER,&vr))
	{
		if (m_dwThread == GAME_THREAD_DATABASE)
		{
			RegistTimer(IDI_ACTIVE_EVENT_GET_TIMER);
			SetTimer(IDI_ACTIVE_EVENT_GET_TIMER,(CT_DWORD)(vr.fMaxVal*1000));
		}
	}
}

CCDBActiveEvent::~CCDBActiveEvent()
{

}

CT_VOID CCDBActiveEvent::OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve)
{
	switch(mainID)
	{
	case MSC_MAIN_DB_ACTIVE_DATA:
		{
			if (subID==MSC_SUB_ACTIVE_STATUS_UPDATE)
			{
				if (len != sizeof(MSC_DB_ActiveEventStatus))
				{
					G_DisposeErrorMes(0, subID, "CCDBActiveEvent::OnUserEvent: len == sizeof(MSC_DB_ActiveEventStatus)");
					return ;
				}
				MSC_DB_ActiveEventStatus *p = (MSC_DB_ActiveEventStatus *)pBuf;
				UpdateActiveStatus(*p);
				break;
			}
			else if(subID==MSC_SUB_ACTIVE_STATUS_UPDATEEX)
			{
				if (len != sizeof(MSC_DB_ActiveEventStatus))
				{
					G_DisposeErrorMes(0, subID, "CCDBActiveEvent::OnUserEvent: len == sizeof(MSC_DB_ActiveEventStatus) ex");
					return ;
				}
				MSC_DB_ActiveEventStatus *p = (MSC_DB_ActiveEventStatus *)pBuf;
				UpdateActiveStatusEx(*p);
				break;
			}
			if (MSC_SUB_TURNTABLE_UPDATE==subID)
			{
				if (len != sizeof(MSC_DB_Turntable))
				{
					G_DisposeErrorMes(0, subID, "CCDBActiveEvent::OnUserEvent: len == sizeof(MSC_DB_Turntable)");
					return ;
				}
				MSC_DB_Turntable *p = (MSC_DB_Turntable *)pBuf;
				UpdateTurntable(*p);
				break;
			}
			if (MSC_SUB_CDKEY_GET==subID)
			{
				if (len != sizeof(MSC_DB_CDKEY))
				{
					G_DisposeErrorMes(0, subID, "CCDBActiveEvent::OnUserEvent: len == sizeof(MSC_DB_CDKEY)");
					return ;
				}
				MSC_DB_CDKEY *p = (MSC_DB_CDKEY *)pBuf;
				GetCDKEY(*p);
				break;
			}
			if (MSC_SUB_CDKEY_UPDATE==subID)
			{
				if (len != sizeof(MSC_DB_CDKEY))
				{
					G_DisposeErrorMes(0, subID, "CCDBActiveEvent::OnUserEvent: len == sizeof(MSC_DB_CDKEY)");
					return ;
				}
				MSC_DB_CDKEY *p = (MSC_DB_CDKEY *)pBuf;
				UpdateCDKEY(*p);
				break;
			}
			if (MSC_SUB_RES_PRODUCT_REWARD == subID)
			{
				if (len != sizeof(MSC_DB_RESREWARD))
				{
					G_DisposeErrorMes(0, subID, "CCDBActiveEvent::OnUserEvent: len == sizeof(MSC_DB_RESREWARD)");
					return ;
				}
				MSC_DB_RESREWARD *p = (MSC_DB_RESREWARD *)pBuf;
				DisposeResReward(*p);
				break;
			}
			

			if (MSC_SUB_ACTIVE_DB_SYS == subID)
			{
				SysServerActive(pBuf, len);
				break;
			}
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCDBTaskEvent::OnUserEvent : mainID=%i, subID=%i", mainID, subID);
			break;
		}
	}
}

CT_VOID CCDBActiveEvent::OnClock(CT_DWORD timeID)
{
	if (timeID==IDI_ACTIVE_EVENT_CLOCK)
	{
		UpdateServerActive();
		UpdateGMList();
	}
}

CT_VOID CCDBActiveEvent::OnTime(CT_DWORD timeID)
{
	if (timeID==IDI_ACTIVE_EVENT_GET_TIMER)
	{
		if (m_dwThread==GAME_THREAD_DATABASE)
		{
			KillTimer(IDI_ACTIVE_EVENT_GET_TIMER);
			UpdateServerActive();
			UpdateGMList();
		}
	}
}

CT_VOID CCDBActiveEvent::UpdateActiveStatus(MSC_DB_ActiveEventStatus &activeStatus)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ActiveEventStatusUpdate");
	m_dbConnector.AddParm("_uid",activeStatus.dwUserID);
	m_dbConnector.AddParm("_aid",activeStatus.iAID);
	m_dbConnector.AddParm("_status",activeStatus.iStatus);
	m_dbConnector.AddParm("_quanity",activeStatus.iQuantity);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		//pResult->GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(activeStatus.dwUserID,"SP_ActiveEventStatusUpdate","_uid:%d _aid:%d _status:%d _quanity:%d ",\
			activeStatus.dwUserID,activeStatus.iAID,activeStatus.iStatus,activeStatus.iQuantity);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCDBActiveEvent::UpdateActiveStatusEx(MSC_DB_ActiveEventStatus &activeStatus)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ActiveEventStatusUpdateEx");
	m_dbConnector.AddParm("_uid",activeStatus.dwUserID);
	m_dbConnector.AddParm("_aid",activeStatus.iAID);
	m_dbConnector.AddParm("_status",activeStatus.iStatus);
	m_dbConnector.AddParm("_quanity",activeStatus.iQuantity);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		//pResult->GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCDBActiveEvent::UpdateTurntable(MSC_DB_Turntable &turntable)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TurnTableUpdate");
	m_dbConnector.AddParm("_uid",turntable.dwUserID);
	m_dbConnector.AddParm("_times1",turntable.iTimes[0]);
	m_dbConnector.AddParm("_times2",turntable.iTimes[1]);
	m_dbConnector.AddParm("_times3",turntable.iTimes[2]);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(turntable.dwUserID,"SP_TurnTableUpdate","_uid:%d ... ",turntable.dwUserID);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCDBActiveEvent::GetCDKEY(MSC_DB_CDKEY &cdkey)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_CDKEYGet");
	m_dbConnector.AddParm("_cdkey",cdkey.cCDKEY);
	m_dbConnector.AddParm("_uid",cdkey.dwUID);
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			if (pResult->MoveNextRow())
			{
				pResult->GetValue(0,cdkey.iType);
			}
		}
		//获取删除的活动，删除用户活动状态表对应的活动
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(cdkey.dwUID,"SP_CDKEYGet","_cdkey:%s ... ",cdkey.cCDKEY);
		G_DBError(m_dbConnector, m_dwThread);
	}
	CT_DWORD threadid = GAME_THREAD_CITY + cdkey.dwUID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	G_SendThreadInfo(threadid,MSC_MAIN_ACTIVE_EVENT_DATA,MSC_SUB_CDKEY_GET,(CT_CHAR*)&cdkey,sizeof(MSC_DB_CDKEY));					

}

CT_VOID CCDBActiveEvent::UpdateCDKEY(MSC_DB_CDKEY &cdkey)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_CDKEYUpdate");
	m_dbConnector.AddParm("_cdkey",cdkey.cCDKEY);
	m_dbConnector.AddParm("_uid",cdkey.dwUID);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(cdkey.dwUID,"SP_CDKEYUpdate","_cdkey:%s,_uid:%d ... ",cdkey.cCDKEY, cdkey.dwUID);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}

}

CT_VOID CCDBActiveEvent::UpdateServerActive()
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ActiveIDGet");
	std::vector<CT_INT32>		v_active;
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				CT_INT32 iAID = 0;
				pResult->GetValue(1,iAID);
				v_active.push_back(iAID);
			}
		}
		pResult->Release();

		size_t num = v_active.size();
		CT_CHAR* pNumArr = (CT_CHAR*)new CT_INT32[num];
		for (size_t i=0; i<num; ++i )
		{
			memcpy(pNumArr + sizeof(CT_INT32)*i, &v_active[i], sizeof(CT_INT32) );
		}
		G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_ACTIVE_EVENT_DATA, MSC_SUB_ACTIVE_GET, pNumArr, num*sizeof(CT_INT32) );
		delete[] pNumArr;
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}

	// 转盘自定义奖池
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TurnTableRelationGet");
	if(m_dbConnector.Exec())
	{
		CT_INT32 i = 0;
		MSC_DB_TurntableRelation ttr[TURNTABLE_ITEM_COUNT*3] = {0};
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow() && i < TURNTABLE_ITEM_COUNT*3)
			{
				pResult->GetValue(0,ttr[i].iTurntableID);
				pResult->GetValue(1,ttr[i].iPos);
				pResult->GetValue(2,ttr[i].iWeight);
				pResult->GetValue(3,ttr[i].iRewardID);
				pResult->GetValue(4,ttr[i].iFakeProID);
				pResult->GetValue(5,ttr[i].iType);
				pResult->GetValue(6,ttr[i].iID);
				pResult->GetValue(7,ttr[i++].iCount);
			}
		}
		pResult->Release();
		G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_ACTIVE_EVENT_DATA, MSC_SUB_TURNTABLE_REL, (CT_CHAR*)ttr, sizeof(ttr) );
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
	}

	// 活动自定义奖池
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ActiveRewardGet");
	if(m_dbConnector.Exec())
	{
		CT_INT32 i = 0;
		G_ActiveEventReward aer = {0};
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0,aer.iAEID);
				pResult->GetValue(1,aer.iGold);
				pResult->GetValue(2,aer.iFood);
				pResult->GetValue(3,aer.iXP);
				pResult->GetValue(4,aer.iTalent);
				pResult->GetValue(5,aer.iPrestige);
				pResult->GetValue(6,aer.iStamina);
				pResult->GetValue(7,aer.iPropID1);
				pResult->GetValue(8,aer.iPropNum1);
				pResult->GetValue(9,aer.iPropID2);
				pResult->GetValue(10,aer.iPropNum2);
				pResult->GetValue(11,aer.iPropID3);
				pResult->GetValue(12,aer.iPropNum3);
				pResult->GetValue(13,aer.iPropID4);
				pResult->GetValue(14,aer.iPropNum4);
				pResult->GetValue(15,aer.iDiamod);
				G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_ACTIVE_EVENT_DATA, MSC_SUB_ACTIVE_REWARD_REL, (CT_CHAR*)&aer, sizeof(G_ActiveEventReward) );
			}
			G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_ACTIVE_EVENT_DATA, MSC_SUB_ACTIVE_REWARD_COMPLETE, 0, 0);
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
	}

	CT_INT32 iSize=0;
	CT_CHAR cBuf[10240]={0};
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ActiveDescpGet");
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{
			while (pResult->MoveNextRow())
			{
				ActiveDsc ad={0};
				pResult->GetValue(0,ad.iType);
				pResult->GetValue(1,ad.cBuf,DESCR_LENEX);

				if(10240 > iSize+sizeof(ad))
				{
					memcpy(cBuf+iSize,&ad,sizeof(ad));
					iSize+=sizeof(ad);
				}
				else
					CCLogMeg::getSingleton().WriteError("CCDBActiveEvent::SysServerActive 10240 > iSize+sizeof(ad)");
			}
			G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_ACTIVE_EVENT_DATA, MSC_SUB_ACTIVE_DSC_GET, cBuf,iSize);
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
	}
}

CT_VOID CCDBActiveEvent::UpdateGMList()
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_GMListGet");
	if(m_dbConnector.Exec())
	{
		CCUserManager::getSingleton().ClearGMList();
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				CT_DWORD dwGM = 0;
				pResult->GetValue(0,dwGM);
				CCUserManager::getSingleton().InsertGMList(dwGM);
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}

}

//同步数据数据 彭文整理
CT_VOID CCDBActiveEvent::SysServerActive(CT_CHAR* pBuf, CT_DWORD len)
{
	if (sizeof(MSC_DB_ActiveEventChange) != len )
	{
		return;
	}

	MSC_DB_ActiveEventChange* pActiveDelete = (MSC_DB_ActiveEventChange*)pBuf;
	for (CT_INT32 i=0;i<pActiveDelete->iNum;++i)
	{
		m_dbConnector.Reset();
		m_dbConnector.SetPcName("SP_ActiveDel");
		m_dbConnector.AddParm("_aid",pActiveDelete->iActive[i]);

		if(m_dbConnector.Exec())
		{
			CCDBResult* pResult = m_dbConnector.GetResult();
			//pResult->GetResult();
			pResult->Release();
		}else
		{
			G_DBError(m_dbConnector, m_dwThread);
			return;
		}
	}
}

CT_VOID	 CCDBActiveEvent::DisposeResReward( MSC_DB_RESREWARD &resReward)
{
	//in _uid int, in _pid int, in _pcount int, in _slotid int
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PresentAdd");
	m_dbConnector.AddParm("_uid",  resReward.dwUID);
	m_dbConnector.AddParm("_pid",  resReward.iPID);
	m_dbConnector.AddParm("_pcount",  resReward.iPNum);
	m_dbConnector.AddParm("_slotid",  1000000);
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(resReward.dwUID,"SP_PresentAdd","_uid:%d",resReward.dwUID);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PresentGet");
	m_dbConnector.AddParm("_uid", resReward.dwUID);

	MSC_DB_Present	dbPresent = {0};
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, dbPresent.dwUID );
				pResult->GetValue(1, dbPresent.iIndex );
				pResult->GetValue(2, dbPresent.iP_ID);
				pResult->GetValue(3, dbPresent.iP_Count);
				pResult->GetValue(4, dbPresent.iSLotID);
				pResult->GetValue(5, dbPresent.cDesc, 255);

				CT_DWORD threadid = GAME_THREAD_CITY + resReward.dwUID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid,MSC_MAIN_ACTIVE_EVENT_DATA,MSC_SUB_PRESEND_ADD,(CT_CHAR*)&dbPresent,sizeof(MSC_DB_Present));					
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
	}
}