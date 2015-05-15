/********************************************************************
	created:	2012/11/12
	created:	12:11:2012   13:54
	file base:	CUserDaily
	file ext:	cpp
	author:		xingweizhen
	
	purpose:	
*********************************************************************/
#include "CUserDaily.h"
#include "CUser.h"
#include "CMD_Game.h"
#include "CNetInterface.h"
#include <time.h>
#include "CGlobeData.h"
#include "CThreadMes.h"
#include "CGameConfig.h"
#include "CGlobeFunc.h"
#include "CActive.h"

CT_BOOL CCUserDaily::GetDailyQuestData(CT_CHAR* pBuf, CT_INT32& bufLen)
{
    _AUTOMUTEX(m_pMutex);         
    if ((CT_UINT32)bufLen < m_DailyQuestList.size() * sizeof(DailyQuestData))
    {
        return CT_FALSE;
    }
    int i = 0;
    bufLen = 0;
    for (DQ_IT it = m_DailyQuestList.begin(); it != m_DailyQuestList.end(); ++it)
    {
        //CCLogMeg::getSingleton().WriteInfo("[DAILY] Get: User=%u,ID=%d,Times=%d", m_pUser->GetUserID(), it->second.iQuestID, it->second.iCurrent);
        *((CMD_G_DailyQuestData*)pBuf + (i++)) = it->second;
        bufLen += sizeof(it->second);
    }
    return CT_TRUE;
}

CT_VOID CCUserDaily::InsertQuestData(DailyQuestData &data) {
    _AUTOMUTEX(m_pMutex); 
    //CCLogMeg::getSingleton().WriteInfo("[DAILY] Insert: User=%u,ID=%d,Times=%d", m_pUser->GetUserID(), data.iQuestID, data.iCurrent);
    G_DailyQuest stDaily = {0};
    CCMasterScriptSetting::getSingleton().GetDailyQuest(data.iQuestID, stDaily);
    data.iFinish = stDaily.iCompleteTimes;
    m_DailyQuestList.insert(DailyQuestList::value_type(data.iQuestID, data));
}

CT_INT32 CCUserDaily::ActiveUpgrade(CT_BOOL bSendFailMsg)
{
    ENUM_LevelUpgradeFail enFail = (ENUM_LevelUpgradeFail)-1;
    CT_INT32 addLv = 0;

    // 连续升级
    do
    {
        G_DailyQuestReward stReward = {0};
        if(! CCMasterScriptSetting::getSingleton().GetDailyQuestReward(m_ActiveData.iLevel + 1 + addLv, stReward))
        {
            enFail    = LUF_NO_NEXT_UPGRADE;
            break;
        }

        if (m_ActiveData.iActive < stReward.iActiveValue)
        {
            // 活跃度不足
            enFail    = LUF_NO_XP;
            break;
        }
        addLv++;
    } while (!bSendFailMsg);

    if (addLv > 0)
    {
        m_ActiveData.iLevel += addLv;

        CMD_G_LevelUpgradeSuc ucs;
        memset(&ucs, 0, sizeof(ucs));
        ucs.dwUser = m_pUser->GetUserID();
        ucs.iLevel = m_ActiveData.iLevel;
        ucs.iXp    = m_ActiveData.iActive;
        m_pUser->GetNetInterface()->Send(MAIN_G_DAILY_EVENT, SUB_G_ACTIVE_UPGRADE_SUC,(CT_CHAR*)&ucs, sizeof(ucs), false, true);
    }
    return addLv;
}

// 完成任务
CT_VOID CCUserDaily::FinishQuest(CT_INT32 iQuestID)
{
    G_DailyQuest stQuest = {0};
    CCMasterScriptSetting::getSingleton().GetDailyQuest(iQuestID, stQuest);
    // 活跃度增加
    MasterVariableData mvd = {0};
    m_ActiveData.iActive += stQuest.iActiveReward;
    
    // to Client
    CMD_G_DailyQuestFinish stFinish = {0};
    stFinish.dwUserID = m_pUser->GetUserID();
    stFinish.iQuestID = iQuestID;
    stFinish.iActive = m_ActiveData.iActive;
    m_pUser->GetNetInterface()->Send(MAIN_G_DAILY_EVENT, SUB_G_DAILY_QUEST_FINISH, (CT_CHAR*)&stFinish, sizeof(stFinish), false, true);

    CT_INT32 addLv = ActiveUpgrade(false);

    // 更新活跃度等级和活跃度数值到数据库
    MSC_DB_W_ActiveInfo stInfo = {0};
    stInfo.dwUserID = m_pUser->GetUserID();
    stInfo.iLevel = addLv;
    stInfo.iActive = stQuest.iActiveReward;
    CT_INT32 threadID =  GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
    G_SendThreadInfo(threadID,  MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_W_ACTIVE_INFO, (CT_CHAR*)&stInfo, sizeof(stInfo));
}

// 更新日常任务
CT_VOID CCUserDaily::UpdateQuestData(CT_INT32 iQuestID, CT_INT32 nProgress)
{
    _AUTOMUTEX(m_pMutex); 

    DQ_IT it = m_DailyQuestList.find(iQuestID);
    if (it == m_DailyQuestList.end()) {
        G_DailyQuest stDaily = {0};
        DailyQuestData stData = {0};
        stData.iQuestID = iQuestID;
        if(CCMasterScriptSetting::getSingleton().GetDailyQuest(iQuestID, stDaily)) {
            stData.iFinish = stDaily.iCompleteTimes;
        }
        m_DailyQuestList.insert(DailyQuestList::value_type(stData.iQuestID, stData));
        it = m_DailyQuestList.find(iQuestID);
    }
    if (it != m_DailyQuestList.end() && (it->second.iFinish == 0 || it->second.iCurrent + nProgress <= it->second.iFinish)) {
        DailyQuestData stData = it->second;
        stData.iCurrent += nProgress;
        
        // to Memory
        it->second = stData;
        
        // 更新任务进度到数据库
        MSC_DB_W_DailyQuest stQuest = {0};
        stQuest.dwUserID = m_pUser->GetUserID();
        stQuest.iQuestID = stData.iQuestID;
        stQuest.iCurrent = nProgress;
        CT_INT32 threadID =  GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
        G_SendThreadInfo(threadID,  MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_W_DAILY_QUEST, (CT_CHAR*)&stQuest, sizeof(stQuest));

        // to Client
        CMD_G_DailyQuest st = {0};
        st.dwUserID = m_pUser->GetUserID();
        st.iQuestID = stData.iQuestID;
        st.iCurrent = stData.iCurrent;
        st.iFinish  = stData.iFinish;
        m_pUser->GetNetInterface()->Send(MAIN_G_DAILY_EVENT, SUB_G_DAILY_QUEST_INFO, (CT_CHAR*)&st, sizeof(st), false, true);

        if (stData.iCurrent == stData.iFinish)
        {
            FinishQuest(iQuestID);
        }
    }
}

// 更新签到记录
CT_VOID CCUserDaily::UpdateSignData()
{
    _AUTOMUTEX(m_pMutex); 
    // 无效时间不处理
    if (m_SignData.llLastSignIn == 0) return;
    
    // 当前时间
    time_t llCurrentTime = time(0);
    struct tm *local = NULL;
    struct tm currTime = {0};
    struct tm lastTime = {0};
    struct tm tmpTime = {0};
    local = localtime(&llCurrentTime);
    if (local != NULL)
    {
        currTime = *local;
    }

    // 上次签到时间
    local = localtime((time_t*)&m_SignData.llLastSignIn);
    if (local != NULL) {
        lastTime = *local;

        CT_CHAR szLog[256] = "";
        sprintf(szLog, "Last SignIn: %lld = %04d-%02d-%02d %02d:%02d:%02d", m_SignData.llLastSignIn,
            local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);
        CCLogMeg::getSingleton().WriteInfo("%s", szLog);
    }    

    // 前一天
    tmpTime.tm_year = currTime.tm_year;
    tmpTime.tm_mon = currTime.tm_mon;
    tmpTime.tm_mday = currTime.tm_mday - 1;
    mktime(&tmpTime);

    CT_BOOL bFirstLogin = CT_TRUE;
    // 上次签到是前一天
    if (tmpTime.tm_year == lastTime.tm_year && tmpTime.tm_mon == lastTime.tm_mon && tmpTime.tm_mday == lastTime.tm_mday)
    {
        if (m_SignData.nSigned < MAX_SIGN_TIMES) {
            m_SignData.nSigned += 1;
        }
        m_SignData.bReward = 0;

		//登陆天数活动
		CCActive::getSingleton().ActiveProcess(ACTIVE_LOGIN_DAYS,1,m_pUser);
    }
    // 同一天登陆
    else if (currTime.tm_year == lastTime.tm_year && currTime.tm_mon == lastTime.tm_mon && currTime.tm_mday == lastTime.tm_mday)
    {
        // 没有操作
        bFirstLogin = CT_FALSE;
    }
    else
    {
        //清除连续签到记录
        memset(&m_SignData, 0, sizeof(m_SignData));

        MSC_DB_D_SignInfo stSign = {0};
        stSign.dwUserID = m_pUser->GetUserID();
        CT_INT32 threadID =  GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
        G_SendThreadInfo(threadID,  MSC_MAIN_DB_DEL_DATA, MSC_SUB_DB_D_SIGN_DATA, (CT_CHAR*)&stSign, sizeof(stSign));

        m_SignData.nSigned += 1;
		//登陆天数活动
		CCActive::getSingleton().ActiveProcess(ACTIVE_LOGIN_DAYS,1,m_pUser);
    }

    if (bFirstLogin)
    {
        // 更新连续签到次数到数据库
        MSC_DB_W_SignInfo stSign = {0};
        stSign.dwUserID = m_pUser->GetUserID();
        stSign.nSigned = 1;
        stSign.bReward = m_SignData.bReward;
        CT_INT32 threadID =  GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
        G_SendThreadInfo(threadID,  MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_W_SIGN_INFO, (CT_CHAR*)&stSign, sizeof(stSign));

        m_SignData.llLastSignIn = time(0);
    }

    CMD_G_DailyLogin stLogin = {0};
    stLogin.dwUserID = m_pUser->GetUserID();
    stLogin.iCurrent = m_SignData.nSigned;
    stLogin.iFinish = MAX_SIGN_TIMES;
    stLogin.bReward = m_SignData.bReward;
    m_pUser->GetNetInterface()->Send(MAIN_G_ENTER_ENV, SUB_G_SIGN_INFO, (CT_CHAR*)&stLogin, sizeof(stLogin), false, true);
}

// 领取签到奖励
ENUM_SignRewardFail CCUserDaily::RecvSignReward(CT_DWORD dwThread, CT_INT32 nSigned, CT_INT32 iDiamond)
{
    ENUM_SignRewardFail enFail = ESR_DATA_ERROR;
    G_DailySignIn stReward = {0};
    if(CCMasterScriptSetting::getSingleton().GetSignReward(nSigned, iDiamond, stReward))
    {
        if (m_SignData.bReward != 0) {
            return ESR_RECIEVED;
        }
        if (m_SignData.nSigned != nSigned)
        {
            return ESR_EXPIRED;
        }

        CT_INT32 IDs[SIGNIN_REWARD_ITEM_CNT] = {0};
        CT_INT32 Cnts[SIGNIN_REWARD_ITEM_CNT] = {0};
        IDs[0] = stReward.iItem1Type;
        IDs[1] = stReward.iItem2Type;
        IDs[2] = stReward.iItem3Type;
        IDs[3] = stReward.iItem4Type;
        Cnts[0] = stReward.iItem1Value;
        Cnts[1] = stReward.iItem2Num;
        Cnts[2] = stReward.iItem3Num;
        Cnts[3] = stReward.iItem4Num;
        if (!m_pUser->m_userItem.AutoAddItemData(SAVETAG_RECV_SIGN_REWARD, IDs, Cnts, SIGNIN_REWARD_ITEM_CNT, ITEM_OP_MODE_CHECK))
        {
            return ESR_NO_SLOT;
        }
        enFail = ESR_SUCCESS; 
        m_SignData.bReward = CT_TRUE;
        
        // 更新到内存
        MasterVariableData mvd = {0};
        mvd.stRes.iGold = stReward.iGold;
        mvd.stRes.iFood = stReward.iFood;
        mvd.stRes.iStamina = stReward.iStamina;
        mvd.stRes.iTalentNum = stReward.iTalent;
        mvd.stRank.iCurrentExp = stReward.iPrestige;
        mvd.stRes.iDiamod = stReward.iDiamonCost;
        m_pUser->m_userBase.AddMastData(mvd, SAVETAG_RECV_SIGN_REWARD);

        m_pUser->m_userItem.AutoAddItemData(SAVETAG_RECV_SIGN_REWARD, IDs, Cnts, SIGNIN_REWARD_ITEM_CNT, ITEM_OP_MODE_EXEC);

        // 更新领取奖励记录到数据库
        //MSC_DB_W_SignReward stDb = {0};
		MSC_LOGDB_SignReward stDb={0};
        stDb.dwUserID = m_pUser->GetUserID();
		stDb.iUserGrade = m_pUser->m_userBase.m_masterData.mvd.stGrade.iLevel;
        stDb.nSigned = nSigned;
        stDb.iGold = stReward.iGold;
        stDb.iFood = stReward.iFood;
        stDb.iStamina = stReward.iStamina;
        stDb.iTalentNum = stReward.iTalent;
        stDb.iRankXp = stReward.iPrestige;
        for (CT_INT32 i = 0; i < SIGNIN_REWARD_ITEM_CNT; ++i)
        {
            stDb.ItemReward[i].iID = IDs[i];
            stDb.ItemReward[i].iNum = Cnts[i];
        }
        stDb.iCostDiamond = stReward.iDiamonCost;
        CT_INT32 threadID =  GAME_THREAD_LOGDATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
        G_SendThreadInfo(threadID,  MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_SIGNREWARD, (CT_CHAR*)&stDb, sizeof(stDb));

        // 更新领取记录
        MSC_DB_W_SignInfo stSign = {0};
        stSign.dwUserID = m_pUser->GetUserID();
        stSign.nSigned = 0;
        stSign.bReward = 1;
        threadID =  GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
        G_SendThreadInfo(threadID,  MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_W_SIGN_INFO, (CT_CHAR*)&stSign, sizeof(stSign));

        // 更新到客户端
        m_pUser->m_userBase.GetMasterData(mvd);
        CMD_G_SignRewardSuc stSuc = {0};
        stSuc.dwUserID = m_pUser->GetUserID();
        stSuc.nSigned = nSigned;
        stSuc.iGold = mvd.stRes.iGold;
        stSuc.iFood = mvd.stRes.iFood;
        stSuc.iStamina = mvd.stRes.iStamina;
        stSuc.iTalent = mvd.stRes.iTalentNum;
        stSuc.iPrestige = mvd.stRank.iCurrentExp;
        stSuc.iDiamond = mvd.stRes.iDiamod;
        m_pUser->GetNetInterface()->Send(MAIN_G_DAILY_EVENT, SUB_G_SIGN_REWARD_SUC,(CT_CHAR*)&stSuc, sizeof(stSuc), false, true); 
    }
    return enFail;
}

// 领取活跃度奖励
ENUM_ActiveRewardFail CCUserDaily::RecvActiveReward(CT_DWORD dwThread, CT_INT32 iLevel)
{
    ENUM_ActiveRewardFail enFail = EAR_DATA_ERROR;
    
    G_DailyQuestReward stReward = {0};
    if(CCMasterScriptSetting::getSingleton().GetDailyQuestReward(iLevel, stReward))
    {
        if (m_ActiveData.iLevel < iLevel)
        {
            return ESR_NO_LEVEL;
        }
        if (m_ActiveData.bReward[iLevel - 1])
        {
            return EAR_RECIEVED;
        }

        CT_INT32 IDs[SIGNIN_REWARD_ITEM_CNT] = {0};
        CT_INT32 Cnts[SIGNIN_REWARD_ITEM_CNT] = {0};
        IDs[0] = stReward.iItem1Type;
        IDs[1] = stReward.iItem2Type;
        IDs[2] = stReward.iItem3Type;
        IDs[3] = stReward.iItem4Type;
        Cnts[0] = stReward.iItem1Value;
        Cnts[1] = stReward.iItem2Num;
        Cnts[2] = stReward.iItem3Num;
        Cnts[3] = stReward.iItem4Num;
        if (!m_pUser->m_userItem.AutoAddItemData(SAVETAG_RECV_ACTIVE_REWARD, IDs, Cnts, SIGNIN_REWARD_ITEM_CNT, ITEM_OP_MODE_CHECK))
        {
            return EAR_NO_SLOT;
        }        
        enFail = EAR_SUCCESS;
        m_ActiveData.bReward[iLevel - 1] = CT_TRUE;

        // 更新到内存
        MasterVariableData mvd = {0};
        mvd.stRes.iGold = stReward.iGold;
        mvd.stRes.iFood = stReward.iFood;
        mvd.stRes.iStamina = stReward.iStamina;
        mvd.stRes.iTalentNum = stReward.iTalent;
        mvd.stRank.iCurrentExp = stReward.iPrestige;
        m_pUser->m_userBase.AddMastData(mvd, SAVETAG_RECV_ACTIVE_REWARD);

        m_pUser->m_userItem.AutoAddItemData(SAVETAG_RECV_ACTIVE_REWARD, IDs, Cnts, SIGNIN_REWARD_ITEM_CNT, ITEM_OP_MODE_EXEC);

		//更新记录到游戏数据库
		MSC_DB_W_ActiveReward  dbar = {0};
		dbar.dwUserID = m_pUser->GetUserID();
		dbar.iLevel = iLevel;
		CT_INT32 threadID =  GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,  MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_W_ACTIVE_REWARD, (CT_CHAR*)&dbar, sizeof(dbar));

        // 更新领取奖励记录到日志数据库
        MSC_LOGDB_ActiveReward stDb = {0};
        stDb.dwUserID = m_pUser->GetUserID();
        stDb.iLevel= iLevel;
        stDb.iGold = stReward.iGold;
        stDb.iFood = stReward.iFood;
        stDb.iStamina = stReward.iStamina;
        stDb.iTalentNum = stReward.iTalent;
        stDb.iRankXp = stReward.iPrestige;
        stDb.ItemReward[0].iID = stReward.iItem1Type;
        stDb.ItemReward[1].iID = stReward.iItem2Type;
        stDb.ItemReward[2].iID = stReward.iItem3Type;
        stDb.ItemReward[3].iID = stReward.iItem4Type;
        stDb.ItemReward[0].iNum = stReward.iItem1Value;
        stDb.ItemReward[1].iNum = stReward.iItem2Num;
        stDb.ItemReward[2].iNum = stReward.iItem3Num;
        stDb.ItemReward[3].iNum = stReward.iItem4Num;
        CT_DWORD threadid = GAME_THREAD_LOGDATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
        G_SendThreadInfo(threadid,  MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_ACTIVEREWARD, (CT_CHAR*)&stDb, sizeof(stDb));

        // 更新到客户端
        m_pUser->m_userBase.GetMasterData(mvd);
        CMD_G_ActiveRewardSuc stSuc = {0};
        stSuc.dwUserID = m_pUser->GetUserID();
        stSuc.iLevel = iLevel;
        stSuc.iGold = mvd.stRes.iGold;
        stSuc.iFood = mvd.stRes.iFood;
        stSuc.iStamina = mvd.stRes.iStamina;
        stSuc.iTalent = mvd.stRes.iTalentNum;
        stSuc.iPrestige = mvd.stRank.iCurrentExp;
        m_pUser->GetNetInterface()->Send(MAIN_G_DAILY_EVENT, SUB_G_DAILY_REWARD_SUC,(CT_CHAR*)&stSuc, sizeof(stSuc), false, true);
    }
    return enFail;
}

// 刷新签到信息
CT_VOID CCUserDaily::UpdateSignIn(void)
{
    _AUTOMUTEX(m_pMutex);

    // 清空活跃度和日常任务
    memset(&m_ActiveData, 0, sizeof(m_ActiveData));
    m_DailyQuestList.clear();
    // 更新签到信息
    UpdateSignData();
    // 完成当前首次登录任务
    UpdateQuestData(DAILY_QUEST_SIGNIN, 1);
}