#ifndef CCCITYUSERBUILDEVENT_H_
#define CCCITYUSERBUILDEVENT_H_
#include "CGameMesEvent.h"
#include "CMD_Game.h"

class  CCUser;
struct CMD_G_UpgradeBuild;
struct G_BuildingUpgrade;
struct UpgradingBuildData;
struct G_Building;
struct MSC_DB_R_BuildUpgradeFinish;

class CCCityUserBuildEvent :
	public CCGameMesEvent
{
public:
	CCCityUserBuildEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCCityUserBuildEvent(void);

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
	//��������
	CT_VOID DisposeUpgradeBuilding(const CMD_G_UpgradeBuild&  ubData);
	
	//�������
	CT_VOID DisposeUpgradeBuildSpeedUp(const CMD_G_UpgradeBuildSpeedUp& ubsu);
	
	//������
	CT_VOID DisposeUpgradeBuilding(CT_DWORD subID,   CT_CHAR* pBuf,		CT_DWORD len);
	
	//������
	//CT_VOID DisposeDBBuilding(CT_DWORD subID,   CT_CHAR* pBuf, CT_DWORD len);

	//���������
	CT_VOID DisposeDBBuildingFinish(CT_DWORD subID,   CT_CHAR* pBuf, CT_DWORD len);

	//���������
	CT_VOID DisposeDBBuildingFinish(const MSC_DB_R_BuildUpgradeFinish& buildUpgradeFinish);

	//�����Զ���������
	CT_VOID DisposeAutoBuilding(CCUser* pUser, const MSC_DB_R_BuildUpgradeFinish& buildUpgradeFinish);

	//��ȡ����������Դ
	CT_VOID GetBuildSpeedUpRequest(const CMD_G_UpgradeBuildSpeedUp& ubsu);

private:
	//���������ɹ�
	CT_VOID DisposeDBUpgadeSuc(CCUser* pUser,   const UpgradingBuildData& ubd);

private:
	//�����Դ�Ƿ�����
	CT_BOOL CheckResForUpgrade(  CCUser* pUser, const CMD_G_UpgradeBuild& ubData, const G_BuildingUpgrade& bu);

	//����db������Ϣ
	//CT_VOID SendDBUpgradeMes( CCUser* pUser, const G_Building  buildingInfo, const G_BuildingUpgrade& bu, const CMD_G_UpgradeBuild& ubData);

	//���ͽ������
	CT_VOID SendBuildError(CCUser* pUser, CT_DWORD rMain, CT_DWORD rSub, ENUM_UpgradeBuildingFail en);
};


#endif