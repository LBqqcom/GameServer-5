/********************************************************************
	created:	2012/11/12
	created:	12:11:2012   13:53
	file base:	CUserDaily
	file ext:	h
	author:		xingweizhen
	
	purpose:	
*********************************************************************/
#ifndef CUSERDAILY_H_
#define CUSERDAILY_H_
#include <map>
#include "CGDefine.h"
#include "CMutex.h"
#include "GlobeScriptInterface.h"
#include "CMD_Game.h"
#include "CMasterScriptSetting.h"
#include "CLogMeg.h"

//#define DAILY_DEBUG_LOG(fmt, ...) CCLogMeg::getSingleton().WriteInfo("[DAILY]" fmt, __VA_ARGS__)

class CCMutex;
class CCUser;

struct CMD_G_DailyQuest;
typedef CMD_G_DailyQuestData DailyQuestData;
struct SignData
{
    CT_INT32 nSigned;                       // ����ǩ������
    CT_INT32 bReward;                       // ǩ��������ȡ��¼
    CT_INT64 llLastSignIn;                  // �ϴ�ǩ��ʱ��
};
typedef CMD_G_ActiveData ActiveData;

class CCUserDaily
{
    CCMutex*    m_pMutex;
    CCUser*     m_pUser;

    typedef std::map<CT_INT32, DailyQuestData>              DailyQuestList;            
    typedef std::map<CT_INT32, DailyQuestData>::iterator    DQ_IT;
    DailyQuestList                                          m_DailyQuestList;   // �ճ������б�

    SignData m_SignData;
    ActiveData m_ActiveData;

public:
    CCUserDaily() {}
    ~CCUserDaily() {}
public:
    CT_VOID Init(CCUser* pUser, CCMutex* pMutex)
    {
        m_pUser = pUser;
        m_pMutex = pMutex;

        memset(&m_SignData, 0, sizeof(m_SignData)); 
        memset(&m_ActiveData, 0, sizeof(m_ActiveData));
        m_DailyQuestList.clear();
    }

	//�����û�����
	CT_VOID Clear()
	{
		_AUTOMUTEX(m_pMutex);
		memset(&m_SignData, 0, sizeof(m_SignData)); 
		memset(&m_ActiveData, 0, sizeof(m_ActiveData));
        m_DailyQuestList.clear();
	}
public:
    /** �õ��ճ���������
     * @ pBuf   ���������
     * @ bufLen ����������Ĵ�С
     */
    CT_BOOL GetDailyQuestData(CT_CHAR* pBuf, CT_INT32& bufLen);

    /** �õ���Ծ����Ϣ
     * @ pBuf   ���������
     * @ bufLen ����������Ĵ�С
     */
    CT_BOOL GetActiveData(CT_CHAR* pBuf, CT_INT32& bufLen)
    {
        _AUTOMUTEX(m_pMutex);
        
        if ((CT_UINT32)bufLen < sizeof(m_ActiveData))
        {
            return CT_FALSE;
        }        
        *((CMD_G_ActiveData *)pBuf) = m_ActiveData;
        bufLen = sizeof(m_ActiveData);
        return CT_TRUE;
    }

    /** ��������ݿ��ȡ�����ճ�����
     * @ data   ��������
     */
    CT_VOID InsertQuestData(DailyQuestData &data);

    /** ��������ݿ��ȡ����ǩ����Ϣ
     * @ data   ǩ����Ϣ
     */
    CT_VOID SetSignData(SignData &data) {
        _AUTOMUTEX(m_pMutex);
        m_SignData = data;
    }

    /** ��������ݿ��ȡ���Ļ�Ծ����Ϣ
     * @ data   ��Ծ����Ϣ
     */
    CT_VOID SetActiveData(ActiveData &data) {
        _AUTOMUTEX(m_pMutex);
        m_ActiveData = data;
    }

    /** �����ճ�����, ����ճ����������е��¼�ʱ�����½��ȣ�ͬ�������ݿ⣬��֪ͨ�ͻ���
     * @ iQuestID   ����ID
     * @ nProgress  ��ɵ�����
     */
    CT_VOID UpdateQuestData(CT_INT32 iQuestID, CT_INT32 nProgress);

    /** ����ǩ����¼
     */
    CT_VOID UpdateSignData();

    /** ��ȡǩ������
     * @ nSigned    ����ǩ����
     */
    ENUM_SignRewardFail RecvSignReward(CT_DWORD dwThread, CT_INT32 nSigned, CT_INT32 iDiamond);

    /** ��ȡ��Ծ�Ƚ���
     * @ iLevel     ��Ծ�ȵȼ�
     */
    ENUM_ActiveRewardFail RecvActiveReward(CT_DWORD dwThread, CT_INT32 iLevel);

    /** ˢ��ǩ����Ϣ, ÿ���賿0��0��0�룬����ǩ����¼��֪ͨ�����û�
     */
    CT_VOID UpdateSignIn(void);

    /** ����ָ������
     * @ iQuestID   ����ID
     * @ stQuest      ���������Ϣ
     */
    CT_BOOL SearchDailyQuest(CT_INT32 iQuestID, DailyQuestData &stQuest) {
        _AUTOMUTEX(m_pMutex); 
        DQ_IT it = m_DailyQuestList.find(iQuestID);
        if (it != m_DailyQuestList.end())
        {
            stQuest = it->second;
            return CT_TRUE;
        }
        return CT_FALSE;
    }
private:
    /** �������: ���»�Ծ��, ͬ����Ծ�ȵ��ڴ�����ݿ�;
     *            ��õ��߽�����ͬ�������ڴ�����ݿ�
     * @ iQuestID   ��ɵ�����ID
     */
    CT_VOID FinishQuest(CT_INT32 iQuestID);

    /** ��Ծ������
     * Return: �ȼ�����
     */
    CT_INT32 ActiveUpgrade(CT_BOOL bSendFailMsg);
};
#endif // CUSERDAILY_H_