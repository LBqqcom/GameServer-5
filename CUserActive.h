#ifndef CUserActive_H_
#define CUserActive_H_

#include <map>
#include <vector>
#include "CUser.h"
#include "CMD_Game.h"
#include "GlobeScriptInterface.h"

#define ACTIVE_MONTH_CARD				4101 //月卡活动

struct MSC_DB_Present;
struct ActiveStatus
{
	CT_INT32		iStatus;		//活动状态
	CT_INT32		iQuantity;    //累计活动完成量
};

struct MSC_DB_ActiveEventChange;

class CCUser;
class CCUserActive
{
	friend class CCUser;
private:
	CCUser*             m_pUser; 
	CCMutex*			m_pMutex;
	//只有两种情况记录数据库，1，活动已经领取，2，积累型的活动，目前是充值和消费钻石，其他的活动已完成也不用记录数据库，不对，已完成的也要记录数据库
	typedef std::map<CT_INT32,ActiveStatus>		ActiveStatusMap;			//活动状态map
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
	//初始化
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex)
	{
		m_pUser = pUser;
		m_pMutex = pMutex;
		m_ActiveStatusMap.clear();
		m_iRefTime = time(0);
	}

	//清理用户数据
	CT_VOID Clear();
public:
	CCUserActive();
	~CCUserActive();
public:
	CT_BOOL		CheckActiveWhetherComplete(CT_INT32 iAID);		//查看活动是否已经完成，可领取奖励
	CT_BOOL		CheckActiveReceive(CT_INT32 iAID);		//查看活动是否已经领取奖励
	CT_VOID		ActiveRewardGet(CT_INT32 iAID);			//活动奖励领取
	CT_VOID		PushNewAddActive(MSC_DB_ActiveEventChange &activeAdd);						//推动用户新增活动
	CT_BOOL		CheckActiveTypeUsed(CT_INT32 iType);		//判断活动类型用户是否能参与
	CT_VOID		PushDelActive(MSC_DB_ActiveEventChange &activeDel);					//推送删除活动
	CT_VOID		SetActiveStatus(CT_INT32 iAID,CT_INT32 iStatus,CT_INT32 iQuanity);							//获取活动状态
	CT_VOID		SendUserActiveInfo();
	CT_BOOL		CheckActiveComplete(CT_INT32 iAID);		//接到活动的时候，判断活动是否已经完成
	CT_VOID		UpdateActiveStatus(CT_INT32 iAID,CT_INT32 iType,CT_INT32 iQuanity,G_ActiveEventData &activeData);	//更新活动状态
	CT_VOID		UpdateActiveOnline();	//更新在线奖励活动
	CT_BOOL		CheckActivePre(CT_INT32 iAEID);	//检查前置任务是否已完成
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

	CT_BOOL		ActivedDragonBall(CMD_G_DragonBallFindFail &dbff);			//激活龙珠
	CT_BOOL 	ActiveDragonBallReward(CT_INT32 iRewardID, ENUM_DragonBallFial &enfail, CMD_G_DragonBallFindSuc *pdbfs = 0);		//领取龙珠奖励

	CT_VOID		UpdataDragonBallStatus();	//零点更新龙珠状态
	//零点更新月卡状态
	CT_VOID		UpdataMonthCardStatus();	
};

#endif


