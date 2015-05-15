/********************************************************************
	created:	2012/09/20
	created:	20:9:2012   15:23
	file base:	CDBMasterEvent
	file ext:	h
	author:		xingweizhen
	
	purpose:	����DB��
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
    //�����û���Ϣ
    virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID, CT_CHAR* pBuf,		CT_DWORD len, CT_LONGLONG reserve);
    //�û���ʱ������
    virtual CT_VOID OnTime(CT_DWORD timeID);
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID);

    // ������Ϣ����
    CT_VOID DisposeSaveData(CT_DWORD subID, CT_CHAR* pBuf,	CT_DWORD len);
private:
    // ����Ǳ����Ϣ
    CT_VOID DisposeTalentSaveData(const MSC_DB_W_TalentData& talentSaveData);
    // ���漼����Ϣ
    CT_VOID DisposeSkillSaveData(const MSC_DB_W_SkillData& skillSaveData);    
    // ��ӳ�ֵ��¼
    CT_VOID DisposeAddCharge(const MSC_DB_A_Charge &addCharge);
    // ���³�ֵ��¼
    CT_VOID DisposeUpdateCharge(const MSC_DB_U_Charge &updateCharge);
    // ���¸�ħ��¼
    CT_VOID DisposeUpdateEnchant(const MSC_DB_W_EnchantData &enchantData);
	// ���¸�ħʧ�ܼ�¼
	CT_VOID DisposeUpdataEnchantFail(const MSC_DB_EnchantFailTime &enchantFail);
	// ���¸�ħ������¼
	CT_VOID DisposeUpdataEnchantReward(const MSC_DB_EnchantReward &enchantReward);
    // ͬ��������Ϣ
    CT_VOID DisposeSyncItemData(const MSC_DB_SyncItemData &syncData);
    // ͬ��������Ϣ
    CT_VOID DisposeSyncBuffData(const MSC_DB_SyncBuffData &syncData);
    // ͬ��װ����Ϣ
    CT_VOID DisposeSyncEquipData(const MSC_DB_SyncEquipData &syncData);
    // ͬ��Ԫ����Ϣ
    CT_VOID DisposeSyncElmData(const MSC_DB_SyncItemData &syncData);
    // ͬ��Ԫ��������Ϣ
    CT_VOID DisposeSyncElmAttrData(const MSC_DB_SyncElmAttrData &syncData);
    // ����ս����Ϣ
    CT_VOID DisposeBattleFlagSaveData(const MSC_DB_SaveBattleFlag &saveData);
	// ����ͷ����Ϣ
	CT_VOID DisposeUpdataTitleData(const MSC_DB_TitleData &syncData);
	// ����ͷ���б�
	CT_VOID DisposeTitleListSave();
};

#endif // CCDBMASTEREVENT_H_