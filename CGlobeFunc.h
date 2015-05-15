/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CGlobeFun.h
Version:		1.0
Description:    全局函数
Author:			pengwen
Create Date:	2012-8-20
History:        处理用户事件     

******************************************************************/
#ifndef CGLOBEFUNC_H_
#define CGLOBEFUNC_H_
#include <cstdlib>
#include "CTType.h"
#include "CGlobeData.h"
#include "CGDefine.h"
#include "GlobeScriptInterface.h"
#include<map>

class CCDBConnector;
class CCNetInterface;
class CCGameMesManager;
class CCUser;
class CCDBResult;
struct CMD_G_NormalReward;
struct Resoure; 
struct Item;

extern CCNetInterface*  g_pPCentServer;				//中心服务网络接口
extern CCNetInterface*  g_pPChargeServer;           // 充值服务网络接口
extern CCNetInterface*  g_pCorssServer;				//跨服服务
extern CT_INT32		    g_iServerID;				//服务ID
extern CT_INT64			g_lHeartBeat;				//心跳时间
extern CT_INT64			g_lOpenServerTime;			//服务开启时间
extern CT_INT32			g_iDBUpdataValue;			//数据库更新阀值
extern CT_INT32			g_iGlobeData[GLOBEDATA_TYPE_MAX];	//全局数据0:红米 1:米3

//数据库读取写入记录
struct ReadCount
{
	CT_INT64		lId;
	CT_INT32		iMainID;
	CT_INT32		iSubID;
	CT_INT32		iCount;	
};

//奖励消耗
struct RewardCost
{
	CT_INT32			iUseResType[TEAM_RES_CNT];			//使用资源类型
	CT_INT32			iUseResCount[TEAM_RES_CNT];			//使用资源数目
	CT_INT32			iCurrentResCount[TEAM_RES_CNT];		//当前资源数目
	CT_INT32			iUseItem[GOODS_COUNT*2];			//使用资源类型
	CT_INT32			iUseItemCount[GOODS_COUNT*2];			//使用资源数目
};

typedef std::map<CT_INT64,ReadCount>				ReadCountMap;
typedef std::map<CT_INT64,ReadCount>::iterator		rc_it;
extern ReadCountMap g_ReadCountMap;

struct UserFightingStatus;
struct LookUserInfo;
struct MasterVariableData;
struct CombatUnitData;
struct MSC_DB_MasterSaveData;
struct PTRLinkUserData;
struct MasterCombatStatus;
struct MSC_DB_Seeked_Info;

//消息处理错误记录
CT_VOID G_DisposeErrorMes(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pErrorScr);

//发送线程消息
CT_VOID G_SendThreadInfo(CT_DWORD threadID,  CT_DWORD mainID, CT_DWORD subID, const CT_CHAR* pBuf, CT_INT32 len);

//处理db错误
CT_VOID G_DBError(CCDBConnector& dbConnector, CT_DWORD dwThread);

//发送db通用消息
CT_VOID G_SendThreadDbStandardInfo  (CT_DWORD threadID, CT_DWORD mainID, CT_DWORD subID,  CT_DWORD dwUser, const CCNetInterface* pNetInter);

//发送建筑通用消息
CT_VOID G_SendThreadDBBuildingInfo  (CT_DWORD threadID, CT_DWORD mainID, CT_DWORD subID,  CT_DWORD dwUser, CT_INT32 iBuildID, const CCNetInterface* pNetInter);

//保存领主货币资源数据
CT_VOID G_SaveMastVariableData(CCUser* pUser, ENUM_BaseDataSaveTag saveTag);
//离线用户数据保存
CT_VOID G_SaveMastvariableData(MSC_DB_MasterSaveData &msd);

//获取军队
CT_INT32 G_GetArmyID(CT_INT32 iArmyID,CT_INT32 iGrade);

//获取体力购买消费
CT_INT32 G_GetStatminaCost(CCUser *pUser);

//体力消费记录
CT_BOOL G_SaveStatminaCost(CCUser *pUser);

// 字符时间格式转换为秒数
CT_INT64 G_StrTimeToSeconds(CT_CHAR *strTime);

//time转时间格式
CT_VOID G_SecondsToTime(CT_CHAR *strTime,CT_INT64 lTime);

//
CT_VOID G_ReadCount();

//副本关卡KEY
CT_INT32 G_GetKeyValue(int value0, int value1, int value2);

//获取为概率数据
CT_VOID G_GetFakeProDta(CCUser *pUser,CT_INT32 iTage,FakeProBase &fb);
//清理为概率数据（道具抽取后执行此函数）
CT_VOID G_ClearFakeProDat(CCUser *pUser,CT_INT32 iTage,FakeProBase &fb);
//为概率计算
CT_INT32 G_GetFakeProCount(CT_INT32 iID,FakeProBase &fb);
//道具组抽取
CT_INT32 G_GetGoodsGroupInfo(const CT_INT32 iGroupID,CT_INT32 iGoods[GOODS_COUNT],CT_INT32 iCount[GOODS_COUNT],FakeProBase &fb);


//获取玩家战斗属性
CT_VOID G_GetFightingData(CCUser *pUser,UserFightingStatus &UserData,CT_BOOL bSeekFight=CT_FALSE);
//获取领主战斗属性
CT_VOID G_GetMasterFightingData(const UserFightingStatus &data,CombatUnitData &masterdata);
//获取领主信息
CT_BOOL G_GetLookUserInfo(PTRLinkUserData &linkUserData,UserFightingStatus &Fightinginfo,LookUserInfo &Lookinfo,CT_BOOL bSeekFight=CT_FALSE);
//资源类型转换
CT_BOOL G_ChangeResType(MasterVariableData &mvd, const CT_INT32 iResType, CT_INT32& iRes);
//获取探访信息
CT_BOOL G_GetSeekerData(CCUser* pUser,MSC_DB_Seeked_Info& seeker);

//奖励或者消耗错误码
enum ENUM_CostRewardErrorCode
{
	ENUM_CREC_RES_SUC       = 0,       //成功
	ENUM_CREC_RES_FAIL      = 1,	   //资源不足
	ENUM_CREC_ITEM_FAIL     = 2,	   //道具问题，不足或者格子不足
	ENUM_CREC_ELMBAG_NOAREA = 4,	   //元素背包不足
	ENUM_CREC_NO_ID_FAIL    = 8        //id找不到响应的奖励
};

//执行的方式
enum ENUM_PerformCostRewardType
{
	ENUM_PCRT_TEST = 1,              //测试
	ENUM_PCRT_PERFORM = 2,           //执行
	ENUM_PCRT_TEST_PERFORM=3,       //测试并执行
};

//添加奖励或者消耗
ENUM_CostRewardErrorCode G_PerformCostReward(CT_INT32 iRewardID, CCUser* pUser, ENUM_PerformCostRewardType type, CMD_G_NormalReward& addReward, CT_INT32* pGoods, CT_INT32* pCount,FakeProBase &fb);

//添加奖励或者消耗
//参数标准 Resoure res[TEAM_RES_CNT], Item item[TEAM_ITEM_CNT], CT_INT32 iGoods[GOODS_COUNT*2 ], CT_INT32 iCount[GOODS_COUNT*2 ])
CT_VOID G_GetReward(CT_INT32 iRewardID, Resoure* pRes, Item* pItem, CT_INT32* pGoods, CT_INT32* pCount,FakeProBase &fb);

//添加奖励消耗
CT_INT32 G_GetCostReward(CT_INT32 iID,CCUser *pUser,const CT_DWORD dwtype,const CT_DWORD saveTag,RewardCost &rc,FakeProBase &fb);

// 产生用户随机
CT_INT32 G_Random(CT_DWORD dwUserID, CT_INT32 iParam1, CT_INT32 iParam2);

CT_VOID G_SendClientData(CCUser &user, CT_DWORD mainID, const CT_DWORD subID, CT_CHAR *buf, CT_INT32 bufLen );

//保存战报
CT_VOID G_SaveFightingRecord(CCDBConnector& m_dbConnector,UserFightingStatus &FightingData,CT_INT32 iPos,CT_INT32 iSeed);
//读取战报
CT_VOID G_GetFightingRecord(CCDBResult* pResult,CombatUnitData &data,CT_INT32 &iSceneStar,CT_INT32 &iSeed,CT_INT32 &iIndex,CT_DWORD &dwUserID);

//滚动公告拼接
CT_BOOL G_GetRollBroadecastData(CT_INT32 iID,CT_CHAR *pOutBuf,CT_INT32 iOutLen,CT_CHAR *pInBuf,CT_INT32 iInBufLen);
//发送公告
CT_VOID G_SendRollBroadecast(CT_INT32 iID,CT_INT32 iBroadcastType,CT_INT32 iBroadcastTime,CT_CHAR *pInBuf,CT_INT32 iInBufLen);

//发送城市线程消息
CT_VOID G_SendCityThreadInfo(CT_DWORD dwUserID,CT_DWORD mainID, CT_DWORD subID, const CT_CHAR* pBuf, CT_INT32 len);
//发送战斗线程消息
CT_VOID G_SendFightThreadInfo(CT_DWORD dwUserID,CT_DWORD mainID, CT_DWORD subID, const CT_CHAR* pBuf, CT_INT32 len);
//发送DB线程消息
CT_VOID G_SendDBThreadInfo(CT_DWORD dwUserID,CT_DWORD mainID, CT_DWORD subID, const CT_CHAR* pBuf, CT_INT32 len);
//发送LOGDB线程消息
CT_VOID G_SendLogDBThreadInfo(CT_DWORD dwUserID,CT_DWORD mainID, CT_DWORD subID, const CT_CHAR* pBuf, CT_INT32 len);

//战斗属性加成
CT_VOID G_AddFightAttribute(MasterCombatStatus &mcs,const CT_INT32 iType,const CT_FLOAT fValue,const CT_FLOAT fValuePercent=1.0f);
//获取战斗属性值
CT_INT32 G_GetMasterCombatStatusValue(CT_INT32 iType,MasterCombatStatus &mcs);

//发送错误消息
CT_VOID G_SendErrorInfo(CCUser *pUser,CT_INT32 iType, CT_INT32 iID);

#endif
