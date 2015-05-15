/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCUserBuilding.h
Version:		1.0
Description:    �û���������
Author:			pengwen
Create Date:	2012-9-29
History:        �û���������     

******************************************************************/
#ifndef CCUSERBUILDING_H_
#define CCUSERBUILDING_H_
#include <map>
#include "CGDefine.h"
#include "CMutex.h"
#include "GlobeScriptInterface.h"

class CCUser;
struct CMD_G_BuildingData;

//��������������Ϣ
struct UpgradingBuildData
{
	CT_INT32			iID;					//����id
	CT_LONGLONG			llBeginTime;			//��ʼ����ʱ��
	CT_BOOL             bSpeedUpTime;           //�Ƿ���٣�ֻ�п�����0��1��
	CT_INT32            iNeedTime;              //��Ҫ��ʱ��
	G_BuildingUpgrade   buildingUpgrade;        //�����������ݼ�¼
};

//������Ϣ
struct BuildingData
{
	CT_INT32 iID;              //����id
	CT_INT32 iGrade;           //�����ȼ�
	CT_BOOL  bChange;          //�Ƿ�ı����
};

//���������ṹ
struct ProductingBuilding
{
	CT_INT32     iBType;            //��������
	CT_INT32     iBid;              //����id
	CT_INT32     iType;				//��������
	CT_LONGLONG  llTime;			//������ʼʱ��
	CT_BOOL      bSendCanHavest;	//�Ƿ��Ϳ����ջ񾯸�
	CT_INT32     iLastingTime;      //��������ʱ��
	CT_INT32     iWorker ;          //���ĵĹ���
	CT_INT32     iBGrade;           //�����ȼ�
	CT_INT32	 iDiamod;			//���ĵ���ʯ
	CT_FLOAT	 fSeekAddition;		//̽�üӳ�

};

//�����������ͳ��key
struct ProductedBuildingResultKey
{
	CT_INT32            iProp1;           //��õĵ���1id
	CT_INT32            iProp2;           //��õĵ���2id
	CT_INT32            iBid;             //����id
	bool operator<(const ProductedBuildingResultKey& src) const
	{
		if (src.iBid < iBid)
		{
			return true;
		}

		if (src.iBid == iBid && src.iProp1 < iProp1)
		{
			return true;
		}

		if (src.iBid == iBid && src.iProp1== src.iProp1 && src.iProp2 < iProp2)
		{
			return true;
		}
		return false;
	}
};
//�����������ͳ��
struct ProductedBuildingResult
{
	CT_INT32            iGold;            //��õĽ��
	CT_INT32            iFood;            //���ʳ��
	CT_INT32			iCivil;			  //���������
	CT_INT32			iDiamod;		//������ʯ
	CT_INT32            iWorker;          //���ĵĹ���
	CT_INT32            iStamina;         //���ĵ�����ֵ
	CT_INT32            iProp1Num;        //��õ���1����Ŀ
	CT_INT32            iProp2Num;        //��õ���2����Ŀ
	//CT_INT32		iSpeedupTimes;     //���ٴ���
	CT_INT32			iProductingTimes;  //��������
};


//��һ�����������ͳ��
struct UpgradedBuildingResult
{
	CT_DWORD     dwBid;             //����id
	//CT_CHAR      cName[NAME_SIZE];  //��������
	CT_INT32     iDiamod;           //���ĵ���ʯ��ͳ��
	CT_INT32     iGold ;            //���ĵĽ�ҵ�ͳ��
	CT_INT32     iFood;             //���ĵ�ʳ���ͳ��
	CT_INT32     iWorker;           //���ĵĹ����˴ε�ͳ��
	CT_INT32     iProp1;            //���ĵĵ���id1
	CT_INT32     iProp1Num;         //���ĵĵ���1��Ŀ��ͳ��
	CT_INT32     iProp2;            //���ĵĵ���id1
	CT_INT32     iProp2Num;         //���ĵĵ���1��Ŀ��ͳ��
	CT_INT32     iProp3;            //���ĵĵ���id1
	CT_INT32     iProp3Num;         //���ĵĵ���1��Ŀ��ͳ��
	CT_INT32     iProp4;            //���ĵĵ���id1
	CT_INT32     iProp4Num;         //���ĵĵ���1��Ŀ��ͳ��
	CT_INT32     iCiviNum;          //�����������ȵ�ͳ��
	//CT_INT32     iSpeedTimes;       //��ʯ���ٴ�����ͳ��
	CT_INT32     iXp;               //�ۼƵľ���
	CT_INT32     iUpgradeTimes;     //��������
};

class CCUserBuilding
{
	friend class CCUser;
public:
	typedef std::map<CT_DWORD, UpgradingBuildData>			  UpgradeingBuildDataMap; //���ڸ��µĽ�������
	typedef std::map<CT_DWORD, ProductingBuilding>            ProductingtManualBuildingMap;  //�������ֶ���������
	typedef std::map<CT_DWORD, BuildingData>                  BuildingDataMap;        //����������
	
private:
	CCUser*             m_pUser; 
	CCMutex*			m_pMutex;
	
	typedef std::map<CT_DWORD, UpgradingBuildData>::iterator  UBD_IT;
	typedef std::map<CT_DWORD, BuildingData>::iterator        BD_IT;
    typedef std::map<CT_DWORD, ProductingBuilding>            ProductingtAutoBuildingMap;    //�������Զ���������
	typedef std::map<CT_DWORD, UpgradedBuildingResult>        UpgradedBuildingResultMap;     //�����������ͳ������
	typedef std::map<ProductedBuildingResultKey, ProductedBuildingResult> ProductedBuildingResultMap; //��������ͳ������


	ProductingtManualBuildingMap    m_productingManualBuildingMap;   //�ֶ����������Ľ���   
	UpgradeingBuildDataMap	        m_upgradeBuildDataMap;           //������������
	BuildingDataMap					m_buildingDataMap;               //��������
	ProductingtAutoBuildingMap      m_productingAtuoBuildingMap;     //�Զ����������Ľ���
	UpgradedBuildingResultMap		m_upgradedBuildingResultMap;	 //�����������ͳ������
	ProductedBuildingResultMap      m_productedBuildingResultMap;    //�����������ͳ������
	CT_LONGLONG                     m_llSaveBuildingDataLastTime;    //�ϴα��潨�����ݵ�ʱ��
	CT_LONGLONG                     m_llSaveProductingDataLastTime;  //�ϴα����������ݵ��¼�
public:
	CCUserBuilding();
	~CCUserBuilding(void);
	CCUserBuilding operator = (const CCUserBuilding& userBuilding)
	{ 
	}

public:
	//��ʼ��  
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex)
	{
		m_pUser = pUser;
		m_pMutex = pMutex;
		m_productingAtuoBuildingMap.clear();
		m_productingManualBuildingMap.clear();
		m_buildingDataMap.clear();
		m_upgradeBuildDataMap.clear();
		m_upgradedBuildingResultMap.clear();
		m_productedBuildingResultMap.clear();
		m_llSaveBuildingDataLastTime = time(0);
		m_llSaveProductingDataLastTime=time(0);
	}

	//�����û�����
	CT_VOID Clear()
	{
		_AUTOMUTEX(m_pMutex);
		m_productingAtuoBuildingMap.clear();
		m_productingManualBuildingMap.clear();
		m_buildingDataMap.clear();
		m_upgradeBuildDataMap.clear();
		m_upgradedBuildingResultMap.clear();
		m_productedBuildingResultMap.clear();
		m_llSaveBuildingDataLastTime = 0;
		m_llSaveProductingDataLastTime=0;
	}

public:
	//���³��н�������(�ɶ�ʱ���߶��ĸ���)
	CT_VOID UpdateUpgradingBuilding( CT_DWORD dwThreadID);
	//�õ���������
	CT_BOOL GetBuildingData( CT_CHAR* pBuf, CT_INT32& bufLen);
	//�õ�������������
	CT_BOOL GetUpgardingBuildingData(CT_CHAR* pBuf, CT_INT32& bufLen);
	//�õ�������������
	CT_BOOL GetUpgardingBuildingData(int bid, UpgradingBuildData& ubd);
	//���³��н�������
	CT_BOOL UpdateUpgradingBuilding(int bid, UpgradingBuildData& ubd);
	//�õ����������Ľ���
	CT_BOOL GetUpgradingBuildingData(UpgradingBuildData* pBd, CT_INT32 iLen);
	//��ý�����Ϣ
	CT_BOOL GetBuildingData(const CT_INT32 iType,CMD_G_BuildingData &BuildData);
	//��ý�����Ϣ
	CT_BOOL GetBuildingDataEx(const CT_INT32 iBID,CMD_G_BuildingData &BuildData);

	//��⽨����������
	CT_BOOL FindUpgradeBuildData(CT_INT32 bid);
	//����Ƿ�ӵ���������
	CT_BOOL FindBuildData(CT_INT32 bid, BuildingData& db);
	//����Ƿ�ӵ���������
	CT_BOOL FindBuildData(CT_INT32 bid);
	//����������������
	CT_VOID InsertUpgradeBuildData(const UpgradingBuildData& ubd) {_AUTOMUTEX(m_pMutex);   m_upgradeBuildDataMap.insert(UpgradeingBuildDataMap::value_type(ubd.iID, ubd)); }
	//���뽨������
	CT_VOID InsertBuildData(const BuildingData& ud) {_AUTOMUTEX(m_pMutex);   m_buildingDataMap.insert(BuildingDataMap::value_type(ud.iID, ud));}
	//���½�������
	CT_VOID UpateBuildData(const BuildingData& ud);
	//ɾ����������
	CT_VOID DeleteUpagradeBuildData(CT_DWORD bid);

public:
	//���뽨������
	CT_BOOL InsertBuildingProductMap(const ProductingBuilding& bp);
	//��������н���
	CT_VOID DeleteBuildingProductMap(CT_INT32 bid);

	//���ҽ�������
	CT_BOOL FindBuildingProduct(CT_INT32 bid);
	//�ҵ�����������������
	CT_BOOL FindBuildingProduct(CT_INT32 bid, ProductingBuilding& bp);
	//�õ�����������������
	CT_BOOL GetProductingData(CT_CHAR *pBuf, CT_INT32 &bufLen);
	//�õ�����������������
	CT_BOOL GetProductingData(ProductingBuilding *pBp,CT_INT32 ilen);
	//�õ�����������������
	CT_VOID UpdateBuildingProduct(CT_INT32 bid, ProductingBuilding& bp);
	//�����Զ������Ľ����¼�
	CT_VOID SysProductingBuilding();
	//���������н����¼�
	CT_VOID UpdateProductingBuilding();
private:
	//��⽨���Ƿ�����ջ�
	CT_VOID CheckHarvest(CT_INT64 timeNow,  ProductingBuilding& pb);

public:
	//�����Զ�����
	//�����Զ���������
	CT_BOOL FindAutoBuilding(CT_INT32 bid);
	//�����Զ���������
	CT_BOOL FindAutoBuilding(CT_INT32 bid, ProductingBuilding& pb);
	//�����Ѿ��������Զ�����
	CT_BOOL FindAutoProductingBuilding(CT_INT32 bid);
	//�����Զ���������
	CT_VOID UpdateAutoBuilding(CT_INT32 bid, ProductingBuilding& pb);
	//�����Զ���������
	CT_VOID InsertAutoBuilding(CT_INT32 bid, ProductingBuilding& pb);
	//Ϊ����ϵͳ���ӽӿڣ���Ҫͨ���������ͻ�ȡ���ݣ�1������ָ��������ָ������	��:����������ĵ�2�� 2��ӵ�е�N��ָ������	��:ӵ�еڶ�����ӣ�
	CT_INT32 GetUserBuildByType(CT_INT32 iBuildType,CT_INT32 iLevel);
	//��������
	CT_BOOL BuildValidate(CT_INT32 iType);
	//��������һ���ջ�
	CT_VOID DisposeHarvestAutoOnce();
public:
	//������������ͳ��
	CT_VOID AddUpgradedBuildingResult(const UpgradedBuildingResult& result);
	//������������ͳ��
	CT_VOID AddProductedBuildingResult(const ProductedBuildingResultKey& key, const ProductedBuildingResult& result);
public:
	//ͬ���������ݵ����ݿ�
	CT_VOID UpdateSaveBuildingData(CT_BOOL bForceBuildData, CT_BOOL bForceUpgradeBuildData,  CT_BOOL bForceResult );
	//ͬ�������������ݵ����ݿ�
	CT_VOID UpdateSaveProductingData(CT_BOOL bForceBuildProducting  ,CT_BOOL bForceResult);

	
};

#endif