/********************************************************************
	created:	2012/10/18
	created:	18:10:2012   11:30
	file base:	CMasterItemEvent
	file ext:	h
	author:		xingweizhen
	
	purpose:	��������¼�����
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
    //�����û���Ϣ
    virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID, CT_CHAR* pBuf,		CT_DWORD len, CT_LONGLONG reserve);
    //�û���ʱ������
    virtual CT_VOID OnTime(CT_DWORD timeID);
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}
private:
    // �������������¼�
    CT_VOID DisposeItemEvent(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
    CT_VOID DisposeDbItemUpdate(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
    CT_VOID DisposeUsrItemUpdate(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
    CT_VOID DisposeElmPowerEvent(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
private:
    // ����ʹ�õ����¼���װ��/ж��/�ϳ�/ʹ�ã�
    CT_VOID DisposeUseItem(const CMD_G_UseItem &useItem);
    // ��������ƶ�
    CT_VOID DisposeMoveItem(const CMD_G_MoveItem &mvItem);
    // ������չ��������
    CT_VOID DisposeIncBagSlot(const CMD_G_IncBagSlot &bagSlot);
    // ����������
    CT_VOID DisposeSortBag(const CMD_G_SortBag &sortBag);
	//�����������±���
	CT_VOID DisposeElmBag(const CMD_G_SortBag &elmBag);
    // ����װ��ǿ��
    CT_VOID DisposeSmithingEquip(const CMD_G_SmithingEquip &smithingEquip);
    // ����װ��ϴ��
    CT_VOID DisposeForgeEquip(const CMD_G_ForgeEquip &forgeEquip);   
	//����װ��ϴ�������滻
	CT_VOID DisposeForgeEquipConfirm(const CMD_G_ForgeEquipConfirm &forgeEquipConfirm);  //Add by Victor 2013-1-8
    // ����װ�����
    CT_VOID DisposePunchEquip(const CMD_G_PunchEquip &punchEquip);
    // ����װ����Ƕ
    CT_VOID DisposeEmbedEquip(const CMD_G_EmbedEquip &embedEquip);
    // ����װ���̳�
    CT_VOID DisposeInheritEquip(const CMD_G_InheritEquip &inheritEquip);
    // ������۵���
    CT_VOID DisposeSellItem(const CMD_G_SellItem &sellItem);
    // ���������ȴ
    CT_VOID DisposeClearCD(const CMD_G_ClearCD &clearCD);
    // ����ħ����
    CT_VOID DisposeEnchant(const CMD_G_Enchant &enchant);
	// ����ħ��������
	CT_VOID DisposeEnchantReward(const CMD_G_EnchantReward &enchantReward);
    // ����Ԫ����������
    CT_VOID DisposeEatElm(const CMD_G_EatElm &eatElm);
    // �������ɶ��Ԫ������
    CT_VOID DisposeEatSomeElm(const CMD_G_EatSomeElm &eatSome);
    // ����Ԫ��һ����������
    CT_VOID DisposeEatAllElm(const CMD_G_EatAllElm &eatAll);
    // ����Ԫ��װ������
    CT_VOID DisposeEquipElm(const CMD_G_EquipElm &euipElm);
    // ����Ԫ�ر�����չ����
    CT_VOID DisposeIncElmSlot(const CMD_G_IncBagSlot &incSlot);
private:
    // ����õ�����Ч���¼�
    CT_VOID DisposeGetBuff(const MSC_DB_W_BuffData &itemBuff);
    // ����ʧȥ������Ч���¼�
    CT_VOID DisposeLoseBuff(const MSC_DB_LoseBuffData &itemBuff);
    // �û����߸���
    CT_VOID DisposeChangeItem(const MSC_DB_W_ItemData &itemData);
    // װ����Ϣ����
    CT_VOID DisposeChangeEquip(const MSC_DB_W_EquipData &equipData);
private:
    // ��ӵ���
    CT_VOID DisposeAddItem(const MSC_USR_AddItem &addItem);
	//�޸���������
	CT_VOID DisposeChangeMasterName(CT_CHAR *pBuf,CT_INT32 len);

};

#endif // CMASTERITEMEVENT_H