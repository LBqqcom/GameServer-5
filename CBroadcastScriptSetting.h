#ifndef CBroadcastScriptSetting_H_
#define CBroadcastScriptSetting_H_

#include <map>
#include "CSingleton.h"
#include "CTType.h"
#include "GlobeScriptInterface.h"
#include "CMD_Game.h"

#define ANNOUNCEMENT_CONTENT_LEN   (ANNOUNCEMENT_LEN)

struct Announcement
{
	CT_INT32 iIndex;
	CT_CHAR cContent[ANNOUNCEMENT_CONTENT_LEN];
	CT_INT32 iFlag;
};

/*
次结构为滚动公告插入结构，不可增加长度和字段
*/
struct RollBroadcast
{
	CT_INT32	iBroadInfo;
	CT_CHAR		cBroadInfo[CHAT_LEN+1];

	CT_VOID SetValue(CT_INT32 iBi,CT_CHAR *pBuf)
	{
		iBroadInfo=iBi;
		memcpy(cBroadInfo,pBuf,strlen(pBuf));
	}
};



class CCBroadcastScriptSetting: public CCSingleton<CCBroadcastScriptSetting>
{
public:
	typedef std::map<CT_INT32,Announcement>				 AnnouncementMap;
	typedef std::map<CT_INT32,Announcement>::iterator	 AnnouncementMap_IT;

    AnnouncementMap g_announcementMap;

public:
	CCBroadcastScriptSetting();
	~CCBroadcastScriptSetting();
public:
	CT_BOOL GetBroadcastMes(CT_INT32 iType,G_BroadcastInfo *broadcastInfo);
	CT_VOID BroadcastMessage(CMD_G_BroadCastMes &pBroadCast);

	// 初始化公告信息
	CT_VOID AnnouncementInit(CT_INT32 iIndex, const Announcement &anno);
	// 获取所有公告
	CT_BOOL GetAnnouncement(CT_CHAR* buf, CT_INT32 &bufLen);
	// 获取活动公告
	CT_BOOL GetAnnouncementByIndex(CT_CHAR* buf, CT_INT32 &bufLen, CT_INT32 iIndex);
};

#endif