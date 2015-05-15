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
	//处理升级
	CT_VOID DisposeUpgradeBuilding(const CMD_G_UpgradeBuild&  ubData);
	
	//处理加速
	CT_VOID DisposeUpgradeBuildSpeedUp(const CMD_G_UpgradeBuildSpeedUp& ubsu);
	
	//处理建造
	CT_VOID DisposeUpgradeBuilding(CT_DWORD subID,   CT_CHAR* pBuf,		CT_DWORD len);
	
	//处理建造
	//CT_VOID DisposeDBBuilding(CT_DWORD subID,   CT_CHAR* pBuf, CT_DWORD len);

	//处理建造完成
	CT_VOID DisposeDBBuildingFinish(CT_DWORD subID,   CT_CHAR* pBuf, CT_DWORD len);

	//处理建造完成
	CT_VOID DisposeDBBuildingFinish(const MSC_DB_R_BuildUpgradeFinish& buildUpgradeFinish);

	//处理自动生产建筑
	CT_VOID DisposeAutoBuilding(CCUser* pUser, const MSC_DB_R_BuildUpgradeFinish& buildUpgradeFinish);

	//获取升级加速资源
	CT_VOID GetBuildSpeedUpRequest(const CMD_G_UpgradeBuildSpeedUp& ubsu);

private:
	//处理升级成功
	CT_VOID DisposeDBUpgadeSuc(CCUser* pUser,   const UpgradingBuildData& ubd);

private:
	//检测资源是否满足
	CT_BOOL CheckResForUpgrade(  CCUser* pUser, const CMD_G_UpgradeBuild& ubData, const G_BuildingUpgrade& bu);

	//发送db升级消息
	//CT_VOID SendDBUpgradeMes( CCUser* pUser, const G_Building  buildingInfo, const G_BuildingUpgrade& bu, const CMD_G_UpgradeBuild& ubData);

	//发送建造错误
	CT_VOID SendBuildError(CCUser* pUser, CT_DWORD rMain, CT_DWORD rSub, ENUM_UpgradeBuildingFail en);
};


#endif