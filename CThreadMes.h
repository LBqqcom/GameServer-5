/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCDatabaseMes.h
Version:		1.0
Description:    数据线程通讯协议
Author:			pengwen
Create Date:	2012-8-16
History:        数据线程通讯协议     

******************************************************************/
#ifndef CTHREADMES_H_
#define CTHREADMES_H_

#include "CTData.h"
#include "CMD_Game.h"
#include "CUser.h" 
#include "CGlobeData.h"
#include "PVPInfo.h"

class CCNetInterface;
class CCUser;

//db线程主消息
//用户登录
#define  MSC_MAIN_DB_LOGIN 						(SYS_MSC_M_MIN + 200)
//建筑信息保存
#define  MSC_MAIN_DB_SAVE_BUILDING              (SYS_MSC_M_MIN + 201)
//升级完成
#define  MSC_MAIN_DB_UPGRAE_FINISH              (SYS_MSC_M_MIN + 204)  //需要优化
//保存数据
#define  MSC_MAIN_DB_SAVE_DATA                  (SYS_MSC_M_MIN + 205)  //需要优化
//删除数据
#define  MSC_MAIN_DB_DEL_DATA                   (SYS_MSC_M_MIN + 206)
//得到数据
#define  MSC_MAIN_DB_GET_DATA                   (SYS_MSC_M_MIN + 207)
//战斗奖励
#define  MSC_MAIN_DB_FIGHTING_REWARD			(SYS_MSC_M_MIN + 208)
//事件奖励
#define  MSC_MAIN_DB_BUILDING_EVENT             (SYS_MSC_M_MIN + 209)
// 道具数据更新 [10/10/2012 John]
#define  MSC_MAIN_DB_ITEM_UPDATE                (SYS_MSC_M_MIN + 210)
// 读取用户战斗数据
//#define  MSC_MAIN_DB_USER_BATTLE                (SYS_MSC_M_MIN + 211)
//区域数据更新
#define  MSC_MAIN_DB_REGION_UPDATE				(SYS_MSC_M_MIN + 220)
//军队数据
#define  MSC_MAIN_DB_ARMY_DATA					(SYS_MSC_M_MIN + 230)
//阵法数据
#define  MSC_MAIN_DB_BATTLE_ARRAY_DATA			(SYS_MSC_M_MIN + 240)
//任务数据更新
#define  MSC_MAIN_DB_TASK_UPDATE				(SYS_MSC_M_MIN + 250)
//竞技场数据
#define  MSC_MAIN_DB_PVP_DATA					(SYS_MSC_M_MIN + 260)
//商店数据
#define	 MSC_MAIN_DB_MARKET_DATA				(SYS_MSC_M_MIN + 270)
//商店数据（城市）
#define  MSC_MAIN_MARKETDATA                    (SYS_MSC_M_MIN + 271)                 

//宠物数据
#define  MSC_MAIN_DB_PET_DATA					(SYS_MSC_M_MIN + 280)
//邮件数据
#define  MSC_MAIN_DB_EMAIL_DATA					(SYS_MSC_M_MIN + 290)
//用户退出游戏
#define  MSC_MAIN_DB_USER_EXIT                  (SYS_MSC_M_MIN + 300)
//vip数据
#define  MSC_MAIN_DB_VIP_DATA					(SYS_MSC_M_MIN + 310)
// 充值数据
#define  MSC_MAIN_DB_CHARGE_DATA                (SYS_MSC_M_MIN + 320)
//日志数据库
#define  MSC_MAIN_LOGDB_DATA					(SYS_MSC_M_MIN + 330)     //需要优化
//用户登录信息插入
#define  MSC_MAIN_INSERT_USERDATA				(SYS_MSC_M_MIN + 340)
//活动数据
#define  MSC_MAIN_ACTIVE_EVENT_DATA				(SYS_MSC_M_MIN + 350)
//祭祀数据
#define  MSC_MAIN_WORSHIP_EVENT_DATA			(SYS_MSC_M_MIN + 360)
//好友数据
#define  MSC_MAIN_DB_FRIEND_DATA				(SYS_MSC_M_MIN + 370)
//好友进程间通讯
#define  MSC_MAIN_FRIEND_EVENT_DATA				(SYS_MSC_M_MIN + 380)
//战队数据
#define  MSC_MAIN_DB_FIGHTTEAM_DATA				(SYS_MSC_M_MIN + 390)
//战队进程间通讯
#define  MSC_MAIN_FIGHTTEAM_EVENT				(SYS_MSC_M_MIN + 400)
//活动数据
#define  MSC_MAIN_DB_ACTIVE_DATA				(SYS_MSC_M_MIN + 410)
//元素塔数据
#define  MSC_MAIN_DB_TOWER_DATA					(SYS_MSC_M_MIN + 420)
//公告
#define MSC_MAIN_DB_ANNOUNCEMENT				(SYS_MSC_M_MIN + 430)
//全局数据
#define MSC_MAIN_DB_GLOBEDATA					(SYS_MSC_M_MIN + 440)

#define MSC_MAIN_DB_UPDATEDATA					(SYS_MSC_M_MIN + 450)

//排行榜
#define MSC_MAIN_TOP_INFO                       (SYS_MSC_M_MIN + 460)
//排行榜db
#define MSC_MAIN_TOP_DB_INFO                    (SYS_MSC_M_MIN + 470)  
//世界BOSS
#define MSC_MAIN_DB_WORLDBOSS                   (SYS_MSC_M_MIN + 480)  

#define MSC_MAIN_TASK_EVENT_DATA				(SYS_MSC_M_MIN + 490)  
//跨服战
#define MSC_MAIN_DB_CORSS_FIGHT					(SYS_MSC_M_MIN + 500)

//龙岛
#define MSC_MAIN_DB_DRAGONISLAND				(SYS_MSC_M_MIN + 510)

//探访
#define MSC_MAIN_DB_SEEK						(SYS_MSC_M_MIN + 520)

//////////////////////////////////////////////////////////////////////////
// 用户线程主消息
// 道具数据更新
#define  MSC_MAIN_USR_ITEM_UPDATE               (SYS_MSC_M_MIN + 101)
//战斗事件[10/23/2012 wangchen]
#define  MSC_MAIN_FIGHTING_EVENT				(SYS_MSC_M_MIN + 102)
//副本事件[10/24/2012 wangchen]
#define  MSC_MAIN_DUPLICATE_EVENT				(SYS_MSC_M_MIN + 103)
//城市事件[11/6/2012 wangchen]
#define  MSC_MAIN_EVENT_CITY					(SYS_MSC_M_MIN + 104)
//竞技场[11/22/2012 wangchen]
#define  MSC_MAIN_EVENT_PVP			  			(SYS_MSC_M_MIN + 105)
//宠物
#define	 MSC_MAIN_EVENT_PET						(SYS_MSC_M_MIN + 106)
//用户退出游戏
#define  MSC_MAIN_USER_EXIT						(SYS_MSC_M_MIN + 107)
//阵形信息
#define  MSC_MAIN_BATTLE_ARRAY					(SYS_MSC_M_MIN + 108)
//广播
#define  MSC_MAIN_BROADCAST                     (SYS_MSC_M_MIN + 109)
//组队副本
#define  MSC_MAIN_TEAMDUPLICATE                 (SYS_MSC_M_MIN + 110)
//资源争夺
#define  MSC_MAIN_RES_OCCUPY                    (SYS_MSC_M_MIN + 111)
//元素塔进程通讯
#define MSC_MAIN_TOWER_EVENT				    (SYS_MSC_M_MIN + 112)
//公告更新
#define MSC_MAIN_ANNOUNCEMENT_UPDATA			(SYS_MSC_M_MIN + 113)


//领主消息
#define MSC_MAIN_MASTER                         (SYS_MSC_M_MIN + 114)
//用户登入消息
#define MSC_MAIN_USER_ENTER                     (SYS_MSC_M_MIN + 115)
//资源争夺
#define  MSC_MAIN_WORLD_BOSS                    (SYS_MSC_M_MIN + 116)
//跨服战
#define MSC_MAIN_CORSS_FIGHT					(SYS_MSC_M_MIN + 117)

//龙岛
#define MSC_MAIN_ISLAND_EVENT					(SYS_MSC_M_MIN + 118)
//探访
#define MSC_MAIN_SEEK							(SYS_MSC_M_MIN + 119)
//////////////////////////////////////////////////////////////////////////
//线程子消息
//请求数据库服务消息
//用户进入
#define  MSC_SUB_DB_USRE_ENTER					(10)
//排行榜信息
#define  MSC_SUB_DB_TOP_INFO					(12)
//PVP对手信息
#define	 MSC_SUB_DB_CHALLENG					(13)
//查看他人信息
#define MSC_SUB_DB_ANOTHER						(14)
//获取战队成员战斗数据
#define MSC_SUB_DB_FTFightingData				(15)
//保存领主数据
#define  MSC_SUB_DB_MASTER_DATA                 (20)
//保存领主能力冷却信息
#define  MSC_SUB_DB_MASTER_COOLDOWN             (21)

#define  MSC_SUB_DB_MASTER_DATA_EX              (22)
//保存领主战斗数据
#define  MSC_SUB_DB_MASTER_FIGHT_DATA           (23)
//生产同步
#define  MSC_SUB_DB_SYS_PROPDUCTING             (29)
//手动建筑生产数据
#define  MSC_SUB_DB_PRODUCTING_DATA             (30)
//更新策划奖励道具领取状态
#define  MSC_SUB_DB_PRESENT						(31)


// Begin [11/29/2012 wangchen]
//保存购买体力消耗
#define  MSC_SUB_DB_BUY_STATMINA                (32)
// end [11/29//2012 wangchen]

// Begin [9/20/2012 John]
// 保存潜能信息
#define  MSC_SUB_DB_W_TALENT_INFO               (40)
// 保存战旗信息
#define  MSC_SUB_DB_W_BATTLEFLAG_INFO           (41)
// 保存技能信息
#define  MSC_SUB_DB_W_SKILL_INFO                (50)
// 同步元素信息
#define  MSC_SUB_DB_W_ELM_SYNC                  (55)
// 同步元素属性信息
#define  MSC_SUB_DB_W_ELMATTR_SYNC              (56)
// 同步装备信息
#define  MSC_SUB_DB_W_EQUIP_SYNC                (57)
// 同步道具信息
#define  MSC_SUB_DB_W_ITEM_SYNC                 (58) 
// 同步增益信息
#define  MSC_SUB_DB_W_BUFF_SYNC                 (59)
// 修改领主名称
#define  MSC_SUB_DB_W_MASTER_NAME               (60)
// 更新连续签到
#define  MSC_SUB_DB_W_SIGN_INFO                 (110)
// 清除连续登陆记录 
#define  MSC_SUB_DB_D_SIGN_DATA                 (111)
// 更新日常任务进度
#define  MSC_SUB_DB_W_DAILY_QUEST               (120)
// 清除每日记录
#define  MSC_SUB_DB_D_DAILY_DATA                (121)
// 更新活跃度信息
#define  MSC_SUB_DB_W_ACTIVE_INFO               (130)
// 更新领取签到奖励
#define  MSC_SUB_DB_W_SIGN_REWARD               (140)
// 更新领取活跃度奖励
#define  MSC_SUB_DB_W_ACTIVE_REWARD             (150)
// End [9/20/2012 John]

//PVP非在线对手信息
#define  MSC_SUB_CHALLENGE						(160)
//排行榜信息
#define  MSC_SUB_TOP							(161)
//战报记录
#define  MSC_SUB_RECTORD						(162)
// 更新附魔信息
#define  MSC_SUB_DB_W_ENCHANT_DATA              (170)
// 更新附魔失败次数信息
#define  MSC_SUB_DB_W_ENCHANT_FAILTIME			(171)
// 更新附魔奖励信息
#define  MSC_SUB_DB_W_ENCHANT_REWARD			(172)
//组队副本离开
#define  MSC_SUB_TEAMDUPLICATE_LEAVE			(180)
//组队副本战斗结束
#define  MSC_SUB_TEAMDUPLICATE_FIGHTINGEND		(181)
//组队副本战斗
#define MSC_SUB_TEAMDUPLICATE_FIGHTING			(182)
//组队副本自动匹配
#define MSC_SUB_TEAMDUPLICATE_AUTOMATCH			(183)
//组队副本自动匹配信息
#define MSC_SUB_TEAMDUPLICATE_AUTOMATCHINFO		(184)
// 更新头衔信息
#define MSC_SUB_DB_W_TITLE_DATA					(185)					

// End [11/15/2012 John]
// 失去增益
#define  MSC_SUB_DB_LOSE_BUFF                   (1000)

// 添加充值记录
#define  MSC_SUB_DB_ADD_CHARGE                  (2001)
// 更新充值记录
#define  MSC_SUB_DB_UPDATE_CHARGE               (2002)

//日志数据
//////////////////////////////////////////////////////////////////////////
//军队升级记录
#define  MSC_SUB_LOGDB_ARMYUPDATAGRADE			(3000)
//军队进阶
#define  MSC_SUB_LOGDB_ARMYUPDATARANK			(3001)
//购买背包格子记录
#define  MSC_SUB_LOGDB_BAGSLOTUPDATA			(3002)
//增加城市记录
#define  MSC_SUB_LOGDB_MASTERAREA				(3003)
//道具增强
#define  MSC_SUB_LOGDB_PROPADDSTRENGTHEN		(3004)
//道具使用完记录
#define  MSC_SUB_LOGDB_PROPFINISH				(3005)
//道具继承记录
#define  MSC_SUB_LOGDB_PROPINHERIT				(3006)
//道具增强
#define  MSC_SUB_LOGDB_PROPUPDATATRAIN			(3007)
//保存技能记录
#define  MSC_SUB_LOGDB_SAVESKILL				(3008)
//事件完成记录
#define	 MSC_SUB_LOGDB_SAVEEVENTFINISH			(3009)
//保存天赋记录
#define  MSC_SUB_LOGDB_SAVETALENT				(3010)
//保存任务完成记录
#define  MSC_SUB_LOGDB_TASKFINISH				(3011)
//用户退出记录
#define  MSC_SUB_LOGDB_USEREXIT					(3012)
//用户升级记录
#define  MSC_SUB_LOGDB_UPGRADEFINISH			(3013)
//vip申请记录
#define  MSC_SUB_LOGDB_VIPAPPLY					(3014)
//活跃度奖励
#define  MSC_SUB_LOGDB_ACTIVEREWARD				(3015)
//在线人数统计
#define  MSC_SUB_LOGDB_ONLINEUSER				(3016)
//用户聊天记录
#define  MSC_SUB_LOGDB_CHATTIME					(3017)
//用户生产记录
#define  MSC_SUB_LOGDB_PRODUCTFINISH			(3018)
//用户充值记录
#define	 MSC_SUB_LOGDB_CHARGE					(3019)
//用户充值更新
#define  MSC_SUB_LOGDB_CHARGEUPDATE				(3020)
//每日签到
#define  MSC_SUB_LOGDB_SIGNREWARD				(3021)
//PVP宝箱
#define  MSC_SUB_LOGDB_CHEST					(3022)
//宠物主动技能升级
#define  MSC_SUB_LOGDB_PETUPSKILL				(3023)
//宠物命名
#define  MSC_SUB_LOGDB_PETRENAME				(3024)
//物品出售
#define  MSC_SUB_LOGDB_ITEMSELL					(3025)
//商店购买记录
#define  MSC_SUB_LOGDB_SHOPPING					(3026)
//体力购买
#define  MSC_SUB_LOGDB_STATMINA					(3027)
//pvp奖励
#define  MSC_SUB_LOGDB_PVPREWARD				(3028)
//副本战斗奖励
#define  MSC_SUB_LOGDB_DUPLICATE				(3029)
// 附魔记录
#define  MSC_SUB_LOGDB_ENCHANT                  (3030)
//副本奖励领取
#define  MSC_SUB_LOGDB_DUPLICATEREWARD			(3031)
//副本进入
#define  MSC_SUB_LOGDB_DUPLICATEENTER			(3032)
//竞技场战斗消耗
#define  MSC_SUB_LOGDB_PVPFIGHTINGCOST			(3033)
//活动奖励
#define  MSC_SUB_LOGDB_ACTIVEEVENTREWARD		(3034)
//副本战斗次数购买
#define  MSC_SUB_LOGDB_BUYFIGHTINGTIE			(3035)
//祭祀奖励
#define MSC_SUB_LOGDB_WORSHIP_REWARD			(3036)
//强制刷新
#define MSC_SUB_LOGDB_PVPREFRESH				(3037)
//钻石减少强化冷却时间
#define MSC_SUB_LOGDB_CLEARCD					(3038)
//信息反馈
#define MSC_SUB_LOGDB_FEEDBACK					(3039)
//扫荡加速
#define MSC_SUB_LOGDB_RAIDSSPEED				(3040)
//组队副本奖励
#define MSC_SUB_LOGDB_TEAMDUPLICATEREWARD		(3041)
//组队副本消耗
#define MSC_SUB_LOGDB_TEAMDUPLICATECOST			(3042)
//战队
#define MSC_SUB_LOGDB_FIGHTTEAM					(3043)
//军衔俸禄
#define MSC_SUB_LOGDB_SALARY					(3044)
//军衔升级
#define MSC_SUB_LOGDB_RANKINGUPGRADE			(3045)
//物品使用记录
#define MSC_SUB_LOGDB_USESITEM					(3046)
//事件奖励记录
#define MSC_SUB_LOGDB_EVENTREWARD				(3047)
//宠物升级
#define MSC_SUB_LOGDB_PETUPGRADE				(3048)
//masterdatarecord
#define MSC_SUB_LOGDB_MASTER_DATA				(3049)
//战队升级
#define MSC_SUB_LOGDB_FIGHTTEAM_UPGRADE			(3050)
//战队召唤消耗
#define MSC_SUB_LOGDB_FIGHTTEAM_SUMMONCOST		(3051)
//战队召唤奖励资源
#define MSC_SUB_LOGDB_FIGHTTEAM_SUMMONREWARDRES	(3052)
//战队召唤奖励道具
#define MSC_SUB_LOGDB_FIGHTTEAM_SUMMONREWARDITEM (3053)
//战队科技升级
#define MSC_SUB_LOGDB_FIGHTTEAM_SCIENCE_UPGRADE	(3054)
//元素塔通关
#define MSC_SUB_LOGDB_TOWER_PASS				(3055)
#define MSC_SUB_LOGDB_TOWER_OCCUPY				(3056)
#define MSC_SUB_LOGDB_TOWER_HELP				(3057)
#define MSC_SUB_LOGDB_ADDSITEM					(3058)
#define MSC_SUB_LOGDB_BATTLEFLAG				(3059)
#define MSC_SUB_LOGDB_EATELM					(3060)
#define MSC_SUB_LOGDB_TREASUREUSE				(3061)
#define MSC_SUB_LOGDB_EMBEDEQUIP				(3062)

//跨服战
#define MSC_SUB_LOGDB_CSREWARD					(3063)
#define MSC_SUB_LOGDB_INSPORE_COST				(3064)
#define MSC_SUB_LOGDB_SUPPORT					(3065)

//头衔
#define MSC_SUB_LOGDB_TITLE						(3066)
//龙岛
#define MSC_SUB_LOGDB_ISLAND					(3067)
//城墙加成
#define MSC_SUB_LOGDB_WALLADDITION				(3068)
//掠夺
#define MSC_SUB_LOGDB_PLUNDER					(3069)
//探访记录
#define MSC_SUB_LOGDB_SEEKDATA					(3070)						

//db线程返回的子消息标志位
//数据库网络异常错误
#define  MSC_SUB_DB_R_NET_ERROR                 (10000)
//数据库库没有发现用户
#define  MSC_SUB_DB_R_NO_USER                   (10001)
//读取数据完成
#define  MSC_SUB_DB_R_ENTER_FINISH              (10005)





//登录消息
//领主信息
// 装备信息
#define  MSC_SUB_DB_R_EQUIP_INFO                (10011)

// End [9/20/2012 John]



// 用户线程子消息
// 添加道具
#define  MSC_SUB_USR_ADD_ITEM                     (11000)
//删除道具
#define  MSC_SUB_G_DELETE_ITEM					  (11001)
//战斗事件[10/23/2012 wangchen]
#define  MSC_SUB_FIGHTING_EVENT                   (20001)
//战斗结束[10/23/2012 wangchen]
#define  MSC_SUB_FIGHTING_END                     (20002)
//观看战斗[10/23/2012 wangchen]
#define  MSC_SUB_LOOK_FIGHTING                    (20003)
//事件战斗
#define  MSC_SUB_EVENT_FIGHTING					  (20004)
//事件战斗结束[11/6/2012 wangchen]
#define  MSC_SUB_EVENT_FIGHTING_END               (20005)
//竞技场
#define	 MSC_SUS_PVP_FIGHTING					  (20006)
//竞技场战斗结束
#define  MSC_SUB_PVP_FIGHTING_END                 (20007)
//PVP观看战斗
#define  MSC_SUB_PVP_LOOK_FIGHTING                (20008)
//副本入口完成
#define  MSC_SUB_DUPLICATE_FINISH			      (20009)
//对手信息
#define  MSC_SUB_PVP_CHALLENG					(20010)
//PVP战报信息
#define  MSC_SUB_PVP_RECORD						(20011)
//pvp插入低级对手
#define  MSC_SUB_PVP_LOW_CHALLER                (20012)
//pvp插入高级对手
#define  MSC_SUB_PVP_HIGN_CHALLER               (20013)
//pvp插入对手完成
#define  MSC_SUB_PVP_INSERT_CHALLER_FINISH      (20014)


//扫荡奖励
#define  MSC_SUB_RAIDS_REWARD					(20112)
//资源掠夺
#define MSC_SUB_RESREBBERY_TEAMPVP				(20113)
//资源掠夺
#define MSC_SUB_RESREBBERY_TEAMPVE				(20114)
//查看战队战斗PVP
#define MSC_SUB_FIGHTTEAM_PVP					(20115)
//查看战队战斗PVE
#define MSC_SUB_FIGHTTEAM_PVE					(20116)
//资源的活动开启
#define MSC_SUB_RESACTVIE_OPEN					(20117)
//资源点活动读取完成
#define MSC_SUB_RESACTVIE_READFINISH			(20118)
//元素塔关卡通关
#define MSC_SUB_TOWER_CP_PASS					(20119)
//元素塔关卡通关记录查看
#define MSC_SUB_TOWER_CPPASS_LOOK				(20120)

#define  MSC_SUB_FIGHTING_REWARD                (20121)
//资源的活动插入
#define  MSC_SUB_INSERT_RESACTIVE				(20122)

//元素塔用户之间资源抢夺
#define MSC_SUB_TOWER_RES_PVP                   (20150)
//元素塔PVP战斗查看
#define MSC_SUB_TOWER_PVP_LOOK                  (20151)
//战队BOSS
#define MSC_SUB_FT_SUMMON_BOSS					(20160)
//战队时限BOSS
#define MSC_SUB_FT_TIME_BOSS					(20161)
//战队时限BOSS离开
#define MSC_SUB_FT_TIME_BOSS_LEAVE				(20162)

//世界BOSS活动开启
#define MSC_SUB_BOSSACTVIE_OPEN					(20170)


//登录结束
//建造建筑
//同步建筑信息
#define  MSC_SUB_DB_SYS_BUILDING_UPGRADINT    (21002)
//保存建筑信息
#define  MSC_SUB_DB_SAVE_BUILDING             (21003)
//保持正在升级的建筑信息
#define  MSC_SUB_DB_SAVE_UPGRADING_BUILDING   (21004)
//升级完成
#define  MSC_SUB_DB_R_BUILDING_UPGRADING_FINISH (21006)
//建造结束

//建筑事件
#define  MSC_SUB_DB_SAVE_AWARD_DATA             (22000)
//事件结束
//区域更新记录
#define MSC_SUB_DB_REGION_RECORD_DATA			(23000)

//任务更新记录
#define MSC_SUB_DB_TASK_DATA					(24000)
#define MSC_SUB_DB_TASK_RECORD_DATA				(24001)

//////////////////////////////////////////////////////////////////
//商店数据库记录
#define MSC_SUB_DB_MARKET_RECORD_DATA			(25000)
//用户黑店,商城功能开启
#define MSC_SUB_DB_MARKET_ADD				    (25001)
//黑店信息刷新
#define MSC_SUB_DB_BLACK_MARKET_REFRESH			(25003)
//黑店购买
#define MSC_SUB_DB_BLACK_MARKET_SHOPPING		(25004)
//个人限购
#define MSC_SUB_DB_MARKET_PERSONAL				(25005)
//个人限购刷新
#define MSC_SUB_DB_REFRESH						(25006)
//个人限购信息
#define MSC_SUB_UPDATE_PERSON                   (25007)                 

////////////////////////////////////////////////////////////////
//宠物获得
#define MSC_SUB_DB_PET_INFO						(26000)
//得到幻化
#define MSC_SUB_DB_PET_CHANGE					(26001)
//宠物蛋使用
#define MSC_SUB_USE_EGG							(26101)

/////////////////////////////////////////////////////////////////
//邮件数据
//用户邮件记录
#define MSC_SUB_DB_EMAIL_USER					(27001)


// Begin [10/20/2012 wangchen]
//世界场景信息
#define MSC_SUB_DB_R_DUPLICATE_WORLD_SCENE		(30001)
//场景信息
#define MSC_SUB_DB_R_DUPLICATE_SCENE_INFO		(30002)
//战斗信息
#define MSC_SUB_DB_W_FIGHTING_INFO				(30003)
//购买战斗次数
#define MSC_SUB_DB_W_BUYFIGHTINGTIME			(30004)
//扫荡信息
#define MSC_SUB_DB_RAIDS_INFO					(30005)
//扫荡奖励
#define MSC_SUB_DB_RAIDS_REWARD					(30006)
//组队副本开启
#define MSC_SUB_DB_TEAMDUPLICATE_SCENE			(30007)
//组队副本自动匹配
#define MSC_SUB_DB_TEAMDUPLICATE_AUTOMATCH		(30008)

//军队信息
//#define MSC_SUB_DB_R_ARMY_INFO					(31001)
//军队升级
#define MSC_SUB_DB_R_ARMY_UPGRADE				(31002)
//军队进阶
#define MSC_SUB_DB_R_ARMY_ADVANCED				(31003)
//军队信息
#define MSC_SUB_DB_ARMY_INFO					(31004)
//进阶更新阵法
#define MSC_SUB_UPDATA_BATTLE_ARRAY				(31005)

//阵法信息
//#define MSC_SUB_DB_BATTLE_ARRAY_INFO			(32001)
//阵法信息
#define MSC_SUB_DB_W_BATTLE_ARRAY_INFO			(32002)

//竞技场信息
#define MSC_SUB_DB_R_PVP_INFO					(33001)
//刷新对手
#define MSC_SUB_DB_W_REFRESH					(33004)
//战报记录
#define MSC_SUB_DB_PVP_FIGHTING_RECORD			(33008)
//读取用户信息
#define MSC_SUB_DB_R_USER_INFO					(33009)
//战斗记录
#define MSC_SUB_DB_FIGHTING_END					(33013)
//挑战状况
#define MSC_SUB_DB_CHALLENGE_INFO				(33015)
//竞技场终结
#define MSC_SUB_DB_PVPENDING					(33016)
//竞技场对手数据写到数据库
#define MSC_SUB_DB_PVP_CHALLGER_SAVE            (33017)

//vip礼包领取
#define MSC_SUB_DB_GIFT_GET						(34001)
//vip续费
#define  MSC_SUB_DB_VIP_RENEWAL					(34002)
//vip到期
#define MSC_SUB_DB_VIP_END						(34003)
// End [10/20/2012 wangchen]

//用户登录数据插入
////////////////////////////////////////////////////////////
//领主信息
#define  MSC_SUB_INSERT_MASTERDATA				(35000)
//得到正在升级建筑的信息
#define  MSC_SUB_INSERT_UPGRADEBUILD			(35001)
//得到潜能数据
#define  MSC_SUB_INSERT_TALENTINFO				(35002)
//得到道具数据
#define  MSC_SUB_INSERT_ITEMINFO				(35003)
//得到装备信息
#define	 MSC_SUB_INSERT_EQUIPINFO				(35004)
//得到增益数据
#define  MSC_SUB_INSERT_BUFFINFO				(35005)
//得到冷却信息
#define  MSC_SUB_INSERT_COOLDOWNINFO			(35006)
//得到军队信息
#define  MSC_SUB_INSERT_ARMYINFO				(35007)
//得到正在生产的建筑
#define  MSC_SUB_INSERT_PRODUCTING				(35008)
//得到世界场景数据
#define  MSC_SUB_INSERT_WORLDSCENE				(35009)
//得到场景数据
#define  MSC_SUB_INSERT_SCENEINFO				(35010)
//得到用户解锁区域
#define  MSC_SUB_INSERT_REGIONINFO				(35011)
//得到阵法信息
#define  MSC_SUB_INSERT_BATTLEARRAY				(35012)
//得到用户已完成任务
#define  MSC_SUB_INSERT_TASKFINISH				(35013)
//得到用户在进行任务
#define  MSC_SUB_INSERT_TASKPORGRESS			(35014)
//得到日常任务进度信息
#define  MSC_SUB_INSERT_DAILYQUEST				(35015)
//得到活跃度信息
#define  MSC_SUB_INSERT_DAILYACTIVE				(35016)
//得到建筑信息
#define  MSC_SUB_INSERT_BUILDINGINFO			(35017)
//得到商店状态
#define  MSC_SUB_INSERT_MARKETFUNCTION			(35018)
//获取黑店信息
#define  MSC_SUB_INSERT_BLACKMARKETINFO			(35019)
//得到宠物信息
#define  MSC_SUB_INSERT_PETINFO					(35020)
//获取vip信息
#define  MSC_SUB_INSERT_VIPINFO					(35021)
//竞技场信息
#define  MSC_SUB_INSERT_PVPINFO					(35022)
//PVP对手信息
#define  MSC_SUB_INSERT_PVPCHALLENG				(35023)
//排行榜信息
#define  MSC_SUB_INSERT_TOP					(35024)
//pvp战报记录
#define  MSC_SUB_INSERT_PVPRECORD				(35025)
//得到技能数据
#define  MSC_SUB_INSERT_SKILLDATA				(35026)
//得到签到信息
#define  MSC_SUB_INSERT_SIGNINFO				(35027)
//获取附魔信息
#define  MSC_SUB_INSERT_ENCHANTDATA				(35028)
//得到购买战斗次数
#define  MSC_SUB_INSERT_BUYFIGHTINGTIME			(35029)
//扫荡信息
#define  MSC_SUB_INSERT_RAIDSINFO				(35030)
//奖励信息
#define  MSC_SUB_INSERT_RAIDSREWARD				(35031)
//好友信息
#define  MSC_SUB_INSERT_FRIENDINFO				(35032)
//组队副本信息
#define  MSC_SUB_INSERT_TEAMDUPLICATE			(35033)
//自动领取运营道具奖励
#define  MSC_SUB_INSERT_PRESENT					(35034)
//个人限购
#define  MSC_SUB_INSERT_MARKETPERSONAL			(35035)
//元素之力
#define  MSC_SUB_INSERT_ELMPOWER                (35036)
//战旗
#define  MSC_SUB_INSERT_BATTLEFLAG              (35037)
// 元素属性
#define  MSC_SUB_INSERT_ELMATTR                 (35038)
//元素之塔
#define  MSC_SUB_INSERT_TOWER					(35039)
#define  MSC_SUB_INSERT_TOWERSLOTREWARD			(35040)
#define  MSC_SUB_INSERT_MSG						(35041)
//战队科技
#define  MSC_SUB_INSERT_FIGHTTEAM_SCIENCE		(35042)
#define  MSC_SUB_INSERT_TOWEREVENT				(35043)
#define  MSC_SUB_INSERT_TOWERSLOTFIGHTING		(35044)

#define  MSC_SUB_INSERT_TOWERHELP				(35045)
//附魔奖励
#define  MSC_SUB_INSERT_ENCHANT_REWARD			(35046)
#define  MSC_SUB_INSERT_BOSS_RECORD				(35047)
//得到宠物幻化
#define  MSC_SUB_INSERT_PETCHANGE				(35048)
//头衔
#define  MSC_SUB_INSERT_TITLE					(35049)
//龙岛
#define  MSC_SUB_INSERT_ISLANDDATA				(35050)
#define  MSC_SUB_INSERT_ISLAND_GIVEUID			(35051)
#define  MSC_SUB_INSERT_ISLAND_SCOREREWARD		(35052)
#define  MSC_SUB_INSERT_ISLAND_PRESENTNAME		(35053)
//探访
#define  MSC_SUB_INSERT_SEEK_WALLADDITION		(35054)
#define  MSC_SUB_INSERT_SEEK_WALLARMY			(35055)
#define  MSC_SUB_INSERT_SEEK_RANDPLAYER			(35056)
#define  MSC_SUB_INSERT_HAD_SEEKEDPLAYER		(35057)
#define  MSC_SUB_INSERT_SEEKED_PLAYERINFO		(35058)
#define  MSC_SUB_INSERT_SEEK_TEAMFLAG			(35059)
#define  MSC_SUB_INSERT_SEEK_SAINTESSKIDNAP		(35060)
#define  MSC_SUB_INSERT_SEEK_WORKERKIDNAP		(35061)
#define  MSC_SUB_INSERT_SEEK_WILLLIST			(35062)
#define  MSC_SUB_INSERT_SEEK_SPITELIST			(35063)
#define  MSC_SUB_INSERT_SEEK_DATA				(35064)
#define  MSC_SUB_INSERT_SEEK_VISITORINFO		(35065)
#define  MSC_SUB_INSERT_SEEK_FIGHTRECORD		(35066)
#define  MSC_SUB_INSERT_MASTGER_FIGHTADD		(35067)
#define  MSC_SUB_INSERT_SEEK_OTHERSINFO			(35068)
#define  MSC_SUB_INSERT_SEEK_PETKIDNAP			(35069)
///////////////////////////////////////////////////////////////////////////
//活动 信息
#define  MSC_SUB_ACTIVE_STATUS_UPDATE			(36001)
//服务器启动时候需要读取活动信息、
//#define  MSC_SUB_ACTIVE_INFO_GET				(36002)
#define  MSC_SUB_TURNTABLE_UPDATE				(36003)
#define  MSC_SUB_CDKEY_GET						(36004)
#define  MSC_SUB_CDKEY_UPDATE					(36005)
#define  MSC_SUB_RES_PRODUCT_REWARD				(36006)
#define  MSC_SUB_PRESEND_ADD					(36007)
#define  MSC_SUB_ACTIVE_GET                     (36006)
#define  MSC_SUB_ACTIVE_DB_SYS                  (36007)
#define  MSC_SUB_ACTIVE_DSC_GET                 (37008)
#define  MSC_SUB_TURNTABLE_REL			        (36009)
#define  MSC_SUB_ACTIVE_REWARD_REL		        (36010)
#define  MSC_SUB_ACTIVE_REWARD_COMPLETE			(36011)
#define  MSC_SUB_ACTIVE_STATUS_UPDATEEX			(36012)

//////////////////////////////////////////////////////////////////////////
//祭祀信息
#define MSC_SUB_WORSHIP_STATUS_UPDATE			(37001)
//////////////////////////////////////////////////////////////////////////


//好友信息
#define MSC_SUB_DB_FRIEND_UPDATE					(38001)
#define MSC_SUB_DB_FRIEND_GET						(38002)
#define MSC_SUB_DB_FRIEND_DELETE					(38003)
#define MSC_SUB_DB_FRIEND_FIND						(38004)
#define MSC_SUB_FRIEND_REQUST_S						(38005)
#define MSC_SUB_FRIEND_ACCEPT_S						(38006)
#define MSC_SUB_FRIEND_REJECT_S						(38007)
#define MSC_SUB_FRIEND_BLACK_S						(38008)
#define MSC_SUB_DB_FRIEND_FIND_BLACK				(38009)
#define MSC_SUB_FRIEND_DELETE_S						(38010)
#define MSC_SUB_FRIEND_EXIT_EVENT					(38011)
#define MSC_SUB_DB_FRIEND_GET_TITLE					(38012)
#define MSC_SUB_FRIEND_SEND_UNLINK					(38013)					

#define MSC_SUB_DB_FIGHTTEAM_UPDATE					(39001)
#define MSC_SUB_DB_FIGHTTEAM_DELETE					(39002)
#define MSC_SUB_DB_FIGHTTEAM_FIND					(39003)
#define MSC_SUB_DB_FIGHTTEAMMEMBER_UPDATE			(39004)
#define MSC_SUB_DB_FIGHTTEAMMEMBER_DELETE			(39005)
#define MSC_SUB_DB_FIGHTTEAMMEMBER_FIND				(39006)
#define MSC_SUB_DB_FIGHTTEAM_RECRUIT_UPDATE			(39007)
#define MSC_SUB_DB_FIGHTTEAM_RECRUIT_DELETE			(39008)
#define MSC_SUB_DB_FIGHTTEAMFIGHTINGDATA			(39009)
#define MSC_SUB_DB_FIGHTTEAM_EVENT_SAVE				(39010)
#define MSC_SUB_DB_FIGHTTEAM_MEM_REFRESH			(39019) //彭文添加 解决战队刷新
#define MSC_SUB_FIGHTTEAM_SUMMON_FIGHTING			(39020)
#define MSC_SUB_FIGHTTEAM_SUMMON_LEAVE				(39021)
#define MSC_SUB_DB_FIGHTTEAM_SCIENCE_UPGRADE		(39022)
#define MSC_SUB_DB_FIGHTTEAM_SUMMON_DATA			(39023)
#define MSC_SUB_DB_FIGHTTEAM_SUMMON_ITEM			(39024)
#define MSC_SUB_DB_FIGHTTEAM_DOWN_GRADE				(39025)
#define MSC_SUB_FIGHTTEAM_TOWER_CP					(39026)
#define MSC_SUB_FINGTTEAM_UPGRADE_TASK				(39027)
#define	MSC_DB_CS_INSPORE_UP						(39028)//威能升级
#define	MSC_DB_CS_READ_INSPORE						(39029)//读取个人威能
#define	MSC_SUB_CHANGE_FTNAME						(39030)//修改战队名称
#define	MSC_SUB_CLEAR_TIMEBOSS						(39031)//清理时限boss
#define	MSC_SUB_TIMEBOSS_INFO						(39032)//时限boss信息
#define	MSC_SUB_TIMEBOSS_MUMBER						(39033)//时限boss成员


//资源争夺
#define MSC_SUB_DBLOG_RES_PRODUCT_SAVE              (40000)  //资源生产
#define MSC_SUB_DBLOG_RES_OCCUPY_SAVE               (40001)  //占领消耗
#define MSC_SUB_DB_SYS_EXIT                         (40003)  //系统退出（数据库线程使用）
#define MSC_SUB_DB_RES_ACTIVE						(40004)	 //读取活动资源的信息
#define MSC_SUB_DB_RES_SAVE                         (40005)  //保存资源占领信息
#define MSC_SUB_SYS_EXIT                            (40006)  //系统退出 （逻辑线程使用）

//元素塔
#define MSC_SUB_DB_TOWER_UPDATE						(41001) 
#define MSC_SUB_DB_TOWER_OFFLINE_DATA               (41002)  //读取离线数据
#define MSC_SUB_DB_TOWER_SLOT_REWARD                (41003)  //生产的奖励
#define MSC_SUB_DB_TOWER_SLOT_REWARD_DEL            (41004)  //生产的奖励
#define MSC_SUB_DB_TOWER_MSG_ADD					(41005)
#define MSC_SUB_DB_TOWER_MSG_DEL					(41006)
#define MSC_SUB_DB_TOWER_FIGHT						(41007)
#define MSC_SUB_DB_TOWERSLOT_FIGHT					(41008)
#define MSC_SUB_DB_TOWER_EVENT						(41009)
#define MSC_SUB_DB_TOWER_HELP						(41010)

//世界BOSS
#define MSC_SUB_DB_BOSS_TOPCLEAR					(41051)
#define MSC_SUB_DB_BOSS_TOPADD						(41052)
#define MSC_SUB_DB_BOSS_RECORDADD					(41053)

//全局数据
#define MSC_SUB_DB_GLOBEDATA						(42000)	 //全局数据
#define MSC_SUB_DB_GLOBEDATA_READ                   (42001)  //全局数据读取，红米，小米3
#define MSC_SUB_DB_UPDATA_DATA						(43000)

         
//排行榜
#define MSC_SUB_DB_TOP_GET                          (43100)  //得到排行信息
#define MSC_SUB_DB_TOP_SAVE                         (43101)  //保存排行信息
#define MSC_SUB_DB_TOP_REWARD						(43102)  //更新排行榜奖励

#define MSC_SUB_TOP_MASTER_INFO                     (43200)  //得到领主信息
#define MSC_SUB_TOP_MASTER_FINISH                   (43201)  //领主信息得到
#define MSC_SUB_TOP_UPDATE                          (43202)  //排行更新
#define MSC_SUB_TOP_FIGHTING						(43203)	//排行榜战斗


//广播
#define MSC_SUB_BROADCAST_WORLD                     (43300) //世界聊天
#define MSC_SUB_BROADCAST_SYSTEM                    (43301) //系统广播
#define MSC_SUB_BROADCAST_GM                        (43302) //gm广播


//领主
#define  MSC_SUB_CHECK_OTHER_FINISH                 (43400) //领主查看别的用户数据

#define  MSC_SUB_CHARGE_RESEND						(43401) //重连充值服务时重发所有订单


//用户进入
#define  MSC_SUB_ENTER_ACTIEVE_ADD                  (43500) //活动添加
#define  MSC_SUB_ENTER_TRUNTABLE_ADD                (43501) //转盘添加
#define  MSC_SUB_ENTER_WORSHIP_ADD                  (43502) //祭祀添加

//世界boss
#define  MSC_SUB_WORLDBOSS_FINGHTING				(44001)	//世界boss
#define  MSC_SUB_WORLDBOSS_LOGIN					(44002)	//世界boss用户登入

//任务
#define  MSC_SUB_TASK_FIGHTTEAM_EVENT				(44101)

//用户数据
#define  MSC_SUB_USER_DATA_INSERT					(45000)	//用户数据插入
#define  MSC_SUB_USER_SEEKDATA_INSERT				(45001) // 用户被探访数据插入

//跨服战
#define  MSC_SUB_CS_ACTIVE_BEGIN					(46000)//跨服战活动
#define  MSC_SUB_CS_FIGHTING						(46001)//跨服战斗
#define	 MSC_SUB_CS_FINAL_EIGHT						(46002)//8强战斗
#define  MSC_SUB_CS_FIGHT_RECORD					(46003)//查看战报
#define  MSC_SUB_CS_ACTIVE_FIGHTING					(46004)//活动开始战斗
#define  MSC_SUB_DB_CS_REWARD						(46005)//奖励记录
#define	 MSC_SUB_DB_CS_SUPPORT						(46006)//个人支持
#define	 MSC_SUB_DB_DELREWARD						(46007)//删除奖励

//龙岛
#define  MSC_SUB_ISLAND_FIGHT						(47000)	//龙岛战斗事件
#define  MSC_SUB_DB_ISLAND_UPDATE					(47001) //龙岛数据
#define  MSC_SUB_DB_ISLAND_GIVEUID					(47002)	//赠送次数的uid
#define  MSC_SUB_DB_ISLAND_SCOREREWARD				(47003) //已获得的战队积分宝箱
#define  MSC_SUB_DB_ISLAND_GIVETIMES				(47004) //给离线队友赠送次数
#define  MSC_SUB_GIVETIMES_OUTLINE					(47005) //离线赠送次数失败
#define  MSC_SUB_UPDATE_FTMEMBER_SCORE				(47006) //更新战队成员的积分
#define  MSC_SUB_OPEN_ISLAND						(47007) //开启龙岛
#define  MSC_SUB_DB_ISLAND_ONCLOCK					(47008)	//龙岛定时刷新
#define  MSC_SUB_TASK_COMPLETE						(47009)	//完成任务

//得到头衔列表
#define  MSC_SUB_TITLELIST_GET						(48000) //起服时得到头衔列表
#define  MSC_SUB_TITLELIST_UPDATE					(48001)	//0点得到头衔列表

//探访
#define	 MSC_SUB_DB_WALL_ADDITION					(49001)	//城墙加成
#define	 MSC_SUB_DB_WALL_ARMY						(49002)	//城墙军队
#define  MSC_SUB_DB_GET_OTHERSTOWN					(49003)	//得到他人的城镇信息
#define  MSC_SUB_KIDNAP_FIGHT						(49004)	//绑架战斗
#define  MSC_SUB_RESCUE_FIGHT						(49005)	//营救战斗
#define  MSC_SUB_DB_GET_SEEKED						(49006) //得到被访者信息
#define  MSC_SUB_GET_SEEKED_RETURN					(49007) //返回处理被访者信息
#define  MSC_SUB_SEEKED_TIMES_MINUS					(49008) //被访者的被访问次数处理
#define  MSC_SUB_PLUNDER_FIGHT						(49009) //掠夺战斗
#define  MSC_SUB_SEEK_TIMES_UPDATE					(49010)	//更新访问次数
#define  MSC_SUB_SEEKED_PLAYER_UPDATE				(49011) //已探访状态更新
#define  MSC_SUB_SEEK_LIST_UPDATE					(49012) //探访列表更新
#define  MSC_SUB_SEEKED_DATA_UPDATE					(49013) //探访者基本信息更新
#define  MSC_SUB_TEAM_FLAG_UPDATE					(49014) //更新旗帜
#define  MSC_SUB_SEEK_PRESENT_UPDATE				(49015) //更新礼物信息
#define  MSC_SUB_SEEK_DATA_UPDATE					(49016) //更新探访者基本信息
#define  MSC_SUB_TEAM_FLAG_DELETE					(49017) //删除旗帜
#define  MSC_SUB_KIDNAP_UPDATE						(49018) //插入人质
#define  MSC_SUB_KIDNAP_DELETE						(49019) //删除人质
#define  MSC_SUB_SEEK_PRESENT_DEL					(49020) //删除礼物
#define  MSC_SUB_SEEK_VISTORINFO					(49021) //访客记录
#define  MSC_SUB_SEEK_LOOK_FIGHTING					(49022) //查看战斗
#define  MSC_SUB_DB_SEEK_FIGHTING_RECORD			(49023) //战斗记录
#define  MSC_SUB_DB_PLUNDER_RES_UPDATE				(49024)	//更新掠夺的资源

#  pragma pack(push, 4)   /* 通讯协议结构体请定义在此行之后 */
//用户进入信息
struct  MSC_DB_UserEnter
{
	CT_DWORD          dwUserID;        //用户id
	CT_DWORD          dwDstUserID;     //目标ID
	CCNetInterface*   pNetInterface;   //用户通讯接口
	CT_DWORD          dwThreadID;      //线程id		
	CT_DWORD		  dwSubID;			//
};

//用户退出
struct MSC_DB_UserExit
{
	CT_DWORD          dwUserID;       //用户id
	CT_INT64          llTimeNow;      //当前时间
};


//对手信息
struct MSC_DB_Challeng
{
	CT_DWORD		  dwUserID;
	CT_DWORD		  dwChalleng[PVP_CHALLENGE_COUNT];

};

//对手信息
struct MSC_DB_ChallengData
{
	CT_DWORD			dwUserID;
	UserFightingStatus  UserFightinginfo[PVP_CHALLENGE_COUNT];//用户战斗信息
	LookUserInfo		LookUserinfo[PVP_CHALLENGE_COUNT];	//查看用户信息
	
};

//战报记录
struct  MSC_DB_PVPRecord
{
	CT_DWORD		dwUserID;
	CT_DWORD		dwChallengID[PVP_FIGHTING_COUNT];		
};



//建筑建造完成
struct MSC_DB_R_BuildUpgradeFinish 
{
	CT_DWORD            dwUserID;             //用户id
	CT_INT32            iBID;                 //建筑id
	CT_INT32            iGrade;               //建筑等级
};


//建筑同步信息
struct MSC_DB_SysBuildingUpgrading
{
	CT_DWORD           dwUserID;            //用户id
};

//保存建筑信息
struct MSC_DB_BuildingInfo
{
	CT_DWORD           dwUserID;             //用户id
	CT_INT32           iBid;                 //建筑id
	CT_INT32           iGrade;               //建筑等级
};

//保存建筑升级升级信息
struct MSC_DB_BuildingUpgradingInfo
{
	CT_DWORD            dwUserID;             //用户id
	CT_INT32			iID;					//建筑id
	CT_LONGLONG			llBeginTime;			//开始升级时间
	//CT_BOOL             bSpeedUpTime;           //加速次数（只有可能是0和1）
	CT_INT32            iNeedTime;              //需要的时间
	G_BuildingUpgrade   buildingUpgrade;        //升级所需数据记录
};







//db用户基本信息
struct MSC_DB_R_BaseInfo
{
	CT_DWORD			dwUserID;         //用户id
	CCNetInterface*		pNetInterface;    //用户的通讯接口
};

//组队副本自动匹配
struct MSC_DB_TeamDuplicateUserInfo
{
	CT_DWORD			dwUserID;         //用户id
	UserFightingStatus  UserFightinginfo[TEAM_NUM-1];//用户战斗信息
	LookUserInfo		LookUserinfo[TEAM_NUM-1];	//查看用户信息
};

//db建筑基本信息
struct MSC_DB_R_BuildingBaseInfo : public MSC_DB_R_BaseInfo
{
	CT_INT32            iBuildID;
};


//保存用户数据
struct MSC_DB_MasterSaveData
{
	CT_DWORD             dwUserID;             //用户id
	// 适应数据结构修改 [9/11/2012 John]
	MasterVariableData mvd;
   // CT_INT32           iBagSlots;
	CT_INT64           llExitTime;          //退出时间
	CT_INT32           iSaveTag;            //保存原因
};

//保存用户战斗数据
typedef MasterFightAdd MSC_DB_MasterFightSaveData;
//struct MSC_DB_MasterFightSaveData
//{
//	CT_DWORD				dwUserID;             //用户id
//	CT_INT32				iType;					//职业
//	MasterCombatStatus		mcs;
//};

struct MSC_DB_MasterSaveDataEx
{
	CT_DWORD            dwUserID;             //用户id
	CT_INT32			iSaveTagEx;
	CT_INT64			iInt64Value;       //
	CT_INT32			iInt32Value;       //

};

//修改领主名称
struct MSC_DB_MasterName
{
	CT_DWORD			dwUserID;			//用户Id
	CT_CHAR				cName[NAME_SIZE+1];	//名称
	CT_INT32			iPropID;			//道具id
};

//修改战队名称
typedef MSC_DB_MasterName MSC_DB_FTName ;

//保存生产数据
struct MSC_DB_SaveProductingData
{
	CT_DWORD            dwUserID;           //用户id
	ProductingBuilding  productingBuilding; //生产信息
};


//同步生产的建筑
struct MSC_DB_SysProductingData
{
      CT_DWORD            dwUserID;           //用户id
};


//读取生产数据
//struct MSC_DB_ReadProductingData
//{
//	CT_DWORD            dwUserID;         //用户id
//	ProductingBuilding  productingBuilding; //生产信息
//};

// Begin [9/20/2012 John]
// 读取潜能信息
//struct MSC_DB_R_TalentData
//{
//    CT_DWORD			dwUserID;          //用户id
//    TalentData			Talent;            // 潜能信息
//};
// 读取技能信息
//struct MSC_DB_R_SkillData
//{
//    CT_DWORD			dwUserID;          //用户id
//    SkillData			Skill;             // 技能信息
//};
// 读取道具信息
//struct MSC_DB_R_ItemData
//{
//    CT_DWORD			dwUserID;
//    ItemData			item;
//};
// 读取装备信息
//struct MSC_DB_R_EquipData
//{
//    CT_DWORD            dwUserID;
//    EquipmentData       equip;
//};
// 读取增益信息
//struct MSC_DB_R_BuffData
//{
//    CT_DWORD dwUserID;          // 用户ID
//    BuffData buff;
//};
// 读取冷却信息
//struct MSC_DB_R_CooldownData
//{
//    CT_DWORD     dwUserID;      // 用户ID
//    CooldownData cooldown;
//};
// 保存冷却信息
struct MSC_DB_W_CooldownData
{
    CT_DWORD     dwUserID;      // 用户ID
    CT_INT32     iType;         // 冷却类型
    CT_INT64     llLeftTime;    // 剩余时间
};
// 保存潜能信息
struct MSC_DB_W_TalentData
{
    CT_DWORD			dwUserID;          // 用户ID
    CT_INT32			iTalentID;         // 潜能ID
    CT_CHAR				szName[NAME_SIZE]; // 潜能名称
    //CT_INT32			iExp;              // 消耗的潜能点数
};

// 保存技能信息
struct MSC_DB_W_SkillData
{
    CT_DWORD			dwUserID;          // 用户ID
    CT_INT32			iSkillID;          // 技能ID
    CT_CHAR				szName[NAME_SIZE]; // 技能名称
    CT_INT32			iGradeInc;         // 等级增加值
    //CT_INT32			iExp;              // 消耗的技魂 彭文删除
    CT_INT32			Used;              // 是否装备
};
// 保存道具信息
struct MSC_DB_W_ItemData
{
    CT_DWORD            dwThread;          // 线程ID
    CT_DWORD 			dwUserID;          // 用户ID
    CT_INT32            key;               // 唯一标识
    CT_INT32 			iItemID;           // 道具ID
    CT_CHAR  			szName[NAME_SIZE]; // 道具名称
    CT_INT32 			iCount;            // 道具数量
    CT_INT32 			iIndex;            // 道具所在槽索引值               
};
// 保存增益信息
struct MSC_DB_W_BuffData
{
    CT_DWORD            dwThread;          // 线程ID
    CT_DWORD            dwUserID;          // 用户ID
    CT_INT32            key;               // 道具的唯一标识
    CT_INT32            iGroup;            // 组ID
    CT_INT32            iBuffID;           // 增益ID
    CT_INT64            toggleTime;        // 获得时间
    CT_INT32            iDuration;         // 持续时间
};

// 增益消失
struct MSC_DB_LoseBuffData
{
    CT_DWORD            dwUserID;             //用户id
    CT_DWORD            dwThread;             //线程id
    CT_INT32            iGroup;               // 组ID
    CT_INT32            iBuffID;              // 增益ID
};

// 同步道具数据
struct MSC_DB_SyncItemData
{
    CT_DWORD            dwUserID;
    CT_DWORD            dwThread;
    ItemData            stItems[ITEM_SLOT_COUNT];
};
// 同步装备数据
struct MSC_DB_SyncEquipData
{
    CT_DWORD            dwUserID;
    CT_DWORD            dwThread;
    EquipmentData       stEquips[ITEM_SLOT_COUNT];
};
// 同步增益数据
struct MSC_DB_SyncBuffData
{
    CT_DWORD            dwUserID;
    CT_DWORD            dwThread;
    BuffData            stBuffs[BUFF_SLOTS];
};
// 同步元素属性
struct MSC_DB_SyncElmAttrData
{
    CT_DWORD            dwUserID;
    CT_DWORD            dwThread;
    ElmMarrowAttr       stDatas[ITEM_SLOT_COUNT];
};
// 保存战旗信息
struct MSC_DB_SaveBattleFlag
{
    CT_DWORD            dwUserID;
    CT_DWORD            dwThread;
    CT_INT32            iType;
    CT_INT32            iAdd;
};

// 道具交换
struct MSC_DB_W_SwapItemData
{
    CT_DWORD            dwThread;          // 线程ID
    CT_DWORD            dwUserID;          // 用户ID
    struct tagItemPair
    {
        CT_INT32        key;               // 唯一标识
        CT_INT32 		iItemID;           // 道具ID
        CT_CHAR  		szName[NAME_SIZE]; // 道具名称
        CT_INT32 		iCount;            // 道具数量
        CT_INT32 		iIndex;            // 道具所在槽索引值
    }Pair[2];
};
// 保存装备信息
struct MSC_DB_W_EquipData
{
    CT_DWORD            dwThread;          // 线程ID
    CT_DWORD 			dwUserID;          // 用户ID
    CT_DWORD            key;               // 唯一标识
    CT_INT32            iIndex;            // 装备在物品栏的位置
    CT_INT32            iLevel;            // 强化等级
    CT_INT32            nSlot;             // 镶孔数量
    CT_INT32            Gem[GEM_COUNTS];   // 宝石信息
    CMD_G_Forge         Forge[FORGE_ATTRS];
};
// 强化的插孔
struct MSC_DB_W_SmithingAndSlot
{
    CT_DWORD            dwThread;          // 线程ID
    CT_DWORD 			dwUserID;          // 用户ID
    CT_DWORD            key;               // 唯一标识
    CT_INT32            iEquipID;          // 道具ID
    CT_INT32            iLevel;            // 强化等级变化值
    CT_INT32            nSlot;             // 镶孔数量变化值    
    CT_INT32            iDiamond;          // 消耗钻石数
    CT_INT32            iGold;             // 消耗金币数
    CT_INT32            iItemID;           // 消耗的道具ID
    CT_INT32            iItemNum;          // 消耗的道具数量
};

// 洗炼
struct MSC_DB_W_ForgeData
{
    CT_DWORD            dwThread;          // 线程ID
    CT_DWORD 			dwUserID;          // 用户ID
    CT_DWORD            key;               // 唯一标识
    CT_INT32            iEquipID;          // 道具ID
    CMD_G_Forge         Forge[FORGE_ATTRS];// 洗炼信息
    CT_INT32            iDiamond;          // 消耗钻石数
    CT_INT32            iGold;             // 消耗金币数
    CT_INT32            iItemID;           // 消耗的道具ID
    CT_INT32            iItemNum;          // 消耗的道具数量
};
// 镶嵌
struct MSC_DB_W_EmbedData
{
    CT_DWORD            dwThread;          // 线程ID
    CT_DWORD 			dwUserID;          // 用户ID
    CT_DWORD            key;               // 唯一标识
    CT_INT32            iGemID;            // 宝石信息
    CT_INT32            iSlot;             // 插孔序号    
};
// 继承
struct MSC_DB_W_InheritData
{
    CT_DWORD            dwThread;          // 线程ID
    CT_DWORD 			dwUserID;          // 用户ID
    CT_DWORD            keySrc;            // 被继承的装备唯一标识
    CT_INT32            iSrcID;            // 被继承的装备ID
    CT_INT32            iLvSrc;            // 被继承时的装备等级 
    CT_DWORD            keyDst;            // 继承的装备唯一标识
    CT_INT32            iDstID;            // 继承的装备ID
    CT_INT32            iDiamond;          // 消耗钻石数
    CT_INT32            iGold;             // 消耗金币数
    CT_INT32            iItemID;           // 消耗的道具ID
    CT_INT32            iItemNum;          // 消耗的道具数量
};

// 更新背包格子
struct MSC_DB_W_BagSlot
{
    CT_DWORD            dwThread;          // 线程ID
    CT_DWORD 			dwUserID;          // 用户ID
    CT_INT32            nSlot;             // 新增的格子数
   // CT_INT32            iDiamond;          // 消耗的钻石
};
// End [9/20/2012 John]


// Begin [9/20/2012 wangchen]
// 保存战斗奖励
struct MSC_DB_W_FightingReward
{
	CT_INT32					iExperience;		//经验
	CT_INT32					iGold;				//金币
	CT_INT32 				iDiamond;			//钻石
	CT_INT32					iPotential;			//潜力点
	CT_INT32					iPrestige;			//声望
	CT_INT32					iMobility;			//行动力
};
// End [9/20/2012 wangchen]

//城市事件奖励
struct MSC_DB_EventAward
{
	CT_DWORD 			dwUserID;       	//用户id
	CT_INT32 			iGold;				//金币
	CT_INT32 			iFood;				//食物
	CT_INT32 			iCiviliNum;     	//文明度
	CT_CHAR             cProp1Name[NAME_SIZE];       //道具1名字
	CT_INT32            iProp1;     		//道具1类型
	CT_INT32            iPropNum1;  		//道具1数量
	CT_CHAR             cProp2Name[NAME_SIZE];       //道具2名字  
	CT_INT32            iProp2;     		//道具2类型
	CT_INT32            iPropNum2;  		//道具2数量 
};
//////////////////////////////////////////////////////////////////////////
struct MSC_USR_AddItem
{
    CT_DWORD    dwUserID;   // 用户ID
    CT_INT32    iItemID;    // 道具ID
    CT_INT32    iCount;     // 道具数量
};

//删除道具
struct MSC_USR_DeleteItem
{
	CT_DWORD    dwUserID;   // 用户ID
	CT_INT32    iItemID;    // 道具ID
	CT_INT32    iCount;     // 道具数量
};

//修改领主名称
struct MSC_USR_ChangeMasterName
{
	CT_DWORD	dwUserID;
	CT_INT32	iPropID;
	CT_INT32	iStatus;
	CT_CHAR		cName[NAME_SIZE];
};

// Begin [10/20/2012 wangchen]
//世界场景信息
struct MSC_DB_WorldSceneInfo
{
	CT_DWORD			dwUserID;			//用户id
	CT_INT32			iSceneID;			//场景ID
	CT_INT32			iReward;			//奖励类型
};

//场景信息
struct MSC_DB_SceneInfo
{
	CT_INT32		iWorldSceneID;						//世界场景ID
	CT_INT32		iSceneID;							//场景ID
	CT_DWORD		dwUserID;							//用户ID
	CT_INT32		iSceneStar;							//场景星级
	CT_INT32		iFightingNum;						//战斗次数
	CT_INT32		iDifficulty;						//难度
};

//更新购买战斗次数
struct  MSC_DB_BuyFightingTime
{
	CT_DWORD		dwUserID;							//用户id
	CT_INT32		iBuyFightingTime;					//购买战斗次数
};


//副本战斗
struct MSC_Fighting_Info
{
	CT_DWORD		dwUserID;							//发起用户ID
	CT_INT32		iWorldSceneID;						//世界场景ID
	CT_INT32		iSceneID;							//场景ID
	CT_INT32		iDifficulty;						//难度
	CT_INT32		iSceneStar;							//场景星级
	CT_INT32		iSeed;								//随机种子

	UserFightingStatus	FightingData;					//战斗数据
};

//资源掠夺
struct MSC_TeamPVP_ResRobbery
{
	CT_DWORD		dwUserID;							//发起用户ID
	CT_INT32		iSeed;								//随机种子
	CT_INT32        iWin;                               //发起方是否赢了

	CT_INT32		iFood;								//消耗的食物    
	CT_INT32		iScenceID;							//场景id
	CT_INT32		iSpotID;							//资源点id   
	CT_DWORD		dwTeamID;							//战队id  
	CT_BOOL			bLookLeft;

	UserFightingStatus	LeftFightingData[TEAM_NUM];					//战斗数据
	UserFightingStatus	RightFightingData[TEAM_NUM];					//战斗数据
};

//资源掠夺
struct MSC_TeamPVE_ResRobbery
{
	CT_DWORD		dwUserID;							//发起用户ID
	CT_INT32		iSeed;								//随机种子
	CT_INT32        iResSpotQuality;                    //资源点品质 
	CT_INT32        iWin;                               //发起方是否赢了


	CT_INT32		iFood;								//消耗的食物    
	CT_INT32		iScenceID;							//场景id
	CT_INT32		iSpotID;							//资源点id   
	CT_DWORD		dwTeamID;							//战队id
	
	UserFightingStatus FightingData[SPOT_GUARD_MEMBER];					//战斗数据
};

//元素之塔占领
struct MSC_TowerPVP_ResRobbery
{
	CT_DWORD		dwUserID;							//发起用户ID
	CT_INT32		iSeed;								//随机种子
	CT_INT32        iWin;                               //发起方是否赢了
	CT_BOOL			bLookLeft;							//查看左方?

	CT_INT32        iTowerID;                           //塔的id
	CT_INT32        iFloorID;                           //层id
	CT_INT32        iSlotID;                            //资源点id
	CT_INT32        iOccupyTime;                        //占领时间
	UserFightingStatus	LeftFightingData;				//挑战方战斗数据
	UserFightingStatus	RightFightingData;              //被挑战方的战斗数据
	CMD_G_NormalReward  towerProductCost;             //挑战的消耗
	CT_INT64		iFID;
};


//元素之塔的离线战斗数据
struct MSC_TowerOfflineUserFightData
{
	CT_DWORD                 dwUserID;                   //用户id
	CT_DWORD                 dwOfflineUserID;            //离线用户id
	UserFightingStatus		 offlineFightingData;       //离线用户战斗数据
	MSC_TowerPVP_ResRobbery  towerPVPResRobbery;        //元素之塔占领数据
};

//组队副本
struct MSC_TeamDuplicate_Fighting
{
	CT_DWORD		dwUserID;							//发起用户ID
	CT_INT32		iSceneID;							//场景ID
	CT_INT32		iDifficulty;						//难度
	CT_INT32		iSeed;								//随机种子
	CT_INT32		iWiner;								//胜利方
	//CT_INT32        iWin;                               //发起方是否赢了
	UserFightingStatus FightingData[SPOT_GUARD_MEMBER];		    //战斗数据
};

//元素之塔关卡通关
struct MSC_TowerCheckPoint_Fighting
{
	CT_DWORD		dwUserID;							//用户ID
	CT_INT32		iCPID;								//关卡ID
	CT_INT32		iSeed;								//随机种子
	CT_INT32		iScore;								//成绩

	UserFightingStatus	FightingData;					//战斗数据
};

//观战数据
struct MSC_Look_Fighting
{
	MSC_Fighting_Info				FightingInfo;		//战斗数据
	CT_DWORD						dwUserID;			//用户ID
};

//竞技场战斗记录
struct	MSC_DB_W_PVPFightingInfo
{
	CT_INT32		iDifficulty;
	PVPFightingInfo	PVPFightinginfo;		//战报记录
};

//竞技场终结
struct MSC_DB_PVPEnding
{
	CT_DWORD		dwUserID;				//用户id
};

//战斗结束
struct MSC_Fighting_End
{
	CT_INT32		iWorldSceneID;						//世界场景ID
	CT_INT32		iSceneID;							//场景ID
	CT_INT32		iUser;								//用户数据
	CT_INT32		iSceneStar;							//星级
	CT_INT32		iSeed;								//随机种子
	CT_INT32		iUserPos;							//用户位置
	CT_INT32		iArmy1ID;							//军队ID
	CT_INT32		iArmy1Pos;							//军队位置
	CT_INT32		iArmy1Grade;						//军队等级
	CT_INT32		iArmy2ID;							//军队ID
	CT_INT32		iArmy2Pos;							//军队位置
	CT_INT32		iArmy2Grade;						//军队等级
	CT_INT32		iSkill1ID;							//用户技能
	CT_INT32		iSkill2ID;							//用户技能
	CT_INT32		iSkill3ID;							//用户技能
};

//战斗数据
struct MSC_Event_Fighting_End
{
	CT_DWORD		dwUserID;							//发起用户ID
	CT_INT32		iIndex;								//事件索引
	CT_INT32		iAwardID;							//事件ID
	CT_INT32		iWiner;								//胜利

};

struct MSC_PVEFighting_Reward
{
	CT_DWORD		dwUserID;
	CT_INT32		iWorldSceneID;
	CT_INT32		iSceneID;
	CT_INT32		iDifficulty;
	CT_INT32		iSceneStar;
};

//副本入口完成
struct MSC_Duplicate_Finish
{
	CT_DWORD		dwUserID;							//用户ID
};

//扫荡信息
struct MSC_DB_RaidsInfo
{
	CT_DWORD		dwUserID;							//用户id
	CT_INT32		iWorldID;							//世界场景
	CT_INT32		iSceneID;							//场景ID
	CT_INT32		iDifficulty;						//难度
	CT_INT32		iAllTime;							//总次数
	CT_INT32		iCurrentTime;						//当前次数
	CT_INT64		lBeginTime;							//开始时间

	CT_INT32		iLastStamina;						//剩余体力
};

//扫荡奖励
struct MSC_DB_RaidsReward
{
	CT_DWORD		dwUserID;							//用户ID
	CT_INT32		iExperience;						//经验
	CT_INT32		iGold;								//金币
	CT_INT32		iDiamond;							//钻石
	CT_INT32		iPotential;							//潜力点
	CT_INT32		iSkillPoint;						//技能点强化点
	CT_INT32		iGoods[GOODS_COUNT];				//装备ID
	CT_INT32		iGoodsCount[GOODS_COUNT];			//数目
	CT_INT32		iCurrentTime;						//当前次数
	CT_INT64		lTime;								//时间

};

//组队副本开启
struct MSC_DB_TeamDuplicateScene
{
	CT_DWORD		dwUserID;							//组队副本
	CT_INT32		iScnenID;							//场景ID
	CT_INT32		iDifficulty;						//难度
	CT_INT32		iWin;								//是否胜利
};

//组队副本自动匹配
struct MSC_DB_TeamDuplicateAutoMatch
{
	CT_DWORD		dwUserID;							//组队副本
	CT_DWORD		dwDstUserID[TEAM_NUM];				//用户ID
	CT_INT32		iFightingPower;						//战斗力
	CT_INT32		iGrade;								//等级
};

//组队副本自动匹配
struct MSC_DB_TeamDuplicateAutoMatchInfo
{
	CT_DWORD		dwUserID;							//用户id
	CT_DWORD		dwDstUserID[TEAM_NUM-1];	//抽取用户
};

//计算奖励
struct MSC_RaidsReward
{
	CT_DWORD		dwUserID;							//用户id
};
// End [10/20/2012 wangchen]
//区域解锁信息
struct MSC_DB_W_RegionUnlockData
{
	CT_DWORD	dwUserID;	//用户ID
	CT_INT32	iRegionID;	//区域ID
	CT_INT32	iGold;		//消耗金币
	CT_INT32	iDiamod;	//消耗钻石
};

//读取用户解锁区域id
struct MSC_DB_R_UnlockRegion
{
	CT_DWORD	dwUserID;	//用户id
	CT_INT32	iRegionID;	//区域id
};


// Begin [11/8/2012 wangchen]
//获取军队信息
struct MSC_DB_GetArmyInfo
{
	CT_DWORD	dwUserID;		//用户ID
};

//军队升级
struct MSC_DB_R_ArmyGrade
{
	CT_DWORD	dwUserID;		//用户ID
	CT_INT32	iArmyID;		//军队ID

	//CT_INT32	iGold;			//消耗金币
	//CT_INT32	iFood;			//消耗食物
	//CT_INT32	iGoods;			//消耗物品
	//CT_INT32	iCount;			//消耗数量
};

//军队进阶
struct MSC_DB_R_ArmyAdvanced
{
	CT_DWORD	dwUserID;		//用户ID
	CT_INT32	iOldArmyID;		//军队ID
	CT_INT32	iNewArmyID;		//军队ID
	CT_INT32	iArmyGrade;		//军队等级

	//CT_INT32	iGold;			//消耗金币  //彭文删除
	//CT_INT32	iGoods;			//消耗物品
	//CT_INT32	iCount;			//消耗数量
};

//军队信息
struct MSC_DB_ArmyInfo
{
	CT_DWORD	dwUserID;		//用户ID
	CT_INT32	iArmyGrade;		//军队等级
	CT_INT32	iArmyID;		//军队ID
};

//阵法信息
struct MSC_DB_BattleArray
{
	CT_DWORD	dwUserID;				//用户ID
	CT_INT32	iPosition[POST_COUNT/2];//位置
};

// End [11/8/2012 wangchen]
//读取用户已完成任务
struct MSC_DB_TaskFinishData
{
	CT_DWORD	dwUserID;	//用户id
	CT_INT32	iTaskID;	//任务id
};

//读取用户进行任务信息
struct MSC_DB_TaskProgressData
{
	CT_DWORD	dwUserID;	//用户id
	CT_INT32	iTaskID;	//任务id
	CT_INT32	iSubTaskID;	//子任务id
	CT_INT32	iTime;		//完成次数
	CT_INT32	iCompleted;	//是否完成
};

//////////////////////////////////////////////////////////////////////////
// 读取的日常任务信息 [11/14/2012 John]
//struct MSC_DB_R_DailyQuestInfo
//{
//    CT_DWORD	dwUserID;
//    DailyQuestData daily;
//};
// 读取的签到信息 [11/14/2012 John]
//struct MSC_DB_R_SignInfo
//{
//    CT_DWORD	dwUserID;
//    SignData    sign;
//};
// 读取的活跃度信息 [11/14/2012 John]
//struct MSC_DB_R_ActiveInfo
//{
//    CT_DWORD	dwUserID;
//    ActiveData  active;
//};

// 更新的签到信息 [11/14/2012 John]
struct MSC_DB_W_SignInfo
{
    CT_DWORD    dwUserID;   // 用户id
    CT_INT32    nSigned;    // 签到次数
    CT_INT32    bReward;    // 是否领取
};
// 日常任务进度信息 [11/14/2012 John]
struct MSC_DB_W_DailyQuest
{
    CT_DWORD    dwUserID;   // 用户id
    CT_INT32    iQuestID;   // 任务id
    CT_INT32    iCurrent;   // 当前进度
};
// 活跃度信息 [11/14/2012 John]
struct MSC_DB_W_ActiveInfo
{
    CT_DWORD    dwUserID;   // 用户id
    CT_INT32    iLevel;     // 活跃度等级
    CT_INT32    iActive;    // 活跃度数值
};

struct MSC_DB_W_ActiveReward
{
    CT_DWORD    dwUserID;   // 用户id
    CT_INT32    iLevel;     // 活跃度等级
};

// 清除连续登陆记录 [11/15/2012 John]
struct MSC_DB_D_SignInfo
{
    CT_DWORD    dwUserID;   // 用户ID
};
// 清除日常记录 [11/15/2012 John]
struct MSC_DB_D_DailyInfo
{
    CT_DWORD    flag;       // 无参数..
};

//竞技场信息
struct MSC_DB_W_PVPInfo
{
	CT_DWORD		dwUserID;								//用户id
	CT_BYTE			cbChestStatus;							//宝箱状态
	CT_INT32		iFightingCount;							//可挑战次数
	CT_INT64		lRefreshTime;							//刷新时间
	CT_INT32		iPVPDegree;								//竞技度
	CT_INT32		iPVPChestNum;							//领取宝箱次数
};

//竞技场信息
struct MSC_DB_PVPInfo
{
	CT_DWORD		dwUserID;								//用户Id
	CT_DWORD		dwChallengUserID[PVP_CHALLENGE_COUNT];	//挑战用户
	CT_BYTE			cbChestStatus;							//宝箱状态
	CT_INT32		iFightingCount;							//可挑战次数
	CT_INT64		lRefreshTime;							//刷新时间
	CT_BYTE			cbChallengStatus[PVP_CHALLENGE_COUNT];	//挑战状态
	CT_INT32		iDegree;								//竞技度
	CT_INT32		iChestNum;								//领取宝箱次数
};

//pvp保存对手数据到数据库
struct MSC_DB_PVPChallgerSave
{
	CT_DWORD		dwUserID;								//用户id
	PVPInfo         pvpInfo;							    //pvp数据
};


//刷新对手
struct	MSC_DB_Refresh
{
	CT_DWORD		dwUserID;						//用户id
};


//读取战报记录
struct	MSC_DB_R_PVPFightingRecord
{
	CT_DWORD		dwUserID;			//用户Id
	CT_INT32		iIndex;				//索引
};

//读取战报记录
struct	MSC_DB_FighitngRecord
{
	CT_DWORD		dwUserID;			//用户Id	
	PVPFightingInfo	PVPFightinginfo;		//战报记录
};

//读取用户信息
struct	MSC_DB_R_UserInfo
{
	CT_DWORD		dwUserID;			//用户ID
};

//用户信息
struct	MSC_DB_UserInfo
{
	CT_DWORD		dwUserID;			//用户id
};

//读取战斗信息
struct	MSC_DB_R_FightingInfo
{
	CT_DWORD		dwUserID;			//用户Id
	CT_INT32		iPVPFightingIndex;	//战斗索引
};

//战斗信息
struct	MSC_DB_FighitngInfo
{
	CT_DWORD				dwUserID;							//发起用户ID
	CT_INT32				iUserGrade;							//用户等级
	CT_CHAR					cUserName[NAME_SIZE];				//用户名称
	CT_INT32				iSceneStar;							//用户星级
	CT_INT32				iCivilGrade;						//文明等级
	CT_INT32				iProfession;						//用户职业
	MasterCombatStatus		FightingInfo;						//战斗信息
	CT_INT32				iUserPos;							//用户位置
	CT_INT32				iSkill1ID;							//用户技能
	CT_INT32				iSkill2ID;							//用户技能
	CT_INT32				iSkill3ID;							//用户技能
	CT_INT32				iWorldSceneID;						//世界场景ID
	CT_INT32				iSceneID;							//场景ID
	CT_INT32				iSeed;								//随机种子
	CT_INT32				iArmy1ID;							//军队ID
	CT_INT32				iArmy1Pos;							//军队位置
	CT_INT32				iArmy1Grade;						//军队等级
	CT_INT32				iArmy2ID;							//军队ID
	CT_INT32				iArmy2Pos;							//军队位置
	CT_INT32				iArmy2Grade;						//军队等级
};

//挑战状况
struct MSC_DB_ChallengeInfo
{
	CT_DWORD		dwUserID;								//用户ID
	CT_INT32		iDifficulty;							//挑战难度
	CT_BOOL			bWiner[PVP_CHALLENGE_COUNT/2];			//是否战胜
	CT_DWORD		dwDstUserID[PVP_CHALLENGE_COUNT/2];		//目标
};

//读取排行榜
struct	MSC_DB_R_RangkingInfo
{
	CT_DWORD		dwUserID;					//用户id
};
//排行榜
struct	MSC_DB_RankingInfo
{
	CT_DWORD		dwUserID;					//用户id
	LookUserInfo		Ranking[TOP_COUNT];	//排行榜信息
};

//////////////////////////////////////////////////////////////////////
//商店数据库结构
////购买记录
//struct MSC_DB_ShoppingRecord
//{
//	CT_DWORD	dwUserID;			//用户id
//	CT_INT32	iGoodsID;			//物品id
//	CT_INT32	iGoodsNum;			//物品数量
//	CT_INT32	iDiamod;			//花费钻石
//	CT_INT32	iGold;				//花费金币
//	CT_INT32	iItemID;			//花费道具
//	CT_INT32	iItemNum;			//道具数量
//	CT_INT32	iMarketType;		//商店类型
//};

//黑店商品刷新
struct MSC_DB_BlackMarketRefresh
{
	CT_DWORD	dwUserID;			//用户id
	CT_INT32	iTime;				//剩余刷新次数
	CT_INT32	iGoodsID1;			//道具id
	CT_INT32	iNum1;				//道具数量
	CT_INT32	iGoodsID2;			//道具id
	CT_INT32	iNum2;				//道具数量
	CT_INT32	iGoodsID3;			//道具id
	CT_INT32	iNum3;				//道具数量
	CT_INT32	iGoodsID4;			//道具id
	CT_INT32	iNum4;				//道具数量
	CT_INT32	iGoodsID5;			//道具id
	CT_INT32	iNum5;				//道具数量
	CT_INT32	iGoodsID6;			//道具id
	CT_INT32	iNum6;				//道具数量
	CT_INT64	iLastRefTime;		//最后刷新时间
};

//黑店购买物品
struct MSC_DB_BlackMarketShopping
{
	CT_DWORD	dwUserID;			//用户id
//	CT_INT32	iTime;				//刷新次数	//zhoujun 购买时不需要更新刷新次数
	CT_INT32	iNumList[MARKET_BLACKNUM_SYS+MARKET_BLACKNUM_PAY];			//购买道具数量
};

//商店功能状态
struct MSC_DB_MarketFunction
{
	CT_DWORD	dwUserID;			//用户id
	CT_INT32	iMall;				//商城状态
	CT_INT32	iBlackMarket;		//黑店状态
};

//商店个人限购
struct MSC_DB_MarketPersonal
{
	CT_DWORD	dwUserID;			//用户
	CT_INT32	iGoodsID;			//物品id
	CT_INT32	iNum;				//购买次数
};


////体力购买记录
//struct MSC_DB_W_BuyStatmina
//{
//	CT_DWORD	dwUserID;			//用户ID
//	CT_INT32	iBuyNum;			//购买次数
//	CT_INT32	iDiamod;			//消耗钻石
//};
////////////////////////////////////////////////////
//宠物信息
struct MSC_DB_PetBornInfo
{
	CT_DWORD	dwUserID;
	PetInfo		pi;
};

//宠物幻化
struct MSC_DB_PetChange
{
	CT_DWORD	dwUserID;
	CT_INT32	iChangeID;
	CT_INT32	iSkillID;
};

//宠物技能
struct MSC_DB_PetSkill
{
	CT_DWORD	dwUserID;
	CT_INT32	iSkillID[PET_SKILL_COUNT];
};

//宠物蛋使用
struct MSC_UsePetEgg
{
	CT_DWORD	dwUserID;
	CT_INT32	iItemID;
};

///////////////////////////////////////////////////////////////
//邮件信息
struct MSC_DB_EmailUserResult
{
	CT_DWORD		dwUserID;			//用户id
	CT_INT32		iRet;				//用于判断邮件是否发送成功
};

////////////////////////////////////////////////////////////////////
//vip信息
struct MSC_DB_VipGiftGet
{
	CT_DWORD		dwUserID;			//用户id
	CT_INT32		iGiftGet;			//礼包是否领取
};

struct MSC_DB_VipApply
{
	CT_DWORD		dwUserID;			//用户id
	CT_INT32		iGrade;				//vip等级
	CT_INT64		llTime;				//d到期时间
	//CT_INT32		iPropID;			//道具id      //彭文删除
	//CT_INT32		iPropNum;			//道具数量
	//CT_INT32		iDiamod;			//钻石
	//CT_INT32		iDays;				//开通天数
	//CT_INT32		iGradeOpen;			//开通等级
	CT_INT32		iGift;				//礼包是否领取
};

struct MSC_DB_VipInfo
{

};

// 添加充值记录
struct MSC_DB_A_Charge
{
    CT_INT32 dwUserID;      // 用户ID
    CT_INT32 iPrice;          // 价格
    CT_INT32 iDiamond;      // 货物
    CT_INT32 iState;        // 订单状态：0（成功），1（提交）
    CT_CHAR szSign[EXORDER_NO_LEN + 1];   // 订单签名
};

// 更新充值记录信息
struct MSC_DB_U_Charge
{
    CT_CHAR szSign[EXORDER_NO_LEN + 1];   // 订单签名
    CT_INT32 iIndex;                // 订单编号
    CT_INT32 iState;                // 订单状态
};

/* 装备附魔 */
#define ENCHANT_INDEX {ENCHANT_ATHENA, ENCHANT_POSEIDON, ENCHANT_HERA, ENCHANT_ARES, ENCHANT_ZEUS, ENCHANT_HARDESS}
struct MSC_DB_R_EnchantData
{
    CT_DWORD dwUserID;
    EnchantData data;
};

struct MSC_DB_W_EnchantData
{
    CT_DWORD dwUserID;
    CT_INT32 iData[EQUIP_COUNTS * ENCHANT_ATTR_CNT];
    //CT_INT32 iBID;               // 部位
    //CT_INT32 iIDX;               // 序号
    //CT_INT32 iGradeAdd;          // 附魔等级变化量(0: 激活; 1:升级)
};

struct MSC_DB_EnchantFailTime
{
	CT_INT32 dwUserID;
	CT_INT32 iBID;				// 部位
	CT_INT32 iIDX;				// 序号
	CT_INT32 iFailCount;		// 失败次数
};

struct MSC_DB_EnchantReward 
{
	CT_INT32 dwUserID;
	CT_INT32 iIndex;			// 奖励索引
};

//日志数据库
////////////////////////////////////////////////////////////
//军队升级
struct MSC_LOGDB_W_ArmyGrade
{
	CT_DWORD	dwUserID;		//用户ID
	CT_DWORD    iUserGarde;     //用户等级
	CT_INT32	iArmyID;		//军队ID
	CT_INT32	iGrade;			//等级

	CT_INT32	iGold;			//消耗金币
	CT_INT32	iFood;			//消耗食物
	CT_INT32	iGoods;			//消耗物品
	CT_INT32	iCount;			//消耗数量
};

//军队进阶
struct MSC_LOGDB_ArmyAdvanced
{
	CT_DWORD	dwUserID;		//用户ID
	CT_INT32	iOldArmyID;		//军队ID
	CT_INT32	iNewArmyID;		//军队ID
	CT_INT32	iArmyGrade;		//军队等级
	CT_INT32	iGold;			//消耗金币
	CT_INT32	iGoods[2];			//消耗物品
	CT_INT32	iCount[2];			//消耗数量
};

// 更新背包格子
struct MSC_LOGDB_BagSlot
{
	CT_DWORD 			dwUserID;          // 用户ID
	CT_INT32            nSlot;             // 新增的格子数
	CT_INT32            iDiamond;          // 消耗的钻石
	CT_INT32            iUserGrade;        //用户等级
};

//区域解锁信息
struct MSC_LOGDB_RegionUnlockData
{
	CT_DWORD	dwUserID;	//用户ID
	CT_INT32    iUserGrade; //用户等级
	CT_INT32	iRegionID;	//区域ID
	CT_INT32	iGold;		//消耗金币
	CT_INT32	iDiamod;	//消耗钻石
};

//道具使用完成
struct CMD_LOGDB_PropFinish
{
	CT_DWORD	dwUserID;		//用户ID
	CT_INT32	iID;			//道具ID
};

// 强化的插孔
struct MSC_LOGDB_SmithingAndSlot
{
	CT_DWORD 			dwUserID;          // 用户ID
	CT_INT32            iUserGrade;        //用户等级
	CT_DWORD            key;               // 唯一标识
	CT_INT32            iEquipID;          // 道具ID
	CT_INT32            iLevel;            // 强化等级变化值
	CT_INT32            nSlot;             // 镶孔数量变化值    
	CT_INT32            iDiamond;          // 消耗钻石数
	CT_INT32            iGold;             // 消耗金币数
	CT_INT32            iItemID;           // 消耗的道具ID
	CT_INT32            iItemNum;          // 消耗的道具数量
};

// 继承
struct MSC_LOGDB_InheritData
{
	CT_DWORD 			dwUserID;          // 用户ID
	CT_DWORD            iUserGrade;        //用户等级
	CT_DWORD            keySrc;            // 被继承的装备唯一标识
	CT_INT32            iSrcID;            // 被继承的装备ID
	CT_INT32            iLvSrc;            // 被继承时的装备等级 
	CT_DWORD            keyDst;            // 继承的装备唯一标识
	CT_INT32            iDstID;            // 继承的装备ID
	CT_INT32            iDiamond;          // 消耗钻石数
	CT_INT32            iGold;             // 消耗金币数
	CT_INT32            iItemID;           // 消耗的道具ID
	CT_INT32            iItemNum;          // 消耗的道具数量
};

// 强化的插孔
struct MSC_LOGDB_PropAddStrengthen
{
	CT_DWORD 			dwUserID;          // 用户ID
	CT_DWORD            iUserGrade;        // 用户等级
	CT_INT32            iEquipID;          // 道具ID  
	CT_INT32            iDiamond;          // 消耗钻石数
	CT_INT32            iGold;             // 消耗金币数
	CT_INT32            iItemID;           // 消耗的道具ID
	CT_INT32            iItemNum;          // 消耗的道具数量
	CT_INT32			iIndex;			   //
};

//城市事件奖励
struct MSC_LOGDB_EventAward
{
	CT_DWORD 			dwUserID;       	//用户id
	CT_INT32 			iGold;				//金币
	CT_INT32 			iFood;				//食物
	CT_INT32 			iCiviliNum;     	//文明度
	CT_CHAR             cProp1Name[NAME_SIZE];       //道具1名字
	CT_INT32            iProp1;     		//道具1类型
	CT_INT32            iPropNum1;  		//道具1数量
	CT_CHAR             cProp2Name[NAME_SIZE];       //道具2名字  
	CT_INT32            iProp2;     		//道具2类型
	CT_INT32            iPropNum2;  		//道具2数量 
};

// 保存潜能信息
struct MSC_LOGDB_TalentData
{
	CT_DWORD			dwUserID;          // 用户ID
	CT_INT32			iTalentID;         // 潜能ID
	CT_INT32			iExp;              // 消耗的潜能点数
};

//任务奖励
struct MSC_LOGDB_TaskReward
{
	CT_DWORD		dwUserID;	//用户ID
	CT_INT32        iUserGrade; //用户等级
	CT_INT32		iTaskID;   //任务id
	CT_INT32		iGold;      //金币
	CT_INT32		iDiamod;	//钻石
	CT_INT32		iXp;        //经验
	CT_INT32		iFood;      //食物
	CT_INT32		iPotential;	//潜力点
	CT_INT32		iSoul;		//技魂点
	CT_INT32		iPrestige;	//声望
	CT_INT32		iCiviliNum; //文明度
	CT_INT32		iStamina;	// 体力值
	CT_INT32		iItemID1;     //道具1类型
	CT_INT32		iNum1;			//道具1数量
	CT_INT32		iItemID2;     //道具2类型
	CT_INT32		iNum2;			//道具2数量
	CT_INT32		iItemID3;     //道具3类型
	CT_INT32		iNum3;			//道具3数量
	CT_INT32		iItemID4;     //道具4类型
	CT_INT32		iNum4;			//道具4数量
	CT_INT32		iDiamodCost;	//快速完成钻石消耗
};

//用户退出
struct MSC_LOGDB_UserExit
{
	CT_DWORD          dwUserID;       //用户id
};

//增加升级记录
struct MSC_LOGDB_AddUpgradeRecord
{
	CT_DWORD            dwUserID;         //用户id
	CT_INT32            iUserGrade;       //用户等级
	CT_INT32            iBid;             //建筑id
	CT_INT32			iGrade;			  //等级
	CT_INT32			iDiamod;		  //钻石
	CT_INT32            iGold;            //获得的金币
	CT_INT32            iFood;            //获得食物
	CT_INT32			iCivil;			  //获得文明度
	CT_INT32            iWorker;          //消耗的工人
	CT_INT32            iStamina;         //消耗的体力值
	CT_INT32            iProp1;           //获得的道具1id
	CT_INT32            iProp1Num;        //获得道具1的数目
	CT_INT32            iProp2;           //获得的道具2id
	CT_INT32            iProp2Num;        //获得道具2的数目
	CT_INT32            iProp3;           //获得的道具3id
	CT_INT32            iProp3Num;        //获得道具3的数目
	CT_INT32            iProp4;           //获得的道具4id
	CT_INT32            iProp4Num;        //获得道具4的数目
	CT_INT32            iXp;              //经验获得
	//CT_INT32            iSpeedTimes;      //加速次数
	CT_INT32            iUpgradeTimes;    //升级次数     
};

struct MSC_LOGDB_VipApply
{
	CT_DWORD		dwUserID;			//用户id
	CT_INT32        iUserGrade;         //用户等级
	CT_INT32		iPropID;			//道具id  
	CT_INT32		iPropNum;			//道具数量
	CT_INT32		iDiamod;			//钻石
	CT_INT32		iDays;				//开通天数
	CT_INT32		iGradeOpen;			//开通等级
};

struct MSC_LOGDB_ActiveReward
{
	CT_DWORD    dwUserID;   // 用户id
	CT_INT32    iLevel;     // 活跃度等级
	CT_INT32    iGold;
	CT_INT32    iFood;
	CT_INT32    iStamina;
	CT_INT32    iTalentNum;
	CT_INT32    iRankXp;
	struct tagItemReward
	{
		CT_INT32 iID;
		CT_INT32 iNum;
	} ItemReward[4];
};

//打开宝箱
struct	MSC_LOGDB_Chest
{
	CT_DWORD		dwUserID;
	CT_INT32		iGold;					//金币
	CT_INT32		iPrestige;				//声望
	CT_INT32		iTalentNum;				//潜能
	CT_INT32		iGoodsID1;				//道具ID1
	CT_INT32		iGoodsCount1;			//数量1
	CT_INT32		iGoodsID2;				//道具ID2
	CT_INT32		iGoodsCount2;			//数量2
	CT_INT32		iGoodsID3;				//道具ID3
	CT_INT32		iGoodsCount3;			//数量3
};

struct MSC_LOGDB_ChatTime
{
	CT_DWORD    dwUserID;   // 用户id
	CT_INT32    iChatTime;	//聊天次数
};

struct MSC_LOGDB_AddProductingRecord
{
	CT_DWORD            dwUserID;         //用户id
	CT_INT32            iUserGrade;       //用户等级
	CT_INT32            iBid;             //建筑id
	CT_INT32            iGold;            //获得的金币
	CT_INT32            iFood;            //获得食物
	CT_INT32			iCivil;			  //获得文明度
	CT_INT32			iDiamod;		//消耗钻石
	CT_INT32            iWorker;          //消耗的工人
	CT_INT32            iStamina;         //消耗的体力值
	CT_INT32            iProp1;           //获得的道具1id
	CT_INT32            iProp1Num;        //获得道具1的数目
	CT_INT32            iProp2;           //获得的道具2id
	CT_INT32            iProp2Num;        //获得道具2的数目
	//CT_INT32            iSpeedupTimes;    //加速次数
	CT_INT32            iProductingTimes; //生产次数

};

// 添加充值记录
struct MSC_LOGDB_Charge
{
	CT_INT32 dwUserID;      // 用户ID
	CT_INT32 iPrice;          // 价格
	CT_INT32 iDiamond;      // 货物
	CT_INT32 iState;        // 订单状态：0（成功），1（提交）
	CT_CHAR szSign[EXORDER_NO_LEN + 1];   // 订单签名
	CT_INT32 iUserGrade;    //用户等级
};

// 更新充值记录信息
struct MSC_LOGDB_UpdateCharge
{
	CT_CHAR szSign[EXORDER_NO_LEN + 1];   // 订单签名
	CT_INT32 iIndex;                // 订单编号
	CT_INT32 iState;                // 订单状态
};

// 更新充值记录信息,苹果的需要修改订单的金额
struct MSC_LOGDB_UpdateCharge_EX
{
	CT_CHAR szSign[EXORDER_NO_LEN + 1];   // 订单签名
	CT_INT32 iIndex;                // 订单编号
	CT_INT32 iState;                // 订单状态
	CT_INT32 iMoney;				// 订单金额
	CT_INT32 iDiamond;				// 钻石
};

// 领取签到奖励 [11/14/2012 John]
struct MSC_LOGDB_SignReward
{
	CT_DWORD    dwUserID;   // 用户id
	CT_INT32    iUserGrade; //用户等级
	CT_INT32    nSigned;    // 签到天数
	CT_INT32    iGold;
	CT_INT32    iFood;
	CT_INT32    iStamina;
	CT_INT32    iTalentNum;
	CT_INT32    iRankXp;
	struct tagItemReward
	{
		CT_INT32 iID;
		CT_INT32 iNum;
	} ItemReward[4];
	CT_INT32    iCostDiamond;   // 消耗的钻石
};

struct MSC_LOGDB_FightRecord
{
	CT_DWORD    dwUserID;   // 用户id
	CT_INT32    iUserGrade; //用户等级
	CT_INT32	iAid;		//世界id
	CT_INT32	iFid;		//副本场景id
	CT_INT32	iDifficulty;//难度
	CT_INT32	iXp;		//经验
	CT_INT32	iGold;		//金币
	CT_INT32	iTalent;	//潜能
	CT_INT32	iDiamod;	//钻石
	CT_INT32	iSkillNum;	//技能点
	CT_INT32	iPropID[PVP_GOODS_COUNT];	//物品id
	CT_INT32	iPropNum[PVP_GOODS_COUNT];	//物品数量
	CT_INT32	iWin;		//结果
};
typedef MSC_LOGDB_FightRecord MSC_LOGDB_TeamDuplicateRecord;

//组队副本消耗
struct MSC_LOGDB_TeamDuplicateCost
{
	CT_DWORD	dwUserID;		//用户Id
	CT_INT32	iSceneID;		//场景ID
	CT_INT32	iDifficulty;	//困难
	CT_INT32	iStaminaCost;	//体力消耗
};

struct MSC_LOGDB_PETUPSKILL
{
	CT_DWORD    dwUserID;   // 用户id
	CT_INT32	iSkillID;		//宠物id
	CT_INT32	iItemID;		//道具ID
	CT_INT32	iNum;			//道具数目
	CT_INT32	iGold;		//金币消耗
};

struct MSC_LOGDB_PetRename
{
	CT_DWORD    dwUserID;   // 用户id
	CT_INT32	iPetID;		//宠物id
	CT_INT32	iIndex;		//索引
	CT_INT32	iPropID;	//物品id
	CT_INT32	iPropNum;	//物品数量
};

struct MSC_LOGDB_ItemSell
{
	CT_DWORD    dwUserID;   // 用户id
	CT_INT32	iPropID;	//物品id
	CT_INT32	iPropNum;	//物品数量
	CT_INT32	iGlod;		//出售金币
};

//购买记录
struct MSC_LOGDB_ShoppingRecord
{
	CT_DWORD	dwUserID;			//用户id
	CT_INT32    iUserGrade;         //用户等级
	CT_INT32	iGoodsID;			//物品id
	CT_INT32	iGoodsNum;			//物品数量
	CT_INT32	iDiamod;			//花费钻石
	CT_INT32	iGold;				//花费金币
	CT_INT32	iItemID;			//花费道具
	CT_INT32	iItemNum;			//道具数量
	CT_INT32	iMarketType;		//商店类型
};

//体力购买记录
struct MSC_LOGDB_BuyStatmina
{
	CT_DWORD	dwUserID;			//用户ID
	CT_INT32    iUserGrade;         //用户等级
	CT_INT32	iStatNum;			//购买体力值
	CT_INT32	iDiamod;			//消耗钻石
};

//pvp战斗奖励
struct MSC_LOGDB_PVPReward
{
	CT_CHAR		cIndex[PVPRECORD_INDEX];				//索引	
	CT_DWORD	dwUserID;				//用户Id
	CT_INT32	iGold;					//金币
	CT_INT32	iPrestige;				//声望
	CT_INT32	iGoods[PVP_GOODS_COUNT];//物品数目
	CT_INT32	iCount[PVP_GOODS_COUNT];//数目
	CT_INT32	iResult;		//结果
	CT_DWORD	dwAimID;			//对手ID
};

//强制刷新记录
struct MSC_LOGDB_Refresh
{
	CT_DWORD	dwUserID;				//用户id
	CT_DWORD    iUserGrade;             //用户等级
	CT_INT32	iDiamod;				//钻石
};

// 附魔记录
struct MSC_LOGDB_Enchant
{
	CT_DWORD dwUserID;
	CT_INT32 iUserGrade;
	CT_INT32 iIndex;              // 属性索引
	CT_INT32 iGrade;              // 属性附魔等级
	CT_INT32 nDiamondCost;        // 激活消耗钻石
    CT_INT32 nGoldCost;           // 激活消耗金币
	CT_INT32 nTalentNumCost;      // 附魔消耗
};

//钻石减少强化冷却时间
struct MSC_LOGDB_ClearCD
{
	CT_DWORD	dwUserID;				//用户id
	CT_INT32	iDiamod;				//钻石
	CT_INT32    iUserGrade;             //用户等级
};


//副本奖励领取
struct MSC_LOGDB_DuplicateReward
{
	CT_DWORD		dwUserID;						//用户id
	CT_INT32        iUserGrade;                     //用户等级
	CT_INT32		iWorldSceneID;					//世界场景ID
	CT_INT32		iRewardID;						//奖励ID
	CT_INT32		iExperience;					//经验
	CT_INT32		iGold;							//金币
	CT_INT32		iScore;							//分数
	CT_INT32		iGoodsID1;						//道具ID
	CT_INT32		iCount1;						//道具ID
	CT_INT32		iGoodsID2;						//道具ID
	CT_INT32		iCount2;						//道具ID
	CT_INT32		iPotential;						//潜力点
	CT_INT32 		iDiamond;						//钻石
	CT_INT32		iSkillPrint;					//技能点
};

//副本进入消耗
struct MSC_LOGDB_DuplicateEnter
{
	CT_DWORD		dwUserID;						//用户id
	CT_INT32		iFood;							//食物消耗
	CT_INT32		iPphysical;						//体力消耗
	CT_INT32		iGoods;							//物品消耗
	CT_INT32		iCount;							//物品数量

};

//副本购买战斗次数
struct  MSC_LOGDB_BuyFightingTime
{
	CT_DWORD		dwUserID;						//用户id
	CT_DWORD        iUserGrade ;                    //用户等级
	CT_INT32		iBuyFightingTime;				//购买战斗次
	CT_INT32		iDiamod;						//钻石
};

//竞技场战斗消耗
struct MSC_LOGDB_PVPFighringCost
{
	CT_DWORD		dwUserID;						//用户id
	CT_INT32        iUserGrade;                     //用户等级
	CT_INT32		iDiamod;						//钻石
	CT_INT32		iStamina;						//体力消耗

};

struct MSC_LOGDB_ActiveEventReward
{
	CT_DWORD		dwUserID;						//用户id
	CT_INT32        iGrade;                         //用户等级
	G_ActiveEventReward	activeReward;				//活动奖励
};

struct MSC_LOGDB_WorshipReward
{
	CT_DWORD		dwUserID;						//用户id
	CT_INT32		iGoldCost;						//消耗金币
	CT_INT32		iGold;							//获得金币
	CT_INT32		iFood;							//获得食物
	CT_INT32		iTalent;						//潜能点
	CT_INT32		iDiamod;						//钻石
	CT_INT32		iXp;							//经验
	CT_INT32		iItemID1;						//道具id1
	CT_INT32		iItemNum1;						//道具数量1
	CT_INT32		iItemID2;						//道具id2
	CT_INT32		iItemNum2;						//道具数量2
	CT_INT32		iGrade;							//领主等级
};

//扫荡加速记录
struct MSC_LOGDB_RaidsSpeed
{
	CT_DWORD		dwUserID;						//用户ID
	CT_INT32		iDiamod;						//钻石
	CT_INT32		iGrade;							//等级
};

//军衔俸禄
struct MSC_LOGDB_Salary
{
	CT_DWORD		dwUserID;						//用户Id
	CT_INT32		iFood;							//食物
	CT_INT32		iGold;							//金币
};

//军衔升级
struct MSC_LOGDB_RankingUpGrade
{
	CT_DWORD		dwUserID;						//用户id
	CT_INT32		iGrade;							//等级
	CT_INT32		iGradeUp;						//升级数目
	CT_INT32		iRankxp;						//消耗经验
};

//物品使用
struct MSC_LOGDB_UserItem
{
	CT_DWORD		dwUserID;						//用户Id
	CT_INT32		iItemID;						//物品ID
	CT_INT32		iItemNum;						//
};

struct MSC_LOGDB_AddItem
{
	CT_DWORD		dwUserID;						//用户Id
    CT_DWORD        saveTag;                        //操作
	CT_INT32		iItemID;						//物品ID
	CT_INT32		iItemNum;						//
};

//事件奖励
struct MSC_LOGDB_EventReward
{
	CT_DWORD		dwUserID;						//用户Id
	CT_INT32        iGold;      //金币
	CT_INT32        iCiviliNum; //文明度
	CT_INT32        iXp;        //经验
	CT_INT32        iFood;      //食物
	CT_INT32        iProp1;     //道具1类型
	CT_INT32        iProp2;     //道具2类型
	CT_INT32        iNum1;     //道具1数量
	CT_INT32        iNum2;     //道具2数量
};

//宠物重命名记录
struct MSC_LOGDB_PetReName
{
	CT_DWORD		dwUserID;				//用户id
	CT_INT32		iIndex;					//宠物索引
	CT_INT32		iPetID;					//宠物ID
	CT_INT32        iProp1;     //道具1类型
	CT_INT32        iNum1;     //道具1数量
};

//宠物升级日志
struct MSC_LOGDB_PetUpGrade
{
	CT_DWORD		dwUserID;			//用户id
	CT_INT32		iPetGrade;			//等级
	CT_INT32		iExp;				//消耗经验
};

//城墙加成日志
struct MSC_LOGDB_WallAddition
{
	CT_DWORD		dwUserID;					//用户id
	CT_INT32		iResID[TEAM_RES_CNT];		//资源ID
	CT_INT32		iResCount[TEAM_RES_CNT];	//资源数量
};

//掠夺日志
struct MSC_LOGDB_Plunder
{
	CT_DWORD		dwUserID;
	CT_INT32		iFood;
	CT_INT32		iGold;
	CT_DWORD		dwDstUserID;
};


//用户登录信息插入
//////////////////////////////////////////////////////////////////////////
//插入领主信息
struct MSC_DB_InsertMasterInfo
{
	CT_DWORD		dwUserID;
	MasterData		MasterInfo;
};

//得到正在升级建筑的信息
struct MSC_DB_InsertUpGradeBuild
{
	CT_DWORD		dwUserID; // 用户id
	CT_INT32        iBid;     //建筑id
	CT_INT32        iLasting; //持续时间
	//CT_INT32        iDiamod;  //加速使用的钻石
	CT_LONGLONG     llBeginTime; //开始时间
	CT_INT32        iGrade;     //即将达到的等级
};
//得到潜能数据
struct  MSC_DB_InsertTalentInfo
{
	TalentData		talentData;
	CT_DWORD		dwUserID;
};

//得到技能数据
struct  MSC_DB_InsertSkillData
{
	SkillData		skillData;
	CT_DWORD		dwUserID;
};

//得到道具数据|元素之力
struct  MSC_DB_InsertItemInfo
{
	ItemData		itemData;
	CT_DWORD		dwUserID;
};

//得到装备信息
struct  MSC_DB_InsertEquipInfo
{
	EquipmentData	equipData;
	CT_DWORD		dwUserID;
};
// 得到元素属性数据
struct  MSC_DB_InsertElmAttrInfo
{
    ElmMarrowAttr elmData;
    CT_DWORD      dwUserID;
};
// 得到战旗数据
struct  MSC_DB_InsertBattleFlagInfo
{
    BattleFlag    flagData;
    CT_DWORD      dwUserID;
};
//得到增益数据
struct  MSC_DB_InsertBuffInfo
{
	BuffData		buffData;
	CT_DWORD		dwUserID;
};
//得到冷却信息
struct  MSC_DB_InsertCooldownInfo
{
	CooldownData	cdData;
	CT_DWORD		dwUserID;
};

//得到冷却信息
struct  MSC_DB_InsertArmyInfo
{
	ArmyInfo	    Armydata;
	CT_DWORD		dwUserID;
};

//得到正在生产的建筑
struct  MSC_DB_InsertProductingBuilding
{
	ProductingBuilding	Producting;
	CT_DWORD		dwUserID;
};

//得到世界场景数据
struct  MSC_DB_InsertWorldScene
{
	MSC_DB_WorldSceneInfo	data;
	CT_DWORD		dwUserID;
};

//得到场景数据
struct  MSC_DB_InsertSceneInfo
{
	//MSC_DB_SceneInfo		data;
	SceneInfo		data;
	CT_DWORD		dwUserID;
};

//得到战斗次数
struct  MSC_DB_InsertBuyFightingTime
{
	CT_DWORD		dwUserID;
	CT_INT32		iBuyFightingTime;
};

//得到用户解锁区域
struct  MSC_DB_InsertRegionInfo
{
	MSC_DB_R_UnlockRegion	unlockRegion;
	CT_DWORD		dwUserID;
};

//得到阵法信息
struct  MSC_DB_InsertBattleArray
{
	MSC_DB_BattleArray		data;
	CT_DWORD		dwUserID;
};

//得到用户已完成任务
struct  MSC_DB_InsertTaskFinish
{
	MSC_DB_TaskFinishData	taskFinishData;
	CT_DWORD		dwUserID;
};

//得到用户在进行任务
struct  MSC_DB_InsertProgressData
{
	MSC_DB_TaskProgressData	taskPData;
	CT_DWORD		dwUserID;
};

//得到签到信息
struct  MSC_DB_InsertDailySignInfo
{
	SignData		stInfo;
	CT_DWORD		dwUserID;
};

//得到活跃度信息
struct  MSC_DB_InsertDailyQuestInfo
{
	DailyQuestData			stInfo;
	CT_DWORD		dwUserID;
};

//得到日常任务进度信息
struct  MSC_DB_InsertDailyActive
{
	ActiveData				stInfo;
	CT_DWORD		dwUserID;
};

//得到建筑信息
struct  MSC_DB_InsertBuildingInfo
{
	BuildingData			buildData;
	CT_DWORD		dwUserID;
};

//得到商店信息
struct  MSC_DB_InsertMarketFunction
{
	MSC_DB_MarketFunction	marketFun;
	CT_DWORD		dwUserID;
};

//商店个人限购
struct MSC_DB_InsertMarketPersonal
{
	CT_DWORD		dwUserID;
	CT_INT32		iGoods;
	CT_INT32		iNum;	
};

//得到建筑信息
struct  MSC_DB_InsertBlackMarketInfo
{
	MSC_DB_BlackMarketRefresh	blackMarket;
	CT_DWORD		dwUserID;
};

//得到宠物信息
struct  MSC_DB_InsertPetInfo
{
	CT_DWORD		dwUserID;
	PetInfo					pi;
};

//宠物幻化
struct MSC_DB_InsertPetChange
{
	CT_DWORD		dwUserID;
	CT_INT32		iChangeID;
	CT_INT32		iSkillID;
};

//获取vip信息
struct  MSC_DB_InsertVipInfo
{
	CT_INT32		iGrade;
	CT_INT32		iTime;
	CT_INT32		iGift;
	CT_DWORD		dwUserID;
};

//竞技场信息
struct  MSC_DB_InsertPVPInfo
{
	UserFightingStatus  UserFightinginfo[PVP_CHALLENGE_COUNT];//用户战斗信息
	LookUserInfo		LookUserinfo[PVP_CHALLENGE_COUNT];	//查看用户信息
	PVPInfo			PVPdata;
	CT_DWORD		dwUserID;
	CCNetInterface*   pNetInterface;   //用户通讯接口
	CT_DWORD          dwThreadID;      //线程id	
};

//排行榜信息
struct  MSC_DB_InsertTop
{
	CT_DWORD		dwUserID;
	CT_INT32		iType;
	UserFightingStatus  UserFightinginfo[TOP_COUNT];//用户战斗信息
	LookUserInfo		LookUserinfo[TOP_COUNT];	//查看用户信息
};

//PVP战报记录
struct  MSC_DB_InsertPVPRecord
{
	CT_DWORD				dwUserID;
	MSC_PVP_Fighting_Info	FightingInfo[PVP_FIGHTING_COUNT];
	MSC_DB_FighitngRecord	FightingRecord[PVP_FIGHTING_COUNT];
};

//获取附魔信息
struct  MSC_DB_InsertEnchantData
{
	CT_DWORD		 dwUserID;
	CT_INT32         iBID;               // 部位
	CT_INT32         iIDX;               // 序号
	CT_INT32         iGrade;             // 附魔等级 
	CT_INT32		 iFailCount;		 // 失败次数
};

//获取附魔奖励信息
struct MSC_DB_InsertEnchantReward
{
	CT_DWORD		dwUserID;
	CT_INT32		iIndex;				// 奖励索引
};

//获取扫荡信息
struct	MSC_DB_InsertRaidsInfo
{
	CT_DWORD		dwUserID;			//用户id
	CT_INT32		iWorldID;			//世界场景
	CT_INT32		iSceneID;			//场景ID
	CT_INT32		iDifficulty;		//难度
	CT_INT32		iAllTime;			//总次数
	CT_INT32		iCurrentTime;		//当前次数
	CT_INT64		lBeginTime;			//开始时间
	CT_INT32		iLastStamina;		//剩余体力
};

//扫荡奖励
struct MSC_DB_InsertRaidsReward
{
	CT_INT32		iCurrentTime;						//当前次数
	CT_DWORD		dwUserID;							//用户ID
	CT_INT32		iExperience;						//经验
	CT_INT32		iGold;								//金币
	CT_INT32		iDiamond;							//钻石
	CT_INT32		iPotential;							//潜力点
	CT_INT32		iSkillPoint;						//技能点强化点
	CT_INT32		iGoods[GOODS_COUNT];				//装备ID
	CT_INT32		iGoodsCount[GOODS_COUNT];			//数目
};

//扫荡奖励
struct MSC_DB_InsertTeamDuplicateScene
{
	CT_DWORD		dwUserID;							//用户id
	CT_INT32		iSceneID;							//场景ID
	CT_INT32		iDifficulty;						//难度
	CT_INT32		iWin;								//是否胜利
};

//战队科技
struct MSC_DB_InsterFightTeamScience
{
	CT_DWORD		dwUserID;							//用户id
	CT_INT32		iScienceID;							//科技id
	CT_INT32		iGrade;								//科技等级
};


//活动状态信息
struct MSC_DB_ActiveEventStatus
{
	CT_DWORD		dwUserID;       //用户id
	CT_INT32		iAID;			//活动id
	CT_INT32		iStatus;		//活动状态
	CT_INT32		iQuantity;		//活动积累
};
//转盘状态信息
struct MSC_DB_Turntable
{
	CT_DWORD		dwUserID;       //用户id
	CT_INT32		iTimes[3];		
};

struct MSC_DB_CDKEY
{
	CT_DWORD		dwUID;
	CT_CHAR			cCDKEY[32];       
	CT_INT32		iType;
};

struct MSC_DB_RESREWARD
{
	CT_DWORD		dwUID;
	CT_INT32		iPID;
	CT_INT32		iPNum;
};

//记录变化的活动
struct MSC_DB_ActiveEventChange
{
	CT_INT32		iActive[ACTIVE_CHANGENUM];		//变化的活动
	CT_INT32		iNum;			//数量
};

//祭祀状态信息
struct MSC_DB_WorshipStatus
{
	CT_DWORD		dwUserID;       //用户id
	CT_INT32		iType;			//祭祀类型
	CT_INT32		iStatus;		//祭祀状态
	CT_INT32		iXp;			//经验/钻石祭祀次数
};

//信息反馈
struct MSC_DB_FeedBack
{
	CT_DWORD			dwUserID;								// 用户ID
	CT_CHAR				cMobile[MOBILE];						//手机号码
	CT_CHAR				cEmailContent[FEEDBACK_CONTENT_SIZE];	//内容最大长度
};

struct MSC_DB_FRIEND
{
	CT_DWORD			dwU_ID;
	CT_DWORD			dwF_UID;
	CT_CHAR				cF_NickName[NAME_SIZE];
	CT_INT32			iF_Occupation;
	CT_INT32			iF_Type;
	CT_INT32			iF_Grade;
	CT_INT32			iF_FightingPower;			// 战斗力(离线状态不计算战斗力)
	CT_INT64			iLastExitTime;				// 最后下线时间
	CT_INT32			iTitleID;					// 使用的头衔ID
};

struct MSC_DB_FRIEND_DELETE
{
	CT_DWORD			dwU_ID;
	CT_DWORD			dwF_UID;
	CT_INT32			iF_Type;
};

struct MSC_DB_FightTeamBase
{
	CT_DWORD	dwFTID;							// 战队ID
	CT_CHAR		cFightTeamName[NAME_SIZE];		// 战队名称
	CT_CHAR		cDescription[DESCRIPTION_SIZE]; // 战队介绍
	CT_DWORD	dwBossUID;						// 首领ID
	CT_CHAR		cBossName[NAME_SIZE];			// 首领名称
	CT_INT32	iFightingPower;					// 总战斗力
	CT_INT32	iTop;							// 排名
	CT_INT32	iPreTop;						// 上次排名
	CT_INT32	iResource;						// 资源数
	CT_INT32	iIcon;
	CT_DWORD	dwElite1;						// 出战精英1
	CT_DWORD	dwElite2;						// 出战精英2
	CT_DWORD	dwElite3;						// 出战精英3
	CT_INT32	iResCount;						// 资源点数

	CT_INT32	iGrade;							//等级
	CT_INT32	iItemCount;						//道具资源数
	CT_INT32	iSummonCount;					//召唤次数
	CT_INT32	iBossMaxIndex;					//最大BOSS索引
	CT_INT32	iTimeBossMaxIndex;				//时限BOSS最大索引
};


struct MSC_DB_FightTeamMember
{
	CT_DWORD	dwFTID;							// 战队ID
	CT_DWORD	dwUserID;						// 队员ID
	CT_CHAR		cNickName[NAME_SIZE];			// 队员名称
	CT_INT32	iFightingPower;					// 队员战斗力
	CT_INT32	iOccupation;					// 职业
	CT_INT32	iGrade;							// 等级
	CT_INT32	iRank;							// 军衔
	CT_INT32	iContribution;					// 贡献
	CT_INT32	iStatus;						// 状态 0.空闲 1.驻守
	CT_INT64	iTime;							// 加入/申请时间
	CT_INT32	iTowerCPID;						// 元素塔最大通关层数
	CT_INT32	iContributeCount[CONTRIBUTE_TYPE];//捐献次数
	CT_INT32	iContributeGross;					//捐献总量
	CT_INT64	iLastExitTime;
	CT_INT32	iTitle;							// 成员头衔ID 
	CT_INT32	iType;							// 成员类型 1:成员 2:申请待审核 3:邀请待答复
};


struct MSC_DB_FightTeamMemberDelete
{
	CT_DWORD	dwFTID;							// 战队ID
	CT_DWORD	dwUserID;						// 队员ID
	CT_INT32	iType;							// 成员类型 1:成员 2:申请待审核 3:邀请待答复
};

struct MSC_DB_FightTeamRecruit
{
	CT_DWORD	dwFTID;							// 战队ID
	CT_INT64	iBeginTime;						// 发布时间
	CT_INT32	iLastingTime;					// 时长
	CT_CHAR		cRecruit[DESCRIPTION_SIZE];		// 招募信息
	CT_INT32	iAuto;							// 创建战队自动招募
};

struct FightTeamMemberUID
{
	CT_DWORD	dwCount;						// 成员数
	CT_DWORD	dwMemberUID[CLAN_MAX_MEMBER];	// 成员UID
};

struct FightTeamFightData
{
	UserFightingStatus	userFightingLeft[SPOT_GUARD_MEMBER];
	UserFightingStatus	userFightingRight[SPOT_GUARD_MEMBER];
	CT_INT32			iSeed;					//随机种子
	CT_INT32			iSpotQuality;			//资源点品质
	CT_BOOL				bPVP;					//TRUE:PVP, FALSE:PVE
	CT_DWORD			dwLeftFTID;
};

struct MSC_DB_FTMemberFightingData
{
	CT_DWORD			dwUserID;        //用户id
	UserFightingStatus	userFightingData;		//战斗信息
};

//彭文添加
struct MSC_DB_FTMemeberRefresh 
{
	CT_DWORD			dwUser;         //用户id
	CT_INT32			iGrade;         //用户等级
	CT_INT32			iFightingPower; //战斗力
};

//召唤战斗
struct MSC_FightTeamSummonFighting
{
	CT_DWORD			dwUserID[SUMMON_MAX];				//召唤人ID
	CT_DWORD			dwFTID;					//战队ID
	CT_INT32			iWin;					//胜利标识
	CT_INT32			iBossIndex;				//BOSS索引
	CT_INT32			iSeed;
	UserFightingStatus	FightingData[CLAN_CRUSADE_MEMBER];	//出战精英
};
//科技升级
struct MSC_DB_FightTeamScienceUpGrade
{
	CT_DWORD			dwUserID;
	CT_INT32			iScienceID;
	CT_INT32			iGrade;
};
//
struct MSC_DB_FightTeamEventLog
{
	CT_DWORD dwFTID;
	CT_INT64 lDate;                          // 事件发生的时间
	CT_INT32 iReport;                        // 事件相关的报告ID（战报）
	CT_CHAR  cEventDesc[DESCRIPTION_SIZE];   // 事件描述
};

//战队召唤
struct MSC_DB_FightTeamSummonInfo
{
	CT_DWORD			dwFTID;							//战队ID
	CT_DWORD			dwUserID;			//用户id
	CT_INT32			iBossIndex;						//BOSS索引
	CT_INT32			iWin;				//胜利
	//CT_DWORD			dwChestIndex;		//宝箱索引
	CMD_G_NormalReward	Res;				//资源
};

//战队召唤道具
struct MSC_DB_FightTeamSummInfoItem
{
	CT_DWORD			dwFTID;							//战队ID
	CT_DWORD			dwUserID;						//用户id
	CT_INT32			iChestIndex;					//宝箱索引
	CT_INT32			iItemID;						//道具ID
	CT_INT32			iCount;							//道具数量
	CT_INT32			iType;							//类型
};

//战队科技惩罚
struct MSC_DB_FightTeamScienceDownGrade
{
	CT_DWORD			dwUserID;						//用户Id
};

struct MSC_LOGDB_FightTeam
{
	CT_DWORD dwUserID;
	CT_INT32 iGoldCost;
	CT_INT32 iFoodCost;
	CT_INT32 iDiamodCost;
	CT_INT32 iGold;
	CT_INT32 iGrade;
	CT_INT32 iItemID;
	CT_INT32 iItemCount;
};

//战队升级
struct MSC_LOGDB_FightTeamUpGrade
{
	CT_DWORD dwFTID;		//战队ID
	CT_DWORD dwUserID;		//队长ID
	CT_INT32 iRes;			//资源数目
	CT_INT32 iItemRes;		//道具资源
	CT_INT32 iGrade;		//战队等级
};

//战队召唤消耗
struct MSC_LOGDB_FightTeamSummonCost
{
	CT_DWORD dwFTID;		//战队ID
	CT_DWORD dwUserID;		//队长ID
	CT_INT32 iRes;			//资源数目
	CT_INT32 iItemRes;		//道具资源
	CT_INT32 iBossIndex;	//boss索引
	CT_INT32 iGrade;		//用户等级
};

//召唤奖励资源
struct MSC_LOGDB_FightTeamSummonRewardRes
{
	CT_DWORD dwFTID;		//战队ID
	CT_DWORD dwUserID;		//用户ID
	CT_INT32 iBossIndex;	//召唤BOSS
	CMD_G_NormalReward Reward;//资源奖励
	CT_INT32 iGarde;		//用户等级
};

//召唤奖励道具
struct MSC_LOGDB_FightTeamSummonRewardItem
{
	CT_DWORD dwFTID;		//战队ID
	CT_DWORD dwUserID;		//用户ID
	CT_INT32 iBossIndex;	//召唤BOSS
	CT_INT32 iItemID[4];		//道具ID
	CT_INT32 iItemCount[4];	//道具数目
	CT_INT32 iGarde;		//用户等级
};

//战队科技升级
struct MSC_LOGDB_FightTeamScienceUpGrade
{
	CT_DWORD dwFTID;
	CT_DWORD dwUserID;
	CT_INT32 iScienceID;
	CT_INT32 iScienceGrade;
	CT_INT32 iCostType;
	CT_INT32 iCostNum;
	CT_INT32 iUserGrade;
};

//召唤BOSS战斗
struct MSC_FTTimeBossFight
{
	CT_DWORD			dwUserID;				//召唤人ID
	CT_DWORD			dwFTID;					//战队ID
	CT_INT32			iWin;					//胜利标识
	CT_INT32			iBossIndex;				//BOSS索引
	CT_INT32			iSeed;
	CT_INT32			iHurt;					//伤害
	UserFightingStatus	FightingData[CLAN_CRUSADE_MEMBER];	//出战精英
};

//清理时限boss
struct MSC_DB_FTTBClear
{
	CT_DWORD			dwFTID;					//战队id
};

//时限boss信息
struct MSC_DB_FTTimeBossInfo
{
	CT_DWORD			dwFTID;					//战队Id
	CT_INT32			iIndex;							//索引
	CT_INT32			iSummonBeginTime;					//召唤开始时间
	CT_INT32			iCurrentHP;						//当前hp
	CT_INT32			iHelpUser[HELP_SUMMON_COUNT];			//助阵用户
};

//时限boss成员信息
struct MSC_DB_FTTimeBossMember
{
	CT_DWORD			dwFTID;					//战队Id
	CT_DWORD			dwUserID;
	CT_INT32			iHurt;								//伤害
	CT_INT32			iFTFood;							//战队粮食
	CT_INT32			iFTContribute;						//战队贡献
	CT_INT32			iFightNum;							//战斗次数
};
//组队副本
//////////////////////////////////////////////////////////////////////////
//用户离开
typedef CMD_G_TeamDuplicateLeave MSC_SUB_TeamDuplicateLeave;

//用户资源生产信息
struct MSC_DB_ResourceProduct
{
	CT_DWORD        dwUserID;         //用户id
	CT_INT32        iIndex;           //生产索引
	CT_INT32        iSenceID;         //场景id
	CT_INT32        iSpotID;          //矿点id
	CT_INT32        iGrade;           //用户等级
	CT_INT32        iProductStatus;   //生产状态
	CT_INT32		iProductCount;    //已经生产的次数
	CT_INT32        iLeftProductTime; //本次生产剩余时间

};

//排行榜
struct MSC_DB_Top
{
	CT_DWORD		dwUserID[TOP_COUNT];	//用户ID
	ENUM_TOP		TopType;				//类型
};

//排行榜信息
struct MSC_DB_TopData
{
	CT_DWORD		dwUserID[TOP_COUNT];		//用户ID
	User_Fighting	FightingData[TOP_COUNT];	//用户数据
	ENUM_TOP		TopType;					//类型
};

//资源生产记录生成的原因
enum ENUM_RES_Propduct_Reason
{
	ENUM_RPODUCT_REASON_PRODUCTED = 0, //生产
	ENUM_PRODUCT_REASON_KICK_OUT  = 1, //踢出
	ENUM_PRODUCT_REASON_DISSOLUTION = 2, //队伍解散
	ENUM_PRODUCT_REASON_OCCUPY = 3, //占领
	ENUM_PRODUCT_REASON_GIVEUP = 4, //放弃
};

//资源点生产记录
struct   MSC_DB_ResourceProductRecord
{
	CT_DWORD		dwFID;   //战队id
	CT_DWORD		dwUID;   //用户id	
	CT_INT32		iScenceID; //场景id
	CT_INT32        iSpotID;     //资源点id
	//CT_INT32        iGold;     //得到金币
	CT_INT32		iRes;		//资源
	CT_INT32		iResType;	//资源类型
	ENUM_RES_Propduct_Reason        iReason;   //原因
};

//资源点占领消耗记录
struct MSC_DB_ResourceOccupyRecord
{
	CT_DWORD       dwFID;     //战队id
	CT_INT32       iScenceID; //场景id
	CT_INT32       iSpotID;   //资源点id
	CT_INT32       iFTeamRes; //消耗的战队资源
	CT_INT32       iOccupyGold; //抢来的金币
};

struct MSC_DB_Present
{
	CT_DWORD		dwUID;
	CT_INT32		iIndex;
	CT_INT32		iP_ID;
	CT_INT32		iP_Count;
	CT_INT32		iSLotID;
	CT_CHAR			cDesc[255];
};

struct MSC_DB_TOWER
{
	CT_DWORD			dwU_ID;
	CT_INT32			iCPID;
	CT_INT32			iTreasure;
	CT_INT32			iTime;
};

struct MSC_DB_TurntableRelation
{
	CT_INT32			iTurntableID;
	CT_INT32			iPos;
	CT_INT32			iWeight;
	CT_INT32			iRewardID;
	CT_INT32			iFakeProID;
	CT_INT32			iType;
	CT_INT32			iID;
	CT_INT32			iCount;
};

struct MSC_DB_TowerHelp
{
	CT_DWORD			dwU_ID;
	CT_DWORD			dwF_ID;
};

//扫荡
struct MSC_DB_RAIDS
{
	CT_DWORD			dwU_ID;
	CT_INT32			iBeginID;
	CT_INT32			iEndID;
};

//全局数据
struct MSC_DB_GlobeData
{
	CT_INT32			iType;
	CT_INT32			iValue;
};


//资源点生产奖励
struct MSC_DB_TowerSlotReward
{
	CT_DWORD  dwUserID;                 //用户id
	CT_INT32  iFloor;				    //层
	CT_INT32  iSlot;                    //资源点
	G_NormalReward mnReward;			//奖励
};

struct MSC_DB_TowerSlotRewardDel
{
	CT_DWORD  dwUserID;                 //用户id
	CT_INT32  iFloor;				    //层
	CT_INT32  iSlot;                    //资源点
};

struct MSC_DB_MSG
{
	CT_DWORD  dwUserID;                 //用户id
	CT_INT32  iMSGType;					//消息类型 1.元素塔 2.竞技场
	CT_INT64  iTime;				    //时间
	CT_CHAR  cMSG[255];                 //MSG内容
};

struct MSC_DB_MSG_DELETE
{
	CT_DWORD  dwUserID;                 //用户id
	CT_INT32  iMSGType;					//消息类型 1.元素塔 2.竞技场
	CT_INT64  iTime;				    //时间
};


struct MSC_DB_TowerFight
{
	CT_INT32		iCPID;								//关卡ID
	CT_INT32		iFID;								//战斗ID
	CT_DWORD		dwUserID;							//用户ID
	CT_INT32		iSeed;								//随机种子
	CT_INT32		iScore;								//成绩

	UserFightingStatus	FightingData;					//战斗数据
};

struct MSC_DB_TowerSlotFight
{
	CT_INT32		iFID;								//战斗ID
	CT_DWORD		dwUserID;							//用户ID
	CT_INT32		iSeed;								//随机种子
	CT_INT32		iScore;								//成绩

	UserFightingStatus	LeftFightingData;				//挑战方战斗数据
	UserFightingStatus	RightFightingData;              //被挑战方的战斗数据
};

struct MSC_FriendExit
{
	CT_DWORD	dwUserID;
	CT_DWORD	dwFriendID;
	CT_INT32	iFType;
	CT_INT64	iExitTime;
};

struct MSC_LOGDB_Tower
{
	CT_DWORD	dwUserID;
	CT_INT32	iBeginCPID;
	CT_INT32	iEndCPID;
	CT_INT32	iResType[4];
	CT_INT32	iResNum[4];
	CT_INT32	iUGrade;
};

struct MSC_LOGDB_TowerSlot
{
	CT_DWORD	dwUserID;
	CT_INT32	iCPID;
	CT_INT32	iSlotID;
	CT_INT32	iResType[4];
	CT_INT32	iResNum[4];
	CT_INT32	iUGrade;
};

struct MSC_LOGDB_TowerHelp
{
	CT_DWORD	dwUserID;
	CT_DWORD	dwFUID;
	CT_INT32	iCPID;
	CT_INT32	iSlotID;
	CT_INT32	iResType[4];
	CT_INT32	iResNum[4];
	CT_INT32	iUGrade;
};

struct MSC_LOGDB_TreasureUse
{
	CT_DWORD	dwUserID;
	CT_INT32	iCPID;
	CT_INT32	iTreasureID;
	CT_INT32	iUseType;
	CT_INT32	iResType[4];
	CT_INT32	iResNum[4];
	CT_INT32	iUGrade;
};

struct MSC_LOGDB_BattleFlag
{
	CT_DWORD	dwUserID;
	CT_INT32	iFlag;
	CT_INT32	iGrade;
	CT_INT32	iUGrade;
};

struct MSC_LOGDB_EatElm
{
	CT_DWORD	dwUserID;
	CT_INT32	iEatID;
	CT_INT32	iEatenID;
	CT_INT32	iCount;
	CT_INT32	iUGrade;
};

struct MSC_LOGDB_EmbedEquip
{
	CT_DWORD dwUserID;              // 用户ID
	CT_DWORD key;                   // 唯一标识
	CT_INT32 iGemID;                // 宝石ID
	CT_INT32 iSlot;                 // 孔位置
	CT_INT32 iGrade;				// 用户等级
};

struct MSC_Tower_Pass
{
	CT_DWORD dwUserID;
	CT_INT32 iCPID;
};


//gm广播 pengwen
struct MSC_Broadcast_GM
{
	CT_DWORD		dwMainID;   //消息主id
	CT_DWORD		dwSubID;	//消息子id
	CMD_G_GMMessage gm;			//gm消息体
};


//pvp插入用户 彭文
struct MSC_PVP_InsertChallger
{
	CT_DWORD dwUserID;         //用户id
	CT_DWORD dwChallger;       //对手id
	CT_INT32 iPos;             //位置
};


//查看别人信息 彭文
struct MSC_PVP_OtherInfo
{
	CT_DWORD dwUserID;        //用户id
	LookUserInfo otherUserInfo;    //他人信息
};


//普通活动添加 彭文
struct MSC_ACTIVE_Status
{
	CT_DWORD dwUserID; 
	CT_INT32 iAID;
	CT_INT32 iStatus;
	CT_INT32 iQuanity;
};

//转盘活动添加 彭文
struct MSC_TurnTable_Status
{
	CT_DWORD  dwUserID; 
	CT_INT32  iTimes1;    
	CT_INT32  iTimes2;
	CT_INT32  iTimes3;
};

//世界BOSS战斗
struct MSC_WorldBoss_Fighting
{
	CT_DWORD		dwUserID;							//发起用户ID
	CT_INT32		iSeed;								//随机种子
	CT_INT32        iWin;                               //发起方是否赢了
	CT_INT32		iHPHurt;							//伤害值

	UserFightingStatus	LeftFightingData;				//挑战方战斗数据
	CT_INT32		iMonsterGroupID;					//世界bossID
};

//战队升级任务
struct MSC_FightTeamTask 
{
	CT_INT32	iTaskID;								//主任务id
	CT_INT32	iSubTaskID;								//子任务id
	CT_INT32	iType;									//1:登录时取得任务，2：新增加任务
	CT_DWORD	dwUserID;								//用户ＩＤ
	CT_INT32	iFightTeamGrade;						//战队等级
	SystemTaskType	sysTask;							//配置对应内容
};

//修改战队名称
struct MSC_ChangeFTName
{
	CT_DWORD	dwUserID;
	CT_CHAR		cName[NAME_SIZE];
};

struct MSC_DB_WorldBossTopClear
{
	CT_INT32	iAEID;
};

struct MSC_DB_WorldBossTop
{
	CT_INT32	iAEID;
	CT_INT32	iTop;
	CT_DWORD	dwUID;
	CT_INT32	iHP;
	CT_CHAR		cUserName[NAME_SIZE];
	CT_FLOAT	fHurtPercent;
};

struct MSC_DB_WorldBossRecord
{
	CT_DWORD	dwUID;
	CT_INT32	iAEID;
};

//用户数据缓存
struct MSC_SUB_UserdataInsert
{
	UserFightingStatus			FData;
	UserFightingStatus			SeekFData;		//探访战斗数据
	LookUserInfo				LookData;
	CT_INT32					iTime;			//数据存留时间
};

//跨服战斗
struct MSC_CS_FIGHTING_DATA
{
	CT_DWORD	dwUserID;
	CT_INT32	iWiner;
	CT_INT32	iSeed;
	CT_INT32	iPos[2];
	CT_BOOL		bLookLeft;
	UserFightingStatus	LeftFightingData[TEAM_NUM];					//战斗数据
	UserFightingStatus	RightFightingData[TEAM_NUM];					//战斗数据
};

//威能升级
struct MSC_DB_INSPORE
{
	CT_DWORD	dwUserID;
	CT_INT32	iID;
	CT_INT32	iExp;
	CT_INT32	iGrade;
};

//奖励记录
struct MSC_DB_CS_REWARD
{
	CT_DWORD	dwUserID;
	CT_INT32	iResType;
	CT_INT32	iResNum;
	CT_INT32	iResID;
};

//删除奖励
struct MSC_DB_CS_DelReward
{
	CT_DWORD	dwUserID;
	CT_INT32	iResType;
	CT_INT32	iResID;
};
//个人支持
struct MSC_DB_CS_SUPPORT
{
	CT_DWORD	dwUserID;
	CT_DWORD	dwFTID;
	CT_INT32	iGSID;
	CT_INT32	iResID;
	CT_INT32	iNum;
	CT_INT32	iIndex;
};

//威能消耗日志
struct MSC_LOGDB_CS_INSPORECOST
{
	CT_DWORD	dwUserID;
	CT_INT32	iID;
	CT_INT32	iGrade;
	CT_INT32	iExp;
	CT_INT32	iResID[4];
	CT_INT32	iResNum[4];
	CT_INT32	iItemID[2];
	CT_INT32	iItemNum[2];
};

//跨服奖励日志
struct MSC_LOGDB_CS_REWARD
{
	CT_DWORD	dwUserID;
	CT_INT32	iResType;
	CT_INT32	iResID;
	CT_INT32	iNum;
};

//个人支持日志
struct MSC_LOGDB_CS_SUPPORT
{
	CT_DWORD	dwUserID;
	CT_INT32	iResID;
	CT_INT32	iNum;
	CT_DWORD	dwFTID;
};

//跨服战斗
struct MSC_TOP_CHALLENGE
{
	CT_DWORD	dwUserID;
	CT_INT32	iWiner;
	CT_INT32	iSeed;
	CT_BOOL		bLookLeft;
	UserFightingStatus	LeftFightingData;					//战斗数据
	UserFightingStatus	RightFightingData;					//战斗数据
};

//头衔
struct MSC_DB_TitleData
{
	CT_DWORD	dwUserID;				// 用户ID
	CT_INT32	iTitleID;				// 头衔ID
	CT_INT32	iActiveTime;			// 激活时间
	CT_INT32	iEffectTime;			// 有效时间
	CT_INT32	iStatus;				// 头衔状态
};

//头衔列表数据
struct MSC_DB_TitleList
{
	CT_DWORD	dwUserID;				// 用户ID
	CT_INT32	iTitleID;				// 头衔ID
};

//头衔日志
struct MSC_LOGDB_Title 
{
	CT_DWORD	dwUserID;				// 用户ID
	CT_INT32	iTitleID;				// 头衔ID
	CT_INT32	iStatus;				// 头衔状态，0：未激活，1：激活，2：使用，3：取消使用
	CT_INT32	iLastTime;				// 头衔有效时间
};

//龙岛
struct MSC_DB_IslandData 
{
	CT_DWORD	dwUserID;				// 用户ID
	CT_INT32	iPoint;					// 棋点
	CT_INT32	iEventID;				// 棋点事件ID
	CT_INT32	iEventStatus;			// 棋点事件状态，0：未完成，1：完成，2：奖励已领取
	CT_INT32	iThrowTimes;			// 投掷次数
	CT_INT32	iStartTimes;			// 当天经过起始点的次数
	CT_INT32	iStartChest;			// 起点宝箱，没有为0
	CT_INT32	iGetTimes;				// 当天获赠次数
	CT_INT32	iFT_ID;					// 更新时记录的战队ID
	CT_INT32	iScore;					// 战队积分
	CT_INT32	iIsOpen;				// 龙岛是否开启
	CT_INT32	iRewardTimes;			// 宝箱获得次数
	CT_INT32	iStep;					// 投掷点数
};

struct MSC_DB_IslandGiveUid 
{
	CT_DWORD	dwUserID;							// 用户ID
	CT_INT32	aGiveUid;							// 已赠送的Uid
};

struct MSC_DB_IslandScoreReward
{
	CT_DWORD	dwUserID;							// 用户ID
	CT_INT32	aScoreReward;						// 已获得的战队积分宝箱
};

struct MSC_DB_IslandPresentUserName
{
	CT_DWORD	dwUserID;							// 用户ID
	CT_CHAR		cUserName[NAME_SIZE+1];			// 用户昵称
};

struct MSC_DB_IslandGiveTimes 
{
	CT_DWORD	dwUserID;				//用户ID
	CT_DWORD	dwFUID;					//队友ID
	CT_DWORD	iGetTimes;				//队友当天已获赠次数
};

struct MSC_DB_IslandFTMemberScore
{
	CT_DWORD	dwUserID;				//用户iD
	CT_INT32	iTeamID;				//战队ID
	CT_INT32	iScore;					//积分
};

//龙岛日志
struct MSC_LOGDB_IslandData 
{
	CT_DWORD	dwUserID;				// 用户ID
	CT_INT32	iPoint;					// 棋点
	CT_INT32	iEventID;				// 棋点事件ID
	CT_INT32	iEventStatus;			// 棋点事件状态，0：未完成，1：完成，2：奖励已领取
	CT_INT32	iThrowTimes;			// 投掷次数
	CT_INT32	iStartTimes;			// 当天经过起始点的次数
	CT_INT32	iGetTimes;				// 当天获赠次数
	CT_INT32	iFT_ID;					// 更新时记录的战队ID
	CT_INT32	iScore;					// 战队积分
	CT_INT32	iRewardTimes;			// 奖励获得次数
};

//龙岛战斗
struct MSC_Island_FightEvent 
{
	CT_DWORD		dwUserID;							//用户ID
	CT_INT32		iSeed;								//随机种子
	CT_INT32		iMonsterID;							//怪物组ID
	CT_INT32		iEventID;							//事件ID
	CT_INT32		iStatus;							//事件结果

	UserFightingStatus	FightingData;					//战斗数据
};

//龙岛任务
struct MSC_IslandTask
{
	CT_DWORD		dwUserID;							//用户ID
	CT_INT32		iTaskID;							//任务ID
};

//探访

//得到探访好友/随机玩家
struct MSC_Seek_FriendGet
{
	CT_DWORD		dwUserID;							//自己ID
	CT_DWORD		dwFUserID;							//探访者 ID
	CT_CHAR			cUserName[NAME_SIZE];				//探访者 昵称
	CT_INT32		iOccupation;						//探访者 职业
	CT_INT32		iLevel;								//探访者 等级
};

//绑票战斗
struct MSC_KIDNAP_FIGHT
{
	CT_DWORD		dwUserID;							//发起用户ID
	CT_INT32		iSeed;								//随机种子
	CT_INT32        iWin;                               //发起方是否赢了
	CT_INT32		iType;								//绑架类型
	CT_INT32		iPetChange;							//幻化ID

	UserFightingStatus	LeftFightingData[TEAM_NUM];					//战斗数据
	UserFightingStatus	RightFightingData[TEAM_NUM];					//战斗数据
};

//营救战斗
struct MSC_RESCUE_FIGHT
{
	CT_DWORD		dwUserID;							//发起用户ID
	CT_INT32		iSeed;								//随机种子
	CT_INT32        iWin;                               //发起方是否赢了

	UserFightingStatus	LeftFightingData[TEAM_NUM];					//战斗数据
	UserFightingStatus	RightFightingData[TEAM_NUM];					//战斗数据
};

//掠夺战斗
struct MSC_PLUNDER_FIGHT
{
	CT_DWORD		dwUserID;							//发起用户ID
	CT_INT32		iSeed;								//随机种子
	CT_INT32        iWin;                               //发起方是否赢了
	CT_INT32		iBulidID;							//建筑ID
	CT_DWORD		dwDstUserID;						//目标ID

	UserFightingStatus	LeftFightingData[TEAM_NUM];					//战斗数据
	UserFightingStatus	RightFightingData[TEAM_NUM];					//战斗数据
};

//城墙加成
struct MSC_DB_WALL_ADDITION
{
	CT_DWORD	dwUserID;
	CT_INT32	iID;
	CT_INT32	iGrade;
};

//插入城墙信息
struct MSC_DB_InsertWallAddition
{
	CT_DWORD	dwUserID;
	CT_INT32	iID;
	CT_INT32	iGrade;
};

//插入城墙军队
struct MSC_DB_InsertWallArmy
{
	CT_DWORD	dwUserID;
	CT_INT32	iPos[POST_COUNT/2];
};

//城墙军队
struct MSC_DB_WALL_ARMY
{
	CT_DWORD	dwUserID;
	CT_INT32	iPos[POST_COUNT/2];
};

//进入他人城镇
struct MSC_DB_EnterOtherTown
{
	CT_DWORD dwUserID;
	CT_DWORD dwFUserID;									// 好友ID
	CT_CHAR cUserName[NAME_SIZE];						// 用户名
	CT_INT32 iOccupation;								// 职业
	CT_INT32 iLevel;									// 等级
	CT_INT32 iDiamond;									// 钻石
	CT_INT32 iMoney;									// 金币
	CT_INT32 iFood;										// 粮食
	CT_INT32 iWorker;									// 工人
	CT_INT32 iMaxWorker;								// 最大工人
	CT_INT32 iStamina;									// 体力
	CT_INT32 iVip;										// Vip
	CT_INT32 iFightingPower;							// 战斗力
	TeamSeek aTeam[SEEK_TEAM_FLAG_NUM];					// 战旗
	KidnapInfo aSaintessInfo[SEEKED_SPITE_TIMES];		// 圣女人质信息
	KidnapInfo aFarmerInfo[SEEKED_SPITE_TIMES];			// 农民人质信息
	KidnapInfo aPetInfo[SEEKED_SPITE_TIMES];				// 宠物肉票信息
	CT_INT32 iPetID[SEEK_KIDNAP_PET];					// 剩余的宠物ID
	BuildData			building[SEEK_BUILD_NUM];			// 基础建筑信息
	UpgradingBuildData	upgradeBuilding[SEEK_BUILD_NUM];	// 升级的建筑
	ProductingBuilding	productBuilding[SEEK_BUILD_NUM];	// 生产的建筑
	CT_INT32 iOpenFunction[MAX_FUCTION_NUM];			// 已开启的功能
	CT_INT32 iSaintess;									// 被绑架的圣女数
};

//获取被探访者信息
struct MSC_DB_SeekedGet
{
	CT_DWORD dwUserID;		// 自己ID
	CT_DWORD dwFUserID;		// 被探访者ID
	CT_INT32 iType;			// 探访类型
	CT_INT32 iSeekedWillTimes;							// 被善访的次数
	CT_INT32 iSeekedSpiteTimes;							// 被恶访的次数
};

//得到被探访者信息
struct MSC_DB_OtherSeekedInfo
{
	MSC_DB_SeekedGet	seekInfo;						// 探访信息
	MSC_DB_EnterOtherTown townInfo;						// 城镇信息
};

//插入已探访的玩家
struct MSC_DB_Had_SeekedPlayer
{
	CT_DWORD			dwUserID;
	CT_DWORD			dwFUserID;
	CT_INT32			iType;
};

//被访者的被访次数处理
struct MSC_DB_Seeked_TimesMinus 
{
	CT_DWORD			dwFUserID;
	CT_INT32			iType;
};

struct MSC_DB_Seeked_Info 
{
	CT_DWORD dwUserID;	// 好友ID
	CT_INT32 iDiamond;										// 钻石
	CT_INT32 iMoney;										// 金币
	CT_INT32 iFood;											// 粮食
	CT_INT32 iWorker;										// 工人
	CT_INT32 iMaxWorker;									// 最大工人
	CT_INT32 iKidnapWorkerNum;								// 被绑架的工人
	CT_INT32 iKidnapSaintess;								// 圣女是否被绑架
	CT_INT32 iPetKidnap[SEEK_KIDNAP_PET];					// 宠物绑架
	CT_INT32 iSeekedWillTimes;								// 被善访的次数
	CT_INT32 iSeekedSpiteTimes;								// 被恶访的次数
	TeamSeek teamFlag[SEEK_TEAM_FLAG_NUM];					// 战旗
	BuildData			buildingData[SEEK_BUILD_NUM];		// 建筑基本信息
	UpgradingBuildData	upgradeBuilding[SEEK_BUILD_NUM];	// 升级的建筑
	ProductingBuilding	productBuilding[SEEK_BUILD_NUM];	// 生产的建筑
	SeekPresent			present[SEEKED_PRESENT_NUM];		// 得到的礼品信息
	CT_INT32 iTime;											// 存入时间
};

// 战旗
struct MSC_DB_SeekTeamFlag
{
	CT_DWORD dwUserID;					
	TeamSeek teamFlag;						//旗帜信息
};

//人质
struct MSC_DB_SeekKidnap
{
	CT_DWORD dwUserID;
	CT_INT32 iType;							//人质类型
	KidnapInfo kidnap;						//人质信息
};

//探访列表（登录时获取）
struct MSC_DB_SeekListPlayer
{
	CT_DWORD dwUserID;
	UserInfo playerInfo;					//列表玩家信息
};

//探访基本数据
struct MSC_DB_SeekData
{
	CT_DWORD dwUserID;
	SeekData seekData;						//探访信息
};

//探访者礼物
struct MSC_DB_SeekPresent 
{
	CT_DWORD dwUserID;
	SeekPresent present;					//收到的礼物
};

//探访界面玩家列表
struct MSC_DB_SeekPlayerList
{
	CT_DWORD dwUserID;					
	UserInfo aWillPlayer[SEEK_PLAYER_NUM];	//善意列表的玩家ID
	UserInfo aSpitePlayer[SEEK_PLAYER_NUM];	//恶意列表的玩家ID
};

//来访纪录
struct MSC_DB_SeekVistorInfo 
{
	CT_DWORD	dwUserID;					//
	VistorInfo	vistorInfo;					//来访者的记录
};

typedef SeekFightingData MSC_DB_SeekFightRecord;

//掠夺资源
struct MSC_DB_PlunderRes
{
	CT_DWORD	dwUserID;
	CT_INT32	iPlunderScore;				//掠夺积分
};

// 排行榜奖励
struct MSC_DB_TopReward
{
	CT_DWORD	dwUserID;					
	CT_INT32	iType;						//排行榜类型
	CT_INT32	iStatus;					//奖励状态
};

#  pragma pack(pop)   /* 通讯协议结构体请定义在此行之前 */
#endif  // CTHREADMES_H_

