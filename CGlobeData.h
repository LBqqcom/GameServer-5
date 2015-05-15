/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		GlobeData.h
Version:		1.0
Description:    ȫ�����ݶ���
Author:			pengwen
Create Date:	2012-8-16
History:             

******************************************************************/

#ifndef GLOBEDATA_H_
#define GLOBEDATA_H_
#include "CTData.h"

//�����߳�������10�����ã�
#define  GAME_THREAD_CITY            (SYS_THREAD_MIN + 10)
//ս���߳�������10�����ã�
#define  GAME_THREAD_FIGHT           (SYS_THREAD_MIN + 20)
//���ݿ��߳�����(10������)
#define  GAME_THREAD_DATABASE        (SYS_THREAD_MIN + 80)
//��־���ݿ��߳�
#define  GAME_THREAD_LOGDATABASE	 (SYS_THREAD_MIN + 100)


//��ʱ��
//http��ʱ��ʱ��
#define  IDI_HTTP_CHECK_TIMER                  99

//������ʱ��
#define IDI_HEART_BEAT_TIMER					100
//ɾ����֤��ʱ��
#define IDI_DELETE_VALIDATE_TIMER				101
//�������ݿ�ʱ��
#define IDI_UPDATA_DATA_TIMER					102
//ɾ��ս����ʱ��
#define IDI_DELETE_FIGHTING_DATA_TIMER			103
//���ݿ����Ӷ�ʱ��
#define IDI_DB_CONNECT_TIMER                    200
//��־���ݿ����Ӷ�ʱ��
//#define IDI_LOGDB_CONNECT_TIME					201
//�������¶�ʱ��
#define IDI_UPGRADE_BUILDING_TIMER              300
//��Ϸע��ʱ��
#define  IDI_REGISTER_GAME						400		
//ͬ��ʱ��
#define  IDI_SYNCHRONOUS						401		
//����������ʱ��
#define  IDI_BUILDING_PRODUCT_TIMER             500
//�¼���ʱ��
#define  IDI_EVENT_TIMER                        600
// �������涨ʱ�� [10/10/2012 John]
#define IDI_MASTER_BUFF_TIMER                   (700)
// ����������ȴ��ʱ�� [10/30/2012 John]
#define IDI_MASTER_COOLDOWN_TIMER               (800)
// ���������ָ���ʱ��
#define IDI_MASTER_STAMINA_RECOVER              (801)
// ��������ͬ����ʱ��
#define IDI_MASTER_ITEM_SYNC_TIMER              (802)
//��ֵ��������ʱ�ط����ж���
#define IDI_CHARGE_RECONNECT_TIMER				(803)
//GM��Ϣ��ʱ��
#define IDI_GM_MESSAGE_TIMER					900
// �ճ���������ʱ�� [11/14/2012 John]
#define IDI_DAILY_DATA_RESET_CLOCK              1000
// ����ٺ»ʱ��
#define IDI_RANK_SALARY_CLOCK                   1010
//���������ʱ��
#define IDI_PET_BORN_TIMER						1100
//���ﱥʳ�ȿ۳���ʱ��
#define IDI_PET_HUNGER_TIMER					1200
//�̵꣬�̳�ˢ������
#define IDI_MARKET_REFRESH_CLOCK				1300	//1300-1309

//GM�ʼ���ʱ��
#define IDI_GM_EMAIL_TIMER						1400
//������
#define IDI_PVP_DEGREE							1500
//vipˢ��ʱ��
#define IDI_VIP_TIMER							1600
//vip���ˢ��
#define IDI_VIP_GIFT_CLOCK						1700
//��������ͳ��
#define IDI_ONLINE_STATIC_TIMER					1800
//�ˢ��ʱ��
#define IDI_ACTIVE_EVENT_CLOCK					1900
#define IDI_ACTIVE_ONLINE_DUR					1901
//������������ȡ���Ϣ
#define IDI_ACTIVE_EVENT_GET_TIMER				2000
//ս�����ݶ�ʱ����
#define IDI_FIGHTTEAM_SAVE_TIMER				2100
//�ڵ�ˢ�����ӣ������
#define IDI_BLACK_MARKET_REFRESH_CLOCK			3000	//3000-3009

//��������ͬ�����ݿⶨʱ��
#define IDI_BUILDING_SAVE_DATABASE_TIMER        4000
//��������ͬ�������ݿⶨʱ��
#define IDI_PRODUCTING_SAVE_DATABASE_TIMER      4001


//����������ʱ�򣬶�ȡ��Դ����Ϣ�Ķ�ʱ��
#define IDI_RESOURCE_READ_SPOT_INFO             4100
//��Դ��������ʱ��                              
#define IDI_RESOURCE_SPOT_PRODUCT               4101                 
//����
#define IDI_TOP_INFO							4200
//ÿ��0�����
#define IDI_UPDATA_DATA							4300
//����������ʱ�򣬶�ȡս����Ϣ�Ķ�ʱ��
#define IDI_FIGHTTEAM_READ_INFO					4400
//��ʱ���ս����ļ����
#define IDI_FIGHTTEAM_RECRUIT					4401
//����Դ���ݷ������ݿ�
#define IDI_RESOURCE_SAVE_DB                    4402
//������������ֹ�ں˶Ͽ���
#define IDI_SERVER_HERT							4500
//��Դ������(4600-4648)
#define IDI_RESACTIVE_OPEN						4600
//��Դ�Ļ����ʱ��
#define IDI_RESACTIVE_KEEP						4649
//��Դ�Ļ�����л�ʱ��
#define IDI_RESACTIVE_CHANGE					4650
//Ԫ��֮����Դ���ض�ʱ�� 
#define IDI_TOWER_LOAD                          4670
//Ԫ��֮����Դ���涨ʱ��
#define IDI_TOWER_SAVE                          4671
//Ԫ��֮��ɨ�趨ʱ��
#define  IDI_TOWER_RES_PRODUCT_CHECK            4679
//�����޹�ˢ��
#define IDI_MARKET_PERSONAL						4700
//�����޹�ˢ��
#define IDI_MARKET_PERSONAL_UPDATA				4701
//������¶�ʱ��
#define IDI_ANNOUNCEMENT_UPDATA                 4702
//����������ʱ�򣬶�ȡBOSS��Ϣ�Ķ�ʱ��
#define IDI_BOSS_READ_INFO						4800
//BOSS�����(4801-4849)
#define IDI_BOSSACTIVE_OPEN						4801
//BOSS�����ʱ��
#define IDI_BOSSACTIVE_KEEP						4850
//BOSSѪ������
#define IDI_BOSSACTIVE_BOSSINFO					4851
#define IDI_BOSSACTIVE_REFRESH					4852
//ͨ��ʱ����ʱ��(4900-4999)
#define IDI_TIMEACTIVE_OPEN						4900
//5000-5019
#define IDI_TIMEACTIVE_KEEP						5000
//���ս����ͬ��
#define IDI_CS_FIGHITN_DATA						5100
//����ͷ�ζ�ʱ��
#define IDI_MASTER_TITLE_TIMER					5200				
//����ÿ����¶�ʱ��
#define IDI_ISLAND_UPDATE_TIME					5300
//����������ʱ��ȡͷ���б�
#define IDI_TITLE_LIST_TIME						5400
//0�����¶�ȡͷ���б�
#define IDI_TITLE_LIST_UPDATE					5500
//̽�����ﶨʱ��
#define IDI_SEEK_PRESENT_TIME					5600

//����Դ���ݷ������ݿ��ʱ��
#define TIME_RESOURCE_SAVE_DB               (1000*60*10) //10����
//��Ϸע��ʱ��
#define  TIME_REGISTER_GAME					(1000*1)
//ͬ��ʱ��
#define  TIME_SYNCHRONOUS					(1000*5)
//�������ݿ�ʱ��
#define	 TIME_UPDATA						(500)
//��ȡ��Դ����Ϣ��ʱ��
#define  TIME_RESOURCE_READ_SPOT		    (20*1000)  //20����ȡ
//��Դ������ʱ����
#define  TIME_RESOURCE_PRODUCT              (3*1000)

#define  TIME_BOSS_READ_TOP				    (15*1000) //15����ȡ

//����ʱ��
#define  TIME_TOP_INFO						(1000*60)
//��ȡս����Ϣ��ʱ��
#define  TIME_FIGHTTEAM_READ_SPOT		    (10*1000) //10����ȡ
//���ս����ļ����ʱ��
#define TIME_FIGHTTEAM_RECRUIT_SPOT			(600*1000)

//������������ֹ�ں˶Ͽ���
#define TIME_SERVER_HURT					(3*1000*60)			//��������ʱ��
      
//Ԫ��֮����Դ����ʱ����
#define TIME_TOWER_LOAD                      (30*1000)   //30��            
//Ԫ��֮������ʱ����
#define TIME_TOWER_SAVE                      (10*60*1000) //10����
//Ԫ��֮��ɨ����
#define TIME_TOWER_RES_PRODUCT_CHECK         (3*1000)
//�����޹����
#define TIME_MARKET_PERSONAL_UPDATA			 (5*60*1000)
//�����޹����
#define TIME_MARKET_PERSONAL			 (5*60*1000)

//�������ʱ��								
#define TIME_ANNOUNCEMENT_UPDATA			(10*60*1000) //10����

enum ENUM_BaseDataSaveTag
{
//savetag
  SAVETAG_CIVIL_UPGRADE_FINISH   = (2),
  SAVETAG_BUILD_UPGRADE_FINISH   = (3),
  SAVETAG_MASTER_UPGRADE_FINISH  = (4),
// Begin [9/20/2012 John]
  SAVETAG_RANK_UPGRADE_FINISH    = (5),        // �����������
  SAVETAG_LEARN_TALENT_FINISH    = (6),        // ѧϰǱ�����
  SAVETAG_LEARN_SKILL_FINISH     = (7),        // ǿ���������
  SAVETAG_USE_ITEM_FINISH        = (8),        // ʹ�õ������
  SAVETAG_STRENGTHEN_EQUIP_FINISH= (9),        // ǿ��װ�����
  SAVETAG_WORLD_SCENE_REWARD	 = (10),	   // ���糡������
  SAVETAG_DUPLICATE_REWARD		 = (11),	   // ��������
  SAVETAG_ARMY_CONSUME			 = (12),	   // ��������
  SAVETAG_PRODUCT_START          = (13),       // ��ʼ����
  SAVETAG_PRODUCT_FINISH         = (14),       // �������
  SAVETAG_ACTIVE_UPGRADE_FINISH  = (15),       // ��Ծ������
  SAVETAG_RECV_SIGN_REWARD       = (16),       // ��ȡǩ������
  SAVETAG_RECV_ACTIVE_REWARD     = (17),       // ��ȡ��Ծ�Ƚ���
  SAVETAG_SELL_ITEM              = (18),       // ���۵��� 
  SAVETAG_PVP_COST				 = (19),       // PVP����
  SAVETAG_PVP_REWARD			= (20),       // PVP����
  SAVETAG_PVP_VICTORYTIMES		 = (21),		//PVPʤ��

// End [9/20/2012 John]

  SAVETAG_CIVIL_UPGRADE_SPEE_UP_FINISH     =  (22), //��������
  SAVETAG_BUILDING_PRODUCT_SPEE_UP_FINISH  =  (23), //��������
  SAVETAG_REGION_UNLOCK_FINISH			   =  (24), //���������ɸ���������Դ
  SAVETAG_EVENT_REWARD                     =  (30), //�¼�����
  SAVETAG_EVENT_PVP						   =  (31), //������

  SAVETAG_TASK_REWARD						=	(40), //������
  SAVETAG_TASK_FAST_COMPLETE				=	(41), //����������������ʯ
  SAVETAG_MARKET_NORMAL						=	(42), //��ͨ�̵깺��
  SAVETAG_MARKET_MALL						=   (43),  //�̳ǹ���
  SAVETAG_MARKET_BLACK						=   (44),  //�ڵ깺��
  SAVETAG_MARKET_BLACK_REFRESH				=	(45),  //�ڵ�ˢ��
  SAVETAG_PET_UP_GRADE						=	(46),	//��������
  SAVETAG_PET_SKILL							=	(47),   //���＼��

  SAVETAG_EVENT_AWARD						=    (51),  //�¼�����
  SAVETAG_TURN_AWARD						=    (52),  //ת�̽���
  SAVETAG_CDKEY_AWARD						=    (53),  //cdkey����
  SAVETAG_PRESENT_AWARD						=    (54),  //��������

  SAVETAG_PVP_CHEST							=	(60),  //�򿪱���

  SAVETAG_DUPLICATE_STAMINA					=	(61), //������������

  SAVETAG_STAMINA_COST						=	(62), //������������

  SAVETAG_VIP_RENEWAL						=	(63), //vip��ͨ

  SAVETAG_WORSHIP_REWARD					=	(64), //���뽱��
  SAVETAG_INC_BAGSLOT                       =   (65), // ��չ����
  SAVETAG_USE_ITEM_CHIP                     =   (66), // ��Ƭ�ϳ�����
  SAVETAG_USE_ITEM_DESIGN                   =   (67), // ���ͼ�ϳ�����

  SAVETAG_PVP_PVPREFRESH					=	(68),//PVPǿ��ˢ��	

  SAVETAG_FIGHTTEAM_CREATE					=	(69), //����ս��
  SAVETAG_FIGHTTEAM_CONTRIBUTE				=	(70), //ս�ӳ�Ա����
  SAVETAG_FIGHTTEAM_RECRUIT					=	(71), //ս�ӷ�����ļ
  SAVETAG_FIGHTTEAM_RESREWARD				=	(72), //ս����Դ����
  SAVETAG_FIGHTTEAM_SUMMONBOSS				=	(73), //ս��boss
  SAVETAG_FIGHTTEAM_TIMEBOSS				=	(74), //ս��ʱ��boss

  SAVETAG_RESET_SKILL            = (101),           // ���ü��ܳɹ�
  SAVETAG_RANK_SALARY            = (102),           // ����ٺ»
  SAVETAG_STAMINA_RECOVER        = (103),           // �����ָ�
  SAVETAG_SKILLSLOT_INC          = (104),           // ����������  

  SAVETAG_TIMING_UPDATA			 = (105),			//��ʱ����
  SAVETAG_CLEAR_CD               = (106),           // �����ȴʱ��
  SAVETAG_CHARGE_SUC             = (107),           // ��ֵ�ɹ�
  SAVETAG_ENCHANT_SUC            = (108),           // ����/��ħ�ɹ�

  SAVETAG_ACTIVE_EVENT_REWARD	=	(109),			//�����
  SAVETAG_RAIDS_SPEED			=	(110),			//ɨ������
  SAVETAG_RAIDS_REWARD			=	(111),			//ɨ������

  SAVETAG_TEAMDUOLICATE_COST	=	(112),			//��Ӹ�������
  SAVETAG_TEAMDUOLICATE_REWARD	=	(113),			//��Ӹ�������
	
  SAVETAG_TOWER_OCCUPY_COST     =   (114),         //Ԫ��֮��ռ������
  SAVETAG_TOWRE_HELP_REWARD     =   (115),         //Ԫ��֮������
  SAVETAG_ELMPOWER_EAT          =   (116),          //Ԫ�����ɵ�����
  SAVETAG_TOWRE_PASS_REWARD		=	(117),			//Ԫ����ͨ�ؽ���
  SAVETAG_TOWRE_SLOT_REWARD		=	(118),			//Ԫ����ռ����������
  SAVETAG_TOWRE_USE_TREASURE	=	(119),			//ʹ��Ԫ������������
  SAVETAG_TOWRE_TREASURE_REWARD	=	(120),			//ʹ��Ԫ�������佱��
  SAVETAG_RESOURCE_PRODUCT      =   (121),          //��Դ����
  SAVETAG_RESOURCE_OCCUPYAPPLY	=	(122),			//��Դ��ǿ��
  SAVATAG_ENCHANT_REWARD		=	(123),			//��ħ����
  SAVATAG_BOSS_BATTLE			=	(124),			//����BOSSǿ��
  SAVATAG_BOSS_ENCOURAGE		=	(125),			//����BOSS����
  SAVATAG_BOSS_REWARD			=	(126),			//����BOSS����
  SAVATAG_DRAGONBALL_COST		=	(127),			//��������
  SAVATAG_DRAGONBALL_REWARD		=	(128),			//���齱��

  SAVATAG_CS_SUPPORT			=	(129),			//���ս����
  SAVATAG_CS_INSPORE			=	(130),			//���ս����
  SAVATAG_CS_REWARD				=	(131),			//���ս������ȡ

  SAVETAG_TASK_COST				=	(132),			//��������
  SAVETAG_FLAG_UPGRADE			=	(133),			//ս������
  SAVETAG_EQUIP_WASH			=	(134),			//װ��ϴ��
  SAVETAG_EQUIP_EMBED			=	(135),			//װ����Ƕ
  SAVETAG_WORSHIP_COST			=	(136),			//��������

  SAVETAG_ACTIVE_TITLE			=	(137),			//����ͷ��
  SAVETAG_ISLAND_COST			=	(138),			//���������������
  SAVATAG_ISLAND_REWARD			=	(139),			//�����������

  SAVATAG_CHANGE_MASTERNAME		=	(140),			//�޸���������
  SAVATAG_CHANGE_FTNAME			=	(141),			//�޸�ս������
  
  SAVETAG_MONTH_CARD			=	(142),			//�¿�
  SAVETAG_SEEK_WALLADDITION		=	(143),			//��ǽ�ӳ�
  SAVATAG_SEEK_RANSOM			=	(144),			//���
  SAVATAG_SEEK_PLUNDER			=	(145),			//�Ӷ�
  SAVATAG_SEEK_FALGS			=	(146),			//�����Ľ���
  SAVATAG_SEND_PRESENT			=	(147),			//��������
  SAVATAG_GET_PRESENT			=	(148),			//��ȡ����
  SAVATAG_KIDNAP				=	(149),			//��Ʊ
  SAVATAG_SEEK_UPDATE			=	(150),			//ˢ���б�����
  SAVATAG_SEEK_BLACK			=	(151),			//̽�úڵ�
  SAVATAG_ITEM_BOSS				=	(152),			//boss����
  SAVATAG_ITEM_FIGHTADD			=	(153),			//ս�����Լӳ�

  SAVATAG_TOP_REWARD			=	(154),			//���а���
  SAVATAG_PLUNDER_REWARD		=	(155),			//�Ӷ�������

  SAVETAG_EVENT_EXIT            =  (256), //�˳����� ����
};

enum ENUM_BaseDataSaveTagEx
{
	SAVETAGEX_TOWER_HELP		= (3),
	SAVETAGEX_CHARGEDIAMOD		= (4),
	SAVETAGEX_COSTDIAMOD		= (5),
	SAVETAGEX_TOWER_TIME		= (6),
	SAVETAGEEX_WORSHIP_TIME0	= (7),	//����5-11
	SAVETAGEEX_WORSHIP_TIME1	= (8),	//����5-11
	SAVETAGEEX_WORSHIP_TIME2	= (9),	//����5-11
	SAVETAGEEX_WORSHIP_TIME3	= (10),	//����5-11
	SAVETAGEEX_WORSHIP_TIME4	= (11),	//����5-11
	SAVETAGEEX_WORSHIP_TIME5	= (12),	//����5-11
	SAVETAGEEX_OPEN_CHEST		= (13), //��ħ��
	SAVETAGEEX_FINISH_CAVE		= (14), //��Ѩ
	SAVETAGEEX_PVP_CHEST		= (15), //PVP����
	SAVETAGEEX_FT_CHEST			= (16), //�ٻ�����
	SAVETAGEEX_TURNTABLE0		= (17),	//ת��
	SAVETAGEEX_TURNTABLE1		= (18),	//ת��
	SAVETAGEEX_TURNTABLE2		= (19),	//ת��
};


//��ʱ������

//ս��ϵͳ
///////////////////////////////////////////////////////////////////////
//#define Game_Player		6				    //��Ϸ����
//#define Status			6				    //�û�״̬
#define RoundMax		30						//���غ�
#define MosterCount		3						//������Ŀ
//#define PostCount		10						//λ����Ŀ
#define SKILLAFFECT		4						//����Ч��

//����Ч��
#define Positive_Effect	1						//����Ч��
#define Negative_Effect	2						//����Ч��

//����������Ϣ
enum ENUM_DUPLICATE
{
	 ERROR_FOOD			=1,						//ʳ�ﲻ��
	 ERROR_PPHYSICAL	=2,						//��������
	 ERROR_GOODS		=3,						//���߲���
	 ERROR_CONDITION	=4,						//������������
	 ERROR_PAYDIAMOD	=5,						//֧����ʯ
	 ERROR_NO_NUM		=6,						//��������
	 ERROR_DATA			=7,						//���ݴ���
	 ERROR_WORLD_GRADE		=8,						//�ȼ�����
	 ERROR_SCENE_GRADE		=9,						//�ȼ�����
	 ERROR_DIAMOD		=10,						//��ʯ
	 ERROR_TASK			=11,					//����
	 ERROR_DIFFICULTY	=12,					//�Ѷ�
	 ERROR_MAX_NUM		=13,					//������
	 ERROR_PASS_SCENE	=14,					//�ϸ�����δͨ��
};

//ɨ��������Ϣ
enum ENUM_RAIDS
{
	RAIDS_ERROR			= 1,
	RAIDS_TIME			= 2,
	RAIDS_DATA			= 3,
};

//������ʾ
enum ENUM_DUPLICATE_REWARD
{
	 REWARD_CLOSE		=0,					//����δ����
	 GET_REWARD			=1,					//����ȡ����
	 GET_REWARD_FINISH	=2,					//����ȡ����
};

//��ȴ����
enum ENUM_COOLTYPE
{
	SMITHINGEQUIP_COOL	=1,					//ǿ��װ����ȴ
	BLACKREFRESH_COOL	=2,					//������ȴ
	MARKETREFRESH_COOL	=3,					//�̳���ȴ
	FTNUMBERLEAVE_COOL	=4,					//�����ȴ
	TOPCHALLENGE_COOL	=5,					//���а���ս��ȴ
	SEEKLISTUPDATE		=6,					//̽���б�ˢ��
	FTTIMEBOSS_SUMMON	=7,					//ʱ��boss�ٻ�
	FTTIMEBOSS_FIGHT	=8,					//ʱ��bossս��
};

//Ϊ����
enum ENUM_FAKEPRO_TYPE
{
	FAKEPRO_TYPE_WORSHIP	=1,				//��������
	FAKEPRO_TYPE_TOWER		=2,				//Ԫ��������
	FAKEPRO_TYPE_CHEST		=3,				//ħ������
	FAKEPRO_TYPE_CAVE		=4,				//��Ѩ����
	FAKEPRO_TYPE_PVP_CHEST	=5,				//����������
	FAKEPRO_TYPE_FT_CHEST	=6,				//ս���ٻ�����
	FAKEPRO_TYPE_TURNTABLE	=7,				//ת������
};

//��������
enum ENUM_WORSHIP_TYPE
{
	WORSHIP_TYPE_FOOD		=0,				//����Ů��
	WORSHIP_TYPE_TALENT		=1,				//ս��Ů��
	WORSHIP_TYPE_LUCK		=2,				//����Ů��
	WORSHIP_TYPE_FOOD_D		=3,				//����Ů����ʯ
	WORSHIP_TYPE_TALENT_D	=4,				//ս��Ů����ʯ
	WORSHIP_TYPE_LUCK_D		=5,				//����Ů����ʯ
};

//�����޹�
enum ENUM_MARKET_PERSONAL
{
	GOODS_STATUS_NORMAL		=0,				//����״̬
	GOODS_STATUS_UPDATA		=1,				//����״̬,
	GOODS_STATUS_DELETE		=2,				//ɾ��״̬
};

//���ս֧�ִ���
enum ENUM_CS_SUPPORT_ERROR
{
	SUPPORT_ERROR_RES		=1,				//��Դ����
	SUPPORT_ERROR_DATA		=2,				//���ݴ���
	SUPPORT_ERROR_NO_NUM	=3,				//��������
	SUPPORT_ERROR_REWARD	=4,				//����δ��ȡ
	SUPPORT_ERROR_TEAM		=5,				//ս�Ӳ���֧��
};

//���ս�ʸ�ս����
enum ENUM_CS_SEAT_FIGHT_ERROR
{
	SEAT_FIGHT_POS			=1,				//����λ��
	SEAT_FIGHT_SEAT			=2,				//û���ʸ�
	SEAT_FIGHT_CAPTAIN		=3,				//���Ƕӳ�
	SEAT_FIGHT_DATA			=4,				//���ݴ���
	SEAT_FIGHT_COOL_TIME	=5,				//��ȴʱ��
};

//����
enum ENUM_CS_INSPORE_ERROR
{
	INSPORE_DATA			=1,				//���ݴ���
	INSPORE_RES				=2,				//��Դ����
	INSPORE_ID				=3,				//û������
};

//����콱
enum ENUM_CS_REWARD
{
	REWARD_BEG				=1,				//����λ��
	REWARD_DATA				=2,				//���ݴ���
};

//��������
enum ENUM_PET_UPGRADE
{
	UPGRADE_ITEM_NUM		=1,				//���߲���
	UPGRADE_ITEM_ERR		=2,				//���߲�������
	UPGRADE_ITEM_DATA		=3,				//���ݴ���
};

//���＼��
enum ENUM_PET_SKILL
{
	SKILL_ITEM				=1,				//���ߴ���
	SKILL_DATA				=2,				//���ݴ���
	SKILL_GRADE				=3,				//���ܵȼ�
	SKILL_SAME				= 4,             //������λ��װ��ͬ���ͼ���
};

//����û�
enum ENUM_PET_CHANGE
{
	CHANGE_ID				=1,				//�û�ID
};

//����������������
enum ENUM_PET_SKILL_UPGARDE
{
	PET_SKILL_DATA			=1,				//���ݴ���
	PET_SKILL_MAX			=2,				//��߼�
	PET_SKILL_COST			=3,				//���Ĳ���
	PET_SKILL_GRADE			=4,				//�ȼ�����
};

//�¿�
enum ENUM_MONTH_CARD
{
	MONTH_CARD_DATA			=1,				//���ݴ���
	MONTH_CARD_COST			=2,				//���Ĳ���
	MONTH_CARD_ACTIVE		=3,				//�δ����
	MONTH_CARD_TIME			=4,				//����
	MONTH_CARD_GET			=5,				//����ȡ
};

//��ǽ�ӳ�
enum ENUM_WALLADDITION
{
	WALLADDITION_DATA			=1,				//���ݴ���
	WALLADDITION_COST			=2,				//���Ĳ���
	WALLADDITION_GRADE			=3,				//�ȼ�����
};

//��ǽ����
enum ENUM_WALLARMY
{
	WALLARMY_ID					=1,				//����ID
	WALLARMY_SAME				=2,				//�ظ�
	WALLARMY_COUNT				=3,				//����
	WALLARMY_MASTER				=4,				//����
};

//��Ʊ����
enum ENUM_KIDNAP_TYPE
{
	KIDNAP_WOMAN				=1,				//ʥŮ
	KIDNAP_FARMER				=2,				//ũ��
	KIDNAP_PET					=3,				//����
};

//��Ʊ
enum ENUM_KIDNAP
{
	KIDNAP_TYPE					=1,				//���ʹ���
	KIDNAP_DATA					=2,				//���ݴ���
	KIDNAP_USER					=3,				//�޴��û�
	KIDNAP_STATUS				=4,				//״̬
	KIDNAP_NUM					=5,				//��������
	KIDNAP_ENOUGH				=6,				//�Ѿ�ץ��ũ��ʥŮ
	KIDNAP_NO_PET				=7,				//�����ѱ�ץ��
};

//̽��״̬
enum ENUM_KIDNAP_STATUS
{
	KIDNAP_STATUS_NO			=0,				//û̽�ù�
	KIDNAP_STATUS_YES			=1,				//̽�ù�
};

//Ӫ������
enum ENUM_RESCUE_TYPE
{
	RESCUE_FIGHT				=1,				//ս��
	RESCUE_MONEY				=2,				//���
};

//Ӫ��
enum ENUM_RESCUE
{
	RESCUE_DATA					=1,				//���ݴ���
	RESCUE_RES					=2,				//��Դ����
	RESCUE_USER					=3,				//�޴��û�
	RESCUE_HOSTAGE				=4,				//û������
	RESCUE_NUM					=5,				//��������
};

//�Ӷ�
enum ENUM_PLUNDER
{
	PLUNDER_BUILD_ID			=1,				//ID����
	PLUNDER_BUILD_TYPE			=2,				//����
	PLUNDER_NUM					=3,				//����
	PLUNDER_DATA				=4,				//���ݴ���
};

//̽�ò鿴ս��
enum ENUM_SEEK_FIGHTING
{
	SEEK_FIGHTING_DATA			=1,				//���ݴ���
	SEEK_FIGHTING_USER			=2,				//�޴��û�
};

//α�����
#define RAND_MAX_EX 888888	

//�������ݿֵⷧ
#define UPDATA_DATA_VALUE	20					//	

////////////////////////////////////////////////////////////////////////
//����ѡ��


//#define  _TEST_CLOSE_NET					  //���Ի����������ر�����
//#define  _SHOW_FIGHT_INFO                   //ս����Ϣ��ӡ
//#define  _SHOW_MESSAGE_INFO                 //��Ϣͳ�ƴ�ӡ
//#define  _SHOW_MP_INFO                      //����ħ����Ϣ

#ifndef _WIN32
//#define _DEBUG                                //����linux�µ�debug
#endif

#ifdef _DEBUG
#define CT_ASSERT(exp) assert(exp)
#else
#define CT_ASSERT(exp)
#endif

/////////////////////////////////////////////////////////////////////////
#endif