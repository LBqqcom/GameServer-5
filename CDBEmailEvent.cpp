#include "CDatabaseMes.h"
#include "CGlobeFunc.h"
#include "CLogMeg.h"
#include "CDBConnector.h"
#include "CGameConfig.h"
#include "CUserManager.h"
#include "CDBEmailEvent.h"
#include "CDBLogMes.h"

CCDBEmailEvent::CCDBEmailEvent(CCGameMesManager *pMesManager,CT_DWORD id,
							   CCDBConnector& dbConnector):CCGameMesEvent(pMesManager,id),m_dbConnector(dbConnector)
{
	RegistMessage(MSC_MAIN_DB_EMAIL_DATA,MSC_SUB_DB_EMAIL_USER);
}

CT_VOID CCDBEmailEvent::OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve)
{
	if (mainID==MSC_MAIN_DB_EMAIL_DATA)
	{
		if (subID==MSC_SUB_DB_EMAIL_USER)
		{
			DisopseUserEmail(pBuf,len);
		}
	}
	else
	{
		CCLogMeg::getSingleton().WriteError("CCDBEmailEvent::OnUserEvent mian id %d",mainID);
		return;
	}
}

CT_VOID CCDBEmailEvent::DisopseUserEmail(CT_CHAR *pBuf,CT_DWORD len)
{
	CT_DWORD aimUserID = 0,userID=0;
	CT_INT32 iRet = 0;
	memcpy(&aimUserID,pBuf,sizeof(CT_DWORD));
	memcpy(&userID,pBuf+sizeof(CT_DWORD)+NAME_SIZE,sizeof(CT_DWORD)); 
	//通过id查找用户
	if (aimUserID>0)
	{
		m_dbConnector.Reset();
		m_dbConnector.SetPcName("SP_FindMasterByID");

		m_dbConnector.AddParm("_uid",aimUserID);

		if(m_dbConnector.Exec())
		{
			CCDBResult* pResult = m_dbConnector.GetResult();
			if (pResult->GetResult())
			{  
				while (pResult->MoveNextRow())
				{
					pResult->GetValue(0,  iRet );
				}
			}
			pResult->Release();
		}
		else
		{
			CCDBLogMes::getSingleton().WriteInfo(userID,"SP_FindMasterByID","_uid:%d",aimUserID);
			G_DBError(m_dbConnector, m_dwThread);
			return;
		}
	}
	//通过昵称找用户
	else
	{
		CT_CHAR cName[NAME_SIZE];
		memset(cName,0,NAME_SIZE);
		memcpy(cName,pBuf+sizeof(CT_DWORD),NAME_SIZE);

		m_dbConnector.Reset();
		m_dbConnector.SetPcName("SP_FindMasterByNickName");

		m_dbConnector.AddParm("_nickName",cName);

		if(m_dbConnector.Exec())
		{
			CCDBResult* pResult = m_dbConnector.GetResult();
			if (pResult->GetResult())
			{  
				while (pResult->MoveNextRow())
				{
					pResult->GetValue(0,  iRet );
					//用户存在，获取用户id,写数据库
					if (iRet>0)
					{
						pResult->GetValue(1, aimUserID );
					}
				}
			}
			pResult->Release();
		}
		else
		{
			CCDBLogMes::getSingleton().WriteInfo(userID,"SP_FindMasterByNickName","_nickName:%s",cName);
			G_DBError(m_dbConnector, m_dwThread);
			return;
		}
	}
	//发回城市线程，iret大于0表示用户存在，邮件发送成功
	MSC_DB_EmailUserResult eur={0};
	eur.dwUserID=userID;
	eur.iRet=iRet;
	CT_DWORD threadid = GAME_THREAD_CITY + userID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	G_SendThreadInfo(threadid,MAIN_G_EMAIL_EVENT_CITY,SUB_G_DB_USER_EMAIL,(CT_CHAR*)&eur,sizeof(MSC_DB_EmailUserResult));
	//iRet大于0，表示用户存在，不在线，写数据库
	if (iRet>0)
	{
		CMD_G_EmailUser eu = {0};
		memcpy(eu.cUserName,pBuf+2*sizeof(CT_DWORD)+NAME_SIZE,NAME_SIZE);
		memcpy(eu.cEmailTitle,pBuf+2*sizeof(CT_DWORD)+2*NAME_SIZE,EMAIL_TITLE_SIZE);
		memcpy(eu.cEmailContent,pBuf+2*sizeof(CT_DWORD)+2*NAME_SIZE+EMAIL_TITLE_SIZE,EMAIL_CONTENT_SIZE);
		m_dbConnector.Reset();
		m_dbConnector.SetPcName("SP_EmailAddUserData");
		m_dbConnector.AddParm("_uid ",aimUserID);
		m_dbConnector.AddParm("_ufid ",userID);     //发件人id
		m_dbConnector.AddParm("_ufname",eu.cUserName);  //发件人昵称
		m_dbConnector.AddParm("_title",eu.cEmailTitle);
		m_dbConnector.AddParm("_info",eu.cEmailContent);

		if(m_dbConnector.Exec())
		{
			CCDBResult* pResult = m_dbConnector.GetResult();
			//pResult->GetResult();
			pResult->Release();
		}else
		{
			CCDBLogMes::getSingleton().WriteInfo(userID,"SP_EmailAddUserData","_uid:%d _ufid:%d _ufname:%s _title:%s _info:%s",aimUserID,userID,eu.cUserName,eu.cEmailTitle,eu.cEmailContent);
			G_DBError(m_dbConnector, m_dwThread);
			return;
		}
	}
}