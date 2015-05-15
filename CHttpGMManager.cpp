#include <ctime>
#include "CHttpGMManager.h"
#include "../sdk/include/CLogMeg.h"
#include "../sdk/include/CNetInterface.h"
#include "CGlobeData.h"
#include "../sdk/include/CThreadManager.h"
#include "../sdk/json/include/json/json.h"
#include "CGameConfig.h"
#include "CGmOrder.h"

CCHttpGMInfo::CCHttpGMInfo() : m_strInfo(), m_llTimeTag(0)
{

}

CCHttpGMInfo::~CCHttpGMInfo()
{
	m_strInfo.clear();
	m_llTimeTag = 0;
}



template<> CCHttpGMManager* CCSingleton<CCHttpGMManager>::ms_Singleton=0;
CCHttpGMManager::CCHttpGMManager(void)
{
}

CCHttpGMManager::~CCHttpGMManager(void)
{
}


//更新httpInfo数据
CT_VOID CCHttpGMManager::UpdateHttpInfoMap(CCNetInterface* pNetInterface, const std::string& info )
{
	_AUTOMUTEX(&m_mutex);
	HttpGMInfoMap::iterator it = m_httpInfoMap.find(pNetInterface);
	if (it != m_httpInfoMap.end())
	{
		(*it).second.m_strInfo += info;	
		AnalysisHttpData(pNetInterface, (*it).second.m_strInfo);
	}else
	{
		if (pNetInterface->GetIP() ==  ((CCGameConfig&)CCConfig::getSingleton()).m_strGMip && m_httpInfoMap.size() < ((CCGameConfig&)CCConfig::getSingleton()).m_maxRowClientNum  )
		{
			CCHttpGMInfo ht;
			ht.m_strInfo   = info;
			ht.m_llTimeTag = time(0);
			m_httpInfoMap.insert(HttpGMInfoMap::value_type(pNetInterface, ht));
			
			AnalysisHttpData(pNetInterface, info);
			
			
		}else
		{
			CCLogMeg::getSingleton().WriteError("非指定gm发来的信息：ip=%s, info=%s 或者连接满 httpInfoMap.size=%i",	pNetInterface->GetIP(), info.c_str(),  m_httpInfoMap.size());
			DisposeClear(pNetInterface);
		}
	}
}

//删除
CT_BOOL CCHttpGMManager::EaseHttpInfoMap(CCNetInterface* pNetInterface)
{
	_AUTOMUTEX(&m_mutex);
	HttpGMInfoMap::iterator it = m_httpInfoMap.find(pNetInterface);
	if (it != m_httpInfoMap.end())
	{
		m_httpInfoMap.erase(pNetInterface);
		return CT_TRUE;
	}else
	{
		return CT_FALSE;
	}
}

//超时更新
CT_VOID CCHttpGMManager::UpdateHttpInfoMapTimeout()
{  
	_AUTOMUTEX(&m_mutex);
	CT_INT64 llTimeNow = time(0);
	HttpGMInfoMap::iterator it = m_httpInfoMap.begin();
	HttpGMInfoMap::iterator itcp ; 
	for (; it != m_httpInfoMap.end(); )
	{
		itcp = it;
		++itcp;
		if ( llTimeNow -  (*it).second.m_llTimeTag  > ((CCGameConfig&)CCConfig::getSingleton()).m_iGmDelayTime ) 
		{
			CCLogMeg::getSingleton().WriteError("GM连接超时：CCHttpGMManager::UpdateHttpInfoMap(), ip=%s, info=%s",  (*it).first->GetIP(), (*it).second.m_strInfo.c_str());
			(*it).first->Close();
			m_httpInfoMap.erase(it);
		}
		it = itcp;
	}
}



//分析http数据
CT_VOID CCHttpGMManager::AnalysisHttpData(CCNetInterface* pNetInterface, const std::string& info)
{
#define  TAG_BEGIN		("{")
#define  TAG_END		("}")
#define  TAG_MAINID     ("mid")
#define  TAG_SUBID		("sid")

	std::size_t posBegin = info.find(TAG_BEGIN);
	std::size_t posEnd   = info.find(TAG_END);
		
	if (posBegin != std::string::npos && posEnd != std::string::npos && info.find(TAG_MAINID) != std::string::npos && info.find(TAG_SUBID) != std::string::npos)
	{
		std::string data; 
		data.assign(info.c_str() + posBegin,   posEnd - posBegin+1);
				
		Json::Reader reader;
		Json::Value root;
		if (!reader.parse(data.c_str(), root))
		{
			CCLogMeg::getSingleton().WriteError("json解析错误： CCHttpGMManager::AnalysisHttpData, %s", data.c_str());
			pNetInterface->Close();
			DisposeClear(pNetInterface);
			return ;
		}
		CT_DWORD mainID = atoi(root[TAG_MAINID].asCString());
		CT_DWORD subID  = atoi(root[TAG_SUBID].asCString());

		CCGmOrder order; 
		order.DisposeGMOrder(mainID, subID, root, pNetInterface);
		DisposeClear(pNetInterface);
			
		
	}else
	{
		CCLogMeg::getSingleton().WriteError("不合法的GM消息： CCHttpGMManager::AnalysisHttpData, %s", info.c_str());
	}
}


CT_VOID CCHttpGMManager::DisposeClear(CCNetInterface* pNetInterface)
{
	//处理完成删除这个链接
	HttpGMInfoMap::iterator it = m_httpInfoMap.find(pNetInterface);
	if (it != m_httpInfoMap.end())
	{
		m_httpInfoMap.erase(it);
	}
}
