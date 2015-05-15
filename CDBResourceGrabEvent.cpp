#include "CDBResourceGrabEvent.h"
#include "CResourceGrabMgr.h"
#include "CLogMeg.h"
#include "CDBConnector.h"
#include "CGlobeData.h"
#include "CGlobeFunc.h"
#include "CThreadMes.h"
#include "CFightTeamManager.h"

#define MAX_BUFFER			15*1024
CCDBResourceGrabEvent::CCDBResourceGrabEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector) : CCGameMesEvent(pMesManager, id), m_dbConnector(dbConnector)
{
	
	if (id == GAME_THREAD_DATABASE )
	{
		RegistTimer(IDI_RESOURCE_READ_SPOT_INFO);
		SetTimer(IDI_RESOURCE_READ_SPOT_INFO,  TIME_RESOURCE_READ_SPOT); //һ��Ҫ��ս����Ϣ�����ڴ����

	
		RegistMessage( MSC_MAIN_RES_OCCUPY, MSC_SUB_DB_RES_ACTIVE);
		RegistMessage(MSC_MAIN_RES_OCCUPY, MSC_SUB_DB_RES_SAVE);
	}
}

CCDBResourceGrabEvent::~CCDBResourceGrabEvent(void)
{
}

//�û���Ϣ
CT_VOID CCDBResourceGrabEvent::OnUserEvent(CT_DWORD mainID,	CT_DWORD subID, CT_CHAR* pBuf,		CT_DWORD len, CT_LONGLONG reserve)
{
	if (MSC_MAIN_RES_OCCUPY == mainID)
	{
		if( MSC_SUB_DB_RES_SAVE == subID)
		{
			SaveResourceData(pBuf, len);
		}
		else if(MSC_SUB_DB_RES_ACTIVE == subID)
		{
			DisposeResActive();
		}
	}
}

//��ʱ��
CT_VOID CCDBResourceGrabEvent::OnTime(CT_DWORD timeID)
{
	if (timeID == IDI_RESOURCE_READ_SPOT_INFO)
	{
		KillTimer(IDI_RESOURCE_READ_SPOT_INFO);
		GetAllScene();
		GetAllResSpot();
		GetAllResSpotProduct();
		return;
	}

	
}
//�û�ʱ�Ӵ���
CT_VOID CCDBResourceGrabEvent::OnClock(CT_DWORD timeID)
{

}

//�õ�ȫ����Դ��Ϣ
CT_VOID CCDBResourceGrabEvent::GetAllResSpot()  //����ʱ��ֻ��һ��
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ResourceSpotGet");
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				ResourceSpot spot = {0};
				CT_INT32  iSceneID = 0;
				CT_INT32  iSpotID  = 0;
				pResult->GetValue(0,  iSpotID);
				pResult->GetValue(1,  iSceneID);
				pResult->GetValue(2,  spot.resBase.dwFTID);
				pResult->GetValue(3,  spot.resBase.szNote, SPOT_VECTORY_SIGN_SIZE );
				pResult->GetValue(4,  spot.resBase.iLeftProtectTime);
				pResult->GetValue(5,  spot.resBase.iLeftOccupyTime);
				pResult->GetValue(6,  spot.resBase.bSendProtectMes);
				pResult->GetValue(7,  spot.guardTag.dwUserID[0]);
				pResult->GetValue(8,  spot.guardTag.dwUserID[1]);
				pResult->GetValue(9,  spot.guardTag.dwUserID[2]);
				CCResourceGrabMgr::getSingleton().RecoverResourceSpot(iSceneID, iSpotID,  spot);//����ʱ��ִֻ��һ��	
				CCFightTeamManager::getSingleton().SendFightTeamMemberGuard(spot.guardTag.dwUserID[0]);
				CCFightTeamManager::getSingleton().SendFightTeamMemberGuard(spot.guardTag.dwUserID[1]);
				CCFightTeamManager::getSingleton().SendFightTeamMemberGuard(spot.guardTag.dwUserID[2]);
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return ;  
	}
}

//�õ���Դ������Ϣ
CT_VOID CCDBResourceGrabEvent::GetAllScene()  //����ʱ��ֻ��һ��
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ResourceSceneGet");
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				CT_INT32  iSceneID = 0;
				pResult->GetValue(0,  iSceneID);
				CCResourceGrabMgr::getSingleton().SetResourceScene(iSceneID);	
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return ;  
	}
}

//�õ�ȫ����Դ������Ϣ
CT_VOID CCDBResourceGrabEvent::GetAllResSpotProduct()//����ʱ��ִֻ��һ��
{
	//����û���������Ϣ
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ResourceProductGet");
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				MSC_DB_ResourceProduct	resProduct = {0};
				pResult->GetValue(0, resProduct.dwUserID);
				pResult->GetValue(1, resProduct.iIndex );
				pResult->GetValue(2, resProduct.iSenceID);
				pResult->GetValue(3, resProduct.iSpotID);
				pResult->GetValue(4, resProduct.iGrade);
				pResult->GetValue(5, resProduct.iProductStatus);
				pResult->GetValue(6, resProduct.iProductCount);
				pResult->GetValue(7, resProduct.iLeftProductTime);
				CCResourceGrabMgr::getSingleton().SysResourceProduct(resProduct);  				
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return ;
	}
}

//������Դ�������ݵ����ݿ�
CT_VOID CCDBResourceGrabEvent::SaveResourceData(CT_CHAR* pBuf, CT_DWORD len)
{
	m_dbConnector.Reset();
	if(m_dbConnector.Exec(pBuf, len))
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return ;
	}
}

//��ȡ��Դ��
CT_VOID CCDBResourceGrabEvent::DisposeResActive()
{
	//����û���������Ϣ
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ActiveResspotGet");
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				ResActive ra={0};
				pResult->GetValue(0, ra.iSpot);
				pResult->GetValue(1, ra.iSceneID);
				G_SendThreadInfo(GAME_THREAD_CITY,MSC_MAIN_RES_OCCUPY,MSC_SUB_INSERT_RESACTIVE, (CT_CHAR*)&ra,sizeof(ResActive));
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return ;
	}

	G_SendThreadInfo(GAME_THREAD_CITY,MSC_MAIN_RES_OCCUPY,MSC_SUB_RESACTVIE_READFINISH, 0, 0);
	return;
}