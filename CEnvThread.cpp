#include <set>
#include <cassert>
#include <cstdlib>
#include <time.h>
#include "CEnvThread.h"
#include "CNetInterface.h"
#include "CLogMeg.h"
#include "CMesBlock.h"
#include "CThreadManager.h"
#include "CMD_Game.h"
#include "CGlobeFunc.h"
#include "CUserEnterManager.h"
#include "CUserManager.h"
#include "CDatabaseMes.h"
#include "CGlobeData.h"
#include "GlobeScriptInterface.h"
#include "CTimerThread.h"
#include "CUser.h"
#include "CGameConfig.h"
#include "CBuildingScriptSetting.h"
#include "CMasterScriptSetting.h"
#include "CFightingScriptSetting.h"
#include "CItemScriptSetting.h"
#include "CRegionScriptSetting.h"
#include "CFightingEvnet.h"
#include "CRandEventManager.h"
#include "CFightingRecord.h"
#include "CBattleArray.h"
#include "CTaskScriptSetting.h"
#include "CBroadcastScriptSetting.h"
#include "CMarket.h"
#include "CTop.h"
#include "CUserExitEvent.h"
#include "CDBLogMes.h"
#include "COrderList.h"
#include "CMD_Charge.h"
#include "CActive.h"
#include "CHttpGMManager.h"
#include "CCompress.h"
#include "CFightTeamManager.h"
#include "CResourceGrabMgr.h"
#include "CTeamDuplicate.h"
#include "CTowerResManager.h"
#include "CBossManager.h"
#include "../MessageDefined/CMD_Center.h"
#include "CFightingData.h"
#include "CCSFightData.h"
#include "CMD_CrossServer.h"
#include "CIslandEvent.h"
#include "CTitle.h"

//用户进入管理
CCUserEnterManager*		G_pUEManager;
//用户管理
CCUserManager*			G_pUManager;
//城市建筑配置
CCBuildingScriptSetting*  G_pCBSetting;
//领主建筑配置
CCMasterScriptSetting*   G_pMSSeting;
//事件脚本配置
CCEventScriptSetting    *G_pESSeting; 
//战斗系统配置
CCFightingScriptSetting	*G_pFighting;
//随机事件管理器
CCRandEventManager      *G_pRandEventManager;
// 道具信息配置
CCItemScriptSetting     *G_pItem;
//战斗记录
CCFightingRecord		*G_pFightingRecord;
//区域配置信息
CCRegionScriptSetting	*G_pRSSetting;
//阵法
CCBattleArray			*G_pBattleArray;
//任务配置信息
CCTaskScriptSetting		*G_pTaskSciptSetting;
//广播信息
CCBroadcastScriptSetting	*G_pBroadcastScriptSetting;
//商店信息
CCMarket				*G_pMarket;
//排行榜
CCTop					*G_pTop;
//活动
CCActive				*G_pActive;
//数据库错误日志
CCDBLogMes				*G_dbLogMes;
CCOrderList             *G_pOrderList;
//http连接管理器
CCHttpGMManager         *G_pHttpGMManger;
//战队管理器
CCFightTeamManager		*G_pFTManager;
// 资源争夺系统配置
CCResourceGrabMgr		*G_pResourceGrabMgr;
//组队副本
CCTeamDuplicate			*G_pTeamDuplicate;
//元素之塔资源管理器
CCTowerResManager       *G_pTowerResManager;
//世界BOSS管理器
CCBossManager			*G_pBossManager;
//战斗数据
CCFightingData			*G_pFightingData;
//跨服战
CCCSFightData			*G_pCSFightData;
//头衔
CCTitle					*G_pTitle;


CT_INT64		 g_lHeartBeat;					//心跳时间
CT_INT64		 g_lOpenServerTime;			//服务开启时间
CT_INT32		 g_iDBUpdataValue;			//数据库更新阀值
CT_INT32		 g_iGlobeData[GLOBEDATA_TYPE_MAX];	//全局数据0:红米 1:米3

CCEnvThread::CCEnvThread(CT_VOID) 
{
	CCLogMeg::getSingleton().WriteInfo("Init Script Module Begin");
	std::string  strPath("./GameIni/UTF-8/");
	CT_INT32 ret = G_InitModule((char*)strPath.c_str(), strPath.size());
	CT_ASSERT(ret == 1);
	if (ret == 0)
	{
		CCLogMeg::getSingleton().WriteError("CCEnvThread::CCEnvThread(CT_VOID): Fail to run G_InitModule");
		exit(0);
	}
	CCLogMeg::getSingleton().WriteInfo("Init Script Module Finish");
	CCLogMeg::getSingleton().WriteInfo("Read Script Module Begin");
	ret = G_ReadModule();

	CT_ASSERT(ret == 1);
	if (ret == 0)
	{
		CCLogMeg::getSingleton().WriteError("CCEnvThread::CCEnvThread(CT_VOID): Fail to run G_ReadModule");
		exit(0);
	}
	CCLogMeg::getSingleton().WriteInfo("Read Script Module Finish");

	g_lHeartBeat=HEART_BEAT_MIN;

	G_pUEManager = new CCUserEnterManager;
	G_pUManager  = new CCUserManager;
	G_pCBSetting = new CCBuildingScriptSetting;
	G_pMSSeting  = new CCMasterScriptSetting;
	G_pFighting	 = new CCFightingScriptSetting;
	G_pESSeting  = new CCEventScriptSetting;

	G_pRandEventManager = new CCRandEventManager;
    G_pItem      = new CCItemScriptSetting;
	G_pFightingRecord =new CCFightingRecord;
	G_pRSSetting = new CCRegionScriptSetting;
	G_pBattleArray=new CCBattleArray;
	G_pTaskSciptSetting = new CCTaskScriptSetting;
	G_pBroadcastScriptSetting = new CCBroadcastScriptSetting;
	G_pMarket	= new CCMarket;
	G_pActive	= new CCActive;
	G_pTop			=new CCTop;

	std::string str("./log/GameServerDB");   //使用时间确定游戏配置文件名字， 防止下次启动的时候文件被改写
	str += G_GetNow();
	str += ".log";
	G_dbLogMes		= new CCDBLogMes(str.c_str());
    G_pOrderList    = new CCOrderList;
	G_pHttpGMManger = new CCHttpGMManager();
	G_pResourceGrabMgr = new CCResourceGrabMgr;
	G_pFTManager	= new CCFightTeamManager;
	G_pTeamDuplicate =new CCTeamDuplicate;
	G_pTowerResManager = new CCTowerResManager;
	G_pBossManager = new CCBossManager;
	G_pFightingData=new CCFightingData;
	G_pCSFightData=new CCCSFightData;
	G_pTitle=new CCTitle;
}

CCEnvThread::~CCEnvThread(CT_VOID)
{
	delete G_pUEManager;
	G_pUEManager = 0;
	delete G_pUManager;
	G_pUManager = 0;
	delete G_pCBSetting;
	G_pCBSetting = 0;
	delete G_pMSSeting;
	G_pMSSeting = 0;
	delete G_pFighting;
	G_pFighting=0;
	delete G_pRandEventManager;
	G_pRandEventManager = 0;
    delete G_pItem;
    G_pItem = 0;
	delete G_pFightingRecord;
	G_pFightingRecord = 0;
	delete G_pRSSetting;
	G_pRSSetting = 0;
	delete G_pMarket;
	G_pMarket = 0;
	delete G_dbLogMes;
	G_dbLogMes = 0;
	delete G_pTop;
	G_pTop=0;

	G_ReleaseModule();




	delete G_pESSeting;
	G_pESSeting = 0;

    delete G_pOrderList;
    G_pOrderList = 0;

	delete G_pHttpGMManger;
	G_pHttpGMManger = 0;

	delete G_pResourceGrabMgr;
	G_pResourceGrabMgr = 0;
	delete G_pFTManager;
	G_pFTManager = 0;
	delete G_pTowerResManager;
	G_pTowerResManager = 0;

	delete G_pFightingData;
	G_pFightingData=0;

	delete G_pCSFightData;
	G_pCSFightData=0;
}

//定时器
CT_VOID CCEnvThread::OnTimer(CT_DWORD timeID) 
{
	if (timeID == IDI_HTTP_CHECK_TIMER)
	{
		CCHttpGMManager::getSingleton().UpdateHttpInfoMapTimeout();
	}
	else if(timeID == IDI_SERVER_HERT)
	{
		g_pPCentServer->Send((CT_DWORD)0,(CT_DWORD)0);
		g_pPChargeServer->Send((CT_DWORD)0,(CT_DWORD)0);
		g_pCorssServer->Send((CT_DWORD)0,(CT_DWORD)0);

	}
	else
	{
		m_GameMesManager.DeliverTime(timeID); 
	}
}

//打开
CT_VOID CCEnvThread::OnOpen()
{

	CCTimerThread::getSingleton().SetTimer(IDI_SERVER_HERT,this,TIME_SERVER_HURT);

	CCTimerThread::getSingleton().SetTimer(IDI_HTTP_CHECK_TIMER, this, ((CCGameConfig&)CCConfig::getSingleton()).m_iGMCheckTime * 1000 );

} 

//网络连接进入
CT_VOID CCEnvThread::OnNetEventOpen(CCNetInterface* pNetHandle)
{
	//((CCServerEvent*)m_pServerEvent)->OnNetEventOpen(pNetHandle);
	if (g_pPChargeServer == pNetHandle)
	{
		G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_MASTER,MSC_SUB_CHARGE_RESEND, 0, 0);
	}
	else if(g_pCorssServer == pNetHandle)
	{
		CMD_CS_RegisterServer rs={0};
		rs.iGSID=((CCGameConfig&)CCConfig::getSingleton()).m_iGSID;
		rs.dwPort=((CCGameConfig&)CCConfig::getSingleton()).m_dwCSPort;
		memcpy(rs.cIP,((CCGameConfig&)CCConfig::getSingleton()).m_strCSIP.c_str(),IPAddrEx);
		g_pCorssServer->Send(MAIN_CS_GAME,SUB_CS_REGISTER_SERVER,(CT_CHAR*)&rs,sizeof(rs));
	}
}

//网络数据到来
CT_VOID CCEnvThread::OnNetEventRead(CCNetInterface* pNetHandle,CT_DWORD mainID, CT_DWORD subID,  CT_CHAR* pBuf,  CT_DWORD len) 
{
	switch (mainID)
	{
		//根据主消息判断那个线程处理
	case MAIN_G_APPLY_UPGRADE_CITY:
	case MAIN_G_BUILDING_EVENT_CITY:
	case MAIN_G_MASTER_EVENT_CITY:
	case MAIN_G_EVENT_CITY:
	case MAIN_G_CHAT_CITY:
	case MAIN_G_REGION_EVENT_CITY:
	case MAIN_G_DUPLICATE_EVENT:
	case MAIN_G_TASK_EVENT_CITY:
    case MAIN_G_DAILY_EVENT:
	case MAIN_G_MARKET_EVENT_CITY:
	case MAIN_G_PVP_EVENT:
	case MAIN_G_PET_EVENT_CITY:
	case MAIN_G_HEART_BEAT:
	case MAIN_G_EMAIL_EVENT_CITY:
	case MAIN_G_VIP_EVENT_CITY:
    case MAIN_CH_CHARGE :
	case MAIN_G_ACTIVE_EVENT_CITY:
	case MAIN_G_WORSHIP_EVENT_CITY:
	case MAIN_G_CUSTOMER_FEEDBACK:
	case MAIN_G_FRIEND_EVENT:
	case MAIN_G_TEAM_DUPLICATE:
	case MAIN_G_FIGHTTEAM_EVENT:
	case MAIN_G_RES_GRAB:
	case MAIN_G_TOP_ENV:
	case MAIN_G_ELEMENT_TOWER:
    case MAIN_G_ELEMENT_POWER:
	case MAIN_G_WORLD_BOSS:
	case MAIN_G_LEAVE_ENV:
	case MAIN_C_GAME:
	case MAIN_CS_GAME:
	case MAIN_G_CROSS_SERVER:
	case MAIN_G_ISLAND:
	case MAIN_G_SEEK:
		{
			CT_INT32* pHead = (CT_INT32*)pBuf;
			CT_INT32 cityThreadID =  GAME_THREAD_CITY +  *pHead % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
			G_SendThreadInfo(cityThreadID, mainID, subID, (CT_CHAR*)pBuf, len);
			break;
		}
	case MAIN_G_ENTER_ENV:  //城市线程特殊处理用户登录 彭文
		{
#define     ENV_BUF_LEN   (1024)
			CT_CHAR   buf[ENV_BUF_LEN] = {0};
			if (len + sizeof(CT_INT32) >= ENV_BUF_LEN )
			{
				break;
			}
			memcpy(buf, pBuf, len);
			CT_INT32 iNetHandle = (CT_INT32)pNetHandle;
			memcpy(buf+len, &iNetHandle, sizeof(CT_INT32));
			CT_INT32* pHead = (CT_INT32*)pBuf;
			CT_INT32 cityThreadID =  GAME_THREAD_CITY +  *pHead % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
			G_SendThreadInfo(cityThreadID, mainID, subID, (CT_CHAR*)buf, len + sizeof(CT_INT32));
			break;
		}

	case MAIN_G_FIGHTING_EVENT:	
		{
			CT_INT32* pHead = (CT_INT32*)pBuf;
			CT_INT32 FightingThreadID =  GAME_THREAD_FIGHT +  *pHead % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum;
			G_SendThreadInfo(FightingThreadID, mainID, subID, (CT_CHAR*)pBuf, len);
			break;
		}

	default:
		{
			m_GameMesManager.DeliverMes(mainID, subID, pBuf, len, (CT_DWORD)pNetHandle);
			break;
		}
	}
}

//网络关闭
CT_VOID CCEnvThread::OnNetEventClose(CCNetInterface* pNetHandle) 
{
	CCUser* pUser = 0;
	CCUserManager::getSingleton().Find(pNetHandle, &pUser);
	if (pUser != 0)
	{
		//CCUserManager::getSingleton().Erase(pUser->GetUserID()); //删除用户				//等待断线重回  
		//CCUserEnterManager::getSingleton().Erase(pUser->GetUserID());//删除验证码
	}
	else if(g_pCorssServer == pNetHandle)
	{
		CMD_CS_RegisterServer rs={0};
		rs.iGSID=((CCGameConfig&)CCConfig::getSingleton()).m_iGSID;
		rs.dwPort=((CCGameConfig&)CCConfig::getSingleton()).m_dwCSPort;
		memcpy(rs.cIP,((CCGameConfig&)CCConfig::getSingleton()).m_strCSIP.c_str(),IPAddrEx);
		g_pCorssServer->Send(MAIN_CS_GAME,SUB_CS_REGISTER_SERVER,(CT_CHAR*)&rs,sizeof(rs));
	}
}

//用户自定义网络数据
CT_VOID CCEnvThread::OnNetEventReadRow(CCNetInterface* pNetHandle, CT_CHAR* pBuf,  CT_DWORD len)
{
   //CCLogMeg::getSingleton().WriteError("接收到未知的消息： CCEnvThread::OnNetEventReadRow");
	CCHttpGMManager::getSingleton().UpdateHttpInfoMap(pNetHandle, pBuf);
}