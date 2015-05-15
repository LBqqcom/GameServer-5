#ifndef CCLOGDBEVNET_H_
#define CCLOGDBEVNET_H_
#include "CGameMesEvent.h"

class CCDBConnector;
class CCLogDBEvent:public CCGameMesEvent
{
private:
	CCDBConnector& m_dbConnector;
public:
	CCLogDBEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCLogDBEvent(void);

private:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}

private:
	//����������¼
	CT_VOID DisposeArmyUpdataGrade(CT_CHAR *pBuf,CT_INT32 iLen);
	//���ӽ���
	CT_VOID DisposeArmyUpdataRank(CT_CHAR *pBuf,CT_INT32 iLen);
	//���򱳰����Ӽ�¼
	CT_VOID DisposeBagSlotUpdata(CT_CHAR *pBuf,CT_INT32 iLen);
	//���ӳ��м�¼
	CT_VOID DisposeMasterArea(CT_CHAR *pBuf,CT_INT32 iLen);
	//������ǿ
	CT_VOID DisposePropAddStrengthen(CT_CHAR *pBuf,CT_INT32 iLen);
	//����ʹ�����¼
	CT_VOID DisposePropFinish(CT_CHAR *pBuf,CT_INT32 iLen);
	//���߼̳м�¼
	CT_VOID DisposePropInherit(CT_CHAR *pBuf,CT_INT32 iLen);
	//������ǿ
	CT_VOID DisposePropUpdataTrain(CT_CHAR *pBuf,CT_INT32 iLen);
	//���漼�ܼ�¼
	CT_VOID DisposeSaveSkill(CT_CHAR *pBuf,CT_INT32 iLen);
	//�¼���ɼ�¼
	CT_VOID DisposeSaveEventFinish(CT_CHAR *pBuf,CT_INT32 iLen);
	//�����츳��¼
	CT_VOID DisposeSaveTalent(CT_CHAR *pBuf,CT_INT32 iLen);
	//����������ɼ�¼
	CT_VOID DisposeTaskFinish(CT_CHAR *pBuf,CT_INT32 iLen);
	//�û��˳���¼
	CT_VOID DispseUserExit(CT_CHAR *pBuf,CT_INT32 iLen);
	//�û�������¼
	CT_VOID DisposeUpGradeFinish(CT_CHAR *pBuf,CT_INT32 iLen);
	//vip�����¼
	CT_VOID DisposeVIPApply(CT_CHAR *pBuf,CT_INT32 iLen);
	//��Ծ�Ƚ�����¼
	CT_VOID DisposeActiveReward(CT_CHAR *pBuf,CT_INT32 iLen);
	//�򿪱���
	CT_VOID DisposeOpenChest(CT_CHAR* pBuf,CT_DWORD len);
	//��������ͳ��
	CT_VOID DisposeOnlineUser(CT_CHAR *pBuf,CT_INT32 iLen);
	//�������ͳ��
	CT_VOID	DisposeChatTime(CT_CHAR *pBuf,CT_INT32 iLen);
	//�û�������¼
	CT_VOID DisposeProductFinish(CT_CHAR *pBuf,CT_INT32 iLen);
	//�û���ֵ��¼
	CT_VOID DisposeCharge(CT_CHAR *pBuf,CT_INT32 iLen);
	//�û���ֵ����
	CT_VOID DisposeChargeUpdate(CT_CHAR *pBuf,CT_INT32 iLen);
	//ÿ��ǩ������
	CT_VOID DisposeSignReward(CT_CHAR *pBuf,CT_INT32 iLen);
	//����������������
	CT_VOID DisposePetUpSkill(CT_CHAR *pBuf,CT_INT32 iLen);
	//��������
	CT_VOID DisposePetRename(CT_CHAR *pBuf,CT_INT32 iLen);
	//��Ʒ����
	CT_VOID	DisposeItemSell(CT_CHAR *pBuf,CT_INT32 iLen);
	//�̵깺��
	CT_VOID DisposeShopping(CT_CHAR *pBuf,CT_INT32 iLen);
	//��������
	CT_VOID	DisposeStatminaBuy(CT_CHAR *pBuf,CT_INT32 iLen);
	//pvp����
	CT_VOID DisposePVPReward(CT_CHAR *pBuf,CT_INT32 iLen);
	//����ս������
	CT_VOID DisposeDuplicate(CT_CHAR *pBuf,CT_INT32 iLen);
    // ��ħ��¼
    CT_VOID DisposeEnchant(CT_CHAR *pBuf,CT_INT32 iLen);
	//����������ȡ
	CT_VOID DisposeDuplicateReward(CT_CHAR *pBuf,CT_INT32 iLen);
	//������������
	CT_VOID DisposeDuplicateEnter(CT_CHAR *pBuf,CT_INT32 iLen);
	//������ս������
	CT_VOID DisposePVPFightingCost(CT_CHAR *pBuf,CT_INT32 iLen);
	//�����
	CT_VOID DisposeActiveEventReward(CT_CHAR *pBuf,CT_INT32 iLen);
	//ս���������
	CT_VOID DisposeBuyFightingTime(CT_CHAR *pBuf,CT_INT32 iLen);
	//ǿ��ˢ��
	CT_VOID DisposePVPRefresh(CT_CHAR *pBuf,CT_INT32 iLen);
	//��ʯ����ǿ����ȴʱ��
	CT_VOID DisposeClearCD(CT_CHAR *pBuf,CT_INT32 iLen);
	//��Ϣ����
	CT_VOID DisposeFeedBack(CT_CHAR *pBuf,CT_INT32 iLen);
	//ɨ������
	CT_VOID DisposeRaidsSpeed(CT_CHAR *pBuf,CT_INT32 iLen);
	//zhoujun ���뽱����־
	CT_VOID DisposeWorshipReward(CT_CHAR *pBuf,CT_INT32 iLen);
	//��Ӹ�������
	CT_VOID DiposeTeamDuplicateReward(CT_CHAR *pBuf,CT_INT32 iLen);
	//��Ӹ�������
	CT_VOID DisposeTeamDuplicateCost(CT_CHAR *pBuf,CT_INT32 iLen);
	//��Դ������¼
	CT_VOID DisposeResProductSave(CT_CHAR *pBuf,CT_INT32 iLen);
	//��Դռ���¼
	CT_VOID DisposeResOccpySave(CT_CHAR *pBuf,CT_INT32 iLen);
	//����ٺ»��¼
	CT_VOID DisposeSalary(CT_CHAR *pBuf,CT_INT32 iLen);
	//��������
	CT_VOID DisposeRankingUpGrade(CT_CHAR *pBuf,CT_INT32 iLen);
	//��Ʒʹ�ü�¼
	CT_VOID DisposeUserItem(CT_CHAR *pBuf,CT_INT32 iLen);
	//�¼�������¼
	CT_VOID DisposeEventReward(CT_CHAR *pBuf,CT_INT32 iLen);
	//��������
	CT_VOID DisposePetUpGrade(CT_CHAR *pBuf,CT_INT32 iLen);
	//MasterSaveData
	CT_VOID DisposeMasterSaveData(CT_CHAR *pBuf,CT_INT32 iLen);
	//ս������
	CT_VOID DisposeFightTeamUpGrade(CT_CHAR *pBuf,CT_INT32 iLen);
	//ս���ٻ�����
	CT_VOID DisposeFightTeamSummonCost(CT_CHAR *pBuf,CT_INT32 iLen);
	//ս���ٻ�������Դ
	CT_VOID DisposeFightTeamSummonRewardRes(CT_CHAR *pBuf,CT_INT32 iLen);
	//ս���ٻ���������
	CT_VOID DisposeFightTeamSummonRewardItem(CT_CHAR *pBuf,CT_INT32 iLen);
	//ս�ӿƼ�����
	CT_VOID DisposeFightTeamScienceUpGrade(CT_CHAR *pBuf,CT_INT32 iLen);
	//Ԫ����ͨ��
	CT_VOID DisposeTowerPass(CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID DisposeTowerOccupy(CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID DisposeTowerHelp(CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID DisposeTreasureUse(CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID DisposeAddItem(CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID DisposeBattleFlag(CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID DisposeEatElm(CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID DisposeEmbedEquip(CT_CHAR *pBuf,CT_INT32 iLen);
	//�������
	CT_VOID DisposeCSReward(CT_CHAR *pBuf,CT_INT32 iLen);
	//��������
	CT_VOID DisposeInsporeCost(CT_CHAR *pBuf,CT_INT32 iLen);
	//֧��
	CT_VOID DiposeSupport(CT_CHAR *pBuf,CT_INT32 iLen);
	//ͷ��
	CT_VOID DisposeTitle(CT_CHAR *pBuf,CT_INT32 iLen);
	//����
	CT_VOID DisposeIsland(CT_CHAR* pBuf, CT_INT32 iLen);
	//ս��
	CT_VOID DisposeFightTeam(CT_CHAR *pBuf,CT_INT32 iLen);
	//��ǽ�ӳ�
	CT_VOID DisposeWallAddition(CT_CHAR *pBuf,CT_INT32 iLen);
	//�Ӷ�
	CT_VOID DisposePlunder(CT_CHAR *pBuf,CT_INT32 iLen);
	//̽�ü�¼
	CT_VOID DisposeSeekData(CT_CHAR* pBuf,CT_INT32 iLen);
};
#endif