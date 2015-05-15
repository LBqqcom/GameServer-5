/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCUserEvent.h
Version:		1.0
Description:    用户道具数据
Author:			pengwen
Create Date:	2012-9-29
History:        用户道具数据，包括了道具，装备     

******************************************************************/
#ifndef CCUSERITEM_H_
#define CCUSERITEM_H_
#include <map>
#include "CGDefine.h"
#include "CMutex.h"
#include "CApi.h"
#include "GlobeScriptInterface.h"
#include "CUserData.h"

class  CCUser;
class  CCMutex;
struct BuffData;
struct EquipmentData;
struct CMD_G_ItemData;

enum ITEM_OP_MODE
{
    ITEM_OP_MODE_CHECK = 1,     // 检查
    ITEM_OP_MODE_EXEC  = 2,     // 执行
    ITEM_OP_MODE_CKEXE = 3,     // 检查并执行
};

// 道具信息
struct ItemData
{
    CT_DWORD key;                   // 唯一标识
    CT_INT32 iIndex;                // 道具所在槽索引值
    CT_INT32 iID;                   // 道具ID
	CT_INT32 iCount;                // 道具数量
    CT_CHAR  szName[NAME_SIZE*2];     // 道具名称
};

class CCUserItem : public CCUserData<CT_INT32, ItemData>
{
    friend class CCUserBase;
    friend class CCUserBattleFlag;
    friend class CCUserElmAttr;
private:
    CT_INT32 m_SlotIdx;
public:
    inline CT_INT32 getSlotIdx() { return m_SlotIdx; }
protected:
    virtual CT_INT32 transData(const ItemData &src, CT_CHAR *pDst) const;
public:
    CT_VOID Init(CCUser* pUser, CCMutex* pMutex, CT_INT32 slotIdx); 
    CT_VOID Clear();
    virtual CT_VOID SyncData(CT_INT64 currTime, CT_BOOL bForce = CT_FALSE);
private:
    // 增加道具数量
    CT_VOID AddItemCount(CT_DWORD saveTag, DataMap_IT it, CT_INT32 addValue, CT_BOOL bSendData = CT_TRUE);
    // 交换道具位置
    CT_VOID SwapItem(DataMap_IT itSrc, DataMap_IT itDst, CT_BOOL bSendData = CT_TRUE);
    // 移动道具位置
    CT_VOID MoveItem(DataMap_IT it, CT_INT32 slotTo, CT_BOOL bSendData = CT_TRUE);
    // 道具叠加
    CT_VOID StackItem(DataMap_IT itSrc, DataMap_IT itDst, CT_BOOL bSendData = CT_TRUE);
    // 清理道具信息
    CT_VOID TrimItem(void);
    // 生成道具唯一Key
    CT_UINT32 GenerateKey(void);
    /** 计算当前背包放置某种道具需要多少个空格子
     * @ iItemID   道具ID
     * @ iCount    道具数量
     * Return      消耗的空格子数
     */
    CT_INT32 CalcItemSlot(CT_INT32 iItemID, CT_INT32 iCount);
public:
	// 得到已装备道具列表
	CT_BOOL GetEquipedItemData(CT_CHAR *pBuf, CT_INT32 &bufLen); 
    // 获得空槽数量
    CT_INT32 CalcEmptySlot(CT_INT32 slotType = 0);
    // 查找空的槽
    CT_INT32 SearchSlot(CT_INT32 slotType = 0);
    // 查找道具数据
    CT_BOOL GetClientData(CT_INT32 index, CMD_G_ItemData &data);
    CT_INT32 SearchItemData(CT_INT32 iID, DataMap_IT its[], CT_INT32 &slotCount);        // 某种道具的信息
    CT_BOOL SearchItemData(const EquipmentData &equip, ItemData &data);             // 某个装备的信息
    // 计算某种道具的总数量
    CT_INT32 CalcItemCount(CT_INT32 iItemID);

public:
    /** 消耗一种道具 **
     * @ saveTag    操作的原因
     * @ iItemID    道具ID
     * @ Count      道具数量(正数)
     * @ enMode     = ITEM_OP_MODE_CHECK : 仅检查能否消耗成功，不进行消耗操作
     *              = ITEM_OP_MODE_EXEC  : 不检查，直接进行消耗操作。
     *              = ITEM_OP_MODE_CKEXE : 先检查能否消耗成功，然后进行消耗操作
     * Return       返回成功/失败
     */
    CT_BOOL AutoExpendItem(const CT_DWORD saveTag, const CT_INT32 iItemID, CT_INT32 Count, ITEM_OP_MODE enMode = ITEM_OP_MODE_CKEXE);

    /** 消耗多种道具 **
     * @ saveTag    操作的原因
     * @ iItemIDs   多个道具ID
     * @ Counts     多个道具数量(正数)
     * @ itemTypes  道具种类数
     * @ enMode     = ITEM_OP_MODE_CHECK : 仅检查能否消耗成功，不进行消耗操作
     *              = ITEM_OP_MODE_EXEC  : 不检查，直接进行消耗操作。
     *              = ITEM_OP_MODE_CKEXE : 先检查能否消耗成功，然后进行消耗操作
     * Return       返回成功/失败
     */
    CT_BOOL AutoExpendItem(const CT_DWORD saveTag, const CT_INT32 iItemIDs[], CT_INT32 Counts[], CT_INT32 itemTypes, ITEM_OP_MODE enMode = ITEM_OP_MODE_CKEXE);

public:
    /** 消耗一个格子里的道具
    * @ iSlot      背包格子
    * @ Count      道具数量(正数), 返回消耗之后的道具数量
    * Return       指定的格子内有足够的道具消耗返回真，否则返回假
    */
    CT_BOOL ExpendItem(const CT_DWORD saveTag, const CT_INT32 iSlot, CT_INT32 &Count, ITEM_OP_MODE enMode = ITEM_OP_MODE_CKEXE);

    /** 添加一种道具 **
     * @ saveTag    操作的原因
     * @ iItemID    道具ID
     * @ Count      道具数量, 返回添加后该道具总共的数量
     * @ enMode     = ITEM_OP_MODE_CHECK : 仅检查能否消耗成功，不进行消耗操作
     *              = ITEM_OP_MODE_EXEC  : 不检查，直接进行消耗操作。
     *              = ITEM_OP_MODE_CKEXE : 先检查能否消耗成功，然后进行消耗操作
     * Return       返回未添加的道具（0表示全部添加成功）
     */
    CT_INT32 AutoAddItemData(const CT_DWORD saveTag, const CT_INT32 iItemID, CT_INT32 &iCount, ITEM_OP_MODE enMode = ITEM_OP_MODE_CKEXE);
    // 添加一种道具, 同AutoAddItemData, 输出参数@iSlot表示该道具被放到了哪个格子(如果叠加到多个，仅记录第一个格子 )
    CT_INT32 AddItemData(const CT_DWORD saveTag, const CT_INT32 iItemID, CT_INT32 &iCount, CT_INT32 &iSlot, ITEM_OP_MODE enMode = ITEM_OP_MODE_CKEXE);

    /** 添加多种道具 **
     * @ saveTag    操作的原因
     * @ iItemIDs   多个道具ID
     * @ Counts     多个道具数量
     * @ itemTypes  道具种类数
     * @ enMode     = ITEM_OP_MODE_CHECK : 仅检查能否消耗成功，不进行消耗操作
     *              = ITEM_OP_MODE_EXEC  : 不检查，直接进行消耗操作。
     *              = ITEM_OP_MODE_CKEXE : 先检查能否消耗成功，然后进行消耗操作
     * Return       返回成功/失败
     */
    CT_BOOL AutoAddItemData(const CT_DWORD saveTag, const CT_INT32 iItemIDs[], CT_INT32 Counts[], CT_INT32 itemTypes, ITEM_OP_MODE enMode = ITEM_OP_MODE_CKEXE);
    // 添加多种道具, 同AutoAddItemData, 输出参数@iSlots表示这些道具被放到了哪些格子(如果叠加到多个，仅记录第一个格子 )
    CT_BOOL AddItemData(const CT_DWORD saveTag, const CT_INT32 iItemIDs[], CT_INT32 Counts[], CT_INT32 Slots[], CT_INT32 itemTypes, ITEM_OP_MODE enMode = ITEM_OP_MODE_CKEXE);

    // 交换两个道具的位置
    CT_INT32 SwapItemData(const CT_INT32 iSrcSlot, const CT_INT32 iDstSlot);    
    // 道具排序
    CT_VOID SortItemData();
	// 判断物品道具是否需要交换
	CT_BOOL IsItemSwap(const DataMap_IT &it, const DataMap_IT &itNext);
	// 判断纹章道具是否需要交换
	CT_BOOL IsElmSwap(const DataMap_IT &it, const DataMap_IT &itNext);
	//宝箱开包
	CT_VOID OpenChest(const CT_INT32 iID,CT_INT32 iGoods[],CT_INT32 iCount[],CT_INT32 &iNum,ResAmount Chest[],FakeProBase &fb);

    // 使用制作书合成道具
    CT_INT32 UseItemDesign(const ItemData &useItem);
    // 消耗道具获得增益
    CT_INT32 UseItemBuff(const ItemData &useItem);
    // 使用碎片合成道具
    CT_INT32 UseItemChip(const ItemData &useItem);
    // 装备/卸下
    CT_INT32 UseEquipment(const ItemData &useItem);
    // 打开宝箱
    CT_INT32 UseItemChest(const ItemData &useItem);
    // 使用恢复资源的道具
    CT_INT32 UseItemResource(const ItemData &useItem);
	// 使用魔化道具
	CT_INT32 UseItemEnchant(const ItemData &useItem);
    // 使用广播道具
    CT_INT32 UseItemBroadCast(const ItemData &useItem);
	//使用宠物蛋
	CT_INT32 UseItemPetEgg(const ItemData &useItem);
	// 使用头衔道具
	CT_INT32 UserItemTitle(const ItemData &userItem);
	// 使用BOSS道具
	CT_INT32 UserItemBoss(const ItemData &userItem);
	// 使用战队改名道具
	CT_INT32 UserItemChangeFTName(const ItemData &userItem,CT_CHAR *pName);
	// 使用制作书合成道具
	CT_INT32 UseItemElmPowerDesign(const ItemData &useItem);
	// 使用属性增加
	CT_INT32 UseItemAttributeAdd(const ItemData &useItem);

public:
    // 获得道具名称
    CT_CHAR *GetItemName(const CT_INT32 iID, CT_CHAR *szBuff, CT_INT32 nLen);
    // 检查物品使用条件
    CT_BOOL CheckItemUsable(const G_ItemBase &itBase);
    // 检查槽是否有效
    CT_BOOL CheckSlotValid(const CT_INT32 iIndex);
    // 得到道具数量
    CT_INT32 GetItemCount(const CT_INT32 iItemID);
	//给任务功能增加，判断用户是否已经拥有某道具,返回拥有数量(拥有指定强化等级以上的指定装备	例:拥有一个强化等级300的屠龙刀)
	CT_INT32 UserOwnItemGet(const CT_INT32 iID,const CT_INT32 iLevel); //iLevel装备强化等级
public:
    // 发送道具信息到客户端
    CT_VOID SendItemData(const ItemData &itemData);
    // 同步所有道具信息到数据库 
    //CT_VOID SyncItemData(CT_INT64 currTime, CT_BOOL bForce = CT_FALSE);    
};
#endif