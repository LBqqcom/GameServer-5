#include <cstdlib>
#include <ctime>
#include "CMD_Game.h"
#include "CMD_Center.h"
#include "CDatabaseMes.h"
#include "CUserManager.h"
#include "CGlobeFunc.h"
#include "CLogMeg.h"
#include "CGlobeData.h"
#include "CGameConfig.h"
#include "GlobeScriptInterface.h"
#include "CEmailEvent.h"


#define  MESS_LEN   (1024)

CCEmailEvent::CCEmailEvent(CCGameMesManager *pMesManager,CT_DWORD id):CCGameMesEvent(pMesManager,id)
{
	RegistMessage(MAIN_G_EMAIL_EVENT_CITY,SUB_G_USER_EMAIL);
	RegistMessage(MAIN_G_EMAIL_EVENT_CITY,SUB_G_DB_USER_EMAIL);
}

CCEmailEvent::~CCEmailEvent()
{
	
}

CT_VOID CCEmailEvent::OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve)
{
	if (mainID==MAIN_G_EMAIL_EVENT_CITY)
	{
		if (subID==SUB_G_USER_EMAIL)
		{
			DisposeUserEmail(pBuf,len);
		}
		else if (subID==SUB_G_DB_USER_EMAIL)
		{
			GetEmailSendResult(pBuf,len);
		}
		else
		{
			CCLogMeg::getSingleton().WriteError("CCEmailEvent::OnUserEvent subid %d",subID);
			return;
		}
	}
	else
	{
		CCLogMeg::getSingleton().WriteError("CCEmailEvent::OnUserEvent main id %d",mainID);
		return;
	}
}

CT_VOID CCEmailEvent::TransformMes(CT_CHAR *pBuf,CT_INT32 len,CT_DWORD aimUserID)
{
	CMD_G_EmailUserSendBack eusb = {0};
	CT_INT32 iMesLen = 0;
	eusb.dwAimUserID = aimUserID;
	iMesLen+=sizeof(CT_DWORD);
	memcpy(&eusb.dwUserID,pBuf+sizeof(CT_DWORD)+NAME_SIZE,sizeof(CT_DWORD));
	iMesLen+=sizeof(CT_DWORD);
	memcpy(eusb.cUserName,pBuf+2*sizeof(CT_DWORD)+NAME_SIZE,NAME_SIZE);
	iMesLen+=NAME_SIZE;
	//发送时间，转换为yyyy-mm-dd hh:mm:ss
	time_t timer; 

	struct tm *tblock; 

	timer=time(NULL); 

	tblock=localtime(&timer);

	CT_INT32 year = tblock->tm_year+1900;
	CT_INT32 month = tblock->tm_mon+1;
	
	sprintf(eusb.iSendTime,"%d-%d-%d %d:%d:%d",year,month,tblock->tm_mday,tblock->tm_hour,tblock->tm_min,tblock->tm_sec);

	iMesLen+=TIME_SIZE;

	memcpy(eusb.cEmailTitle,pBuf+2*sizeof(CT_DWORD)+2*NAME_SIZE,EMAIL_TITLE_SIZE);
	iMesLen+=EMAIL_TITLE_SIZE;
	memcpy(eusb.cEmailContent,pBuf+2*sizeof(CT_DWORD)+2*NAME_SIZE+EMAIL_TITLE_SIZE,len-(2*sizeof(CT_DWORD)+2*NAME_SIZE+EMAIL_TITLE_SIZE));
	iMesLen+=len-(2*sizeof(CT_DWORD)+2*NAME_SIZE+EMAIL_TITLE_SIZE);
	CCUser *pUser;
	if (CCUserManager::getSingleton().Find(aimUserID,&pUser))
	{
		pUser->GetNetInterface()->Send(MAIN_G_EMAIL_EVENT_CITY,SUB_G_USER_EMAIL,(CT_CHAR*)&eusb,iMesLen);
	}
}

CT_VOID CCEmailEvent::DisposeUserEmail(CT_CHAR* pBuf,CT_DWORD len)
{
	if (len>sizeof(CMD_G_EmailUser))
	{
		CCLogMeg::getSingleton().WriteError("CCEmailEvent::DisposeUserEmail mes too long %d",len);
		return;
	}
	CT_DWORD aimUserID = 0,userID=0;
	memcpy(&aimUserID,pBuf,sizeof(CT_DWORD));
	memcpy(&userID,pBuf+sizeof(CT_DWORD)+NAME_SIZE,sizeof(CT_DWORD));
	//如果uid大于0，通过uid判断
	CCUser *pUser;
	if (aimUserID>0)
	{
		//不能给自己邮件
		if (aimUserID==userID)
		{
			CCLogMeg::getSingleton().WriteError("CCEmailEvent::DisposeUserEmail can't send email to yourself!");
			return;
		}
		if (CCUserManager::getSingleton().Find(aimUserID,&pUser))
		{
			TransformMes(pBuf,len,aimUserID);
			if (CCUserManager::getSingleton().Find(userID,&pUser))
			{
				pUser->GetNetInterface()->Send(MAIN_G_EMAIL_EVENT_CITY,SUB_G_USER_EMAIL_SUC,(CT_CHAR*)NULL,0);
			}
			return;
		}
	}
	else
	{
		CT_CHAR cName[NAME_SIZE],cUserName[NAME_SIZE];
		memset(cName,0,NAME_SIZE);
		memset(cUserName,0,NAME_SIZE);
		memcpy(cName,pBuf+sizeof(CT_DWORD),NAME_SIZE);
		memcpy(cUserName,pBuf+2*sizeof(CT_DWORD)+NAME_SIZE,NAME_SIZE);
		if (strcmp(cName,cUserName)==0)
		{
			CCLogMeg::getSingleton().WriteError("CCEmailEvent::DisposeUserEmail can't send email to yourself! %s",cUserName);
			return;
		}
		if (CCUserManager::getSingleton().Find(cName,&pUser))
		{
			TransformMes(pBuf,len,pUser->GetUserID());
			if (CCUserManager::getSingleton().Find(userID,&pUser))
			{
				pUser->GetNetInterface()->Send(MAIN_G_EMAIL_EVENT_CITY,SUB_G_USER_EMAIL_SUC,(CT_CHAR*)NULL,0);
			}
			return;
		}
	}
	//在线用户没有查询到，访问数据库
	if (CCUserManager::getSingleton().Find(userID,&pUser))
	{
		CT_DWORD DBthreadid = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(DBthreadid,MSC_MAIN_DB_EMAIL_DATA,MSC_SUB_DB_EMAIL_USER,pBuf,len);
	}
}

CT_VOID CCEmailEvent::GetEmailSendResult(CT_CHAR *pBuf,CT_DWORD len)
{
	if (len!=sizeof(MSC_DB_EmailUserResult))
	{
		CCLogMeg::getSingleton().WriteError("CCEmailEvent::GetEmailSendResult len %d",len);
		return;
	}
	MSC_DB_EmailUserResult *p = (MSC_DB_EmailUserResult *)pBuf;
	CCUser *pUser;
	CCUserManager::getSingleton().Find(p->dwUserID,&pUser);
	if (p->iRet>0)
	{
		pUser->GetNetInterface()->Send(MAIN_G_EMAIL_EVENT_CITY,SUB_G_USER_EMAIL_SUC,(CT_CHAR*)NULL,0);
	}
	else
	{
		pUser->GetNetInterface()->Send(MAIN_G_EMAIL_EVENT_CITY,SUB_G_USER_EMAIL_FAIL,(CT_CHAR*)NULL,0);
	}
}