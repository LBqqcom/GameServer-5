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
    CT_INT32 nSigned;                       // 连续签到次数
    CT_INT32 bReward;                       // 签到奖励领取记录
    CT_INT64 llLastSignIn;                  // 上次签到时间
};
typedef CMD_G_ActiveData ActiveData;

class CCUserDaily
{
    CCMutex*    m_pMutex;
    CCUser*     m_pUser;

    typedef std::map<CT_INT32, DailyQuestData>              DailyQuestList;            
    typedef std::map<CT_INT32, DailyQuestData>::iterator    DQ_IT;
    DailyQuestList                                          m_DailyQuestList;   // 日常任务列表

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

	//清理用户数据
	CT_VOID Clear()
	{
		_AUTOMUTEX(m_pMutex);
		memset(&m_SignData, 0, sizeof(m_SignData)); 
		memset(&m_ActiveData, 0, sizeof(m_ActiveData));
        m_DailyQuestList.clear();
	}
public:
    /** 得到日常任务数据
     * @ pBuf   输出缓冲区
     * @ bufLen 输出缓冲区的大小
     */
    CT_BOOL GetDailyQuestData(CT_CHAR* pBuf, CT_INT32& bufLen);

    /** 得到活跃度信息
     * @ pBuf   输出缓冲区
     * @ bufLen 输出缓冲区的大小
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

    /** 插入从数据库读取到的日常任务
     * @ data   任务数据
     */
    CT_VOID InsertQuestData(DailyQuestData &data);

    /** 插入从数据库读取到的签到信息
     * @ data   签到信息
     */
    CT_VOID SetSignData(SignData &data) {
        _AUTOMUTEX(m_pMutex);
        m_SignData = data;
    }

    /** 插入从数据库读取到的活跃度信息
     * @ data   活跃度信息
     */
    CT_VOID SetActiveData(ActiveData &data) {
        _AUTOMUTEX(m_pMutex);
        m_ActiveData = data;
    }

    /** 更新日常任务, 完成日常任务配置中的事件时，更新进度，同步到数据库，并通知客户端
     * @ iQuestID   任务ID
     * @ nProgress  完成的数量
     */
    CT_VOID UpdateQuestData(CT_INT32 iQuestID, CT_INT32 nProgress);

    /** 更新签到记录
     */
    CT_VOID UpdateSignData();

    /** 领取签到奖励
     * @ nSigned    连续签到数
     */
    ENUM_SignRewardFail RecvSignReward(CT_DWORD dwThread, CT_INT32 nSigned, CT_INT32 iDiamond);

    /** 领取活跃度奖励
     * @ iLevel     活跃度等级
     */
    ENUM_ActiveRewardFail RecvActiveReward(CT_DWORD dwThread, CT_INT32 iLevel);

    /** 刷新签到信息, 每天凌晨0点0分0秒，更新签到记录，通知在线用户
     */
    CT_VOID UpdateSignIn(void);

    /** 查找指定任务
     * @ iQuestID   任务ID
     * @ stQuest      任务进度信息
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
    /** 任务完成: 更新活跃度, 同步活跃度到内存和数据库;
     *            获得道具奖励，同步道具内存和数据库
     * @ iQuestID   完成的任务ID
     */
    CT_VOID FinishQuest(CT_INT32 iQuestID);

    /** 活跃度升级
     * Return: 等级增量
     */
    CT_INT32 ActiveUpgrade(CT_BOOL bSendFailMsg);
};
#endif // CUSERDAILY_H_