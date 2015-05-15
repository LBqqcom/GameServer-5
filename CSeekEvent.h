#ifndef CSEEKEVENT_H_
#define CSEEKEVENT_H_

#include "CGameMesEvent.h"
#include "CGDefine.h"

class CCUser;
struct RewardCost;
struct CMD_G_SeekKidnapApply;
struct CMD_G_SeekRescurApply;
struct CMD_G_SeekPlunderApply;
class CCSeekEvent : public CCGameMesEvent
{
	struct Totem
	{
		CT_FLOAT	fFood;
		CT_FLOAT	fGold;
	};
private:
	CT_FLOAT		m_fFarmland;		//农田掠夺比例(与策划协商，此值固定不会改变)
	CT_FLOAT		m_fPanning;			//金矿掠夺比例(与策划协商，此值固定不会改变)
	Totem			m_Totem;			//图腾掠夺比例(与策划协商，此值固定不会改变)
public:
	CCSeekEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCSeekEvent();

private:
	// 处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve);
	// 定时
	virtual CT_VOID OnTime(CT_DWORD timeID);
	// 定点
	virtual CT_VOID OnClock(CT_DWORD timeID);

private:
	// 刷新被探访人
	CT_VOID DisposeSeekedPlayerUpdate(CT_CHAR* pBuf, CT_INT32 iLen);
	// 处理探访请求
	CT_VOID DisposeSeek(CT_CHAR* pBuf, CT_INT32 iLen, CT_INT32 iType);
	// 送礼
	CT_VOID DisposePresent(CT_CHAR* pBuf, CT_INT32 iLen);
	// 生产、升级加速
	CT_VOID DisposeBuildingUpSpeed(CT_CHAR* pBuf, CT_INT32 iLen);
	//城墙加成
	CT_VOID DisposeWallAddition(CT_CHAR* pBuf, CT_INT32 iLen);
	//掠夺
	CT_VOID DisposePlunder(CT_CHAR* pBuf, CT_INT32 iLen);
	//拔掉旗帜
	CT_VOID DisposePunderFlag(CT_CHAR* pBuf, CT_INT32 iLen);
	//绑票请求
	CT_VOID DisposeKidnap(CT_CHAR* pBuf, CT_INT32 iLen);
	//营救请求
	CT_VOID DisposeRescue(CT_CHAR* pBuf, CT_INT32 iLen);
	//城墙军队
	CT_VOID DisposeArmyApply(CT_CHAR* pBuf, CT_INT32 iLen);
	// 进入他人城镇
	CT_VOID DisposeEnterOthersTown(CT_CHAR* pBuf,CT_INT32 iLen);
	//增产
	CT_VOID DisoposeAddProduct(CT_CHAR* pBuf,CT_INT32 iLen);
	//领取礼物
	CT_VOID DisposeGetPresent(CT_CHAR* pBuf, CT_INT32 iLen);
	//回营
	CT_VOID DisposeSeekBack(CT_CHAR* pBuf, CT_INT32 iLen);
	//查看战斗
	CT_VOID DisposeLookFighting(CT_CHAR* pBuf, CT_INT32 iLen);
	//界面掠夺请求
	CT_VOID DisposePlunderRequest(CT_CHAR* pBuf, CT_INT32 iLen);

public:
	//绑票战果
	CT_VOID DisposeKidnapFight(CT_CHAR* pBuf, CT_INT32 iLen);
	//营救战果
	CT_VOID DisposeRescueFight(CT_CHAR* pBuf, CT_INT32 iLen);
	//掠夺结果
	CT_VOID DisposePlunderFight(CT_CHAR* pBuf, CT_INT32 iLen);

public:
	//城墙技能升级验证
	CT_INT32 WallAdditionValidate(CCUser *pUser,const CT_INT32 iID,RewardCost &rc);
	//城墙军队验证
	CT_INT32 WallArmyValidate(CCUser *pUser,CT_INT32 iPos[POST_COUNT/2]);
	//绑票验证
	CT_INT32 KidnapValidate(CCUser *pUser,CMD_G_SeekKidnapApply *pSka);
	//营救验证
	CT_INT32 RescueValidate(CCUser *pUser,CMD_G_SeekRescurApply *pSra);
	//掠夺验证
	CT_INT32 PlunderValidete(CCUser *pUser,CMD_G_SeekPlunderApply *pSpa);
	//增产验证
	ENUM_BuildingAdditionFail BuildProValidate(CCUser *pUser,const CT_INT32 iBuildID);
	//数值纠正
	CT_INT32 SeekValueCorrection(CT_INT32 iType,CT_INT32 iValue);
	//战报索引
	static CT_VOID MD5FightIndex(CT_UCHAR *pDst,CT_DWORD dwDstID,CT_DWORD dwMyID,CT_INT32 iType);

private:
	//得到他人城镇信息
	CT_VOID DisposeGetOthersTownInfo(CT_CHAR* pBuf,CT_INT32 iLen);
	//得到被访者信息
	CT_VOID DisposeSeekedGet(CT_CHAR* pBuf,CT_INT32 iLen);

};
#endif