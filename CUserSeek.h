#ifndef CUSERSEEK_H_
#define CUSERSEEK_H_

/********************************************************************
	created:	2014/9/23
	created:	12:11:2012   13:53
	file base:	CCUserSeek
	file ext:	h
	author:		wangchen
	
	purpose:	
*********************************************************************/
#include <map>
#include "CGDefine.h"
#include "CMutex.h"
#include "GlobeScriptInterface.h"
#include "CLogMeg.h"
#include "CUserData.h"
#include "CUserPVPInfo.h"

class CCUser;
struct MSC_DB_EnterOtherTown;

//��ǽ�ӳ�
struct WallAddition
{
	CT_INT32	iID;
	CT_INT32	iGrade;
};

//������Ϣ
struct BuildData
{
	CMD_G_BuildingData	bd;
	CT_INT32			iPlunderNum;
};

//��Ʒ��Ϣ
struct SeekPresent 
{
	CT_INT32	iChestID;
	CT_INT32	iGetTime;
};

//ս��
struct TeamSeek
{
	CT_DWORD dwUserID;
	CT_INT32 iType;				// �������� 1��ʥŮ��2������
	CT_CHAR cTName[NAME_SIZE];	// ս������
	CT_INT32 iFightResult;		// ս���Ӷ���(1:ʤ��,2:ʧ��)
};


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
//	TeamSeek TeamFlag[SEEKED_SPITE_TIMES];					// ս��
//	BuildData					buildingData[SEEK_BUILD_NUM];		// ����������Ϣ
//	CMD_G_UpgradeBuildingData	upgradeBuilding[SEEK_BUILD_NUM];	// �����Ľ���
//	CMD_G_ProductingBuildingData	productBuilding[SEEK_BUILD_NUM];// �����Ľ���
//};

//̽������
struct SeekData
{
	CT_INT32		iSeekWillTimes;					// ���ƷõĴ���
	CT_INT32		iSeekSpiteTimes;				// �Ѷ�õĴ���
	CT_INT32		iSeekedWillTimes;				// �ѱ��ƷõĴ���
	CT_INT32		iSeekedSpiteTimes;				// �ѱ���õĴ���
	CT_INT32		iRescueNum;						// Ӫ�ȴ���
	CT_INT32		iWomanNum;						// ʥŮ
	CT_INT32		iFarmarNum;						// ũ��
	CT_INT32		iPet[SEEK_KIDNAP_PET];			// ����
	CT_INT32		iLevel;							// ������˳���ʱ���Լ��ĵȼ�
	CT_DWORD		dwSeekID;						// ��ǰ��̽���ߵ�ID
	CT_INT32		iType;							// ��ǰ̽������
	SeekPresent		present[SEEKED_PRESENT_NUM];	// �õ�����Ʒ��Ϣ
	CT_INT32		iPresentStatus[SEEK_PRESENT_ITEM_NUM];	// ���������Ƿ����͹���״̬
	CT_INT32		iAddSpiteTimes;					// ���ӵĶ�ô���
};

//ս��
struct SeekFightingData
{
	CT_UCHAR		ucIndex[SEEK_FIGHT_RECORD_INDEX+1];	//����
	CT_DWORD		dwUserID;							//�û�ID
	CT_INT32		iType;								//̽������
	CT_INT32		iSeed;								//�������

	UserFightingStatus	LeftFightingData;				//��ս��ս������
	UserFightingStatus	RightFightingData;              //����ս����ս������
};

struct StatisticsInfo 
{
	CT_INT32		iPlayerNum[SEEK_TYPE_NUM+1];		//��¼������� 
	CT_INT32		iGold;								//�Ӷ�Ľ��
	CT_INT32		iFood;								//�Ӷ����ʳ
	CT_INT32		iRank;								//�Ӷ������
	CT_INT32		iSaintess;							//��ܵ�ʥŮ
	CT_INT32		iFarmer;							//��ܵ�ũ��
	CT_INT32		iBuildingNum;						//�������ٵĽ�����
	CT_INT32		iPresentID[SEEK_PRESENT_ITEM_NUM];	//����ID
	CT_INT32		iPresentNum[SEEK_PRESENT_ITEM_NUM];	//��������
	CT_INT32		iPetID[SEEK_KIDNAP_PET];			//��ܵĳ���ID
	CT_INT32		iPetNum[SEEK_KIDNAP_PET];			//��ܵĳ�������
};

typedef std::map<CT_INT32,WallAddition>		WallAdditionMap;
typedef std::map<CT_LONGLONG,TeamSeek>		TeamSeekMap;		
typedef std::map<CT_DWORD,KidnapInfo>		KidnapMap;					
typedef std::map<CT_DWORD,UserInfo>			UserInfoMap;	
typedef std::map<CT_LONGLONG,VistorInfo>	VistorInfoMap;		
typedef std::vector<CT_DWORD>				SeekVector;	
typedef std::map<CT_DWORD,BuildData>		BuildDataMap;
typedef std::vector<CT_INT32>				SpeedUpBIDVector;
typedef std::map<std::string,SeekFightingData>	SeekFightingDataMap;
class CCUserSeek: public CCUserData<CT_INT32, SeekData>
{
public:
	WallAdditionMap			m_WallAdditionMap;			//��ǽ����
	CT_INT32				m_iPos[POST_COUNT/2];		//��ǽ��
	
private:
	TeamSeekMap				m_TeamSeekMap;				// ս��
	KidnapMap				m_KidnapGirlsMap;			// ��ܵ�ʥŮ
	KidnapMap				m_kidnapWorkersMap;			// ��ܵĹ���
	UserInfoMap				m_FriendInfoMap;			// ̽�ý������
	UserInfoMap				m_RandInfoMap;				// ̽�ý����������
	VistorInfoMap			m_VistorInfoMap;			// ��̽�õļ�¼
	SeekVector				m_FriendSeekVector;			// ���Ʒõ���
	SeekVector				m_RandSeekVector;			// �Ѷ�õ���
	UserInfoMap				m_AllRandPlayerMap;			// ���п�������ʵ��˵���Ϣ
	BuildDataMap			m_BuildDataMap;				// ̽����ҽ�����Ϣ
	SpeedUpBIDVector		m_SpeedUpBidVector;			// ��ǰ�Լ��ٹ��Ľ���ID
	SeekFightingDataMap		m_SeekFightingMap;			//ս��
	VistorInfoMap			m_SeekInfoMap;				// ̽�ñ��˵ļ�¼
	KidnapMap				m_PetKidnapMap;				// ��ܵĳ���

	SeekData				m_SeekData;					//̽����Ա
	VistorInfo				m_VistorInfo[SEEK_DOTYPE_NUM];	//��ǰ̽�õļ�¼

public:
    CCUserSeek();
    ~CCUserSeek();
public:
    CT_VOID Init(CCUser* pUser, CCMutex* pMutex);

	//�����û�����
	CT_VOID Clear();
protected:
	virtual CT_INT32 transData(const SeekData &src, CT_CHAR *pDst) const;
public:
	virtual CT_VOID SyncData(CT_INT64 currTime, CT_BOOL bForce = CT_FALSE);

public:
	CT_INT64 Get64Int(CT_INT32 high,CT_INT32 low);

	/*��½��ʼ��*/
	//�����ǽ����
	CT_VOID InsertWallAddition(WallAddition & wa);
	//�����ǽʿ��
	CT_VOID InsertWallArmy(CT_INT32 iPos[POST_COUNT/2]);
	//��������
	CT_VOID InsertHostages(const CT_INT32 iType,KidnapInfo &ki);
	//�������̽������б�
	CT_VOID InsertSeekRandPlayer(UserInfo userInfo) { m_AllRandPlayerMap.insert(UserInfoMap::value_type(userInfo.dwUserID, userInfo)); }
	//�������Ʒù��ĺ���
	CT_VOID InsertSeekedGoodwill(CT_DWORD dwFUserID) { m_FriendSeekVector.push_back(dwFUserID); }
	//�����Ѷ�ù��ĺ���
	CT_VOID InsertSeekedSpite(CT_DWORD dwFUserID) { m_RandSeekVector.push_back(dwFUserID); }
	//��������
	CT_VOID InsertFlags(TeamSeek ts);
	//����̽�ý������
	CT_VOID InsertFriendList(UserInfo friendInfo) { m_FriendInfoMap.insert(UserInfoMap::value_type(friendInfo.dwUserID, friendInfo)); }
	//�����ý���������
	CT_VOID InsertRandList(UserInfo randInfo) { m_RandInfoMap.insert(UserInfoMap::value_type(randInfo.dwUserID, randInfo)); }
	//����̽�û�����Ϣ
	CT_VOID InsertSeekData(SeekData seekdata);
	//����ս��
	CT_VOID InsertFightingData(SeekFightingData &sfd,CT_BOOL bSave=CT_FALSE);

public:
	//���ӳ�ǽ����
	CT_VOID AddWallAddition(const CT_INT32 iID);
	//�ͷ�����
	CT_VOID ReleaseHostages(const CT_DWORD dwUserID);


public:
	//��ȡ��ǽ����
	CT_BOOL GetWallAddition(const CT_INT32 iID,WallAddition &wa);
	CT_BOOL GetWallAddition(CT_CHAR *pBuf,CT_INT32 &iLen);
	//��ȡ��ǽʿ��
	CT_BOOL GetWallArmy(CT_INT32 iPos[POST_COUNT/2]);
	CT_BOOL GetWallArmy(CT_CHAR *pBuf,CT_INT32 &iLen);
	//��ȡ�Ӷ��û�ս����Ϣ
	CT_BOOL GetKidnpaFightData(const CT_DWORD dwUserID,UserFightingStatus &ufs);
	//��ȡ̽����Ϣ
	CT_VOID GetSeekData(SeekData& sd);
	//��ȡ�ÿ���Ϣ
	CT_BOOL GetVistorInfo(CT_INT32 iType, const CT_DWORD dwUserID,VistorInfo &vi);
	//��ȡ�������
	CT_INT32 GetKidnapNum(const CT_DWORD dwUserID,const CT_INT32 iType);
	//��ȡ������Ŀ
	CT_INT32 GetFlagCount(){_AUTOMUTEX(m_pMutex);return m_TeamSeekMap.size(); }
	//��ȡ̽������
	CT_BOOL GetSeekInfo(CT_CHAR* pBuf,CT_INT32 &iLen);
	//��ȡ������Ϣ
	CT_VOID GetBuildData(const CT_INT32 iBuildID,BuildData &bd);
	// �õ���������
	CT_BOOL GetTeamFlag(CT_CHAR* pBuf,CT_INT32& ilen);
	// �õ�ʥŮ������Ϣ
	CT_BOOL GetKidnapGirls(CT_CHAR* pBuf,CT_INT32 iLen);
	// �õ�����������Ϣ
	CT_BOOL GetKidnapWorkers(CT_CHAR* pBuf,CT_INT32 iLen);
	// �õ�����������Ϣ
	CT_BOOL GetKidnapPets(CT_CHAR* pBuf,CT_INT32 iLen);
	// �õ���̽�ô���
	CT_INT32 GetSeekTimes(CT_INT32 iType);
	// �õ���ƷID
	CT_INT32 GetSeekPresent(G_ChestByLevel& chest);
	// �õ�������Ʒ
	CT_BOOL GetAllPresent(CT_CHAR* pBuf, CT_INT32& iLen);
	//���ս����Ϣ
	CT_BOOL GetFightingData(CT_UCHAR *pIndex,SeekFightingData &sfd);

public:
	//���ó�ǽʿ��
	CT_VOID SetWallArmy(CT_INT32 iPos[POST_COUNT/2]);
	//���ý�����Ϣ
	CT_VOID SetBuildData(const CT_INT32 iBuildID,BuildData &bd);

public:
	// ˢ��̽����
	CT_VOID UpdateSeekedPerson(CT_INT32 iType);
//	CT_VOID UpdateSeekedPerson();
	// ���뱻���ߵĻ�����Ϣ
	CT_VOID InsertSeekerBaseInfo(const CMD_G_EnterTown_Others_Suc& enterTown);
	// �Ƿ��Ʒù�
	CT_BOOL IsSeekGoodwill(CT_DWORD dwFUserID);
	// �Ƿ񱻶�ù�
	CT_BOOL IsSeekSpite(CT_DWORD dwFUserID);
	// ���̽�ô����Ƿ��㹻
	CT_BOOL CheckSeekTimes(CT_INT32 iType);
	// ��ⱻ̽�ô����Ƿ��㹻
	CT_BOOL CheckSeekedTimes(CT_INT32 iType);	
	// ��⽨���Ƿ�ɼ���
	CT_BOOL CheckBuildingSpeedUp(CT_INT32 iBID);
	// ��������Ƿ��������
	CT_BOOL CheckPresentStatus(CT_INT32* pPresentID);
	// �������Ƿ񱻰��
	CT_BOOL IsPetKidnap(CT_INT32 iPetChangeID);

	// ̽��һ��
	CT_VOID SeekOneTimes(CT_INT32 iType, CT_DWORD dwFUserID);
	// ��̽��һ��
	CT_VOID SeekedOneTimes(CT_INT32 iType);
	// �жϺ����Ƿ���̽�ý�����
	CT_BOOL IsInSeekedList(CT_DWORD dwUserID);
	// �õ��ѻ�õ��������
	CT_INT32 GetPresentNum();
	// �յ�����
	CT_VOID GetPresent(CT_INT32 iChestID);
	// �õ����ڱ�̽����
	CT_DWORD GetSeeked() { return m_SeekData.dwSeekID; }
	// ����̽����Ϣ
	CT_VOID UpdateSeekData(SeekData& sd);
	// ����ս��
	CT_VOID InsertTeamFlag(TeamSeek& ts);
	// �ε�ս��
	CT_VOID PullTeamFlag(const CT_DWORD dwUserID,const CT_INT32 iType);
	// ��������
	CT_VOID UpdateKidnap(CT_INT32 iType, KidnapInfo& kidnap);
	// ɾ������
	CT_VOID DeleteKidnap(CT_INT32 iType, KidnapInfo& kidnap);
	// �õ�̽�ý���ÿ�ID
	CT_BOOL GetSeekListPlayer(CT_INT32 iType, CT_CHAR* pBuf, CT_INT32 iLen);
	// ���̽�ñ��˵ļ�¼��Ϣ
	CT_VOID AddVistorInfo(VistorInfo& vi);
	// ��ӱ�̽�õļ�¼��Ϣ
	CT_VOID AddSeekInfo(VistorInfo& vi);
	// �õ���ǰ��̽����Ϣ(���ʱ��ˣ�
	CT_BOOL GetCurrentVistorInfo(VistorInfo* pvi, CT_INT32& iLen);
	// ���õ�ǰ̽����Ϣ
	CT_VOID SetVistorInfo(VistorInfo vi);
	// ��ȡ����
	CT_VOID UsePresent();
	// ���ٽ�������¼ID
	CT_VOID SpeedUpBuilding(CT_INT32 iBID) {_AUTOMUTEX(m_pMutex); m_SpeedUpBidVector.push_back(iBID); }
	// ���������¼ID
	CT_VOID SendPresent(CT_INT32 iPresentID);
	// ������������
	CT_VOID UpdatePresent();
	// �����Լ��ķÿͼ�¼
	CT_VOID UpdateSeekVisitorInfo(CT_INT32 iType, const VistorInfo& vi);

	// ��Ӫ
	CT_VOID SeekVistor();
	CT_VOID GetVisitorDes(VistorInfo& vi, CT_CHAR* pInfo,CT_INT32 iSize);

	// 0�����
	CT_VOID UpdateAllSeekData();

	// ���Ӷ�ô���
	CT_BOOL AddSpiteTimes(CT_INT32 iTimes);

	//���ͺڵ�
	CT_VOID SendBlacketMarket();

	//�õ�����ͳ��
	CT_BOOL GetTotalInfo(TotalInfo* pInfo, CT_INT32 iSize,CT_INT32& iNum, CT_BOOL IsToday = CT_FALSE);

	//����ͳ�Ƽ�¼
	CT_VOID SingleStatistics(VistorInfoMap& vistorInfoMap, StatisticsInfo& statistics,CT_BOOL isToday = CT_FALSE);

};

#endif

