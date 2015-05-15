/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCGameConfig.h
Version:		1.0
Description:    ��Ϸ�����ļ�
Author:			pengwen
Create Date:	2012-8-16
History:        ������Ϸ�йؼ�����     

******************************************************************/
#ifndef CCGAMECONFIG_H_
#define CCGAMECONFIG_H_
#include "CConfig.h"
//�����߳���Ŀ
#define  GAME_THREAD_CITY_NUM_STR      ("GameThreadCityNum")
//ս���߳���Ŀ
#define  GAME_THREAD_FIGHT_NUM_STR     ("GameThreadFightNum")
//���ݿ��߳�����
#define  GAME_THREAD_DATABASE_NUM_STR  ("GameThreadDataBaseNum")
//��־���ݿ��߳�����
#define  GAME_THREAD_LOGDATABASE_NUM_STR  ("GameThreadLogDataBaseNum")
//���ķ�����ip
#define  CENTER_SERVER_IP_STR          ("CenterIP")
//���ķ������˿�
#define  CENTER_SERVER_PORT_STR        ("CenterPort")
//��Ϸ�������˿�
#define  GAME_SERVER_PORT_STR          ("GameServerPort")
//��Ϸ�����ַ
#define  GAME_SERVER_ADDR               ("GameServerAddr")
// ��ֵ��������ַ
#define  CHARGE_SERVER_ADDR             ("ChargeServerAddr")
// ��ֵ�������˿�
#define  CHARGE_SERVER_PORT_STR         ("ChargeServerPort")
//���ݿ�id
#define  DB_SERVER_IP_STR              ("DBServerIP")
//���ݿ�˿�
#define  DB_SERVER_PORT_STR            ("DBServerPort")
//���ݿ�����
#define  DB_NAME_STR                   ("DBName")
//���ݿ��û�
#define  DB_USER_NAME_STR              ("DBUser")
//���ݿ��û�����
#define  DB_USER_KEY_STR               ("DBKey")
//��־���ݿ�id
#define  LOGDB_SERVER_IP_STR              ("LOGDBServerIP")
//��־���ݿ�˿�
#define  LOGDB_SERVER_PORT_STR            ("LOGDBServerPort")
//��־���ݿ�����
#define  LOGDB_NAME_STR                   ("LOGDBName")
//��־���ݿ��û�
#define  LOGDB_USER_NAME_STR              ("LOGDBUser")
//��־���ݿ��û�����
#define  LOGDB_USER_KEY_STR               ("LOGDBKey")
//���ݿ�д��
#define	 DB_UPDATA_TIME					("DBUpdataTime")
//���ݿ�д�뷧ֵ
#define	 DB_UPDATA_VALUE				("DBUpdataValue")

//gm���Ӽ��ʱ��
#define  GM_HTTP_CHECK_STR				("GMHttpCheckTime")
//gm���ȴ�ʱ��
#define  GM_HTTP_DELAY_TIME_STR			("GMHttpDelayTime")
//gm��������ַ
#define  GM_SERVER_IP_STR				("GMServerIP")
//gm��Ӧ�Ķ˿�
#define  GM_PORT_STR                    ("GMHttpPort")
//��������
#define  SERVER_NAME                    ("GameServerName")

//����ID
#define  GAME_SERVER_ID					("GameServerID")
//���IP
#define  CROSS_SERVER_IP				("CrossServerIP")
//����˿�
#define  CROSS_SERVER_PORT				("CrossServerPort")

class CCGameConfig : public CCConfig
{
public:
	//�����߳���Ŀ
	CT_DWORD    m_gameThreadCityNum;
	//ս���߳���Ŀ
	CT_DWORD    m_gameThreadFightNum;
	//���ݿ��߳���Ŀ
	CT_DWORD    m_gameThreadDataBaseNum;
	//���ݿ��߳���Ŀ
	CT_DWORD    m_gameThreadLogDataBaseNum;
	//���ķ�������ip
	std::string m_centIP;
	//���ķ������˿�
	CT_WORD     m_centPort;
	//��Ϸ�������˿�
	CT_WORD     m_gamePort; 
	//��Ϸ��������ַ
	std::string m_gameAddr; 
    //��ֵ�������˿�
    CT_WORD     m_chargePort; 
    //��ֵ��������ַ
    std::string m_chargeAddr; 
	//���ݿ�ip
	std::string m_dbIP;
	//���ݿ�˿�
	CT_WORD     m_dbPort;
    //���ݿ��û�
	std::string m_dbUser;
	//���ݿ�����
	std::string m_dbKey;
	//���ݿ�����
	std::string m_dbName;
	//��־���ݿ�ip
	std::string m_LogdbIP;
	//��־���ݿ�˿�
	CT_WORD     m_LogdbPort;
	//��־���ݿ��û�
	std::string m_LogdbUser;
	//��־���ݿ�����
	std::string m_LogdbKey;
	//��־���ݿ�����
	std::string m_LogdbName;
	//���ݿ�д��
	CT_INT32	m_iDBUpdataTime;
	//���ݿ�д�뷧ֵ
	CT_INT32	m_iDBUpdataValue;
	
	//���ʱ��
	CT_INT32     m_iGMCheckTime;
	//��ֵⷧ
	CT_INT32     m_iGmDelayTime; 
	//gm��ip
	std::string  m_strGMip;
	//����gm�������Ķ˿�
	CT_WORD		m_wGMHttpPort; 
    // ��Ϸ����������
    std::string m_strServerName;

	//��Ϸ����ID
	CT_INT32	m_iGSID;
	//���IP
	std::string m_strCSIP;
	//����˿�
	CT_DWORD	m_dwCSPort;
public:
	CCGameConfig(CT_VOID);
	~CCGameConfig(CT_VOID);
};
#endif

