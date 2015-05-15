#ifndef CUserBlackMarket_H_
#define CUserBlackMarket_H_

#include <map>
#include <vector>
#include "CUser.h"
#include "CMD_Game.h"
#include "GlobeScriptInterface.h"

struct MarketPersonalAll;
struct BlackGoodsStatus
{
	CT_INT32	iGoodsID;		//��Ʒid
	CT_INT32	iNum;			//��Ʒ����
};
typedef BlackGoodsStatus	MarketPersonal;

typedef std::map<CT_INT32,BlackGoodsStatus>				BlackGoodsStatusMap;
typedef std::map<CT_INT32,BlackGoodsStatus>::iterator	BlackGoodsStatusMap_IT;

typedef std::map<CT_INT32,MarketPersonal>						MarketPersonalMap;
typedef std::map<CT_INT32,MarketPersonal>::iterator			MP_IT;

typedef std::vector<CT_INT32>						GoodsVector;	//��Ʒ��id vector

struct MSC_DB_MarketFunction;
struct MSC_DB_BlackMarketRefresh;

class CCUserBlackMarket
{
	friend class CCUser;
private:
	CCUser*             m_pUser; 
	CCMutex*			m_pMutex;
	
	BlackGoodsStatusMap		m_blackGoodsStatusMap;		//�û��ڵ���Ʒmap
	BlackGoodsStatusMap		m_blackGoodsSeekMap;		//�û�̽�úڵ���Ʒmap
	CT_INT64			iLastRefTime;					//���һ��ϵͳˢ��ʱ��	zhoujun

	CT_INT32			iMall;							//�̳ǹ���״̬
	CT_INT32			iBlackMarket;					//�ڵ깦��״̬
	CT_INT32			iRefreshTime;					//ʣ�ึ��ˢ�´���
	GoodsVector			m_GoodsIDVector;				//��Ʒ��id vector����������Ȩ�������ȡ��Ʒ
	GoodsVector			m_GoodsWeightVector;			//��ƷȨ��vector

	MarketPersonalMap	m_MarketPersonalMap;			//�����޹�
public:
	CCUserBlackMarket();
	~CCUserBlackMarket();
public:
	//��ʼ��
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex)
	{
		m_pUser = pUser;
		m_pMutex = pMutex;
		iMall=0;
		iBlackMarket=0;
		iRefreshTime=0;
		m_blackGoodsStatusMap.clear();
		m_MarketPersonalMap.clear();
		m_blackGoodsSeekMap.clear();
	}

	//�����û�����
	CT_VOID Clear()
	{
		_AUTOMUTEX(m_pMutex);
		iMall=0;
		iBlackMarket=0;
		iRefreshTime=0;
		m_blackGoodsStatusMap.clear();
		m_MarketPersonalMap.clear();
		m_blackGoodsSeekMap.clear();
	}
private:
	CT_INT32 RandGoodsBlackMarket(CT_INT32 iNum);
	CT_VOID	GetGoodsWeightVector(GoodsVector &v_id,GoodsVector &v_weight,CT_INT32 iGoodsIDList[],CT_INT32 &iWeight,CT_INT32 iNum,CT_INT32 iGrade);

public:
	//��������޹�
	CT_VOID InsertMarketPersonal(const CT_INT32 iGoodsID,const CT_INT32 iNum);
	//��ȡ�����޹�
	CT_BOOL GetMarketPersonal(CT_CHAR *pBuf,CT_INT32 &iLen);
	//���¸����޹�
	CT_VOID UpdataMarketPersonal(CT_CHAR *pBuf,CT_INT32 iLen);
	//������Ʒ
	CT_INT32 BuyGood(CT_INT32 iGoods,const CT_INT32 iNum);

public:
	//�����������ȡpos
	CT_INT32 GetPosByRandNum(GoodsVector &v_weight,CT_INT32 begin,CT_INT32 end,CT_INT32 iRand);
	//�ڵ�ϵͳˢ�£���ȡ��Ʒ���͸�ÿ���û�
	CT_VOID RefreshBlackMarket(CT_INT32 iType);
	//�ڵ깺��
	CT_BOOL BlackMarketShopping(CT_INT32 iPosition,CT_INT32 iNum,CT_INT32 iMarketType);
	//̽�úڵ깺��
	CT_BOOL SeekBlackMarketShopping(CT_INT32 iPosition,CT_INT32 iNum,CT_INT32 iMarketType);
	CT_INT32 GetBlackMarketGoodsID(CT_INT32 iPosition);
	CT_INT32 GetSeekBlackMarketGoodsID(CT_INT32 iPosition);
	//�����û�ˢ�´���
	CT_BOOL RefreshOnce();
	//�жϺڵ��Ƿ���
	CT_BOOL	ViewBlackMarket();
	//�ж��̳��Ƿ���
	CT_BOOL	ViewMallMarket();
	//�����û��ڵ���Ϣ
	CT_VOID SendBlackMarketInfo();
	//�����û�ʣ��ˢ�´���
	CT_VOID SendBlackRefreshTime();
	//�����̵깦��
	CT_VOID OpenMallMarketFunction();
	//�����ڵ깦��
	CT_VOID OpenBlackMarketFucntion();
	//��½��ȡ�̵�״̬
	CT_VOID GetMarketStatus(MSC_DB_MarketFunction &marketFun);
	//��½��ȡ�ڵ�״̬
	CT_VOID GetBlackMarketInfo(MSC_DB_BlackMarketRefresh &blackMarketInfo);
	//ˢ���̳Ǻ��̵���Ʒ
	CT_VOID RefreshMarketGoods();
	//������Ʒʱ��ˢ���̳ǣ��̵�������Ʒ����
	CT_VOID UpdateSingleGoodsStatus(CMD_G_MarketGoodsInfo &marketGoodsInfo);
	//���¼����̵�ڵ���ȴʱ��
//	CT_VOID RefreshShopTime();;
	//̽�ú���
	CT_VOID SendSeekBlackMarketInfo();
};

#endif