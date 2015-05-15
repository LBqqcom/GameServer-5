#ifndef CCHTTPGMMANAGER_H_
#define CCHTTPGMMANAGER_H_

#include "../sdk/include/CTType.h"
#include "../sdk/include/CSingleton.h"
#include "../sdk/include/CMutex.h"
#include <string>
#include <map>

class CCNetInterface;
class CCHttpGMInfo
{
public:
	//通讯内容
	std::string			m_strInfo;          //网络信息
	CT_INT64            m_llTimeTag;         //时间戳
public:
	CCHttpGMInfo();
	~CCHttpGMInfo();
};


class CCHttpGMManager: public CCSingleton<CCHttpGMManager>
{
	CCMutex      m_mutex;
public:
	CCHttpGMManager(void);
	~CCHttpGMManager(void);
public:
	typedef std::map<CCNetInterface*, CCHttpGMInfo> HttpGMInfoMap;
private:
	HttpGMInfoMap m_httpInfoMap; 

public:
	//更新httpInfo数据
	CT_VOID UpdateHttpInfoMap(CCNetInterface* pNetInterface, const std::string& info );
	//删除
	CT_BOOL EaseHttpInfoMap(CCNetInterface* pNetInterface);
	//超时更新
	CT_VOID UpdateHttpInfoMapTimeout();

private:
	//处理错误
	CT_VOID DisposeClear(CCNetInterface* pNetInterface);
	//分析http数据
	CT_VOID AnalysisHttpData(CCNetInterface* pNetInterface, const std::string& info);

};

#endif