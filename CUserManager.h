/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CUserManager.h
Version:		1.0
Description:    �û�����
Author:			pengwen
Create Date:	2012-8-8
History:        �����û�     

******************************************************************/
#ifndef CCUSERMANAGER_H_
#define CCUSERMANAGER_H_
#include <map>
#include <string>
#include "CNetInterface.h"
#include "CSingleton.h"
#include "CUser.h"
#include "CMD_Game.h"
#include "CMutex.h"

//�û�������
class CCUserManager : public CCSingleton<CCUserManager>
{
	
public:
	typedef std::map<CT_DWORD, CCUser*>				UserDataMap;
	typedef std::map<CT_DWORD, CCUser*>::iterator	UDM_IT;

	typedef std::map<CCNetInterface*, CT_DWORD>      UserIDFindMap;
	typedef std::map<CCNetInterface*, CT_DWORD>::iterator UFM_IT;

	typedef std::map<std::string, CCUser*>				UserDataNameMap;
	typedef std::map<std::string, CCUser*>::iterator	UDNM_IT;
private:
	//�û���Ϣ��
	UserDataMap				m_userDataMap;
	//�û�id���ұ�
	UserIDFindMap			m_userFindMap;
	//�û��ǳƱ�
	UserDataNameMap			m_userDataNameMap;
	//������
	CCMutex					m_mutex;  
	CT_CHAR* m_pBuffer;
	std::set<CT_DWORD>		m_GMList;

public:
	CCUserManager(CT_VOID);
	~CCUserManager(CT_VOID);

public:
	//�õ�һ���û�����
	CCUser* GetOneFreeUser(CT_DWORD userID, CCNetInterface* pNetInterface);
public:
	//����ͨѶ���
	CT_BOOL SetNetHandle(CT_DWORD userID, CCNetInterface* pNetHandle);
	//�������� �κ��˲��øĶ����������ʵ�֣�������������⣩����
	CT_BOOL Find(CT_DWORD id);
	//�������� �κ��˲��øĶ����������ʵ�֣�������������⣩����
	CT_BOOL Find(CCNetInterface* pNetHandle);
	//�������� �κ��˲��øĶ����������ʵ�֣�������������⣩����
	CT_BOOL Find(CT_DWORD id, CCUser** p);
	//�������� �κ��˲��øĶ����������ʵ�֣�������������⣩����
	CT_BOOL Find(CCNetInterface* pNetHandle, CCUser**  us);
	//ͨ���ǳƲ��� �κ��˲��øĶ����������ʵ�֣�������������⣩����
	CT_BOOL Find(std::string cNickName, CCUser** us);
	//�������
	CT_VOID Erase(CT_DWORD id);
	//���
	CT_VOID Clear();
	//��ȡ�û��ǳ�idmap
	CT_VOID InsertNickNameMap(const std::string& sNickName,CCUser *pUser){_AUTOMUTEX(&m_mutex); m_userDataNameMap.insert(UserDataNameMap::value_type(sNickName,pUser));};
	//��ȡ�û���Ŀ
	CT_INT32 GetUserCount(){return m_userDataMap.size();};

public:
	//���������Ľ���
	CT_VOID UpdateUpgradingBuilding(CT_INT32 threadIndex, CT_INT32 threadTotalNum, CT_DWORD dwThreadID);
	//ͬ���������ݵ����ݿ�
	CT_VOID UpdateSaveBuildingData(CT_INT32 threadIndex, CT_INT32 threadTotalNum,  CT_DWORD dwThreadID);
	//ͬ���������ݵ����ݿ�
	CT_VOID UpdateSaveProductingData(CT_INT32 threadIndex, CT_INT32 threadTotalNum,  CT_DWORD dwThreadID);

	//���������Ľ���
	CT_VOID UpdateProductingBuilding(CT_INT32 threadIndex, CT_INT32 threadTotalNum);
	//�����¼�
	CT_VOID UpdateEvent(CT_INT32 threadIndex, CT_INT32 threadTotalNum);
    // ������������ [10/10/2012 John]
    CT_VOID UpdateItemBuff();
    // ����������ȴ[10/10/2012 John]
    CT_VOID UpdateCooldown();
    // ͬ������������Ϣ
    CT_VOID SyncItemData();
	// ��������������Ϣ
	CT_VOID SendWorldChat(CMD_G_WorldChat *pWorldChat,CT_DWORD len);
	// ���͹㲥��Ϣ
	CT_VOID BroadCastWorldMessage(CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID BroadCastMessage(CT_DWORD mainID, CT_DWORD subID,CT_CHAR *pBuf,CT_INT32 iLen);
	//����GM��Ϣ
	CT_VOID SendGMMessage(CT_DWORD mid, CT_DWORD sid,  CT_CHAR* pMes,CT_INT32 len,CT_DWORD dwMeUserID=0);
	//ϵͳ��Ϣ
	CT_VOID SendServerMessage(CMD_G_GMMessage &pMes,CT_INT32 len,CT_DWORD dwMeUserID=0);
    // ˢ��ǩ����Ϣ
    CT_VOID UpdateSignIn(CT_INT32 threadIndex, CT_INT32 threadTotalNum);
	//ˢ���̵굥��������Ʒ��Ϣ
	CT_VOID UpdateSingleGoodsStatus(CMD_G_MarketGoodsInfo &pGoodsInfo);
	//��������
	CT_VOID UpdateHeartBeat();
	//���¾�����
	CT_VOID UpdataPVPDegree();
	//��ʱˢ���̵꣬�̳���Ʒ
	CT_VOID RefreshMarketGoods();
	//��ʱˢ�ºڵ���Ʒ
	CT_VOID RefreshBlackMarketGoods();
    // ���ž���ٺ»
    CT_VOID PushRankSalary();
    // �ָ�����ֵ
    CT_VOID RecoverStamina();
	//���¸���
	CT_VOID RefreshDuplicate();
	//���¾�����
	CT_VOID RefreshPVP();
	//�ж�vipʱ��
	CT_VOID VipTimeMonitor();
	//�������
	CT_VOID VipGiftPush();
	//��ȡ��������
	CT_VOID GetOnlineUserNum();
	//�ɾ������
	CT_VOID ActiveDeletePush(MSC_DB_ActiveEventChange &activeDel);
	//���������
	CT_VOID	ActiveAddPush(MSC_DB_ActiveEventChange &activeAdd);
	//���¼���
	CT_VOID RefreshWorship();
	//��ȡ������������б�(�������ͼӺ���,��ս�Ӷ�Ա) iType: 1.���� 2.ս�Ӷ�Ա
	CT_VOID GetOnlineUserData(CT_DWORD UserID, CT_CHAR *out_buf, CT_INT32 &bufLen, CT_INT32 iType=0);
	//��������ʱ�����
	CT_VOID UpdateActiveOnline();
	//����ת��
	CT_VOID RefreshTurntable();
	//����Ԫ����ͨ��
	CT_VOID RefreshTowerPass();
	//������Ӹ���
	CT_VOID UpdataTeamFightingTimes();
	//���¹�������
	CT_VOID UpdataBuyStaminaTimes();
	//�㲥�����������û�
	CT_VOID SendUserMessage(CT_DWORD dwMainID,CT_DWORD dwSubID,CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID SendUserMessage(CT_DWORD dwMainID,CT_DWORD dwSubID);
	//���¸����޹�
	CT_VOID UpdataMarketParsonal(CT_CHAR *pBuf,CT_INT32 iLen);
	//�������������û��Ĺ���
	CT_VOID UpdataUserAnnouncement(CT_CHAR *pBuf,CT_INT32 iLen);
	//��������״̬
	CT_VOID	UpdataDragonBallStatus();
	//����ͷ��״̬
	CT_VOID UpdataTitleStatus();
	//�����¿��״̬
	CT_VOID UpdataMonthCardStatus();
	//���GM ID
	CT_VOID InsertGMList(CT_DWORD dwUID);
	//�ж� is GM
	CT_BOOL IsGM(CT_DWORD dwUID);
	CT_VOID ClearGMList();
	//���Ҿ������û��Ƿ�����
	CT_BOOL FindPVPUser(const CT_DWORD dwUserID);

	//ȫ��Buff����
	CT_VOID BuffReward(CT_INT32 iID);

	CT_VOID RefreshActiveStatus(CT_INT32 iAEID, CT_INT32 iStatus);

	//���ս״̬��Ϣ
	CT_VOID SendCSStatusInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//���ս������Ϣ
	CT_VOID SendCSRewardInfo();
	//���սȫ������
	CT_VOID SendCSFullServerReward(G_FullServiceReward &fsr,CT_CHAR *pBuf,CT_INT32 iLen);

	//����ͷ���б�
	CT_VOID UpdateTitleList();
	//����̽������
	CT_VOID UpdateSeekPresent();

	//�ж��û��Ƿ��ڷ����б�
	CT_BOOL IsInSeekedList(CT_DWORD dwUserID);

	//����̽������
	CT_VOID UpdateSeekData();

	// �������а���״̬
	CT_VOID UpdateTopReward();

public:
	//�������ݿ�
	CT_VOID DBUpdataData();
	
public:
	//���������û�
	CT_VOID  ExitAllUser();
	
};


#endif