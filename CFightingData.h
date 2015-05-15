/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CFightingRecord.h
Version:		1.0
Description:    ս����Ϣ
Author:			wangchen
Create Date:	2013-10-15
History:             

******************************************************************/

#ifndef CCFIGHTINGDATA_H_
#define CCFIGHTINGDATA_H_
#include "CSingleton.h"
#include "CTType.h"
#include "GlobeScriptInterface.h"
#include "CGDefine.h"
#include "CThreadMes.h"
#include<map>

//�û�ս������
//struct FightingData
//{
//	UserFightingStatus			FData;
//	LookUserInfo				LookData;
//	CT_INT32					iTime;			//���ݴ���ʱ��
//};

typedef MSC_SUB_UserdataInsert FightingData;

typedef MSC_DB_Seeked_Info Seekers;

//��ǰ̽�õ�����Ϣ
//struct Seekers
//{
//	CT_DWORD dwUserID;	// ����ID
//	CT_INT32 iDiamond;										// ��ʯ
//	CT_INT32 iMoney;										// ���
//	CT_INT32 iFood;											// ��ʳ
//	CT_INT32 iWorker;										// ����
//	CT_INT32 iMaxWorker;									// �����
//	CT_INT32 iKidnapWorkerNum;								// ����ܵĹ���
//	CT_INT32 iKidnapSaintess;								// ʥŮ�Ƿ񱻰��
//	CT_INT32 iPresentNum;									// �����Ʒ����
//	CT_INT32 iSeekedWillTimes;								// ���ƷõĴ���
//	CT_INT32 iSeekedSpiteTimes;								// ����õĴ���
//	TeamFlag TeamFlag[SEEKED_SPITE_TIMES];					// ս��
//	BuildData			buildingData[SEEK_BUILD_NUM];		// ����������Ϣ
//	UpgradingBuildData	upgradeBuilding[SEEK_BUILD_NUM];	// �����Ľ���
//	ProductingBuilding	productBuilding[SEEK_BUILD_NUM];	// �����Ľ���
//};


class CCMutex;
class CCFightingData: public CCSingleton<CCFightingData>
{
private:
	typedef std::map<CT_DWORD,FightingData>					FightingMap;
	typedef std::map<CT_DWORD,FightingData>::iterator		FD_IT;
	typedef std::map<CT_DWORD,Seekers>						SeekersMap;

	CCMutex				m_Mutex;
	FightingMap			m_FightingDataMap;
	SeekersMap			m_SeekerMap;							// ��ǰ̽���˵���Ϣ

public:
	CCFightingData(void);
	~CCFightingData(void);

public:
	//��������
	CT_VOID InsertData(CT_CHAR *pBuf,CT_INT32 iLen);
	//ɾ������
	CT_VOID DeleteData();

	//����̽������
	CT_VOID InsertSeekedInfo(MSC_DB_Seeked_Info* pInfo);

public:
	//��������
	CT_VOID UpdataFightingData(const CT_DWORD dwUserID);
	/* ��ȡ����
	 * �����û���ȡ��ǰ����
	 * �����û���ȡ�Ѵ�����
	 * ������ˢ�£���Ӹ����Զ�ƥ�������ݿ��ѯ,��ʱ��֧�������ѯ
	 */
	CT_BOOL GetFightingData(const CT_DWORD dwUserID,FightingData &fd);
	CT_BOOL GetFightingData(const CT_DWORD dwUserID,UserFightingStatus &fd,CT_BOOL bSeekFight=CT_FALSE);
	//ɾ��boss
	CT_VOID DeleteBoss(const CT_DWORD dwUserID);

	//�õ�̽��������(�������ݲ������)
	CT_BOOL GetSeekedInfo(CT_DWORD dwUserID, Seekers& seekersInfo);
	//����̽���ߴ���
	CT_BOOL UpdateSeekedTimes(CT_INT32 iType, const Seekers& seekersInfo); 
	//��֤����ID
	CT_BOOL GetBuildData(const CT_DWORD dwUserID,const CT_INT32 iID,BuildData &bd);
	//����̽��������Ϣ�����ݿ�(����������,���ģ�����)
	CT_VOID UpdateSeekersBaseInfo(const CT_DWORD dwUserID,Seekers &seekers);
	//�õ��ѻ�õ��������
	CT_INT32 GetPresentNum(CT_DWORD dwUserID);
	//�õ�����
	CT_VOID GetPresent(CT_DWORD dwUserID, CT_INT32 iChestID);
	//�õ����������Ľ�����Ϣ
	CT_BOOL GetBuildingProduct(CT_DWORD dwUserID, CT_INT32 iBID, ProductingBuilding& proBuilding);
	//�������������Ľ�����Ϣ
	CT_VOID UpdateBuildingProduct(CT_DWORD dwUserID, CT_INT32 iBID, ProductingBuilding& proBuilding);
	//�õ�������������
	CT_BOOL GetUpgradeBuilding(CT_DWORD dwUserID, CT_INT32 iBID, UpgradingBuildData& upgBuilding);
	//����������������
	CT_VOID UpdateUpgradeBuilding(CT_DWORD dwUserID, CT_INT32 iBID, UpgradingBuildData& upgBuilding);
	//����ս����Ϣ
	CT_VOID InsertTeamFlag(CT_DWORD dwUserID, TeamSeek& ts);
	//ɾ��ս��
	CT_VOID DeleteTeamFlag(CT_DWORD dwUserID);
	//����������Ϣ
	CT_VOID UpdateSeekPresent(CT_DWORD dwUserID, SeekPresent& sp);

public:
	//��������ʤ�޸�
	CT_VOID SetPVPConWin(CT_DWORD dwUserID);

private:
	//��ȡ������Ϣ
	CT_BOOL GetLookUserInfo(PTRLinkUserData &linkUserData,UserFightingStatus &Fightinginfo,LookUserInfo &Lookinfo);
};
#endif
