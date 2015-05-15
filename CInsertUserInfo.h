/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CUserPVPInfo.h
Version:		1.0
Description:    �û���Ϣ������
Author:			wangchen
Create Date:	2013-2-20
History:             

******************************************************************/
#ifndef CCINSERTUSERINFO_H_
#define CCINSERTUSERINFO_H_
#include "CGameMesEvent.h"

class CCUser;
class CCInsertUserInfo : public CCGameMesEvent
{
public:
	CCInsertUserInfo(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCInsertUserInfo(void);

private:
	//�����û���Ϣ  ����2������Ϊ����
	virtual CT_VOID OnUserEvent(CT_DWORD mainID, CT_DWORD subID,
		CT_CHAR* pBuf,   CT_DWORD len, 
		CT_LONGLONG reserve);
	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID)
	{

	}
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{

	}

private:
	//������Ϣ
	CT_VOID DisposeMasterInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//�õ�����������������Ϣ
	CT_VOID DisposeUpgradeBuildingInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//�õ�Ǳ������
	CT_VOID DisposeTalentInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//�õ���������
	CT_VOID DisposeSkillData(CT_CHAR *pBuf,CT_INT32 iLen);
	//�õ���������
	CT_VOID DisposeItemInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//�õ�װ����Ϣ
	CT_VOID DisposeEquipInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//�õ���������
	CT_VOID DisposeBuffInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//�õ���ȴ��Ϣ
	CT_VOID DisposeCoolDownInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//�õ�������Ϣ
	CT_VOID DisposeArmyInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//�õ����������Ľ���
	CT_VOID DisposeProducting(CT_CHAR *pBuf,CT_INT32 iLen);
	//�õ����糡������
	CT_VOID DisposeWorldScene(CT_CHAR *pBuf,CT_INT32 iLen);
	//�õ���������
	CT_VOID DisposeSceneInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//�õ��û���������
	CT_VOID DisposeRrgionInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//�õ�����Ϣ
	CT_VOID DisposeBattleArray(CT_CHAR *pBuf,CT_INT32 iLen);
	//�õ��û����������
	CT_VOID DisposeTaskFinish(CT_CHAR *pBuf,CT_INT32 iLen);
	//�õ��û��ڽ�������
	CT_VOID DisposeTaskPorgress(CT_CHAR *pBuf,CT_INT32 iLen);
	//�õ��ճ����������Ϣ
	CT_VOID DisposeDailyQuest(CT_CHAR *pBuf,CT_INT32 iLen);
	//�õ���Ծ����Ϣ
	CT_VOID DisposeDailyActive(CT_CHAR *pBuf,CT_INT32 iLen);
	//�õ�������Ϣ
	CT_VOID DisposeBuildingInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//�õ��̵�״̬
	CT_VOID DisposeMarketFunction(CT_CHAR *pBuf,CT_INT32 iLen);
	//��ȡ�ڵ���Ϣ
	CT_VOID DisposeMarketInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//�õ�������Ϣ
	CT_VOID DisposePetInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//��ȡvip��Ϣ
	CT_VOID DisposeVipInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//��������Ϣ
	CT_VOID DisposePVPInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//���а�
	CT_VOID DisposeTop(CT_CHAR *pBuf,CT_INT32 iLen);
	//ս����¼
	CT_VOID DisposePVPRecord(CT_CHAR *pBuf,CT_INT32 iLen);
	//�õ�ǩ����Ϣ
	CT_VOID DisposeSignInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//��ȡ��ħ��Ϣ
	CT_VOID DisposeEnchantdata(CT_CHAR *pBuf,CT_INT32 iLen);
	//��ȡ��ħ������Ϣ
	CT_VOID DisposeEnchantReward(CT_CHAR *pBuf,CT_INT32 iLen);
	//�õ�����ս������
	CT_VOID DisposeBuyFightingTime(CT_CHAR *pBuf,CT_INT32 iLen);
	//ɨ����Ϣ
	CT_VOID DisposeRaidsInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//ɨ������
	CT_VOID DisposeRaidsReward(CT_CHAR *pBuf,CT_INT32 iLen);
	//������Ϣ
	CT_VOID DisposeFriendInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//��Ӹ���
	CT_VOID DisposeTeamDuplicate(CT_CHAR *pBuf,CT_INT32 iLen);
	//�Զ���ȡ��Ӫ���߽���
	CT_VOID DisposePresent(CT_CHAR *pBuf,CT_INT32 iLen);
	//�����޹�
	CT_VOID DisposeMarketPersonal(CT_CHAR *pBuf,CT_INT32 iLen);
    //Ԫ��֮������
    CT_VOID DisposeElmPowerInfo(CT_CHAR *pBuf,CT_INT32 iLen);
    // ս��
    CT_VOID DisposeBattleFlagInfo(CT_CHAR *pBuf,CT_INT32 iLen);
    // Ԫ������
    CT_VOID DisposeElmAttrInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	// Ԫ����
	CT_VOID DisposeTower(CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID DisposeTowerSlotReward(CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID DisposeTowerEvent(CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID DisposeTowerSlotFighting(CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID DisposeTowerMSG(CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID DisposeTowerHelp(CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID DisposeBossRecord(CT_CHAR *pBuf,CT_INT32 iLen);
	//ս�ӿƼ�
	CT_VOID DisposeFightTeamScience(CT_CHAR *pBuf,CT_INT32 iLen);
	//����û�
	CT_VOID DisposePetChange(CT_CHAR *pBuf,CT_INT32 iLen);
	//��ȡͷ����Ϣ
	CT_VOID DisposeTitleData(CT_CHAR* pBuf, CT_INT32 iLen);
	//��ȡ������Ϣ
	CT_VOID DisposeIslandData(CT_CHAR* pBuf, CT_INT32 iLen);
	//��ȡ�����ʹ��������ID
	CT_VOID DisposeIslandGiveUid(CT_CHAR* pBuf, CT_INT32 iLen);
	//��ȡ����ȡ��ս�ӻ��ֱ���
	CT_VOID DisposeIslandScoreReward(CT_CHAR* pBuf, CT_INT32 iLen);
	//��ȡ���ʹ������Լ�������ǳ�
	CT_VOID DipsoseIslandPresentName(CT_CHAR* pBuf, CT_INT32 iLen);
	//�����ǽ�ӳ�
	CT_VOID DisposeSeekWallAddition(CT_CHAR* pBuf, CT_INT32 iLen);
	//�����ǽ����
	CT_VOID DisposeWallArmy(CT_CHAR* pBuf, CT_INT32 iLen);
	//�����̽������б�
	CT_VOID DisposeSeekRandPlayer(CT_CHAR* pBuf, CT_INT32 iLen);
	//������̽�õ��б�
	CT_VOID DisposeHadSeekedPlayer(CT_CHAR* pBuf, CT_INT32 iLen);
	//����ս������
	CT_VOID DisposeTeamFlagInsert(CT_CHAR* pBuf, CT_INT32 iLen);
	//����ʥŮ������Ϣ
	CT_VOID DisposeSaintessKidnap(CT_CHAR* pBuf, CT_INT32 iLen);
	//���빤��������Ϣ
	CT_VOID DisposeWorkerKidnap(CT_CHAR* pBuf, CT_INT32 iLen);
	//�����ʼ�Ʒ���Ա�б�
	CT_VOID DisposeWillSeekerList(CT_CHAR* pBuf, CT_INT32 iLen);
	//�����ʼ�����Ա�б�
	CT_VOID DisposeSpiteSeekerList(CT_CHAR* pBuf, CT_INT32 iLen);
	//����̽�û�������
	CT_VOID DisposeSeekDataInsert(CT_CHAR* pBuf, CT_INT32 iLen);
	//����̽�ü�¼
	CT_VOID DisposeSeekVisitorInfo(CT_CHAR* pBuf, CT_INT32 iLen);
	//����̽��ս��
	CT_VOID DisposeSeekFightRecord(CT_CHAR* pBuf, CT_INT32 iLen);
	//��������ս����Ϣ
	CT_VOID DisposeMasterFightAdd(CT_CHAR* pBuf, CT_INT32 iLen);
	//����̽�ñ��˵���Ϣ
	CT_VOID DisposeSeekOthersInfo(CT_CHAR* pBuf, CT_INT32 iLen);
	//���������Ʊ
	CT_VOID DisposePetKidnap(CT_CHAR* pBuf, CT_INT32 iLen);
};
#endif
