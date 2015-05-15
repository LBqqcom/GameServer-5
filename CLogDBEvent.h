#ifndef CCLOGDBEVNET_H_
#define CCLOGDBEVNET_H_
#include "CGameMesEvent.h"

class CCDBConnector;
class CCLogDBEvent:public CCGameMesEvent
{
private:
	CCDBConnector& m_dbConnector;
public:
	CCLogDBEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCLogDBEvent(void);

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
	//军队升级记录
	CT_VOID DisposeArmyUpdataGrade(CT_CHAR *pBuf,CT_INT32 iLen);
	//军队进阶
	CT_VOID DisposeArmyUpdataRank(CT_CHAR *pBuf,CT_INT32 iLen);
	//购买背包格子记录
	CT_VOID DisposeBagSlotUpdata(CT_CHAR *pBuf,CT_INT32 iLen);
	//增加城市记录
	CT_VOID DisposeMasterArea(CT_CHAR *pBuf,CT_INT32 iLen);
	//道具增强
	CT_VOID DisposePropAddStrengthen(CT_CHAR *pBuf,CT_INT32 iLen);
	//道具使用完记录
	CT_VOID DisposePropFinish(CT_CHAR *pBuf,CT_INT32 iLen);
	//道具继承记录
	CT_VOID DisposePropInherit(CT_CHAR *pBuf,CT_INT32 iLen);
	//道具增强
	CT_VOID DisposePropUpdataTrain(CT_CHAR *pBuf,CT_INT32 iLen);
	//保存技能记录
	CT_VOID DisposeSaveSkill(CT_CHAR *pBuf,CT_INT32 iLen);
	//事件完成记录
	CT_VOID DisposeSaveEventFinish(CT_CHAR *pBuf,CT_INT32 iLen);
	//保存天赋记录
	CT_VOID DisposeSaveTalent(CT_CHAR *pBuf,CT_INT32 iLen);
	//保存任务完成记录
	CT_VOID DisposeTaskFinish(CT_CHAR *pBuf,CT_INT32 iLen);
	//用户退出记录
	CT_VOID DispseUserExit(CT_CHAR *pBuf,CT_INT32 iLen);
	//用户升级记录
	CT_VOID DisposeUpGradeFinish(CT_CHAR *pBuf,CT_INT32 iLen);
	//vip申请记录
	CT_VOID DisposeVIPApply(CT_CHAR *pBuf,CT_INT32 iLen);
	//活跃度奖励记录
	CT_VOID DisposeActiveReward(CT_CHAR *pBuf,CT_INT32 iLen);
	//打开宝箱
	CT_VOID DisposeOpenChest(CT_CHAR* pBuf,CT_DWORD len);
	//在线人数统计
	CT_VOID DisposeOnlineUser(CT_CHAR *pBuf,CT_INT32 iLen);
	//聊天次数统计
	CT_VOID	DisposeChatTime(CT_CHAR *pBuf,CT_INT32 iLen);
	//用户生产记录
	CT_VOID DisposeProductFinish(CT_CHAR *pBuf,CT_INT32 iLen);
	//用户充值记录
	CT_VOID DisposeCharge(CT_CHAR *pBuf,CT_INT32 iLen);
	//用户充值更新
	CT_VOID DisposeChargeUpdate(CT_CHAR *pBuf,CT_INT32 iLen);
	//每日签到奖励
	CT_VOID DisposeSignReward(CT_CHAR *pBuf,CT_INT32 iLen);
	//宠物主动技能升级
	CT_VOID DisposePetUpSkill(CT_CHAR *pBuf,CT_INT32 iLen);
	//宠物命名
	CT_VOID DisposePetRename(CT_CHAR *pBuf,CT_INT32 iLen);
	//物品出售
	CT_VOID	DisposeItemSell(CT_CHAR *pBuf,CT_INT32 iLen);
	//商店购买
	CT_VOID DisposeShopping(CT_CHAR *pBuf,CT_INT32 iLen);
	//体力购买
	CT_VOID	DisposeStatminaBuy(CT_CHAR *pBuf,CT_INT32 iLen);
	//pvp奖励
	CT_VOID DisposePVPReward(CT_CHAR *pBuf,CT_INT32 iLen);
	//副本战斗奖励
	CT_VOID DisposeDuplicate(CT_CHAR *pBuf,CT_INT32 iLen);
    // 附魔记录
    CT_VOID DisposeEnchant(CT_CHAR *pBuf,CT_INT32 iLen);
	//副本奖励领取
	CT_VOID DisposeDuplicateReward(CT_CHAR *pBuf,CT_INT32 iLen);
	//副本进入消耗
	CT_VOID DisposeDuplicateEnter(CT_CHAR *pBuf,CT_INT32 iLen);
	//竞技场战斗消耗
	CT_VOID DisposePVPFightingCost(CT_CHAR *pBuf,CT_INT32 iLen);
	//活动奖励
	CT_VOID DisposeActiveEventReward(CT_CHAR *pBuf,CT_INT32 iLen);
	//战斗购买次数
	CT_VOID DisposeBuyFightingTime(CT_CHAR *pBuf,CT_INT32 iLen);
	//强制刷新
	CT_VOID DisposePVPRefresh(CT_CHAR *pBuf,CT_INT32 iLen);
	//钻石减少强化冷却时间
	CT_VOID DisposeClearCD(CT_CHAR *pBuf,CT_INT32 iLen);
	//信息反馈
	CT_VOID DisposeFeedBack(CT_CHAR *pBuf,CT_INT32 iLen);
	//扫荡加速
	CT_VOID DisposeRaidsSpeed(CT_CHAR *pBuf,CT_INT32 iLen);
	//zhoujun 祭祀奖励日志
	CT_VOID DisposeWorshipReward(CT_CHAR *pBuf,CT_INT32 iLen);
	//组队副本奖励
	CT_VOID DiposeTeamDuplicateReward(CT_CHAR *pBuf,CT_INT32 iLen);
	//组队副本消耗
	CT_VOID DisposeTeamDuplicateCost(CT_CHAR *pBuf,CT_INT32 iLen);
	//资源生产记录
	CT_VOID DisposeResProductSave(CT_CHAR *pBuf,CT_INT32 iLen);
	//资源占领记录
	CT_VOID DisposeResOccpySave(CT_CHAR *pBuf,CT_INT32 iLen);
	//军衔俸禄记录
	CT_VOID DisposeSalary(CT_CHAR *pBuf,CT_INT32 iLen);
	//军衔升级
	CT_VOID DisposeRankingUpGrade(CT_CHAR *pBuf,CT_INT32 iLen);
	//物品使用记录
	CT_VOID DisposeUserItem(CT_CHAR *pBuf,CT_INT32 iLen);
	//事件奖励记录
	CT_VOID DisposeEventReward(CT_CHAR *pBuf,CT_INT32 iLen);
	//宠物升级
	CT_VOID DisposePetUpGrade(CT_CHAR *pBuf,CT_INT32 iLen);
	//MasterSaveData
	CT_VOID DisposeMasterSaveData(CT_CHAR *pBuf,CT_INT32 iLen);
	//战队升级
	CT_VOID DisposeFightTeamUpGrade(CT_CHAR *pBuf,CT_INT32 iLen);
	//战队召唤消耗
	CT_VOID DisposeFightTeamSummonCost(CT_CHAR *pBuf,CT_INT32 iLen);
	//战队召唤奖励资源
	CT_VOID DisposeFightTeamSummonRewardRes(CT_CHAR *pBuf,CT_INT32 iLen);
	//战队召唤奖励道具
	CT_VOID DisposeFightTeamSummonRewardItem(CT_CHAR *pBuf,CT_INT32 iLen);
	//战队科技升级
	CT_VOID DisposeFightTeamScienceUpGrade(CT_CHAR *pBuf,CT_INT32 iLen);
	//元素塔通关
	CT_VOID DisposeTowerPass(CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID DisposeTowerOccupy(CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID DisposeTowerHelp(CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID DisposeTreasureUse(CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID DisposeAddItem(CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID DisposeBattleFlag(CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID DisposeEatElm(CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID DisposeEmbedEquip(CT_CHAR *pBuf,CT_INT32 iLen);
	//跨服奖励
	CT_VOID DisposeCSReward(CT_CHAR *pBuf,CT_INT32 iLen);
	//威能消耗
	CT_VOID DisposeInsporeCost(CT_CHAR *pBuf,CT_INT32 iLen);
	//支持
	CT_VOID DiposeSupport(CT_CHAR *pBuf,CT_INT32 iLen);
	//头衔
	CT_VOID DisposeTitle(CT_CHAR *pBuf,CT_INT32 iLen);
	//龙岛
	CT_VOID DisposeIsland(CT_CHAR* pBuf, CT_INT32 iLen);
	//战队
	CT_VOID DisposeFightTeam(CT_CHAR *pBuf,CT_INT32 iLen);
	//城墙加成
	CT_VOID DisposeWallAddition(CT_CHAR *pBuf,CT_INT32 iLen);
	//掠夺
	CT_VOID DisposePlunder(CT_CHAR *pBuf,CT_INT32 iLen);
	//探访记录
	CT_VOID DisposeSeekData(CT_CHAR* pBuf,CT_INT32 iLen);
};
#endif