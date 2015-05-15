#include "CDBCityEvent.h"
#include "CDBConnector.h"
#include "CDatabaseMes.h"
#include "CGlobeFunc.h"
#include "CDBLogMes.h"
#include "CBroadcastScriptSetting.h"

CCDBCityEvent::CCDBCityEvent(CCGameMesManager* pMesManager, CT_DWORD id,
									 CCDBConnector& dbConnector) : CCGameMesEvent(pMesManager, id), m_dbConnector(dbConnector)
{
	RegistMessage(MSC_MAIN_DB_BUILDING_EVENT,  MSC_SUB_DB_SAVE_AWARD_DATA);
    RegistMessage(MSC_MAIN_DB_ANNOUNCEMENT, 0);
}

CCDBCityEvent::~CCDBCityEvent(void)
{
}

//处理用户消息
CT_VOID CCDBCityEvent::OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
									   CT_CHAR* pBuf,		CT_DWORD len,
									   CT_LONGLONG reserve)
{
	switch (mainID)
	{
	case  MSC_MAIN_DB_BUILDING_EVENT:
		{
			DisposeBuildingEvent(subID, pBuf,  len);
			break;
		}
	case MSC_MAIN_DB_ANNOUNCEMENT:
		{
			UpdataAnnouncement();
			break;
		}
	default:
		break;
	}
}

//处理建筑事件
CT_VOID	CCDBCityEvent::DisposeBuildingEvent(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len)
{
	switch (subID)
	{
	case  MSC_SUB_DB_SAVE_AWARD_DATA:
		{
            MSC_DB_EventAward* pEventAward = (MSC_DB_EventAward*)pBuf;
			DisposeBuildingEventAward(*pEventAward);
			break;
		}
	default:
		break;
	}
}

//处理建筑奖励
CT_VOID CCDBCityEvent::DisposeBuildingEventAward(const MSC_DB_EventAward& eventAward)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_SaveEventFinishRecord");

	m_dbConnector.AddParm("_uid",   eventAward.dwUserID);
	m_dbConnector.AddParm("_food",  eventAward.iFood);
	m_dbConnector.AddParm("_gold",  eventAward.iGold);
	m_dbConnector.AddParm("_civiliXp", eventAward.iCiviliNum);

	m_dbConnector.AddParm("prop1Name", (CT_CHAR*)eventAward.cProp1Name );
	m_dbConnector.AddParm("prop1",	   eventAward.iProp1);
	m_dbConnector.AddParm("prop1num",  eventAward.iPropNum1);
	m_dbConnector.AddParm("prop2Name", (CT_CHAR*)eventAward.cProp2Name);
	m_dbConnector.AddParm("prop2",     eventAward.iProp2);
	m_dbConnector.AddParm("prop2num",  eventAward.iPropNum2);
			
	if(m_dbConnector.Exec())
	{
		CT_INT32 ret = 0;
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0,  ret );
			}
		}
		pResult->Release();

		//发送给gm数据库

	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(eventAward.dwUserID,"SP_SaveEventFinish","_uid:%d _food:%d _gold:%d _civiliXp:%d prop1Name:%s prop1:%d prop2num:%d prop1Name:%s prop2:%d prop2num:%d",\
										eventAward.dwUserID,eventAward.iFood,eventAward.iGold,eventAward.iCiviliNum,(CT_CHAR*)eventAward.cProp1Name,eventAward.iProp1,eventAward.iPropNum1,(CT_CHAR*)eventAward.cProp2Name,eventAward.iProp2,eventAward.iPropNum2);
		G_DBError(m_dbConnector, m_dwThread);
	}
}

CT_VOID CCDBCityEvent::UpdataAnnouncement()
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_Announcement");

	if (m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{
			while(pResult->MoveNextRow())
			{
				Announcement anno = {0};
				pResult->GetValue(0, anno.iIndex);
				pResult->GetValue(1, anno.cContent, ANNOUNCEMENT_CONTENT_LEN);
				pResult->GetValue(2, anno.iFlag);

				if (anno.iFlag != 0)
				{
					G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_ANNOUNCEMENT_UPDATA, 0, (CT_CHAR*)(&anno), sizeof(Announcement));
				}
			}
		}
		pResult->Release();
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
	}
}