#pragma once

#include <WinSock2.h>

#include "../Common_Define/Define_MonitorStatus.h"
#include "../Common_Define/Define_Structure.h"

#ifdef EZI_SERVO_PLUSER_EXPORTS
#ifdef __cplusplus
#define EZI_PLUSER_API /*extern "C"*/	__declspec(dllexport)
#else
#define EZI_PLUSER_API __declspec(dllexport)
#endif
#else
#ifdef __cplusplus
#define EZI_PLUSER_API /*extern "C"*/	__declspec(dllimport)	
#else
#define EZI_PLUSER_API __declspec(dllimport)
#endif // __cplusplus
#endif

#ifdef __FAS_NAMESPACE
#ifdef __cplusplus
namespace FAS_PER
{
#else
#define FAS_FUNCTION(x)	(x)
#endif	//	__cplusplus
#endif	//	__FAS_NAMESPACE
	/***
	* ----------------------------------------------------------------------------------------------------
	*                               Functions for connection
	* ----------------------------------------------------------------------------------------------------
	*/
	
	EZI_PLUSER_API	INT      WINAPI    FAS_ConnectTCP(IN const INT iID, const BYTE sb1, const BYTE sb2, const BYTE sb3, const BYTE sb4, const WORD wPortNo);	// TCP Protocol
	EZI_PLUSER_API	INT      WINAPI    FAS_ConnectUDP(IN const INT iID, const BYTE sb1, const BYTE sb2, const BYTE sb3, const BYTE sb4, const WORD wPortNo);	// UDP Protocol
	EZI_PLUSER_API	INT      WINAPI    FAS_Close(IN const INT iID);
	EZI_PLUSER_API	INT      WINAPI    FAS_Reconnect(IN const INT iID);
	EZI_PLUSER_API	INT      WINAPI    FAS_SetAutoReconnect(IN const INT iID, IN const BOOL bEnable);

	/***
	* ----------------------------------------------------------------------------------------------------
	*                               Functions for connection status
	* ----------------------------------------------------------------------------------------------------
	*/
	

	/***
	* ----------------------------------------------------------------------------------------------------
	*                               Functions for device status
	* ----------------------------------------------------------------------------------------------------
	*/
	EZI_PLUSER_API INT      WINAPI    FAS_GetMotiongateInfo(IN const INT iID, OUT LPVOID pDeviceInfo, IN const INT cbBuffer);
	EZI_PLUSER_API INT      WINAPI    FAS_GetSlaveInfo(IN const INT iID, IN const BYTE iAxisNo, OUT BYTE* pSlaveType, OUT LPSTR lpszSlaveInfo, IN const INT cbSlaveInfo);
	EZI_PLUSER_API INT      WINAPI    FAS_GetSlaveInfoQuick(IN const INT iID, IN const BYTE iAxisNo, OUT BYTE* pSlaveType, OUT LPSTR lpszSlaveInfo, IN const INT cbSlaveInfo);
	EZI_PLUSER_API INT      WINAPI    FAS_GetMotiongateSystemStatus(IN const INT iID, IN const BYTE iAxisNo, OUT BYTE* pSystemStatus, IN INT cbSystemStatus);
	EZI_PLUSER_API INT      WINAPI    FAS_EnterMonitoringMode(IN const INT iID, IN const BOOL bEnter);
	EZI_PLUSER_API INT      WINAPI    FAS_ChangeMotiongateInfo(IN const INT iID, IN const CHAR* chName, IN const BYTE btMotorCount, IN const BYTE btHomingTimeout);
	EZI_PLUSER_API INT      WINAPI    FAS_ResetMotionGateAlarm(IN const INT iID);
	EZI_PLUSER_API INT      WINAPI    FAS_ChangeSlaveID(IN const INT iID, IN const BYTE iCurAxisNo, IN const BYTE iNewAxisNo);
	EZI_PLUSER_API INT      WINAPI    FAS_RebootMotionGate(IN const INT iID);
	EZI_PLUSER_API INT      WINAPI    FAS_GetAxisStatus(IN const INT iID, IN const BYTE iAxisNo, OUT DWORD* dwAxisStatus);
	EZI_PLUSER_API INT      WINAPI	  FAS_GetAlarmType(IN const INT iID, IN const BYTE iAxisNo, OUT BYTE* pStatus);
	EZI_PLUSER_API INT      WINAPI    FAS_GetAllStatus(IN const INT iID, IN const BYTE iAxisNo, OUT ULONGLONG* uInStatus, OUT DWORD* dwOutStatus, OUT DWORD* dwAxisStatus, OUT long* lCmdPos, OUT long* lActPos, OUT long* lPosErr, OUT long* lActVel, OUT WORD* wPosItemNo);
	EZI_PLUSER_API INT      WINAPI    FAS_RescanMotiongateSlave(IN const INT iID);
	EZI_PLUSER_API INT      WINAPI    FAS_NofityBackupDone(IN const INT iID, IN const WORD wElapse );
	EZI_PLUSER_API INT      WINAPI    FAS_GetLifeCounter(IN const INT iID, IN const BYTE iAxisNo, OUT DWORD* pdwCounter);
	EZI_PLUSER_API INT      WINAPI    FAS_GetMotionGateAlarmStatus(IN const INT iID, OUT BYTE* pStatus);
	EZI_PLUSER_API INT      WINAPI    FAS_SetMotionGateAlarmReset(IN const INT iID);
	EZI_PLUSER_API INT      WINAPI    FAS_GetMotorOrganization(IN const INT iID, IN const BYTE iAxisNo, OUT DWORD* pdwOrganization);

	/***
	* ----------------------------------------------------------------------------------------------------
	*                               Functions for servo driver control
	* ----------------------------------------------------------------------------------------------------		
	*/
	EZI_PLUSER_API INT		WINAPI	FAS_ServoEnable(IN const INT iID, IN const BYTE iSlaveNo, IN const BOOL bOnOff);
	EZI_PLUSER_API INT		WINAPI	FAS_MoveVelocity(IN const INT iID, IN const BYTE iSlaveNo, IN const DWORD lVelocity, const INT iVelDir);
	EZI_PLUSER_API INT		WINAPI	FAS_MoveStop(IN const INT iID, IN BYTE iSlaveNo);
	EZI_PLUSER_API INT		WINAPI	FAS_EmergencyStop(IN const INT iID, IN BYTE iSlaveNo);
	EZI_PLUSER_API INT		WINAPI	FAS_PosTableRunItem(IN const INT iID, IN const BYTE iSlaveNo, IN const WORD wItemNo);
	EZI_PLUSER_API INT		WINAPI	FAS_MoveOriginSingleAxis(IN const INT iID, IN const BYTE iSlaveNo);

	/***
	* ----------------------------------------------------------------------------------------------------
	*                               Functions for parameter control
	* ----------------------------------------------------------------------------------------------------
	*/
	EZI_PLUSER_API INT		WINAPI	FAS_SetParameter(IN const INT iID, IN const BYTE iSlaveNo, IN const BYTE iParamNo, IN const LONG lParamValue);
	EZI_PLUSER_API INT		WINAPI	FAS_GetParameter(IN const INT iID, IN const BYTE iSlaveNo, IN const BYTE iParamNo, OUT LONG* lParamValue);
	EZI_PLUSER_API INT		WINAPI	FAS_SaveAllParameters(IN const INT iID, IN const BYTE iSlaveNo);
	EZI_PLUSER_API INT		WINAPI	FAS_GetROMParameter(IN const INT iID, IN const BYTE iSlaveNo, IN const BYTE iParamNo, OUT long* lRomParam);

	EZI_PLUSER_API INT		WINAPI	FAS_PosTableWriteItem(IN const INT iID, IN const BYTE iSlaveNo, IN const WORD wItemNo, IN LPITEM_NODE lpItem);
	EZI_PLUSER_API INT		WINAPI	FAS_PosTableReadItem(IN const INT iID, IN const BYTE iSlaveNo, IN const WORD wItemNo, OUT LPITEM_NODE lpItem);
	EZI_PLUSER_API INT		WINAPI	FAS_PosTableWriteROM(IN const INT iID, IN const BYTE iSlaveNo);
	EZI_PLUSER_API INT		WINAPI	FAS_PosTableReadROM(IN const INT iID, IN const BYTE iSlaveNo);

#ifdef __FAS_NAMESPACE
#ifdef __cplusplus
}
#endif	//	__cplusplus
#endif	//	__FAS_NAMESPACE
