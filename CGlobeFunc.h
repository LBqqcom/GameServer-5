/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CGlobeFun.h
Version:		1.0
Description:    ȫ�ֺ���
Author:			pengwen
Create Date:	2012-8-20
History:        �����û��¼�     

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

extern CCNetInterface*  g_pPCentServer;				//���ķ�������ӿ�
extern CCNetInterface*  g_pPChargeServer;           // ��ֵ��������ӿ�
extern CCNetInterface*  g_pCorssServer;				//�������
extern CT_INT32		    g_iServerID;				//����ID
extern CT_INT64			g_lHeartBeat;				//����ʱ��
extern CT_INT64			g_lOpenServerTime;			//������ʱ��
extern CT_INT32			g_iDBUpdataValue;			//���ݿ���·�ֵ
extern CT_INT32			g_iGlobeData[GLOBEDATA_TYPE_MAX];	//ȫ������0:���� 1:��3

//���ݿ��ȡд���¼
struct ReadCount
{
	CT_INT64		lId;
	CT_INT32		iMainID;
	CT_INT32		iSubID;
	CT_INT32		iCount;	
};

//��������
struct RewardCost
{
	CT_INT32			iUseResType[TEAM_RES_CNT];			//ʹ����Դ����
	CT_INT32			iUseResCount[TEAM_RES_CNT];			//ʹ����Դ��Ŀ
	CT_INT32			iCurrentResCount[TEAM_RES_CNT];		//��ǰ��Դ��Ŀ
	CT_INT32			iUseItem[GOODS_COUNT*2];			//ʹ����Դ����
	CT_INT32			iUseItemCount[GOODS_COUNT*2];			//ʹ����Դ��Ŀ
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

//��Ϣ��������¼
CT_VOID G_DisposeErrorMes(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pErrorScr);

//�����߳���Ϣ
CT_VOID G_SendThreadInfo(CT_DWORD threadID,  CT_DWORD mainID, CT_DWORD subID, const CT_CHAR* pBuf, CT_INT32 len);

//����db����
CT_VOID G_DBError(CCDBConnector& dbConnector, CT_DWORD dwThread);

//����dbͨ����Ϣ
CT_VOID G_SendThreadDbStandardInfo  (CT_DWORD threadID, CT_DWORD mainID, CT_DWORD subID,  CT_DWORD dwUser, const CCNetInterface* pNetInter);

//���ͽ���ͨ����Ϣ
CT_VOID G_SendThreadDBBuildingInfo  (CT_DWORD threadID, CT_DWORD mainID, CT_DWORD subID,  CT_DWORD dwUser, CT_INT32 iBuildID, const CCNetInterface* pNetInter);

//��������������Դ����
CT_VOID G_SaveMastVariableData(CCUser* pUser, ENUM_BaseDataSaveTag saveTag);
//�����û����ݱ���
CT_VOID G_SaveMastvariableData(MSC_DB_MasterSaveData &msd);

//��ȡ����
CT_INT32 G_GetArmyID(CT_INT32 iArmyID,CT_INT32 iGrade);

//��ȡ������������
CT_INT32 G_GetStatminaCost(CCUser *pUser);

//�������Ѽ�¼
CT_BOOL G_SaveStatminaCost(CCUser *pUser);

// �ַ�ʱ���ʽת��Ϊ����
CT_INT64 G_StrTimeToSeconds(CT_CHAR *strTime);

//timeתʱ���ʽ
CT_VOID G_SecondsToTime(CT_CHAR *strTime,CT_INT64 lTime);

//
CT_VOID G_ReadCount();

//�����ؿ�KEY
CT_INT32 G_GetKeyValue(int value0, int value1, int value2);

//��ȡΪ��������
CT_VOID G_GetFakeProDta(CCUser *pUser,CT_INT32 iTage,FakeProBase &fb);
//����Ϊ�������ݣ����߳�ȡ��ִ�д˺�����
CT_VOID G_ClearFakeProDat(CCUser *pUser,CT_INT32 iTage,FakeProBase &fb);
//Ϊ���ʼ���
CT_INT32 G_GetFakeProCount(CT_INT32 iID,FakeProBase &fb);
//�������ȡ
CT_INT32 G_GetGoodsGroupInfo(const CT_INT32 iGroupID,CT_INT32 iGoods[GOODS_COUNT],CT_INT32 iCount[GOODS_COUNT],FakeProBase &fb);


//��ȡ���ս������
CT_VOID G_GetFightingData(CCUser *pUser,UserFightingStatus &UserData,CT_BOOL bSeekFight=CT_FALSE);
//��ȡ����ս������
CT_VOID G_GetMasterFightingData(const UserFightingStatus &data,CombatUnitData &masterdata);
//��ȡ������Ϣ
CT_BOOL G_GetLookUserInfo(PTRLinkUserData &linkUserData,UserFightingStatus &Fightinginfo,LookUserInfo &Lookinfo,CT_BOOL bSeekFight=CT_FALSE);
//��Դ����ת��
CT_BOOL G_ChangeResType(MasterVariableData &mvd, const CT_INT32 iResType, CT_INT32& iRes);
//��ȡ̽����Ϣ
CT_BOOL G_GetSeekerData(CCUser* pUser,MSC_DB_Seeked_Info& seeker);

//�����������Ĵ�����
enum ENUM_CostRewardErrorCode
{
	ENUM_CREC_RES_SUC       = 0,       //�ɹ�
	ENUM_CREC_RES_FAIL      = 1,	   //��Դ����
	ENUM_CREC_ITEM_FAIL     = 2,	   //�������⣬������߸��Ӳ���
	ENUM_CREC_ELMBAG_NOAREA = 4,	   //Ԫ�ر�������
	ENUM_CREC_NO_ID_FAIL    = 8        //id�Ҳ�����Ӧ�Ľ���
};

//ִ�еķ�ʽ
enum ENUM_PerformCostRewardType
{
	ENUM_PCRT_TEST = 1,              //����
	ENUM_PCRT_PERFORM = 2,           //ִ��
	ENUM_PCRT_TEST_PERFORM=3,       //���Բ�ִ��
};

//��ӽ�����������
ENUM_CostRewardErrorCode G_PerformCostReward(CT_INT32 iRewardID, CCUser* pUser, ENUM_PerformCostRewardType type, CMD_G_NormalReward& addReward, CT_INT32* pGoods, CT_INT32* pCount,FakeProBase &fb);

//��ӽ�����������
//������׼ Resoure res[TEAM_RES_CNT], Item item[TEAM_ITEM_CNT], CT_INT32 iGoods[GOODS_COUNT*2 ], CT_INT32 iCount[GOODS_COUNT*2 ])
CT_VOID G_GetReward(CT_INT32 iRewardID, Resoure* pRes, Item* pItem, CT_INT32* pGoods, CT_INT32* pCount,FakeProBase &fb);

//��ӽ�������
CT_INT32 G_GetCostReward(CT_INT32 iID,CCUser *pUser,const CT_DWORD dwtype,const CT_DWORD saveTag,RewardCost &rc,FakeProBase &fb);

// �����û����
CT_INT32 G_Random(CT_DWORD dwUserID, CT_INT32 iParam1, CT_INT32 iParam2);

CT_VOID G_SendClientData(CCUser &user, CT_DWORD mainID, const CT_DWORD subID, CT_CHAR *buf, CT_INT32 bufLen );

//����ս��
CT_VOID G_SaveFightingRecord(CCDBConnector& m_dbConnector,UserFightingStatus &FightingData,CT_INT32 iPos,CT_INT32 iSeed);
//��ȡս��
CT_VOID G_GetFightingRecord(CCDBResult* pResult,CombatUnitData &data,CT_INT32 &iSceneStar,CT_INT32 &iSeed,CT_INT32 &iIndex,CT_DWORD &dwUserID);

//��������ƴ��
CT_BOOL G_GetRollBroadecastData(CT_INT32 iID,CT_CHAR *pOutBuf,CT_INT32 iOutLen,CT_CHAR *pInBuf,CT_INT32 iInBufLen);
//���͹���
CT_VOID G_SendRollBroadecast(CT_INT32 iID,CT_INT32 iBroadcastType,CT_INT32 iBroadcastTime,CT_CHAR *pInBuf,CT_INT32 iInBufLen);

//���ͳ����߳���Ϣ
CT_VOID G_SendCityThreadInfo(CT_DWORD dwUserID,CT_DWORD mainID, CT_DWORD subID, const CT_CHAR* pBuf, CT_INT32 len);
//����ս���߳���Ϣ
CT_VOID G_SendFightThreadInfo(CT_DWORD dwUserID,CT_DWORD mainID, CT_DWORD subID, const CT_CHAR* pBuf, CT_INT32 len);
//����DB�߳���Ϣ
CT_VOID G_SendDBThreadInfo(CT_DWORD dwUserID,CT_DWORD mainID, CT_DWORD subID, const CT_CHAR* pBuf, CT_INT32 len);
//����LOGDB�߳���Ϣ
CT_VOID G_SendLogDBThreadInfo(CT_DWORD dwUserID,CT_DWORD mainID, CT_DWORD subID, const CT_CHAR* pBuf, CT_INT32 len);

//ս�����Լӳ�
CT_VOID G_AddFightAttribute(MasterCombatStatus &mcs,const CT_INT32 iType,const CT_FLOAT fValue,const CT_FLOAT fValuePercent=1.0f);
//��ȡս������ֵ
CT_INT32 G_GetMasterCombatStatusValue(CT_INT32 iType,MasterCombatStatus &mcs);

//���ʹ�����Ϣ
CT_VOID G_SendErrorInfo(CCUser *pUser,CT_INT32 iType, CT_INT32 iID);

#endif
