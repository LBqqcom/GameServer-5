/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CBuildFunEvent.h
Version:		1.0
Description:    ������н��������¼�
Author:			pengwen
Create Date:	2012-9-13
History:        ������н��������¼�     

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
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
								CT_CHAR* pBuf,		CT_DWORD len,
								CT_LONGLONG reserve);
	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}
private:
	//�������¼�
	CT_VOID DisposeBuildingEvent(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
	//����������
	CT_VOID DisposeProduct(const CMD_G_Product& product);
	//������������
	CT_VOID ProductFail(CCUser* pUser, const CMD_G_Product& product,  int pf);
	//���������ջ�
	CT_VOID DisposeHarvest(const CMD_G_RequestHavest& harvest);
	//����������
	CT_VOID DisposeProductBuildSpeeUp(const CMD_G_ProductBuildSpeedUp& bsu);
	//��ȡ������������
	CT_VOID GetProductBuildSpeedRequest(const CMD_G_ProductBuildSpeedUp& bsu);

private:
	//�������������¼�
	CT_VOID DisposeCivilUpgrade(const CMD_G_LevelUpgrade* pUpgradCivil);

private:
	//�������
	CT_VOID DisposeProduct(CCUser* pUser, const CMD_G_Product& product, int buildType);
	//����Ѩ����
	CT_VOID DisposePetCave(CCUser* pUser, const CMD_G_Product& product, int buildType);
};

#endif
