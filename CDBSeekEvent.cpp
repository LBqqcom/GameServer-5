#include "CDatabaseMes.h"
#include "CDBConnector.h"
#include "CDBSeekEvent.h"
#include "CLogMeg.h"
#include "CBuildingScriptSetting.h"
#include "CDBConnector.h"
#include "CDBLogMes.h"

CCDBSeekEvnet::CCDBSeekEvnet(CCGameMesManager *pMesManager, CT_DWORD id, CCDBConnector &dbConnect):CCGameMesEvent(pMesManager, id),m_dbConnector(dbConnect)
{
	RegistMessage(MSC_MAIN_DB_SEEK, MSC_SUB_DB_WALL_ADDITION);
	RegistMessage(MSC_MAIN_DB_SEEK, MSC_SUB_DB_WALL_ARMY);
	RegistMessage(MSC_MAIN_DB_SEEK, MSC_SUB_SEEKED_TIMES_MINUS);
	RegistMessage(MSC_MAIN_DB_SEEK, MSC_SUB_SEEKED_PLAYER_UPDATE);
	RegistMessage(MSC_MAIN_DB_SEEK, MSC_SUB_SEEK_LIST_UPDATE);
	RegistMessage(MSC_MAIN_DB_SEEK, MSC_SUB_SEEKED_DATA_UPDATE);
	RegistMessage(MSC_MAIN_DB_SEEK, MSC_SUB_TEAM_FLAG_UPDATE);
	RegistMessage(MSC_MAIN_DB_SEEK, MSC_SUB_SEEK_PRESENT_UPDATE);
	RegistMessage(MSC_MAIN_DB_SEEK, MSC_SUB_SEEK_PRESENT_DEL);
	RegistMessage(MSC_MAIN_DB_SEEK, MSC_SUB_SEEK_DATA_UPDATE);
	RegistMessage(MSC_MAIN_DB_SEEK, MSC_SUB_TEAM_FLAG_DELETE);
	RegistMessage(MSC_MAIN_DB_SEEK, MSC_SUB_KIDNAP_UPDATE);
	RegistMessage(MSC_MAIN_DB_SEEK, MSC_SUB_KIDNAP_DELETE);
	RegistMessage(MSC_MAIN_DB_SEEK, MSC_SUB_SEEK_VISTORINFO);
	RegistMessage(MSC_MAIN_DB_SEEK, MSC_SUB_DB_SEEK_FIGHTING_RECORD);
	RegistMessage(MSC_MAIN_DB_SEEK, MSC_SUB_DB_PLUNDER_RES_UPDATE);
}

CCDBSeekEvnet::~CCDBSeekEvnet()
{

}

CT_VOID CCDBSeekEvnet::OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR *pBuf, CT_DWORD len, CT_LONGLONG reserve)
{
	if (MSC_MAIN_DB_SEEK == mainID)
	{
		switch(subID)
		{
		case MSC_SUB_DB_WALL_ADDITION:
			{
				DisposeWallAddition(pBuf, len);
				break;
			}
		case MSC_SUB_DB_WALL_ARMY:
			{
				DisposeWallArmy(pBuf, len);
				break;
			}
		case MSC_SUB_SEEKED_TIMES_MINUS:
			{
				DisposeSeekedTimes(pBuf, len);
				break;
			}
		case MSC_SUB_SEEKED_PLAYER_UPDATE:
			{
				DisposeSeekedPlayerInsert(pBuf,len);
				break;
			}
		case MSC_SUB_SEEK_LIST_UPDATE:
			{
				DisposeSeekListUpdate(pBuf, len);
				break;
			}
		case MSC_SUB_SEEKED_DATA_UPDATE:
			{
				DisposeSeekedDataUpdate(pBuf, len);
				break;
			}
		case MSC_SUB_TEAM_FLAG_UPDATE:
			{
				DisposeTeamFlag(pBuf, len);
				break;
			}
		case MSC_SUB_SEEK_PRESENT_UPDATE:
			{
				DisposeSeekPresent(pBuf, len);
				break;
			}
		case MSC_SUB_SEEK_PRESENT_DEL:
			{
				DisposeSeekPresentDel(pBuf, len);
				break;
			}
		case MSC_SUB_SEEK_DATA_UPDATE:
			{
				DisposeSeekDateUpdate(pBuf, len);
				break;
			}
		case MSC_SUB_TEAM_FLAG_DELETE:
			{
				DisposeTeamFlagDelete(pBuf, len);
				break;
			}
		case MSC_SUB_KIDNAP_UPDATE:
			{
				DisposeKidnapUpdate(pBuf, len);
				break;
			}
		case MSC_SUB_KIDNAP_DELETE:
			{
				DisposeKidnapDelete(pBuf, len);
				break;
			}
		case MSC_SUB_SEEK_VISTORINFO:
			{
				DisposeSeekVistorInfo(pBuf, len);
				break;
			}
		case MSC_SUB_DB_SEEK_FIGHTING_RECORD:
			{
				DisposeSeekFightRecord(pBuf, len);
				break;
			}
		case MSC_SUB_DB_PLUNDER_RES_UPDATE:
			{
				DisposePlunderResUpdate(pBuf, len);
				break;
			}
		default:
			{
				CCLogMeg::getSingleton().WriteError("");
			}
		}

	}
}

CT_VOID CCDBSeekEvnet::OnTime(CT_DWORD timeID)
{

}

CT_VOID CCDBSeekEvnet::OnClock(CT_DWORD timeID)
{

}

//城墙加成信息
CT_VOID CCDBSeekEvnet::DisposeWallAddition(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if(sizeof(MSC_DB_WALL_ADDITION) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCDBSeekEvnet::DisposeWallAddition sizeof(MSC_DB_WALL_ADDITION) != iLen");
		return;
	}

	MSC_DB_WALL_ADDITION *pWa=(MSC_DB_WALL_ADDITION*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_WallAdditionUpdate");
	m_dbConnector.AddParm("_uid",pWa->dwUserID);
	m_dbConnector.AddParm("_pid",pWa->iID);
	m_dbConnector.AddParm("_grade",pWa->iGrade);

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
	return;
}

//城墙军队
CT_VOID CCDBSeekEvnet::DisposeWallArmy(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if(sizeof(MSC_DB_WALL_ARMY) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCDBSeekEvnet::DisposeWallArmy sizeof(MSC_DB_WALL_ARMY) != iLen");
		return;
	}

	MSC_DB_WALL_ARMY *pWa=(MSC_DB_WALL_ARMY*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_WallArmyUpdate");
	m_dbConnector.AddParm("_uid",pWa->dwUserID);
	m_dbConnector.AddParm("_pos1",pWa->iPos[0]);
	m_dbConnector.AddParm("_pos1",pWa->iPos[1]);
	m_dbConnector.AddParm("_pos1",pWa->iPos[2]);
	m_dbConnector.AddParm("_pos1",pWa->iPos[3]);
	m_dbConnector.AddParm("_pos1",pWa->iPos[4]);

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
	return;
}

CT_VOID CCDBSeekEvnet::DisposeSeekedTimes(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_Seeked_TimesMinus))
	{
		CCLogMeg::getSingleton().WriteError("CCDBSeekEvnet::DisposeSeekedTimes iLen != sizeof(MSC_DB_Seeked_TimesMinus)");
		return;
	}

	MSC_DB_Seeked_TimesMinus* pSeeked = (MSC_DB_Seeked_TimesMinus*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_SeekedTimesUpdate");
	m_dbConnector.AddParm("_uid", pSeeked->dwFUserID);
	m_dbConnector.AddParm("_type",	pSeeked->iType);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(pSeeked->dwFUserID,"MSC_DB_Seeked_TimesMinus","_uid:%d",pSeeked->dwFUserID);
		G_DBError(m_dbConnector, m_dwThread);
	}
}

// 已探访状态
CT_VOID CCDBSeekEvnet::DisposeSeekedPlayerInsert(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_Had_SeekedPlayer))
	{
		CCLogMeg::getSingleton().WriteError("CCDBSeekEvnet::DisposeSeekedPlayerUpdate iLen != sizeof(MSC_DB_Had_SeekedPlayer)");
		return;
	}

	MSC_DB_Had_SeekedPlayer* pPlayer = (MSC_DB_Had_SeekedPlayer*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_SeekedPlayerInsert");
	m_dbConnector.AddParm("_uid", pPlayer->dwUserID);
	m_dbConnector.AddParm("_fuid",	pPlayer->dwFUserID);
	m_dbConnector.AddParm("_type",	pPlayer->iType);


	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(pPlayer->dwUserID,"MSC_DB_Had_SeekedPlayer","_uid:%d",pPlayer->dwUserID);
		G_DBError(m_dbConnector, m_dwThread);
	}
}

// 探访界面更新
CT_VOID CCDBSeekEvnet::DisposeSeekListUpdate(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_SeekPlayerList))
	{
		CCLogMeg::getSingleton().WriteError("CCDBSeekEvnet::DisposeSeekListUpdate iLen != sizeof(MSC_DB_SeekPlayer)");
		return;
	}

	MSC_DB_SeekPlayerList* player = (MSC_DB_SeekPlayerList*)pBuf;
	CT_CHAR cSzLine[1024] = {0};
	std::string strTmp;

	sprintf(cSzLine, "update seekplayer set IsInList = 0 where U_ID = %u and Status = 1 and IsInList = 1;", player->dwUserID);
	strTmp.append(cSzLine);

	memset(cSzLine, 0, sizeof(cSzLine));
	sprintf(cSzLine, "delete from seekplayer where U_ID = %u and IsInList = 1;", player->dwUserID);
	strTmp.append(cSzLine);

	for (CT_INT32 i = 0;i < SEEK_PLAYER_NUM;++i)
	{
		if (player->aWillPlayer[i].dwUserID != 0)
		{
			memset(cSzLine, 0, sizeof(cSzLine));
			sprintf(cSzLine, "insert into seekplayer(U_ID,FU_ID,Type,Status,IsInList) values(%u, %u, 1, %d, 1);", 
				player->dwUserID, player->aWillPlayer[i].dwUserID,player->aWillPlayer[i].iStatus);
			strTmp.append(cSzLine);
		}
	}

	for (CT_INT32 i = 0;i < SEEK_PLAYER_NUM;++i)
	{
		if (player->aSpitePlayer[i].dwUserID != 0)
		{
			memset(cSzLine, 0, sizeof(cSzLine));
			sprintf(cSzLine, "insert into seekplayer(U_ID,FU_ID,Type,Status,IsInList) values(%u, %u, 2, %d, 1);",
				player->dwUserID, player->aSpitePlayer[i].dwUserID, player->aSpitePlayer[i].iStatus);
			strTmp.append(cSzLine);
		}
	}

	m_dbConnector.Reset();

	if(m_dbConnector.Exec(const_cast<CT_CHAR*>(strTmp.c_str()), strTmp.length() + 1))
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(player->dwUserID,"MSC_DB_SeekPlayerList","_uid:%d",player->dwUserID);
		G_DBError(m_dbConnector, m_dwThread);
	}
}

// 更新被探访者的基本信息
CT_VOID CCDBSeekEvnet::DisposeSeekedDataUpdate(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if(iLen != sizeof(MSC_DB_Seeked_Info))
	{
		CCLogMeg::getSingleton().WriteError("CCDBSeekEvnet::DisposeSeekDataUpdate iLen != sizeof(MSC_DB_Seeked_Info)");
		return;
	}

	MSC_DB_Seeked_Info* pInfo = (MSC_DB_Seeked_Info*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_SeekedDataUpdate");
	m_dbConnector.AddParm("_uid", pInfo->dwUserID);
	m_dbConnector.AddParm("_diamond",	pInfo->iDiamond);
	m_dbConnector.AddParm("_money",	pInfo->iMoney);
	m_dbConnector.AddParm("_food",		pInfo->iFood);
	m_dbConnector.AddParm("_worker",	pInfo->iWorker);
	m_dbConnector.AddParm("_kidnapWorkerNum", pInfo->iKidnapWorkerNum);
	m_dbConnector.AddParm("_kidnapSaintess",  pInfo->iKidnapSaintess);
	m_dbConnector.AddParm("_seekedWillTimes", pInfo->iSeekedWillTimes);
	m_dbConnector.AddParm("_seekedSpiteTimes", pInfo->iSeekedSpiteTimes);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(pInfo->dwUserID,"MSC_DB_Seeked_Info","_uid:%d",pInfo->dwUserID);
		G_DBError(m_dbConnector, m_dwThread);
	}
}

// 更新战旗
CT_VOID CCDBSeekEvnet::DisposeTeamFlag(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_SeekTeamFlag))
	{
		CCLogMeg::getSingleton().WriteError("CCDBSeekEvnet::DisposeTeamFlag iLen != sizeof(MSC_DB_SeekTeamFlag)");
		return;
	}

	MSC_DB_SeekTeamFlag* pFlag = (MSC_DB_SeekTeamFlag*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TeamFlagInsert");
	m_dbConnector.AddParm("_uid", pFlag->dwUserID);
	m_dbConnector.AddParm("_ownUser",	pFlag->teamFlag.dwUserID);
	m_dbConnector.AddParm("_teamName",	(CT_CHAR*)pFlag->teamFlag.cTName);
	m_dbConnector.AddParm("_result",	pFlag->teamFlag.iFightResult);
	m_dbConnector.AddParm("_type",		pFlag->teamFlag.iType);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(pFlag->dwUserID,"MSC_DB_SeekTeamFlag","_uid:%d",pFlag->dwUserID);
		G_DBError(m_dbConnector, m_dwThread);
	}

}

// 更新礼物
CT_VOID CCDBSeekEvnet::DisposeSeekPresent(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_SeekPresent))
	{
		CCLogMeg::getSingleton().WriteError("CCDBSeekEvnet::DisposeSeekPresent iLen != sizeof(MSC_DB_SeekPresent)");
		return;
	}

	MSC_DB_SeekPresent* present = (MSC_DB_SeekPresent*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_SeekedPresentInsert");
	m_dbConnector.AddParm("_uid", present->dwUserID);
	m_dbConnector.AddParm("_presentID", present->present.iChestID);
	m_dbConnector.AddParm("_time", present->present.iGetTime);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(present->dwUserID,"MSC_DB_SeekPresent","_uid:%d",present->dwUserID);
		G_DBError(m_dbConnector, m_dwThread);
	}
}

// 删除礼物
CT_VOID CCDBSeekEvnet::DisposeSeekPresentDel(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_SeekPresent))
	{
		CCLogMeg::getSingleton().WriteError("CCDBSeekEvnet::DisposeSeekPresent iLen != sizeof(MSC_DB_SeekPresent)");
		return;
	}

	MSC_DB_SeekPresent* present = (MSC_DB_SeekPresent*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_SeekedPresentDel");
	m_dbConnector.AddParm("_uid", present->dwUserID);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(present->dwUserID,"MSC_DB_SeekPresent","_uid:%d",present->dwUserID);
		G_DBError(m_dbConnector, m_dwThread);
	}
}

// 更新探访者信息
CT_VOID CCDBSeekEvnet::DisposeSeekDateUpdate(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_SeekData))
	{
		CCLogMeg::getSingleton().WriteError("CCDBSeekEvnet::DisposeSeekDateUpdate iLen != sizeof(MSC_DB_SeekData)");
		return;
	}

	MSC_DB_SeekData* pdata = (MSC_DB_SeekData*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_SeekDataUpdate");
	m_dbConnector.AddParm("_uid", pdata->dwUserID);
	m_dbConnector.AddParm("_seekWillTimes", pdata->seekData.iSeekWillTimes);
	m_dbConnector.AddParm("_seekSpiteTimes", pdata->seekData.iSeekSpiteTimes);
	m_dbConnector.AddParm("_seekedWillTimes", pdata->seekData.iSeekedWillTimes);
	m_dbConnector.AddParm("_seekedSpiteTimes", pdata->seekData.iSeekedSpiteTimes);
	m_dbConnector.AddParm("_rescueNum", pdata->seekData.iRescueNum);
	m_dbConnector.AddParm("_womanNum", pdata->seekData.iWomanNum);
	m_dbConnector.AddParm("_farmarNum", pdata->seekData.iFarmarNum);
	m_dbConnector.AddParm("_addSpiteTimes", pdata->seekData.iAddSpiteTimes);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(pdata->dwUserID,"MSC_DB_SeekData","_uid:%d",pdata->dwUserID);
		G_DBError(m_dbConnector, m_dwThread);
	}
}

// 删除战旗
CT_VOID CCDBSeekEvnet::DisposeTeamFlagDelete(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_SeekTeamFlag))
	{
		CCLogMeg::getSingleton().WriteError("CCDBSeekEvnet::DisposeTeamFlagDelete iLen != sizeof(MSC_DB_SeekTeamFlag)");
		return;
	}

	MSC_DB_SeekTeamFlag* pFlag = (MSC_DB_SeekTeamFlag*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TeamFlagDelete");
	m_dbConnector.AddParm("_uid", pFlag->dwUserID);
	m_dbConnector.AddParm("_fuid", pFlag->teamFlag.dwUserID);
	m_dbConnector.AddParm("_type", pFlag->teamFlag.iType);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(pFlag->dwUserID,"MSC_DB_SeekTeamFlag","_uid:%d",pFlag->dwUserID);
		G_DBError(m_dbConnector, m_dwThread);
	}
}

// 插入人质
CT_VOID CCDBSeekEvnet::DisposeKidnapUpdate(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_SeekKidnap))
	{
		CCLogMeg::getSingleton().WriteError("CCDBSeekEvnet::DisposeKidnapInsert iLen != sizeof(MSC_DB_SeekKidnap)");
		return;
	}

	MSC_DB_SeekKidnap* pKidnap = (MSC_DB_SeekKidnap*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_KidnapUpdate");
	m_dbConnector.AddParm("_uid", pKidnap->dwUserID);
	m_dbConnector.AddParm("_Type", pKidnap->iType);
	m_dbConnector.AddParm("_Ownerid", pKidnap->kidnap.owerUserId);
	m_dbConnector.AddParm("_Num", pKidnap->kidnap.Num);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(pKidnap->dwUserID,"MSC_DB_SeekKidnap","_uid:%d",pKidnap->dwUserID);
		G_DBError(m_dbConnector, m_dwThread);
	}
}

// 删除人质
CT_VOID CCDBSeekEvnet::DisposeKidnapDelete(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_SeekKidnap))
	{
		CCLogMeg::getSingleton().WriteError("CCDBSeekEvnet::DisposeKidnapInsert iLen != sizeof(MSC_DB_SeekKidnap)");
		return;
	}

	MSC_DB_SeekKidnap* pKidnap = (MSC_DB_SeekKidnap*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_KidnapDelete");
	m_dbConnector.AddParm("_uid", pKidnap->dwUserID);
	m_dbConnector.AddParm("_Ownerid", pKidnap->kidnap.owerUserId);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(pKidnap->dwUserID,"MSC_DB_SeekKidnap","_uid:%d",pKidnap->dwUserID);
		G_DBError(m_dbConnector, m_dwThread);
	}
}

// 添加访客记录
CT_VOID CCDBSeekEvnet::DisposeSeekVistorInfo(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_SeekVistorInfo))
	{
		CCLogMeg::getSingleton().WriteError("CCDBSeekEvnet::DisposeSeekVistorInfo iLen != sizeof(MSC_DB_SeekVistorInfo)");
		return;
	}

	MSC_DB_SeekVistorInfo* pInfo = (MSC_DB_SeekVistorInfo*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_SeekVistorInfoInsert");
	m_dbConnector.AddParm("_uid", pInfo->dwUserID);
	m_dbConnector.AddParm("_fuid", pInfo->vistorInfo.dwUserID);
	m_dbConnector.AddParm("_type", pInfo->vistorInfo.seekInfo.iType);
	m_dbConnector.AddParm("_id0", pInfo->vistorInfo.seekInfo.iID[0]);
	m_dbConnector.AddParm("_num0", pInfo->vistorInfo.seekInfo.iNum[0]);
	m_dbConnector.AddParm("_id1", pInfo->vistorInfo.seekInfo.iID[1]);
	m_dbConnector.AddParm("_num1", pInfo->vistorInfo.seekInfo.iNum[1]);
	m_dbConnector.AddParm("_id2", pInfo->vistorInfo.seekInfo.iID[2]);
	m_dbConnector.AddParm("_num2", pInfo->vistorInfo.seekInfo.iNum[2]);
	m_dbConnector.AddParm("_id3", pInfo->vistorInfo.seekInfo.iID[3]);
	m_dbConnector.AddParm("_num3", pInfo->vistorInfo.seekInfo.iNum[3]);
	m_dbConnector.AddParm("_present0", pInfo->vistorInfo.present[0]);
	m_dbConnector.AddParm("_present1", pInfo->vistorInfo.present[1]);
	m_dbConnector.AddParm("_present2", pInfo->vistorInfo.present[2]);
	m_dbConnector.AddParm("_iskidnap", pInfo->vistorInfo.bIsKidnap);
	m_dbConnector.AddParm("_plunderbid0", pInfo->vistorInfo.iPlunderBID[0]);
	m_dbConnector.AddParm("_plunderbid1", pInfo->vistorInfo.iPlunderBID[1]);
	m_dbConnector.AddParm("_plunderbid2", pInfo->vistorInfo.iPlunderBID[2]);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(pInfo->dwUserID,"SP_SeekVistorInfoInsert","_uid:%d",pInfo->dwUserID);
		G_DBError(m_dbConnector, m_dwThread);
	}
	return;
}

//添加探访战报
CT_VOID CCDBSeekEvnet::DisposeSeekFightRecord(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if(iLen != sizeof(MSC_DB_SeekFightRecord))
	{
		CCLogMeg::getSingleton().WriteError("CCDBSeekEvnet::DisposeSeekFightRecord :len != sizeof(MSC_DB_SeekFightRecord)");
		return;
	}

	MSC_DB_SeekFightRecord *pSfr=(MSC_DB_SeekFightRecord*)pBuf;

	UserFightingStatus ufs[2]={0};
	memcpy(&ufs[0],&pSfr->LeftFightingData,sizeof(ufs[0]));
	memcpy(&ufs[1],&pSfr->RightFightingData,sizeof(ufs[0]));

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_SeekFightExampleClear");

	m_dbConnector.AddParm("_index", (CT_CHAR*)pSfr->ucIndex);

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


	for(CT_INT32 i=0;i<2;++i)
	{
		for(CT_INT32 j=0;j<POST_COUNT/2;++j)
		{
			if(ufs[i].units[j].type == 0)continue;

			m_dbConnector.Reset();
			m_dbConnector.SetPcName("SP_SeekAddFightExample");

			m_dbConnector.AddParm("_index", (CT_CHAR*)pSfr->ucIndex);
			G_SaveFightingRecord(m_dbConnector,ufs[i],j,pSfr->iSeed);
			m_dbConnector.AddParm("_fid", 0);

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
	}

	//插入装备技能
	for(CT_INT32 i=0;i<PVP_USER_COUNT;++i)
	{
		for(CT_INT32 j=0;j<POST_COUNT/2;++j)
		{
			for(CT_INT32 k=0;k<SKILL_ENUIP_COUNT;++k)
			{
				if(ufs[i].units[j].type == 0 || 
					ufs[i].units[j].SkillEnuip.iSkillEquip[k] == 0)continue;

				m_dbConnector.Reset();
				m_dbConnector.SetPcName("SP_SeekAddFightSkill");

				m_dbConnector.AddParm("_index", (CT_CHAR*)pSfr->ucIndex);
				m_dbConnector.AddParm("_uid",  ufs[i].dwUserID);
				m_dbConnector.AddParm("_skillid", ufs[i].units[j].SkillEnuip.iSkillEquip[k]);
				m_dbConnector.AddParm("_pos", j);

				if(m_dbConnector.Exec())
				{
					CCDBResult* pResult = m_dbConnector.GetResult();
					pResult->Release();
				}else
				{
					G_DBError(m_dbConnector, m_dwThread);
					return;
				}
			}
		}
	}
	return;
}

// 记录掠夺的金币和粮食
CT_VOID CCDBSeekEvnet::DisposePlunderResUpdate(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_PlunderRes))
	{
		CCLogMeg::getSingleton().WriteError("CCDBSeekEvnet::DisposePlunderResUpdate iLen != sizeof(MSC_DB_PlunderRes)");
		return;
	}

	MSC_DB_PlunderRes* pPlunder = (MSC_DB_PlunderRes*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PlunderResUpdate");
	m_dbConnector.AddParm("_uid",	pPlunder->dwUserID);
	m_dbConnector.AddParm("_score",	pPlunder->iPlunderScore);

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