/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		UserEnterInfo.h
Version:		1.0
Description:    用户登入服务
Author:			pengwen
Create Date:	2012-8-20
History:        管理中心服务器发来的用户登录消息     

******************************************************************/

#ifndef CCUSERENTERMANGER_H_
#define CCUSERENTERMANGER_H_
#include <map>
#include "CSingleton.h"
#include "CTType.h"
#include "CMutex.h"

//用户进入许可信息
struct UserEnterInfo
{
	CT_DWORD            iID;				//用户id
	CT_BYTE				cbValidate[VALIDATE];	 //验证信息
	CT_LONGLONG         llEnterTime;        //用户登录时间
	CT_BOOL				bTalk;				//禁言
	CT_INT64			lExitTime;        //用户退出时间
};

class CCUserEnterManager : public CCSingleton<CCUserEnterManager>
{
	CCMutex  m_Mutex;
public:
	typedef std::map<CT_DWORD, UserEnterInfo>			UserEnterInfoMap;
	typedef std::map<CT_DWORD, UserEnterInfo>::iterator UEIM_IT;
private:
	//用户进入信息表,这些信息是从中心服务器发来的
	UserEnterInfoMap	m_userEnterInfoMap;	

	//
	CT_INT32			m_iTime;

public:
	CCUserEnterManager(CT_VOID);
	~CCUserEnterManager(CT_VOID);

public:
	//插入数据
	CT_VOID Insert(const UserEnterInfo& us);
	//查找数据
	CT_BOOL Find(CT_DWORD id);
	CT_BOOL	Find(CT_DWORD id,UserEnterInfo& us);
	//清除数据
	CT_VOID Erase(CT_DWORD id);
	//清空
	CT_VOID Clear()
	{
		m_userEnterInfoMap.clear();
	}
	//验证数据,是否玩家在队列中
	CT_BOOL Validate(const CMD_G_EnterGame& eg);
	//删除验证
	CT_VOID DeleteValidate();
	//设置退出时间
	CT_VOID SetExitTime(const CT_DWORD dwUserID);
	//退出超时删除验证
	CT_VOID ExitDeleteValidate();


};
#endif