/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CUserDuplicate.h
Version:		1.0
Description:    �û���������
Author:			wangchen
Create Date:	2012-10-19
History:        �û�����    

******************************************************************/
#ifndef CCUSERDUPLICATE_H_
#define CCUSERDUPLICATE_H_
#include <map>
#include <list>
#include "CGDefine.h"
#include "CMutex.h"
#include "CMD_Game.h"

//������Ϣ
struct SceneInfo
{
	CT_INT32		iWorldSceneID;		//����ID
	CT_INT32		iSceneID;			//����ID
	CT_INT32		iDifficulty;		//�Ѷ�
	CT_INT32		iSceneStar;			//�����Ǽ�
	CT_INT32		iFightingNum;		//ս������
};

//ɨ����Ϣ
struct RaidsInfo
{
	CT_INT32	iWorldID;			//���糡��
	CT_INT32	iSceneID;			//����ID
	CT_INT32	iDifficulty;		//�Ѷ�
	CT_INT32	iAllTime;			//�ܴ���
	CT_INT32	iCurrentTime;		//��ǰ����
	CT_INT64	lBeginTime;			//��ʼʱ��
	CT_INT32	iLastStamina;		//ʣ������
};

//���罱��
struct WorldReward
{
	CT_INT32	iWorldID;			//����ID
	CT_INT32	iRewardID;			//����ID
};

class CCUser;
class CCMutex;
class CCUserDuplicate1
{
	typedef	 std::map<CT_INT32,WorldReward>				RewardMap;//KEY ���糡��ID,��������
	typedef	 std::map<CT_INT32,WorldReward>::iterator		RD_IT;
	typedef  std::map<CT_INT32,SceneInfo>				DifficyltyMap;
	typedef  std::map<CT_INT32,SceneInfo>::iterator		DM_IT;
	typedef  std::map<CT_INT32,DifficyltyMap>			SceneMap;
	typedef  std::map<CT_INT32,DifficyltyMap>::iterator	SM_IT;
	typedef  std::map<CT_INT32,SceneMap>				WorldMap;
	typedef	 std::map<CT_INT32,SceneMap>::iterator		WM_IT;
	typedef std::list<CMD_G_RaidsReward>				RaidsRewardList;
	typedef std::list<CMD_G_RaidsReward>::iterator		RR_IT;


private:
	CT_INT32		m_iBuyFightingTime;			//����ս������
	RewardMap		m_RewardMap;				//�콱����
	WorldMap		m_SceneMap;					//������Ϣ

	RaidsInfo		m_RaidsInfo;				//ɨ����Ϣ
	RaidsRewardList	m_RaidsRewardList;			//ɨ��������Ŀ


private:
	CCUser*  m_pUser;
	CCMutex* m_pMutex;

public:
	CCUserDuplicate1(void);
	~CCUserDuplicate1(void);

public:
	//��ʼ��
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex);
	//�����û�����
	CT_VOID Clear();

public:
	//���볡��
	CT_VOID InsterScene(SceneInfo &Sceneinfo);
	//���뽱��
	CT_VOID InsterReward(CT_INT32 iWorldID,CT_INT32 iCount);
	//����ɨ����Ϣ
	CT_VOID InsterRaidsInfo(RaidsInfo &Raidsinfo);
	//����ɨ����¼
	CT_VOID InsertRaidsReward(const CMD_G_RaidsReward RaidsReward);
	//���빺��ս������
	CT_VOID InsertBuyFighting(CT_INT32 iBuyFighting){_AUTOMUTEX(m_pMutex);m_iBuyFightingTime=iBuyFighting;};

public:
	//���ó�����Ϣ
	CT_VOID SetSceneInfo(SceneInfo &Sceneinfo);

public:
	//��ȡ������Ϣ
	CT_BOOL GetSceneInfo(CT_CHAR *pBuf,CT_INT32 &iLen);
	CT_BOOL GetSceneInfo(CT_INT32 iWorldID,CT_INT32 iSceneID,CT_INT32 iDifficultyID,SceneInfo &Sceneinfo);
	//���������ӣ��ж��û��Ƿ�ͨ���ƶ��ؿ�(ͨ��ָ���ؿ������Ǵﵽָ����	��:��ͨ����������������Ϊ3��)
	CT_BOOL GetUserPassedDuplicate(CT_INT32 iWorldScene,CT_INT32 iScene,CT_INT32 iLevel);  //iLevelͨ���Ǽ�
	//��ȡ��������
	CT_INT32 GetSceneScore(CT_INT32 iWorldID);
	//��ȡ����
	CT_INT32 GetWorldReward(const CT_INT32 iWorldID,const CT_INT32 iRewardID);
	//��ȡ�������
	CT_INT32 GetBuyFightingTime(){_AUTOMUTEX(m_pMutex);return m_iBuyFightingTime;};

public:
	//���ӹ������
	CT_VOID AddBuyFightingTime();
	//����ս������
	CT_VOID AddFightingNum(SceneInfo &Sceneinfo);


public:
	//���³�����Ϣ
	CT_VOID UpdataSceneInfo(SceneInfo &Sceneinfo);
	//ˢ�¸���
	CT_VOID UpdataDuplicate();
	//���½���
	CT_VOID UpdataSceneReward(const CT_INT32 iWorldID,const CT_INT32 iRewardID);

public:
	//����ɨ����Ϣ
	CT_INT32 SetRaidsInfo(RaidsInfo &Raidsinfo);
	//��ȡɨ����Ϣ
	CT_VOID GetRaidsInfo(RaidsInfo &Raidsinfo);
	//����ɨ��
	CT_INT32 UpdataRaids();
	//���ɨ����Ϣ
	CT_VOID ClearRaidsInfo();
	//��ȡɨ��ʱ��
	CT_INT64 GetLastTime(time_t t=0);
	//���뽱��
	CT_BOOL InsertReward();
	//��ȡ����
	CT_BOOL GetRaidsReward(CT_CHAR *pBuf,CT_INT32 &iLen,CT_BOOL bAllReward=CT_FALSE);


};
#endif