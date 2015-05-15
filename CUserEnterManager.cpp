#include "CMD_Game.h"
#include "CUserEnterManager.h"
#include "GlobeScriptInterface.h"
#include "CLogMeg.h"
#include <time.h>
#include<list>

template<> CCUserEnterManager* CCSingleton<CCUserEnterManager>::ms_Singleton=0;

#define EXIT_DELVALIDATE_TIME		10*60
CCUserEnterManager::CCUserEnterManager(CT_VOID)
{
	G_ValueRange vr={0};
	if(!G_GetValueRange(VALUE_TYPE_DELETEVALIDATE,&vr))
	{
		CCLogMeg::getSingleton().WriteError("CCUserEnterManager::DeleteValidate G_GetValueRange VALUE_TYPE_DELETEVALIDATE");
	}
	m_iTime=(CT_INT32)vr.fMinVal;
}

CCUserEnterManager::~CCUserEnterManager(CT_VOID)
{
}

//插入数据
CT_VOID CCUserEnterManager::Insert(const UserEnterInfo& us)
{
	//_AUTOMUTEX(&m_Mutex);
	_AUTOMUTEX(&m_Mutex);
	UEIM_IT it = m_userEnterInfoMap.find(us.iID);
	if (it != m_userEnterInfoMap.end())
	{
		m_userEnterInfoMap.erase(it);	
	}

	m_userEnterInfoMap.insert(UserEnterInfoMap::value_type(us.iID, us));
}

//查找数据
CT_BOOL CCUserEnterManager::Find(CT_DWORD id)
{
	_AUTOMUTEX(&m_Mutex);
	UEIM_IT it = m_userEnterInfoMap.find(id);
	if (it != m_userEnterInfoMap.end())
	{
		return CT_TRUE;	
	}
	return CT_FALSE;
}

CT_BOOL	CCUserEnterManager::Find(CT_DWORD id,UserEnterInfo& us)
{
	_AUTOMUTEX(&m_Mutex);
	UEIM_IT it = m_userEnterInfoMap.find(id);
	if (it != m_userEnterInfoMap.end())
	{
		memcpy(&us,&((*it).second),sizeof(us));
		return CT_TRUE;	
	}
	return CT_FALSE;
}

//清除数据
CT_VOID CCUserEnterManager::Erase(CT_DWORD id)
{
	_AUTOMUTEX(&m_Mutex);
	UEIM_IT it = m_userEnterInfoMap.find(id);
	if (it != m_userEnterInfoMap.end())
	{
		m_userEnterInfoMap.erase(it);
	}
}


//验证数据
CT_BOOL CCUserEnterManager::Validate(const CMD_G_EnterGame& eg)
{
	_AUTOMUTEX(&m_Mutex);
	UEIM_IT it = m_userEnterInfoMap.find(eg.dwUserID);
	if (it != m_userEnterInfoMap.end())
	{
		if (memcmp(eg.cValidate,(*it).second.cbValidate,sizeof(eg.cValidate)) == 0)
		{
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

//删除验证
CT_VOID CCUserEnterManager::DeleteValidate()
{
	_AUTOMUTEX(&m_Mutex);
	//有问题
	//CT_INT64 lTime=time(0);
	//UEIM_IT ItEnd=m_userEnterInfoMap.end();
	//UEIM_IT it;
	//for(UEIM_IT i=m_userEnterInfoMap.begin();i != ItEnd;)
	//{
	//	it=i;
	//	i++;
	//	if(lTime - (*it).second.llEnterTime > m_iTime)
	//	{
	//		m_userEnterInfoMap.erase(it);
	//	}
	//}
	return;
}

//设置退出时间
CT_VOID CCUserEnterManager::SetExitTime(const CT_DWORD dwUserID)
{
	_AUTOMUTEX(&m_Mutex);
	CT_INT64 lTime=time(0);
	UEIM_IT it=m_userEnterInfoMap.find(dwUserID);
	if(it != m_userEnterInfoMap.end())
	{
		(*it).second.lExitTime=lTime;
	}
	return;
}

//退出超时删除验证
CT_VOID CCUserEnterManager::ExitDeleteValidate()
{
	_AUTOMUTEX(&m_Mutex);
	CT_INT64 lTime=time(0);
	G_ValueRange vr={0};
	G_GetValueRange(VALUE_TYPE_EXITDELVALIDATE,&vr);

	std::list<CT_DWORD> UserIDList;
	for(UEIM_IT i=m_userEnterInfoMap.begin();i != m_userEnterInfoMap.end();++i)
	{
		if((*i).second.lExitTime == 0)
		{
			continue;
		}

		if(lTime - (*i).second.lExitTime > (vr.fMaxVal > EXIT_DELVALIDATE_TIME?vr.fMaxVal:EXIT_DELVALIDATE_TIME))
		{
			UserIDList.push_back((*i).first);
		}
	}

	for(std::list<CT_DWORD>::iterator uit=UserIDList.begin();uit != UserIDList.end();++uit)
	{
		m_userEnterInfoMap.erase((*uit));
	}
	return;
}


