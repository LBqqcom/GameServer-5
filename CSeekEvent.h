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
	CT_FLOAT		m_fFarmland;		//ũ���Ӷ����(��߻�Э�̣���ֵ�̶�����ı�)
	CT_FLOAT		m_fPanning;			//����Ӷ����(��߻�Э�̣���ֵ�̶�����ı�)
	Totem			m_Totem;			//ͼ���Ӷ����(��߻�Э�̣���ֵ�̶�����ı�)
public:
	CCSeekEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCSeekEvent();

private:
	// �����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve);
	// ��ʱ
	virtual CT_VOID OnTime(CT_DWORD timeID);
	// ����
	virtual CT_VOID OnClock(CT_DWORD timeID);

private:
	// ˢ�±�̽����
	CT_VOID DisposeSeekedPlayerUpdate(CT_CHAR* pBuf, CT_INT32 iLen);
	// ����̽������
	CT_VOID DisposeSeek(CT_CHAR* pBuf, CT_INT32 iLen, CT_INT32 iType);
	// ����
	CT_VOID DisposePresent(CT_CHAR* pBuf, CT_INT32 iLen);
	// ��������������
	CT_VOID DisposeBuildingUpSpeed(CT_CHAR* pBuf, CT_INT32 iLen);
	//��ǽ�ӳ�
	CT_VOID DisposeWallAddition(CT_CHAR* pBuf, CT_INT32 iLen);
	//�Ӷ�
	CT_VOID DisposePlunder(CT_CHAR* pBuf, CT_INT32 iLen);
	//�ε�����
	CT_VOID DisposePunderFlag(CT_CHAR* pBuf, CT_INT32 iLen);
	//��Ʊ����
	CT_VOID DisposeKidnap(CT_CHAR* pBuf, CT_INT32 iLen);
	//Ӫ������
	CT_VOID DisposeRescue(CT_CHAR* pBuf, CT_INT32 iLen);
	//��ǽ����
	CT_VOID DisposeArmyApply(CT_CHAR* pBuf, CT_INT32 iLen);
	// �������˳���
	CT_VOID DisposeEnterOthersTown(CT_CHAR* pBuf,CT_INT32 iLen);
	//����
	CT_VOID DisoposeAddProduct(CT_CHAR* pBuf,CT_INT32 iLen);
	//��ȡ����
	CT_VOID DisposeGetPresent(CT_CHAR* pBuf, CT_INT32 iLen);
	//��Ӫ
	CT_VOID DisposeSeekBack(CT_CHAR* pBuf, CT_INT32 iLen);
	//�鿴ս��
	CT_VOID DisposeLookFighting(CT_CHAR* pBuf, CT_INT32 iLen);
	//�����Ӷ�����
	CT_VOID DisposePlunderRequest(CT_CHAR* pBuf, CT_INT32 iLen);

public:
	//��Ʊս��
	CT_VOID DisposeKidnapFight(CT_CHAR* pBuf, CT_INT32 iLen);
	//Ӫ��ս��
	CT_VOID DisposeRescueFight(CT_CHAR* pBuf, CT_INT32 iLen);
	//�Ӷ���
	CT_VOID DisposePlunderFight(CT_CHAR* pBuf, CT_INT32 iLen);

public:
	//��ǽ����������֤
	CT_INT32 WallAdditionValidate(CCUser *pUser,const CT_INT32 iID,RewardCost &rc);
	//��ǽ������֤
	CT_INT32 WallArmyValidate(CCUser *pUser,CT_INT32 iPos[POST_COUNT/2]);
	//��Ʊ��֤
	CT_INT32 KidnapValidate(CCUser *pUser,CMD_G_SeekKidnapApply *pSka);
	//Ӫ����֤
	CT_INT32 RescueValidate(CCUser *pUser,CMD_G_SeekRescurApply *pSra);
	//�Ӷ���֤
	CT_INT32 PlunderValidete(CCUser *pUser,CMD_G_SeekPlunderApply *pSpa);
	//������֤
	ENUM_BuildingAdditionFail BuildProValidate(CCUser *pUser,const CT_INT32 iBuildID);
	//��ֵ����
	CT_INT32 SeekValueCorrection(CT_INT32 iType,CT_INT32 iValue);
	//ս������
	static CT_VOID MD5FightIndex(CT_UCHAR *pDst,CT_DWORD dwDstID,CT_DWORD dwMyID,CT_INT32 iType);

private:
	//�õ����˳�����Ϣ
	CT_VOID DisposeGetOthersTownInfo(CT_CHAR* pBuf,CT_INT32 iLen);
	//�õ���������Ϣ
	CT_VOID DisposeSeekedGet(CT_CHAR* pBuf,CT_INT32 iLen);

};
#endif