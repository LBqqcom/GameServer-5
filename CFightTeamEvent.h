#ifndef CCFIGHTTEAMEVNET_H_
#define CCFIGHTTEAMEVNET_H_
#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCFightTeam;
class CCFightTeamEvent:public CCGameMesEvent
{
public:
	CCFightTeamEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCFightTeamEvent(void);

private:
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}

private:
	CT_VOID DisposeFightTeamCreate(CT_CHAR* pBuf,CT_DWORD len);			// 创建战队

	CT_VOID DisposeFightTeamRequest(CT_CHAR* pBuf,CT_DWORD len);		// 加入战队请求

	CT_VOID DisposeFightTeamAgree(CT_CHAR* pBuf, CT_DWORD len);			// 同意申请

	CT_VOID DisposeFightTeamDisagree(CT_CHAR* pBuf, CT_DWORD len);		// 不同意申请

	CT_VOID DisposeFightTeamInvite(CT_CHAR* pBuf, CT_DWORD len);		// 邀请成员

	CT_VOID DisposeFightTeamInviteFind(CT_CHAR* pBuf, CT_DWORD len);

	CT_VOID DisposeFightTeamAccept(CT_CHAR* pBuf, CT_DWORD len);		// 同意邀请

	CT_VOID DisposeFightTeamReject(CT_CHAR* pBuf, CT_DWORD len);		// 不同意邀请

	CT_VOID DisposeFightTeamContribute(CT_CHAR* pBuf, CT_DWORD len);	// 捐献

	CT_INT32 DisposeFightTeamContributeValidate(CCUser *pUser,CMD_G_FightTeam_Donate *pFtd,CCFightTeam *pFightTeam);//捐献验证

	CT_VOID DisposeFightTeamTransfer(CT_CHAR* pBuf, CT_DWORD len);		// 转让队长

	CT_VOID DisposeFightTeamQuit(CT_CHAR* pBuf, CT_DWORD len);			// 退出战队

	CT_VOID DisposeFightTeamElite(CT_CHAR* pBuf, CT_DWORD len);			// 设置战队精英

	CT_VOID DisposeFightTeamDesc(CT_CHAR* pBuf, CT_DWORD len);			// 设置战队公告

	CT_VOID DisposeFightTeamSearch(CT_CHAR* pBuf, CT_DWORD len);		// 查询战队

	CT_VOID DisposeFightTeamRecruit(CT_CHAR* pBuf, CT_DWORD len);		// 发布招募

	CT_VOID DisposeFightTeamRecruitSure(CT_CHAR* pBuf, CT_DWORD len);	// 确认发布招募

	CT_VOID DisposeFightTeamLookRecruit(CT_CHAR* pBuf, CT_DWORD len);	// 请求招募列表

	CT_VOID DisposeFightTeamDisband(CT_CHAR* pBuf, CT_DWORD len);		// 解散战队

	CT_VOID DisposeFightTeamLookEvent(CT_CHAR* pBuf, CT_DWORD len);		// 请求战队动态列表

	CT_VOID DisposeFightTeamLookFight(CT_CHAR* pBuf, CT_DWORD len);		// 查看战斗

	CT_VOID DisposeFightTeamMemberFire(CT_CHAR* pBuf, CT_DWORD len);	// 踢出队员

	CT_VOID DisposeFightTeamList(CT_CHAR* pBuf, CT_DWORD len);			//请求战队列表

	CT_VOID DisposeFightTeamReqList(CT_CHAR* pBuf, CT_DWORD len);		//请求申请列表

	CT_VOID DisposeFightTeamInviteList(CT_CHAR* pBuf, CT_DWORD len);	//请求邀请列表

	CT_VOID DisposeFightTeamPlayList(CT_CHAR* pBuf, CT_DWORD len);		//请求玩家列表

	CT_VOID DisposeFightTeamMember(CT_CHAR* pBuf, CT_DWORD len);		//请求成员列表

	CT_VOID DisposeFightTeamFightingData(CT_CHAR* pBuf, CT_DWORD len);	//插入成员战斗力数据

	CT_VOID DiposeFightTeamMemeRefresh(CT_CHAR* pBuf, CT_DWORD len);    //彭文添加，刷新队员数据

	CT_VOID DiposeFightTeamUpGrade(CT_CHAR *pBuf,CT_DWORD len);		//战队升级

	CT_INT32 FightTeamUpGradeValidate(CCUser *pUser,CMD_G_FightTeamBase &ftb);	//战队升级检测

	CT_VOID DiposeFightTeamNemberScienceUp(CT_CHAR *pBuf,CT_DWORD len);		//成员科技升级

	CT_INT32 FightTeamNemberScienceValidate(CCUser *pUser,CT_INT32 iScienceID,CCFightTeam *pFightTeam);//科技升级检测

	CT_VOID DisposeFightTeamSummonBoss(CT_CHAR *pBuf,CT_DWORD len);			//战队召唤

	CT_INT32 SummonBossValidate(CCUser *pUser,CCFightTeam *pFightTeam,CT_INT32 iBossIndex,CT_INT32 iBossType);		//召唤验证
	
	CT_VOID DisposeFightTeamHelpSummon(CT_CHAR *pBuf,CT_DWORD len);			//召唤助阵

	CT_INT32 HelpSummonValidate(CCUser *pUser,CCFightTeam *pFightTeam);		//助阵验证

	CT_VOID DisposeFightTeamSummonReward(CT_CHAR *pBuf,CT_DWORD len);		//召唤奖励

	CT_INT32 SummonRewardValidate(CCUser *pUser,CCFightTeam *pFightTeam,CT_INT32 iUserIndex,CT_INT32 iChestIndex,CT_DWORD dwFTID);//领奖验证

	CT_VOID DiposeFightTeamHelpSummonLeave(CT_CHAR *pBuf,CT_DWORD len);		//助阵离开

	CT_VOID DisposeFightTeamSummonFighting(CT_CHAR *pBuf,CT_DWORD len);		//召唤战斗

	CT_VOID DisposeFightTeamSummonFightingEnd(CT_CHAR *pBuf,CT_DWORD len);		//召唤战斗结果
	
	CT_VOID DisposeFightTeamInvit(CT_CHAR *pBuf,CT_DWORD len);				//召唤邀请

	CT_VOID SummonFightingAdd(UserFightingStatus *FightingNember,const CT_INT32 iCount,const CT_INT32 iType);	//召唤战斗加成

	CT_VOID DiposeFightTeamTowerCP(CT_CHAR *pBuf,CT_DWORD len);

	CT_VOID DisposeFightTeamUpgreadTask(CT_CHAR *pBuf, CT_DWORD len);		//处理战队升级任务
	//修改战队名称
	CT_VOID DisposeChangeFTName(CT_CHAR *pBuf, CT_DWORD len);
	//时限BOOS召唤
	CT_VOID DisposeFTTBSummon(CT_CHAR *pBuf, CT_DWORD len);
	//时限BOSS加入
	CT_VOID DisposeFTTBAdd(CT_CHAR *pBuf, CT_DWORD len);
	//时限BOOS邀请
	CT_VOID DisposeFTTBInvite(CT_CHAR *pBuf, CT_DWORD len);
	//时限BOOS开始
	CT_VOID DisposeFTTBBegin(CT_CHAR *pBuf, CT_DWORD len);
	//时限BOOS战斗
	CT_VOID DisposeFTTBFight(CT_CHAR *pBuf, CT_DWORD len);
	//时限BOOS战斗返回
	CT_VOID DisposeFTTBFightReturn(CT_CHAR *pBuf, CT_DWORD len);
	//时限BOOS离开
	CT_VOID DisposeFTTBLeave(CT_CHAR *pBuf, CT_DWORD len);
	//时限BOOS奖励
	CT_VOID DisposeFTTBReward(CT_CHAR *pBuf, CT_DWORD len);
	//时限boss加入验证
	CT_INT32 FTTBAddValidate(CCUser *pUser,CCFightTeam *pFightTeam);
	//时限boss战斗验证
	CT_INT32 FTTBFightValidate(CCUser *pUser,CCFightTeam *pFightTeam);
	//领奖验证
	CT_INT32 TBRewardValidate(CCUser *pUser,CCFightTeam *pFightTeam,CT_INT32 iChestIndex);
	//删除时限boss信息
	CT_VOID DeleteTimeBossInfo(CT_DWORD dwUserID,CCFightTeam *pFightTeam);

};
#endif