/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCGameConfig.h
Version:		1.0
Description:    游戏配置文件
Author:			pengwen
Create Date:	2012-8-16
History:        配置游戏中关键数据     

******************************************************************/
#ifndef CCGAMECONFIG_H_
#define CCGAMECONFIG_H_
#include "CConfig.h"
//城市线程数目
#define  GAME_THREAD_CITY_NUM_STR      ("GameThreadCityNum")
//战斗线程数目
#define  GAME_THREAD_FIGHT_NUM_STR     ("GameThreadFightNum")
//数据库线程数据
#define  GAME_THREAD_DATABASE_NUM_STR  ("GameThreadDataBaseNum")
//日志数据库线程数据
#define  GAME_THREAD_LOGDATABASE_NUM_STR  ("GameThreadLogDataBaseNum")
//中心服务器ip
#define  CENTER_SERVER_IP_STR          ("CenterIP")
//中心服务器端口
#define  CENTER_SERVER_PORT_STR        ("CenterPort")
//游戏服务器端口
#define  GAME_SERVER_PORT_STR          ("GameServerPort")
//游戏服务地址
#define  GAME_SERVER_ADDR               ("GameServerAddr")
// 充值服务器地址
#define  CHARGE_SERVER_ADDR             ("ChargeServerAddr")
// 充值服务器端口
#define  CHARGE_SERVER_PORT_STR         ("ChargeServerPort")
//数据库id
#define  DB_SERVER_IP_STR              ("DBServerIP")
//数据库端口
#define  DB_SERVER_PORT_STR            ("DBServerPort")
//数据库名字
#define  DB_NAME_STR                   ("DBName")
//数据库用户
#define  DB_USER_NAME_STR              ("DBUser")
//数据库用户密码
#define  DB_USER_KEY_STR               ("DBKey")
//日志数据库id
#define  LOGDB_SERVER_IP_STR              ("LOGDBServerIP")
//日志数据库端口
#define  LOGDB_SERVER_PORT_STR            ("LOGDBServerPort")
//日志数据库名字
#define  LOGDB_NAME_STR                   ("LOGDBName")
//日志数据库用户
#define  LOGDB_USER_NAME_STR              ("LOGDBUser")
//日志数据库用户密码
#define  LOGDB_USER_KEY_STR               ("LOGDBKey")
//数据库写入
#define	 DB_UPDATA_TIME					("DBUpdataTime")
//数据库写入阀值
#define	 DB_UPDATA_VALUE				("DBUpdataValue")

//gm连接检查时间
#define  GM_HTTP_CHECK_STR				("GMHttpCheckTime")
//gm最大等待时间
#define  GM_HTTP_DELAY_TIME_STR			("GMHttpDelayTime")
//gm服务器地址
#define  GM_SERVER_IP_STR				("GMServerIP")
//gm响应的端口
#define  GM_PORT_STR                    ("GMHttpPort")
//服务器名
#define  SERVER_NAME                    ("GameServerName")

//服务ID
#define  GAME_SERVER_ID					("GameServerID")
//跨服IP
#define  CROSS_SERVER_IP				("CrossServerIP")
//跨服端口
#define  CROSS_SERVER_PORT				("CrossServerPort")

class CCGameConfig : public CCConfig
{
public:
	//城市线程数目
	CT_DWORD    m_gameThreadCityNum;
	//战斗线程数目
	CT_DWORD    m_gameThreadFightNum;
	//数据库线程数目
	CT_DWORD    m_gameThreadDataBaseNum;
	//数据库线程数目
	CT_DWORD    m_gameThreadLogDataBaseNum;
	//中心服务器的ip
	std::string m_centIP;
	//中心服务器端口
	CT_WORD     m_centPort;
	//游戏服务器端口
	CT_WORD     m_gamePort; 
	//游戏服务器地址
	std::string m_gameAddr; 
    //充值服务器端口
    CT_WORD     m_chargePort; 
    //充值服务器地址
    std::string m_chargeAddr; 
	//数据库ip
	std::string m_dbIP;
	//数据库端口
	CT_WORD     m_dbPort;
    //数据库用户
	std::string m_dbUser;
	//数据库密码
	std::string m_dbKey;
	//数据库名字
	std::string m_dbName;
	//日志数据库ip
	std::string m_LogdbIP;
	//日志数据库端口
	CT_WORD     m_LogdbPort;
	//日志数据库用户
	std::string m_LogdbUser;
	//日志数据库密码
	std::string m_LogdbKey;
	//日志数据库名字
	std::string m_LogdbName;
	//数据库写入
	CT_INT32	m_iDBUpdataTime;
	//数据库写入阀值
	CT_INT32	m_iDBUpdataValue;
	
	//检测时间
	CT_INT32     m_iGMCheckTime;
	//检测阀值
	CT_INT32     m_iGmDelayTime; 
	//gm的ip
	std::string  m_strGMip;
	//面向gm服务器的端口
	CT_WORD		m_wGMHttpPort; 
    // 游戏服务器名称
    std::string m_strServerName;

	//游戏服务ID
	CT_INT32	m_iGSID;
	//跨服IP
	std::string m_strCSIP;
	//跨服端口
	CT_DWORD	m_dwCSPort;
public:
	CCGameConfig(CT_VOID);
	~CCGameConfig(CT_VOID);
};
#endif

