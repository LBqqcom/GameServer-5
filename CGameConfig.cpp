#include "CGameConfig.h"

CCGameConfig::CCGameConfig(CT_VOID)
{
	m_gameThreadLogDataBaseNum=1;
	m_gameThreadDataBaseNum=1;
	m_gameThreadFightNum=1;
	m_gameThreadCityNum=1;



	
	m_centPort   = 0;
	m_gamePort   = 0; 
	m_gameAddr   = ""; 
	m_chargePort = 0; 
	m_chargeAddr = ""; 

	m_dbIP   = "";
	m_dbPort = 0;
	m_dbUser = "";
	m_dbKey = "";

	m_dbName = "";
	m_LogdbIP = "";
	m_LogdbPort = 0;
	m_LogdbUser = "";
	m_LogdbKey = "";
	m_LogdbName = "";

	m_iDBUpdataTime = 10;
	m_iDBUpdataValue = 600;


	m_iGMCheckTime = 2;
	m_iGmDelayTime = 10;
	m_strGMip = "";
	m_wGMHttpPort = 16000;

    m_strServerName = "";

	//游戏服务ID
	m_iGSID =10000;
	//跨服IP
	m_strCSIP="";
	//跨服端口
	m_dwCSPort=0;
}

CCGameConfig::~CCGameConfig(CT_VOID)
{
}
