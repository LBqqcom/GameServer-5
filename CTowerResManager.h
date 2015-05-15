/********************************************************************
	created:	2018/8/14
	created:	2018/8/14   11:15
	file base:	ProtocolGame
	file ext:	h
	author:		pengwen
	
	purpose:	Ԫ��֮���Ĺ�����
*********************************************************************/

#ifndef CCTOWERRESMANAGER_H_
#define CCTOWERRESMANAGER_H_
#include <set>
#include <map>
#include "CSingleton.h"
#include "CMutex.h"
#include "../MessageDefined/CMD_Game.h"
#include "CThreadMes.h"

struct MSC_DB_TowerSlotReward;

class CCTowerResManager : public CCSingleton<CCTowerResManager>
{
public:
	//��Դλ
	struct ElementTowerSlotInfo
	{
		 ElemTowerUserInfo		userInfo;										//ռ��������Ϣ
		 CT_LONGLONG			llBeginTime;									//��ʼ����ʱ��         
		 CT_DWORD				iHelpFriend[ELEMENT_TOWER_MAX_FRIED_HELP_NUM];	//Э������ID
	};

	//Ԫ��֮������Ϣ
	struct ElementTowerFoorInfo
	{
	    ElementTowerSlotInfo	slotInfo[ELEMENT_TOWER_MAX_POSITION_NUM];		//ռ��λ��Ϣ
	};
public:
	CCTowerResManager(void);
	~CCTowerResManager(void);
public:
	//�û��������������ݽṹ  
	typedef std::map<CT_DWORD, CT_INT32>      			UserTowerMap;	       //�û����Ķ�Ӧ�� ����֪���û����ĸ������棬��Ϊ�û�id�� ֵΪ��id
	typedef std::map<CT_DWORD, CT_INT64>      			TowerUserMap;		   //���û��Ķ�Ӧ ��λΪ�û�id�� ֵΪ�û��������Ĳ�����32λ���� ��Դ��λ�ã�32λ��
	typedef std::map<CT_INT32, TowerUserMap>  			TowerFindMap;          //Ԫ��֮���ļ��� 

	//���ṹ�� ����������Э����Ϣ
	typedef std::map<CT_INT32, ElementTowerFoorInfo>	ElementTowerResMap;      //��      ��Ϊ���Ĳ�id
	typedef std::map<CT_INT32, ElementTowerResMap>      ElementTowerResMaps;     //������, ��Ϊ����id

	//�û��������Ľṹ����Ҫ�ǹ㲥��
	typedef std::set<CT_DWORD>                          UserEnterSet;            //��������������û��ļ���
	typedef std::map<CT_INT32, UserEnterSet>            UserEnterMap;            //�������û��ļ���

	typedef std::map<CT_INT32, MSC_TowerCheckPoint_Fighting> CPFightingMap;		// FID:Fighting
	typedef std::map<CT_INT32, CPFightingMap>			TowerFightingMap;		// CPID:(FID:Fighting)

private:
	CCMutex             								m_mutex;                //��
	UserTowerMap										m_userTowerMap;			//�û������ұ�
	TowerFindMap										m_towerFindMap;			//���û����ұ�

	ElementTowerResMaps  								m_elementTowerResMap;   //����Դ��
	UserEnterMap                                        m_userEnterMap;         //�����û������¼

	TowerFightingMap									m_towerFightingMap;

public:  //�����û����ң����ú���
	//�û���ѯ����
	CT_VOID InsertUserFind(CT_DWORD dwUserID, CT_INT32 iTowerID, CT_INT32 iFloor, CT_INT32 iSlot);
	//�û���ѯɾ��
	CT_VOID DelUserFind(CT_DWORD dwUserID, CT_INT32 iTowerID);
	//�û��Ƿ�������
	CT_BOOL FindUserInTower(CT_DWORD dwUserID, CT_INT32& iTowerID, CT_INT32& iFloor, CT_INT32& iSlot);
public: //�û������¼
	//�û�������
	CT_VOID InsertUserEnterMap(CT_INT32 iTowerID, CT_DWORD userID);
	//�û��뿪��
	CT_VOID DeleteUserEnterMap( CT_DWORD userID);
	//���㲥
	CT_VOID BroadCastInTower(CT_INT32 iTowerID,  CT_INT32 mainID, CT_INT32 subID, CT_CHAR* pBuf, CT_INT32 bufLen);


public:  //����Դ����
	//�����û�������ռ������
	CT_BOOL SetUserInTower(const ElemTowerUserInfo& userInfo, CT_INT32 iTowerID, CT_INT32 iFoor, CT_INT32 iSlot);
	//�����û������е�ռ������
	CT_VOID EaseUserInTower(CT_DWORD dwUserID, CT_INT32 iTowerID, CT_INT32 iFoor, CT_INT32 iSlot);
	//�����û�Э������
	CT_VOID SetUserHelpInTower(CT_INT32 iTowerID, CT_INT32 iFoor, CT_INT32 iSlot, CT_DWORD dwHelpID);
	//�õ���Դ������
	CT_BOOL GetSlotInTower(CT_INT32 iTowerID, CT_INT32 iFoor, CT_INT32 iSlot, CCTowerResManager::ElementTowerSlotInfo& slotInfo); 

public: //ȫ������Դ����
	//��ȫ�����ûָ����ݽṹ
	CT_VOID RecoverTowerDataFromIni();
	//�����ݿ��еõ�Ԫ��֮������
	CT_VOID RecoverTowerDataFromDB(CT_INT32 iFloor, CT_INT32 iSlot, CCTowerResManager::ElementTowerSlotInfo& slotInfo);
	//�õ�Ԫ��֮��������׼��д�����ݿ�
	CT_VOID GetTowerDataToDB( std::string& data);
	//����ʱ��������ͨ�ؼ�¼
	CT_VOID InitTowerFightExample(MSC_TowerCheckPoint_Fighting& fighting, CT_INT32 iFID);
	//��������ͨ�ؼ�¼
	CT_INT32 InsertTowerFightExample(MSC_TowerCheckPoint_Fighting& fighting);
	CT_BOOL GetTowerFightExample(CT_CHAR *pBuf, CT_INT32 &bufLen);
	CT_BOOL GetTowerFightExample(CT_INT32 iCPID, CT_INT32 iFID,MSC_TowerCheckPoint_Fighting& fighting);

public:
	//�õ�����Ϣ����¼ʱ�����û�
	CT_BOOL GetUserResData(CT_DWORD dwUserID, CT_CHAR* pBuf, CT_INT32& bufLen);
	//�õ�����Ϣ
	CT_BOOL GetTowerResData(CT_INT32 iTower, CT_INT32 iFloorLow, CT_INT32 iFloorHight,  CT_CHAR* pBuf, CT_INT32& bufLen);

public:
	//�����Դ����
	CT_VOID UpdateSlotProduct(CT_BOOL bForce = CT_FALSE);
	//������Դ
	CT_VOID GiveUpSlot(CT_INT32 iTower, CT_INT32 iFloor, CT_INT32 iSlot);

public:
	//����
	CT_VOID Harvast(  CT_INT32 iFoor, CT_INT32 iSlot, CT_DWORD dwUserID, 
					 const MSC_DB_TowerSlotReward&  towerSlotReword, CT_FLOAT rate  );

	//�����������
	CT_BOOL CalHarvastRes( CT_INT32 iFoor, CT_INT32 iSlot,
							CCTowerResManager::ElementTowerSlotInfo& SlotInfo, 
							CT_INT32 iProductID, CT_LONGLONG timenow, CT_BOOL bFroce,
							MSC_DB_TowerSlotReward& towerSlotReword );

public:
	//ս�����ݲ�ѯ�û��Ƿ����
	CT_BOOL FindTowerUser(const CT_DWORD dwUserID);
}; 
#endif
