/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CBuildFunEvent.h
Version:		1.0
Description:    处理城市建筑功能事件
Author:			pengwen
Create Date:	2012-9-13
History:        处理城市建筑功能事件     

******************************************************************/

#ifndef CCBUILDFUNEVENT_H_
#define CCBUILDFUNEVENT_H_
#include "CGameMesEvent.h"

struct CMD_G_LevelUpgrade;
struct CMD_G_Product;
class  CCUser;
struct CMD_G_ProductHarvest;
struct CMD_G_ProductBuildSpeedUp;
struct CMD_G_RequestHavest;
class CCBuildFunEvent :
	public CCGameMesEvent
{
public:
	CCBuildFunEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCBuildFunEvent(void);

private:
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
								CT_CHAR* pBuf,		CT_DWORD len,
								CT_LONGLONG reserve);
	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}
private:
	//处理建筑事件
	CT_VOID DisposeBuildingEvent(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
	//处理建筑生产
	CT_VOID DisposeProduct(const CMD_G_Product& product);
	//处理生产错误
	CT_VOID ProductFail(CCUser* pUser, const CMD_G_Product& product,  int pf);
	//处理请求收获
	CT_VOID DisposeHarvest(const CMD_G_RequestHavest& harvest);
	//处理建筑加速
	CT_VOID DisposeProductBuildSpeeUp(const CMD_G_ProductBuildSpeedUp& bsu);
	//获取生产加速消耗
	CT_VOID GetProductBuildSpeedRequest(const CMD_G_ProductBuildSpeedUp& bsu);

private:
	//处理文明升级事件
	CT_VOID DisposeCivilUpgrade(const CMD_G_LevelUpgrade* pUpgradCivil);

private:
	//民居生产
	CT_VOID DisposeProduct(CCUser* pUser, const CMD_G_Product& product, int buildType);
	//处理洞穴生产
	CT_VOID DisposePetCave(CCUser* pUser, const CMD_G_Product& product, int buildType);
};

#endif
