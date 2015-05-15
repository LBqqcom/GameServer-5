/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CFightTeamManager.h
Version:		1.0
Description:    战队管理
Author:			zhoujun
Create Date:	2013-5-3
History:            

******************************************************************/
#ifndef CCFIGHTTEAM_MANAGER_H_
#define CCFIGHTTEAM_MANAGER_H_
#include <map>
#include <string>
#include "CNetInterface.h"
#include "CSingleton.h"
#include "CFightTeam.h"
#include "CMD_Game.h"
#include "CMutex.h"

#include "CThreadMes.h"

//招募
struct STRecruit
{
	CT_DWORD dwFTID;
	CT_INT64 iBeginTime;
	CT_INT32 iLastingTime;
	CT_CHAR  cRecruit[DESCRIPTION_SIZE];
	CT_INT32 iAuto;							// 创建战队自动招募
};
//战队管理类
class CCFightTeamManager : public CCSingleton<CCFightTeamManager>
{

public:
	typedef std::map<CT_DWORD, CCFightTeam>					FightTeamMap;
	typedef std::map<CT_DWORD, CCFightTeam>::iterator			FTM_IT;

	typedef std::map<std::string, CT_DWORD>					FightTeamNameMap;
	typedef std::map<std::string, CT_DWORD>::iterator		FTNM_IT;

	typedef std::map<CT_DWORD, STRecruit>	FightTeamRecruitMap;
	typedef std::map<CT_DWORD, CT_DWORD>	FightTeamUserMap;

private:
	//战队信息表
	FightTeamMap					m_fightTeamMap;
	//战队名称表
	FightTeamNameMap				m_fightTeamNameMap;
	//用户 -> 战队
	FightTeamUserMap				m_fightTeamUserMap;
	//招募信息
	FightTeamRecruitMap				m_fightTeamRecruitMap;
	//战队排名
	std::map<CT_INT32, CT_DWORD>	m_ftRankingMap;
	//互斥锁
	CCMutex							m_mutex;  

public:
	CCFightTeamManager(CT_VOID);
	~CCFightTeamManager(CT_VOID);

public:
	
	//查找数据
	CT_BOOL Find(CT_DWORD id);
	//查找数据
	CT_BOOL Find(CT_DWORD id, CCFightTeam** ft);
	//通过昵称查找
	CT_BOOL Find(std::string cFightTeamName, CCFightTeam** ft);
	//清除数据
	CT_VOID Erase(CT_DWORD id);
	//清空
	CT_VOID Clear();
	//获取战队数目
	CT_INT32 GetFightTeamCount(){return m_fightTeamMap.size();}

	CT_DWORD NewFightTeam(CCUser *pUser, CT_CHAR* pFightTeamName, CT_CHAR* pDescription,G_FightTeamUpgrade &ftug);		// 新建战队ID
	CT_VOID DeleteFightTeam(CT_DWORD dwFTID);													//	解散战队

	CT_VOID	InitFightTeam(const MSC_DB_FightTeamBase &dbFightTeamBase);						// 初始化插入已有战队
	CT_VOID	InitFightTeamMember(const MSC_DB_FightTeamMember &dbFightTeamMember);				// 初始化插入已有战队的队员
	CT_VOID	InsertFightTeamRecruit(const MSC_DB_FightTeamRecruit &dbFightTeamRecruit);			// 插入招募信息(共用)
	CT_VOID	InitEventLog(const MSC_DB_FightTeamEventLog &dbFightTeamEventLog);					// 初始化插入战队动态
	CT_VOID InitFightTeamSummon(const MSC_DB_FightTeamSummonInfo &dbFightTeamSummoninfo);		//召唤信息
	CT_VOID InitFihgtTeamSummonItem(const MSC_DB_FightTeamSummInfoItem &dbFightTeamSummonItem);	//召唤道具奖励
	CT_VOID InsertCSInspore(MSC_DB_INSPORE &DBCsi);										//威能
	CT_VOID InsertTimeBossInfo(MSC_DB_FTTimeBossInfo &bi);														//插入时限boss
	CT_VOID InsertTimeBossMember(MSC_DB_FTTimeBossMember &bm);														//插入时限boss成员

    //任何人不得改动这个函数的实现（死锁相关） 彭文
	CT_DWORD GetFightTeamIDByUser(CT_DWORD dwUser);

	CT_VOID ClearReqRsp(CT_DWORD dwUser);			// 当某用户已加入某战队后, 清除掉他在其他战队的邀请和申请

	CT_VOID InsertUserMap(CT_DWORD dwUser, CT_DWORD dwFTID) {_AUTOMUTEX(&m_mutex); m_fightTeamUserMap.insert(FightTeamUserMap::value_type(dwUser, dwFTID));}

	CT_VOID DeleteUserMap(CT_DWORD dwUser) {_AUTOMUTEX(&m_mutex); m_fightTeamUserMap.erase(dwUser);}

//	CT_BOOL GetFightTeamBase(CT_DWORD dwUser, CMD_G_FightTeamBase *pFightTeamBase, CT_INT32 iResCount);

	CT_VOID BrowseFightTeam(CT_CHAR *out_buf, CT_INT32 &bufLen);
	CT_VOID BrowseFightTeamRecruit(CT_CHAR *out_buf, CT_INT32 &bufLen);

	CT_VOID GetFightTeamInviteList(CT_DWORD dwUser, CT_CHAR *pBuf, CT_DWORD &bufLen);	// 获取邀请列表

	CT_VOID CheckFightTeamRecruit();
	CT_BOOL CheckFightTeamRecruit(CT_DWORD dwFTID);

	CT_VOID UserExitEvent(CCUser *pUser);		// 用户下线,更新战队成员数据
	CT_VOID UserLoginEvent(CT_DWORD dwUser);	// 用户上线, 推送成员在线状态

	CT_VOID RefreshFightTeamTop();		// 每日刷新战队排名

	CT_VOID ClearFightTeamSummon();		//清理战队召唤

	CT_VOID GetFightTeamTop(CT_DWORD *dwFTList, CT_INT32 &iCount);

	CT_VOID SendFightTeamChat(CMD_G_FightTeamChat *pFightTeamChat, CT_DWORD len);	//战队聊天

	CT_VOID FightTeamMemberRef(const CT_DWORD dwUser, const CT_INT32 iGrade, const CT_INT32 iFightingPower);

	CT_VOID SendFightTeamMemberGuard(const CT_DWORD dwUserID);

	// 更新龙岛数据
	CT_VOID UpdateIslandData();
	//修改战队名字
	CT_VOID ChangeFTName(CT_CHAR cOldName[NAME_SIZE],CT_CHAR cNewName[NAME_SIZE]);

public:
	//更新数据库
	CT_VOID DBUpdataData();

public:

};


#endif