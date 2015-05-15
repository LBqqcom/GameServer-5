#include "CBroadcastScriptSetting.h"
#include "CUser.h"
#include "CLogMeg.h"
#include "CUserManager.h"

template<> CCBroadcastScriptSetting* CCSingleton<CCBroadcastScriptSetting>::ms_Singleton=0;

CCBroadcastScriptSetting::CCBroadcastScriptSetting()
{

}

CCBroadcastScriptSetting::~CCBroadcastScriptSetting()
{

}

CT_BOOL CCBroadcastScriptSetting::GetBroadcastMes(CT_INT32 iType,G_BroadcastInfo *broadcastInfo)
{
	if (G_GetBroadcastMes(iType,broadcastInfo))
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

CT_VOID CCBroadcastScriptSetting::BroadcastMessage(CMD_G_BroadCastMes &pBroadCast)
{

	G_BroadcastInfo broadcastInfo = {0};
	CT_CHAR cBroadcastMes[CHAT_LEN] = {0};
	MasterData  md = {0};
	//根据userid获取玩家昵称
	CCUser *pUser;
	if (!CCUserManager::getSingleton().Find(pBroadCast.dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCPetEvent::DisposePetBorn,can't find user %d",pBroadCast.dwUserID);
		return;
	}
	pUser->m_userBase.GetMasterData(md);
	memset(cBroadcastMes,0,CHAT_LEN);
	CT_INT32	iMesLen = 0;
	if (CCBroadcastScriptSetting::getSingleton().GetBroadcastMes(pBroadCast.iSubType,&broadcastInfo))
	{
		if (strlen(broadcastInfo.cMes1)+strlen(broadcastInfo.cMes2)+strlen(broadcastInfo.cMes3)+strlen(pBroadCast.cProcessName)+strlen(pBroadCast.cResultName)+strlen(md.cNickName)>CHAT_LEN)
		{
			CCLogMeg::getSingleton().WriteError("CCUserManager::BroadCastWorldMessage broadcast mess is too long");
			return;
		}
		if (pBroadCast.iQuality>=broadcastInfo.iLevel)
		{
			//根据配置的消息去拼接广播消息
			memcpy(cBroadcastMes,&(md.cNickName),strlen(md.cNickName));
			iMesLen+=strlen(md.cNickName);
			memcpy(cBroadcastMes+iMesLen,&(broadcastInfo.cMes1),strlen(broadcastInfo.cMes1));
			iMesLen+=strlen(broadcastInfo.cMes1);
			if (strlen(pBroadCast.cProcessName)==0)
			{
				memcpy(cBroadcastMes+iMesLen,&(pBroadCast.cResultName),strlen(pBroadCast.cResultName));
				iMesLen+=strlen(pBroadCast.cResultName);
				memcpy(cBroadcastMes+iMesLen,&(broadcastInfo.cMes2),strlen(broadcastInfo.cMes2));
				iMesLen+=strlen(broadcastInfo.cMes2);
			}
			else
			{
				memcpy(cBroadcastMes+iMesLen,&(pBroadCast.cProcessName),strlen(pBroadCast.cProcessName));
				iMesLen+=strlen(pBroadCast.cProcessName);
				memcpy(cBroadcastMes+iMesLen,&(broadcastInfo.cMes2),strlen(broadcastInfo.cMes2));
				iMesLen+=strlen(broadcastInfo.cMes2);
				memcpy(cBroadcastMes+iMesLen,&(pBroadCast.cResultName),strlen(pBroadCast.cResultName));
				iMesLen+=strlen(pBroadCast.cResultName);
				memcpy(cBroadcastMes+iMesLen,&(broadcastInfo.cMes3),strlen(broadcastInfo.cMes3));
				iMesLen+=strlen(broadcastInfo.cMes3);
			}
			memcpy(cBroadcastMes+iMesLen,"\0",1);
			iMesLen+=1;
			CT_CHAR cTemp[CHAT_LEN] = {0};
			CT_INT32 iBroadcastType = 999;
			CT_INT32 iBroadcastTime = 1;
			memcpy(cTemp, &iBroadcastType, sizeof(iBroadcastType));
			memcpy(cTemp + sizeof(iBroadcastType), &iBroadcastTime, sizeof(iBroadcastTime));
			memcpy(cTemp+sizeof(iBroadcastType)+sizeof(iBroadcastTime), cBroadcastMes, CHAT_LEN - (sizeof(iBroadcastType)+sizeof(iBroadcastTime)));
			iMesLen += (sizeof(iBroadcastType)+sizeof(iBroadcastTime));
			CCUserManager::getSingleton().BroadCastWorldMessage(cTemp,iMesLen);
		}
	}
	
}

// 初始化公告信息
CT_VOID CCBroadcastScriptSetting::AnnouncementInit(CT_INT32 iIndex, const Announcement &anno)
{
	if (g_announcementMap.find(iIndex) != g_announcementMap.end())
	{
		g_announcementMap.erase(iIndex);
	}
	g_announcementMap.insert(AnnouncementMap::value_type(iIndex, anno));
}

//获取所有公告
CT_BOOL CCBroadcastScriptSetting::GetAnnouncement(CT_CHAR *buf, CT_INT32 &bufLen)
{
	CT_INT32 len = 0;
	AnnouncementMap_IT it = g_announcementMap.begin();
	AnnouncementMap_IT itEnd = g_announcementMap.end();

	CMD_G_ANNOUNCEMENT anno = {0};

	for (;it != itEnd;++it)
	{
		anno.iIndex = (*it).second.iIndex;
		memcpy(anno.cContent, (*it).second.cContent, ANNOUNCEMENT_CONTENT_LEN);
		*((CMD_G_ANNOUNCEMENT *)(buf + len)) = anno;		
		len += sizeof(CMD_G_ANNOUNCEMENT);
	}

	bufLen = len;
	return CT_TRUE;
}

// 获取活动公告
CT_BOOL CCBroadcastScriptSetting::GetAnnouncementByIndex(CT_CHAR* buf, CT_INT32 &bufLen, CT_INT32 iIndex)
{
	if (bufLen < sizeof(CMD_G_ANNOUNCEMENT))
	{
		return CT_FALSE;
	}

	AnnouncementMap_IT it = g_announcementMap.find(iIndex);

	if (it == g_announcementMap.end())
	{
		return CT_FALSE;
	}
	CMD_G_ANNOUNCEMENT *pAnno = (CMD_G_ANNOUNCEMENT*)buf;
	pAnno->iIndex = iIndex;
	memcpy(pAnno->cContent, it->second.cContent, ANNOUNCEMENT_CONTENT_LEN);

	bufLen = sizeof(CMD_G_ANNOUNCEMENT);
	return CT_TRUE;
}