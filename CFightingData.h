/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CFightingRecord.h
Version:		1.0
Description:    战斗信息
Author:			wangchen
Create Date:	2013-10-15
History:             

******************************************************************/

#ifndef CCFIGHTINGDATA_H_
#define CCFIGHTINGDATA_H_
#include "CSingleton.h"
#include "CTType.h"
#include "GlobeScriptInterface.h"
#include "CGDefine.h"
#include "CThreadMes.h"
#include<map>

//用户战斗数据
//struct FightingData
//{
//	UserFightingStatus			FData;
//	LookUserInfo				LookData;
//	CT_INT32					iTime;			//数据存留时间
//};

typedef MSC_SUB_UserdataInsert FightingData;

typedef MSC_DB_Seeked_Info Seekers;

//当前探访的人信息
//struct Seekers
//{
//	CT_DWORD dwUserID;	// 好友ID
//	CT_INT32 iDiamond;										// 钻石
//	CT_INT32 iMoney;										// 金币
//	CT_INT32 iFood;											// 粮食
//	CT_INT32 iWorker;										// 工人
//	CT_INT32 iMaxWorker;									// 最大工人
//	CT_INT32 iKidnapWorkerNum;								// 被绑架的工人
//	CT_INT32 iKidnapSaintess;								// 圣女是否被绑架
//	CT_INT32 iPresentNum;									// 获得礼品个数
//	CT_INT32 iSeekedWillTimes;								// 被善访的次数
//	CT_INT32 iSeekedSpiteTimes;								// 被恶访的次数
//	TeamFlag TeamFlag[SEEKED_SPITE_TIMES];					// 战旗
//	BuildData			buildingData[SEEK_BUILD_NUM];		// 建筑基本信息
//	UpgradingBuildData	upgradeBuilding[SEEK_BUILD_NUM];	// 升级的建筑
//	ProductingBuilding	productBuilding[SEEK_BUILD_NUM];	// 生产的建筑
//};


class CCMutex;
class CCFightingData: public CCSingleton<CCFightingData>
{
private:
	typedef std::map<CT_DWORD,FightingData>					FightingMap;
	typedef std::map<CT_DWORD,FightingData>::iterator		FD_IT;
	typedef std::map<CT_DWORD,Seekers>						SeekersMap;

	CCMutex				m_Mutex;
	FightingMap			m_FightingDataMap;
	SeekersMap			m_SeekerMap;							// 当前探访人的信息

public:
	CCFightingData(void);
	~CCFightingData(void);

public:
	//插入数据
	CT_VOID InsertData(CT_CHAR *pBuf,CT_INT32 iLen);
	//删除数据
	CT_VOID DeleteData();

	//插入探访数居
	CT_VOID InsertSeekedInfo(MSC_DB_Seeked_Info* pInfo);

public:
	//更新数据
	CT_VOID UpdataFightingData(const CT_DWORD dwUserID);
	/* 获取数据
	 * 在线用户获取当前数据
	 * 离线用户获取已存数据
	 * 竞技场刷新，组队副本自动匹配是数据库查询,暂时不支持这里查询
	 */
	CT_BOOL GetFightingData(const CT_DWORD dwUserID,FightingData &fd);
	CT_BOOL GetFightingData(const CT_DWORD dwUserID,UserFightingStatus &fd,CT_BOOL bSeekFight=CT_FALSE);
	//删除boss
	CT_VOID DeleteBoss(const CT_DWORD dwUserID);

	//得到探访者数据(在线数据部分填充)
	CT_BOOL GetSeekedInfo(CT_DWORD dwUserID, Seekers& seekersInfo);
	//更新探访者次数
	CT_BOOL UpdateSeekedTimes(CT_INT32 iType, const Seekers& seekersInfo); 
	//验证建筑ID
	CT_BOOL GetBuildData(const CT_DWORD dwUserID,const CT_INT32 iID,BuildData &bd);
	//更新探访人物信息到数据库(不包括建筑,旗帜，礼物)
	CT_VOID UpdateSeekersBaseInfo(const CT_DWORD dwUserID,Seekers &seekers);
	//得到已获得的礼物个数
	CT_INT32 GetPresentNum(CT_DWORD dwUserID);
	//得到礼物
	CT_VOID GetPresent(CT_DWORD dwUserID, CT_INT32 iChestID);
	//得到正在生产的建筑信息
	CT_BOOL GetBuildingProduct(CT_DWORD dwUserID, CT_INT32 iBID, ProductingBuilding& proBuilding);
	//更新正在生产的建筑信息
	CT_VOID UpdateBuildingProduct(CT_DWORD dwUserID, CT_INT32 iBID, ProductingBuilding& proBuilding);
	//得到正在升级建筑
	CT_BOOL GetUpgradeBuilding(CT_DWORD dwUserID, CT_INT32 iBID, UpgradingBuildData& upgBuilding);
	//更新正在升级建筑
	CT_VOID UpdateUpgradeBuilding(CT_DWORD dwUserID, CT_INT32 iBID, UpgradingBuildData& upgBuilding);
	//插入战旗信息
	CT_VOID InsertTeamFlag(CT_DWORD dwUserID, TeamSeek& ts);
	//删除战旗
	CT_VOID DeleteTeamFlag(CT_DWORD dwUserID);
	//更新礼物信息
	CT_VOID UpdateSeekPresent(CT_DWORD dwUserID, SeekPresent& sp);

public:
	//竞技场连胜修改
	CT_VOID SetPVPConWin(CT_DWORD dwUserID);

private:
	//获取领主信息
	CT_BOOL GetLookUserInfo(PTRLinkUserData &linkUserData,UserFightingStatus &Fightinginfo,LookUserInfo &Lookinfo);
};
#endif
