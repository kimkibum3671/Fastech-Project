
#include "EthernetInterface.h"

#include "../../Common_Define/Define_Protocol.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma pack(1)

#define MOTION_GATE_ID	0xfa

CEthernetInterface::CEthernetInterface(BOOL bTCP, IN_ADDR addr, WORD wPort, int iBdID)
	: CFSocket(bTCP, addr, wPort), m_iBdID(iBdID)
{

}

CEthernetInterface::~CEthernetInterface(void)
{

}

//------------------------------------------------------------------------------
//					Inner Functions
//------------------------------------------------------------------------------
const int CEthernetInterface::DoAck(IN const BYTE iAxisNo, IN const DWORD dwWaitTime)
{
	return DoSendCommand(iAxisNo, FRAME_MOTIONGATE_GETAXISINFO, NULL, 0, NULL, 0, dwWaitTime);
}

const int	CEthernetInterface::EnableAutoReconnect(IN const BOOL bEnable)
{
	SetOptionAutoReconnect(bEnable);

	return FMM_OK;
}

//int CEthernetInterface::DoCmdSetParameter(BYTE iAxisNo, BYTE iParaNo, long lParaValue)
//{
//	BYTE byValue[5];
//	UNION_4DATA data;
//
//	byValue[0] = iParaNo;
//	data.lValue = lParaValue;
//	byValue[1] = data.byValue[0];
//	byValue[2] = data.byValue[1];
//	byValue[3] = data.byValue[2];
//	byValue[4] = data.byValue[3];
//
//	return DoSendCommand(iAxisNo, FRAME_FAS_SETPARAMETER, byValue, 5, NULL, 0, SOCKET_WAITTIME_NORMAL);
//}
//
//
//int CEthernetInterface::DoCmdGetSlaveInfoV8(BYTE iAxisNo, DRIVE_INFO* lpDriveInfo)
//{
//	DRIVE_INFO driveinfo = { 0 };
//	int nBuffSize = sizeof(DRIVE_INFO);
//	int nRtn;
//
//
//	nRtn = DoSendCommand(iAxisNo, FRAME_GETSLAVEINFO, NULL, 0, &driveinfo, nBuffSize, SOCKET_WAITTIME_NORMAL, TRUE);
//	if (nRtn == FMM_OK)
//	{
//		if (lpDriveInfo != NULL)
//			memcpy(lpDriveInfo, &driveinfo, nBuffSize);
//	}
//
//	return nRtn;
//}

const INT CEthernetInterface::DoCmdGetMotiongateInfo(OUT LPVOID pDeviceInfo, IN const INT cbBuffer)
{
	MOTIONGATE_WRITE_INFO stRequest;
	const INT cbRequest = sizeof(MOTIONGATE_WRITE_INFO);
	::ZeroMemory(&stRequest, cbRequest);
	stRequest.btSubCommand = 0xa0;

	MOTIONGATE_READ_INFO stResponse;
	const INT cbResponse = sizeof(MOTIONGATE_READ_INFO);
	::ZeroMemory(&stResponse, cbResponse);

	INT nRtn = DoSendCommand(MOTION_GATE_ID, FRAME_MOTIONGATE_GETINFO, &stRequest, cbRequest, &stResponse, cbResponse, SOCKET_WAITTIME_NORMAL, TRUE);
	if (nRtn == FMM_OK)
	{
		const INT cbCopy = min(cbResponse, cbBuffer);
		memcpy_s(pDeviceInfo, cbBuffer, &stResponse, cbCopy);
	}

	return nRtn;
}

const INT CEthernetInterface::DoCmdGetSlaveInfo(IN const BYTE iAxisNo, OUT BYTE* pSlaveType, OUT LPSTR lpszSlaveInfo, IN const INT cbSlaveInfo, IN BOOL bRetry)
{
	const INT cbInternalRevBuffer = MAX_PATH;
	BYTE btData[cbInternalRevBuffer] = { 0, };

	INT nRtn = bRetry ? DoSendCommand(iAxisNo, FRAME_GETSLAVEINFO, NULL, 0, btData, cbInternalRevBuffer, SOCKET_WAITTIME_NORMAL, TRUE) :
		DoTrySendCommand(iAxisNo, FRAME_GETSLAVEINFO, NULL, 0, btData, cbInternalRevBuffer, SOCKET_WAITTIME_NORMAL, TRUE);
	if (nRtn == FMM_OK)
	{
		memcpy_s(lpszSlaveInfo, cbSlaveInfo, btData, cbInternalRevBuffer);
	}

	return nRtn;
}

const INT CEthernetInterface::DoCmdGetMotiongateSystemStatus(IN const BYTE iAxisNo, OUT BYTE* pSystemStatus, IN const INT cbSystemStatus)
{
	return DoSendCommand(MOTION_GATE_ID, FRAME_MOTIONGATE_GET_SYSTEMSTATUS, &iAxisNo, 1, pSystemStatus, cbSystemStatus, SOCKET_WAITTIME_NORMAL, TRUE);
}

const INT CEthernetInterface::DoCmdSetMonitoringMode(IN const BOOL bEnable)
{
	BYTE btValue = static_cast<BYTE>(bEnable);

	return DoSendCommand(MOTION_GATE_ID, FRAME_MOTIONGATE_SETMODE, &btValue, 1, NULL, 0, SOCKET_WAITTIME_NORMAL, TRUE);
}

const INT CEthernetInterface::DoCmdServoEnable(IN const BYTE iAxisNo, IN const BOOL bOnOff)
{
	BYTE btValue = (BYTE)(bOnOff) ? 0x01 : 0x00;

	return DoSendCommand(iAxisNo, FRAME_FAS_SERVOENABLE, &btValue, 1, NULL, 0, SOCKET_WAITTIME_NORMAL * 5, TRUE);
}

const INT CEthernetInterface::DoCmdMoveVelocity(IN const BYTE iAxisNo, IN const DWORD dwSpeed, IN const BYTE btDirection)
{
	BYTE btValue[5] = { 0, };
	const BYTE* pbtData = reinterpret_cast<const BYTE*>(&dwSpeed);
	
	INT nIndex = 0;
	btValue[nIndex++] = pbtData[0];
	btValue[nIndex++] = pbtData[1];
	btValue[nIndex++] = pbtData[2];
	btValue[nIndex++] = pbtData[3];
	btValue[nIndex++] = btDirection;

	return DoSendCommand(iAxisNo, FRAME_FAS_MOVEVELOCITY, &btValue, 5, NULL, 0, SOCKET_WAITTIME_NORMAL, TRUE);
}

const INT CEthernetInterface::DoCmdMoveStop(BYTE iAxisNo)
{
	return DoSendCommand(iAxisNo, FRAME_FAS_MOVESTOP, NULL, 0, NULL, 0, SOCKET_WAITTIME_NORMAL, TRUE);
}

const INT CEthernetInterface::DoCmdEmergencyStop(BYTE iAxisNo)
{
	return DoSendCommand(iAxisNo, FRAME_FAS_EMERGENCYSTOP, NULL, 0, NULL, 0, SOCKET_WAITTIME_NORMAL, TRUE);
}

const INT CEthernetInterface::DoCmdMoveOriginSingleAxis(IN const BYTE iAxisNo)
{
	return DoSendCommand(iAxisNo, FRAME_FAS_MOVEORIGIN, NULL, 0, NULL, 0, SOCKET_WAITTIME_NORMAL, TRUE);
}

const INT CEthernetInterface::DoCmdPosTableRunItem(IN const BYTE iAxisNo, IN const WORD wItemNo)
{
	return DoSendCommand(iAxisNo, FRAME_FAS_POSTAB_RUN_ITEM, &wItemNo, sizeof(WORD), NULL, 0, SOCKET_WAITTIME_NORMAL, TRUE);
}




const INT CEthernetInterface::DoCmdChangeMotiongateInfo(IN const CHAR* strName, IN const INT nStrLen, IN const BYTE btMotorCount, IN const BYTE btHomingTimeout)
{
	MOTIONGATE_WRITE_INFO stRequest;
	const INT cbRequest = sizeof(MOTIONGATE_WRITE_INFO);
	::ZeroMemory(&stRequest, cbRequest);
	stRequest.btSubCommand = 0xa1;
	memcpy(stRequest.chMotiongateName, strName, min(sizeof(stRequest.chMotiongateName), nStrLen));
	stRequest.btMotorCount = btMotorCount;
	stRequest.btHomingTimeout = btHomingTimeout;

	MOTIONGATE_READ_INFO stResponse;
	const INT cbResponse = sizeof(MOTIONGATE_READ_INFO);
	::ZeroMemory(&stResponse, cbResponse);

	INT nRet = DoSendCommand(MOTION_GATE_ID, FRAME_MOTIONGATE_GETINFO, &stRequest, cbRequest, &stResponse, cbResponse, SOCKET_WAITTIME_NORMAL, TRUE);

	return nRet;
}

const INT CEthernetInterface::DoCmdResetMotionGateAlarm()
{
	return DoSendCommand(MOTION_GATE_ID, FRAME_MOTIONGATE_ALARMRESET, NULL, 0, NULL, 0, SOCKET_WAITTIME_NORMAL, TRUE);
}

const int CEthernetInterface::DoCmdSetParameter(IN const BYTE iAxisNo, IN const BYTE iParaNo, IN LONG lParaValue)
{
	union
	{
		BYTE byValue[5];
		struct
		{
			BYTE btParamNo;
			LONG lValue;
		};
	} btData;

	btData.btParamNo = iParaNo;
	btData.lValue = lParaValue;

	return DoSendCommand(iAxisNo, FRAME_FAS_SETPARAMETER, &btData, 5, NULL, NULL, SOCKET_WAITTIME_NORMAL, TRUE);
}

const INT CEthernetInterface::DoCmdGetParameter(IN const BYTE iAxisNo, IN const BYTE iParaNo, OUT LONG* lParamValue)
{
	long lValue = 0;

	const int nRtn = DoSendCommand(iAxisNo, FRAME_FAS_GETPARAMETER, &iParaNo, 1, &lValue, 4, SOCKET_WAITTIME_NORMAL, TRUE);

	if (nRtn == FMM_OK)
	{
		if (lParamValue != NULL)
			*lParamValue = lValue;
	}

	return nRtn;
}

const INT CEthernetInterface::DoCmdGetROMParam(IN const BYTE iAxisNo, IN const BYTE iParaNo, OUT long* lROMParam)
{
	long lValue = 0;

	const int nRtn = DoSendCommand(iAxisNo, FRAME_FAS_GETROMPARAM, &iParaNo, 1, &lValue, 4, SOCKET_WAITTIME_NORMAL, TRUE);

	if (nRtn == FMM_OK)
	{
		if (lROMParam != NULL)
			*lROMParam = lValue;
	}

	return nRtn;
}

const INT CEthernetInterface::DoCmdPosTableWriteItem(IN const BYTE iAxisNo, IN const WORD wItemNo, IN const LPITEM_NODE lpItem)
{
	const INT cbBuffer = sizeof(ITEM_NODE) + 2;
	BYTE btData[cbBuffer] = { 0, };

	*reinterpret_cast<WORD*>(&btData[0]) = wItemNo;
	memcpy(&btData[2], lpItem, sizeof(ITEM_NODE));

	return DoSendCommand(iAxisNo, FRAME_FAS_POSTAB_WRITE_ITEM, &btData, cbBuffer, 0, NULL, SOCKET_WAITTIME_NORMAL, TRUE);
}

const INT CEthernetInterface::DoCmdPosTableReadItem(IN const BYTE iAxisNo, IN const WORD wItemNo, OUT LPITEM_NODE lpItem)
{
	ITEM_NODE item = { 0 };

	const INT nRtn = DoSendCommand(iAxisNo, FRAME_FAS_POSTAB_READ_ITEM, &wItemNo, sizeof(WORD), &item, sizeof(ITEM_NODE), SOCKET_WAITTIME_NORMAL, TRUE);

	if (nRtn == FMM_OK)
	{
		if (lpItem != NULL)
			memcpy(lpItem, &item, sizeof(ITEM_NODE));
	}

	return nRtn;
}

const int CEthernetInterface::DoCmdSaveAllParam(IN const BYTE iAxisNo)
{
	return DoSendCommand(iAxisNo, FRAME_FAS_SAVEALLPARAM, NULL, 0, NULL, 0, SOCKET_WAITTIME_SAVE_PARAM, TRUE);
}

const INT CEthernetInterface::DoCmdPosTableWriteROM(IN const BYTE iAxisNo)
{
	BYTE nRtnCode = 0;

	const INT nRtn = DoSendCommand(iAxisNo, FRAME_FAS_POSTAB_WRITE_ROM, NULL, 0, &nRtnCode, 1, SOCKET_WAITTIME_SAVE_PT);

	if (nRtnCode != 0)
		return FMM_POSTABLE_ERROR;

	return nRtn;
}

const INT CEthernetInterface::DoCmdPosTableReadROM(IN const BYTE iAxisNo)
{
	BYTE nRtnCode = 0;

	const INT nRtn = DoSendCommand(iAxisNo, FRAME_FAS_POSTAB_READ_ROM, NULL, 0, &nRtnCode, 1, SOCKET_WAITTIME_SAVE_PT);

	if (nRtnCode != 0)
		return FMM_POSTABLE_ERROR;

	return nRtn;
}

const INT CEthernetInterface::DoCmdChangeSlaveID(IN const BYTE iCurAxisNo, const IN BYTE iNewAxisNo)
{
	BYTE btData[2] = { iNewAxisNo, 4 };
	return DoSendCommand(iCurAxisNo, FRAME_SETIDVALUE, btData, 2, NULL, 0, SOCKET_WAITTIME_SAVE_PT);
}

const INT CEthernetInterface::DoCmdRebootMotionGate()
{
	return DoSendCommand(MOTION_GATE_ID, FRAME_MOTIONGATE_REBOOT, NULL, 0, NULL, 0, SOCKET_WAITTIME_SAVE_PT);
}

const INT CEthernetInterface::DoCmdRescanMotiongateSlave()
{
	return DoSendCommand(MOTION_GATE_ID, FRAME_MOTIONGATE_RESCAN, NULL, 0, NULL, 0, SOCKET_WAITTIME_NORMAL);
}

const INT CEthernetInterface::DoCmdGetAllStatus(IN const BYTE iAxisNo, OUT ULONGLONG* dwInStatus, OUT DWORD* dwOutStatus, OUT DWORD* dwAxisStatus, OUT long* lCmdPos, OUT long* lActPos, OUT long* lPosErr, OUT long* lActVel, OUT WORD* wPosItemNo)
{
	BYTE byValue[33] = { 0, };

	const INT nRtn = DoSendCommand(iAxisNo, FRAME_FAS_GETALLSTATUS, NULL, 0, byValue, sizeof(byValue), SOCKET_WAITTIME_NORMAL);

	if( FMM_OK == nRtn )
	{
		*dwInStatus = *reinterpret_cast<ULONGLONG*>(&byValue[0]) & 0x000000ffffffffff;
		*dwOutStatus = *reinterpret_cast<DWORD*>(&byValue[5]);
		*dwAxisStatus = *reinterpret_cast<DWORD*>(&byValue[9]);
		*lCmdPos = *reinterpret_cast<LONG*>(&byValue[13]);
		*lActPos = *reinterpret_cast<LONG*>(&byValue[17]);
		*lPosErr = *reinterpret_cast<LONG*>(&byValue[21]);
		*lActVel = *reinterpret_cast<LONG*>(&byValue[25]);
		*wPosItemNo = static_cast<WORD>(*reinterpret_cast<DWORD*>(&byValue[29]));
	}	

	return nRtn;
}

const INT CEthernetInterface::DoCmdGetAxisStatus(IN const BYTE iAxisNo, OUT DWORD* dwAxisStatus)
{
	DWORD dwValue = 0;

	const INT nRtn = DoSendCommand(iAxisNo, FRAME_FAS_GETAXISSTATUS, NULL, 0, &dwValue, sizeof(dwValue), SOCKET_WAITTIME_NORMAL);

	if (FMM_OK == nRtn)
	{
		*dwAxisStatus = dwValue;
	}

	return nRtn;
}

int CEthernetInterface::DoCmdGetAlarmType(BYTE iAxisNo, BYTE* pStatus)
{ // KKB �߰�.
	BYTE byValue;
	int nRtn;

	nRtn = DoSendCommand(iAxisNo, FRAME_FAS_GETALARMTYPE, NULL, 0, &byValue, sizeof(byValue), SOCKET_WAITTIME_NORMAL);

	if (FMM_OK == nRtn)
	{
		*pStatus = byValue;
	}

	return nRtn;
}

const INT CEthernetInterface::DoCmdNotifyBackupDone(IN const WORD wElapse)
{
	WORD wValue = wElapse;

	return DoSendCommand(MOTION_GATE_ID, FRAME_MOTIONGATE_NOTIFY_BACKUP_DONE, &wValue, sizeof(wValue), NULL, 0, SOCKET_WAITTIME_NORMAL);
}


const INT CEthernetInterface::DoCmdGetLifeCounter(IN const BYTE iAxisNo, OUT DWORD* pdwCounter)
{
	DWORD dwValue = 0;

	const INT nRtn = DoSendCommand(iAxisNo, FRAME_FAS_GETLEFTCOUNTER, NULL, 0, &dwValue, sizeof(dwValue), SOCKET_WAITTIME_NORMAL);

	if (FMM_OK == nRtn)
	{
		*pdwCounter = dwValue;
	}

	return nRtn;
}


const INT CEthernetInterface::DoCmdGetMotorOrganization(IN const BYTE iAxisNo, OUT DWORD* pdwOrganization)
{
	DWORD dwValue = 0;

	const INT nRtn = DoSendCommand(iAxisNo, FRAME_MOTIONGATE_GETAXISINFO, NULL, 0, &dwValue, sizeof(dwValue), SOCKET_WAITTIME_NORMAL);

	if (FMM_OK == nRtn)
	{
		*pdwOrganization = dwValue;
	}

	return nRtn;
}



const INT CEthernetInterface::DoCmdGetMotionGateAlarmStatus(BYTE* pStatus)
{
	BYTE byValue = 0;

	int nRtn = DoSendCommand(MOTION_GATE_ID, FRAME_MOTIONGATE_ALARMCHECK, NULL, 0, &byValue, 1, SOCKET_WAITTIME_NORMAL);
	
	if (FMM_OK == nRtn)
	{
		*pStatus = byValue;
	}

	return nRtn;
}
