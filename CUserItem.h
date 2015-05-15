/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCUserEvent.h
Version:		1.0
Description:    �û���������
Author:			pengwen
Create Date:	2012-9-29
History:        �û��������ݣ������˵��ߣ�װ��     

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
    ITEM_OP_MODE_CHECK = 1,     // ���
    ITEM_OP_MODE_EXEC  = 2,     // ִ��
    ITEM_OP_MODE_CKEXE = 3,     // ��鲢ִ��
};

// ������Ϣ
struct ItemData
{
    CT_DWORD key;                   // Ψһ��ʶ
    CT_INT32 iIndex;                // �������ڲ�����ֵ
    CT_INT32 iID;                   // ����ID
	CT_INT32 iCount;                // ��������
    CT_CHAR  szName[NAME_SIZE*2];     // ��������
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
    // ���ӵ�������
    CT_VOID AddItemCount(CT_DWORD saveTag, DataMap_IT it, CT_INT32 addValue, CT_BOOL bSendData = CT_TRUE);
    // ��������λ��
    CT_VOID SwapItem(DataMap_IT itSrc, DataMap_IT itDst, CT_BOOL bSendData = CT_TRUE);
    // �ƶ�����λ��
    CT_VOID MoveItem(DataMap_IT it, CT_INT32 slotTo, CT_BOOL bSendData = CT_TRUE);
    // ���ߵ���
    CT_VOID StackItem(DataMap_IT itSrc, DataMap_IT itDst, CT_BOOL bSendData = CT_TRUE);
    // ���������Ϣ
    CT_VOID TrimItem(void);
    // ���ɵ���ΨһKey
    CT_UINT32 GenerateKey(void);
    /** ���㵱ǰ��������ĳ�ֵ�����Ҫ���ٸ��ո���
     * @ iItemID   ����ID
     * @ iCount    ��������
     * Return      ���ĵĿո�����
     */
    CT_INT32 CalcItemSlot(CT_INT32 iItemID, CT_INT32 iCount);
public:
	// �õ���װ�������б�
	CT_BOOL GetEquipedItemData(CT_CHAR *pBuf, CT_INT32 &bufLen); 
    // ��ÿղ�����
    CT_INT32 CalcEmptySlot(CT_INT32 slotType = 0);
    // ���ҿյĲ�
    CT_INT32 SearchSlot(CT_INT32 slotType = 0);
    // ���ҵ�������
    CT_BOOL GetClientData(CT_INT32 index, CMD_G_ItemData &data);
    CT_INT32 SearchItemData(CT_INT32 iID, DataMap_IT its[], CT_INT32 &slotCount);        // ĳ�ֵ��ߵ���Ϣ
    CT_BOOL SearchItemData(const EquipmentData &equip, ItemData &data);             // ĳ��װ������Ϣ
    // ����ĳ�ֵ��ߵ�������
    CT_INT32 CalcItemCount(CT_INT32 iItemID);

public:
    /** ����һ�ֵ��� **
     * @ saveTag    ������ԭ��
     * @ iItemID    ����ID
     * @ Count      ��������(����)
     * @ enMode     = ITEM_OP_MODE_CHECK : ������ܷ����ĳɹ������������Ĳ���
     *              = ITEM_OP_MODE_EXEC  : ����飬ֱ�ӽ������Ĳ�����
     *              = ITEM_OP_MODE_CKEXE : �ȼ���ܷ����ĳɹ���Ȼ��������Ĳ���
     * Return       ���سɹ�/ʧ��
     */
    CT_BOOL AutoExpendItem(const CT_DWORD saveTag, const CT_INT32 iItemID, CT_INT32 Count, ITEM_OP_MODE enMode = ITEM_OP_MODE_CKEXE);

    /** ���Ķ��ֵ��� **
     * @ saveTag    ������ԭ��
     * @ iItemIDs   �������ID
     * @ Counts     �����������(����)
     * @ itemTypes  ����������
     * @ enMode     = ITEM_OP_MODE_CHECK : ������ܷ����ĳɹ������������Ĳ���
     *              = ITEM_OP_MODE_EXEC  : ����飬ֱ�ӽ������Ĳ�����
     *              = ITEM_OP_MODE_CKEXE : �ȼ���ܷ����ĳɹ���Ȼ��������Ĳ���
     * Return       ���سɹ�/ʧ��
     */
    CT_BOOL AutoExpendItem(const CT_DWORD saveTag, const CT_INT32 iItemIDs[], CT_INT32 Counts[], CT_INT32 itemTypes, ITEM_OP_MODE enMode = ITEM_OP_MODE_CKEXE);

public:
    /** ����һ��������ĵ���
    * @ iSlot      ��������
    * @ Count      ��������(����), ��������֮��ĵ�������
    * Return       ָ���ĸ��������㹻�ĵ������ķ����棬���򷵻ؼ�
    */
    CT_BOOL ExpendItem(const CT_DWORD saveTag, const CT_INT32 iSlot, CT_INT32 &Count, ITEM_OP_MODE enMode = ITEM_OP_MODE_CKEXE);

    /** ���һ�ֵ��� **
     * @ saveTag    ������ԭ��
     * @ iItemID    ����ID
     * @ Count      ��������, ������Ӻ�õ����ܹ�������
     * @ enMode     = ITEM_OP_MODE_CHECK : ������ܷ����ĳɹ������������Ĳ���
     *              = ITEM_OP_MODE_EXEC  : ����飬ֱ�ӽ������Ĳ�����
     *              = ITEM_OP_MODE_CKEXE : �ȼ���ܷ����ĳɹ���Ȼ��������Ĳ���
     * Return       ����δ��ӵĵ��ߣ�0��ʾȫ����ӳɹ���
     */
    CT_INT32 AutoAddItemData(const CT_DWORD saveTag, const CT_INT32 iItemID, CT_INT32 &iCount, ITEM_OP_MODE enMode = ITEM_OP_MODE_CKEXE);
    // ���һ�ֵ���, ͬAutoAddItemData, �������@iSlot��ʾ�õ��߱��ŵ����ĸ�����(������ӵ����������¼��һ������ )
    CT_INT32 AddItemData(const CT_DWORD saveTag, const CT_INT32 iItemID, CT_INT32 &iCount, CT_INT32 &iSlot, ITEM_OP_MODE enMode = ITEM_OP_MODE_CKEXE);

    /** ��Ӷ��ֵ��� **
     * @ saveTag    ������ԭ��
     * @ iItemIDs   �������ID
     * @ Counts     �����������
     * @ itemTypes  ����������
     * @ enMode     = ITEM_OP_MODE_CHECK : ������ܷ����ĳɹ������������Ĳ���
     *              = ITEM_OP_MODE_EXEC  : ����飬ֱ�ӽ������Ĳ�����
     *              = ITEM_OP_MODE_CKEXE : �ȼ���ܷ����ĳɹ���Ȼ��������Ĳ���
     * Return       ���سɹ�/ʧ��
     */
    CT_BOOL AutoAddItemData(const CT_DWORD saveTag, const CT_INT32 iItemIDs[], CT_INT32 Counts[], CT_INT32 itemTypes, ITEM_OP_MODE enMode = ITEM_OP_MODE_CKEXE);
    // ��Ӷ��ֵ���, ͬAutoAddItemData, �������@iSlots��ʾ��Щ���߱��ŵ�����Щ����(������ӵ����������¼��һ������ )
    CT_BOOL AddItemData(const CT_DWORD saveTag, const CT_INT32 iItemIDs[], CT_INT32 Counts[], CT_INT32 Slots[], CT_INT32 itemTypes, ITEM_OP_MODE enMode = ITEM_OP_MODE_CKEXE);

    // �����������ߵ�λ��
    CT_INT32 SwapItemData(const CT_INT32 iSrcSlot, const CT_INT32 iDstSlot);    
    // ��������
    CT_VOID SortItemData();
	// �ж���Ʒ�����Ƿ���Ҫ����
	CT_BOOL IsItemSwap(const DataMap_IT &it, const DataMap_IT &itNext);
	// �ж����µ����Ƿ���Ҫ����
	CT_BOOL IsElmSwap(const DataMap_IT &it, const DataMap_IT &itNext);
	//���俪��
	CT_VOID OpenChest(const CT_INT32 iID,CT_INT32 iGoods[],CT_INT32 iCount[],CT_INT32 &iNum,ResAmount Chest[],FakeProBase &fb);

    // ʹ��������ϳɵ���
    CT_INT32 UseItemDesign(const ItemData &useItem);
    // ���ĵ��߻������
    CT_INT32 UseItemBuff(const ItemData &useItem);
    // ʹ����Ƭ�ϳɵ���
    CT_INT32 UseItemChip(const ItemData &useItem);
    // װ��/ж��
    CT_INT32 UseEquipment(const ItemData &useItem);
    // �򿪱���
    CT_INT32 UseItemChest(const ItemData &useItem);
    // ʹ�ûָ���Դ�ĵ���
    CT_INT32 UseItemResource(const ItemData &useItem);
	// ʹ��ħ������
	CT_INT32 UseItemEnchant(const ItemData &useItem);
    // ʹ�ù㲥����
    CT_INT32 UseItemBroadCast(const ItemData &useItem);
	//ʹ�ó��ﵰ
	CT_INT32 UseItemPetEgg(const ItemData &useItem);
	// ʹ��ͷ�ε���
	CT_INT32 UserItemTitle(const ItemData &userItem);
	// ʹ��BOSS����
	CT_INT32 UserItemBoss(const ItemData &userItem);
	// ʹ��ս�Ӹ�������
	CT_INT32 UserItemChangeFTName(const ItemData &userItem,CT_CHAR *pName);
	// ʹ��������ϳɵ���
	CT_INT32 UseItemElmPowerDesign(const ItemData &useItem);
	// ʹ����������
	CT_INT32 UseItemAttributeAdd(const ItemData &useItem);

public:
    // ��õ�������
    CT_CHAR *GetItemName(const CT_INT32 iID, CT_CHAR *szBuff, CT_INT32 nLen);
    // �����Ʒʹ������
    CT_BOOL CheckItemUsable(const G_ItemBase &itBase);
    // �����Ƿ���Ч
    CT_BOOL CheckSlotValid(const CT_INT32 iIndex);
    // �õ���������
    CT_INT32 GetItemCount(const CT_INT32 iItemID);
	//�����������ӣ��ж��û��Ƿ��Ѿ�ӵ��ĳ����,����ӵ������(ӵ��ָ��ǿ���ȼ����ϵ�ָ��װ��	��:ӵ��һ��ǿ���ȼ�300��������)
	CT_INT32 UserOwnItemGet(const CT_INT32 iID,const CT_INT32 iLevel); //iLevelװ��ǿ���ȼ�
public:
    // ���͵�����Ϣ���ͻ���
    CT_VOID SendItemData(const ItemData &itemData);
    // ͬ�����е�����Ϣ�����ݿ� 
    //CT_VOID SyncItemData(CT_INT64 currTime, CT_BOOL bForce = CT_FALSE);    
};
#endif