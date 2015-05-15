/********************************************************************
	created:	2012/09/20
	created:	20:9:2012   15:23
	file base:	CDBMasterEvent
	file ext:	h
	author:		xingweizhen
	
	purpose:	领主DB类
*********************************************************************/

#ifndef CCDBMASTEREVENT_H_
#define CCDBMASTEREVENT_H_

#include "CGameMesEvent.h"
#include "CThreadMes.h"

class CCDBConnector;
class CCGameMesManager;

#if 0
struct MSC_DB_W_TalentData;
struct MSC_DB_W_SkillData;
struct MSC_DB_W_ItemData;
struct MSC_DB_W_EquipData;
struct MSC_DB_W_BuffData;
struct MSC_DB_LoseBuffData;
struct MSC_DB_W_SwapItemData;
struct MSC_DB_W_SmithingAndSlot;
struct MSC_DB_W_ForgeData;
struct MSC_DB_W_EmbedData;
struct MSC_DB_W_InheritData;
struct MSC_DB_W_BagSlot;
struct MSC_DB_W_BuyStatmina;
struct MSC_DB_A_Charge;
struct MSC_DB_U_Charge;
struct MSC_DB_W_EnchantData;
struct MSC_DB_SyncItemData;
struct MSC_DB_SyncBuffData;
struct MSC_DB_SyncEquipData;
#endif

class CCDBMasterEvent :
    public CCGameMesEvent
{
private:
    CCDBConnector& m_dbConnector;
public:
    CCDBMasterEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
    ~CCDBMasterEvent(void);
private:
    //处理用户消息
    virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID, CT_CHAR* pBuf,		CT_DWORD len, CT_LONGLONG reserve);
    //用户定时器处理
    virtual CT_VOID OnTime(CT_DWORD timeID);
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID);

    // 处理信息保存
    CT_VOID DisposeSaveData(CT_DWORD subID, CT_CHAR* pBuf,	CT_DWORD len);
private:
    // 保存潜能信息
    CT_VOID DisposeTalentSaveData(const MSC_DB_W_TalentData& talentSaveData);
    // 保存技能信息
    CT_VOID DisposeSkillSaveData(const MSC_DB_W_SkillData& skillSaveData);    
    // 添加充值记录
    CT_VOID DisposeAddCharge(const MSC_DB_A_Charge &addCharge);
    // 更新充值记录
    CT_VOID DisposeUpdateCharge(const MSC_DB_U_Charge &updateCharge);
    // 更新附魔记录
    CT_VOID DisposeUpdateEnchant(const MSC_DB_W_EnchantData &enchantData);
	// 更新附魔失败记录
	CT_VOID DisposeUpdataEnchantFail(const MSC_DB_EnchantFailTime &enchantFail);
	// 更新附魔奖励记录
	CT_VOID DisposeUpdataEnchantReward(const MSC_DB_EnchantReward &enchantReward);
    // 同步道具信息
    CT_VOID DisposeSyncItemData(const MSC_DB_SyncItemData &syncData);
    // 同步增益信息
    CT_VOID DisposeSyncBuffData(const MSC_DB_SyncBuffData &syncData);
    // 同步装备信息
    CT_VOID DisposeSyncEquipData(const MSC_DB_SyncEquipData &syncData);
    // 同步元素信息
    CT_VOID DisposeSyncElmData(const MSC_DB_SyncItemData &syncData);
    // 同步元素属性信息
    CT_VOID DisposeSyncElmAttrData(const MSC_DB_SyncElmAttrData &syncData);
    // 保存战旗信息
    CT_VOID DisposeBattleFlagSaveData(const MSC_DB_SaveBattleFlag &saveData);
	// 保存头衔信息
	CT_VOID DisposeUpdataTitleData(const MSC_DB_TitleData &syncData);
	// 保存头衔列表
	CT_VOID DisposeTitleListSave();
};

#endif // CCDBMASTEREVENT_H_