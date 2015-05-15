/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCSFightData.h
Version:		1.0
Description:    ���ս
Author:			wangchen
Create Date:	2014-2-17
History:             

******************************************************************/

#ifndef CCCSFIGHTDATA_H_
#define CCCSFIGHTDATA_H_
#include "CTType.h"
#include "CGDefine.h"
#include "CMD_Game.h"
#include "CThreadMes.h"
#include "CCrossServerFight.h"


enum CSSeat
{
	Seat_False			= 0,			//���ʸ�
	Seat_True			= 1,			//���ʸ�
};

enum CSPosInfo
{
	Pos_Null			= 0,			//λ�ÿ�
	Pos_Me				= 1,			//λ���Լ�
	Pos_Other			= 2,			//λ��������
};

enum CSFinalWin
{
	CS_LOST				= 1,			//
	CS_WIN				= 2,			//
};

class CCCSFightData : public CCCrossServerFight,public CCSingleton<CCCSFightData>
{
public:
	struct CSUserSupport
	{
		CT_DWORD	dwUserID;
		CT_INT32	iResID;
		CT_INT32	iNum;
		CT_DWORD	dwFTID;
		CT_INT32	iGSID;
		CT_INT32	iIndex;
	};

	struct CSUserReward
	{
		CT_INT32	iType;
		CT_INT32	iResID;
		CT_INT32	iNum;
	};
public:
	typedef std::map<CT_INT64,CSUserSupport>								CSSupportMap;	//����֧��
	typedef std::map<CT_DWORD,CT_INT64>										CSFTCoolTimeMap;//��ȴʱ��
	typedef std::map<CT_INT64,CSUserReward>									CSRewardMap;
	typedef std::map<CT_DWORD,CSRewardMap>									CSUserRewardMap;
private:
	CCMutex					m_Mutex;
	CSSupportMap			m_CSSupport;						//����֧��
	CSFTCoolTimeMap			m_CSFTCoolTime;						//��ȴʱ��
	CSUserRewardMap			m_CSUserReward;						//�û�����
	CT_INT32				m_iSeat;							//�ʸ�
	CT_INT32				m_iStatus;							//״̬
public:
	CCCSFightData(void);
	~CCCSFightData(void);

public:
	//�������֧��
	CT_VOID InsertSupport(CSUserSupport &csus);
	//������ȴʱ��
	CT_VOID InsertCoolTime(CT_DWORD dwFTID,CT_INT64 lTime);
	//�����ʸ�
	CT_VOID InsertSeat(CT_INT32 iSeat);
	//���뽱��
	CT_VOID InsertReward(CT_DWORD dwUserID,CSUserReward &csur,CT_BOOL bSave=CT_TRUE);
	
	
public:
	//����״̬
	CT_VOID SetStatus(CT_INT32 iStatus){_AUTOMUTEX(&m_Mutex);m_iStatus=iStatus;};

public:
	//��ȡ����֧��
	CT_BOOL GetSupport(CT_DWORD dwUserID,CT_INT32 iStatus,CSUserSupport &csus);
	//��ȡ��ȴʱ��
	CT_INT64 GetFTCoolTime(CT_DWORD dwFTID);
	//��ȡ�ʸ�
	CT_INT32 GetSeat(CT_DWORD dwFTID);
	//��ȡ�ʸ�
	CT_INT32 GetSeat(){_AUTOMUTEX(&m_Mutex);return m_iSeat;};
	//��ȡ����λ��
	CT_INT32 GetOpponentPos(CT_DWORD dwFTID,CT_INT32 iGSID,CT_INT32 iStatus);
	//��ȡ״̬
	CT_INT32 GetStatus(){_AUTOMUTEX(&m_Mutex);return m_iStatus;};
	//��ȡ����
	CT_BOOL GetUserReward(CT_DWORD dwUserID,CT_CHAR *pBuf,CT_INT32 &iLen);
	//֧�ֽ�������
	CT_VOID SupportReward();
	//8ǿ��������
	CT_VOID FinalEightReward(CT_INT32 iWin,CT_DWORD dwUserID);
	//��ȡս���ʸ�սս��
	CT_BOOL GetCSFightRecord(CT_DWORD dwFTID,CT_INT32 iGSID,CT_CHAR *pBuf,CT_INT32 &iLen);
	//��ȡ8ǿս��
	CT_BOOL GetCSFightRecord(CT_CHAR cIndex[],CT_INT32 iIndexLen,CSFightRecord &csfr);

public:
	//���֧������
	CT_VOID ClearSupport(){_AUTOMUTEX(&m_Mutex);m_CSSupport.clear();};
	//�����������
	CT_VOID ClearReward();
	//�������
	CT_VOID ClearReward(CT_DWORD dwUserID);
};
#endif
