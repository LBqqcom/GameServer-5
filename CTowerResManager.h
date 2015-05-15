/********************************************************************
	created:	2018/8/14
	created:	2018/8/14   11:15
	file base:	ProtocolGame
	file ext:	h
	author:		pengwen
	
	purpose:	元素之塔的管理器
*********************************************************************/

#ifndef CCTOWERRESMANAGER_H_
#define CCTOWERRESMANAGER_H_
#include <set>
#include <map>
#include "CSingleton.h"
#include "CMutex.h"
#include "../MessageDefined/CMD_Game.h"
#include "CThreadMes.h"

struct MSC_DB_TowerSlotReward;

class CCTowerResManager : public CCSingleton<CCTowerResManager>
{
public:
	//资源位
	struct ElementTowerSlotInfo
	{
		 ElemTowerUserInfo		userInfo;										//占领点玩家信息
		 CT_LONGLONG			llBeginTime;									//开始生产时间         
		 CT_DWORD				iHelpFriend[ELEMENT_TOWER_MAX_FRIED_HELP_NUM];	//协助朋友ID
	};

	//元素之塔的信息
	struct ElementTowerFoorInfo
	{
	    ElementTowerSlotInfo	slotInfo[ELEMENT_TOWER_MAX_POSITION_NUM];		//占领位信息
	};
public:
	CCTowerResManager(void);
	~CCTowerResManager(void);
public:
	//用户，塔的索引数据结构  
	typedef std::map<CT_DWORD, CT_INT32>      			UserTowerMap;	       //用户塔的对应， 可以知道用户在哪个塔里面，键为用户id， 值为塔id
	typedef std::map<CT_DWORD, CT_INT64>      			TowerUserMap;		   //塔用户的对应 键位为用户id， 值为用户所在塔的层数（32位）和 资源点位置（32位）
	typedef std::map<CT_INT32, TowerUserMap>  			TowerFindMap;          //元素之塔的集合 

	//塔结构， 保存生产，协助信息
	typedef std::map<CT_INT32, ElementTowerFoorInfo>	ElementTowerResMap;      //塔      键为塔的层id
	typedef std::map<CT_INT32, ElementTowerResMap>      ElementTowerResMaps;     //塔集合, 键为塔的id

	//用户所在塔的结构，主要是广播用
	typedef std::set<CT_DWORD>                          UserEnterSet;            //进入这个塔所有用户的集合
	typedef std::map<CT_INT32, UserEnterSet>            UserEnterMap;            //所有塔用户的集合

	typedef std::map<CT_INT32, MSC_TowerCheckPoint_Fighting> CPFightingMap;		// FID:Fighting
	typedef std::map<CT_INT32, CPFightingMap>			TowerFightingMap;		// CPID:(FID:Fighting)

private:
	CCMutex             								m_mutex;                //锁
	UserTowerMap										m_userTowerMap;			//用户塔查找表
	TowerFindMap										m_towerFindMap;			//塔用户查找表

	ElementTowerResMaps  								m_elementTowerResMap;   //塔资源表
	UserEnterMap                                        m_userEnterMap;         //塔中用户保存记录

	TowerFightingMap									m_towerFightingMap;

public:  //塔，用户查找，设置函数
	//用户查询加入
	CT_VOID InsertUserFind(CT_DWORD dwUserID, CT_INT32 iTowerID, CT_INT32 iFloor, CT_INT32 iSlot);
	//用户查询删除
	CT_VOID DelUserFind(CT_DWORD dwUserID, CT_INT32 iTowerID);
	//用户是否在塔中
	CT_BOOL FindUserInTower(CT_DWORD dwUserID, CT_INT32& iTowerID, CT_INT32& iFloor, CT_INT32& iSlot);
public: //用户进入记录
	//用户进入塔
	CT_VOID InsertUserEnterMap(CT_INT32 iTowerID, CT_DWORD userID);
	//用户离开塔
	CT_VOID DeleteUserEnterMap( CT_DWORD userID);
	//塔广播
	CT_VOID BroadCastInTower(CT_INT32 iTowerID,  CT_INT32 mainID, CT_INT32 subID, CT_CHAR* pBuf, CT_INT32 bufLen);


public:  //塔资源函数
	//设置用户在塔中占领数据
	CT_BOOL SetUserInTower(const ElemTowerUserInfo& userInfo, CT_INT32 iTowerID, CT_INT32 iFoor, CT_INT32 iSlot);
	//擦除用户在塔中的占领数据
	CT_VOID EaseUserInTower(CT_DWORD dwUserID, CT_INT32 iTowerID, CT_INT32 iFoor, CT_INT32 iSlot);
	//增加用户协助数据
	CT_VOID SetUserHelpInTower(CT_INT32 iTowerID, CT_INT32 iFoor, CT_INT32 iSlot, CT_DWORD dwHelpID);
	//得到资源点数据
	CT_BOOL GetSlotInTower(CT_INT32 iTowerID, CT_INT32 iFoor, CT_INT32 iSlot, CCTowerResManager::ElementTowerSlotInfo& slotInfo); 

public: //全局性资源处理
	//从全局配置恢复数据结构
	CT_VOID RecoverTowerDataFromIni();
	//从数据库中得到元素之塔数据
	CT_VOID RecoverTowerDataFromDB(CT_INT32 iFloor, CT_INT32 iSlot, CCTowerResManager::ElementTowerSlotInfo& slotInfo);
	//得到元素之塔的数据准备写往数据库
	CT_VOID GetTowerDataToDB( std::string& data);
	//重启时加载最早通关记录
	CT_VOID InitTowerFightExample(MSC_TowerCheckPoint_Fighting& fighting, CT_INT32 iFID);
	//插入最早通关记录
	CT_INT32 InsertTowerFightExample(MSC_TowerCheckPoint_Fighting& fighting);
	CT_BOOL GetTowerFightExample(CT_CHAR *pBuf, CT_INT32 &bufLen);
	CT_BOOL GetTowerFightExample(CT_INT32 iCPID, CT_INT32 iFID,MSC_TowerCheckPoint_Fighting& fighting);

public:
	//得到塔信息，登录时候发往用户
	CT_BOOL GetUserResData(CT_DWORD dwUserID, CT_CHAR* pBuf, CT_INT32& bufLen);
	//得到塔信息
	CT_BOOL GetTowerResData(CT_INT32 iTower, CT_INT32 iFloorLow, CT_INT32 iFloorHight,  CT_CHAR* pBuf, CT_INT32& bufLen);

public:
	//检查资源生产
	CT_VOID UpdateSlotProduct(CT_BOOL bForce = CT_FALSE);
	//放弃资源
	CT_VOID GiveUpSlot(CT_INT32 iTower, CT_INT32 iFloor, CT_INT32 iSlot);

public:
	//结算
	CT_VOID Harvast(  CT_INT32 iFoor, CT_INT32 iSlot, CT_DWORD dwUserID, 
					 const MSC_DB_TowerSlotReward&  towerSlotReword, CT_FLOAT rate  );

	//计算结算数据
	CT_BOOL CalHarvastRes( CT_INT32 iFoor, CT_INT32 iSlot,
							CCTowerResManager::ElementTowerSlotInfo& SlotInfo, 
							CT_INT32 iProductID, CT_LONGLONG timenow, CT_BOOL bFroce,
							MSC_DB_TowerSlotReward& towerSlotReword );

public:
	//战斗数据查询用户是否存在
	CT_BOOL FindTowerUser(const CT_DWORD dwUserID);
}; 
#endif
