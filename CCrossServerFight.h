/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CTeamDuplicate.h
Version:		1.0
Description:    ���ս
Author:			wangchen
Create Date:	2014-2-17
History:             

******************************************************************/

#ifndef CCCROSSSERVERFIGHT_H_
#define CCCROSSSERVERFIGHT_H_
#include "CTType.h"
#include "CGDefine.h"
#include "CMD_Game.h"
#include "../GameServer/CUserPVPInfo.h"
#include "../GameServer/CFightTeam.h"

class CCMutex;
struct UserFightingStatus;
struct CSInspore;
//8ǿս������λ��
/*
	1				3
		5	7	6	
	2				4
*/
enum CSFightRecordType
{
	Current_Record		= 0,			//��ǰ��¼
	Last_Rcord			= 1,			//�ϴμ�¼
};

enum CSStatus
{
	Final_Eight			= 0,			//8ǿǰ
	Final_Four			= 1,			//4ǿǰ
	Final_Two			= 2,			//�����ǰ
	Final_One			= 3,			//����
	Seat_Fight			= 4,			//�ʸ���
	Seat_Fight_Befor	= 5,			//�ʸ���ǰ
	CS_Close			= 255,			//�ر�״̬
};

enum CSFightRecordIndex
{
	Current_Index		= 1,			//��ǰ����
	Last_Index			= 8,			//��һ������
};

class CCCrossServerFight
{
public:
	struct FinalEightData
	{
		FinalEight			fe;											//λ����Ϣ
		CT_CHAR				cUserName[CLAN_CRUSADE_MEMBER][NAME_SIZE];	//������Ա����
		CT_DWORD			dwUserID[CLAN_CRUSADE_MEMBER];				//������Աid
		UserFightingStatus	ufs[CLAN_CRUSADE_MEMBER];					//ս������
		CSInspore			csi[CLAN_CRUSADE_MEMBER];					//��������
		CT_CHAR cIndex[CROSS_SERVER_RECORD_INDEX+1];					//ս������(8ǿ��Ҫ)
	};

	struct CSFightRecord
	{
		CT_CHAR cIndex[CROSS_SERVER_RECORD_INDEX+1];
		CT_CHAR cFightTeamName[2][NAME_SIZE+1];
		CT_INT32 iGameServerID[2];
		CT_DWORD dwFTID[2];
		CT_INT32 iWiner;
		CT_INT32 iSeed;
		UserFightingStatus ufs[2][CLAN_CRUSADE_MEMBER];
	};

	struct CSSupport
	{
		CT_INT32	iResID;
		CT_INT32	iNum;
		CT_INT32	iPos;
		CT_INT32	iStatus;
	};

	//ս������
	struct RecordIndex
	{
		CT_DWORD	dwFTID;
		CT_CHAR		cIndex[CROSS_SERVER_RECORD_INDEX+1];
	};

	typedef std::map<CT_INT32,FinalEightData>			FinalEightDataMap; //λ��Ϊkey
	typedef std::map<CT_INT64,CSSeatFightReady>			SeatFightTeamMap;	//�ʸ�սս��
	typedef std::map<std::string,CSFightRecord>			FightRecordMap;	//md5��key
	typedef std::list<RecordIndex>						RecordList;		//ս������
	typedef std::map<CT_DWORD,RecordList>				CSFightRecordMap;//ս��ID
	typedef std::map<CT_INT64,CSSupport>				CSSupportDataMap;//�׶�Ͷע��Ϣ

private: 
	CCMutex						m_Mutex;
	FinalEightDataMap			m_FinalEightData;			//��ǰ8��λ�����ݣ�������8ǿս��
	FinalEightDataMap			m_LastFinaEightData;		//����8��λ��
	CSFightRecordMap			m_FinalEightRecord;			//��ǰ8��λ��ս����¼���ʸ�ս,��Ϸ����������棩
	CSFightRecordMap			m_LastFinalEightRecord;		//����8��λ��ս����¼���ʸ�ս,��Ϸ����������棩
	FightRecordMap				m_FightRecord;				//ս����¼
	SeatFightTeamMap			m_SeatFightTeam;			//�ʸ��б�
	CSSupportDataMap			m_SupportData;				//֧����Ϣ(��ǰ״̬0)

public:
	CCCrossServerFight(void);
	~CCCrossServerFight(void);

	CCCrossServerFight(const CCCrossServerFight &cssf){};

public:
	//�����ʸ�ս��
	CT_VOID InsertSeatFightTeam(CSSeatFightReady &cssfr);
	//����8ǿ����
	CT_VOID InsertFinalEightData(CT_INT32 iIndex,FinalEightData &fed);
	//�����ʸ�ս����¼
	CT_VOID InsertCSFightRecord(CT_INT32 iIndex,CSFightRecord &csfr);
	//�����ʸ�ս����¼
	CT_VOID InsertCSFightRecordUser(CT_CHAR cIndex[],CT_INT32 iGroup,CT_INT32 iPos,UserFightingStatus &ufs);
	//����8ǿս����¼(�������������ⶨ��1-7:��ǰ8ǿ 8-14���̻ع�)
	CT_VOID InsertCSFinalEightRecord(CSFightRecord &csfr);
	//����8ǿս����¼�û�����
	CT_VOID InsertCSFightEightRecordUser(CT_CHAR cIndex[],CT_INT32 iGroup,CT_INT32 iPos,UserFightingStatus &ufs);
	//����֧����Ϣ
	CT_VOID InsertCSSupportData(CT_INT32 iStatus,CSSupport &css);


public:
	//��ȡ�ʸ���ս��
	CT_BOOL GetSeatFightTeam(CT_CHAR *pBuf,CT_INT32 &iLen);
	//��ȡ8ǿ����
	CT_BOOL GetFinalEightData(CT_INT32 iIndex,CT_CHAR *pBuf,CT_INT32 &iLen);
	//��ȡ8ǿ����
	CT_BOOL GetFinalEightData(CT_INT32 iIndex,CT_INT32 iPos,FinalEightData &fed);
	//��ȡ8��λ��ս��
	CT_BOOL GetCSFightRecord(CT_INT32 iIndex,CT_DWORD dwFTID,CT_CHAR *pBuf,CT_INT32 &iLen);
	//��ȡ8ǿս��
	CT_BOOL GetCSFightRecord(CT_CHAR cIndex[],CT_INT32 iIndexLen,CSFightRecord &csfr);
	//��ȡս���ʸ�սս��
	CT_BOOL GetCSFightRecord(CT_DWORD dwFTID,CT_CHAR *pBuf,CT_INT32 &iLen);
	//���֧��
	CT_BOOL GetCSSupportData(CT_CHAR *pBuf,CT_INT32 &iLen);
	//���֧��
	CT_BOOL GetCSSupportData(CT_INT32 iStatus,CT_INT32 iPos,CSSupport &css);
	//��ȡս����Ϣ
	CT_BOOL GetFTInfo(CT_DWORD dwFTID,CT_INT32 iGSID,FinalEightData &fed);

public:
	//�����û���Ϣ
	CT_VOID UpdateUserData(CT_INT32 iGSID,UserFightingStatus  &ufs,CSInspore &csi);
	//��������Ϣ
	CT_VOID UpdateFTPos(CT_DWORD dwFTID,CT_INT32 iGSID,UserFightingStatus *pufs,CT_INT32 iCount=CLAN_CRUSADE_MEMBER);

public:
	//
	CT_INT64 Get64Int(CT_INT32 high,CT_INT32 low);
	//�±���
	CT_VOID NewMatch();
	//��ȡ����ս������
	CT_VOID GetMasterFightingData(const UserFightingStatus &data,CombatUnitData &masterdata);
};
#endif
