#ifndef CUSERSEEK_H_
#define CUSERSEEK_H_

/********************************************************************
	created:	2014/9/23
	created:	12:11:2012   13:53
	file base:	CCUserSeek
	file ext:	h
	author:		wangchen
	
	purpose:	
*********************************************************************/
#include <map>
#include "CGDefine.h"
#include "CMutex.h"
#include "GlobeScriptInterface.h"
#include "CLogMeg.h"
#include "CUserData.h"
#include "CUserPVPInfo.h"

class CCUser;
struct MSC_DB_EnterOtherTown;

//城墙加成
struct WallAddition
{
	CT_INT32	iID;
	CT_INT32	iGrade;
};

//建筑信息
struct BuildData
{
	CMD_G_BuildingData	bd;
	CT_INT32			iPlunderNum;
};

//礼品信息
struct SeekPresent 
{
	CT_INT32	iChestID;
	CT_INT32	iGetTime;
};

//战旗
struct TeamSeek
{
	CT_DWORD dwUserID;
	CT_INT32 iType;				// 旗帜类型 1：圣女，2：其他
	CT_CHAR cTName[NAME_SIZE];	// 战队名称
	CT_INT32 iFightResult;		// 战队掠夺结果(1:胜利,2:失败)
};


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
//	TeamSeek TeamFlag[SEEKED_SPITE_TIMES];					// 战旗
//	BuildData					buildingData[SEEK_BUILD_NUM];		// 建筑基本信息
//	CMD_G_UpgradeBuildingData	upgradeBuilding[SEEK_BUILD_NUM];	// 升级的建筑
//	CMD_G_ProductingBuildingData	productBuilding[SEEK_BUILD_NUM];// 生产的建筑
//};

//探访数据
struct SeekData
{
	CT_INT32		iSeekWillTimes;					// 已善访的次数
	CT_INT32		iSeekSpiteTimes;				// 已恶访的次数
	CT_INT32		iSeekedWillTimes;				// 已被善访的次数
	CT_INT32		iSeekedSpiteTimes;				// 已被恶访的次数
	CT_INT32		iRescueNum;						// 营救次数
	CT_INT32		iWomanNum;						// 圣女
	CT_INT32		iFarmarNum;						// 农民
	CT_INT32		iPet[SEEK_KIDNAP_PET];			// 宠物
	CT_INT32		iLevel;							// 进入别人城镇时的自己的等级
	CT_DWORD		dwSeekID;						// 当前被探访者的ID
	CT_INT32		iType;							// 当前探访类型
	SeekPresent		present[SEEKED_PRESENT_NUM];	// 得到的礼品信息
	CT_INT32		iPresentStatus[SEEK_PRESENT_ITEM_NUM];	// 三个礼物是否被赠送过的状态
	CT_INT32		iAddSpiteTimes;					// 增加的恶访次数
};

//战报
struct SeekFightingData
{
	CT_UCHAR		ucIndex[SEEK_FIGHT_RECORD_INDEX+1];	//索引
	CT_DWORD		dwUserID;							//用户ID
	CT_INT32		iType;								//探访类型
	CT_INT32		iSeed;								//随机种子

	UserFightingStatus	LeftFightingData;				//挑战方战斗数据
	UserFightingStatus	RightFightingData;              //被挑战方的战斗数据
};

struct StatisticsInfo 
{
	CT_INT32		iPlayerNum[SEEK_TYPE_NUM+1];		//记录的玩家数 
	CT_INT32		iGold;								//掠夺的金币
	CT_INT32		iFood;								//掠夺的粮食
	CT_INT32		iRank;								//掠夺的声望
	CT_INT32		iSaintess;							//绑架的圣女
	CT_INT32		iFarmer;							//绑架的农民
	CT_INT32		iBuildingNum;						//增产加速的建筑数
	CT_INT32		iPresentID[SEEK_PRESENT_ITEM_NUM];	//礼物ID
	CT_INT32		iPresentNum[SEEK_PRESENT_ITEM_NUM];	//礼物数量
	CT_INT32		iPetID[SEEK_KIDNAP_PET];			//绑架的宠物ID
	CT_INT32		iPetNum[SEEK_KIDNAP_PET];			//绑架的宠物数量
};

typedef std::map<CT_INT32,WallAddition>		WallAdditionMap;
typedef std::map<CT_LONGLONG,TeamSeek>		TeamSeekMap;		
typedef std::map<CT_DWORD,KidnapInfo>		KidnapMap;					
typedef std::map<CT_DWORD,UserInfo>			UserInfoMap;	
typedef std::map<CT_LONGLONG,VistorInfo>	VistorInfoMap;		
typedef std::vector<CT_DWORD>				SeekVector;	
typedef std::map<CT_DWORD,BuildData>		BuildDataMap;
typedef std::vector<CT_INT32>				SpeedUpBIDVector;
typedef std::map<std::string,SeekFightingData>	SeekFightingDataMap;
class CCUserSeek: public CCUserData<CT_INT32, SeekData>
{
public:
	WallAdditionMap			m_WallAdditionMap;			//城墙技能
	CT_INT32				m_iPos[POST_COUNT/2];		//城墙阵法
	
private:
	TeamSeekMap				m_TeamSeekMap;				// 战旗
	KidnapMap				m_KidnapGirlsMap;			// 绑架的圣女
	KidnapMap				m_kidnapWorkersMap;			// 绑架的工人
	UserInfoMap				m_FriendInfoMap;			// 探访界面好友
	UserInfoMap				m_RandInfoMap;				// 探访界面的随机玩家
	VistorInfoMap			m_VistorInfoMap;			// 被探访的记录
	SeekVector				m_FriendSeekVector;			// 已善访的人
	SeekVector				m_RandSeekVector;			// 已恶访的人
	UserInfoMap				m_AllRandPlayerMap;			// 所有可随机访问的人的信息
	BuildDataMap			m_BuildDataMap;				// 探访玩家建筑信息
	SpeedUpBIDVector		m_SpeedUpBidVector;			// 当前以加速过的建筑ID
	SeekFightingDataMap		m_SeekFightingMap;			//战报
	VistorInfoMap			m_SeekInfoMap;				// 探访别人的记录
	KidnapMap				m_PetKidnapMap;				// 绑架的宠物

	SeekData				m_SeekData;					//探访人员
	VistorInfo				m_VistorInfo[SEEK_DOTYPE_NUM];	//当前探访的记录

public:
    CCUserSeek();
    ~CCUserSeek();
public:
    CT_VOID Init(CCUser* pUser, CCMutex* pMutex);

	//清理用户数据
	CT_VOID Clear();
protected:
	virtual CT_INT32 transData(const SeekData &src, CT_CHAR *pDst) const;
public:
	virtual CT_VOID SyncData(CT_INT64 currTime, CT_BOOL bForce = CT_FALSE);

public:
	CT_INT64 Get64Int(CT_INT32 high,CT_INT32 low);

	/*登陆初始化*/
	//插入城墙技能
	CT_VOID InsertWallAddition(WallAddition & wa);
	//插入城墙士兵
	CT_VOID InsertWallArmy(CT_INT32 iPos[POST_COUNT/2]);
	//插入人质
	CT_VOID InsertHostages(const CT_INT32 iType,KidnapInfo &ki);
	//插入随机探访玩家列表
	CT_VOID InsertSeekRandPlayer(UserInfo userInfo) { m_AllRandPlayerMap.insert(UserInfoMap::value_type(userInfo.dwUserID, userInfo)); }
	//插入已善访过的好友
	CT_VOID InsertSeekedGoodwill(CT_DWORD dwFUserID) { m_FriendSeekVector.push_back(dwFUserID); }
	//插入已恶访过的好友
	CT_VOID InsertSeekedSpite(CT_DWORD dwFUserID) { m_RandSeekVector.push_back(dwFUserID); }
	//插入旗帜
	CT_VOID InsertFlags(TeamSeek ts);
	//插入探访界面好友
	CT_VOID InsertFriendList(UserInfo friendInfo) { m_FriendInfoMap.insert(UserInfoMap::value_type(friendInfo.dwUserID, friendInfo)); }
	//插入恶访界面随机玩家
	CT_VOID InsertRandList(UserInfo randInfo) { m_RandInfoMap.insert(UserInfoMap::value_type(randInfo.dwUserID, randInfo)); }
	//插入探访基本信息
	CT_VOID InsertSeekData(SeekData seekdata);
	//插入战报
	CT_VOID InsertFightingData(SeekFightingData &sfd,CT_BOOL bSave=CT_FALSE);

public:
	//增加城墙技能
	CT_VOID AddWallAddition(const CT_INT32 iID);
	//释放人质
	CT_VOID ReleaseHostages(const CT_DWORD dwUserID);


public:
	//获取城墙技能
	CT_BOOL GetWallAddition(const CT_INT32 iID,WallAddition &wa);
	CT_BOOL GetWallAddition(CT_CHAR *pBuf,CT_INT32 &iLen);
	//获取城墙士兵
	CT_BOOL GetWallArmy(CT_INT32 iPos[POST_COUNT/2]);
	CT_BOOL GetWallArmy(CT_CHAR *pBuf,CT_INT32 &iLen);
	//获取掠夺用户战斗信息
	CT_BOOL GetKidnpaFightData(const CT_DWORD dwUserID,UserFightingStatus &ufs);
	//获取探访信息
	CT_VOID GetSeekData(SeekData& sd);
	//获取访客信息
	CT_BOOL GetVistorInfo(CT_INT32 iType, const CT_DWORD dwUserID,VistorInfo &vi);
	//获取绑架人数
	CT_INT32 GetKidnapNum(const CT_DWORD dwUserID,const CT_INT32 iType);
	//获取旗帜数目
	CT_INT32 GetFlagCount(){_AUTOMUTEX(m_pMutex);return m_TeamSeekMap.size(); }
	//获取探访数据
	CT_BOOL GetSeekInfo(CT_CHAR* pBuf,CT_INT32 &iLen);
	//获取建筑信息
	CT_VOID GetBuildData(const CT_INT32 iBuildID,BuildData &bd);
	// 得到所有旗帜
	CT_BOOL GetTeamFlag(CT_CHAR* pBuf,CT_INT32& ilen);
	// 得到圣女人质信息
	CT_BOOL GetKidnapGirls(CT_CHAR* pBuf,CT_INT32 iLen);
	// 得到工人人质信息
	CT_BOOL GetKidnapWorkers(CT_CHAR* pBuf,CT_INT32 iLen);
	// 得到宠物人质信息
	CT_BOOL GetKidnapPets(CT_CHAR* pBuf,CT_INT32 iLen);
	// 得到已探访次数
	CT_INT32 GetSeekTimes(CT_INT32 iType);
	// 得到礼品ID
	CT_INT32 GetSeekPresent(G_ChestByLevel& chest);
	// 得到所有礼品
	CT_BOOL GetAllPresent(CT_CHAR* pBuf, CT_INT32& iLen);
	//获得战报信息
	CT_BOOL GetFightingData(CT_UCHAR *pIndex,SeekFightingData &sfd);

public:
	//设置城墙士兵
	CT_VOID SetWallArmy(CT_INT32 iPos[POST_COUNT/2]);
	//设置建筑信息
	CT_VOID SetBuildData(const CT_INT32 iBuildID,BuildData &bd);

public:
	// 刷新探访人
	CT_VOID UpdateSeekedPerson(CT_INT32 iType);
//	CT_VOID UpdateSeekedPerson();
	// 插入被访者的基础信息
	CT_VOID InsertSeekerBaseInfo(const CMD_G_EnterTown_Others_Suc& enterTown);
	// 是否被善访过
	CT_BOOL IsSeekGoodwill(CT_DWORD dwFUserID);
	// 是否被恶访过
	CT_BOOL IsSeekSpite(CT_DWORD dwFUserID);
	// 检测探访次数是否足够
	CT_BOOL CheckSeekTimes(CT_INT32 iType);
	// 检测被探访次数是否足够
	CT_BOOL CheckSeekedTimes(CT_INT32 iType);	
	// 检测建筑是否可加速
	CT_BOOL CheckBuildingSpeedUp(CT_INT32 iBID);
	// 检测礼物是否可以赠送
	CT_BOOL CheckPresentStatus(CT_INT32* pPresentID);
	// 检测宠物是否被绑架
	CT_BOOL IsPetKidnap(CT_INT32 iPetChangeID);

	// 探访一次
	CT_VOID SeekOneTimes(CT_INT32 iType, CT_DWORD dwFUserID);
	// 被探访一次
	CT_VOID SeekedOneTimes(CT_INT32 iType);
	// 判断好友是否在探访界面上
	CT_BOOL IsInSeekedList(CT_DWORD dwUserID);
	// 得到已获得的礼物个数
	CT_INT32 GetPresentNum();
	// 收到礼物
	CT_VOID GetPresent(CT_INT32 iChestID);
	// 得到当期被探访人
	CT_DWORD GetSeeked() { return m_SeekData.dwSeekID; }
	// 更新探访信息
	CT_VOID UpdateSeekData(SeekData& sd);
	// 插入战旗
	CT_VOID InsertTeamFlag(TeamSeek& ts);
	// 拔掉战旗
	CT_VOID PullTeamFlag(const CT_DWORD dwUserID,const CT_INT32 iType);
	// 更新人质
	CT_VOID UpdateKidnap(CT_INT32 iType, KidnapInfo& kidnap);
	// 删除人质
	CT_VOID DeleteKidnap(CT_INT32 iType, KidnapInfo& kidnap);
	// 得到探访界面访客ID
	CT_BOOL GetSeekListPlayer(CT_INT32 iType, CT_CHAR* pBuf, CT_INT32 iLen);
	// 添加探访别人的记录信息
	CT_VOID AddVistorInfo(VistorInfo& vi);
	// 添加被探访的记录信息
	CT_VOID AddSeekInfo(VistorInfo& vi);
	// 得到当前已探访信息(访问别人）
	CT_BOOL GetCurrentVistorInfo(VistorInfo* pvi, CT_INT32& iLen);
	// 设置当前探访信息
	CT_VOID SetVistorInfo(VistorInfo vi);
	// 领取礼物
	CT_VOID UsePresent();
	// 加速建筑，记录ID
	CT_VOID SpeedUpBuilding(CT_INT32 iBID) {_AUTOMUTEX(m_pMutex); m_SpeedUpBidVector.push_back(iBID); }
	// 赠送礼物，记录ID
	CT_VOID SendPresent(CT_INT32 iPresentID);
	// 更新领主礼物
	CT_VOID UpdatePresent();
	// 更新自己的访客记录
	CT_VOID UpdateSeekVisitorInfo(CT_INT32 iType, const VistorInfo& vi);

	// 回营
	CT_VOID SeekVistor();
	CT_VOID GetVisitorDes(VistorInfo& vi, CT_CHAR* pInfo,CT_INT32 iSize);

	// 0点更新
	CT_VOID UpdateAllSeekData();

	// 增加恶访次数
	CT_BOOL AddSpiteTimes(CT_INT32 iTimes);

	//发送黑店
	CT_VOID SendBlacketMarket();

	//得到今日统计
	CT_BOOL GetTotalInfo(TotalInfo* pInfo, CT_INT32 iSize,CT_INT32& iNum, CT_BOOL IsToday = CT_FALSE);

	//单条统计记录
	CT_VOID SingleStatistics(VistorInfoMap& vistorInfoMap, StatisticsInfo& statistics,CT_BOOL isToday = CT_FALSE);

};

#endif

