/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCMasterEvent.h
Version:		1.0
Description:    领主事件处理器
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
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID, CT_CHAR* pBuf,		CT_DWORD len, CT_LONGLONG reserve);
	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID);
private:
	//处理领主事件
	CT_VOID DisposeMasterEvent(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
    // 处理充值结果
    CT_VOID DisposeChargeEvent(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
    // 战旗-元素之力
    CT_VOID DisposeElmPowerEvent(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
private:
	//处理领主升级事件
	CT_VOID DisposeMasterUpgrade(const CMD_G_LevelUpgrade& upgrade);
    // 处理学习潜能事件
    CT_VOID DisposeLearnTalent(const CMD_G_LearnTalent& learn);
    // 处理军衔提升事件
    CT_VOID DisposeRankUpgrade(const CMD_G_LevelUpgrade& stGrade);
    // 处理学习/注魂技能事件
    CT_VOID DisposeLearnSkill(const CMD_G_LearnSkill &learn);
    // 处理装备技能
    CT_VOID DisposeEquipSkill(const CMD_G_EquipSkill &equip);
    // 处理重置技能
    CT_VOID DisposeResetSkill(const CMD_G_ResetSkill &reset);
    // 处理请求军衔俸禄
    CT_VOID DisposeRankSalaryReq(const CMD_G_RankSalaryReq &salary);
    // 处理充值请求
    CT_VOID DisposeChargeReq(const CMD_G_Charge &charge, const CT_INT32 sub, CT_INT32 iLen);
    // 处理充值超时
    CT_VOID DisposeChargeTimeout(const CMD_CH_ChargeTimeout &fail);
    // 处理订单成功
    CT_VOID DisposeOrderSuc(const CMD_CH_SheetGot &order, const CT_INT32 exLen);
    // 处理充值成功
    CT_VOID DisposeChargeSuc(const CT_CHAR* pBuf, CT_INT32 iLen);
	// 处理充值失败
	CT_VOID DisposeChargeFail(const CMD_CH_ChargeFail &fail);
	// 处理receipt信息返回（苹果一类）
	CT_VOID DisposeReceiptGot(const CMD_CH_ReceiptInfo &ri);
    //查看他人基本信息
	CT_VOID DisposeAnotherInfo(CT_CHAR* pBuf, CT_DWORD len);
    //战旗升级
    CT_VOID DisposeBattleFlagUpgrade(const CMD_G_BattleFlagUpgrade &bfu);
	//使用头衔
	CT_VOID DisposeTiTleUse(const CMD_G_TitleUse &gtu);
	//起服时得到头衔列表
	CT_VOID DisposeTitleListGet(CT_CHAR* pBuf, CT_INT32 iLen);

private:
	//查看别人的信息
	CT_VOID DisposeCheckOtherFinish(CT_CHAR* pBuf, CT_DWORD len);
};

#endif
