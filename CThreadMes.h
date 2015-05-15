/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCDatabaseMes.h
Version:		1.0
Description:    �����߳�ͨѶЭ��
Author:			pengwen
Create Date:	2012-8-16
History:        �����߳�ͨѶЭ��     

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

//db�߳�����Ϣ
//�û���¼
#define  MSC_MAIN_DB_LOGIN 						(SYS_MSC_M_MIN + 200)
//������Ϣ����
#define  MSC_MAIN_DB_SAVE_BUILDING              (SYS_MSC_M_MIN + 201)
//�������
#define  MSC_MAIN_DB_UPGRAE_FINISH              (SYS_MSC_M_MIN + 204)  //��Ҫ�Ż�
//��������
#define  MSC_MAIN_DB_SAVE_DATA                  (SYS_MSC_M_MIN + 205)  //��Ҫ�Ż�
//ɾ������
#define  MSC_MAIN_DB_DEL_DATA                   (SYS_MSC_M_MIN + 206)
//�õ�����
#define  MSC_MAIN_DB_GET_DATA                   (SYS_MSC_M_MIN + 207)
//ս������
#define  MSC_MAIN_DB_FIGHTING_REWARD			(SYS_MSC_M_MIN + 208)
//�¼�����
#define  MSC_MAIN_DB_BUILDING_EVENT             (SYS_MSC_M_MIN + 209)
// �������ݸ��� [10/10/2012 John]
#define  MSC_MAIN_DB_ITEM_UPDATE                (SYS_MSC_M_MIN + 210)
// ��ȡ�û�ս������
//#define  MSC_MAIN_DB_USER_BATTLE                (SYS_MSC_M_MIN + 211)
//�������ݸ���
#define  MSC_MAIN_DB_REGION_UPDATE				(SYS_MSC_M_MIN + 220)
//��������
#define  MSC_MAIN_DB_ARMY_DATA					(SYS_MSC_M_MIN + 230)
//������
#define  MSC_MAIN_DB_BATTLE_ARRAY_DATA			(SYS_MSC_M_MIN + 240)
//�������ݸ���
#define  MSC_MAIN_DB_TASK_UPDATE				(SYS_MSC_M_MIN + 250)
//����������
#define  MSC_MAIN_DB_PVP_DATA					(SYS_MSC_M_MIN + 260)
//�̵�����
#define	 MSC_MAIN_DB_MARKET_DATA				(SYS_MSC_M_MIN + 270)
//�̵����ݣ����У�
#define  MSC_MAIN_MARKETDATA                    (SYS_MSC_M_MIN + 271)                 

//��������
#define  MSC_MAIN_DB_PET_DATA					(SYS_MSC_M_MIN + 280)
//�ʼ�����
#define  MSC_MAIN_DB_EMAIL_DATA					(SYS_MSC_M_MIN + 290)
//�û��˳���Ϸ
#define  MSC_MAIN_DB_USER_EXIT                  (SYS_MSC_M_MIN + 300)
//vip����
#define  MSC_MAIN_DB_VIP_DATA					(SYS_MSC_M_MIN + 310)
// ��ֵ����
#define  MSC_MAIN_DB_CHARGE_DATA                (SYS_MSC_M_MIN + 320)
//��־���ݿ�
#define  MSC_MAIN_LOGDB_DATA					(SYS_MSC_M_MIN + 330)     //��Ҫ�Ż�
//�û���¼��Ϣ����
#define  MSC_MAIN_INSERT_USERDATA				(SYS_MSC_M_MIN + 340)
//�����
#define  MSC_MAIN_ACTIVE_EVENT_DATA				(SYS_MSC_M_MIN + 350)
//��������
#define  MSC_MAIN_WORSHIP_EVENT_DATA			(SYS_MSC_M_MIN + 360)
//��������
#define  MSC_MAIN_DB_FRIEND_DATA				(SYS_MSC_M_MIN + 370)
//���ѽ��̼�ͨѶ
#define  MSC_MAIN_FRIEND_EVENT_DATA				(SYS_MSC_M_MIN + 380)
//ս������
#define  MSC_MAIN_DB_FIGHTTEAM_DATA				(SYS_MSC_M_MIN + 390)
//ս�ӽ��̼�ͨѶ
#define  MSC_MAIN_FIGHTTEAM_EVENT				(SYS_MSC_M_MIN + 400)
//�����
#define  MSC_MAIN_DB_ACTIVE_DATA				(SYS_MSC_M_MIN + 410)
//Ԫ��������
#define  MSC_MAIN_DB_TOWER_DATA					(SYS_MSC_M_MIN + 420)
//����
#define MSC_MAIN_DB_ANNOUNCEMENT				(SYS_MSC_M_MIN + 430)
//ȫ������
#define MSC_MAIN_DB_GLOBEDATA					(SYS_MSC_M_MIN + 440)

#define MSC_MAIN_DB_UPDATEDATA					(SYS_MSC_M_MIN + 450)

//���а�
#define MSC_MAIN_TOP_INFO                       (SYS_MSC_M_MIN + 460)
//���а�db
#define MSC_MAIN_TOP_DB_INFO                    (SYS_MSC_M_MIN + 470)  
//����BOSS
#define MSC_MAIN_DB_WORLDBOSS                   (SYS_MSC_M_MIN + 480)  

#define MSC_MAIN_TASK_EVENT_DATA				(SYS_MSC_M_MIN + 490)  
//���ս
#define MSC_MAIN_DB_CORSS_FIGHT					(SYS_MSC_M_MIN + 500)

//����
#define MSC_MAIN_DB_DRAGONISLAND				(SYS_MSC_M_MIN + 510)

//̽��
#define MSC_MAIN_DB_SEEK						(SYS_MSC_M_MIN + 520)

//////////////////////////////////////////////////////////////////////////
// �û��߳�����Ϣ
// �������ݸ���
#define  MSC_MAIN_USR_ITEM_UPDATE               (SYS_MSC_M_MIN + 101)
//ս���¼�[10/23/2012 wangchen]
#define  MSC_MAIN_FIGHTING_EVENT				(SYS_MSC_M_MIN + 102)
//�����¼�[10/24/2012 wangchen]
#define  MSC_MAIN_DUPLICATE_EVENT				(SYS_MSC_M_MIN + 103)
//�����¼�[11/6/2012 wangchen]
#define  MSC_MAIN_EVENT_CITY					(SYS_MSC_M_MIN + 104)
//������[11/22/2012 wangchen]
#define  MSC_MAIN_EVENT_PVP			  			(SYS_MSC_M_MIN + 105)
//����
#define	 MSC_MAIN_EVENT_PET						(SYS_MSC_M_MIN + 106)
//�û��˳���Ϸ
#define  MSC_MAIN_USER_EXIT						(SYS_MSC_M_MIN + 107)
//������Ϣ
#define  MSC_MAIN_BATTLE_ARRAY					(SYS_MSC_M_MIN + 108)
//�㲥
#define  MSC_MAIN_BROADCAST                     (SYS_MSC_M_MIN + 109)
//��Ӹ���
#define  MSC_MAIN_TEAMDUPLICATE                 (SYS_MSC_M_MIN + 110)
//��Դ����
#define  MSC_MAIN_RES_OCCUPY                    (SYS_MSC_M_MIN + 111)
//Ԫ��������ͨѶ
#define MSC_MAIN_TOWER_EVENT				    (SYS_MSC_M_MIN + 112)
//�������
#define MSC_MAIN_ANNOUNCEMENT_UPDATA			(SYS_MSC_M_MIN + 113)


//������Ϣ
#define MSC_MAIN_MASTER                         (SYS_MSC_M_MIN + 114)
//�û�������Ϣ
#define MSC_MAIN_USER_ENTER                     (SYS_MSC_M_MIN + 115)
//��Դ����
#define  MSC_MAIN_WORLD_BOSS                    (SYS_MSC_M_MIN + 116)
//���ս
#define MSC_MAIN_CORSS_FIGHT					(SYS_MSC_M_MIN + 117)

//����
#define MSC_MAIN_ISLAND_EVENT					(SYS_MSC_M_MIN + 118)
//̽��
#define MSC_MAIN_SEEK							(SYS_MSC_M_MIN + 119)
//////////////////////////////////////////////////////////////////////////
//�߳�����Ϣ
//�������ݿ������Ϣ
//�û�����
#define  MSC_SUB_DB_USRE_ENTER					(10)
//���а���Ϣ
#define  MSC_SUB_DB_TOP_INFO					(12)
//PVP������Ϣ
#define	 MSC_SUB_DB_CHALLENG					(13)
//�鿴������Ϣ
#define MSC_SUB_DB_ANOTHER						(14)
//��ȡս�ӳ�Աս������
#define MSC_SUB_DB_FTFightingData				(15)
//������������
#define  MSC_SUB_DB_MASTER_DATA                 (20)
//��������������ȴ��Ϣ
#define  MSC_SUB_DB_MASTER_COOLDOWN             (21)

#define  MSC_SUB_DB_MASTER_DATA_EX              (22)
//��������ս������
#define  MSC_SUB_DB_MASTER_FIGHT_DATA           (23)
//����ͬ��
#define  MSC_SUB_DB_SYS_PROPDUCTING             (29)
//�ֶ�������������
#define  MSC_SUB_DB_PRODUCTING_DATA             (30)
//���²߻�����������ȡ״̬
#define  MSC_SUB_DB_PRESENT						(31)


// Begin [11/29/2012 wangchen]
//���湺����������
#define  MSC_SUB_DB_BUY_STATMINA                (32)
// end [11/29//2012 wangchen]

// Begin [9/20/2012 John]
// ����Ǳ����Ϣ
#define  MSC_SUB_DB_W_TALENT_INFO               (40)
// ����ս����Ϣ
#define  MSC_SUB_DB_W_BATTLEFLAG_INFO           (41)
// ���漼����Ϣ
#define  MSC_SUB_DB_W_SKILL_INFO                (50)
// ͬ��Ԫ����Ϣ
#define  MSC_SUB_DB_W_ELM_SYNC                  (55)
// ͬ��Ԫ��������Ϣ
#define  MSC_SUB_DB_W_ELMATTR_SYNC              (56)
// ͬ��װ����Ϣ
#define  MSC_SUB_DB_W_EQUIP_SYNC                (57)
// ͬ��������Ϣ
#define  MSC_SUB_DB_W_ITEM_SYNC                 (58) 
// ͬ��������Ϣ
#define  MSC_SUB_DB_W_BUFF_SYNC                 (59)
// �޸���������
#define  MSC_SUB_DB_W_MASTER_NAME               (60)
// ��������ǩ��
#define  MSC_SUB_DB_W_SIGN_INFO                 (110)
// ���������½��¼ 
#define  MSC_SUB_DB_D_SIGN_DATA                 (111)
// �����ճ��������
#define  MSC_SUB_DB_W_DAILY_QUEST               (120)
// ���ÿ�ռ�¼
#define  MSC_SUB_DB_D_DAILY_DATA                (121)
// ���»�Ծ����Ϣ
#define  MSC_SUB_DB_W_ACTIVE_INFO               (130)
// ������ȡǩ������
#define  MSC_SUB_DB_W_SIGN_REWARD               (140)
// ������ȡ��Ծ�Ƚ���
#define  MSC_SUB_DB_W_ACTIVE_REWARD             (150)
// End [9/20/2012 John]

//PVP�����߶�����Ϣ
#define  MSC_SUB_CHALLENGE						(160)
//���а���Ϣ
#define  MSC_SUB_TOP							(161)
//ս����¼
#define  MSC_SUB_RECTORD						(162)
// ���¸�ħ��Ϣ
#define  MSC_SUB_DB_W_ENCHANT_DATA              (170)
// ���¸�ħʧ�ܴ�����Ϣ
#define  MSC_SUB_DB_W_ENCHANT_FAILTIME			(171)
// ���¸�ħ������Ϣ
#define  MSC_SUB_DB_W_ENCHANT_REWARD			(172)
//��Ӹ����뿪
#define  MSC_SUB_TEAMDUPLICATE_LEAVE			(180)
//��Ӹ���ս������
#define  MSC_SUB_TEAMDUPLICATE_FIGHTINGEND		(181)
//��Ӹ���ս��
#define MSC_SUB_TEAMDUPLICATE_FIGHTING			(182)
//��Ӹ����Զ�ƥ��
#define MSC_SUB_TEAMDUPLICATE_AUTOMATCH			(183)
//��Ӹ����Զ�ƥ����Ϣ
#define MSC_SUB_TEAMDUPLICATE_AUTOMATCHINFO		(184)
// ����ͷ����Ϣ
#define MSC_SUB_DB_W_TITLE_DATA					(185)					

// End [11/15/2012 John]
// ʧȥ����
#define  MSC_SUB_DB_LOSE_BUFF                   (1000)

// ��ӳ�ֵ��¼
#define  MSC_SUB_DB_ADD_CHARGE                  (2001)
// ���³�ֵ��¼
#define  MSC_SUB_DB_UPDATE_CHARGE               (2002)

//��־����
//////////////////////////////////////////////////////////////////////////
//����������¼
#define  MSC_SUB_LOGDB_ARMYUPDATAGRADE			(3000)
//���ӽ���
#define  MSC_SUB_LOGDB_ARMYUPDATARANK			(3001)
//���򱳰����Ӽ�¼
#define  MSC_SUB_LOGDB_BAGSLOTUPDATA			(3002)
//���ӳ��м�¼
#define  MSC_SUB_LOGDB_MASTERAREA				(3003)
//������ǿ
#define  MSC_SUB_LOGDB_PROPADDSTRENGTHEN		(3004)
//����ʹ�����¼
#define  MSC_SUB_LOGDB_PROPFINISH				(3005)
//���߼̳м�¼
#define  MSC_SUB_LOGDB_PROPINHERIT				(3006)
//������ǿ
#define  MSC_SUB_LOGDB_PROPUPDATATRAIN			(3007)
//���漼�ܼ�¼
#define  MSC_SUB_LOGDB_SAVESKILL				(3008)
//�¼���ɼ�¼
#define	 MSC_SUB_LOGDB_SAVEEVENTFINISH			(3009)
//�����츳��¼
#define  MSC_SUB_LOGDB_SAVETALENT				(3010)
//����������ɼ�¼
#define  MSC_SUB_LOGDB_TASKFINISH				(3011)
//�û��˳���¼
#define  MSC_SUB_LOGDB_USEREXIT					(3012)
//�û�������¼
#define  MSC_SUB_LOGDB_UPGRADEFINISH			(3013)
//vip�����¼
#define  MSC_SUB_LOGDB_VIPAPPLY					(3014)
//��Ծ�Ƚ���
#define  MSC_SUB_LOGDB_ACTIVEREWARD				(3015)
//��������ͳ��
#define  MSC_SUB_LOGDB_ONLINEUSER				(3016)
//�û������¼
#define  MSC_SUB_LOGDB_CHATTIME					(3017)
//�û�������¼
#define  MSC_SUB_LOGDB_PRODUCTFINISH			(3018)
//�û���ֵ��¼
#define	 MSC_SUB_LOGDB_CHARGE					(3019)
//�û���ֵ����
#define  MSC_SUB_LOGDB_CHARGEUPDATE				(3020)
//ÿ��ǩ��
#define  MSC_SUB_LOGDB_SIGNREWARD				(3021)
//PVP����
#define  MSC_SUB_LOGDB_CHEST					(3022)
//����������������
#define  MSC_SUB_LOGDB_PETUPSKILL				(3023)
//��������
#define  MSC_SUB_LOGDB_PETRENAME				(3024)
//��Ʒ����
#define  MSC_SUB_LOGDB_ITEMSELL					(3025)
//�̵깺���¼
#define  MSC_SUB_LOGDB_SHOPPING					(3026)
//��������
#define  MSC_SUB_LOGDB_STATMINA					(3027)
//pvp����
#define  MSC_SUB_LOGDB_PVPREWARD				(3028)
//����ս������
#define  MSC_SUB_LOGDB_DUPLICATE				(3029)
// ��ħ��¼
#define  MSC_SUB_LOGDB_ENCHANT                  (3030)
//����������ȡ
#define  MSC_SUB_LOGDB_DUPLICATEREWARD			(3031)
//��������
#define  MSC_SUB_LOGDB_DUPLICATEENTER			(3032)
//������ս������
#define  MSC_SUB_LOGDB_PVPFIGHTINGCOST			(3033)
//�����
#define  MSC_SUB_LOGDB_ACTIVEEVENTREWARD		(3034)
//����ս����������
#define  MSC_SUB_LOGDB_BUYFIGHTINGTIE			(3035)
//���뽱��
#define MSC_SUB_LOGDB_WORSHIP_REWARD			(3036)
//ǿ��ˢ��
#define MSC_SUB_LOGDB_PVPREFRESH				(3037)
//��ʯ����ǿ����ȴʱ��
#define MSC_SUB_LOGDB_CLEARCD					(3038)
//��Ϣ����
#define MSC_SUB_LOGDB_FEEDBACK					(3039)
//ɨ������
#define MSC_SUB_LOGDB_RAIDSSPEED				(3040)
//��Ӹ�������
#define MSC_SUB_LOGDB_TEAMDUPLICATEREWARD		(3041)
//��Ӹ�������
#define MSC_SUB_LOGDB_TEAMDUPLICATECOST			(3042)
//ս��
#define MSC_SUB_LOGDB_FIGHTTEAM					(3043)
//����ٺ»
#define MSC_SUB_LOGDB_SALARY					(3044)
//��������
#define MSC_SUB_LOGDB_RANKINGUPGRADE			(3045)
//��Ʒʹ�ü�¼
#define MSC_SUB_LOGDB_USESITEM					(3046)
//�¼�������¼
#define MSC_SUB_LOGDB_EVENTREWARD				(3047)
//��������
#define MSC_SUB_LOGDB_PETUPGRADE				(3048)
//masterdatarecord
#define MSC_SUB_LOGDB_MASTER_DATA				(3049)
//ս������
#define MSC_SUB_LOGDB_FIGHTTEAM_UPGRADE			(3050)
//ս���ٻ�����
#define MSC_SUB_LOGDB_FIGHTTEAM_SUMMONCOST		(3051)
//ս���ٻ�������Դ
#define MSC_SUB_LOGDB_FIGHTTEAM_SUMMONREWARDRES	(3052)
//ս���ٻ���������
#define MSC_SUB_LOGDB_FIGHTTEAM_SUMMONREWARDITEM (3053)
//ս�ӿƼ�����
#define MSC_SUB_LOGDB_FIGHTTEAM_SCIENCE_UPGRADE	(3054)
//Ԫ����ͨ��
#define MSC_SUB_LOGDB_TOWER_PASS				(3055)
#define MSC_SUB_LOGDB_TOWER_OCCUPY				(3056)
#define MSC_SUB_LOGDB_TOWER_HELP				(3057)
#define MSC_SUB_LOGDB_ADDSITEM					(3058)
#define MSC_SUB_LOGDB_BATTLEFLAG				(3059)
#define MSC_SUB_LOGDB_EATELM					(3060)
#define MSC_SUB_LOGDB_TREASUREUSE				(3061)
#define MSC_SUB_LOGDB_EMBEDEQUIP				(3062)

//���ս
#define MSC_SUB_LOGDB_CSREWARD					(3063)
#define MSC_SUB_LOGDB_INSPORE_COST				(3064)
#define MSC_SUB_LOGDB_SUPPORT					(3065)

//ͷ��
#define MSC_SUB_LOGDB_TITLE						(3066)
//����
#define MSC_SUB_LOGDB_ISLAND					(3067)
//��ǽ�ӳ�
#define MSC_SUB_LOGDB_WALLADDITION				(3068)
//�Ӷ�
#define MSC_SUB_LOGDB_PLUNDER					(3069)
//̽�ü�¼
#define MSC_SUB_LOGDB_SEEKDATA					(3070)						

//db�̷߳��ص�����Ϣ��־λ
//���ݿ������쳣����
#define  MSC_SUB_DB_R_NET_ERROR                 (10000)
//���ݿ��û�з����û�
#define  MSC_SUB_DB_R_NO_USER                   (10001)
//��ȡ�������
#define  MSC_SUB_DB_R_ENTER_FINISH              (10005)





//��¼��Ϣ
//������Ϣ
// װ����Ϣ
#define  MSC_SUB_DB_R_EQUIP_INFO                (10011)

// End [9/20/2012 John]



// �û��߳�����Ϣ
// ��ӵ���
#define  MSC_SUB_USR_ADD_ITEM                     (11000)
//ɾ������
#define  MSC_SUB_G_DELETE_ITEM					  (11001)
//ս���¼�[10/23/2012 wangchen]
#define  MSC_SUB_FIGHTING_EVENT                   (20001)
//ս������[10/23/2012 wangchen]
#define  MSC_SUB_FIGHTING_END                     (20002)
//�ۿ�ս��[10/23/2012 wangchen]
#define  MSC_SUB_LOOK_FIGHTING                    (20003)
//�¼�ս��
#define  MSC_SUB_EVENT_FIGHTING					  (20004)
//�¼�ս������[11/6/2012 wangchen]
#define  MSC_SUB_EVENT_FIGHTING_END               (20005)
//������
#define	 MSC_SUS_PVP_FIGHTING					  (20006)
//������ս������
#define  MSC_SUB_PVP_FIGHTING_END                 (20007)
//PVP�ۿ�ս��
#define  MSC_SUB_PVP_LOOK_FIGHTING                (20008)
//����������
#define  MSC_SUB_DUPLICATE_FINISH			      (20009)
//������Ϣ
#define  MSC_SUB_PVP_CHALLENG					(20010)
//PVPս����Ϣ
#define  MSC_SUB_PVP_RECORD						(20011)
//pvp����ͼ�����
#define  MSC_SUB_PVP_LOW_CHALLER                (20012)
//pvp����߼�����
#define  MSC_SUB_PVP_HIGN_CHALLER               (20013)
//pvp����������
#define  MSC_SUB_PVP_INSERT_CHALLER_FINISH      (20014)


//ɨ������
#define  MSC_SUB_RAIDS_REWARD					(20112)
//��Դ�Ӷ�
#define MSC_SUB_RESREBBERY_TEAMPVP				(20113)
//��Դ�Ӷ�
#define MSC_SUB_RESREBBERY_TEAMPVE				(20114)
//�鿴ս��ս��PVP
#define MSC_SUB_FIGHTTEAM_PVP					(20115)
//�鿴ս��ս��PVE
#define MSC_SUB_FIGHTTEAM_PVE					(20116)
//��Դ�Ļ����
#define MSC_SUB_RESACTVIE_OPEN					(20117)
//��Դ����ȡ���
#define MSC_SUB_RESACTVIE_READFINISH			(20118)
//Ԫ�����ؿ�ͨ��
#define MSC_SUB_TOWER_CP_PASS					(20119)
//Ԫ�����ؿ�ͨ�ؼ�¼�鿴
#define MSC_SUB_TOWER_CPPASS_LOOK				(20120)

#define  MSC_SUB_FIGHTING_REWARD                (20121)
//��Դ�Ļ����
#define  MSC_SUB_INSERT_RESACTIVE				(20122)

//Ԫ�����û�֮����Դ����
#define MSC_SUB_TOWER_RES_PVP                   (20150)
//Ԫ����PVPս���鿴
#define MSC_SUB_TOWER_PVP_LOOK                  (20151)
//ս��BOSS
#define MSC_SUB_FT_SUMMON_BOSS					(20160)
//ս��ʱ��BOSS
#define MSC_SUB_FT_TIME_BOSS					(20161)
//ս��ʱ��BOSS�뿪
#define MSC_SUB_FT_TIME_BOSS_LEAVE				(20162)

//����BOSS�����
#define MSC_SUB_BOSSACTVIE_OPEN					(20170)


//��¼����
//���콨��
//ͬ��������Ϣ
#define  MSC_SUB_DB_SYS_BUILDING_UPGRADINT    (21002)
//���潨����Ϣ
#define  MSC_SUB_DB_SAVE_BUILDING             (21003)
//�������������Ľ�����Ϣ
#define  MSC_SUB_DB_SAVE_UPGRADING_BUILDING   (21004)
//�������
#define  MSC_SUB_DB_R_BUILDING_UPGRADING_FINISH (21006)
//�������

//�����¼�
#define  MSC_SUB_DB_SAVE_AWARD_DATA             (22000)
//�¼�����
//������¼�¼
#define MSC_SUB_DB_REGION_RECORD_DATA			(23000)

//������¼�¼
#define MSC_SUB_DB_TASK_DATA					(24000)
#define MSC_SUB_DB_TASK_RECORD_DATA				(24001)

//////////////////////////////////////////////////////////////////
//�̵����ݿ��¼
#define MSC_SUB_DB_MARKET_RECORD_DATA			(25000)
//�û��ڵ�,�̳ǹ��ܿ���
#define MSC_SUB_DB_MARKET_ADD				    (25001)
//�ڵ���Ϣˢ��
#define MSC_SUB_DB_BLACK_MARKET_REFRESH			(25003)
//�ڵ깺��
#define MSC_SUB_DB_BLACK_MARKET_SHOPPING		(25004)
//�����޹�
#define MSC_SUB_DB_MARKET_PERSONAL				(25005)
//�����޹�ˢ��
#define MSC_SUB_DB_REFRESH						(25006)
//�����޹���Ϣ
#define MSC_SUB_UPDATE_PERSON                   (25007)                 

////////////////////////////////////////////////////////////////
//������
#define MSC_SUB_DB_PET_INFO						(26000)
//�õ��û�
#define MSC_SUB_DB_PET_CHANGE					(26001)
//���ﵰʹ��
#define MSC_SUB_USE_EGG							(26101)

/////////////////////////////////////////////////////////////////
//�ʼ�����
//�û��ʼ���¼
#define MSC_SUB_DB_EMAIL_USER					(27001)


// Begin [10/20/2012 wangchen]
//���糡����Ϣ
#define MSC_SUB_DB_R_DUPLICATE_WORLD_SCENE		(30001)
//������Ϣ
#define MSC_SUB_DB_R_DUPLICATE_SCENE_INFO		(30002)
//ս����Ϣ
#define MSC_SUB_DB_W_FIGHTING_INFO				(30003)
//����ս������
#define MSC_SUB_DB_W_BUYFIGHTINGTIME			(30004)
//ɨ����Ϣ
#define MSC_SUB_DB_RAIDS_INFO					(30005)
//ɨ������
#define MSC_SUB_DB_RAIDS_REWARD					(30006)
//��Ӹ�������
#define MSC_SUB_DB_TEAMDUPLICATE_SCENE			(30007)
//��Ӹ����Զ�ƥ��
#define MSC_SUB_DB_TEAMDUPLICATE_AUTOMATCH		(30008)

//������Ϣ
//#define MSC_SUB_DB_R_ARMY_INFO					(31001)
//��������
#define MSC_SUB_DB_R_ARMY_UPGRADE				(31002)
//���ӽ���
#define MSC_SUB_DB_R_ARMY_ADVANCED				(31003)
//������Ϣ
#define MSC_SUB_DB_ARMY_INFO					(31004)
//���׸�����
#define MSC_SUB_UPDATA_BATTLE_ARRAY				(31005)

//����Ϣ
//#define MSC_SUB_DB_BATTLE_ARRAY_INFO			(32001)
//����Ϣ
#define MSC_SUB_DB_W_BATTLE_ARRAY_INFO			(32002)

//��������Ϣ
#define MSC_SUB_DB_R_PVP_INFO					(33001)
//ˢ�¶���
#define MSC_SUB_DB_W_REFRESH					(33004)
//ս����¼
#define MSC_SUB_DB_PVP_FIGHTING_RECORD			(33008)
//��ȡ�û���Ϣ
#define MSC_SUB_DB_R_USER_INFO					(33009)
//ս����¼
#define MSC_SUB_DB_FIGHTING_END					(33013)
//��ս״��
#define MSC_SUB_DB_CHALLENGE_INFO				(33015)
//�������ս�
#define MSC_SUB_DB_PVPENDING					(33016)
//��������������д�����ݿ�
#define MSC_SUB_DB_PVP_CHALLGER_SAVE            (33017)

//vip�����ȡ
#define MSC_SUB_DB_GIFT_GET						(34001)
//vip����
#define  MSC_SUB_DB_VIP_RENEWAL					(34002)
//vip����
#define MSC_SUB_DB_VIP_END						(34003)
// End [10/20/2012 wangchen]

//�û���¼���ݲ���
////////////////////////////////////////////////////////////
//������Ϣ
#define  MSC_SUB_INSERT_MASTERDATA				(35000)
//�õ�����������������Ϣ
#define  MSC_SUB_INSERT_UPGRADEBUILD			(35001)
//�õ�Ǳ������
#define  MSC_SUB_INSERT_TALENTINFO				(35002)
//�õ���������
#define  MSC_SUB_INSERT_ITEMINFO				(35003)
//�õ�װ����Ϣ
#define	 MSC_SUB_INSERT_EQUIPINFO				(35004)
//�õ���������
#define  MSC_SUB_INSERT_BUFFINFO				(35005)
//�õ���ȴ��Ϣ
#define  MSC_SUB_INSERT_COOLDOWNINFO			(35006)
//�õ�������Ϣ
#define  MSC_SUB_INSERT_ARMYINFO				(35007)
//�õ����������Ľ���
#define  MSC_SUB_INSERT_PRODUCTING				(35008)
//�õ����糡������
#define  MSC_SUB_INSERT_WORLDSCENE				(35009)
//�õ���������
#define  MSC_SUB_INSERT_SCENEINFO				(35010)
//�õ��û���������
#define  MSC_SUB_INSERT_REGIONINFO				(35011)
//�õ�����Ϣ
#define  MSC_SUB_INSERT_BATTLEARRAY				(35012)
//�õ��û����������
#define  MSC_SUB_INSERT_TASKFINISH				(35013)
//�õ��û��ڽ�������
#define  MSC_SUB_INSERT_TASKPORGRESS			(35014)
//�õ��ճ����������Ϣ
#define  MSC_SUB_INSERT_DAILYQUEST				(35015)
//�õ���Ծ����Ϣ
#define  MSC_SUB_INSERT_DAILYACTIVE				(35016)
//�õ�������Ϣ
#define  MSC_SUB_INSERT_BUILDINGINFO			(35017)
//�õ��̵�״̬
#define  MSC_SUB_INSERT_MARKETFUNCTION			(35018)
//��ȡ�ڵ���Ϣ
#define  MSC_SUB_INSERT_BLACKMARKETINFO			(35019)
//�õ�������Ϣ
#define  MSC_SUB_INSERT_PETINFO					(35020)
//��ȡvip��Ϣ
#define  MSC_SUB_INSERT_VIPINFO					(35021)
//��������Ϣ
#define  MSC_SUB_INSERT_PVPINFO					(35022)
//PVP������Ϣ
#define  MSC_SUB_INSERT_PVPCHALLENG				(35023)
//���а���Ϣ
#define  MSC_SUB_INSERT_TOP					(35024)
//pvpս����¼
#define  MSC_SUB_INSERT_PVPRECORD				(35025)
//�õ���������
#define  MSC_SUB_INSERT_SKILLDATA				(35026)
//�õ�ǩ����Ϣ
#define  MSC_SUB_INSERT_SIGNINFO				(35027)
//��ȡ��ħ��Ϣ
#define  MSC_SUB_INSERT_ENCHANTDATA				(35028)
//�õ�����ս������
#define  MSC_SUB_INSERT_BUYFIGHTINGTIME			(35029)
//ɨ����Ϣ
#define  MSC_SUB_INSERT_RAIDSINFO				(35030)
//������Ϣ
#define  MSC_SUB_INSERT_RAIDSREWARD				(35031)
//������Ϣ
#define  MSC_SUB_INSERT_FRIENDINFO				(35032)
//��Ӹ�����Ϣ
#define  MSC_SUB_INSERT_TEAMDUPLICATE			(35033)
//�Զ���ȡ��Ӫ���߽���
#define  MSC_SUB_INSERT_PRESENT					(35034)
//�����޹�
#define  MSC_SUB_INSERT_MARKETPERSONAL			(35035)
//Ԫ��֮��
#define  MSC_SUB_INSERT_ELMPOWER                (35036)
//ս��
#define  MSC_SUB_INSERT_BATTLEFLAG              (35037)
// Ԫ������
#define  MSC_SUB_INSERT_ELMATTR                 (35038)
//Ԫ��֮��
#define  MSC_SUB_INSERT_TOWER					(35039)
#define  MSC_SUB_INSERT_TOWERSLOTREWARD			(35040)
#define  MSC_SUB_INSERT_MSG						(35041)
//ս�ӿƼ�
#define  MSC_SUB_INSERT_FIGHTTEAM_SCIENCE		(35042)
#define  MSC_SUB_INSERT_TOWEREVENT				(35043)
#define  MSC_SUB_INSERT_TOWERSLOTFIGHTING		(35044)

#define  MSC_SUB_INSERT_TOWERHELP				(35045)
//��ħ����
#define  MSC_SUB_INSERT_ENCHANT_REWARD			(35046)
#define  MSC_SUB_INSERT_BOSS_RECORD				(35047)
//�õ�����û�
#define  MSC_SUB_INSERT_PETCHANGE				(35048)
//ͷ��
#define  MSC_SUB_INSERT_TITLE					(35049)
//����
#define  MSC_SUB_INSERT_ISLANDDATA				(35050)
#define  MSC_SUB_INSERT_ISLAND_GIVEUID			(35051)
#define  MSC_SUB_INSERT_ISLAND_SCOREREWARD		(35052)
#define  MSC_SUB_INSERT_ISLAND_PRESENTNAME		(35053)
//̽��
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
//� ��Ϣ
#define  MSC_SUB_ACTIVE_STATUS_UPDATE			(36001)
//����������ʱ����Ҫ��ȡ���Ϣ��
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
//������Ϣ
#define MSC_SUB_WORSHIP_STATUS_UPDATE			(37001)
//////////////////////////////////////////////////////////////////////////


//������Ϣ
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
#define MSC_SUB_DB_FIGHTTEAM_MEM_REFRESH			(39019) //������� ���ս��ˢ��
#define MSC_SUB_FIGHTTEAM_SUMMON_FIGHTING			(39020)
#define MSC_SUB_FIGHTTEAM_SUMMON_LEAVE				(39021)
#define MSC_SUB_DB_FIGHTTEAM_SCIENCE_UPGRADE		(39022)
#define MSC_SUB_DB_FIGHTTEAM_SUMMON_DATA			(39023)
#define MSC_SUB_DB_FIGHTTEAM_SUMMON_ITEM			(39024)
#define MSC_SUB_DB_FIGHTTEAM_DOWN_GRADE				(39025)
#define MSC_SUB_FIGHTTEAM_TOWER_CP					(39026)
#define MSC_SUB_FINGTTEAM_UPGRADE_TASK				(39027)
#define	MSC_DB_CS_INSPORE_UP						(39028)//��������
#define	MSC_DB_CS_READ_INSPORE						(39029)//��ȡ��������
#define	MSC_SUB_CHANGE_FTNAME						(39030)//�޸�ս������
#define	MSC_SUB_CLEAR_TIMEBOSS						(39031)//����ʱ��boss
#define	MSC_SUB_TIMEBOSS_INFO						(39032)//ʱ��boss��Ϣ
#define	MSC_SUB_TIMEBOSS_MUMBER						(39033)//ʱ��boss��Ա


//��Դ����
#define MSC_SUB_DBLOG_RES_PRODUCT_SAVE              (40000)  //��Դ����
#define MSC_SUB_DBLOG_RES_OCCUPY_SAVE               (40001)  //ռ������
#define MSC_SUB_DB_SYS_EXIT                         (40003)  //ϵͳ�˳������ݿ��߳�ʹ�ã�
#define MSC_SUB_DB_RES_ACTIVE						(40004)	 //��ȡ���Դ����Ϣ
#define MSC_SUB_DB_RES_SAVE                         (40005)  //������Դռ����Ϣ
#define MSC_SUB_SYS_EXIT                            (40006)  //ϵͳ�˳� ���߼��߳�ʹ�ã�

//Ԫ����
#define MSC_SUB_DB_TOWER_UPDATE						(41001) 
#define MSC_SUB_DB_TOWER_OFFLINE_DATA               (41002)  //��ȡ��������
#define MSC_SUB_DB_TOWER_SLOT_REWARD                (41003)  //�����Ľ���
#define MSC_SUB_DB_TOWER_SLOT_REWARD_DEL            (41004)  //�����Ľ���
#define MSC_SUB_DB_TOWER_MSG_ADD					(41005)
#define MSC_SUB_DB_TOWER_MSG_DEL					(41006)
#define MSC_SUB_DB_TOWER_FIGHT						(41007)
#define MSC_SUB_DB_TOWERSLOT_FIGHT					(41008)
#define MSC_SUB_DB_TOWER_EVENT						(41009)
#define MSC_SUB_DB_TOWER_HELP						(41010)

//����BOSS
#define MSC_SUB_DB_BOSS_TOPCLEAR					(41051)
#define MSC_SUB_DB_BOSS_TOPADD						(41052)
#define MSC_SUB_DB_BOSS_RECORDADD					(41053)

//ȫ������
#define MSC_SUB_DB_GLOBEDATA						(42000)	 //ȫ������
#define MSC_SUB_DB_GLOBEDATA_READ                   (42001)  //ȫ�����ݶ�ȡ�����ף�С��3
#define MSC_SUB_DB_UPDATA_DATA						(43000)

         
//���а�
#define MSC_SUB_DB_TOP_GET                          (43100)  //�õ�������Ϣ
#define MSC_SUB_DB_TOP_SAVE                         (43101)  //����������Ϣ
#define MSC_SUB_DB_TOP_REWARD						(43102)  //�������а���

#define MSC_SUB_TOP_MASTER_INFO                     (43200)  //�õ�������Ϣ
#define MSC_SUB_TOP_MASTER_FINISH                   (43201)  //������Ϣ�õ�
#define MSC_SUB_TOP_UPDATE                          (43202)  //���и���
#define MSC_SUB_TOP_FIGHTING						(43203)	//���а�ս��


//�㲥
#define MSC_SUB_BROADCAST_WORLD                     (43300) //��������
#define MSC_SUB_BROADCAST_SYSTEM                    (43301) //ϵͳ�㲥
#define MSC_SUB_BROADCAST_GM                        (43302) //gm�㲥


//����
#define  MSC_SUB_CHECK_OTHER_FINISH                 (43400) //�����鿴����û�����

#define  MSC_SUB_CHARGE_RESEND						(43401) //������ֵ����ʱ�ط����ж���


//�û�����
#define  MSC_SUB_ENTER_ACTIEVE_ADD                  (43500) //����
#define  MSC_SUB_ENTER_TRUNTABLE_ADD                (43501) //ת�����
#define  MSC_SUB_ENTER_WORSHIP_ADD                  (43502) //�������

//����boss
#define  MSC_SUB_WORLDBOSS_FINGHTING				(44001)	//����boss
#define  MSC_SUB_WORLDBOSS_LOGIN					(44002)	//����boss�û�����

//����
#define  MSC_SUB_TASK_FIGHTTEAM_EVENT				(44101)

//�û�����
#define  MSC_SUB_USER_DATA_INSERT					(45000)	//�û����ݲ���
#define  MSC_SUB_USER_SEEKDATA_INSERT				(45001) // �û���̽�����ݲ���

//���ս
#define  MSC_SUB_CS_ACTIVE_BEGIN					(46000)//���ս�
#define  MSC_SUB_CS_FIGHTING						(46001)//���ս��
#define	 MSC_SUB_CS_FINAL_EIGHT						(46002)//8ǿս��
#define  MSC_SUB_CS_FIGHT_RECORD					(46003)//�鿴ս��
#define  MSC_SUB_CS_ACTIVE_FIGHTING					(46004)//���ʼս��
#define  MSC_SUB_DB_CS_REWARD						(46005)//������¼
#define	 MSC_SUB_DB_CS_SUPPORT						(46006)//����֧��
#define	 MSC_SUB_DB_DELREWARD						(46007)//ɾ������

//����
#define  MSC_SUB_ISLAND_FIGHT						(47000)	//����ս���¼�
#define  MSC_SUB_DB_ISLAND_UPDATE					(47001) //��������
#define  MSC_SUB_DB_ISLAND_GIVEUID					(47002)	//���ʹ�����uid
#define  MSC_SUB_DB_ISLAND_SCOREREWARD				(47003) //�ѻ�õ�ս�ӻ��ֱ���
#define  MSC_SUB_DB_ISLAND_GIVETIMES				(47004) //�����߶������ʹ���
#define  MSC_SUB_GIVETIMES_OUTLINE					(47005) //�������ʹ���ʧ��
#define  MSC_SUB_UPDATE_FTMEMBER_SCORE				(47006) //����ս�ӳ�Ա�Ļ���
#define  MSC_SUB_OPEN_ISLAND						(47007) //��������
#define  MSC_SUB_DB_ISLAND_ONCLOCK					(47008)	//������ʱˢ��
#define  MSC_SUB_TASK_COMPLETE						(47009)	//�������

//�õ�ͷ���б�
#define  MSC_SUB_TITLELIST_GET						(48000) //���ʱ�õ�ͷ���б�
#define  MSC_SUB_TITLELIST_UPDATE					(48001)	//0��õ�ͷ���б�

//̽��
#define	 MSC_SUB_DB_WALL_ADDITION					(49001)	//��ǽ�ӳ�
#define	 MSC_SUB_DB_WALL_ARMY						(49002)	//��ǽ����
#define  MSC_SUB_DB_GET_OTHERSTOWN					(49003)	//�õ����˵ĳ�����Ϣ
#define  MSC_SUB_KIDNAP_FIGHT						(49004)	//���ս��
#define  MSC_SUB_RESCUE_FIGHT						(49005)	//Ӫ��ս��
#define  MSC_SUB_DB_GET_SEEKED						(49006) //�õ���������Ϣ
#define  MSC_SUB_GET_SEEKED_RETURN					(49007) //���ش���������Ϣ
#define  MSC_SUB_SEEKED_TIMES_MINUS					(49008) //�����ߵı����ʴ�������
#define  MSC_SUB_PLUNDER_FIGHT						(49009) //�Ӷ�ս��
#define  MSC_SUB_SEEK_TIMES_UPDATE					(49010)	//���·��ʴ���
#define  MSC_SUB_SEEKED_PLAYER_UPDATE				(49011) //��̽��״̬����
#define  MSC_SUB_SEEK_LIST_UPDATE					(49012) //̽���б����
#define  MSC_SUB_SEEKED_DATA_UPDATE					(49013) //̽���߻�����Ϣ����
#define  MSC_SUB_TEAM_FLAG_UPDATE					(49014) //��������
#define  MSC_SUB_SEEK_PRESENT_UPDATE				(49015) //����������Ϣ
#define  MSC_SUB_SEEK_DATA_UPDATE					(49016) //����̽���߻�����Ϣ
#define  MSC_SUB_TEAM_FLAG_DELETE					(49017) //ɾ������
#define  MSC_SUB_KIDNAP_UPDATE						(49018) //��������
#define  MSC_SUB_KIDNAP_DELETE						(49019) //ɾ������
#define  MSC_SUB_SEEK_PRESENT_DEL					(49020) //ɾ������
#define  MSC_SUB_SEEK_VISTORINFO					(49021) //�ÿͼ�¼
#define  MSC_SUB_SEEK_LOOK_FIGHTING					(49022) //�鿴ս��
#define  MSC_SUB_DB_SEEK_FIGHTING_RECORD			(49023) //ս����¼
#define  MSC_SUB_DB_PLUNDER_RES_UPDATE				(49024)	//�����Ӷ����Դ

#  pragma pack(push, 4)   /* ͨѶЭ��ṹ���붨���ڴ���֮�� */
//�û�������Ϣ
struct  MSC_DB_UserEnter
{
	CT_DWORD          dwUserID;        //�û�id
	CT_DWORD          dwDstUserID;     //Ŀ��ID
	CCNetInterface*   pNetInterface;   //�û�ͨѶ�ӿ�
	CT_DWORD          dwThreadID;      //�߳�id		
	CT_DWORD		  dwSubID;			//
};

//�û��˳�
struct MSC_DB_UserExit
{
	CT_DWORD          dwUserID;       //�û�id
	CT_INT64          llTimeNow;      //��ǰʱ��
};


//������Ϣ
struct MSC_DB_Challeng
{
	CT_DWORD		  dwUserID;
	CT_DWORD		  dwChalleng[PVP_CHALLENGE_COUNT];

};

//������Ϣ
struct MSC_DB_ChallengData
{
	CT_DWORD			dwUserID;
	UserFightingStatus  UserFightinginfo[PVP_CHALLENGE_COUNT];//�û�ս����Ϣ
	LookUserInfo		LookUserinfo[PVP_CHALLENGE_COUNT];	//�鿴�û���Ϣ
	
};

//ս����¼
struct  MSC_DB_PVPRecord
{
	CT_DWORD		dwUserID;
	CT_DWORD		dwChallengID[PVP_FIGHTING_COUNT];		
};



//�����������
struct MSC_DB_R_BuildUpgradeFinish 
{
	CT_DWORD            dwUserID;             //�û�id
	CT_INT32            iBID;                 //����id
	CT_INT32            iGrade;               //�����ȼ�
};


//����ͬ����Ϣ
struct MSC_DB_SysBuildingUpgrading
{
	CT_DWORD           dwUserID;            //�û�id
};

//���潨����Ϣ
struct MSC_DB_BuildingInfo
{
	CT_DWORD           dwUserID;             //�û�id
	CT_INT32           iBid;                 //����id
	CT_INT32           iGrade;               //�����ȼ�
};

//���潨������������Ϣ
struct MSC_DB_BuildingUpgradingInfo
{
	CT_DWORD            dwUserID;             //�û�id
	CT_INT32			iID;					//����id
	CT_LONGLONG			llBeginTime;			//��ʼ����ʱ��
	//CT_BOOL             bSpeedUpTime;           //���ٴ�����ֻ�п�����0��1��
	CT_INT32            iNeedTime;              //��Ҫ��ʱ��
	G_BuildingUpgrade   buildingUpgrade;        //�����������ݼ�¼
};







//db�û�������Ϣ
struct MSC_DB_R_BaseInfo
{
	CT_DWORD			dwUserID;         //�û�id
	CCNetInterface*		pNetInterface;    //�û���ͨѶ�ӿ�
};

//��Ӹ����Զ�ƥ��
struct MSC_DB_TeamDuplicateUserInfo
{
	CT_DWORD			dwUserID;         //�û�id
	UserFightingStatus  UserFightinginfo[TEAM_NUM-1];//�û�ս����Ϣ
	LookUserInfo		LookUserinfo[TEAM_NUM-1];	//�鿴�û���Ϣ
};

//db����������Ϣ
struct MSC_DB_R_BuildingBaseInfo : public MSC_DB_R_BaseInfo
{
	CT_INT32            iBuildID;
};


//�����û�����
struct MSC_DB_MasterSaveData
{
	CT_DWORD             dwUserID;             //�û�id
	// ��Ӧ���ݽṹ�޸� [9/11/2012 John]
	MasterVariableData mvd;
   // CT_INT32           iBagSlots;
	CT_INT64           llExitTime;          //�˳�ʱ��
	CT_INT32           iSaveTag;            //����ԭ��
};

//�����û�ս������
typedef MasterFightAdd MSC_DB_MasterFightSaveData;
//struct MSC_DB_MasterFightSaveData
//{
//	CT_DWORD				dwUserID;             //�û�id
//	CT_INT32				iType;					//ְҵ
//	MasterCombatStatus		mcs;
//};

struct MSC_DB_MasterSaveDataEx
{
	CT_DWORD            dwUserID;             //�û�id
	CT_INT32			iSaveTagEx;
	CT_INT64			iInt64Value;       //
	CT_INT32			iInt32Value;       //

};

//�޸���������
struct MSC_DB_MasterName
{
	CT_DWORD			dwUserID;			//�û�Id
	CT_CHAR				cName[NAME_SIZE+1];	//����
	CT_INT32			iPropID;			//����id
};

//�޸�ս������
typedef MSC_DB_MasterName MSC_DB_FTName ;

//������������
struct MSC_DB_SaveProductingData
{
	CT_DWORD            dwUserID;           //�û�id
	ProductingBuilding  productingBuilding; //������Ϣ
};


//ͬ�������Ľ���
struct MSC_DB_SysProductingData
{
      CT_DWORD            dwUserID;           //�û�id
};


//��ȡ��������
//struct MSC_DB_ReadProductingData
//{
//	CT_DWORD            dwUserID;         //�û�id
//	ProductingBuilding  productingBuilding; //������Ϣ
//};

// Begin [9/20/2012 John]
// ��ȡǱ����Ϣ
//struct MSC_DB_R_TalentData
//{
//    CT_DWORD			dwUserID;          //�û�id
//    TalentData			Talent;            // Ǳ����Ϣ
//};
// ��ȡ������Ϣ
//struct MSC_DB_R_SkillData
//{
//    CT_DWORD			dwUserID;          //�û�id
//    SkillData			Skill;             // ������Ϣ
//};
// ��ȡ������Ϣ
//struct MSC_DB_R_ItemData
//{
//    CT_DWORD			dwUserID;
//    ItemData			item;
//};
// ��ȡװ����Ϣ
//struct MSC_DB_R_EquipData
//{
//    CT_DWORD            dwUserID;
//    EquipmentData       equip;
//};
// ��ȡ������Ϣ
//struct MSC_DB_R_BuffData
//{
//    CT_DWORD dwUserID;          // �û�ID
//    BuffData buff;
//};
// ��ȡ��ȴ��Ϣ
//struct MSC_DB_R_CooldownData
//{
//    CT_DWORD     dwUserID;      // �û�ID
//    CooldownData cooldown;
//};
// ������ȴ��Ϣ
struct MSC_DB_W_CooldownData
{
    CT_DWORD     dwUserID;      // �û�ID
    CT_INT32     iType;         // ��ȴ����
    CT_INT64     llLeftTime;    // ʣ��ʱ��
};
// ����Ǳ����Ϣ
struct MSC_DB_W_TalentData
{
    CT_DWORD			dwUserID;          // �û�ID
    CT_INT32			iTalentID;         // Ǳ��ID
    CT_CHAR				szName[NAME_SIZE]; // Ǳ������
    //CT_INT32			iExp;              // ���ĵ�Ǳ�ܵ���
};

// ���漼����Ϣ
struct MSC_DB_W_SkillData
{
    CT_DWORD			dwUserID;          // �û�ID
    CT_INT32			iSkillID;          // ����ID
    CT_CHAR				szName[NAME_SIZE]; // ��������
    CT_INT32			iGradeInc;         // �ȼ�����ֵ
    //CT_INT32			iExp;              // ���ĵļ��� ����ɾ��
    CT_INT32			Used;              // �Ƿ�װ��
};
// ���������Ϣ
struct MSC_DB_W_ItemData
{
    CT_DWORD            dwThread;          // �߳�ID
    CT_DWORD 			dwUserID;          // �û�ID
    CT_INT32            key;               // Ψһ��ʶ
    CT_INT32 			iItemID;           // ����ID
    CT_CHAR  			szName[NAME_SIZE]; // ��������
    CT_INT32 			iCount;            // ��������
    CT_INT32 			iIndex;            // �������ڲ�����ֵ               
};
// ����������Ϣ
struct MSC_DB_W_BuffData
{
    CT_DWORD            dwThread;          // �߳�ID
    CT_DWORD            dwUserID;          // �û�ID
    CT_INT32            key;               // ���ߵ�Ψһ��ʶ
    CT_INT32            iGroup;            // ��ID
    CT_INT32            iBuffID;           // ����ID
    CT_INT64            toggleTime;        // ���ʱ��
    CT_INT32            iDuration;         // ����ʱ��
};

// ������ʧ
struct MSC_DB_LoseBuffData
{
    CT_DWORD            dwUserID;             //�û�id
    CT_DWORD            dwThread;             //�߳�id
    CT_INT32            iGroup;               // ��ID
    CT_INT32            iBuffID;              // ����ID
};

// ͬ����������
struct MSC_DB_SyncItemData
{
    CT_DWORD            dwUserID;
    CT_DWORD            dwThread;
    ItemData            stItems[ITEM_SLOT_COUNT];
};
// ͬ��װ������
struct MSC_DB_SyncEquipData
{
    CT_DWORD            dwUserID;
    CT_DWORD            dwThread;
    EquipmentData       stEquips[ITEM_SLOT_COUNT];
};
// ͬ����������
struct MSC_DB_SyncBuffData
{
    CT_DWORD            dwUserID;
    CT_DWORD            dwThread;
    BuffData            stBuffs[BUFF_SLOTS];
};
// ͬ��Ԫ������
struct MSC_DB_SyncElmAttrData
{
    CT_DWORD            dwUserID;
    CT_DWORD            dwThread;
    ElmMarrowAttr       stDatas[ITEM_SLOT_COUNT];
};
// ����ս����Ϣ
struct MSC_DB_SaveBattleFlag
{
    CT_DWORD            dwUserID;
    CT_DWORD            dwThread;
    CT_INT32            iType;
    CT_INT32            iAdd;
};

// ���߽���
struct MSC_DB_W_SwapItemData
{
    CT_DWORD            dwThread;          // �߳�ID
    CT_DWORD            dwUserID;          // �û�ID
    struct tagItemPair
    {
        CT_INT32        key;               // Ψһ��ʶ
        CT_INT32 		iItemID;           // ����ID
        CT_CHAR  		szName[NAME_SIZE]; // ��������
        CT_INT32 		iCount;            // ��������
        CT_INT32 		iIndex;            // �������ڲ�����ֵ
    }Pair[2];
};
// ����װ����Ϣ
struct MSC_DB_W_EquipData
{
    CT_DWORD            dwThread;          // �߳�ID
    CT_DWORD 			dwUserID;          // �û�ID
    CT_DWORD            key;               // Ψһ��ʶ
    CT_INT32            iIndex;            // װ������Ʒ����λ��
    CT_INT32            iLevel;            // ǿ���ȼ�
    CT_INT32            nSlot;             // �������
    CT_INT32            Gem[GEM_COUNTS];   // ��ʯ��Ϣ
    CMD_G_Forge         Forge[FORGE_ATTRS];
};
// ǿ���Ĳ��
struct MSC_DB_W_SmithingAndSlot
{
    CT_DWORD            dwThread;          // �߳�ID
    CT_DWORD 			dwUserID;          // �û�ID
    CT_DWORD            key;               // Ψһ��ʶ
    CT_INT32            iEquipID;          // ����ID
    CT_INT32            iLevel;            // ǿ���ȼ��仯ֵ
    CT_INT32            nSlot;             // ��������仯ֵ    
    CT_INT32            iDiamond;          // ������ʯ��
    CT_INT32            iGold;             // ���Ľ����
    CT_INT32            iItemID;           // ���ĵĵ���ID
    CT_INT32            iItemNum;          // ���ĵĵ�������
};

// ϴ��
struct MSC_DB_W_ForgeData
{
    CT_DWORD            dwThread;          // �߳�ID
    CT_DWORD 			dwUserID;          // �û�ID
    CT_DWORD            key;               // Ψһ��ʶ
    CT_INT32            iEquipID;          // ����ID
    CMD_G_Forge         Forge[FORGE_ATTRS];// ϴ����Ϣ
    CT_INT32            iDiamond;          // ������ʯ��
    CT_INT32            iGold;             // ���Ľ����
    CT_INT32            iItemID;           // ���ĵĵ���ID
    CT_INT32            iItemNum;          // ���ĵĵ�������
};
// ��Ƕ
struct MSC_DB_W_EmbedData
{
    CT_DWORD            dwThread;          // �߳�ID
    CT_DWORD 			dwUserID;          // �û�ID
    CT_DWORD            key;               // Ψһ��ʶ
    CT_INT32            iGemID;            // ��ʯ��Ϣ
    CT_INT32            iSlot;             // ������    
};
// �̳�
struct MSC_DB_W_InheritData
{
    CT_DWORD            dwThread;          // �߳�ID
    CT_DWORD 			dwUserID;          // �û�ID
    CT_DWORD            keySrc;            // ���̳е�װ��Ψһ��ʶ
    CT_INT32            iSrcID;            // ���̳е�װ��ID
    CT_INT32            iLvSrc;            // ���̳�ʱ��װ���ȼ� 
    CT_DWORD            keyDst;            // �̳е�װ��Ψһ��ʶ
    CT_INT32            iDstID;            // �̳е�װ��ID
    CT_INT32            iDiamond;          // ������ʯ��
    CT_INT32            iGold;             // ���Ľ����
    CT_INT32            iItemID;           // ���ĵĵ���ID
    CT_INT32            iItemNum;          // ���ĵĵ�������
};

// ���±�������
struct MSC_DB_W_BagSlot
{
    CT_DWORD            dwThread;          // �߳�ID
    CT_DWORD 			dwUserID;          // �û�ID
    CT_INT32            nSlot;             // �����ĸ�����
   // CT_INT32            iDiamond;          // ���ĵ���ʯ
};
// End [9/20/2012 John]


// Begin [9/20/2012 wangchen]
// ����ս������
struct MSC_DB_W_FightingReward
{
	CT_INT32					iExperience;		//����
	CT_INT32					iGold;				//���
	CT_INT32 				iDiamond;			//��ʯ
	CT_INT32					iPotential;			//Ǳ����
	CT_INT32					iPrestige;			//����
	CT_INT32					iMobility;			//�ж���
};
// End [9/20/2012 wangchen]

//�����¼�����
struct MSC_DB_EventAward
{
	CT_DWORD 			dwUserID;       	//�û�id
	CT_INT32 			iGold;				//���
	CT_INT32 			iFood;				//ʳ��
	CT_INT32 			iCiviliNum;     	//������
	CT_CHAR             cProp1Name[NAME_SIZE];       //����1����
	CT_INT32            iProp1;     		//����1����
	CT_INT32            iPropNum1;  		//����1����
	CT_CHAR             cProp2Name[NAME_SIZE];       //����2����  
	CT_INT32            iProp2;     		//����2����
	CT_INT32            iPropNum2;  		//����2���� 
};
//////////////////////////////////////////////////////////////////////////
struct MSC_USR_AddItem
{
    CT_DWORD    dwUserID;   // �û�ID
    CT_INT32    iItemID;    // ����ID
    CT_INT32    iCount;     // ��������
};

//ɾ������
struct MSC_USR_DeleteItem
{
	CT_DWORD    dwUserID;   // �û�ID
	CT_INT32    iItemID;    // ����ID
	CT_INT32    iCount;     // ��������
};

//�޸���������
struct MSC_USR_ChangeMasterName
{
	CT_DWORD	dwUserID;
	CT_INT32	iPropID;
	CT_INT32	iStatus;
	CT_CHAR		cName[NAME_SIZE];
};

// Begin [10/20/2012 wangchen]
//���糡����Ϣ
struct MSC_DB_WorldSceneInfo
{
	CT_DWORD			dwUserID;			//�û�id
	CT_INT32			iSceneID;			//����ID
	CT_INT32			iReward;			//��������
};

//������Ϣ
struct MSC_DB_SceneInfo
{
	CT_INT32		iWorldSceneID;						//���糡��ID
	CT_INT32		iSceneID;							//����ID
	CT_DWORD		dwUserID;							//�û�ID
	CT_INT32		iSceneStar;							//�����Ǽ�
	CT_INT32		iFightingNum;						//ս������
	CT_INT32		iDifficulty;						//�Ѷ�
};

//���¹���ս������
struct  MSC_DB_BuyFightingTime
{
	CT_DWORD		dwUserID;							//�û�id
	CT_INT32		iBuyFightingTime;					//����ս������
};


//����ս��
struct MSC_Fighting_Info
{
	CT_DWORD		dwUserID;							//�����û�ID
	CT_INT32		iWorldSceneID;						//���糡��ID
	CT_INT32		iSceneID;							//����ID
	CT_INT32		iDifficulty;						//�Ѷ�
	CT_INT32		iSceneStar;							//�����Ǽ�
	CT_INT32		iSeed;								//�������

	UserFightingStatus	FightingData;					//ս������
};

//��Դ�Ӷ�
struct MSC_TeamPVP_ResRobbery
{
	CT_DWORD		dwUserID;							//�����û�ID
	CT_INT32		iSeed;								//�������
	CT_INT32        iWin;                               //�����Ƿ�Ӯ��

	CT_INT32		iFood;								//���ĵ�ʳ��    
	CT_INT32		iScenceID;							//����id
	CT_INT32		iSpotID;							//��Դ��id   
	CT_DWORD		dwTeamID;							//ս��id  
	CT_BOOL			bLookLeft;

	UserFightingStatus	LeftFightingData[TEAM_NUM];					//ս������
	UserFightingStatus	RightFightingData[TEAM_NUM];					//ս������
};

//��Դ�Ӷ�
struct MSC_TeamPVE_ResRobbery
{
	CT_DWORD		dwUserID;							//�����û�ID
	CT_INT32		iSeed;								//�������
	CT_INT32        iResSpotQuality;                    //��Դ��Ʒ�� 
	CT_INT32        iWin;                               //�����Ƿ�Ӯ��


	CT_INT32		iFood;								//���ĵ�ʳ��    
	CT_INT32		iScenceID;							//����id
	CT_INT32		iSpotID;							//��Դ��id   
	CT_DWORD		dwTeamID;							//ս��id
	
	UserFightingStatus FightingData[SPOT_GUARD_MEMBER];					//ս������
};

//Ԫ��֮��ռ��
struct MSC_TowerPVP_ResRobbery
{
	CT_DWORD		dwUserID;							//�����û�ID
	CT_INT32		iSeed;								//�������
	CT_INT32        iWin;                               //�����Ƿ�Ӯ��
	CT_BOOL			bLookLeft;							//�鿴��?

	CT_INT32        iTowerID;                           //����id
	CT_INT32        iFloorID;                           //��id
	CT_INT32        iSlotID;                            //��Դ��id
	CT_INT32        iOccupyTime;                        //ռ��ʱ��
	UserFightingStatus	LeftFightingData;				//��ս��ս������
	UserFightingStatus	RightFightingData;              //����ս����ս������
	CMD_G_NormalReward  towerProductCost;             //��ս������
	CT_INT64		iFID;
};


//Ԫ��֮��������ս������
struct MSC_TowerOfflineUserFightData
{
	CT_DWORD                 dwUserID;                   //�û�id
	CT_DWORD                 dwOfflineUserID;            //�����û�id
	UserFightingStatus		 offlineFightingData;       //�����û�ս������
	MSC_TowerPVP_ResRobbery  towerPVPResRobbery;        //Ԫ��֮��ռ������
};

//��Ӹ���
struct MSC_TeamDuplicate_Fighting
{
	CT_DWORD		dwUserID;							//�����û�ID
	CT_INT32		iSceneID;							//����ID
	CT_INT32		iDifficulty;						//�Ѷ�
	CT_INT32		iSeed;								//�������
	CT_INT32		iWiner;								//ʤ����
	//CT_INT32        iWin;                               //�����Ƿ�Ӯ��
	UserFightingStatus FightingData[SPOT_GUARD_MEMBER];		    //ս������
};

//Ԫ��֮���ؿ�ͨ��
struct MSC_TowerCheckPoint_Fighting
{
	CT_DWORD		dwUserID;							//�û�ID
	CT_INT32		iCPID;								//�ؿ�ID
	CT_INT32		iSeed;								//�������
	CT_INT32		iScore;								//�ɼ�

	UserFightingStatus	FightingData;					//ս������
};

//��ս����
struct MSC_Look_Fighting
{
	MSC_Fighting_Info				FightingInfo;		//ս������
	CT_DWORD						dwUserID;			//�û�ID
};

//������ս����¼
struct	MSC_DB_W_PVPFightingInfo
{
	CT_INT32		iDifficulty;
	PVPFightingInfo	PVPFightinginfo;		//ս����¼
};

//�������ս�
struct MSC_DB_PVPEnding
{
	CT_DWORD		dwUserID;				//�û�id
};

//ս������
struct MSC_Fighting_End
{
	CT_INT32		iWorldSceneID;						//���糡��ID
	CT_INT32		iSceneID;							//����ID
	CT_INT32		iUser;								//�û�����
	CT_INT32		iSceneStar;							//�Ǽ�
	CT_INT32		iSeed;								//�������
	CT_INT32		iUserPos;							//�û�λ��
	CT_INT32		iArmy1ID;							//����ID
	CT_INT32		iArmy1Pos;							//����λ��
	CT_INT32		iArmy1Grade;						//���ӵȼ�
	CT_INT32		iArmy2ID;							//����ID
	CT_INT32		iArmy2Pos;							//����λ��
	CT_INT32		iArmy2Grade;						//���ӵȼ�
	CT_INT32		iSkill1ID;							//�û�����
	CT_INT32		iSkill2ID;							//�û�����
	CT_INT32		iSkill3ID;							//�û�����
};

//ս������
struct MSC_Event_Fighting_End
{
	CT_DWORD		dwUserID;							//�����û�ID
	CT_INT32		iIndex;								//�¼�����
	CT_INT32		iAwardID;							//�¼�ID
	CT_INT32		iWiner;								//ʤ��

};

struct MSC_PVEFighting_Reward
{
	CT_DWORD		dwUserID;
	CT_INT32		iWorldSceneID;
	CT_INT32		iSceneID;
	CT_INT32		iDifficulty;
	CT_INT32		iSceneStar;
};

//����������
struct MSC_Duplicate_Finish
{
	CT_DWORD		dwUserID;							//�û�ID
};

//ɨ����Ϣ
struct MSC_DB_RaidsInfo
{
	CT_DWORD		dwUserID;							//�û�id
	CT_INT32		iWorldID;							//���糡��
	CT_INT32		iSceneID;							//����ID
	CT_INT32		iDifficulty;						//�Ѷ�
	CT_INT32		iAllTime;							//�ܴ���
	CT_INT32		iCurrentTime;						//��ǰ����
	CT_INT64		lBeginTime;							//��ʼʱ��

	CT_INT32		iLastStamina;						//ʣ������
};

//ɨ������
struct MSC_DB_RaidsReward
{
	CT_DWORD		dwUserID;							//�û�ID
	CT_INT32		iExperience;						//����
	CT_INT32		iGold;								//���
	CT_INT32		iDiamond;							//��ʯ
	CT_INT32		iPotential;							//Ǳ����
	CT_INT32		iSkillPoint;						//���ܵ�ǿ����
	CT_INT32		iGoods[GOODS_COUNT];				//װ��ID
	CT_INT32		iGoodsCount[GOODS_COUNT];			//��Ŀ
	CT_INT32		iCurrentTime;						//��ǰ����
	CT_INT64		lTime;								//ʱ��

};

//��Ӹ�������
struct MSC_DB_TeamDuplicateScene
{
	CT_DWORD		dwUserID;							//��Ӹ���
	CT_INT32		iScnenID;							//����ID
	CT_INT32		iDifficulty;						//�Ѷ�
	CT_INT32		iWin;								//�Ƿ�ʤ��
};

//��Ӹ����Զ�ƥ��
struct MSC_DB_TeamDuplicateAutoMatch
{
	CT_DWORD		dwUserID;							//��Ӹ���
	CT_DWORD		dwDstUserID[TEAM_NUM];				//�û�ID
	CT_INT32		iFightingPower;						//ս����
	CT_INT32		iGrade;								//�ȼ�
};

//��Ӹ����Զ�ƥ��
struct MSC_DB_TeamDuplicateAutoMatchInfo
{
	CT_DWORD		dwUserID;							//�û�id
	CT_DWORD		dwDstUserID[TEAM_NUM-1];	//��ȡ�û�
};

//���㽱��
struct MSC_RaidsReward
{
	CT_DWORD		dwUserID;							//�û�id
};
// End [10/20/2012 wangchen]
//���������Ϣ
struct MSC_DB_W_RegionUnlockData
{
	CT_DWORD	dwUserID;	//�û�ID
	CT_INT32	iRegionID;	//����ID
	CT_INT32	iGold;		//���Ľ��
	CT_INT32	iDiamod;	//������ʯ
};

//��ȡ�û���������id
struct MSC_DB_R_UnlockRegion
{
	CT_DWORD	dwUserID;	//�û�id
	CT_INT32	iRegionID;	//����id
};


// Begin [11/8/2012 wangchen]
//��ȡ������Ϣ
struct MSC_DB_GetArmyInfo
{
	CT_DWORD	dwUserID;		//�û�ID
};

//��������
struct MSC_DB_R_ArmyGrade
{
	CT_DWORD	dwUserID;		//�û�ID
	CT_INT32	iArmyID;		//����ID

	//CT_INT32	iGold;			//���Ľ��
	//CT_INT32	iFood;			//����ʳ��
	//CT_INT32	iGoods;			//������Ʒ
	//CT_INT32	iCount;			//��������
};

//���ӽ���
struct MSC_DB_R_ArmyAdvanced
{
	CT_DWORD	dwUserID;		//�û�ID
	CT_INT32	iOldArmyID;		//����ID
	CT_INT32	iNewArmyID;		//����ID
	CT_INT32	iArmyGrade;		//���ӵȼ�

	//CT_INT32	iGold;			//���Ľ��  //����ɾ��
	//CT_INT32	iGoods;			//������Ʒ
	//CT_INT32	iCount;			//��������
};

//������Ϣ
struct MSC_DB_ArmyInfo
{
	CT_DWORD	dwUserID;		//�û�ID
	CT_INT32	iArmyGrade;		//���ӵȼ�
	CT_INT32	iArmyID;		//����ID
};

//����Ϣ
struct MSC_DB_BattleArray
{
	CT_DWORD	dwUserID;				//�û�ID
	CT_INT32	iPosition[POST_COUNT/2];//λ��
};

// End [11/8/2012 wangchen]
//��ȡ�û����������
struct MSC_DB_TaskFinishData
{
	CT_DWORD	dwUserID;	//�û�id
	CT_INT32	iTaskID;	//����id
};

//��ȡ�û�����������Ϣ
struct MSC_DB_TaskProgressData
{
	CT_DWORD	dwUserID;	//�û�id
	CT_INT32	iTaskID;	//����id
	CT_INT32	iSubTaskID;	//������id
	CT_INT32	iTime;		//��ɴ���
	CT_INT32	iCompleted;	//�Ƿ����
};

//////////////////////////////////////////////////////////////////////////
// ��ȡ���ճ�������Ϣ [11/14/2012 John]
//struct MSC_DB_R_DailyQuestInfo
//{
//    CT_DWORD	dwUserID;
//    DailyQuestData daily;
//};
// ��ȡ��ǩ����Ϣ [11/14/2012 John]
//struct MSC_DB_R_SignInfo
//{
//    CT_DWORD	dwUserID;
//    SignData    sign;
//};
// ��ȡ�Ļ�Ծ����Ϣ [11/14/2012 John]
//struct MSC_DB_R_ActiveInfo
//{
//    CT_DWORD	dwUserID;
//    ActiveData  active;
//};

// ���µ�ǩ����Ϣ [11/14/2012 John]
struct MSC_DB_W_SignInfo
{
    CT_DWORD    dwUserID;   // �û�id
    CT_INT32    nSigned;    // ǩ������
    CT_INT32    bReward;    // �Ƿ���ȡ
};
// �ճ����������Ϣ [11/14/2012 John]
struct MSC_DB_W_DailyQuest
{
    CT_DWORD    dwUserID;   // �û�id
    CT_INT32    iQuestID;   // ����id
    CT_INT32    iCurrent;   // ��ǰ����
};
// ��Ծ����Ϣ [11/14/2012 John]
struct MSC_DB_W_ActiveInfo
{
    CT_DWORD    dwUserID;   // �û�id
    CT_INT32    iLevel;     // ��Ծ�ȵȼ�
    CT_INT32    iActive;    // ��Ծ����ֵ
};

struct MSC_DB_W_ActiveReward
{
    CT_DWORD    dwUserID;   // �û�id
    CT_INT32    iLevel;     // ��Ծ�ȵȼ�
};

// ���������½��¼ [11/15/2012 John]
struct MSC_DB_D_SignInfo
{
    CT_DWORD    dwUserID;   // �û�ID
};
// ����ճ���¼ [11/15/2012 John]
struct MSC_DB_D_DailyInfo
{
    CT_DWORD    flag;       // �޲���..
};

//��������Ϣ
struct MSC_DB_W_PVPInfo
{
	CT_DWORD		dwUserID;								//�û�id
	CT_BYTE			cbChestStatus;							//����״̬
	CT_INT32		iFightingCount;							//����ս����
	CT_INT64		lRefreshTime;							//ˢ��ʱ��
	CT_INT32		iPVPDegree;								//������
	CT_INT32		iPVPChestNum;							//��ȡ�������
};

//��������Ϣ
struct MSC_DB_PVPInfo
{
	CT_DWORD		dwUserID;								//�û�Id
	CT_DWORD		dwChallengUserID[PVP_CHALLENGE_COUNT];	//��ս�û�
	CT_BYTE			cbChestStatus;							//����״̬
	CT_INT32		iFightingCount;							//����ս����
	CT_INT64		lRefreshTime;							//ˢ��ʱ��
	CT_BYTE			cbChallengStatus[PVP_CHALLENGE_COUNT];	//��ս״̬
	CT_INT32		iDegree;								//������
	CT_INT32		iChestNum;								//��ȡ�������
};

//pvp����������ݵ����ݿ�
struct MSC_DB_PVPChallgerSave
{
	CT_DWORD		dwUserID;								//�û�id
	PVPInfo         pvpInfo;							    //pvp����
};


//ˢ�¶���
struct	MSC_DB_Refresh
{
	CT_DWORD		dwUserID;						//�û�id
};


//��ȡս����¼
struct	MSC_DB_R_PVPFightingRecord
{
	CT_DWORD		dwUserID;			//�û�Id
	CT_INT32		iIndex;				//����
};

//��ȡս����¼
struct	MSC_DB_FighitngRecord
{
	CT_DWORD		dwUserID;			//�û�Id	
	PVPFightingInfo	PVPFightinginfo;		//ս����¼
};

//��ȡ�û���Ϣ
struct	MSC_DB_R_UserInfo
{
	CT_DWORD		dwUserID;			//�û�ID
};

//�û���Ϣ
struct	MSC_DB_UserInfo
{
	CT_DWORD		dwUserID;			//�û�id
};

//��ȡս����Ϣ
struct	MSC_DB_R_FightingInfo
{
	CT_DWORD		dwUserID;			//�û�Id
	CT_INT32		iPVPFightingIndex;	//ս������
};

//ս����Ϣ
struct	MSC_DB_FighitngInfo
{
	CT_DWORD				dwUserID;							//�����û�ID
	CT_INT32				iUserGrade;							//�û��ȼ�
	CT_CHAR					cUserName[NAME_SIZE];				//�û�����
	CT_INT32				iSceneStar;							//�û��Ǽ�
	CT_INT32				iCivilGrade;						//�����ȼ�
	CT_INT32				iProfession;						//�û�ְҵ
	MasterCombatStatus		FightingInfo;						//ս����Ϣ
	CT_INT32				iUserPos;							//�û�λ��
	CT_INT32				iSkill1ID;							//�û�����
	CT_INT32				iSkill2ID;							//�û�����
	CT_INT32				iSkill3ID;							//�û�����
	CT_INT32				iWorldSceneID;						//���糡��ID
	CT_INT32				iSceneID;							//����ID
	CT_INT32				iSeed;								//�������
	CT_INT32				iArmy1ID;							//����ID
	CT_INT32				iArmy1Pos;							//����λ��
	CT_INT32				iArmy1Grade;						//���ӵȼ�
	CT_INT32				iArmy2ID;							//����ID
	CT_INT32				iArmy2Pos;							//����λ��
	CT_INT32				iArmy2Grade;						//���ӵȼ�
};

//��ս״��
struct MSC_DB_ChallengeInfo
{
	CT_DWORD		dwUserID;								//�û�ID
	CT_INT32		iDifficulty;							//��ս�Ѷ�
	CT_BOOL			bWiner[PVP_CHALLENGE_COUNT/2];			//�Ƿ�սʤ
	CT_DWORD		dwDstUserID[PVP_CHALLENGE_COUNT/2];		//Ŀ��
};

//��ȡ���а�
struct	MSC_DB_R_RangkingInfo
{
	CT_DWORD		dwUserID;					//�û�id
};
//���а�
struct	MSC_DB_RankingInfo
{
	CT_DWORD		dwUserID;					//�û�id
	LookUserInfo		Ranking[TOP_COUNT];	//���а���Ϣ
};

//////////////////////////////////////////////////////////////////////
//�̵����ݿ�ṹ
////�����¼
//struct MSC_DB_ShoppingRecord
//{
//	CT_DWORD	dwUserID;			//�û�id
//	CT_INT32	iGoodsID;			//��Ʒid
//	CT_INT32	iGoodsNum;			//��Ʒ����
//	CT_INT32	iDiamod;			//������ʯ
//	CT_INT32	iGold;				//���ѽ��
//	CT_INT32	iItemID;			//���ѵ���
//	CT_INT32	iItemNum;			//��������
//	CT_INT32	iMarketType;		//�̵�����
//};

//�ڵ���Ʒˢ��
struct MSC_DB_BlackMarketRefresh
{
	CT_DWORD	dwUserID;			//�û�id
	CT_INT32	iTime;				//ʣ��ˢ�´���
	CT_INT32	iGoodsID1;			//����id
	CT_INT32	iNum1;				//��������
	CT_INT32	iGoodsID2;			//����id
	CT_INT32	iNum2;				//��������
	CT_INT32	iGoodsID3;			//����id
	CT_INT32	iNum3;				//��������
	CT_INT32	iGoodsID4;			//����id
	CT_INT32	iNum4;				//��������
	CT_INT32	iGoodsID5;			//����id
	CT_INT32	iNum5;				//��������
	CT_INT32	iGoodsID6;			//����id
	CT_INT32	iNum6;				//��������
	CT_INT64	iLastRefTime;		//���ˢ��ʱ��
};

//�ڵ깺����Ʒ
struct MSC_DB_BlackMarketShopping
{
	CT_DWORD	dwUserID;			//�û�id
//	CT_INT32	iTime;				//ˢ�´���	//zhoujun ����ʱ����Ҫ����ˢ�´���
	CT_INT32	iNumList[MARKET_BLACKNUM_SYS+MARKET_BLACKNUM_PAY];			//�����������
};

//�̵깦��״̬
struct MSC_DB_MarketFunction
{
	CT_DWORD	dwUserID;			//�û�id
	CT_INT32	iMall;				//�̳�״̬
	CT_INT32	iBlackMarket;		//�ڵ�״̬
};

//�̵�����޹�
struct MSC_DB_MarketPersonal
{
	CT_DWORD	dwUserID;			//�û�
	CT_INT32	iGoodsID;			//��Ʒid
	CT_INT32	iNum;				//�������
};


////���������¼
//struct MSC_DB_W_BuyStatmina
//{
//	CT_DWORD	dwUserID;			//�û�ID
//	CT_INT32	iBuyNum;			//�������
//	CT_INT32	iDiamod;			//������ʯ
//};
////////////////////////////////////////////////////
//������Ϣ
struct MSC_DB_PetBornInfo
{
	CT_DWORD	dwUserID;
	PetInfo		pi;
};

//����û�
struct MSC_DB_PetChange
{
	CT_DWORD	dwUserID;
	CT_INT32	iChangeID;
	CT_INT32	iSkillID;
};

//���＼��
struct MSC_DB_PetSkill
{
	CT_DWORD	dwUserID;
	CT_INT32	iSkillID[PET_SKILL_COUNT];
};

//���ﵰʹ��
struct MSC_UsePetEgg
{
	CT_DWORD	dwUserID;
	CT_INT32	iItemID;
};

///////////////////////////////////////////////////////////////
//�ʼ���Ϣ
struct MSC_DB_EmailUserResult
{
	CT_DWORD		dwUserID;			//�û�id
	CT_INT32		iRet;				//�����ж��ʼ��Ƿ��ͳɹ�
};

////////////////////////////////////////////////////////////////////
//vip��Ϣ
struct MSC_DB_VipGiftGet
{
	CT_DWORD		dwUserID;			//�û�id
	CT_INT32		iGiftGet;			//����Ƿ���ȡ
};

struct MSC_DB_VipApply
{
	CT_DWORD		dwUserID;			//�û�id
	CT_INT32		iGrade;				//vip�ȼ�
	CT_INT64		llTime;				//d����ʱ��
	//CT_INT32		iPropID;			//����id      //����ɾ��
	//CT_INT32		iPropNum;			//��������
	//CT_INT32		iDiamod;			//��ʯ
	//CT_INT32		iDays;				//��ͨ����
	//CT_INT32		iGradeOpen;			//��ͨ�ȼ�
	CT_INT32		iGift;				//����Ƿ���ȡ
};

struct MSC_DB_VipInfo
{

};

// ��ӳ�ֵ��¼
struct MSC_DB_A_Charge
{
    CT_INT32 dwUserID;      // �û�ID
    CT_INT32 iPrice;          // �۸�
    CT_INT32 iDiamond;      // ����
    CT_INT32 iState;        // ����״̬��0���ɹ�����1���ύ��
    CT_CHAR szSign[EXORDER_NO_LEN + 1];   // ����ǩ��
};

// ���³�ֵ��¼��Ϣ
struct MSC_DB_U_Charge
{
    CT_CHAR szSign[EXORDER_NO_LEN + 1];   // ����ǩ��
    CT_INT32 iIndex;                // �������
    CT_INT32 iState;                // ����״̬
};

/* װ����ħ */
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
    //CT_INT32 iBID;               // ��λ
    //CT_INT32 iIDX;               // ���
    //CT_INT32 iGradeAdd;          // ��ħ�ȼ��仯��(0: ����; 1:����)
};

struct MSC_DB_EnchantFailTime
{
	CT_INT32 dwUserID;
	CT_INT32 iBID;				// ��λ
	CT_INT32 iIDX;				// ���
	CT_INT32 iFailCount;		// ʧ�ܴ���
};

struct MSC_DB_EnchantReward 
{
	CT_INT32 dwUserID;
	CT_INT32 iIndex;			// ��������
};

//��־���ݿ�
////////////////////////////////////////////////////////////
//��������
struct MSC_LOGDB_W_ArmyGrade
{
	CT_DWORD	dwUserID;		//�û�ID
	CT_DWORD    iUserGarde;     //�û��ȼ�
	CT_INT32	iArmyID;		//����ID
	CT_INT32	iGrade;			//�ȼ�

	CT_INT32	iGold;			//���Ľ��
	CT_INT32	iFood;			//����ʳ��
	CT_INT32	iGoods;			//������Ʒ
	CT_INT32	iCount;			//��������
};

//���ӽ���
struct MSC_LOGDB_ArmyAdvanced
{
	CT_DWORD	dwUserID;		//�û�ID
	CT_INT32	iOldArmyID;		//����ID
	CT_INT32	iNewArmyID;		//����ID
	CT_INT32	iArmyGrade;		//���ӵȼ�
	CT_INT32	iGold;			//���Ľ��
	CT_INT32	iGoods[2];			//������Ʒ
	CT_INT32	iCount[2];			//��������
};

// ���±�������
struct MSC_LOGDB_BagSlot
{
	CT_DWORD 			dwUserID;          // �û�ID
	CT_INT32            nSlot;             // �����ĸ�����
	CT_INT32            iDiamond;          // ���ĵ���ʯ
	CT_INT32            iUserGrade;        //�û��ȼ�
};

//���������Ϣ
struct MSC_LOGDB_RegionUnlockData
{
	CT_DWORD	dwUserID;	//�û�ID
	CT_INT32    iUserGrade; //�û��ȼ�
	CT_INT32	iRegionID;	//����ID
	CT_INT32	iGold;		//���Ľ��
	CT_INT32	iDiamod;	//������ʯ
};

//����ʹ�����
struct CMD_LOGDB_PropFinish
{
	CT_DWORD	dwUserID;		//�û�ID
	CT_INT32	iID;			//����ID
};

// ǿ���Ĳ��
struct MSC_LOGDB_SmithingAndSlot
{
	CT_DWORD 			dwUserID;          // �û�ID
	CT_INT32            iUserGrade;        //�û��ȼ�
	CT_DWORD            key;               // Ψһ��ʶ
	CT_INT32            iEquipID;          // ����ID
	CT_INT32            iLevel;            // ǿ���ȼ��仯ֵ
	CT_INT32            nSlot;             // ��������仯ֵ    
	CT_INT32            iDiamond;          // ������ʯ��
	CT_INT32            iGold;             // ���Ľ����
	CT_INT32            iItemID;           // ���ĵĵ���ID
	CT_INT32            iItemNum;          // ���ĵĵ�������
};

// �̳�
struct MSC_LOGDB_InheritData
{
	CT_DWORD 			dwUserID;          // �û�ID
	CT_DWORD            iUserGrade;        //�û��ȼ�
	CT_DWORD            keySrc;            // ���̳е�װ��Ψһ��ʶ
	CT_INT32            iSrcID;            // ���̳е�װ��ID
	CT_INT32            iLvSrc;            // ���̳�ʱ��װ���ȼ� 
	CT_DWORD            keyDst;            // �̳е�װ��Ψһ��ʶ
	CT_INT32            iDstID;            // �̳е�װ��ID
	CT_INT32            iDiamond;          // ������ʯ��
	CT_INT32            iGold;             // ���Ľ����
	CT_INT32            iItemID;           // ���ĵĵ���ID
	CT_INT32            iItemNum;          // ���ĵĵ�������
};

// ǿ���Ĳ��
struct MSC_LOGDB_PropAddStrengthen
{
	CT_DWORD 			dwUserID;          // �û�ID
	CT_DWORD            iUserGrade;        // �û��ȼ�
	CT_INT32            iEquipID;          // ����ID  
	CT_INT32            iDiamond;          // ������ʯ��
	CT_INT32            iGold;             // ���Ľ����
	CT_INT32            iItemID;           // ���ĵĵ���ID
	CT_INT32            iItemNum;          // ���ĵĵ�������
	CT_INT32			iIndex;			   //
};

//�����¼�����
struct MSC_LOGDB_EventAward
{
	CT_DWORD 			dwUserID;       	//�û�id
	CT_INT32 			iGold;				//���
	CT_INT32 			iFood;				//ʳ��
	CT_INT32 			iCiviliNum;     	//������
	CT_CHAR             cProp1Name[NAME_SIZE];       //����1����
	CT_INT32            iProp1;     		//����1����
	CT_INT32            iPropNum1;  		//����1����
	CT_CHAR             cProp2Name[NAME_SIZE];       //����2����  
	CT_INT32            iProp2;     		//����2����
	CT_INT32            iPropNum2;  		//����2���� 
};

// ����Ǳ����Ϣ
struct MSC_LOGDB_TalentData
{
	CT_DWORD			dwUserID;          // �û�ID
	CT_INT32			iTalentID;         // Ǳ��ID
	CT_INT32			iExp;              // ���ĵ�Ǳ�ܵ���
};

//������
struct MSC_LOGDB_TaskReward
{
	CT_DWORD		dwUserID;	//�û�ID
	CT_INT32        iUserGrade; //�û��ȼ�
	CT_INT32		iTaskID;   //����id
	CT_INT32		iGold;      //���
	CT_INT32		iDiamod;	//��ʯ
	CT_INT32		iXp;        //����
	CT_INT32		iFood;      //ʳ��
	CT_INT32		iPotential;	//Ǳ����
	CT_INT32		iSoul;		//�����
	CT_INT32		iPrestige;	//����
	CT_INT32		iCiviliNum; //������
	CT_INT32		iStamina;	// ����ֵ
	CT_INT32		iItemID1;     //����1����
	CT_INT32		iNum1;			//����1����
	CT_INT32		iItemID2;     //����2����
	CT_INT32		iNum2;			//����2����
	CT_INT32		iItemID3;     //����3����
	CT_INT32		iNum3;			//����3����
	CT_INT32		iItemID4;     //����4����
	CT_INT32		iNum4;			//����4����
	CT_INT32		iDiamodCost;	//���������ʯ����
};

//�û��˳�
struct MSC_LOGDB_UserExit
{
	CT_DWORD          dwUserID;       //�û�id
};

//����������¼
struct MSC_LOGDB_AddUpgradeRecord
{
	CT_DWORD            dwUserID;         //�û�id
	CT_INT32            iUserGrade;       //�û��ȼ�
	CT_INT32            iBid;             //����id
	CT_INT32			iGrade;			  //�ȼ�
	CT_INT32			iDiamod;		  //��ʯ
	CT_INT32            iGold;            //��õĽ��
	CT_INT32            iFood;            //���ʳ��
	CT_INT32			iCivil;			  //���������
	CT_INT32            iWorker;          //���ĵĹ���
	CT_INT32            iStamina;         //���ĵ�����ֵ
	CT_INT32            iProp1;           //��õĵ���1id
	CT_INT32            iProp1Num;        //��õ���1����Ŀ
	CT_INT32            iProp2;           //��õĵ���2id
	CT_INT32            iProp2Num;        //��õ���2����Ŀ
	CT_INT32            iProp3;           //��õĵ���3id
	CT_INT32            iProp3Num;        //��õ���3����Ŀ
	CT_INT32            iProp4;           //��õĵ���4id
	CT_INT32            iProp4Num;        //��õ���4����Ŀ
	CT_INT32            iXp;              //������
	//CT_INT32            iSpeedTimes;      //���ٴ���
	CT_INT32            iUpgradeTimes;    //��������     
};

struct MSC_LOGDB_VipApply
{
	CT_DWORD		dwUserID;			//�û�id
	CT_INT32        iUserGrade;         //�û��ȼ�
	CT_INT32		iPropID;			//����id  
	CT_INT32		iPropNum;			//��������
	CT_INT32		iDiamod;			//��ʯ
	CT_INT32		iDays;				//��ͨ����
	CT_INT32		iGradeOpen;			//��ͨ�ȼ�
};

struct MSC_LOGDB_ActiveReward
{
	CT_DWORD    dwUserID;   // �û�id
	CT_INT32    iLevel;     // ��Ծ�ȵȼ�
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

//�򿪱���
struct	MSC_LOGDB_Chest
{
	CT_DWORD		dwUserID;
	CT_INT32		iGold;					//���
	CT_INT32		iPrestige;				//����
	CT_INT32		iTalentNum;				//Ǳ��
	CT_INT32		iGoodsID1;				//����ID1
	CT_INT32		iGoodsCount1;			//����1
	CT_INT32		iGoodsID2;				//����ID2
	CT_INT32		iGoodsCount2;			//����2
	CT_INT32		iGoodsID3;				//����ID3
	CT_INT32		iGoodsCount3;			//����3
};

struct MSC_LOGDB_ChatTime
{
	CT_DWORD    dwUserID;   // �û�id
	CT_INT32    iChatTime;	//�������
};

struct MSC_LOGDB_AddProductingRecord
{
	CT_DWORD            dwUserID;         //�û�id
	CT_INT32            iUserGrade;       //�û��ȼ�
	CT_INT32            iBid;             //����id
	CT_INT32            iGold;            //��õĽ��
	CT_INT32            iFood;            //���ʳ��
	CT_INT32			iCivil;			  //���������
	CT_INT32			iDiamod;		//������ʯ
	CT_INT32            iWorker;          //���ĵĹ���
	CT_INT32            iStamina;         //���ĵ�����ֵ
	CT_INT32            iProp1;           //��õĵ���1id
	CT_INT32            iProp1Num;        //��õ���1����Ŀ
	CT_INT32            iProp2;           //��õĵ���2id
	CT_INT32            iProp2Num;        //��õ���2����Ŀ
	//CT_INT32            iSpeedupTimes;    //���ٴ���
	CT_INT32            iProductingTimes; //��������

};

// ��ӳ�ֵ��¼
struct MSC_LOGDB_Charge
{
	CT_INT32 dwUserID;      // �û�ID
	CT_INT32 iPrice;          // �۸�
	CT_INT32 iDiamond;      // ����
	CT_INT32 iState;        // ����״̬��0���ɹ�����1���ύ��
	CT_CHAR szSign[EXORDER_NO_LEN + 1];   // ����ǩ��
	CT_INT32 iUserGrade;    //�û��ȼ�
};

// ���³�ֵ��¼��Ϣ
struct MSC_LOGDB_UpdateCharge
{
	CT_CHAR szSign[EXORDER_NO_LEN + 1];   // ����ǩ��
	CT_INT32 iIndex;                // �������
	CT_INT32 iState;                // ����״̬
};

// ���³�ֵ��¼��Ϣ,ƻ������Ҫ�޸Ķ����Ľ��
struct MSC_LOGDB_UpdateCharge_EX
{
	CT_CHAR szSign[EXORDER_NO_LEN + 1];   // ����ǩ��
	CT_INT32 iIndex;                // �������
	CT_INT32 iState;                // ����״̬
	CT_INT32 iMoney;				// �������
	CT_INT32 iDiamond;				// ��ʯ
};

// ��ȡǩ������ [11/14/2012 John]
struct MSC_LOGDB_SignReward
{
	CT_DWORD    dwUserID;   // �û�id
	CT_INT32    iUserGrade; //�û��ȼ�
	CT_INT32    nSigned;    // ǩ������
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
	CT_INT32    iCostDiamond;   // ���ĵ���ʯ
};

struct MSC_LOGDB_FightRecord
{
	CT_DWORD    dwUserID;   // �û�id
	CT_INT32    iUserGrade; //�û��ȼ�
	CT_INT32	iAid;		//����id
	CT_INT32	iFid;		//��������id
	CT_INT32	iDifficulty;//�Ѷ�
	CT_INT32	iXp;		//����
	CT_INT32	iGold;		//���
	CT_INT32	iTalent;	//Ǳ��
	CT_INT32	iDiamod;	//��ʯ
	CT_INT32	iSkillNum;	//���ܵ�
	CT_INT32	iPropID[PVP_GOODS_COUNT];	//��Ʒid
	CT_INT32	iPropNum[PVP_GOODS_COUNT];	//��Ʒ����
	CT_INT32	iWin;		//���
};
typedef MSC_LOGDB_FightRecord MSC_LOGDB_TeamDuplicateRecord;

//��Ӹ�������
struct MSC_LOGDB_TeamDuplicateCost
{
	CT_DWORD	dwUserID;		//�û�Id
	CT_INT32	iSceneID;		//����ID
	CT_INT32	iDifficulty;	//����
	CT_INT32	iStaminaCost;	//��������
};

struct MSC_LOGDB_PETUPSKILL
{
	CT_DWORD    dwUserID;   // �û�id
	CT_INT32	iSkillID;		//����id
	CT_INT32	iItemID;		//����ID
	CT_INT32	iNum;			//������Ŀ
	CT_INT32	iGold;		//�������
};

struct MSC_LOGDB_PetRename
{
	CT_DWORD    dwUserID;   // �û�id
	CT_INT32	iPetID;		//����id
	CT_INT32	iIndex;		//����
	CT_INT32	iPropID;	//��Ʒid
	CT_INT32	iPropNum;	//��Ʒ����
};

struct MSC_LOGDB_ItemSell
{
	CT_DWORD    dwUserID;   // �û�id
	CT_INT32	iPropID;	//��Ʒid
	CT_INT32	iPropNum;	//��Ʒ����
	CT_INT32	iGlod;		//���۽��
};

//�����¼
struct MSC_LOGDB_ShoppingRecord
{
	CT_DWORD	dwUserID;			//�û�id
	CT_INT32    iUserGrade;         //�û��ȼ�
	CT_INT32	iGoodsID;			//��Ʒid
	CT_INT32	iGoodsNum;			//��Ʒ����
	CT_INT32	iDiamod;			//������ʯ
	CT_INT32	iGold;				//���ѽ��
	CT_INT32	iItemID;			//���ѵ���
	CT_INT32	iItemNum;			//��������
	CT_INT32	iMarketType;		//�̵�����
};

//���������¼
struct MSC_LOGDB_BuyStatmina
{
	CT_DWORD	dwUserID;			//�û�ID
	CT_INT32    iUserGrade;         //�û��ȼ�
	CT_INT32	iStatNum;			//��������ֵ
	CT_INT32	iDiamod;			//������ʯ
};

//pvpս������
struct MSC_LOGDB_PVPReward
{
	CT_CHAR		cIndex[PVPRECORD_INDEX];				//����	
	CT_DWORD	dwUserID;				//�û�Id
	CT_INT32	iGold;					//���
	CT_INT32	iPrestige;				//����
	CT_INT32	iGoods[PVP_GOODS_COUNT];//��Ʒ��Ŀ
	CT_INT32	iCount[PVP_GOODS_COUNT];//��Ŀ
	CT_INT32	iResult;		//���
	CT_DWORD	dwAimID;			//����ID
};

//ǿ��ˢ�¼�¼
struct MSC_LOGDB_Refresh
{
	CT_DWORD	dwUserID;				//�û�id
	CT_DWORD    iUserGrade;             //�û��ȼ�
	CT_INT32	iDiamod;				//��ʯ
};

// ��ħ��¼
struct MSC_LOGDB_Enchant
{
	CT_DWORD dwUserID;
	CT_INT32 iUserGrade;
	CT_INT32 iIndex;              // ��������
	CT_INT32 iGrade;              // ���Ը�ħ�ȼ�
	CT_INT32 nDiamondCost;        // ����������ʯ
    CT_INT32 nGoldCost;           // �������Ľ��
	CT_INT32 nTalentNumCost;      // ��ħ����
};

//��ʯ����ǿ����ȴʱ��
struct MSC_LOGDB_ClearCD
{
	CT_DWORD	dwUserID;				//�û�id
	CT_INT32	iDiamod;				//��ʯ
	CT_INT32    iUserGrade;             //�û��ȼ�
};


//����������ȡ
struct MSC_LOGDB_DuplicateReward
{
	CT_DWORD		dwUserID;						//�û�id
	CT_INT32        iUserGrade;                     //�û��ȼ�
	CT_INT32		iWorldSceneID;					//���糡��ID
	CT_INT32		iRewardID;						//����ID
	CT_INT32		iExperience;					//����
	CT_INT32		iGold;							//���
	CT_INT32		iScore;							//����
	CT_INT32		iGoodsID1;						//����ID
	CT_INT32		iCount1;						//����ID
	CT_INT32		iGoodsID2;						//����ID
	CT_INT32		iCount2;						//����ID
	CT_INT32		iPotential;						//Ǳ����
	CT_INT32 		iDiamond;						//��ʯ
	CT_INT32		iSkillPrint;					//���ܵ�
};

//������������
struct MSC_LOGDB_DuplicateEnter
{
	CT_DWORD		dwUserID;						//�û�id
	CT_INT32		iFood;							//ʳ������
	CT_INT32		iPphysical;						//��������
	CT_INT32		iGoods;							//��Ʒ����
	CT_INT32		iCount;							//��Ʒ����

};

//��������ս������
struct  MSC_LOGDB_BuyFightingTime
{
	CT_DWORD		dwUserID;						//�û�id
	CT_DWORD        iUserGrade ;                    //�û��ȼ�
	CT_INT32		iBuyFightingTime;				//����ս����
	CT_INT32		iDiamod;						//��ʯ
};

//������ս������
struct MSC_LOGDB_PVPFighringCost
{
	CT_DWORD		dwUserID;						//�û�id
	CT_INT32        iUserGrade;                     //�û��ȼ�
	CT_INT32		iDiamod;						//��ʯ
	CT_INT32		iStamina;						//��������

};

struct MSC_LOGDB_ActiveEventReward
{
	CT_DWORD		dwUserID;						//�û�id
	CT_INT32        iGrade;                         //�û��ȼ�
	G_ActiveEventReward	activeReward;				//�����
};

struct MSC_LOGDB_WorshipReward
{
	CT_DWORD		dwUserID;						//�û�id
	CT_INT32		iGoldCost;						//���Ľ��
	CT_INT32		iGold;							//��ý��
	CT_INT32		iFood;							//���ʳ��
	CT_INT32		iTalent;						//Ǳ�ܵ�
	CT_INT32		iDiamod;						//��ʯ
	CT_INT32		iXp;							//����
	CT_INT32		iItemID1;						//����id1
	CT_INT32		iItemNum1;						//��������1
	CT_INT32		iItemID2;						//����id2
	CT_INT32		iItemNum2;						//��������2
	CT_INT32		iGrade;							//�����ȼ�
};

//ɨ�����ټ�¼
struct MSC_LOGDB_RaidsSpeed
{
	CT_DWORD		dwUserID;						//�û�ID
	CT_INT32		iDiamod;						//��ʯ
	CT_INT32		iGrade;							//�ȼ�
};

//����ٺ»
struct MSC_LOGDB_Salary
{
	CT_DWORD		dwUserID;						//�û�Id
	CT_INT32		iFood;							//ʳ��
	CT_INT32		iGold;							//���
};

//��������
struct MSC_LOGDB_RankingUpGrade
{
	CT_DWORD		dwUserID;						//�û�id
	CT_INT32		iGrade;							//�ȼ�
	CT_INT32		iGradeUp;						//������Ŀ
	CT_INT32		iRankxp;						//���ľ���
};

//��Ʒʹ��
struct MSC_LOGDB_UserItem
{
	CT_DWORD		dwUserID;						//�û�Id
	CT_INT32		iItemID;						//��ƷID
	CT_INT32		iItemNum;						//
};

struct MSC_LOGDB_AddItem
{
	CT_DWORD		dwUserID;						//�û�Id
    CT_DWORD        saveTag;                        //����
	CT_INT32		iItemID;						//��ƷID
	CT_INT32		iItemNum;						//
};

//�¼�����
struct MSC_LOGDB_EventReward
{
	CT_DWORD		dwUserID;						//�û�Id
	CT_INT32        iGold;      //���
	CT_INT32        iCiviliNum; //������
	CT_INT32        iXp;        //����
	CT_INT32        iFood;      //ʳ��
	CT_INT32        iProp1;     //����1����
	CT_INT32        iProp2;     //����2����
	CT_INT32        iNum1;     //����1����
	CT_INT32        iNum2;     //����2����
};

//������������¼
struct MSC_LOGDB_PetReName
{
	CT_DWORD		dwUserID;				//�û�id
	CT_INT32		iIndex;					//��������
	CT_INT32		iPetID;					//����ID
	CT_INT32        iProp1;     //����1����
	CT_INT32        iNum1;     //����1����
};

//����������־
struct MSC_LOGDB_PetUpGrade
{
	CT_DWORD		dwUserID;			//�û�id
	CT_INT32		iPetGrade;			//�ȼ�
	CT_INT32		iExp;				//���ľ���
};

//��ǽ�ӳ���־
struct MSC_LOGDB_WallAddition
{
	CT_DWORD		dwUserID;					//�û�id
	CT_INT32		iResID[TEAM_RES_CNT];		//��ԴID
	CT_INT32		iResCount[TEAM_RES_CNT];	//��Դ����
};

//�Ӷ���־
struct MSC_LOGDB_Plunder
{
	CT_DWORD		dwUserID;
	CT_INT32		iFood;
	CT_INT32		iGold;
	CT_DWORD		dwDstUserID;
};


//�û���¼��Ϣ����
//////////////////////////////////////////////////////////////////////////
//����������Ϣ
struct MSC_DB_InsertMasterInfo
{
	CT_DWORD		dwUserID;
	MasterData		MasterInfo;
};

//�õ�����������������Ϣ
struct MSC_DB_InsertUpGradeBuild
{
	CT_DWORD		dwUserID; // �û�id
	CT_INT32        iBid;     //����id
	CT_INT32        iLasting; //����ʱ��
	//CT_INT32        iDiamod;  //����ʹ�õ���ʯ
	CT_LONGLONG     llBeginTime; //��ʼʱ��
	CT_INT32        iGrade;     //�����ﵽ�ĵȼ�
};
//�õ�Ǳ������
struct  MSC_DB_InsertTalentInfo
{
	TalentData		talentData;
	CT_DWORD		dwUserID;
};

//�õ���������
struct  MSC_DB_InsertSkillData
{
	SkillData		skillData;
	CT_DWORD		dwUserID;
};

//�õ���������|Ԫ��֮��
struct  MSC_DB_InsertItemInfo
{
	ItemData		itemData;
	CT_DWORD		dwUserID;
};

//�õ�װ����Ϣ
struct  MSC_DB_InsertEquipInfo
{
	EquipmentData	equipData;
	CT_DWORD		dwUserID;
};
// �õ�Ԫ����������
struct  MSC_DB_InsertElmAttrInfo
{
    ElmMarrowAttr elmData;
    CT_DWORD      dwUserID;
};
// �õ�ս������
struct  MSC_DB_InsertBattleFlagInfo
{
    BattleFlag    flagData;
    CT_DWORD      dwUserID;
};
//�õ���������
struct  MSC_DB_InsertBuffInfo
{
	BuffData		buffData;
	CT_DWORD		dwUserID;
};
//�õ���ȴ��Ϣ
struct  MSC_DB_InsertCooldownInfo
{
	CooldownData	cdData;
	CT_DWORD		dwUserID;
};

//�õ���ȴ��Ϣ
struct  MSC_DB_InsertArmyInfo
{
	ArmyInfo	    Armydata;
	CT_DWORD		dwUserID;
};

//�õ����������Ľ���
struct  MSC_DB_InsertProductingBuilding
{
	ProductingBuilding	Producting;
	CT_DWORD		dwUserID;
};

//�õ����糡������
struct  MSC_DB_InsertWorldScene
{
	MSC_DB_WorldSceneInfo	data;
	CT_DWORD		dwUserID;
};

//�õ���������
struct  MSC_DB_InsertSceneInfo
{
	//MSC_DB_SceneInfo		data;
	SceneInfo		data;
	CT_DWORD		dwUserID;
};

//�õ�ս������
struct  MSC_DB_InsertBuyFightingTime
{
	CT_DWORD		dwUserID;
	CT_INT32		iBuyFightingTime;
};

//�õ��û���������
struct  MSC_DB_InsertRegionInfo
{
	MSC_DB_R_UnlockRegion	unlockRegion;
	CT_DWORD		dwUserID;
};

//�õ�����Ϣ
struct  MSC_DB_InsertBattleArray
{
	MSC_DB_BattleArray		data;
	CT_DWORD		dwUserID;
};

//�õ��û����������
struct  MSC_DB_InsertTaskFinish
{
	MSC_DB_TaskFinishData	taskFinishData;
	CT_DWORD		dwUserID;
};

//�õ��û��ڽ�������
struct  MSC_DB_InsertProgressData
{
	MSC_DB_TaskProgressData	taskPData;
	CT_DWORD		dwUserID;
};

//�õ�ǩ����Ϣ
struct  MSC_DB_InsertDailySignInfo
{
	SignData		stInfo;
	CT_DWORD		dwUserID;
};

//�õ���Ծ����Ϣ
struct  MSC_DB_InsertDailyQuestInfo
{
	DailyQuestData			stInfo;
	CT_DWORD		dwUserID;
};

//�õ��ճ����������Ϣ
struct  MSC_DB_InsertDailyActive
{
	ActiveData				stInfo;
	CT_DWORD		dwUserID;
};

//�õ�������Ϣ
struct  MSC_DB_InsertBuildingInfo
{
	BuildingData			buildData;
	CT_DWORD		dwUserID;
};

//�õ��̵���Ϣ
struct  MSC_DB_InsertMarketFunction
{
	MSC_DB_MarketFunction	marketFun;
	CT_DWORD		dwUserID;
};

//�̵�����޹�
struct MSC_DB_InsertMarketPersonal
{
	CT_DWORD		dwUserID;
	CT_INT32		iGoods;
	CT_INT32		iNum;	
};

//�õ�������Ϣ
struct  MSC_DB_InsertBlackMarketInfo
{
	MSC_DB_BlackMarketRefresh	blackMarket;
	CT_DWORD		dwUserID;
};

//�õ�������Ϣ
struct  MSC_DB_InsertPetInfo
{
	CT_DWORD		dwUserID;
	PetInfo					pi;
};

//����û�
struct MSC_DB_InsertPetChange
{
	CT_DWORD		dwUserID;
	CT_INT32		iChangeID;
	CT_INT32		iSkillID;
};

//��ȡvip��Ϣ
struct  MSC_DB_InsertVipInfo
{
	CT_INT32		iGrade;
	CT_INT32		iTime;
	CT_INT32		iGift;
	CT_DWORD		dwUserID;
};

//��������Ϣ
struct  MSC_DB_InsertPVPInfo
{
	UserFightingStatus  UserFightinginfo[PVP_CHALLENGE_COUNT];//�û�ս����Ϣ
	LookUserInfo		LookUserinfo[PVP_CHALLENGE_COUNT];	//�鿴�û���Ϣ
	PVPInfo			PVPdata;
	CT_DWORD		dwUserID;
	CCNetInterface*   pNetInterface;   //�û�ͨѶ�ӿ�
	CT_DWORD          dwThreadID;      //�߳�id	
};

//���а���Ϣ
struct  MSC_DB_InsertTop
{
	CT_DWORD		dwUserID;
	CT_INT32		iType;
	UserFightingStatus  UserFightinginfo[TOP_COUNT];//�û�ս����Ϣ
	LookUserInfo		LookUserinfo[TOP_COUNT];	//�鿴�û���Ϣ
};

//PVPս����¼
struct  MSC_DB_InsertPVPRecord
{
	CT_DWORD				dwUserID;
	MSC_PVP_Fighting_Info	FightingInfo[PVP_FIGHTING_COUNT];
	MSC_DB_FighitngRecord	FightingRecord[PVP_FIGHTING_COUNT];
};

//��ȡ��ħ��Ϣ
struct  MSC_DB_InsertEnchantData
{
	CT_DWORD		 dwUserID;
	CT_INT32         iBID;               // ��λ
	CT_INT32         iIDX;               // ���
	CT_INT32         iGrade;             // ��ħ�ȼ� 
	CT_INT32		 iFailCount;		 // ʧ�ܴ���
};

//��ȡ��ħ������Ϣ
struct MSC_DB_InsertEnchantReward
{
	CT_DWORD		dwUserID;
	CT_INT32		iIndex;				// ��������
};

//��ȡɨ����Ϣ
struct	MSC_DB_InsertRaidsInfo
{
	CT_DWORD		dwUserID;			//�û�id
	CT_INT32		iWorldID;			//���糡��
	CT_INT32		iSceneID;			//����ID
	CT_INT32		iDifficulty;		//�Ѷ�
	CT_INT32		iAllTime;			//�ܴ���
	CT_INT32		iCurrentTime;		//��ǰ����
	CT_INT64		lBeginTime;			//��ʼʱ��
	CT_INT32		iLastStamina;		//ʣ������
};

//ɨ������
struct MSC_DB_InsertRaidsReward
{
	CT_INT32		iCurrentTime;						//��ǰ����
	CT_DWORD		dwUserID;							//�û�ID
	CT_INT32		iExperience;						//����
	CT_INT32		iGold;								//���
	CT_INT32		iDiamond;							//��ʯ
	CT_INT32		iPotential;							//Ǳ����
	CT_INT32		iSkillPoint;						//���ܵ�ǿ����
	CT_INT32		iGoods[GOODS_COUNT];				//װ��ID
	CT_INT32		iGoodsCount[GOODS_COUNT];			//��Ŀ
};

//ɨ������
struct MSC_DB_InsertTeamDuplicateScene
{
	CT_DWORD		dwUserID;							//�û�id
	CT_INT32		iSceneID;							//����ID
	CT_INT32		iDifficulty;						//�Ѷ�
	CT_INT32		iWin;								//�Ƿ�ʤ��
};

//ս�ӿƼ�
struct MSC_DB_InsterFightTeamScience
{
	CT_DWORD		dwUserID;							//�û�id
	CT_INT32		iScienceID;							//�Ƽ�id
	CT_INT32		iGrade;								//�Ƽ��ȼ�
};


//�״̬��Ϣ
struct MSC_DB_ActiveEventStatus
{
	CT_DWORD		dwUserID;       //�û�id
	CT_INT32		iAID;			//�id
	CT_INT32		iStatus;		//�״̬
	CT_INT32		iQuantity;		//�����
};
//ת��״̬��Ϣ
struct MSC_DB_Turntable
{
	CT_DWORD		dwUserID;       //�û�id
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

//��¼�仯�Ļ
struct MSC_DB_ActiveEventChange
{
	CT_INT32		iActive[ACTIVE_CHANGENUM];		//�仯�Ļ
	CT_INT32		iNum;			//����
};

//����״̬��Ϣ
struct MSC_DB_WorshipStatus
{
	CT_DWORD		dwUserID;       //�û�id
	CT_INT32		iType;			//��������
	CT_INT32		iStatus;		//����״̬
	CT_INT32		iXp;			//����/��ʯ�������
};

//��Ϣ����
struct MSC_DB_FeedBack
{
	CT_DWORD			dwUserID;								// �û�ID
	CT_CHAR				cMobile[MOBILE];						//�ֻ�����
	CT_CHAR				cEmailContent[FEEDBACK_CONTENT_SIZE];	//������󳤶�
};

struct MSC_DB_FRIEND
{
	CT_DWORD			dwU_ID;
	CT_DWORD			dwF_UID;
	CT_CHAR				cF_NickName[NAME_SIZE];
	CT_INT32			iF_Occupation;
	CT_INT32			iF_Type;
	CT_INT32			iF_Grade;
	CT_INT32			iF_FightingPower;			// ս����(����״̬������ս����)
	CT_INT64			iLastExitTime;				// �������ʱ��
	CT_INT32			iTitleID;					// ʹ�õ�ͷ��ID
};

struct MSC_DB_FRIEND_DELETE
{
	CT_DWORD			dwU_ID;
	CT_DWORD			dwF_UID;
	CT_INT32			iF_Type;
};

struct MSC_DB_FightTeamBase
{
	CT_DWORD	dwFTID;							// ս��ID
	CT_CHAR		cFightTeamName[NAME_SIZE];		// ս������
	CT_CHAR		cDescription[DESCRIPTION_SIZE]; // ս�ӽ���
	CT_DWORD	dwBossUID;						// ����ID
	CT_CHAR		cBossName[NAME_SIZE];			// ��������
	CT_INT32	iFightingPower;					// ��ս����
	CT_INT32	iTop;							// ����
	CT_INT32	iPreTop;						// �ϴ�����
	CT_INT32	iResource;						// ��Դ��
	CT_INT32	iIcon;
	CT_DWORD	dwElite1;						// ��ս��Ӣ1
	CT_DWORD	dwElite2;						// ��ս��Ӣ2
	CT_DWORD	dwElite3;						// ��ս��Ӣ3
	CT_INT32	iResCount;						// ��Դ����

	CT_INT32	iGrade;							//�ȼ�
	CT_INT32	iItemCount;						//������Դ��
	CT_INT32	iSummonCount;					//�ٻ�����
	CT_INT32	iBossMaxIndex;					//���BOSS����
	CT_INT32	iTimeBossMaxIndex;				//ʱ��BOSS�������
};


struct MSC_DB_FightTeamMember
{
	CT_DWORD	dwFTID;							// ս��ID
	CT_DWORD	dwUserID;						// ��ԱID
	CT_CHAR		cNickName[NAME_SIZE];			// ��Ա����
	CT_INT32	iFightingPower;					// ��Աս����
	CT_INT32	iOccupation;					// ְҵ
	CT_INT32	iGrade;							// �ȼ�
	CT_INT32	iRank;							// ����
	CT_INT32	iContribution;					// ����
	CT_INT32	iStatus;						// ״̬ 0.���� 1.פ��
	CT_INT64	iTime;							// ����/����ʱ��
	CT_INT32	iTowerCPID;						// Ԫ�������ͨ�ز���
	CT_INT32	iContributeCount[CONTRIBUTE_TYPE];//���״���
	CT_INT32	iContributeGross;					//��������
	CT_INT64	iLastExitTime;
	CT_INT32	iTitle;							// ��Աͷ��ID 
	CT_INT32	iType;							// ��Ա���� 1:��Ա 2:�������� 3:�������
};


struct MSC_DB_FightTeamMemberDelete
{
	CT_DWORD	dwFTID;							// ս��ID
	CT_DWORD	dwUserID;						// ��ԱID
	CT_INT32	iType;							// ��Ա���� 1:��Ա 2:�������� 3:�������
};

struct MSC_DB_FightTeamRecruit
{
	CT_DWORD	dwFTID;							// ս��ID
	CT_INT64	iBeginTime;						// ����ʱ��
	CT_INT32	iLastingTime;					// ʱ��
	CT_CHAR		cRecruit[DESCRIPTION_SIZE];		// ��ļ��Ϣ
	CT_INT32	iAuto;							// ����ս���Զ���ļ
};

struct FightTeamMemberUID
{
	CT_DWORD	dwCount;						// ��Ա��
	CT_DWORD	dwMemberUID[CLAN_MAX_MEMBER];	// ��ԱUID
};

struct FightTeamFightData
{
	UserFightingStatus	userFightingLeft[SPOT_GUARD_MEMBER];
	UserFightingStatus	userFightingRight[SPOT_GUARD_MEMBER];
	CT_INT32			iSeed;					//�������
	CT_INT32			iSpotQuality;			//��Դ��Ʒ��
	CT_BOOL				bPVP;					//TRUE:PVP, FALSE:PVE
	CT_DWORD			dwLeftFTID;
};

struct MSC_DB_FTMemberFightingData
{
	CT_DWORD			dwUserID;        //�û�id
	UserFightingStatus	userFightingData;		//ս����Ϣ
};

//�������
struct MSC_DB_FTMemeberRefresh 
{
	CT_DWORD			dwUser;         //�û�id
	CT_INT32			iGrade;         //�û��ȼ�
	CT_INT32			iFightingPower; //ս����
};

//�ٻ�ս��
struct MSC_FightTeamSummonFighting
{
	CT_DWORD			dwUserID[SUMMON_MAX];				//�ٻ���ID
	CT_DWORD			dwFTID;					//ս��ID
	CT_INT32			iWin;					//ʤ����ʶ
	CT_INT32			iBossIndex;				//BOSS����
	CT_INT32			iSeed;
	UserFightingStatus	FightingData[CLAN_CRUSADE_MEMBER];	//��ս��Ӣ
};
//�Ƽ�����
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
	CT_INT64 lDate;                          // �¼�������ʱ��
	CT_INT32 iReport;                        // �¼���صı���ID��ս����
	CT_CHAR  cEventDesc[DESCRIPTION_SIZE];   // �¼�����
};

//ս���ٻ�
struct MSC_DB_FightTeamSummonInfo
{
	CT_DWORD			dwFTID;							//ս��ID
	CT_DWORD			dwUserID;			//�û�id
	CT_INT32			iBossIndex;						//BOSS����
	CT_INT32			iWin;				//ʤ��
	//CT_DWORD			dwChestIndex;		//��������
	CMD_G_NormalReward	Res;				//��Դ
};

//ս���ٻ�����
struct MSC_DB_FightTeamSummInfoItem
{
	CT_DWORD			dwFTID;							//ս��ID
	CT_DWORD			dwUserID;						//�û�id
	CT_INT32			iChestIndex;					//��������
	CT_INT32			iItemID;						//����ID
	CT_INT32			iCount;							//��������
	CT_INT32			iType;							//����
};

//ս�ӿƼ��ͷ�
struct MSC_DB_FightTeamScienceDownGrade
{
	CT_DWORD			dwUserID;						//�û�Id
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

//ս������
struct MSC_LOGDB_FightTeamUpGrade
{
	CT_DWORD dwFTID;		//ս��ID
	CT_DWORD dwUserID;		//�ӳ�ID
	CT_INT32 iRes;			//��Դ��Ŀ
	CT_INT32 iItemRes;		//������Դ
	CT_INT32 iGrade;		//ս�ӵȼ�
};

//ս���ٻ�����
struct MSC_LOGDB_FightTeamSummonCost
{
	CT_DWORD dwFTID;		//ս��ID
	CT_DWORD dwUserID;		//�ӳ�ID
	CT_INT32 iRes;			//��Դ��Ŀ
	CT_INT32 iItemRes;		//������Դ
	CT_INT32 iBossIndex;	//boss����
	CT_INT32 iGrade;		//�û��ȼ�
};

//�ٻ�������Դ
struct MSC_LOGDB_FightTeamSummonRewardRes
{
	CT_DWORD dwFTID;		//ս��ID
	CT_DWORD dwUserID;		//�û�ID
	CT_INT32 iBossIndex;	//�ٻ�BOSS
	CMD_G_NormalReward Reward;//��Դ����
	CT_INT32 iGarde;		//�û��ȼ�
};

//�ٻ���������
struct MSC_LOGDB_FightTeamSummonRewardItem
{
	CT_DWORD dwFTID;		//ս��ID
	CT_DWORD dwUserID;		//�û�ID
	CT_INT32 iBossIndex;	//�ٻ�BOSS
	CT_INT32 iItemID[4];		//����ID
	CT_INT32 iItemCount[4];	//������Ŀ
	CT_INT32 iGarde;		//�û��ȼ�
};

//ս�ӿƼ�����
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

//�ٻ�BOSSս��
struct MSC_FTTimeBossFight
{
	CT_DWORD			dwUserID;				//�ٻ���ID
	CT_DWORD			dwFTID;					//ս��ID
	CT_INT32			iWin;					//ʤ����ʶ
	CT_INT32			iBossIndex;				//BOSS����
	CT_INT32			iSeed;
	CT_INT32			iHurt;					//�˺�
	UserFightingStatus	FightingData[CLAN_CRUSADE_MEMBER];	//��ս��Ӣ
};

//����ʱ��boss
struct MSC_DB_FTTBClear
{
	CT_DWORD			dwFTID;					//ս��id
};

//ʱ��boss��Ϣ
struct MSC_DB_FTTimeBossInfo
{
	CT_DWORD			dwFTID;					//ս��Id
	CT_INT32			iIndex;							//����
	CT_INT32			iSummonBeginTime;					//�ٻ���ʼʱ��
	CT_INT32			iCurrentHP;						//��ǰhp
	CT_INT32			iHelpUser[HELP_SUMMON_COUNT];			//�����û�
};

//ʱ��boss��Ա��Ϣ
struct MSC_DB_FTTimeBossMember
{
	CT_DWORD			dwFTID;					//ս��Id
	CT_DWORD			dwUserID;
	CT_INT32			iHurt;								//�˺�
	CT_INT32			iFTFood;							//ս����ʳ
	CT_INT32			iFTContribute;						//ս�ӹ���
	CT_INT32			iFightNum;							//ս������
};
//��Ӹ���
//////////////////////////////////////////////////////////////////////////
//�û��뿪
typedef CMD_G_TeamDuplicateLeave MSC_SUB_TeamDuplicateLeave;

//�û���Դ������Ϣ
struct MSC_DB_ResourceProduct
{
	CT_DWORD        dwUserID;         //�û�id
	CT_INT32        iIndex;           //��������
	CT_INT32        iSenceID;         //����id
	CT_INT32        iSpotID;          //���id
	CT_INT32        iGrade;           //�û��ȼ�
	CT_INT32        iProductStatus;   //����״̬
	CT_INT32		iProductCount;    //�Ѿ������Ĵ���
	CT_INT32        iLeftProductTime; //��������ʣ��ʱ��

};

//���а�
struct MSC_DB_Top
{
	CT_DWORD		dwUserID[TOP_COUNT];	//�û�ID
	ENUM_TOP		TopType;				//����
};

//���а���Ϣ
struct MSC_DB_TopData
{
	CT_DWORD		dwUserID[TOP_COUNT];		//�û�ID
	User_Fighting	FightingData[TOP_COUNT];	//�û�����
	ENUM_TOP		TopType;					//����
};

//��Դ������¼���ɵ�ԭ��
enum ENUM_RES_Propduct_Reason
{
	ENUM_RPODUCT_REASON_PRODUCTED = 0, //����
	ENUM_PRODUCT_REASON_KICK_OUT  = 1, //�߳�
	ENUM_PRODUCT_REASON_DISSOLUTION = 2, //�����ɢ
	ENUM_PRODUCT_REASON_OCCUPY = 3, //ռ��
	ENUM_PRODUCT_REASON_GIVEUP = 4, //����
};

//��Դ��������¼
struct   MSC_DB_ResourceProductRecord
{
	CT_DWORD		dwFID;   //ս��id
	CT_DWORD		dwUID;   //�û�id	
	CT_INT32		iScenceID; //����id
	CT_INT32        iSpotID;     //��Դ��id
	//CT_INT32        iGold;     //�õ����
	CT_INT32		iRes;		//��Դ
	CT_INT32		iResType;	//��Դ����
	ENUM_RES_Propduct_Reason        iReason;   //ԭ��
};

//��Դ��ռ�����ļ�¼
struct MSC_DB_ResourceOccupyRecord
{
	CT_DWORD       dwFID;     //ս��id
	CT_INT32       iScenceID; //����id
	CT_INT32       iSpotID;   //��Դ��id
	CT_INT32       iFTeamRes; //���ĵ�ս����Դ
	CT_INT32       iOccupyGold; //�����Ľ��
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

//ɨ��
struct MSC_DB_RAIDS
{
	CT_DWORD			dwU_ID;
	CT_INT32			iBeginID;
	CT_INT32			iEndID;
};

//ȫ������
struct MSC_DB_GlobeData
{
	CT_INT32			iType;
	CT_INT32			iValue;
};


//��Դ����������
struct MSC_DB_TowerSlotReward
{
	CT_DWORD  dwUserID;                 //�û�id
	CT_INT32  iFloor;				    //��
	CT_INT32  iSlot;                    //��Դ��
	G_NormalReward mnReward;			//����
};

struct MSC_DB_TowerSlotRewardDel
{
	CT_DWORD  dwUserID;                 //�û�id
	CT_INT32  iFloor;				    //��
	CT_INT32  iSlot;                    //��Դ��
};

struct MSC_DB_MSG
{
	CT_DWORD  dwUserID;                 //�û�id
	CT_INT32  iMSGType;					//��Ϣ���� 1.Ԫ���� 2.������
	CT_INT64  iTime;				    //ʱ��
	CT_CHAR  cMSG[255];                 //MSG����
};

struct MSC_DB_MSG_DELETE
{
	CT_DWORD  dwUserID;                 //�û�id
	CT_INT32  iMSGType;					//��Ϣ���� 1.Ԫ���� 2.������
	CT_INT64  iTime;				    //ʱ��
};


struct MSC_DB_TowerFight
{
	CT_INT32		iCPID;								//�ؿ�ID
	CT_INT32		iFID;								//ս��ID
	CT_DWORD		dwUserID;							//�û�ID
	CT_INT32		iSeed;								//�������
	CT_INT32		iScore;								//�ɼ�

	UserFightingStatus	FightingData;					//ս������
};

struct MSC_DB_TowerSlotFight
{
	CT_INT32		iFID;								//ս��ID
	CT_DWORD		dwUserID;							//�û�ID
	CT_INT32		iSeed;								//�������
	CT_INT32		iScore;								//�ɼ�

	UserFightingStatus	LeftFightingData;				//��ս��ս������
	UserFightingStatus	RightFightingData;              //����ս����ս������
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
	CT_DWORD dwUserID;              // �û�ID
	CT_DWORD key;                   // Ψһ��ʶ
	CT_INT32 iGemID;                // ��ʯID
	CT_INT32 iSlot;                 // ��λ��
	CT_INT32 iGrade;				// �û��ȼ�
};

struct MSC_Tower_Pass
{
	CT_DWORD dwUserID;
	CT_INT32 iCPID;
};


//gm�㲥 pengwen
struct MSC_Broadcast_GM
{
	CT_DWORD		dwMainID;   //��Ϣ��id
	CT_DWORD		dwSubID;	//��Ϣ��id
	CMD_G_GMMessage gm;			//gm��Ϣ��
};


//pvp�����û� ����
struct MSC_PVP_InsertChallger
{
	CT_DWORD dwUserID;         //�û�id
	CT_DWORD dwChallger;       //����id
	CT_INT32 iPos;             //λ��
};


//�鿴������Ϣ ����
struct MSC_PVP_OtherInfo
{
	CT_DWORD dwUserID;        //�û�id
	LookUserInfo otherUserInfo;    //������Ϣ
};


//��ͨ���� ����
struct MSC_ACTIVE_Status
{
	CT_DWORD dwUserID; 
	CT_INT32 iAID;
	CT_INT32 iStatus;
	CT_INT32 iQuanity;
};

//ת�̻��� ����
struct MSC_TurnTable_Status
{
	CT_DWORD  dwUserID; 
	CT_INT32  iTimes1;    
	CT_INT32  iTimes2;
	CT_INT32  iTimes3;
};

//����BOSSս��
struct MSC_WorldBoss_Fighting
{
	CT_DWORD		dwUserID;							//�����û�ID
	CT_INT32		iSeed;								//�������
	CT_INT32        iWin;                               //�����Ƿ�Ӯ��
	CT_INT32		iHPHurt;							//�˺�ֵ

	UserFightingStatus	LeftFightingData;				//��ս��ս������
	CT_INT32		iMonsterGroupID;					//����bossID
};

//ս����������
struct MSC_FightTeamTask 
{
	CT_INT32	iTaskID;								//������id
	CT_INT32	iSubTaskID;								//������id
	CT_INT32	iType;									//1:��¼ʱȡ������2������������
	CT_DWORD	dwUserID;								//�û��ɣ�
	CT_INT32	iFightTeamGrade;						//ս�ӵȼ�
	SystemTaskType	sysTask;							//���ö�Ӧ����
};

//�޸�ս������
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

//�û����ݻ���
struct MSC_SUB_UserdataInsert
{
	UserFightingStatus			FData;
	UserFightingStatus			SeekFData;		//̽��ս������
	LookUserInfo				LookData;
	CT_INT32					iTime;			//���ݴ���ʱ��
};

//���ս��
struct MSC_CS_FIGHTING_DATA
{
	CT_DWORD	dwUserID;
	CT_INT32	iWiner;
	CT_INT32	iSeed;
	CT_INT32	iPos[2];
	CT_BOOL		bLookLeft;
	UserFightingStatus	LeftFightingData[TEAM_NUM];					//ս������
	UserFightingStatus	RightFightingData[TEAM_NUM];					//ս������
};

//��������
struct MSC_DB_INSPORE
{
	CT_DWORD	dwUserID;
	CT_INT32	iID;
	CT_INT32	iExp;
	CT_INT32	iGrade;
};

//������¼
struct MSC_DB_CS_REWARD
{
	CT_DWORD	dwUserID;
	CT_INT32	iResType;
	CT_INT32	iResNum;
	CT_INT32	iResID;
};

//ɾ������
struct MSC_DB_CS_DelReward
{
	CT_DWORD	dwUserID;
	CT_INT32	iResType;
	CT_INT32	iResID;
};
//����֧��
struct MSC_DB_CS_SUPPORT
{
	CT_DWORD	dwUserID;
	CT_DWORD	dwFTID;
	CT_INT32	iGSID;
	CT_INT32	iResID;
	CT_INT32	iNum;
	CT_INT32	iIndex;
};

//����������־
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

//���������־
struct MSC_LOGDB_CS_REWARD
{
	CT_DWORD	dwUserID;
	CT_INT32	iResType;
	CT_INT32	iResID;
	CT_INT32	iNum;
};

//����֧����־
struct MSC_LOGDB_CS_SUPPORT
{
	CT_DWORD	dwUserID;
	CT_INT32	iResID;
	CT_INT32	iNum;
	CT_DWORD	dwFTID;
};

//���ս��
struct MSC_TOP_CHALLENGE
{
	CT_DWORD	dwUserID;
	CT_INT32	iWiner;
	CT_INT32	iSeed;
	CT_BOOL		bLookLeft;
	UserFightingStatus	LeftFightingData;					//ս������
	UserFightingStatus	RightFightingData;					//ս������
};

//ͷ��
struct MSC_DB_TitleData
{
	CT_DWORD	dwUserID;				// �û�ID
	CT_INT32	iTitleID;				// ͷ��ID
	CT_INT32	iActiveTime;			// ����ʱ��
	CT_INT32	iEffectTime;			// ��Чʱ��
	CT_INT32	iStatus;				// ͷ��״̬
};

//ͷ���б�����
struct MSC_DB_TitleList
{
	CT_DWORD	dwUserID;				// �û�ID
	CT_INT32	iTitleID;				// ͷ��ID
};

//ͷ����־
struct MSC_LOGDB_Title 
{
	CT_DWORD	dwUserID;				// �û�ID
	CT_INT32	iTitleID;				// ͷ��ID
	CT_INT32	iStatus;				// ͷ��״̬��0��δ���1�����2��ʹ�ã�3��ȡ��ʹ��
	CT_INT32	iLastTime;				// ͷ����Чʱ��
};

//����
struct MSC_DB_IslandData 
{
	CT_DWORD	dwUserID;				// �û�ID
	CT_INT32	iPoint;					// ���
	CT_INT32	iEventID;				// ����¼�ID
	CT_INT32	iEventStatus;			// ����¼�״̬��0��δ��ɣ�1����ɣ�2����������ȡ
	CT_INT32	iThrowTimes;			// Ͷ������
	CT_INT32	iStartTimes;			// ���쾭����ʼ��Ĵ���
	CT_INT32	iStartChest;			// ��㱦�䣬û��Ϊ0
	CT_INT32	iGetTimes;				// �����������
	CT_INT32	iFT_ID;					// ����ʱ��¼��ս��ID
	CT_INT32	iScore;					// ս�ӻ���
	CT_INT32	iIsOpen;				// �����Ƿ���
	CT_INT32	iRewardTimes;			// �����ô���
	CT_INT32	iStep;					// Ͷ������
};

struct MSC_DB_IslandGiveUid 
{
	CT_DWORD	dwUserID;							// �û�ID
	CT_INT32	aGiveUid;							// �����͵�Uid
};

struct MSC_DB_IslandScoreReward
{
	CT_DWORD	dwUserID;							// �û�ID
	CT_INT32	aScoreReward;						// �ѻ�õ�ս�ӻ��ֱ���
};

struct MSC_DB_IslandPresentUserName
{
	CT_DWORD	dwUserID;							// �û�ID
	CT_CHAR		cUserName[NAME_SIZE+1];			// �û��ǳ�
};

struct MSC_DB_IslandGiveTimes 
{
	CT_DWORD	dwUserID;				//�û�ID
	CT_DWORD	dwFUID;					//����ID
	CT_DWORD	iGetTimes;				//���ѵ����ѻ�������
};

struct MSC_DB_IslandFTMemberScore
{
	CT_DWORD	dwUserID;				//�û�iD
	CT_INT32	iTeamID;				//ս��ID
	CT_INT32	iScore;					//����
};

//������־
struct MSC_LOGDB_IslandData 
{
	CT_DWORD	dwUserID;				// �û�ID
	CT_INT32	iPoint;					// ���
	CT_INT32	iEventID;				// ����¼�ID
	CT_INT32	iEventStatus;			// ����¼�״̬��0��δ��ɣ�1����ɣ�2����������ȡ
	CT_INT32	iThrowTimes;			// Ͷ������
	CT_INT32	iStartTimes;			// ���쾭����ʼ��Ĵ���
	CT_INT32	iGetTimes;				// �����������
	CT_INT32	iFT_ID;					// ����ʱ��¼��ս��ID
	CT_INT32	iScore;					// ս�ӻ���
	CT_INT32	iRewardTimes;			// ������ô���
};

//����ս��
struct MSC_Island_FightEvent 
{
	CT_DWORD		dwUserID;							//�û�ID
	CT_INT32		iSeed;								//�������
	CT_INT32		iMonsterID;							//������ID
	CT_INT32		iEventID;							//�¼�ID
	CT_INT32		iStatus;							//�¼����

	UserFightingStatus	FightingData;					//ս������
};

//��������
struct MSC_IslandTask
{
	CT_DWORD		dwUserID;							//�û�ID
	CT_INT32		iTaskID;							//����ID
};

//̽��

//�õ�̽�ú���/������
struct MSC_Seek_FriendGet
{
	CT_DWORD		dwUserID;							//�Լ�ID
	CT_DWORD		dwFUserID;							//̽���� ID
	CT_CHAR			cUserName[NAME_SIZE];				//̽���� �ǳ�
	CT_INT32		iOccupation;						//̽���� ְҵ
	CT_INT32		iLevel;								//̽���� �ȼ�
};

//��Ʊս��
struct MSC_KIDNAP_FIGHT
{
	CT_DWORD		dwUserID;							//�����û�ID
	CT_INT32		iSeed;								//�������
	CT_INT32        iWin;                               //�����Ƿ�Ӯ��
	CT_INT32		iType;								//�������
	CT_INT32		iPetChange;							//�û�ID

	UserFightingStatus	LeftFightingData[TEAM_NUM];					//ս������
	UserFightingStatus	RightFightingData[TEAM_NUM];					//ս������
};

//Ӫ��ս��
struct MSC_RESCUE_FIGHT
{
	CT_DWORD		dwUserID;							//�����û�ID
	CT_INT32		iSeed;								//�������
	CT_INT32        iWin;                               //�����Ƿ�Ӯ��

	UserFightingStatus	LeftFightingData[TEAM_NUM];					//ս������
	UserFightingStatus	RightFightingData[TEAM_NUM];					//ս������
};

//�Ӷ�ս��
struct MSC_PLUNDER_FIGHT
{
	CT_DWORD		dwUserID;							//�����û�ID
	CT_INT32		iSeed;								//�������
	CT_INT32        iWin;                               //�����Ƿ�Ӯ��
	CT_INT32		iBulidID;							//����ID
	CT_DWORD		dwDstUserID;						//Ŀ��ID

	UserFightingStatus	LeftFightingData[TEAM_NUM];					//ս������
	UserFightingStatus	RightFightingData[TEAM_NUM];					//ս������
};

//��ǽ�ӳ�
struct MSC_DB_WALL_ADDITION
{
	CT_DWORD	dwUserID;
	CT_INT32	iID;
	CT_INT32	iGrade;
};

//�����ǽ��Ϣ
struct MSC_DB_InsertWallAddition
{
	CT_DWORD	dwUserID;
	CT_INT32	iID;
	CT_INT32	iGrade;
};

//�����ǽ����
struct MSC_DB_InsertWallArmy
{
	CT_DWORD	dwUserID;
	CT_INT32	iPos[POST_COUNT/2];
};

//��ǽ����
struct MSC_DB_WALL_ARMY
{
	CT_DWORD	dwUserID;
	CT_INT32	iPos[POST_COUNT/2];
};

//�������˳���
struct MSC_DB_EnterOtherTown
{
	CT_DWORD dwUserID;
	CT_DWORD dwFUserID;									// ����ID
	CT_CHAR cUserName[NAME_SIZE];						// �û���
	CT_INT32 iOccupation;								// ְҵ
	CT_INT32 iLevel;									// �ȼ�
	CT_INT32 iDiamond;									// ��ʯ
	CT_INT32 iMoney;									// ���
	CT_INT32 iFood;										// ��ʳ
	CT_INT32 iWorker;									// ����
	CT_INT32 iMaxWorker;								// �����
	CT_INT32 iStamina;									// ����
	CT_INT32 iVip;										// Vip
	CT_INT32 iFightingPower;							// ս����
	TeamSeek aTeam[SEEK_TEAM_FLAG_NUM];					// ս��
	KidnapInfo aSaintessInfo[SEEKED_SPITE_TIMES];		// ʥŮ������Ϣ
	KidnapInfo aFarmerInfo[SEEKED_SPITE_TIMES];			// ũ��������Ϣ
	KidnapInfo aPetInfo[SEEKED_SPITE_TIMES];				// ������Ʊ��Ϣ
	CT_INT32 iPetID[SEEK_KIDNAP_PET];					// ʣ��ĳ���ID
	BuildData			building[SEEK_BUILD_NUM];			// ����������Ϣ
	UpgradingBuildData	upgradeBuilding[SEEK_BUILD_NUM];	// �����Ľ���
	ProductingBuilding	productBuilding[SEEK_BUILD_NUM];	// �����Ľ���
	CT_INT32 iOpenFunction[MAX_FUCTION_NUM];			// �ѿ����Ĺ���
	CT_INT32 iSaintess;									// ����ܵ�ʥŮ��
};

//��ȡ��̽������Ϣ
struct MSC_DB_SeekedGet
{
	CT_DWORD dwUserID;		// �Լ�ID
	CT_DWORD dwFUserID;		// ��̽����ID
	CT_INT32 iType;			// ̽������
	CT_INT32 iSeekedWillTimes;							// ���ƷõĴ���
	CT_INT32 iSeekedSpiteTimes;							// ����õĴ���
};

//�õ���̽������Ϣ
struct MSC_DB_OtherSeekedInfo
{
	MSC_DB_SeekedGet	seekInfo;						// ̽����Ϣ
	MSC_DB_EnterOtherTown townInfo;						// ������Ϣ
};

//������̽�õ����
struct MSC_DB_Had_SeekedPlayer
{
	CT_DWORD			dwUserID;
	CT_DWORD			dwFUserID;
	CT_INT32			iType;
};

//�����ߵı��ô�������
struct MSC_DB_Seeked_TimesMinus 
{
	CT_DWORD			dwFUserID;
	CT_INT32			iType;
};

struct MSC_DB_Seeked_Info 
{
	CT_DWORD dwUserID;	// ����ID
	CT_INT32 iDiamond;										// ��ʯ
	CT_INT32 iMoney;										// ���
	CT_INT32 iFood;											// ��ʳ
	CT_INT32 iWorker;										// ����
	CT_INT32 iMaxWorker;									// �����
	CT_INT32 iKidnapWorkerNum;								// ����ܵĹ���
	CT_INT32 iKidnapSaintess;								// ʥŮ�Ƿ񱻰��
	CT_INT32 iPetKidnap[SEEK_KIDNAP_PET];					// ������
	CT_INT32 iSeekedWillTimes;								// ���ƷõĴ���
	CT_INT32 iSeekedSpiteTimes;								// ����õĴ���
	TeamSeek teamFlag[SEEK_TEAM_FLAG_NUM];					// ս��
	BuildData			buildingData[SEEK_BUILD_NUM];		// ����������Ϣ
	UpgradingBuildData	upgradeBuilding[SEEK_BUILD_NUM];	// �����Ľ���
	ProductingBuilding	productBuilding[SEEK_BUILD_NUM];	// �����Ľ���
	SeekPresent			present[SEEKED_PRESENT_NUM];		// �õ�����Ʒ��Ϣ
	CT_INT32 iTime;											// ����ʱ��
};

// ս��
struct MSC_DB_SeekTeamFlag
{
	CT_DWORD dwUserID;					
	TeamSeek teamFlag;						//������Ϣ
};

//����
struct MSC_DB_SeekKidnap
{
	CT_DWORD dwUserID;
	CT_INT32 iType;							//��������
	KidnapInfo kidnap;						//������Ϣ
};

//̽���б���¼ʱ��ȡ��
struct MSC_DB_SeekListPlayer
{
	CT_DWORD dwUserID;
	UserInfo playerInfo;					//�б������Ϣ
};

//̽�û�������
struct MSC_DB_SeekData
{
	CT_DWORD dwUserID;
	SeekData seekData;						//̽����Ϣ
};

//̽��������
struct MSC_DB_SeekPresent 
{
	CT_DWORD dwUserID;
	SeekPresent present;					//�յ�������
};

//̽�ý�������б�
struct MSC_DB_SeekPlayerList
{
	CT_DWORD dwUserID;					
	UserInfo aWillPlayer[SEEK_PLAYER_NUM];	//�����б�����ID
	UserInfo aSpitePlayer[SEEK_PLAYER_NUM];	//�����б�����ID
};

//���ü�¼
struct MSC_DB_SeekVistorInfo 
{
	CT_DWORD	dwUserID;					//
	VistorInfo	vistorInfo;					//�����ߵļ�¼
};

typedef SeekFightingData MSC_DB_SeekFightRecord;

//�Ӷ���Դ
struct MSC_DB_PlunderRes
{
	CT_DWORD	dwUserID;
	CT_INT32	iPlunderScore;				//�Ӷ����
};

// ���а���
struct MSC_DB_TopReward
{
	CT_DWORD	dwUserID;					
	CT_INT32	iType;						//���а�����
	CT_INT32	iStatus;					//����״̬
};

#  pragma pack(pop)   /* ͨѶЭ��ṹ���붨���ڴ���֮ǰ */
#endif  // CTHREADMES_H_

