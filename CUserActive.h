#ifndef CUserActive_H_
#define CUserActive_H_

#include <map>
#include <vector>
#include "CUser.h"
#include "CMD_Game.h"
#include "GlobeScriptInterface.h"

#define ACTIVE_MONTH_CARD				4101 //�¿��

struct MSC_DB_Present;
struct ActiveStatus
{
	CT_INT32		iStatus;		//�״̬
	CT_INT32		iQuantity;    //�ۼƻ�����
};

struct MSC_DB_ActiveEventChange;

class CCUser;
class CCUserActive
{
	friend class CCUser;
private:
	CCUser*             m_pUser; 
	CCMutex*			m_pMutex;
	//ֻ�����������¼���ݿ⣬1����Ѿ���ȡ��2�������͵Ļ��Ŀǰ�ǳ�ֵ��������ʯ�������Ļ�����Ҳ���ü�¼���ݿ⣬���ԣ�����ɵ�ҲҪ��¼���ݿ�
	typedef std::map<CT_INT32,ActiveStatus>		ActiveStatusMap;			//�״̬map
	typedef std::map<CT_INT32,ActiveStatus>::iterator		ActiveStatusMap_IT;

	ActiveStatusMap		m_ActiveStatusMap;
	CT_INT32			m_TurntableCount[3];
	CT_INT64			m_iRefTime;

	typedef std::map<CT_INT32, CMD_G_Present>	ActivePresentMap;
	ActivePresentMap							m_PresentMap;

	struct PresentDesc
	{
		CT_INT32 iSLotID;
		CT_CHAR Desc[255];
	};
	std::map<CT_INT32, PresentDesc>			m_PresentDescMap;
public:
	//��ʼ��
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex)
	{
		m_pUser = pUser;
		m_pMutex = pMutex;
		m_ActiveStatusMap.clear();
		m_iRefTime = time(0);
	}

	//�����û�����
	CT_VOID Clear();
public:
	CCUserActive();
	~CCUserActive();
public:
	CT_BOOL		CheckActiveWhetherComplete(CT_INT32 iAID);		//�鿴��Ƿ��Ѿ���ɣ�����ȡ����
	CT_BOOL		CheckActiveReceive(CT_INT32 iAID);		//�鿴��Ƿ��Ѿ���ȡ����
	CT_VOID		ActiveRewardGet(CT_INT32 iAID);			//�������ȡ
	CT_VOID		PushNewAddActive(MSC_DB_ActiveEventChange &activeAdd);						//�ƶ��û������
	CT_BOOL		CheckActiveTypeUsed(CT_INT32 iType);		//�жϻ�����û��Ƿ��ܲ���
	CT_VOID		PushDelActive(MSC_DB_ActiveEventChange &activeDel);					//����ɾ���
	CT_VOID		SetActiveStatus(CT_INT32 iAID,CT_INT32 iStatus,CT_INT32 iQuanity);							//��ȡ�״̬
	CT_VOID		SendUserActiveInfo();
	CT_BOOL		CheckActiveComplete(CT_INT32 iAID);		//�ӵ����ʱ���жϻ�Ƿ��Ѿ����
	CT_VOID		UpdateActiveStatus(CT_INT32 iAID,CT_INT32 iType,CT_INT32 iQuanity,G_ActiveEventData &activeData);	//���»״̬
	CT_VOID		UpdateActiveOnline();	//�������߽����
	CT_BOOL		CheckActivePre(CT_INT32 iAEID);	//���ǰ�������Ƿ������
	CT_VOID		DisposNextActive(CT_INT32 iAEID);
	CT_VOID		PushTurntable();
	CT_VOID		SetTurntableCount(CT_INT32 iTimes1,CT_INT32 iTimes2,CT_INT32 iTimes3);
	CT_INT32	GetTurntableCount(CT_INT32 iTurntableID);
	CT_INT32	AddTurntableCount(CT_INT32 iTurntableID);
	CT_INT32    GetActiveQuanity(const CT_INT32 iAID);

	CT_BOOL		GetPresent(CT_INT32 iIndex, CMD_G_Present& present);
	CT_BOOL		GetPresent(CT_CHAR *pBuf, CT_INT32 &bufLen);
	CT_BOOL		GetPresentDesc(CT_CHAR *pBuf, CT_INT32 &bufLen);
	CT_VOID		InsertPresent(MSC_DB_Present &dbPresent);
	CT_VOID		AddPresent(MSC_DB_Present &dbPresent);
	CT_VOID		DeletePresent(CT_INT32 iIndex) {_AUTOMUTEX(m_pMutex); m_PresentMap.erase(iIndex);}
	CT_BOOL		CheckPresent(CT_INT32 iIndex);

	CT_BOOL		ActivedDragonBall(CMD_G_DragonBallFindFail &dbff);			//��������
	CT_BOOL 	ActiveDragonBallReward(CT_INT32 iRewardID, ENUM_DragonBallFial &enfail, CMD_G_DragonBallFindSuc *pdbfs = 0);		//��ȡ���齱��

	CT_VOID		UpdataDragonBallStatus();	//����������״̬
	//�������¿�״̬
	CT_VOID		UpdataMonthCardStatus();	
};

#endif


