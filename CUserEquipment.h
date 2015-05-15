/********************************************************************
	created:	2012/10/18
	created:	18:10:2012   11:28
	file base:	CUserEquipment
	file ext:	h
	author:		xingweizhen
	
	purpose:	装备强化/洗练/镶孔信息
*********************************************************************/
#ifndef CUSEREQUIPMENT_H
#define CUSEREQUIPMENT_H
#include <map>
#include "CGDefine.h"
#include "CMutex.h"
#include "CApi.h"
#include "GlobeScriptInterface.h"
#include "CMD_Game.h"
#include "CUserData.h"
#include "CUserBase.h"
#include "CUserItem.h"

class CCUser;
class CCMutex;

struct ItemData;
struct CMD_G_Forge;
struct CMD_G_ForgeEquip;
struct EquipmentData
{
    CT_DWORD        key;               // 标识唯一装备
    CT_INT32        iLevel;            // 强化等级
    CT_INT32        nSlot;             // 镶孔数量
    CT_INT32        Gem[GEM_COUNTS];   // 宝石信息
    CMD_G_Forge     Forge[FORGE_ATTRS];// 洗炼信息
};

class CCUserEquipment : public CCUserData<CT_DWORD, EquipmentData>
{
    friend class CCUserItem;
private:
	DataMap m_EquipForgeTempDataList; //装备洗练临时数据    
protected:
    virtual CT_INT32 transData(const EquipmentData &src, CT_CHAR *pDst) const;
public:
    CT_VOID Init(CCUser* pUser, CCMutex* pMutex);
	CT_VOID Clear();
    virtual CT_VOID SyncData(CT_INT64 currTime, CT_BOOL bForce = CT_FALSE);
public:
    // 强化装备
    CT_INT32 SmithingEquip(const ItemData &item, EquipmentData &equip);
    // 洗练装备
    CT_INT32 ForgeEquip(const ItemData &item, const CMD_G_ForgeEquip &forge, EquipmentData &equip);
	//洗练装备属性替换确认
	CT_INT32 ForgeEquipConfirm(const ItemData &item, EquipmentData &equip, ENUM_ForgeEquipConfirm confirmFlag);
    // 打孔
    CT_INT32 PunchEquip(const ItemData &item, EquipmentData &equip);
    // 镶嵌
    CT_INT32 EmbedEquip(const ItemData &item, const CMD_G_EmbedEquip &embed, EquipmentData &equip);
    // 继承
    CT_INT32 InheritEquip(const ItemData &src, const ItemData &dst, EquipmentData &eqSrc, EquipmentData &eqDst);
public:
    // 检查消耗
    CT_INT32 CheckExpend(const CT_INT32 iGold, const CT_INT32 iDiamond, const CT_INT32 iItemID, const CT_INT32 iItemNum, const CT_INT32 iLevel = 0);    
private:    
    // 计算强化消耗
    CT_VOID CalcSmithingCost(const EquipmentData &equipData, CT_INT32 &iGold, CT_INT32 &iDiamond, CT_INT32 &iItemID, CT_INT32 &iItemCnt);
    // 计算打孔消耗
    CT_VOID CalcPunchCost(const EquipmentData &equipData, CT_INT32 &iGold, CT_INT32 &iDiamond, CT_INT32 &iItemID, CT_INT32 &iItemCnt);
    // 计算洗炼消耗
    CT_VOID CalcForgeCost(const G_ItemBase &itBase, const CMD_G_ForgeEquip &forgeData, CT_INT32 &iGold, CT_INT32 &iDiamond, CT_INT32 &iItemID, CT_INT32 &iItemCnt);
    // 计算继承
    CT_VOID CalcInheritCost(const ItemData &itSrc, const ItemData &itDst, EquipmentData &equipSrc, EquipmentData &equipDst);
public:
    static CT_VOID UpdateBattleStatus(MasterCombatStatus &mcs, const CCUserItem::DataMap &itemMap, const DataMap &equipMap);
};


#endif // CUSEREQUIPMENT_H