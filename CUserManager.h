/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CUserManager.h
Version:		1.0
Description:    用户服务
Author:			pengwen
Create Date:	2012-8-8
History:        管理用户     

******************************************************************/
#ifndef CCUSERMANAGER_H_
#define CCUSERMANAGER_H_
#include <map>
#include <string>
#include "CNetInterface.h"
#include "CSingleton.h"
#include "CUser.h"
#include "CMD_Game.h"
#include "CMutex.h"

//用户管理类
class CCUserManager : public CCSingleton<CCUserManager>
{
	
public:
	typedef std::map<CT_DWORD, CCUser*>				UserDataMap;
	typedef std::map<CT_DWORD, CCUser*>::iterator	UDM_IT;

	typedef std::map<CCNetInterface*, CT_DWORD>      UserIDFindMap;
	typedef std::map<CCNetInterface*, CT_DWORD>::iterator UFM_IT;

	typedef std::map<std::string, CCUser*>				UserDataNameMap;
	typedef std::map<std::string, CCUser*>::iterator	UDNM_IT;
private:
	//用户信息表
	UserDataMap				m_userDataMap;
	//用户id查找表
	UserIDFindMap			m_userFindMap;
	//用户昵称表
	UserDataNameMap			m_userDataNameMap;
	//互斥锁
	CCMutex					m_mutex;  
	CT_CHAR* m_pBuffer;
	std::set<CT_DWORD>		m_GMList;

public:
	CCUserManager(CT_VOID);
	~CCUserManager(CT_VOID);

public:
	//得到一个用户数据
	CCUser* GetOneFreeUser(CT_DWORD userID, CCNetInterface* pNetInterface);
public:
	//设置通讯句柄
	CT_BOOL SetNetHandle(CT_DWORD userID, CCNetInterface* pNetHandle);
	//查找数据 任何人不得改动这个函数的实现（会产生死锁问题）彭文
	CT_BOOL Find(CT_DWORD id);
	//查找数据 任何人不得改动这个函数的实现（会产生死锁问题）彭文
	CT_BOOL Find(CCNetInterface* pNetHandle);
	//查找数据 任何人不得改动这个函数的实现（会产生死锁问题）彭文
	CT_BOOL Find(CT_DWORD id, CCUser** p);
	//查找数据 任何人不得改动这个函数的实现（会产生死锁问题）彭文
	CT_BOOL Find(CCNetInterface* pNetHandle, CCUser**  us);
	//通过昵称查找 任何人不得改动这个函数的实现（会产生死锁问题）彭文
	CT_BOOL Find(std::string cNickName, CCUser** us);
	//清除数据
	CT_VOID Erase(CT_DWORD id);
	//清空
	CT_VOID Clear();
	//获取用户昵称idmap
	CT_VOID InsertNickNameMap(const std::string& sNickName,CCUser *pUser){_AUTOMUTEX(&m_mutex); m_userDataNameMap.insert(UserDataNameMap::value_type(sNickName,pUser));};
	//获取用户数目
	CT_INT32 GetUserCount(){return m_userDataMap.size();};

public:
	//更新升级的建筑
	CT_VOID UpdateUpgradingBuilding(CT_INT32 threadIndex, CT_INT32 threadTotalNum, CT_DWORD dwThreadID);
	//同步建筑数据到数据库
	CT_VOID UpdateSaveBuildingData(CT_INT32 threadIndex, CT_INT32 threadTotalNum,  CT_DWORD dwThreadID);
	//同步生产数据到数据库
	CT_VOID UpdateSaveProductingData(CT_INT32 threadIndex, CT_INT32 threadTotalNum,  CT_DWORD dwThreadID);

	//更新生产的建筑
	CT_VOID UpdateProductingBuilding(CT_INT32 threadIndex, CT_INT32 threadTotalNum);
	//更新事件
	CT_VOID UpdateEvent(CT_INT32 threadIndex, CT_INT32 threadTotalNum);
    // 更新领主增益 [10/10/2012 John]
    CT_VOID UpdateItemBuff();
    // 更新领主冷却[10/10/2012 John]
    CT_VOID UpdateCooldown();
    // 同步领主道具信息
    CT_VOID SyncItemData();
	// 发送世界聊天消息
	CT_VOID SendWorldChat(CMD_G_WorldChat *pWorldChat,CT_DWORD len);
	// 发送广播消息
	CT_VOID BroadCastWorldMessage(CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID BroadCastMessage(CT_DWORD mainID, CT_DWORD subID,CT_CHAR *pBuf,CT_INT32 iLen);
	//发送GM消息
	CT_VOID SendGMMessage(CT_DWORD mid, CT_DWORD sid,  CT_CHAR* pMes,CT_INT32 len,CT_DWORD dwMeUserID=0);
	//系统消息
	CT_VOID SendServerMessage(CMD_G_GMMessage &pMes,CT_INT32 len,CT_DWORD dwMeUserID=0);
    // 刷新签到信息
    CT_VOID UpdateSignIn(CT_INT32 threadIndex, CT_INT32 threadTotalNum);
	//刷新商店单个限量物品信息
	CT_VOID UpdateSingleGoodsStatus(CMD_G_MarketGoodsInfo &pGoodsInfo);
	//更新心跳
	CT_VOID UpdateHeartBeat();
	//更新竞技度
	CT_VOID UpdataPVPDegree();
	//定时刷新商店，商城商品
	CT_VOID RefreshMarketGoods();
	//定时刷新黑店商品
	CT_VOID RefreshBlackMarketGoods();
    // 发放军衔俸禄
    CT_VOID PushRankSalary();
    // 恢复体力值
    CT_VOID RecoverStamina();
	//更新副本
	CT_VOID RefreshDuplicate();
	//更新竞技场
	CT_VOID RefreshPVP();
	//判断vip时间
	CT_VOID VipTimeMonitor();
	//礼包推送
	CT_VOID VipGiftPush();
	//获取在线人数
	CT_VOID GetOnlineUserNum();
	//活动删除推送
	CT_VOID ActiveDeletePush(MSC_DB_ActiveEventChange &activeDel);
	//活动增加推送
	CT_VOID	ActiveAddPush(MSC_DB_ActiveEventChange &activeAdd);
	//更新祭祀
	CT_VOID RefreshWorship();
	//获取部分在线玩家列表(用来推送加好友,加战队队员) iType: 1.好友 2.战队队员
	CT_VOID GetOnlineUserData(CT_DWORD UserID, CT_CHAR *out_buf, CT_INT32 &bufLen, CT_INT32 iType=0);
	//更新在线时奖励活动
	CT_VOID UpdateActiveOnline();
	//更新转盘
	CT_VOID RefreshTurntable();
	//更新元素塔通关
	CT_VOID RefreshTowerPass();
	//更新组队副本
	CT_VOID UpdataTeamFightingTimes();
	//更新购买体力
	CT_VOID UpdataBuyStaminaTimes();
	//广播所有在在线用户
	CT_VOID SendUserMessage(CT_DWORD dwMainID,CT_DWORD dwSubID,CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID SendUserMessage(CT_DWORD dwMainID,CT_DWORD dwSubID);
	//更新个人限购
	CT_VOID UpdataMarketParsonal(CT_CHAR *pBuf,CT_INT32 iLen);
	//更新所有在线用户的公告
	CT_VOID UpdataUserAnnouncement(CT_CHAR *pBuf,CT_INT32 iLen);
	//更新龙珠状态
	CT_VOID	UpdataDragonBallStatus();
	//更新头衔状态
	CT_VOID UpdataTitleStatus();
	//更新月卡活动状态
	CT_VOID UpdataMonthCardStatus();
	//添加GM ID
	CT_VOID InsertGMList(CT_DWORD dwUID);
	//判断 is GM
	CT_BOOL IsGM(CT_DWORD dwUID);
	CT_VOID ClearGMList();
	//查找竞技场用户是否引用
	CT_BOOL FindPVPUser(const CT_DWORD dwUserID);

	//全服Buff奖励
	CT_VOID BuffReward(CT_INT32 iID);

	CT_VOID RefreshActiveStatus(CT_INT32 iAEID, CT_INT32 iStatus);

	//跨服战状态信息
	CT_VOID SendCSStatusInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//跨服战奖励信息
	CT_VOID SendCSRewardInfo();
	//跨服战全服奖励
	CT_VOID SendCSFullServerReward(G_FullServiceReward &fsr,CT_CHAR *pBuf,CT_INT32 iLen);

	//更新头衔列表
	CT_VOID UpdateTitleList();
	//更新探访礼物
	CT_VOID UpdateSeekPresent();

	//判断用户是否在访问列表
	CT_BOOL IsInSeekedList(CT_DWORD dwUserID);

	//更新探访数据
	CT_VOID UpdateSeekData();

	// 更新排行榜奖励状态
	CT_VOID UpdateTopReward();

public:
	//更新数据库
	CT_VOID DBUpdataData();
	
public:
	//清退所有用户
	CT_VOID  ExitAllUser();
	
};


#endif