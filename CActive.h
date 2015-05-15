#ifndef CActive_H_
#define CActive_H_

#include "CSingleton.h"
#include "CTType.h"
#include <map>
#include <vector>
#include "GlobeScriptInterface.h"
#include "CMutex.h"
#include "CUser.h"

//�����
struct ActiveDsc
{
	CT_INT32		iType;
	CT_CHAR			cBuf[256];
};

struct TurntableItem
{
	CT_INT32	iOpen;							//�Ƿ���
	CT_INT32	iWeight[TURNTABLE_ITEM_COUNT];	//Ȩ��
	CT_INT32	iRewardID[TURNTABLE_ITEM_COUNT];
	CT_INT32	iFakeProID[TURNTABLE_ITEM_COUNT];	//α����ID
	G_NomalValue gReward[TURNTABLE_ITEM_COUNT];		//����
};

struct TimeActiveStatus
{
	CT_INT32	iAEID;		//��ǰ/����ID
	CT_INT32	iStatus;	//0.�ر� 1.����
	CT_INT64	iTime;		//
};

typedef std::vector<CT_INT32>				ActiveEventVector;
typedef std::map<CT_INT32, TurntableItem>	TurntableMap;		//TurntableID:Turntable
typedef	std::map<CT_INT32,ActiveDsc>		ActiveDscMap;	// �����
typedef std::map<CT_INT32,G_ActiveEventReward> ActiveRewardMap;
typedef std::map<CT_INT32, TimeActiveStatus> TimeActiveStatusMap;

struct MSC_DB_TurntableRelation;
class CCActive : public CCSingleton<CCActive>
{
private:
	CCMutex   m_mutex;
public:
	CCActive();
	~CCActive();
private:
	ActiveEventVector		m_ActiveEventVector;
	TurntableMap			m_TurntableMap;
	ActiveDscMap			m_ActiveDscMap;
	ActiveRewardMap			m_RewardMap;
	TimeActiveStatusMap		m_TimeActiveStatusMap;
public:
	//��������
	CT_VOID		InsterActiveDsc(CT_CHAR *pBuf,CT_INT32 iLen);
	//���벹�佱����
	CT_VOID		InsertReward(G_ActiveEventReward &reward);
	CT_BOOL		GetReward(CT_INT32 iRewardID, G_ActiveEventReward *pReward);
	CT_BOOL		GetReward(CT_CHAR *pBuf, CT_INT32 &bufLen);
	//��ȡ�����
	CT_BOOL		GetActiveDsc(CT_CHAR *pBuf,CT_INT32 &iLen);
	//��ʱˢ�»
	CT_VOID		UpdateActive(ActiveEventVector &v_active,MSC_DB_ActiveEventChange &activeEventDelete, MSC_DB_ActiveEventChange &activeEventAdd);
	//��ȡ���еĻid
	CT_VOID		GetServerActive(MSC_DB_ActiveEventChange &activeList);
	//�����
	CT_VOID		ActiveProcess(CT_INT32 iType,CT_INT32 iQuanity,CCUser *pUser, CT_INT32 iAID=0);
	//��Ʒ���ۻ����ȡ��Ʒ���ۿ�
	CT_FLOAT	GetMarketDiscount(CT_INT32	iGoodsID);
	//��ȡĳ���ͻ�Ļ�б�
	CT_INT32	GetActiveByType(CT_INT32 iType, CT_INT32 *ActiveList);
	//ͨ���ID�õ������
	CT_INT32	GetActiveType(CT_INT32 iAID);
	//ɾ��ת�̻
	CT_VOID		DelTurntableActive(CT_INT32 iAID);
	//����ת�̻
	CT_VOID		NewTurntableActive(CT_INT32 iAID);
	CT_VOID		RefTurntable();

	CT_VOID		GetTurntableItem(CT_INT32 iTurntableID, TurntableItem *Item);
	CT_VOID		SetTurntableItem(MSC_DB_TurntableRelation &ttr);

	CT_VOID		SetTimeActiveStatus(CT_INT32 iType, CT_INT32 iAEID, CT_INT64 iTime, CT_INT32 iStatus=0);
	CT_VOID		GetTimeActiveStatus(CT_INT32 iType, CT_INT32 &iAEID, CT_INT64 &iTime, CT_INT32 &iStatus);
private:
	//���ת�̽���
	CT_INT32	GetPosByRandNum(G_TurntableRelation *ttrList,CT_INT32 begin,CT_INT32 end,CT_INT32 iRand);

};

#endif