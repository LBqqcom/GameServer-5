/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCResourceGrabMgr.h
Version:		1.0
Description:    ��Դ����ϵͳ
Author:			LinQ
Create Date:	2013-05-04
History:        ��Դ����ϵͳ��������

******************************************************************/
#ifndef _CRESOURCE_GRAB_H_
#define _CRESOURCE_GRAB_H_

#include "CTType.h"
#include "GlobeScriptInterface.h"
#include "CSingleton.h"
#include <map>
#include <set>
#include "../sdk/include/CApi.h"
#include "../sdk/include/GlobeScriptInterface.h"
#include "CGDefine.h"
#include "../sdk/include/CMutex.h"
#include "CThreadMes.h"

struct MSC_DB_ResourceProduct;
struct CMD_G_ResSpotApply;
class CCUser;
class CCFightTeam;

//��Դ����
struct ResourceProduct
{
    CT_DWORD        dwUserID;         //�û�id
	CT_INT32        iGrade;           //�û��ȼ�
	CT_INT32		iProductCount;    //�Ѿ������Ĵ���
	CT_INT32        iLeftProductTime; //��������ʣ��ʱ��
	ENUM_RES_SPOT_STATUS iStatus;      //��Դ��״̬
};


//��Դ��ʶ
struct ResSpot
{
	CT_INT32		 iSpotID;							  //��Դ��ID
	CT_INT32		 iSceneID;							  //��Դ����ID
};


//��Դռ�켯��
struct ResSpotTag
{
	ResSpot          resSpot[SPOT_RES_SIZE];             //ս��ռ�����Դ����
};

//פ����Ա
struct GuardTag
{
	CT_DWORD         dwUserID[SPOT_GUARD_MEMBER];        //פ����Ա
};

//��Դ�������Ϣ
struct ResourceBase
{
	CT_DWORD		 dwFTID;								  //ս��id
	CT_CHAR			 szNote[SPOT_VECTORY_SIGN_SIZE];	  //ʤ������
	CT_INT32		 iLeftProtectTime;					  //ռ�챣��ʱ��
	CT_INT32         iLeftOccupyTime;					  //ʣ��ռ��ʱ��
	CT_BOOL          bSendProtectMes;                     //�Ƿ��͹���������֪ͨ
};

// ��Դ����
struct ResourceSpot
{
	ResourceBase     resBase;                             //��Դ����Ϣ
	GuardTag         guardTag;                            //������Ա
	ResourceProduct  resProduct[CLAN_MAX_MEMBER];		  //������Ϣ
};

//���Դ��
struct ResActive
{
	CT_INT32		iSceneID;
	CT_INT32		iSpot;
	CT_INT32		iResEffect;				//���Ч��
	CT_INT32		iProductType;			//��������
	CT_INT32		iResID;					//��Դid
	CT_INT32		iProductNum;			//��������
	CT_INT32		iProductTime;			//����ʱ��
	CT_INT32		iProtectTime;			//����ʱ��
	CT_INT32		iFocusOccupyACTVType;	//ǿ����������
	CT_INT32		iFocusOccupyACTVNum;	//ǿ��������Ŀ
	CT_FLOAT		fProductMultiple;		//��������
};



// ��Դ����
class CCResourceGrabMgr : public CCSingleton<CCResourceGrabMgr>
{
private:
	//��Դ���
	typedef std::map<CT_INT32,    ResourceSpot>			SpotMap;		//��Դ�㼯�� keyΪ��Դ��id
	typedef std::map<CT_INT32,    SpotMap>				SenceMap;		//�������� keyΪ����id
	typedef std::multimap<CT_DWORD,    CT_LONGLONG>     ResSpotFindMap; //ս�ӣ���Դ����ұ�  keyս�ӵ�id
	typedef std::set<CT_DWORD>                          SenceUserSet;   //��ų������û�����
	typedef std::map<CT_INT32,   SenceUserSet>          SenceUserMap;   //�����û�����
	typedef std::map<CT_INT64,ResActive>				ResActiveMap;	//��Դ��

	SenceMap		m_mapSpots;					// ������Դ�㣬����ID��Դ��ID�ϳ�Ϊkey��value��ResourceSpot����ָ��
	CCMutex			m_mutex;                    // ��
	ResSpotFindMap  m_mapResSpotFind;           //ս�ӣ���Դ����ұ�  
	SenceUserMap    m_mapSenceUser;             //���������û��ļ���

	CT_BOOL			m_bResActive;				//��Դ�Ļ
	ResActiveMap    m_ResActiveMap;				//��Դ��
	CT_INT64		m_lLastTiem;				//ʣ��ʱ��
	std::set<CT_INT32>		m_SceneList;		//���õĳ����б�

public:
	CCResourceGrabMgr(void);
	~CCResourceGrabMgr(void);

private:
	//�������ж�ȡ���е���Դ��ָ����ڴ�
	CT_VOID ProduceResourceSpots(CT_VOID);
	//�õ���Դ����������
	CT_BOOL GetResourceArea(CT_INT32 iScenceID, G_ResourceArea& resArea);
	//�����û��������
	CT_VOID SaveUserProduct(CT_DWORD fid,  CT_DWORD uid, CT_INT32 iRes,CT_INT32 iResItem,CT_INT32 iResType,CT_INT32 iSCID, CT_INT32 iSpot, ENUM_RES_Propduct_Reason iReason );
	//�õ��û���Դ��Ϣ
	CT_VOID GetResourceMes(CT_DWORD dwUserID, const ResourceSpot& resSpot,const CT_INT32 iResID,const CT_INT32 iScenceID,CMD_G_ResSpotInfo& rsi);
public:
	//�����ݿ����һ���Դ�����Ϣ���ָ�����Դ�Ӷ������
	CT_VOID RecoverResourceSpot(CT_INT32 iScenceID, CT_INT32 iSpotID, ResourceSpot&);
	//�û����볡����Ϣ
	CT_VOID EnterSence(CT_INT32 iSenceID, CCUser* pUser);
	//ͬ��������Ϣ
	CT_VOID SysResourceProduct(const MSC_DB_ResourceProduct& rp);
	//�õ���Դ����������
	CT_BOOL GetResourceQuality(CT_INT32 iScenceID, CT_INT32 iSpotID, G_ResourceQuality& resQuality,CT_INT32 &iResType);
	//������Դ������
	CT_INT32 GetResourceSpot(CT_INT32 iScenceID,CT_INT32 iSpot,G_ResourceQuality& resQuality,CT_INT32 iGrade);
	//��������
	CT_FLOAT GetProductionLimit(CT_INT32 iSceneID,CT_INT32 iGrade);

	CT_VOID SetResourceScene(CT_INT32 iSceneID);
	CT_BOOL GetResourceScene(CT_CHAR *pBuf, CT_INT32 &bufLen);

private:
	//������Դ��ս��
	CT_VOID AddSpotToFTeam(CT_INT32 iScenceID, CT_INT32 iSpot, CT_DWORD fTeamID);
	//ɾ����Դ��ս��
	CT_VOID EaseSpotFromFTeam(CT_INT32 iScenceID, CT_INT32 iSpot, CT_DWORD fTeamID);
public:
	//�û��˳�
	CT_VOID UserExit(CT_DWORD dwUserID);

public:
	//ս�ӵõ�ӵ�е���Դ��
	CT_INT32 TeamFindResSpot(CT_DWORD teamID, ResSpotTag& rst);
	//���������Ϣ �û��������߳�����ʱ�����
	CT_VOID  TeamKickOutUser(CT_DWORD teamID, CT_DWORD uid);
	//ս�ӽ�ɢ
	CT_VOID  TeamDissolution(CT_DWORD teamID);

public:
	//ս�ӵĵõ���Դ��������
	CT_BOOL TeamFindResSpotGuard(CT_INT32 iSenceID, CT_INT32 iSpot, GuardTag& gt);
	//����ս������Դ���ϵ�����
	CT_BOOL TeamSetResSpotGuard(CCFightTeam* pFightTeam, CT_INT32 iSenceID, CT_INT32 iSpot, GuardTag& gt);

public:
	//�����Դ����Ϣ
	CT_VOID  ResetResourceInfo(CT_INT32 iSenceID, CT_INT32 iSpot);
	//�õ���Դ������Ϣ
	CT_BOOL  GetResourceBase(CT_INT32 iSenceID, CT_INT32 iSpot, ResourceBase& protecTime);
	//������Դ������Ϣ
	CT_BOOL  SetResourceBase(CT_INT32 iSenceID, CT_INT32 iSpot, const ResourceBase& protecTime);

	//�õ�Ŀǰս��ռ�����Դ������
	CT_INT32 GetResSpotNum(CT_DWORD iFightTeamID);
	//�õ�Ŀǰ����Դ���ϸ��û�����״̬
	CT_BOOL  GetResProduct(CT_DWORD dwUserID, CT_INT32 iSenceID, CT_INT32 iSpot,ResourceProduct& rp);

	//�Ƿ��Ѿ���ռ��
	CT_BOOL  DoResSpotOccupy(CT_DWORD iFightTeamID, CT_INT32 iSenceID, CT_INT32 iSpot);
	//��Դ������
	CT_BOOL  SpotProduct(CCUser* pUser, CT_INT32 iScenceID, CT_INT32 iSpotID, CT_DWORD dwTeamID);
	
	//��Դ����
	CT_INT32 ResSpotOccupy(CT_INT32 iSenceID, CT_INT32 iSpot, CT_DWORD iTeamID);

public:
	//������Դ��
	CT_BOOL FindSpot(CT_INT32 iScenceID, CT_INT32 iSpotID);
	//�û��˳�����
	CT_VOID UserExitScence(CT_INT32 iScenceID, CT_DWORD dwUserID);
	//ս�ӷ�����Դ��
	CT_VOID TeamGiveUpSpot(const CMD_G_ResSpotApply& rsa);
	//�û�����
	CT_VOID UserHavestSpotRes(CCFightTeam* pFightTeam, CCUser* pUser, const CMD_G_ResSpotApply& rsa);
	CT_VOID UserHavestSpotRes(CCFightTeam* pFightTeam, CT_DWORD dwUserID, const CMD_G_ResSpotApply& rsa);
public:
	//�����㲥
	CT_VOID BroadcastSence(CT_INT32 iScenceID, CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_INT32 bufLen);

public:
	//����������Ϣ
	CT_VOID UpdateProduct();
	//�õ�ȫ���ı�����Ϣ
	CT_VOID GetAllInfoToSave(std::string& info);
	//�õ��û���Դ����
	CT_BOOL GetUserResData(CT_DWORD dwUserID, CT_CHAR* buf, CT_INT32& bufLen);

private:
	//�������
	CT_VOID CheckProduct(CT_INT32 iSceneID, CT_INT32 iSpotID, const ResourceBase& resBase, ResourceProduct& rp);
	//��������λ��
	CT_BOOL FindProductPos(const ResourceProduct* pResProduct, CT_DWORD uid, CT_INT32& pos);
	//����������Ľ��
	CT_INT32 CalFightTeamType(CCFightTeam* pFightTeam, CT_INT32 iRes, CT_DWORD dwUserID);
public:
	//�㲥פ����Ϣ
	CT_VOID BroadcastSenceGuard(CT_INT32 iScenceID, CT_INT32 iSpotID, CCFightTeam* pFightTeam,  CT_CHAR pVictorySign[SPOT_VECTORY_SIGN_SIZE], CT_DWORD pDependPlayer[SPOT_GUARD_MEMBER]);

public:
	//���û
	CT_VOID SetResActive(CT_BOOL bResActive);
	//��ȡ�
	CT_BOOL GetResActive(){_AUTOMUTEX(&m_mutex);return m_bResActive;};
	//������Դ��
	CT_VOID InsterResActive(CT_CHAR *pBuf,CT_INT32 iLen);
	//��ȡ��Դ��
	CT_BOOL GetResActiveSpot(CT_CHAR *pBuf,CT_INT32 &iLen);
	//�ǿ���ջ�
	CT_VOID ResActiveHavest();
	//����ʣ��ʱ��
	CT_VOID SetLastTime(CT_INT64 lLastTime){_AUTOMUTEX(&m_mutex);m_lLastTiem=lLastTime;};
	//��ȡʣ��ʱ��
	CT_INT64 GetLastTime(){_AUTOMUTEX(&m_mutex);return m_lLastTiem;};
	//��ȡ���Դ��
	CT_BOOL GetActiveResSpot(CT_INT32 iScenceID,CT_INT32 iSpotID);
	//��ȡ���Դ��
	CT_BOOL GetActiveResSpot(CT_INT32 iScenceID,CT_INT32 iSpotID,ResActive &ra);
	//�����������
	CT_VOID RandProductType();
	//�������Ϣ
	CT_VOID ClearSpot(){_AUTOMUTEX(&m_mutex);m_ResActiveMap.clear();};
};

#endif