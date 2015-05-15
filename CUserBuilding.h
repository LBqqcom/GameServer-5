/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCUserBuilding.h
Version:		1.0
Description:    用户建筑数据
Author:			pengwen
Create Date:	2012-9-29
History:        用户建筑数据     

******************************************************************/
#ifndef CCUSERBUILDING_H_
#define CCUSERBUILDING_H_
#include <map>
#include "CGDefine.h"
#include "CMutex.h"
#include "GlobeScriptInterface.h"

class CCUser;
struct CMD_G_BuildingData;

//正在升级建筑信息
struct UpgradingBuildData
{
	CT_INT32			iID;					//建筑id
	CT_LONGLONG			llBeginTime;			//开始升级时间
	CT_BOOL             bSpeedUpTime;           //是否加速（只有可能是0和1）
	CT_INT32            iNeedTime;              //需要的时间
	G_BuildingUpgrade   buildingUpgrade;        //升级所需数据记录
};

//建筑信息
struct BuildingData
{
	CT_INT32 iID;              //建筑id
	CT_INT32 iGrade;           //建筑等级
	CT_BOOL  bChange;          //是否改变过呢
};

//建筑生产结构
struct ProductingBuilding
{
	CT_INT32     iBType;            //建筑类型
	CT_INT32     iBid;              //建筑id
	CT_INT32     iType;				//生产类型
	CT_LONGLONG  llTime;			//生产开始时间
	CT_BOOL      bSendCanHavest;	//是否发送可以收获警告
	CT_INT32     iLastingTime;      //生产持续时间
	CT_INT32     iWorker ;          //消耗的工人
	CT_INT32     iBGrade;           //建筑等级
	CT_INT32	 iDiamod;			//消耗的钻石
	CT_FLOAT	 fSeekAddition;		//探访加成

};

//建筑生产结果统计key
struct ProductedBuildingResultKey
{
	CT_INT32            iProp1;           //获得的道具1id
	CT_INT32            iProp2;           //获得的道具2id
	CT_INT32            iBid;             //建筑id
	bool operator<(const ProductedBuildingResultKey& src) const
	{
		if (src.iBid < iBid)
		{
			return true;
		}

		if (src.iBid == iBid && src.iProp1 < iProp1)
		{
			return true;
		}

		if (src.iBid == iBid && src.iProp1== src.iProp1 && src.iProp2 < iProp2)
		{
			return true;
		}
		return false;
	}
};
//建筑生产结果统计
struct ProductedBuildingResult
{
	CT_INT32            iGold;            //获得的金币
	CT_INT32            iFood;            //获得食物
	CT_INT32			iCivil;			  //获得文明度
	CT_INT32			iDiamod;		//消耗钻石
	CT_INT32            iWorker;          //消耗的工人
	CT_INT32            iStamina;         //消耗的体力值
	CT_INT32            iProp1Num;        //获得道具1的数目
	CT_INT32            iProp2Num;        //获得道具2的数目
	//CT_INT32		iSpeedupTimes;     //加速次数
	CT_INT32			iProductingTimes;  //生产次数
};


//单一建筑升级结果统计
struct UpgradedBuildingResult
{
	CT_DWORD     dwBid;             //建筑id
	//CT_CHAR      cName[NAME_SIZE];  //建筑名称
	CT_INT32     iDiamod;           //消耗的钻石的统计
	CT_INT32     iGold ;            //消耗的金币的统计
	CT_INT32     iFood;             //消耗的食物的统计
	CT_INT32     iWorker;           //消耗的工人人次的统计
	CT_INT32     iProp1;            //消耗的道具id1
	CT_INT32     iProp1Num;         //消耗的道具1数目的统计
	CT_INT32     iProp2;            //消耗的道具id1
	CT_INT32     iProp2Num;         //消耗的道具1数目的统计
	CT_INT32     iProp3;            //消耗的道具id1
	CT_INT32     iProp3Num;         //消耗的道具1数目的统计
	CT_INT32     iProp4;            //消耗的道具id1
	CT_INT32     iProp4Num;         //消耗的道具1数目的统计
	CT_INT32     iCiviNum;          //产生的文明度的统计
	//CT_INT32     iSpeedTimes;       //钻石加速次数的统计
	CT_INT32     iXp;               //累计的经验
	CT_INT32     iUpgradeTimes;     //升级次数
};

class CCUserBuilding
{
	friend class CCUser;
public:
	typedef std::map<CT_DWORD, UpgradingBuildData>			  UpgradeingBuildDataMap; //正在更新的建筑类型
	typedef std::map<CT_DWORD, ProductingBuilding>            ProductingtManualBuildingMap;  //生产中手动建筑容器
	typedef std::map<CT_DWORD, BuildingData>                  BuildingDataMap;        //建筑表类型
	
private:
	CCUser*             m_pUser; 
	CCMutex*			m_pMutex;
	
	typedef std::map<CT_DWORD, UpgradingBuildData>::iterator  UBD_IT;
	typedef std::map<CT_DWORD, BuildingData>::iterator        BD_IT;
    typedef std::map<CT_DWORD, ProductingBuilding>            ProductingtAutoBuildingMap;    //生产中自动建筑容器
	typedef std::map<CT_DWORD, UpgradedBuildingResult>        UpgradedBuildingResultMap;     //建筑升级结果统计容器
	typedef std::map<ProductedBuildingResultKey, ProductedBuildingResult> ProductedBuildingResultMap; //建筑生产统计容器


	ProductingtManualBuildingMap    m_productingManualBuildingMap;   //手动正在生产的建筑   
	UpgradeingBuildDataMap	        m_upgradeBuildDataMap;           //升级建筑数据
	BuildingDataMap					m_buildingDataMap;               //建筑数据
	ProductingtAutoBuildingMap      m_productingAtuoBuildingMap;     //自动正在生产的建筑
	UpgradedBuildingResultMap		m_upgradedBuildingResultMap;	 //建筑升级结果统计容器
	ProductedBuildingResultMap      m_productedBuildingResultMap;    //建筑生产结果统计容器
	CT_LONGLONG                     m_llSaveBuildingDataLastTime;    //上次保存建筑数据的时间
	CT_LONGLONG                     m_llSaveProductingDataLastTime;  //上次保存生产数据的事件
public:
	CCUserBuilding();
	~CCUserBuilding(void);
	CCUserBuilding operator = (const CCUserBuilding& userBuilding)
	{ 
	}

public:
	//初始化  
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex)
	{
		m_pUser = pUser;
		m_pMutex = pMutex;
		m_productingAtuoBuildingMap.clear();
		m_productingManualBuildingMap.clear();
		m_buildingDataMap.clear();
		m_upgradeBuildDataMap.clear();
		m_upgradedBuildingResultMap.clear();
		m_productedBuildingResultMap.clear();
		m_llSaveBuildingDataLastTime = time(0);
		m_llSaveProductingDataLastTime=time(0);
	}

	//清理用户数据
	CT_VOID Clear()
	{
		_AUTOMUTEX(m_pMutex);
		m_productingAtuoBuildingMap.clear();
		m_productingManualBuildingMap.clear();
		m_buildingDataMap.clear();
		m_upgradeBuildDataMap.clear();
		m_upgradedBuildingResultMap.clear();
		m_productedBuildingResultMap.clear();
		m_llSaveBuildingDataLastTime = 0;
		m_llSaveProductingDataLastTime=0;
	}

public:
	//更新城市建筑升级(由定时器催动的更新)
	CT_VOID UpdateUpgradingBuilding( CT_DWORD dwThreadID);
	//得到建筑数据
	CT_BOOL GetBuildingData( CT_CHAR* pBuf, CT_INT32& bufLen);
	//得到升级建筑数据
	CT_BOOL GetUpgardingBuildingData(CT_CHAR* pBuf, CT_INT32& bufLen);
	//得到升级建筑数据
	CT_BOOL GetUpgardingBuildingData(int bid, UpgradingBuildData& ubd);
	//更新城市建筑升级
	CT_BOOL UpdateUpgradingBuilding(int bid, UpgradingBuildData& ubd);
	//得到正在升级的建筑
	CT_BOOL GetUpgradingBuildingData(UpgradingBuildData* pBd, CT_INT32 iLen);
	//获得建筑信息
	CT_BOOL GetBuildingData(const CT_INT32 iType,CMD_G_BuildingData &BuildData);
	//获得建筑信息
	CT_BOOL GetBuildingDataEx(const CT_INT32 iBID,CMD_G_BuildingData &BuildData);

	//检测建筑正在升级
	CT_BOOL FindUpgradeBuildData(CT_INT32 bid);
	//检测是否拥有这个建筑
	CT_BOOL FindBuildData(CT_INT32 bid, BuildingData& db);
	//检测是否拥有这个建筑
	CT_BOOL FindBuildData(CT_INT32 bid);
	//插入升级建筑数据
	CT_VOID InsertUpgradeBuildData(const UpgradingBuildData& ubd) {_AUTOMUTEX(m_pMutex);   m_upgradeBuildDataMap.insert(UpgradeingBuildDataMap::value_type(ubd.iID, ubd)); }
	//插入建筑数据
	CT_VOID InsertBuildData(const BuildingData& ud) {_AUTOMUTEX(m_pMutex);   m_buildingDataMap.insert(BuildingDataMap::value_type(ud.iID, ud));}
	//更新建筑数据
	CT_VOID UpateBuildData(const BuildingData& ud);
	//删除更新数据
	CT_VOID DeleteUpagradeBuildData(CT_DWORD bid);

public:
	//插入建筑生产
	CT_BOOL InsertBuildingProductMap(const ProductingBuilding& bp);
	//清除生产中建筑
	CT_VOID DeleteBuildingProductMap(CT_INT32 bid);

	//查找建筑生产
	CT_BOOL FindBuildingProduct(CT_INT32 bid);
	//找到正在生产建筑数据
	CT_BOOL FindBuildingProduct(CT_INT32 bid, ProductingBuilding& bp);
	//得到正在生产建筑数据
	CT_BOOL GetProductingData(CT_CHAR *pBuf, CT_INT32 &bufLen);
	//得到正在生产建筑数据
	CT_BOOL GetProductingData(ProductingBuilding *pBp,CT_INT32 ilen);
	//得到正在生产建筑数据
	CT_VOID UpdateBuildingProduct(CT_INT32 bid, ProductingBuilding& bp);
	//计算自动生产的建筑事件
	CT_VOID SysProductingBuilding();
	//更新生产中建筑事件
	CT_VOID UpdateProductingBuilding();
private:
	//检测建筑是否可以收获
	CT_VOID CheckHarvest(CT_INT64 timeNow,  ProductingBuilding& pb);

public:
	//处理自动生产
	//查找自动生产建筑
	CT_BOOL FindAutoBuilding(CT_INT32 bid);
	//查找自动生产建筑
	CT_BOOL FindAutoBuilding(CT_INT32 bid, ProductingBuilding& pb);
	//查找已经生产的自动建筑
	CT_BOOL FindAutoProductingBuilding(CT_INT32 bid);
	//更新自动生产建筑
	CT_VOID UpdateAutoBuilding(CT_INT32 bid, ProductingBuilding& pb);
	//插入自动生产建筑
	CT_VOID InsertAutoBuilding(CT_INT32 bid, ProductingBuilding& pb);
	//为任务系统增加接口，需要通过建筑类型获取数据（1，升级指定建筑到指定级别	例:将民居升级的到2级 2，拥有第N个指定建筑	例:拥有第二个民居）
	CT_INT32 GetUserBuildByType(CT_INT32 iBuildType,CT_INT32 iLevel);
	//建筑存在
	CT_BOOL BuildValidate(CT_INT32 iType);
	//生产建筑一键收获
	CT_VOID DisposeHarvestAutoOnce();
public:
	//增加升级数据统计
	CT_VOID AddUpgradedBuildingResult(const UpgradedBuildingResult& result);
	//增加生产数据统计
	CT_VOID AddProductedBuildingResult(const ProductedBuildingResultKey& key, const ProductedBuildingResult& result);
public:
	//同步建筑数据到数据库
	CT_VOID UpdateSaveBuildingData(CT_BOOL bForceBuildData, CT_BOOL bForceUpgradeBuildData,  CT_BOOL bForceResult );
	//同步建筑生产数据到数据库
	CT_VOID UpdateSaveProductingData(CT_BOOL bForceBuildProducting  ,CT_BOOL bForceResult);

	
};

#endif