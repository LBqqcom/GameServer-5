/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		GlobeData.h
Version:		1.0
Description:    全局数据定义
Author:			pengwen
Create Date:	2012-8-16
History:             

******************************************************************/

#ifndef GLOBEDATA_H_
#define GLOBEDATA_H_
#include "CTData.h"

//城市线程索引（10个可用）
#define  GAME_THREAD_CITY            (SYS_THREAD_MIN + 10)
//战斗线程索引（10个可用）
#define  GAME_THREAD_FIGHT           (SYS_THREAD_MIN + 20)
//数据库线程索引(10个可用)
#define  GAME_THREAD_DATABASE        (SYS_THREAD_MIN + 80)
//日志数据库线程
#define  GAME_THREAD_LOGDATABASE	 (SYS_THREAD_MIN + 100)


//定时器
//http超时定时器
#define  IDI_HTTP_CHECK_TIMER                  99

//心跳定时器
#define IDI_HEART_BEAT_TIMER					100
//删除验证码时间
#define IDI_DELETE_VALIDATE_TIMER				101
//更新数据库时间
#define IDI_UPDATA_DATA_TIMER					102
//删除战斗数时间
#define IDI_DELETE_FIGHTING_DATA_TIMER			103
//数据库连接定时器
#define IDI_DB_CONNECT_TIMER                    200
//日志数据库连接定时器
//#define IDI_LOGDB_CONNECT_TIME					201
//建筑更新定时器
#define IDI_UPGRADE_BUILDING_TIMER              300
//游戏注册时间
#define  IDI_REGISTER_GAME						400		
//同步时间
#define  IDI_SYNCHRONOUS						401		
//建筑生产定时器
#define  IDI_BUILDING_PRODUCT_TIMER             500
//事件定时器
#define  IDI_EVENT_TIMER                        600
// 领主增益定时器 [10/10/2012 John]
#define IDI_MASTER_BUFF_TIMER                   (700)
// 领主能力冷却定时器 [10/30/2012 John]
#define IDI_MASTER_COOLDOWN_TIMER               (800)
// 领主体力恢复定时器
#define IDI_MASTER_STAMINA_RECOVER              (801)
// 领主道具同步定时器
#define IDI_MASTER_ITEM_SYNC_TIMER              (802)
//充值服务重连时重发所有订单
#define IDI_CHARGE_RECONNECT_TIMER				(803)
//GM消息定时器
#define IDI_GM_MESSAGE_TIMER					900
// 日常数据重置时钟 [11/14/2012 John]
#define IDI_DAILY_DATA_RESET_CLOCK              1000
// 发放俸禄时钟
#define IDI_RANK_SALARY_CLOCK                   1010
//宠物孵化定时器
#define IDI_PET_BORN_TIMER						1100
//宠物饱食度扣除定时器
#define IDI_PET_HUNGER_TIMER					1200
//商店，商城刷新闹钟
#define IDI_MARKET_REFRESH_CLOCK				1300	//1300-1309

//GM邮件定时器
#define IDI_GM_EMAIL_TIMER						1400
//竞技度
#define IDI_PVP_DEGREE							1500
//vip刷新时间
#define IDI_VIP_TIMER							1600
//vip礼包刷新
#define IDI_VIP_GIFT_CLOCK						1700
//在线人数统计
#define IDI_ONLINE_STATIC_TIMER					1800
//活动刷新时间
#define IDI_ACTIVE_EVENT_CLOCK					1900
#define IDI_ACTIVE_ONLINE_DUR					1901
//服务器启动获取活动信息
#define IDI_ACTIVE_EVENT_GET_TIMER				2000
//战队数据定时保存
#define IDI_FIGHTTEAM_SAVE_TIMER				2100
//黑店刷新闹钟（多个）
#define IDI_BLACK_MARKET_REFRESH_CLOCK			3000	//3000-3009

//建筑数据同步数据库定时器
#define IDI_BUILDING_SAVE_DATABASE_TIMER        4000
//生产数据同步到数据库定时器
#define IDI_PRODUCTING_SAVE_DATABASE_TIMER      4001


//服务器启动时候，读取资源点信息的定时器
#define IDI_RESOURCE_READ_SPOT_INFO             4100
//资源点生产定时器                              
#define IDI_RESOURCE_SPOT_PRODUCT               4101                 
//排名
#define IDI_TOP_INFO							4200
//每日0点更新
#define IDI_UPDATA_DATA							4300
//服务器启动时候，读取战队信息的定时器
#define IDI_FIGHTTEAM_READ_INFO					4400
//定时检查战队招募过期
#define IDI_FIGHTTEAM_RECRUIT					4401
//将资源数据发往数据库
#define IDI_RESOURCE_SAVE_DB                    4402
//服务心跳（防止内核断开）
#define IDI_SERVER_HERT							4500
//资源点活动开启(4600-4648)
#define IDI_RESACTIVE_OPEN						4600
//资源的活动持续时间
#define IDI_RESACTIVE_KEEP						4649
//资源的活动道具切换时间
#define IDI_RESACTIVE_CHANGE					4650
//元素之塔资源加载定时器 
#define IDI_TOWER_LOAD                          4670
//元素之塔资源保存定时器
#define IDI_TOWER_SAVE                          4671
//元素之塔扫描定时器
#define  IDI_TOWER_RES_PRODUCT_CHECK            4679
//个人限购刷新
#define IDI_MARKET_PERSONAL						4700
//个人限购刷新
#define IDI_MARKET_PERSONAL_UPDATA				4701
//公告更新定时器
#define IDI_ANNOUNCEMENT_UPDATA                 4702
//服务器启动时候，读取BOSS信息的定时器
#define IDI_BOSS_READ_INFO						4800
//BOSS活动开启(4801-4849)
#define IDI_BOSSACTIVE_OPEN						4801
//BOSS活动持续时间
#define IDI_BOSSACTIVE_KEEP						4850
//BOSS血量推送
#define IDI_BOSSACTIVE_BOSSINFO					4851
#define IDI_BOSSACTIVE_REFRESH					4852
//通用时间活动定时器(4900-4999)
#define IDI_TIMEACTIVE_OPEN						4900
//5000-5019
#define IDI_TIMEACTIVE_KEEP						5000
//跨服战数据同步
#define IDI_CS_FIGHITN_DATA						5100
//领主头衔定时器
#define IDI_MASTER_TITLE_TIMER					5200				
//龙岛每天更新定时器
#define IDI_ISLAND_UPDATE_TIME					5300
//服务器启动时读取头衔列表
#define IDI_TITLE_LIST_TIME						5400
//0点重新读取头衔列表
#define IDI_TITLE_LIST_UPDATE					5500
//探访礼物定时器
#define IDI_SEEK_PRESENT_TIME					5600

//将资源数据发往数据库的时间
#define TIME_RESOURCE_SAVE_DB               (1000*60*10) //10分钟
//游戏注册时间
#define  TIME_REGISTER_GAME					(1000*1)
//同步时间
#define  TIME_SYNCHRONOUS					(1000*5)
//更新数据库时间
#define	 TIME_UPDATA						(500)
//读取资源点信息的时间
#define  TIME_RESOURCE_READ_SPOT		    (20*1000)  //20秒后读取
//资源点生产时间间隔
#define  TIME_RESOURCE_PRODUCT              (3*1000)

#define  TIME_BOSS_READ_TOP				    (15*1000) //15秒后读取

//排名时间
#define  TIME_TOP_INFO						(1000*60)
//读取战队信息的时间
#define  TIME_FIGHTTEAM_READ_SPOT		    (10*1000) //10秒后读取
//检查战队招募过期时长
#define TIME_FIGHTTEAM_RECRUIT_SPOT			(600*1000)

//服务心跳（防止内核断开）
#define TIME_SERVER_HURT					(3*1000*60)			//服务心跳时间
      
//元素之塔资源加载时间间隔
#define TIME_TOWER_LOAD                      (30*1000)   //30秒            
//元素之塔保存时间间隔
#define TIME_TOWER_SAVE                      (10*60*1000) //10分钟
//元素之塔扫描间隔
#define TIME_TOWER_RES_PRODUCT_CHECK         (3*1000)
//个人限购间隔
#define TIME_MARKET_PERSONAL_UPDATA			 (5*60*1000)
//个人限购间隔
#define TIME_MARKET_PERSONAL			 (5*60*1000)

//公告更新时间								
#define TIME_ANNOUNCEMENT_UPDATA			(10*60*1000) //10分钟

enum ENUM_BaseDataSaveTag
{
//savetag
  SAVETAG_CIVIL_UPGRADE_FINISH   = (2),
  SAVETAG_BUILD_UPGRADE_FINISH   = (3),
  SAVETAG_MASTER_UPGRADE_FINISH  = (4),
// Begin [9/20/2012 John]
  SAVETAG_RANK_UPGRADE_FINISH    = (5),        // 军衔提升完成
  SAVETAG_LEARN_TALENT_FINISH    = (6),        // 学习潜能完成
  SAVETAG_LEARN_SKILL_FINISH     = (7),        // 强化技能完成
  SAVETAG_USE_ITEM_FINISH        = (8),        // 使用道具完成
  SAVETAG_STRENGTHEN_EQUIP_FINISH= (9),        // 强化装备完成
  SAVETAG_WORLD_SCENE_REWARD	 = (10),	   // 世界场景奖励
  SAVETAG_DUPLICATE_REWARD		 = (11),	   // 副本奖励
  SAVETAG_ARMY_CONSUME			 = (12),	   // 军队消耗
  SAVETAG_PRODUCT_START          = (13),       // 开始生产
  SAVETAG_PRODUCT_FINISH         = (14),       // 生产完成
  SAVETAG_ACTIVE_UPGRADE_FINISH  = (15),       // 活跃度升级
  SAVETAG_RECV_SIGN_REWARD       = (16),       // 领取签到奖励
  SAVETAG_RECV_ACTIVE_REWARD     = (17),       // 领取活跃度奖励
  SAVETAG_SELL_ITEM              = (18),       // 出售道具 
  SAVETAG_PVP_COST				 = (19),       // PVP消耗
  SAVETAG_PVP_REWARD			= (20),       // PVP奖励
  SAVETAG_PVP_VICTORYTIMES		 = (21),		//PVP胜率

// End [9/20/2012 John]

  SAVETAG_CIVIL_UPGRADE_SPEE_UP_FINISH     =  (22), //升级加速
  SAVETAG_BUILDING_PRODUCT_SPEE_UP_FINISH  =  (23), //生产加速
  SAVETAG_REGION_UNLOCK_FINISH			   =  (24), //区域解锁完成更新领主资源
  SAVETAG_EVENT_REWARD                     =  (30), //事件奖励
  SAVETAG_EVENT_PVP						   =  (31), //竞技场

  SAVETAG_TASK_REWARD						=	(40), //任务奖励
  SAVETAG_TASK_FAST_COMPLETE				=	(41), //任务快速完成消耗钻石
  SAVETAG_MARKET_NORMAL						=	(42), //普通商店购买
  SAVETAG_MARKET_MALL						=   (43),  //商城购买
  SAVETAG_MARKET_BLACK						=   (44),  //黑店购买
  SAVETAG_MARKET_BLACK_REFRESH				=	(45),  //黑店刷新
  SAVETAG_PET_UP_GRADE						=	(46),	//宠物升级
  SAVETAG_PET_SKILL							=	(47),   //宠物技能

  SAVETAG_EVENT_AWARD						=    (51),  //事件奖励
  SAVETAG_TURN_AWARD						=    (52),  //转盘奖励
  SAVETAG_CDKEY_AWARD						=    (53),  //cdkey奖励
  SAVETAG_PRESENT_AWARD						=    (54),  //奖励发放

  SAVETAG_PVP_CHEST							=	(60),  //打开宝箱

  SAVETAG_DUPLICATE_STAMINA					=	(61), //副本体力消耗

  SAVETAG_STAMINA_COST						=	(62), //购买体力消耗

  SAVETAG_VIP_RENEWAL						=	(63), //vip开通

  SAVETAG_WORSHIP_REWARD					=	(64), //祭祀奖励
  SAVETAG_INC_BAGSLOT                       =   (65), // 扩展背包
  SAVETAG_USE_ITEM_CHIP                     =   (66), // 碎片合成消耗
  SAVETAG_USE_ITEM_DESIGN                   =   (67), // 设计图合成消耗

  SAVETAG_PVP_PVPREFRESH					=	(68),//PVP强制刷新	

  SAVETAG_FIGHTTEAM_CREATE					=	(69), //创建战队
  SAVETAG_FIGHTTEAM_CONTRIBUTE				=	(70), //战队成员捐献
  SAVETAG_FIGHTTEAM_RECRUIT					=	(71), //战队发布招募
  SAVETAG_FIGHTTEAM_RESREWARD				=	(72), //战队资源奖励
  SAVETAG_FIGHTTEAM_SUMMONBOSS				=	(73), //战队boss
  SAVETAG_FIGHTTEAM_TIMEBOSS				=	(74), //战队时限boss

  SAVETAG_RESET_SKILL            = (101),           // 重置技能成功
  SAVETAG_RANK_SALARY            = (102),           // 军衔俸禄
  SAVETAG_STAMINA_RECOVER        = (103),           // 体力恢复
  SAVETAG_SKILLSLOT_INC          = (104),           // 技能栏增加  

  SAVETAG_TIMING_UPDATA			 = (105),			//定时更新
  SAVETAG_CLEAR_CD               = (106),           // 清除冷却时间
  SAVETAG_CHARGE_SUC             = (107),           // 充值成功
  SAVETAG_ENCHANT_SUC            = (108),           // 激活/附魔成功

  SAVETAG_ACTIVE_EVENT_REWARD	=	(109),			//活动奖励
  SAVETAG_RAIDS_SPEED			=	(110),			//扫荡加速
  SAVETAG_RAIDS_REWARD			=	(111),			//扫荡奖励

  SAVETAG_TEAMDUOLICATE_COST	=	(112),			//组队副本消耗
  SAVETAG_TEAMDUOLICATE_REWARD	=	(113),			//组队副本奖励
	
  SAVETAG_TOWER_OCCUPY_COST     =   (114),         //元素之塔占领消耗
  SAVETAG_TOWRE_HELP_REWARD     =   (115),         //元素之塔奖励
  SAVETAG_ELMPOWER_EAT          =   (116),          //元素吞噬的消耗
  SAVETAG_TOWRE_PASS_REWARD		=	(117),			//元素塔通关奖励
  SAVETAG_TOWRE_SLOT_REWARD		=	(118),			//元素塔占领生产奖励
  SAVETAG_TOWRE_USE_TREASURE	=	(119),			//使用元素塔宝箱消耗
  SAVETAG_TOWRE_TREASURE_REWARD	=	(120),			//使用元素塔宝箱奖励
  SAVETAG_RESOURCE_PRODUCT      =   (121),          //资源生产
  SAVETAG_RESOURCE_OCCUPYAPPLY	=	(122),			//资源点强攻
  SAVATAG_ENCHANT_REWARD		=	(123),			//附魔奖励
  SAVATAG_BOSS_BATTLE			=	(124),			//世界BOSS强攻
  SAVATAG_BOSS_ENCOURAGE		=	(125),			//世界BOSS激励
  SAVATAG_BOSS_REWARD			=	(126),			//世界BOSS奖励
  SAVATAG_DRAGONBALL_COST		=	(127),			//龙珠消耗
  SAVATAG_DRAGONBALL_REWARD		=	(128),			//龙珠奖励

  SAVATAG_CS_SUPPORT			=	(129),			//跨服战请求
  SAVATAG_CS_INSPORE			=	(130),			//跨服战威能
  SAVATAG_CS_REWARD				=	(131),			//跨服战奖励领取

  SAVETAG_TASK_COST				=	(132),			//任务消耗
  SAVETAG_FLAG_UPGRADE			=	(133),			//战旗升级
  SAVETAG_EQUIP_WASH			=	(134),			//装备洗练
  SAVETAG_EQUIP_EMBED			=	(135),			//装备镶嵌
  SAVETAG_WORSHIP_COST			=	(136),			//祭祀消耗

  SAVETAG_ACTIVE_TITLE			=	(137),			//激活头衔
  SAVETAG_ISLAND_COST			=	(138),			//龙岛立即完成消耗
  SAVATAG_ISLAND_REWARD			=	(139),			//龙岛奖励获得

  SAVATAG_CHANGE_MASTERNAME		=	(140),			//修改领主名称
  SAVATAG_CHANGE_FTNAME			=	(141),			//修改战队名称
  
  SAVETAG_MONTH_CARD			=	(142),			//月卡
  SAVETAG_SEEK_WALLADDITION		=	(143),			//城墙加成
  SAVATAG_SEEK_RANSOM			=	(144),			//赎金
  SAVATAG_SEEK_PLUNDER			=	(145),			//掠夺
  SAVATAG_SEEK_FALGS			=	(146),			//拔旗帜奖励
  SAVATAG_SEND_PRESENT			=	(147),			//送礼消耗
  SAVATAG_GET_PRESENT			=	(148),			//领取礼物
  SAVATAG_KIDNAP				=	(149),			//绑票
  SAVATAG_SEEK_UPDATE			=	(150),			//刷新列表消耗
  SAVATAG_SEEK_BLACK			=	(151),			//探访黑店
  SAVATAG_ITEM_BOSS				=	(152),			//boss道具
  SAVATAG_ITEM_FIGHTADD			=	(153),			//战斗属性加成

  SAVATAG_TOP_REWARD			=	(154),			//排行榜奖励
  SAVATAG_PLUNDER_REWARD		=	(155),			//掠夺请求奖励

  SAVETAG_EVENT_EXIT            =  (256), //退出程序， 彭文
};

enum ENUM_BaseDataSaveTagEx
{
	SAVETAGEX_TOWER_HELP		= (3),
	SAVETAGEX_CHARGEDIAMOD		= (4),
	SAVETAGEX_COSTDIAMOD		= (5),
	SAVETAGEX_TOWER_TIME		= (6),
	SAVETAGEEX_WORSHIP_TIME0	= (7),	//祭祀5-11
	SAVETAGEEX_WORSHIP_TIME1	= (8),	//祭祀5-11
	SAVETAGEEX_WORSHIP_TIME2	= (9),	//祭祀5-11
	SAVETAGEEX_WORSHIP_TIME3	= (10),	//祭祀5-11
	SAVETAGEEX_WORSHIP_TIME4	= (11),	//祭祀5-11
	SAVETAGEEX_WORSHIP_TIME5	= (12),	//祭祀5-11
	SAVETAGEEX_OPEN_CHEST		= (13), //打开魔盒
	SAVETAGEEX_FINISH_CAVE		= (14), //洞穴
	SAVETAGEEX_PVP_CHEST		= (15), //PVP宝箱
	SAVETAGEEX_FT_CHEST			= (16), //召唤宝箱
	SAVETAGEEX_TURNTABLE0		= (17),	//转盘
	SAVETAGEEX_TURNTABLE1		= (18),	//转盘
	SAVETAGEEX_TURNTABLE2		= (19),	//转盘
};


//定时器结束

//战斗系统
///////////////////////////////////////////////////////////////////////
//#define Game_Player		6				    //游戏人数
//#define Status			6				    //用户状态
#define RoundMax		30						//最大回合
#define MosterCount		3						//怪物数目
//#define PostCount		10						//位置数目
#define SKILLAFFECT		4						//技能效果

//攻击效果
#define Positive_Effect	1						//正面效果
#define Negative_Effect	2						//负面效果

//副本错误信息
enum ENUM_DUPLICATE
{
	 ERROR_FOOD			=1,						//食物不足
	 ERROR_PPHYSICAL	=2,						//体力不足
	 ERROR_GOODS		=3,						//道具不足
	 ERROR_CONDITION	=4,						//开启条件不足
	 ERROR_PAYDIAMOD	=5,						//支付钻石
	 ERROR_NO_NUM		=6,						//次数不够
	 ERROR_DATA			=7,						//数据错误
	 ERROR_WORLD_GRADE		=8,						//等级不足
	 ERROR_SCENE_GRADE		=9,						//等级不足
	 ERROR_DIAMOD		=10,						//钻石
	 ERROR_TASK			=11,					//任务
	 ERROR_DIFFICULTY	=12,					//难度
	 ERROR_MAX_NUM		=13,					//最大次数
	 ERROR_PASS_SCENE	=14,					//上个场景未通过
};

//扫荡错误信息
enum ENUM_RAIDS
{
	RAIDS_ERROR			= 1,
	RAIDS_TIME			= 2,
	RAIDS_DATA			= 3,
};

//奖励表示
enum ENUM_DUPLICATE_REWARD
{
	 REWARD_CLOSE		=0,					//奖励未开启
	 GET_REWARD			=1,					//可领取奖励
	 GET_REWARD_FINISH	=2,					//已领取奖励
};

//冷却类型
enum ENUM_COOLTYPE
{
	SMITHINGEQUIP_COOL	=1,					//强化装备冷却
	BLACKREFRESH_COOL	=2,					//黑市冷却
	MARKETREFRESH_COOL	=3,					//商城冷却
	FTNUMBERLEAVE_COOL	=4,					//离队冷却
	TOPCHALLENGE_COOL	=5,					//排行榜挑战冷却
	SEEKLISTUPDATE		=6,					//探访列表刷新
	FTTIMEBOSS_SUMMON	=7,					//时限boss召唤
	FTTIMEBOSS_FIGHT	=8,					//时限boss战斗
};

//为概率
enum ENUM_FAKEPRO_TYPE
{
	FAKEPRO_TYPE_WORSHIP	=1,				//祭祀类型
	FAKEPRO_TYPE_TOWER		=2,				//元素塔类型
	FAKEPRO_TYPE_CHEST		=3,				//魔盒类型
	FAKEPRO_TYPE_CAVE		=4,				//洞穴类型
	FAKEPRO_TYPE_PVP_CHEST	=5,				//竞技场宝箱
	FAKEPRO_TYPE_FT_CHEST	=6,				//战队召唤宝箱
	FAKEPRO_TYPE_TURNTABLE	=7,				//转盘类型
};

//祭祀类型
enum ENUM_WORSHIP_TYPE
{
	WORSHIP_TYPE_FOOD		=0,				//丰收女神
	WORSHIP_TYPE_TALENT		=1,				//战争女神
	WORSHIP_TYPE_LUCK		=2,				//幸运女神
	WORSHIP_TYPE_FOOD_D		=3,				//丰收女神钻石
	WORSHIP_TYPE_TALENT_D	=4,				//战争女神钻石
	WORSHIP_TYPE_LUCK_D		=5,				//幸运女神钻石
};

//个人限购
enum ENUM_MARKET_PERSONAL
{
	GOODS_STATUS_NORMAL		=0,				//正常状态
	GOODS_STATUS_UPDATA		=1,				//更新状态,
	GOODS_STATUS_DELETE		=2,				//删除状态
};

//跨服战支持错误
enum ENUM_CS_SUPPORT_ERROR
{
	SUPPORT_ERROR_RES		=1,				//资源不足
	SUPPORT_ERROR_DATA		=2,				//数据错误
	SUPPORT_ERROR_NO_NUM	=3,				//次数不足
	SUPPORT_ERROR_REWARD	=4,				//奖励未领取
	SUPPORT_ERROR_TEAM		=5,				//战队不能支持
};

//跨服战资格战错误
enum ENUM_CS_SEAT_FIGHT_ERROR
{
	SEAT_FIGHT_POS			=1,				//已有位置
	SEAT_FIGHT_SEAT			=2,				//没有资格
	SEAT_FIGHT_CAPTAIN		=3,				//不是队长
	SEAT_FIGHT_DATA			=4,				//数据错误
	SEAT_FIGHT_COOL_TIME	=5,				//冷却时间
};

//威能
enum ENUM_CS_INSPORE_ERROR
{
	INSPORE_DATA			=1,				//数据错误
	INSPORE_RES				=2,				//资源不足
	INSPORE_ID				=3,				//没此威能
};

//跨服领奖
enum ENUM_CS_REWARD
{
	REWARD_BEG				=1,				//背包位置
	REWARD_DATA				=2,				//数据错误
};

//宠物升级
enum ENUM_PET_UPGRADE
{
	UPGRADE_ITEM_NUM		=1,				//道具不足
	UPGRADE_ITEM_ERR		=2,				//道具不能吞噬
	UPGRADE_ITEM_DATA		=3,				//数据错误
};

//宠物技能
enum ENUM_PET_SKILL
{
	SKILL_ITEM				=1,				//道具错误
	SKILL_DATA				=2,				//数据错误
	SKILL_GRADE				=3,				//技能等级
	SKILL_SAME				= 4,             //其他槽位已装备同类型技能
};

//宠物幻化
enum ENUM_PET_CHANGE
{
	CHANGE_ID				=1,				//幻化ID
};

//宠物主动技能升级
enum ENUM_PET_SKILL_UPGARDE
{
	PET_SKILL_DATA			=1,				//数据错误
	PET_SKILL_MAX			=2,				//最高级
	PET_SKILL_COST			=3,				//消耗不足
	PET_SKILL_GRADE			=4,				//等级不足
};

//月卡
enum ENUM_MONTH_CARD
{
	MONTH_CARD_DATA			=1,				//数据错误
	MONTH_CARD_COST			=2,				//消耗不足
	MONTH_CARD_ACTIVE		=3,				//活动未开启
	MONTH_CARD_TIME			=4,				//过期
	MONTH_CARD_GET			=5,				//已领取
};

//城墙加成
enum ENUM_WALLADDITION
{
	WALLADDITION_DATA			=1,				//数据错误
	WALLADDITION_COST			=2,				//消耗不足
	WALLADDITION_GRADE			=3,				//等级不足
};

//城墙军队
enum ENUM_WALLARMY
{
	WALLARMY_ID					=1,				//军队ID
	WALLARMY_SAME				=2,				//重复
	WALLARMY_COUNT				=3,				//数量
	WALLARMY_MASTER				=4,				//领主
};

//绑票类型
enum ENUM_KIDNAP_TYPE
{
	KIDNAP_WOMAN				=1,				//圣女
	KIDNAP_FARMER				=2,				//农民
	KIDNAP_PET					=3,				//宠物
};

//绑票
enum ENUM_KIDNAP
{
	KIDNAP_TYPE					=1,				//类型错误
	KIDNAP_DATA					=2,				//数据错误
	KIDNAP_USER					=3,				//无此用户
	KIDNAP_STATUS				=4,				//状态
	KIDNAP_NUM					=5,				//人数不足
	KIDNAP_ENOUGH				=6,				//已经抓了农民、圣女
	KIDNAP_NO_PET				=7,				//宠物已被抓走
};

//探访状态
enum ENUM_KIDNAP_STATUS
{
	KIDNAP_STATUS_NO			=0,				//没探访过
	KIDNAP_STATUS_YES			=1,				//探访过
};

//营救类型
enum ENUM_RESCUE_TYPE
{
	RESCUE_FIGHT				=1,				//战斗
	RESCUE_MONEY				=2,				//赎金
};

//营救
enum ENUM_RESCUE
{
	RESCUE_DATA					=1,				//数据错误
	RESCUE_RES					=2,				//资源不足
	RESCUE_USER					=3,				//无次用户
	RESCUE_HOSTAGE				=4,				//没有人质
	RESCUE_NUM					=5,				//次数不足
};

//掠夺
enum ENUM_PLUNDER
{
	PLUNDER_BUILD_ID			=1,				//ID错误
	PLUNDER_BUILD_TYPE			=2,				//类型
	PLUNDER_NUM					=3,				//次数
	PLUNDER_DATA				=4,				//数据错误
};

//探访查看战报
enum ENUM_SEEK_FIGHTING
{
	SEEK_FIGHTING_DATA			=1,				//数据错误
	SEEK_FIGHTING_USER			=2,				//无此用户
};

//伪随机数
#define RAND_MAX_EX 888888	

//更新数据库阀值
#define UPDATA_DATA_VALUE	20					//	

////////////////////////////////////////////////////////////////////////
//编译选项


//#define  _TEST_CLOSE_NET					  //测试环境不主动关闭连接
//#define  _SHOW_FIGHT_INFO                   //战斗信息打印
//#define  _SHOW_MESSAGE_INFO                 //消息统计打印
//#define  _SHOW_MP_INFO                      //消耗魔法信息

#ifndef _WIN32
//#define _DEBUG                                //启用linux下的debug
#endif

#ifdef _DEBUG
#define CT_ASSERT(exp) assert(exp)
#else
#define CT_ASSERT(exp)
#endif

/////////////////////////////////////////////////////////////////////////
#endif