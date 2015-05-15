/********************************************************************
	created:	2012/10/18
	created:	18:10:2012   11:30
	file base:	CMasterItemEvent
	file ext:	h
	author:		xingweizhen
	
	purpose:	道具相关事件处理
*********************************************************************/
#ifndef CMASTERITEMEVENT_H
#define CMASTERITEMEVENT_H
#include "CGameMesEvent.h"
#include "CUserManager.h"

struct CMD_G_UseItem;
struct CMD_G_MoveItem;
struct CMD_G_IncBagSlot;
struct CMD_G_SortBag;
struct CMD_G_SmithingEquip;
struct CMD_G_InheritEquip;
struct CMD_G_SellItem;
struct CMD_G_ClearCD;

struct MSC_DB_LoseBuffData;
struct MSC_DB_W_ItemData;
struct MSC_DB_W_EquipData;
struct MSC_DB_W_BuffData;
struct MSC_USR_AddItem;

class CCMasterItemEvent: public CCGameMesEvent
{
public:
    CCMasterItemEvent(CCGameMesManager* pMesManager, CT_DWORD id);
    ~CCMasterItemEvent(void);
private:
    //处理用户消息
    virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID, CT_CHAR* pBuf,		CT_DWORD len, CT_LONGLONG reserve);
    //用户定时器处理
    virtual CT_VOID OnTime(CT_DWORD timeID);
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}
private:
    // 处理领主道具事件
    CT_VOID DisposeItemEvent(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
    CT_VOID DisposeDbItemUpdate(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
    CT_VOID DisposeUsrItemUpdate(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
    CT_VOID DisposeElmPowerEvent(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
private:
    // 处理使用道具事件（装备/卸下/合成/使用）
    CT_VOID DisposeUseItem(const CMD_G_UseItem &useItem);
    // 处理道具移动
    CT_VOID DisposeMoveItem(const CMD_G_MoveItem &mvItem);
    // 处理扩展背包数量
    CT_VOID DisposeIncBagSlot(const CMD_G_IncBagSlot &bagSlot);
    // 处理整理背包
    CT_VOID DisposeSortBag(const CMD_G_SortBag &sortBag);
	//处理整理纹章背包
	CT_VOID DisposeElmBag(const CMD_G_SortBag &elmBag);
    // 处理装备强化
    CT_VOID DisposeSmithingEquip(const CMD_G_SmithingEquip &smithingEquip);
    // 处理装备洗炼
    CT_VOID DisposeForgeEquip(const CMD_G_ForgeEquip &forgeEquip);   
	//处理装备洗练属性替换
	CT_VOID DisposeForgeEquipConfirm(const CMD_G_ForgeEquipConfirm &forgeEquipConfirm);  //Add by Victor 2013-1-8
    // 处理装备打孔
    CT_VOID DisposePunchEquip(const CMD_G_PunchEquip &punchEquip);
    // 处理装备镶嵌
    CT_VOID DisposeEmbedEquip(const CMD_G_EmbedEquip &embedEquip);
    // 处理装备继承
    CT_VOID DisposeInheritEquip(const CMD_G_InheritEquip &inheritEquip);
    // 处理出售道具
    CT_VOID DisposeSellItem(const CMD_G_SellItem &sellItem);
    // 处理清除冷却
    CT_VOID DisposeClearCD(const CMD_G_ClearCD &clearCD);
    // 处理附魔请求
    CT_VOID DisposeEnchant(const CMD_G_Enchant &enchant);
	// 处理附魔奖励请求
	CT_VOID DisposeEnchantReward(const CMD_G_EnchantReward &enchantReward);
    // 处理元素吞噬请求
    CT_VOID DisposeEatElm(const CMD_G_EatElm &eatElm);
    // 处理吞噬多个元素请求
    CT_VOID DisposeEatSomeElm(const CMD_G_EatSomeElm &eatSome);
    // 处理元素一键吞噬请求
    CT_VOID DisposeEatAllElm(const CMD_G_EatAllElm &eatAll);
    // 处理元素装备请求
    CT_VOID DisposeEquipElm(const CMD_G_EquipElm &euipElm);
    // 处理元素背包扩展请求
    CT_VOID DisposeIncElmSlot(const CMD_G_IncBagSlot &incSlot);
private:
    // 处理得到增益效果事件
    CT_VOID DisposeGetBuff(const MSC_DB_W_BuffData &itemBuff);
    // 处理失去了增益效果事件
    CT_VOID DisposeLoseBuff(const MSC_DB_LoseBuffData &itemBuff);
    // 用户道具更新
    CT_VOID DisposeChangeItem(const MSC_DB_W_ItemData &itemData);
    // 装备信息更新
    CT_VOID DisposeChangeEquip(const MSC_DB_W_EquipData &equipData);
private:
    // 添加道具
    CT_VOID DisposeAddItem(const MSC_USR_AddItem &addItem);
	//修改领主名称
	CT_VOID DisposeChangeMasterName(CT_CHAR *pBuf,CT_INT32 len);

};

#endif // CMASTERITEMEVENT_H