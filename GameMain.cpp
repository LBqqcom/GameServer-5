#include <iostream>
#include <math.h>
#include "CPaseIni.h"
#include "CThreadManager.h"
#include "CNetEngine.h"
#include "CLogMeg.h"
#include "CEnvThread.h"
#include "CityThread.h"
#include "CGameConfig.h"
#include "CDataBaseThread.h"
#include "CGlobeData.h"
#include "CFightingThread.h"
#include "CLogDBThread.h"
#include "CUserManager.h"
#include "CApi.h"
#ifdef WIN32
//#include "CGlobeCrashReport.h"
#endif
//线程管理器
CCThreadManager* gPM ;
CCNetInterface*  g_pPCentServer;				//中心服务网络接口
CCNetInterface*  g_pPChargeServer;				//充值服务网络接口
CCNetInterface*	 g_pCorssServer;				//跨服服务
CT_INT32		 g_iServerID;					//服务ID

//退出服务
static CT_VOID ExitSev(CT_INT32 sig)
{
	CCUserManager::getSingleton().ExitAllUser();
	G_Sleep(5000*2); //睡眠10秒，等待工作线程写完数据到数据库
	CCThreadManager::getSingleton().Clear();
	delete gPM;
	gPM = 0;
	std::cout << "Game service : Suc to exit" << std::endl;
	exit(0);
}

//初始化配置
CT_BOOL InitSetting()
{
	g_pPCentServer=NULL;
	g_iServerID=0;

	std::string str("./log/GameServer");   //使用时间确定游戏配置文件名字， 防止下次启动的时候文件被改写
	str += G_GetNow();
	str += ".log";
	CCThreadManager::getSingleton().InitRes<CCGameConfig>(str.c_str() );

#ifndef _WIN32
	if (signal(SIGUSR2, ExitSev) == SIG_ERR)
	{
		CCLogMeg::getSingleton().WriteError("Error: singnal(SIGUSR2 , ExitSev) == SIG_ERR");
		exit(0);
	}

	//osc提供
	signal(SIGCHLD, SIG_DFL); /* A child process dies */
	signal(SIGTSTP, SIG_IGN); /* Various TTY signals */
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGHUP,  SIG_IGN); /* Ignore hangup signal */
	signal(SIGTERM, SIG_DFL); /* Die on SIGTERM */
	signal(SIGPIPE, SIG_IGN);
#endif
	CT_BOOL bRet = CCPaseIni::getSingleton().Open("GameServer.ini");
	if (!bRet)
	{
		CCLogMeg::getSingleton().WriteError("CCPaseIni::Open GameServer.ini failed!\n");
		return CT_FALSE;
	}
	bRet = CCPaseIni::getSingleton().Read();
	if (!bRet)
	{
		CCLogMeg::getSingleton().WriteError("CCPaseIni::Read failed!\n");
		return CT_FALSE;	
	}

	CCGameConfig& gc = ((CCGameConfig&)CCConfig::getSingleton());

	CCPaseIni::getSingleton().GetValue(SYS_MAX_ROWCLIENT_NUM_STR,     CCConfig::getSingleton().m_maxRowClientNum);
	CCPaseIni::getSingleton().GetValue(SYS_MAX_CLIENT_NUM_STR,        CCConfig::getSingleton().m_maxClientNum);
	CCPaseIni::getSingleton().GetValue(SYS_MAX_INPUT_BUFF_SIZE_STR,   CCConfig::getSingleton().m_inputBuffSize);
	CCPaseIni::getSingleton().GetValue(SYS_MAX_OUTPUT_BUFF_SIZE_STR,  CCConfig::getSingleton().m_outputBufffSize);
	CT_DWORD iLogLevel = CCLogMeg::LOG_WARNING;
	CCPaseIni::getSingleton().GetValue(SYS_LOG_LEVEL, iLogLevel);
	CCLogMeg::getSingleton().SetLogLevel(iLogLevel);

	CCPaseIni::getSingleton().GetValue(GAME_THREAD_CITY_NUM_STR,     ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum); 
	CCPaseIni::getSingleton().GetValue(GAME_THREAD_FIGHT_NUM_STR,    ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum); 
	CCPaseIni::getSingleton().GetValue(GAME_THREAD_DATABASE_NUM_STR, ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum); 
	CCPaseIni::getSingleton().GetValue(GAME_THREAD_LOGDATABASE_NUM_STR, ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum);
	CCPaseIni::getSingleton().GetValue(CENTER_SERVER_IP_STR,         ((CCGameConfig&)CCConfig::getSingleton()).m_centIP ); 
	CCPaseIni::getSingleton().GetValue(CENTER_SERVER_PORT_STR,       ((CCGameConfig&)CCConfig::getSingleton()).m_centPort ); 
	CCPaseIni::getSingleton().GetValue(GAME_SERVER_PORT_STR,         ((CCGameConfig&)CCConfig::getSingleton()).m_gamePort ); 
	CCPaseIni::getSingleton().GetValue(GAME_SERVER_ADDR,			 ((CCGameConfig&)CCConfig::getSingleton()).m_gameAddr ); 
    CCPaseIni::getSingleton().GetValue(CHARGE_SERVER_PORT_STR,       ((CCGameConfig&)CCConfig::getSingleton()).m_chargePort ); 
    CCPaseIni::getSingleton().GetValue(CHARGE_SERVER_ADDR,			 ((CCGameConfig&)CCConfig::getSingleton()).m_chargeAddr );


	CCPaseIni::getSingleton().GetValue(DB_SERVER_IP_STR,         ((CCGameConfig&)CCConfig::getSingleton()).m_dbIP );
	CCPaseIni::getSingleton().GetValue(DB_SERVER_PORT_STR,       ((CCGameConfig&)CCConfig::getSingleton()).m_dbPort );
	CCPaseIni::getSingleton().GetValue(DB_NAME_STR,              ((CCGameConfig&)CCConfig::getSingleton()).m_dbName );
	CCPaseIni::getSingleton().GetValue(DB_USER_NAME_STR,         ((CCGameConfig&)CCConfig::getSingleton()).m_dbUser );
	CCPaseIni::getSingleton().GetValue(DB_USER_KEY_STR,          ((CCGameConfig&)CCConfig::getSingleton()).m_dbKey );

	CCPaseIni::getSingleton().GetValue(LOGDB_SERVER_IP_STR,         ((CCGameConfig&)CCConfig::getSingleton()).m_LogdbIP );
	CCPaseIni::getSingleton().GetValue(LOGDB_SERVER_PORT_STR,       ((CCGameConfig&)CCConfig::getSingleton()).m_LogdbPort );
	CCPaseIni::getSingleton().GetValue(LOGDB_NAME_STR,              ((CCGameConfig&)CCConfig::getSingleton()).m_LogdbName );
	CCPaseIni::getSingleton().GetValue(LOGDB_USER_NAME_STR,         ((CCGameConfig&)CCConfig::getSingleton()).m_LogdbUser );
	CCPaseIni::getSingleton().GetValue(LOGDB_USER_KEY_STR,          ((CCGameConfig&)CCConfig::getSingleton()).m_LogdbKey );

	CCPaseIni::getSingleton().GetValue(DB_UPDATA_TIME,           ((CCGameConfig&)CCConfig::getSingleton()).m_iDBUpdataTime );
	CCPaseIni::getSingleton().GetValue(DB_UPDATA_VALUE,          ((CCGameConfig&)CCConfig::getSingleton()).m_iDBUpdataValue );

	CCPaseIni::getSingleton().GetValue(GM_HTTP_CHECK_STR,         ((CCGameConfig&)CCConfig::getSingleton()).m_iGMCheckTime );
	CCPaseIni::getSingleton().GetValue(GM_HTTP_DELAY_TIME_STR,    ((CCGameConfig&)CCConfig::getSingleton()).m_iGmDelayTime );
	CCPaseIni::getSingleton().GetValue(GM_SERVER_IP_STR,          ((CCGameConfig&)CCConfig::getSingleton()).m_strGMip );

    CCPaseIni::getSingleton().GetValue(GM_PORT_STR,               ((CCGameConfig&)CCConfig::getSingleton()).m_wGMHttpPort);

    CCPaseIni::getSingleton().GetValue(SERVER_NAME,               ((CCGameConfig&)CCConfig::getSingleton()).m_strServerName);

	CCPaseIni::getSingleton().GetValue(GAME_SERVER_ID,               ((CCGameConfig&)CCConfig::getSingleton()).m_iGSID);
	CCPaseIni::getSingleton().GetValue(CROSS_SERVER_IP,              ((CCGameConfig&)CCConfig::getSingleton()).m_strCSIP);
	CCPaseIni::getSingleton().GetValue(CROSS_SERVER_PORT,            ((CCGameConfig&)CCConfig::getSingleton()).m_dwCSPort);

	return CT_TRUE;
}

//运行线程
CT_VOID RunThread()
{
	//定时器线程初始化并运行...
	CCThreadManager::getSingleton().InitTimerThread();
	

	//环境线程初始化并运行...
	CCThreadManager::getSingleton().InitTaskThread< CCEnvThread >( SYS_THREAD_GATE );
	

	//城市线程初始化并运行
	for (CT_DWORD i=0; i<((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum; ++i   )
	{
		CCThreadManager::getSingleton().InitTaskThread< CCityThread >( GAME_THREAD_CITY + i );
	}


	//数据库线程初始化并运行
	for (CT_DWORD i=0; i<((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum; ++i   )
	{
		CCThreadManager::getSingleton().InitTaskThread< CCDataBaseThread >( GAME_THREAD_DATABASE + i );
	}

	//log数据库线程初始化并运行
	CT_INT32 iCount=((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	for (CT_DWORD i=0; i<((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum; ++i   )
	{
		CCThreadManager::getSingleton().InitTaskThread< CCLogDBThread >( GAME_THREAD_LOGDATABASE + i );
	}

	//战斗线程初始化并运行
	for (CT_DWORD i=0; i<((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum; ++i   )
	{
		CCThreadManager::getSingleton().InitTaskThread< CCFightingThread >( GAME_THREAD_FIGHT + i );
	}

	//初始化网络服务线程
	CCThreadManager::getSingleton().InitNetThread();
	g_pPCentServer = CCNetEngine::getSingleton().CreateGameConnector(((CCGameConfig&)CCConfig::getSingleton()).m_centIP.c_str(), ((CCGameConfig&)CCConfig::getSingleton()).m_centPort);
    g_pPChargeServer = CCNetEngine::getSingleton().CreateGameConnector(((CCGameConfig&)CCConfig::getSingleton()).m_chargeAddr.c_str(), ((CCGameConfig&)CCConfig::getSingleton()).m_chargePort);
	g_pCorssServer= CCNetEngine::getSingleton().CreateGameConnector(((CCGameConfig&)CCConfig::getSingleton()).m_strCSIP.c_str(),((CCGameConfig&)CCConfig::getSingleton()).m_dwCSPort);
	CCNetEngine::getSingleton().CreateRowServer( ((CCGameConfig&)CCConfig::getSingleton()).m_wGMHttpPort);
	CCNetEngine::getSingleton().CreateGameServer( ((CCGameConfig&)CCConfig::getSingleton()).m_gamePort );    
	//启动网络服务线程
	CCThreadManager::getSingleton().RunTaskThread(SYS_THREAD_GATE);
	CCThreadManager::getSingleton().RunTimerThread();

	//城市线程初始化并运行
	for (CT_DWORD i=0; i<((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum; ++i   )
	{
		CCThreadManager::getSingleton().RunTaskThread(GAME_THREAD_CITY + i);
	}

	//数据库线程初始化并运行
	for (CT_DWORD i=0; i<((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum; ++i   )
	{
		CCThreadManager::getSingleton().RunTaskThread(GAME_THREAD_DATABASE + i);
	}

	//log数据库线程初始化并运行
	for (CT_DWORD i=0; i<((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum; ++i   )
	{
		CCThreadManager::getSingleton().RunTaskThread(GAME_THREAD_LOGDATABASE + i);
	}

	//战斗线程初始化并运行
	for (CT_DWORD i=0; i<((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum; ++i   )
	{
		CCThreadManager::getSingleton().RunTaskThread(GAME_THREAD_FIGHT + i);
	}

	CCThreadManager::getSingleton().RunNetThread();
}

//等待退出
CT_VOID WaitExit()
{
#ifdef _WIN32
	CT_CHAR buf[BUFSIZ] = {0};
	CT_INT32 p = 1;
	while (p != 0)
	{
		std::cout << "GameServer :  Type exit to exit program" << std::endl;
		std::cin >> buf;
		p = strcmp(buf, "exit");
	}
	ExitSev(0);
#else
	CCLogMeg::getSingleton().WriteError("kill  -s SIGUSR2 procid  to exit");
	sleep(-1);
#endif
}

CT_INT32 main(CT_INT32 argc, char* argv[])
{
#ifdef _WIN32
//	CCGlobeCrashReport::Install("GameServer", "0.0.0.1");
#endif

	gPM = new CCThreadManager;
	if(!InitSetting())
	{
		return -1;
	}
	RunThread();
    WaitExit();

#ifdef _WIN32
//	CCGlobeCrashReport::Unstaill();
#endif
	return 0;
}




