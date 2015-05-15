/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCMasterEvent.h
Version:		1.0
Description:    �����¼�������
Author:			pengwen
Create Date:	2012-8-16
History:             

******************************************************************/
#ifndef CCMASTEREVENT_H_
#define CCMASTEREVENT_H_
#include "CGameMesEvent.h"
#include "CUserManager.h"

struct CMD_G_LevelUpgrade;
struct CMD_G_LearnTalent;
struct CMD_G_LearnSkill;
struct CMD_G_EquipSkill;
struct CMD_G_RankSalaryReq;
struct CMD_G_Charge;
struct CMD_CH_ChargeTimeout;
struct CMD_CH_ChargeSuc;
struct CMD_CH_SheetGot;
struct CMD_CH_ChargeFail;
struct CMD_CH_ReceiptInfo;

class CCMasterEvent :
	public CCGameMesEvent
{
public:
	CCMasterEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCMasterEvent(void);
private:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID, CT_CHAR* pBuf,		CT_DWORD len, CT_LONGLONG reserve);
	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID);
private:
	//���������¼�
	CT_VOID DisposeMasterEvent(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
    // �����ֵ���
    CT_VOID DisposeChargeEvent(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
    // ս��-Ԫ��֮��
    CT_VOID DisposeElmPowerEvent(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
private:
	//�������������¼�
	CT_VOID DisposeMasterUpgrade(const CMD_G_LevelUpgrade& upgrade);
    // ����ѧϰǱ���¼�
    CT_VOID DisposeLearnTalent(const CMD_G_LearnTalent& learn);
    // ������������¼�
    CT_VOID DisposeRankUpgrade(const CMD_G_LevelUpgrade& stGrade);
    // ����ѧϰ/ע�꼼���¼�
    CT_VOID DisposeLearnSkill(const CMD_G_LearnSkill &learn);
    // ����װ������
    CT_VOID DisposeEquipSkill(const CMD_G_EquipSkill &equip);
    // �������ü���
    CT_VOID DisposeResetSkill(const CMD_G_ResetSkill &reset);
    // �����������ٺ»
    CT_VOID DisposeRankSalaryReq(const CMD_G_RankSalaryReq &salary);
    // �����ֵ����
    CT_VOID DisposeChargeReq(const CMD_G_Charge &charge, const CT_INT32 sub, CT_INT32 iLen);
    // �����ֵ��ʱ
    CT_VOID DisposeChargeTimeout(const CMD_CH_ChargeTimeout &fail);
    // �������ɹ�
    CT_VOID DisposeOrderSuc(const CMD_CH_SheetGot &order, const CT_INT32 exLen);
    // �����ֵ�ɹ�
    CT_VOID DisposeChargeSuc(const CT_CHAR* pBuf, CT_INT32 iLen);
	// �����ֵʧ��
	CT_VOID DisposeChargeFail(const CMD_CH_ChargeFail &fail);
	// ����receipt��Ϣ���أ�ƻ��һ�ࣩ
	CT_VOID DisposeReceiptGot(const CMD_CH_ReceiptInfo &ri);
    //�鿴���˻�����Ϣ
	CT_VOID DisposeAnotherInfo(CT_CHAR* pBuf, CT_DWORD len);
    //ս������
    CT_VOID DisposeBattleFlagUpgrade(const CMD_G_BattleFlagUpgrade &bfu);
	//ʹ��ͷ��
	CT_VOID DisposeTiTleUse(const CMD_G_TitleUse &gtu);
	//���ʱ�õ�ͷ���б�
	CT_VOID DisposeTitleListGet(CT_CHAR* pBuf, CT_INT32 iLen);

private:
	//�鿴���˵���Ϣ
	CT_VOID DisposeCheckOtherFinish(CT_CHAR* pBuf, CT_DWORD len);
};

#endif
