#include "CDatabaseMes.h"
#include "CGlobeFunc.h"
#include "CLogMeg.h"
#include "CDBConnector.h"
#include "CDBPetEvent.h"
#include "CUserPet.h"
#include "CGameConfig.h"
#include "CUserManager.h"
#include "CDBLogMes.h"

CCDBPetEvent::CCDBPetEvent(CCGameMesManager *pMesManager,
						   CT_DWORD id,CCDBConnector& dbConnector):CCGameMesEvent(pMesManager,id),m_dbConnector(dbConnector)
{
	RegistMessage(MSC_MAIN_DB_PET_DATA,MSC_SUB_DB_PET_INFO);
	RegistMessage(MSC_MAIN_DB_PET_DATA,MSC_SUB_DB_PET_CHANGE);
}

CT_VOID CCDBPetEvent::OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve)
{
	if(mainID != MSC_MAIN_DB_PET_DATA)
	{
		CCLogMeg::getSingleton().WriteError("CCDBPetEvent::OnUserEvent mainID != MSC_MAIN_DB_PET_DATA");
		return;
	}
	switch(subID)
	{
	case MSC_SUB_DB_PET_INFO:
		{
			DisposePetBorn(pBuf,len);
			break;
		}
	case MSC_SUB_DB_PET_CHANGE:
		{
			DisposePetChange(pBuf,len);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCDBPetEvent::OnUserEvent : mainID=%i, subID=%i", mainID, subID);
			break;
		}
	}
}

CT_VOID CCDBPetEvent::DisposePetBorn(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_PetBornInfo) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCDBPetEvent::DisposePetBorn sizeof(MSC_DB_PetBornInfo) != iLen");
		return;
	}

	MSC_DB_PetBornInfo *pPbi=(MSC_DB_PetBornInfo*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PetInsert");
	m_dbConnector.AddParm("_uid",pPbi->dwUserID);
	m_dbConnector.AddParm("_pid",pPbi->pi.iPetID);
	m_dbConnector.AddParm("_grade",pPbi->pi.iGrade);
	m_dbConnector.AddParm("_xp",pPbi->pi.iExp);
	m_dbConnector.AddParm("_ChangeID",pPbi->pi.iChangeID);
	m_dbConnector.AddParm("_Skill1",pPbi->pi.iSkillID[0]);
	m_dbConnector.AddParm("_Skill2",pPbi->pi.iSkillID[1]);
	m_dbConnector.AddParm("_Skill3",pPbi->pi.iSkillID[2]);
	m_dbConnector.AddParm("_Skill4",pPbi->pi.iSkillID[3]);
	m_dbConnector.AddParm("_Skill5",pPbi->pi.iSkillID[4]);
	m_dbConnector.AddParm("_Skill5",pPbi->pi.iSkillID[5]);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}

}

//³èÎï»Ã»¯
CT_VOID CCDBPetEvent::DisposePetChange(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_PetChange) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCDBPetEvent::DisposePetChange sizeof(MSC_DB_PetChange) != iLen");
		return;
	}

	MSC_DB_PetChange *pPc=(MSC_DB_PetChange*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PetChangeInsert");
	m_dbConnector.AddParm("_uid",pPc->dwUserID);
	m_dbConnector.AddParm("_changeid",pPc->iChangeID);
	m_dbConnector.AddParm("_skill",pPc->iSkillID);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		//pResult->GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}