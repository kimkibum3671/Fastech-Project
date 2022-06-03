// dllmain.cpp : DLL 애플리케이션의 진입점을 정의합니다.

#include "FAS_EziMotionPlusER.h"

#include ".\Ethernet\EthernetManager.h"

CEthernetManager* m_pCmd = CEthernetManager::GetInstance();

#ifdef __FAS_NAMESPACE
#ifdef __cplusplus
namespace FAS_PER
{
#endif
#endif
	EZI_PLUSER_API INT      WINAPI    FAS_ConnectTCP(IN const INT iID, const BYTE sb1, const BYTE sb2, const BYTE sb3, const BYTE sb4, const WORD wPortNo)	// TCP Protocol
	{
		return m_pCmd->MakeNewSocket(TRUE, iID, sb1, sb2, sb3, sb4, wPortNo);
	}

	EZI_PLUSER_API INT      WINAPI    FAS_ConnectUDP(IN const INT iID, const BYTE sb1, const BYTE sb2, const BYTE sb3, const BYTE sb4, const WORD wPortNo)	// TCP Protocol
	{
		return m_pCmd->MakeNewSocket(FALSE, iID, sb1, sb2, sb3, sb4, wPortNo);
	}

	EZI_PLUSER_API INT      WINAPI    FAS_Close(IN const INT iID)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		return m_pCmd->RemoveSocket(iID);
	}

	EZI_PLUSER_API INT      WINAPI    FAS_Reconnect(IN const INT iID)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		return pSocket->Reconnect();
	}

	EZI_PLUSER_API INT      WINAPI    FAS_SetAutoReconnect(IN const INT iID, IN const BOOL bEnable)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		return pSocket->EnableAutoReconnect(bEnable);
	}

	/*EZI_PLUSER_API INT      WINAPI    FAS_GetMotiongateInfo(IN const INT iID, OUT LPSTR lpszDeviceType, IN const INT cbDeviceType, OUT LPSTR lpszDeviceName, IN const INT cbDeviceName)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		INT nResult = pSocket->DoCmdGetMotiongateInfo(lpszDeviceType, cbDeviceType, lpszDeviceName, cbDeviceName);

		return nResult;
	}*/

	EZI_PLUSER_API INT      WINAPI    FAS_GetMotiongateInfo(IN const INT iID, OUT LPVOID pDeviceInfo, IN const INT cbBuffer )
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		INT nResult = pSocket->DoCmdGetMotiongateInfo(pDeviceInfo, cbBuffer);

		return nResult;
	}

	EZI_PLUSER_API INT      WINAPI    FAS_GetSlaveInfo(IN const INT iID, IN const BYTE iAxisNo, OUT BYTE* pSlaveType, OUT LPSTR lpszSlaveInfo, IN const INT cbSlaveInfo)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		INT nResult = pSocket->DoCmdGetSlaveInfo(iAxisNo, pSlaveType, lpszSlaveInfo, cbSlaveInfo);

		return nResult;
	}

	EZI_PLUSER_API INT      WINAPI    FAS_GetSlaveInfoQuick(IN const INT iID, IN const BYTE iAxisNo, OUT BYTE* pSlaveType, OUT LPSTR lpszSlaveInfo, IN const INT cbSlaveInfo)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		INT nResult = pSocket->DoCmdGetSlaveInfo(iAxisNo, pSlaveType, lpszSlaveInfo, cbSlaveInfo, FALSE);

		return nResult;
	}

	EZI_PLUSER_API INT      WINAPI    FAS_GetMotiongateSystemStatus(IN const INT iID, IN BYTE iAxisNo, OUT BYTE* pSystemStatus, IN INT cbSystemStatus)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		INT nResult = pSocket->DoCmdGetMotiongateSystemStatus(iAxisNo, pSystemStatus, cbSystemStatus);

		return nResult;
	}

	EZI_PLUSER_API INT      WINAPI    FAS_EnterMonitoringMode(IN const INT iID, IN const BOOL bEnter)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		INT nResult = pSocket->DoCmdSetMonitoringMode(bEnter);

		return nResult;
	}

	EZI_PLUSER_API INT		WINAPI	FAS_ServoEnable(IN const INT iID, IN const BYTE iSlaveNo, IN const BOOL bOnOff)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		INT nResult = pSocket->DoCmdServoEnable(iSlaveNo, bOnOff);

		return nResult;
	}

	EZI_PLUSER_API INT WINAPI	FAS_MoveVelocity(IN const INT iID, BYTE iSlaveNo, DWORD lVelocity, int iVelDir)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		INT nResult = pSocket->DoCmdMoveVelocity(iSlaveNo, lVelocity, iVelDir);

		return nResult;
	}

	EZI_PLUSER_API INT		WINAPI	FAS_MoveStop(IN const INT iID, IN BYTE iSlaveNo)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		INT nResult = pSocket->DoCmdMoveStop(iSlaveNo);

		return nResult;
	}

	EZI_PLUSER_API INT		WINAPI	FAS_EmergencyStop(IN const INT iID, IN BYTE iSlaveNo)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		INT nResult = pSocket->DoCmdEmergencyStop(iSlaveNo);

		return nResult;
	}

	EZI_PLUSER_API INT		WINAPI	WINAPI	FAS_PosTableRunItem(IN const INT iID, IN BYTE iSlaveNo, IN WORD wItemNo)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		INT nResult = pSocket->DoCmdPosTableRunItem(iSlaveNo, wItemNo);

		return nResult;
	}

	EZI_PLUSER_API INT		WINAPI	WINAPI	FAS_MoveOriginSingleAxis(IN const INT iID, IN BYTE iSlaveNo)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		INT nResult = pSocket->DoCmdMoveOriginSingleAxis(iSlaveNo);

		return nResult;
	}

	EZI_PLUSER_API INT      WINAPI    FAS_ChangeMotiongateInfo(IN const INT iID, IN const CHAR* chName, IN const BYTE btMotorCount, IN const BYTE btHomingTimeout)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		INT nResult = pSocket->DoCmdChangeMotiongateInfo(chName, strlen(chName), btMotorCount, btHomingTimeout);

		return nResult;
	}

	EZI_PLUSER_API INT      WINAPI    FAS_ResetMotionGateAlarm(IN const INT iID)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		INT nResult = pSocket->DoCmdResetMotionGateAlarm();

		return nResult;
	}

	EZI_PLUSER_API INT		WINAPI	FAS_SetParameter(IN const INT iID, IN const BYTE iSlaveNo, IN const BYTE iParamNo, IN const LONG lParamValue)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		INT nResult = pSocket->DoCmdSetParameter(iSlaveNo, iParamNo, lParamValue);

		return nResult;
	}

	EZI_PLUSER_API INT		WINAPI	FAS_GetParameter(IN const INT iID, IN const BYTE iSlaveNo, IN const BYTE iParamNo, OUT LONG* lParamValue)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		INT nResult = pSocket->DoCmdGetParameter(iSlaveNo, iParamNo, lParamValue);

		return nResult;
	}

	EZI_PLUSER_API INT		WINAPI	FAS_SaveAllParameters(IN const INT iID, IN const BYTE iSlaveNo)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		INT nResult = pSocket->DoCmdSaveAllParam(iSlaveNo);

		return nResult;
	}

	EZI_PLUSER_API INT		WINAPI	FAS_PosTableWriteItem(IN const INT iID, IN const BYTE iSlaveNo, IN const WORD wItemNo, IN LPITEM_NODE lpItem)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		INT nResult = pSocket->DoCmdPosTableWriteItem(iSlaveNo, wItemNo, lpItem);

		return nResult;
	}

	EZI_PLUSER_API INT		WINAPI	FAS_PosTableReadItem(IN const INT iID, IN const BYTE iSlaveNo, IN const WORD wItemNo, OUT LPITEM_NODE lpItem)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		INT nResult = pSocket->DoCmdPosTableReadItem(iSlaveNo, wItemNo, lpItem);

		return nResult;
	}

	EZI_PLUSER_API INT		WINAPI	FAS_PosTableWriteROM(IN const INT iID, IN const BYTE iSlaveNo)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		INT nResult = pSocket->DoCmdPosTableWriteROM(iSlaveNo);

		return nResult;
	}

	EZI_PLUSER_API INT		WINAPI	FAS_GetROMParameter(IN const INT iID, IN const BYTE iSlaveNo, IN const BYTE iParamNo, OUT long* lRomParam)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		INT nResult = pSocket->DoCmdGetROMParam(iSlaveNo, iParamNo, lRomParam);

		return nResult;
	}

	EZI_PLUSER_API INT		WINAPI	FAS_PosTableReadROM(IN const INT iID, IN const BYTE iSlaveNo)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		INT nResult = pSocket->DoCmdPosTableReadROM(iSlaveNo);

		return nResult;
	}

	EZI_PLUSER_API INT      WINAPI    FAS_ChangeSlaveID(IN const INT iID, IN const BYTE iCurAxisNo, IN const BYTE iNewAxisNo)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		INT nResult = pSocket->DoCmdChangeSlaveID(iCurAxisNo, iNewAxisNo);

		return nResult;
	}

	EZI_PLUSER_API INT      WINAPI    FAS_RebootMotionGate(IN const INT iID)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;
		
		INT nResult = pSocket->DoCmdRebootMotionGate();

		return nResult;
	}


	EZI_PLUSER_API INT      WINAPI    FAS_GetAllStatus(IN const INT iID, IN const BYTE iAxisNo, OUT ULONGLONG* uInStatus, OUT DWORD* dwOutStatus, OUT DWORD* dwAxisStatus, OUT long* lCmdPos, OUT long* lActPos, OUT long* lPosErr, OUT long* lActVel, OUT WORD* wPosItemNo)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		return pSocket->DoCmdGetAllStatus(iAxisNo, uInStatus, dwOutStatus, dwAxisStatus, lCmdPos, lActPos, lPosErr, lActVel, wPosItemNo);
	}

	EZI_PLUSER_API INT      WINAPI    FAS_GetAxisStatus(IN const INT iID, IN const BYTE iAxisNo, OUT DWORD* dwAxisStatus)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		return pSocket->DoCmdGetAxisStatus(iAxisNo, dwAxisStatus);
	}

	EZI_PLUSER_API INT      WINAPI    FAS_GetAlarmType(IN const INT iID, IN const BYTE iAxisNo, OUT BYTE* pStatus)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		return pSocket->DoCmdGetAlarmType(iAxisNo, pStatus);
	}

	

	EZI_PLUSER_API INT      WINAPI    FAS_RescanMotiongateSlave(IN const INT iID)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		return pSocket->DoCmdRescanMotiongateSlave();
	}

	EZI_PLUSER_API INT      WINAPI    FAS_NofityBackupDone(IN const INT iID, IN const WORD wElapse)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		return pSocket->DoCmdNotifyBackupDone(wElapse);
	}

	EZI_PLUSER_API INT      WINAPI    FAS_GetLifeCounter(IN const INT iID, IN const BYTE iAxisNo, OUT DWORD* pdwCounter )
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		return pSocket->DoCmdGetLifeCounter(iAxisNo, pdwCounter);
	}

	EZI_PLUSER_API INT      WINAPI    FAS_GetMotorOrganization(IN const INT iID, IN const BYTE iAxisNo, OUT DWORD* pdwOrganization)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		return pSocket->DoCmdGetMotorOrganization(iAxisNo, pdwOrganization);
	}

	
	EZI_PLUSER_API INT      WINAPI    FAS_GetMotionGateAlarmStatus(IN const INT iID, OUT BYTE* pStatus)
	{
		CEthernetInterface* pSocket  = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		return pSocket->DoCmdGetMotionGateAlarmStatus(pStatus);
	}

	EZI_PLUSER_API INT      WINAPI    FAS_SetMotionGateAlarmReset(IN const INT iID)
	{
		CEthernetInterface* pSocket = m_pCmd->GetSocketbyBdID(iID);

		if (pSocket == NULL) return FMM_NOT_OPEN;

		return pSocket->DoCmdResetMotionGateAlarm();
	}

	

#ifdef __FAS_NAMESPACE
#ifdef __cplusplus
}
#endif
#endif