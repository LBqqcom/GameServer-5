#ifndef CCFIGHTINGEVNET_H_
#define CCFIGHTINGEVNET_H_
#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"
#include "CFightingLogic.h"

//战斗站位
/************************************************************************/
/*   用户方			怪物方
	 3		0		5		8
		2				7
	 4		1		6		9											
	 
*/
/************************************************************************/

#define				ROUNDD_MAX					15				//最大回合
class CCUser; 

struct MSC_Fighting_End;
struct MSC_DB_W_PVPFightingInfo;
struct MSC_Fighting_Info;
struct MSC_PVP_Fighting_Info;
struct MSC_TeamPVP_ResRobbery;
struct MSC_TeamPVE_ResRobbery;

struct FightingUserInfo;
struct MasterCombatStatus;

struct G_MonsterGroup;
struct G_FightingScene;

enum ENUM_LoadResRobbery
{
	LOAD_FAIL			=	0,			//资源掠夺失败
	LOAD_SUC			=	1,			//资源掠夺成功
	LOAD_OVER			=	2,			//资源掠夺结束
};


struct FightingTeamPVP
{
	CT_INT32							iLen[FIGHTING_NUM];				//战报长度
	CT_INT32							iWiner[FIGHTING_NUM];			//胜利方
	CMD_G_MasterInfo					MasterInfo[TEAM_NUM];		//领主信息
	CMD_G_MasterInfo					RivalInfo[TEAM_NUM];		//对手信息
	CCFightingLogic::FightingSaveList	FSList[FIGHTING_NUM];			//战报
};

//(需要内部构造,可能有问题)
struct FightingTeamPVE
{
	CT_INT32							iLen[FIGHTING_NUM];				//战报长度
	CT_INT32							iWiner[FIGHTING_NUM];			//胜利方
	CMD_G_MasterInfo					MasterInfo[TEAM_NUM];		//领主信息
	CMD_G_MosterInfo					MosterInfo[TEAM_NUM];		//怪物信息
	CCFightingLogic::FightingSaveList	FSList[FIGHTING_NUM];			//战报
};

class CCFightingEvnet1:public CCGameMesEvent
{
private:
	CCFightingLogic		*m_pFightingLogic;			//战斗逻辑
	CT_CHAR				*m_pFightingBuffer;			//战报缓冲

public:
	CCFightingEvnet1(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCFightingEvnet1(void);

private:
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}

	//消息函数
private:
	//战斗消息
	CT_BOOL DisposeFightingPVE(CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//PVP
	CT_BOOL DisposeFightingPVP(CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//观看战斗
	CT_BOOL	DisposeLookFighting(CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//事件战斗
	CT_BOOL	DisposeEventFighting(CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//PVP观战
	CT_BOOL DisposePVPLookFighting(CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//资源掠夺
	CT_BOOL DisposeResRobberyPVP(CT_CHAR* pBuf,CT_DWORD len,CT_DWORD dwSubID);
	//资源掠夺
	CT_BOOL DisposeResRobberyPVE(CT_CHAR* pBuf,CT_DWORD len,CT_DWORD dwSubID);
	//组队副本
	CT_BOOL DiposeTeamDuplicate(CT_CHAR* pBuf,CT_DWORD len);
	//元素之塔
	CT_BOOL DisposeTowerCheckPoint(CT_CHAR *pBuf,CT_DWORD len,CT_DWORD dwSubID);
	//
	CT_BOOL DisposeTowerPVPResRobbery(CT_CHAR *pBuf,CT_DWORD len,CT_DWORD dwSubID);
	//战队BOSS
	CT_BOOL DiposeFTSummonBoss(CT_CHAR *pBuf,CT_DWORD len);
	//世界BOSS
	CT_BOOL DiposeWorldBoss(CT_CHAR *pBuf,CT_DWORD len);
	//跨服战斗
	CT_BOOL DisposeCSFighting(CT_CHAR *pBuf,CT_DWORD len,CT_DWORD dwSubID);
	//8强战斗
	CT_BOOL DisposeCSFinalEight(CT_CHAR *pBuf,CT_DWORD len);
	//龙胆战斗
	CT_BOOL DisposeIslandFight(CT_CHAR *pBuf,CT_DWORD len);
	//掠夺战斗
	CT_BOOL DisopsePlunderFight(CT_CHAR *pBuf,CT_DWORD len);
	//绑票战斗
	CT_BOOL DisopseKidnapFight(CT_CHAR *pBuf,CT_DWORD len);
	//营救战斗
	CT_BOOL DisposeRescueFight(CT_CHAR *pBuf,CT_DWORD len,CT_DWORD dwSubID);
	//时限boss战斗
	CT_BOOL DisposeFTTimeBoss(CT_CHAR *pBuf,CT_DWORD len);

	//重置函数
private:
	//重置函数
	CT_VOID ResetVariable();

	//加载函数
private:
	//加载战斗
	CT_BOOL	LoadFighting(const MSC_Fighting_Info &FightingInfo,const G_FightingScene &FightingScene,CCNetInterface* pNetHandle);
	//加载事件战斗
	CT_BOOL	LoadEventFihting(const MSC_Fighting_Info &FightingInfo,CCNetInterface* pNetHandle);
	//加载PVP战斗
	CT_BOOL LoadPVPFighting(const MSC_PVP_Fighting_Info &FightingInfo,CCNetInterface* pNetHandle,CT_DWORD dwMeUserID);
	//加载组队战斗
	CT_BYTE LoadTeamFighting(const UserFightingStatus *FightingInfo,CT_BOOL bWin,CT_DWORD dwMeUserID,CT_INT32 iIndex);

private:
	//加载怪物
	CT_BOOL LoadMoster(CCNetInterface* pNetHandle,CT_INT32 iWorldSceneID,CT_INT32 iSceneID,CT_INT32 iDifficulty,CT_INT32 iMasterGrade);
	//加载事件怪物
	CT_BOOL LoadEventMoster(CCNetInterface* pNetHandle,CT_INT32 iEvent,CT_INT32 iMasterGrade);
	//加载组队怪物
	CT_BYTE LoadTeamMoster(const CT_INT32 iTeamMoster[TEAM_NUM],CT_INT32 iIndex,CT_BOOL bWin,const CT_INT32 iSceneID,CT_INT32 iMasterGrade);

	//战斗函数
private:
	//开始战斗
	CT_BOOL BeginFighting();
	//战斗结束
	CT_BOOL EndFighting(CCNetInterface* pNetHandle,CT_DWORD dwSubID);

	//战报保存
private:
	//战报保存
	CT_VOID SaveFighting(const MSC_Fighting_Info &FightingPVE);
	//PVP战报保存
	CT_VOID SavePVPFighting(const MSC_PVP_Fighting_Info &FightingPVP);

private:
#if 0 //移植城市线程
	//奖励信息
	CT_VOID FightingReward(CCUser *pUser,CT_INT32 iWorldSceneID,CT_INT32 iSceneID,CT_INT32 iDifficulty,CT_INT32 iStarCount);
	//PVP奖励信息
	CT_BOOL PVPFightingReward(CCUser *pUser,CT_INT32 iWiner,const MSC_PVP_Fighting_Info &FightingInfo,MSC_DB_W_PVPFightingInfo &PVPFighting);
	//组队副本奖励
	CT_VOID TeamDuplicateReward(CCUser *pUser,const CT_INT32 iSceneID,const CT_INT32 iDifficulty);
#endif

private:
	//获得星级
	CT_INT32 GetStar(CT_INT32 iWorldSceneID,CT_INT32 iSceneID,CT_INT32 iDifficulty,CT_INT32 iScore);
	//获取组队信息
	CT_BOOL GetFightingTeam(const UserFightingStatus *FightingInfo,CMD_G_MasterInfo *FightingTeam,CT_INT32 iNum=TEAM_NUM);
	//获取组队怪物信息
	CT_BOOL GetTeamMoster(const CT_INT32 iTeamMoster[TEAM_NUM],CMD_G_MosterInfo *FightingTeam,const CT_INT32 iSceneID);

private:
	//发送战报
	CT_VOID SendFighting(FightingTeamPVP &TeamPVP,CCNetInterface* pNetHandle,const CT_INT32 iSubID);
	//发送战报
	CT_VOID SendFighting(FightingTeamPVE &TeamPVE,CCNetInterface* pNetHandle,const CT_INT32 iSubID);

private:
	CT_VOID Decompress(CT_INT32 *pBuffer,CT_INT32 iLen,CT_INT32 *pFightinginfo);
	//////////////////////////////////////////////////////////////////////////
};
#endif
