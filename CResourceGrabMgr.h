/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCResourceGrabMgr.h
Version:		1.0
Description:    资源争夺系统
Author:			LinQ
Create Date:	2013-05-04
History:        资源争夺系统，管理器

******************************************************************/
#ifndef _CRESOURCE_GRAB_H_
#define _CRESOURCE_GRAB_H_

#include "CTType.h"
#include "GlobeScriptInterface.h"
#include "CSingleton.h"
#include <map>
#include <set>
#include "../sdk/include/CApi.h"
#include "../sdk/include/GlobeScriptInterface.h"
#include "CGDefine.h"
#include "../sdk/include/CMutex.h"
#include "CThreadMes.h"

struct MSC_DB_ResourceProduct;
struct CMD_G_ResSpotApply;
class CCUser;
class CCFightTeam;

//资源生产
struct ResourceProduct
{
    CT_DWORD        dwUserID;         //用户id
	CT_INT32        iGrade;           //用户等级
	CT_INT32		iProductCount;    //已经生产的次数
	CT_INT32        iLeftProductTime; //本次生产剩余时间
	ENUM_RES_SPOT_STATUS iStatus;      //资源点状态
};


//资源标识
struct ResSpot
{
	CT_INT32		 iSpotID;							  //资源点ID
	CT_INT32		 iSceneID;							  //资源场景ID
};


//资源占领集合
struct ResSpotTag
{
	ResSpot          resSpot[SPOT_RES_SIZE];             //战队占领的资源集合
};

//驻守人员
struct GuardTag
{
	CT_DWORD         dwUserID[SPOT_GUARD_MEMBER];        //驻守人员
};

//资源点基本信息
struct ResourceBase
{
	CT_DWORD		 dwFTID;								  //战队id
	CT_CHAR			 szNote[SPOT_VECTORY_SIGN_SIZE];	  //胜利留言
	CT_INT32		 iLeftProtectTime;					  //占领保护时间
	CT_INT32         iLeftOccupyTime;					  //剩余占领时间
	CT_BOOL          bSendProtectMes;                     //是否发送过保护过期通知
};

// 资源点类
struct ResourceSpot
{
	ResourceBase     resBase;                             //资源点信息
	GuardTag         guardTag;                            //防守人员
	ResourceProduct  resProduct[CLAN_MAX_MEMBER];		  //生产信息
};

//活动资源点
struct ResActive
{
	CT_INT32		iSceneID;
	CT_INT32		iSpot;
	CT_INT32		iResEffect;				//矿点效果
	CT_INT32		iProductType;			//产出类型
	CT_INT32		iResID;					//资源id
	CT_INT32		iProductNum;			//产出数量
	CT_INT32		iProductTime;			//产出时间
	CT_INT32		iProtectTime;			//保护时间
	CT_INT32		iFocusOccupyACTVType;	//强攻消耗类型
	CT_INT32		iFocusOccupyACTVNum;	//强攻消耗数目
	CT_FLOAT		fProductMultiple;		//产出倍数
};



// 资源争夺
class CCResourceGrabMgr : public CCSingleton<CCResourceGrabMgr>
{
private:
	//资源标记
	typedef std::map<CT_INT32,    ResourceSpot>			SpotMap;		//资源点集合 key为资源点id
	typedef std::map<CT_INT32,    SpotMap>				SenceMap;		//场景集合 key为场景id
	typedef std::multimap<CT_DWORD,    CT_LONGLONG>     ResSpotFindMap; //战队，资源点查找表  key战队的id
	typedef std::set<CT_DWORD>                          SenceUserSet;   //存放场景中用户集合
	typedef std::map<CT_INT32,   SenceUserSet>          SenceUserMap;   //场景用户集合
	typedef std::map<CT_INT64,ResActive>				ResActiveMap;	//资源点活动

	SenceMap		m_mapSpots;					// 所有资源点，场景ID资源点ID合成为key，value是ResourceSpot对象指针
	CCMutex			m_mutex;                    // 锁
	ResSpotFindMap  m_mapResSpotFind;           //战队，资源点查找表  
	SenceUserMap    m_mapSenceUser;             //场景进入用户的集合

	CT_BOOL			m_bResActive;				//资源的活动
	ResActiveMap    m_ResActiveMap;				//资源点活动
	CT_INT64		m_lLastTiem;				//剩余时间
	std::set<CT_INT32>		m_SceneList;		//起用的场景列表

public:
	CCResourceGrabMgr(void);
	~CCResourceGrabMgr(void);

private:
	//从配置中读取所有的资源点恢复到内存
	CT_VOID ProduceResourceSpots(CT_VOID);
	//得到资源点区域数据
	CT_BOOL GetResourceArea(CT_INT32 iScenceID, G_ResourceArea& resArea);
	//保存用户金币数据
	CT_VOID SaveUserProduct(CT_DWORD fid,  CT_DWORD uid, CT_INT32 iRes,CT_INT32 iResItem,CT_INT32 iResType,CT_INT32 iSCID, CT_INT32 iSpot, ENUM_RES_Propduct_Reason iReason );
	//得到用户资源信息
	CT_VOID GetResourceMes(CT_DWORD dwUserID, const ResourceSpot& resSpot,const CT_INT32 iResID,const CT_INT32 iScenceID,CMD_G_ResSpotInfo& rsi);
public:
	//从数据库中找回资源点的信息；恢复到资源掠夺管理器
	CT_VOID RecoverResourceSpot(CT_INT32 iScenceID, CT_INT32 iSpotID, ResourceSpot&);
	//用户进入场景信息
	CT_VOID EnterSence(CT_INT32 iSenceID, CCUser* pUser);
	//同步生产信息
	CT_VOID SysResourceProduct(const MSC_DB_ResourceProduct& rp);
	//得到资源点生产数据
	CT_BOOL GetResourceQuality(CT_INT32 iScenceID, CT_INT32 iSpotID, G_ResourceQuality& resQuality,CT_INT32 &iResType);
	//计算资源点生产
	CT_INT32 GetResourceSpot(CT_INT32 iScenceID,CT_INT32 iSpot,G_ResourceQuality& resQuality,CT_INT32 iGrade);
	//产量控制
	CT_FLOAT GetProductionLimit(CT_INT32 iSceneID,CT_INT32 iGrade);

	CT_VOID SetResourceScene(CT_INT32 iSceneID);
	CT_BOOL GetResourceScene(CT_CHAR *pBuf, CT_INT32 &bufLen);

private:
	//增加资源到战队
	CT_VOID AddSpotToFTeam(CT_INT32 iScenceID, CT_INT32 iSpot, CT_DWORD fTeamID);
	//删除资源到战队
	CT_VOID EaseSpotFromFTeam(CT_INT32 iScenceID, CT_INT32 iSpot, CT_DWORD fTeamID);
public:
	//用户退出
	CT_VOID UserExit(CT_DWORD dwUserID);

public:
	//战队得到拥有的资源点
	CT_INT32 TeamFindResSpot(CT_DWORD teamID, ResSpotTag& rst);
	//清除生产信息 用户被首领踢出队伍时候调用
	CT_VOID  TeamKickOutUser(CT_DWORD teamID, CT_DWORD uid);
	//战队解散
	CT_VOID  TeamDissolution(CT_DWORD teamID);

public:
	//战队的得到资源点上守卫
	CT_BOOL TeamFindResSpotGuard(CT_INT32 iSenceID, CT_INT32 iSpot, GuardTag& gt);
	//设置战队在资源点上的守卫
	CT_BOOL TeamSetResSpotGuard(CCFightTeam* pFightTeam, CT_INT32 iSenceID, CT_INT32 iSpot, GuardTag& gt);

public:
	//清空资源点信息
	CT_VOID  ResetResourceInfo(CT_INT32 iSenceID, CT_INT32 iSpot);
	//得到资源基础信息
	CT_BOOL  GetResourceBase(CT_INT32 iSenceID, CT_INT32 iSpot, ResourceBase& protecTime);
	//设置资源基本信息
	CT_BOOL  SetResourceBase(CT_INT32 iSenceID, CT_INT32 iSpot, const ResourceBase& protecTime);

	//得到目前战队占领的资源点数量
	CT_INT32 GetResSpotNum(CT_DWORD iFightTeamID);
	//得到目前的资源点上该用户生产状态
	CT_BOOL  GetResProduct(CT_DWORD dwUserID, CT_INT32 iSenceID, CT_INT32 iSpot,ResourceProduct& rp);

	//是否已经被占领
	CT_BOOL  DoResSpotOccupy(CT_DWORD iFightTeamID, CT_INT32 iSenceID, CT_INT32 iSpot);
	//资源点生产
	CT_BOOL  SpotProduct(CCUser* pUser, CT_INT32 iScenceID, CT_INT32 iSpotID, CT_DWORD dwTeamID);
	
	//资源抢夺
	CT_INT32 ResSpotOccupy(CT_INT32 iSenceID, CT_INT32 iSpot, CT_DWORD iTeamID);

public:
	//查找资源点
	CT_BOOL FindSpot(CT_INT32 iScenceID, CT_INT32 iSpotID);
	//用户退出场景
	CT_VOID UserExitScence(CT_INT32 iScenceID, CT_DWORD dwUserID);
	//战队放弃资源点
	CT_VOID TeamGiveUpSpot(const CMD_G_ResSpotApply& rsa);
	//用户生产
	CT_VOID UserHavestSpotRes(CCFightTeam* pFightTeam, CCUser* pUser, const CMD_G_ResSpotApply& rsa);
	CT_VOID UserHavestSpotRes(CCFightTeam* pFightTeam, CT_DWORD dwUserID, const CMD_G_ResSpotApply& rsa);
public:
	//场景广播
	CT_VOID BroadcastSence(CT_INT32 iScenceID, CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_INT32 bufLen);

public:
	//更新生产信息
	CT_VOID UpdateProduct();
	//得到全部的保存信息
	CT_VOID GetAllInfoToSave(std::string& info);
	//得到用户资源数据
	CT_BOOL GetUserResData(CT_DWORD dwUserID, CT_CHAR* buf, CT_INT32& bufLen);

private:
	//生产检测
	CT_VOID CheckProduct(CT_INT32 iSceneID, CT_INT32 iSpotID, const ResourceBase& resBase, ResourceProduct& rp);
	//查找生产位置
	CT_BOOL FindProductPos(const ResourceProduct* pResProduct, CT_DWORD uid, CT_INT32& pos);
	//计算修正后的金币
	CT_INT32 CalFightTeamType(CCFightTeam* pFightTeam, CT_INT32 iRes, CT_DWORD dwUserID);
public:
	//广播驻守信息
	CT_VOID BroadcastSenceGuard(CT_INT32 iScenceID, CT_INT32 iSpotID, CCFightTeam* pFightTeam,  CT_CHAR pVictorySign[SPOT_VECTORY_SIGN_SIZE], CT_DWORD pDependPlayer[SPOT_GUARD_MEMBER]);

public:
	//设置活动
	CT_VOID SetResActive(CT_BOOL bResActive);
	//获取活动
	CT_BOOL GetResActive(){_AUTOMUTEX(&m_mutex);return m_bResActive;};
	//插入活动资源点
	CT_VOID InsterResActive(CT_CHAR *pBuf,CT_INT32 iLen);
	//获取资源点
	CT_BOOL GetResActiveSpot(CT_CHAR *pBuf,CT_INT32 &iLen);
	//活动强制收获、
	CT_VOID ResActiveHavest();
	//设置剩余时间
	CT_VOID SetLastTime(CT_INT64 lLastTime){_AUTOMUTEX(&m_mutex);m_lLastTiem=lLastTime;};
	//获取剩余时间
	CT_INT64 GetLastTime(){_AUTOMUTEX(&m_mutex);return m_lLastTiem;};
	//获取活动资源点
	CT_BOOL GetActiveResSpot(CT_INT32 iScenceID,CT_INT32 iSpotID);
	//获取活动资源点
	CT_BOOL GetActiveResSpot(CT_INT32 iScenceID,CT_INT32 iSpotID,ResActive &ra);
	//随机生产类型
	CT_VOID RandProductType();
	//清理点信息
	CT_VOID ClearSpot(){_AUTOMUTEX(&m_mutex);m_ResActiveMap.clear();};
};

#endif