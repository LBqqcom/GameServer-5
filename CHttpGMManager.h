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
	//ͨѶ����
	std::string			m_strInfo;          //������Ϣ
	CT_INT64            m_llTimeTag;         //ʱ���
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
	//����httpInfo����
	CT_VOID UpdateHttpInfoMap(CCNetInterface* pNetInterface, const std::string& info );
	//ɾ��
	CT_BOOL EaseHttpInfoMap(CCNetInterface* pNetInterface);
	//��ʱ����
	CT_VOID UpdateHttpInfoMapTimeout();

private:
	//�������
	CT_VOID DisposeClear(CCNetInterface* pNetInterface);
	//����http����
	CT_VOID AnalysisHttpData(CCNetInterface* pNetInterface, const std::string& info);

};

#endif