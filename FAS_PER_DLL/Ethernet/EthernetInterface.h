#pragma once

#include "FSocket.h"

#include <Windows.h>

#include "../../Common_Define/Define_ReturnCode.h"
#include "../../Common_Define/Define_Structure.h"

class CEthernetInterface : public CFSocket
{
public:
	CEthernetInterface(BOOL bTCP, IN_ADDR addr, WORD wPort, int iBdID);
	virtual ~CEthernetInterface(void);

protected:
	//------------------------------------------------------------------------------
	//					Connect and BdID
	//------------------------------------------------------------------------------
	// BdID
protected:
	int		m_iBdID;
	BOOL	m_bReconnect;

public:
	void		SetBdID(int iBdID)	{m_iBdID = iBdID;}
	const int	GetBdID() const		{return m_iBdID;}
	
public:
	//------------------------------------------------------------------------------
	//					Inner Functions
	//------------------------------------------------------------------------------
	const INT	DoAck(IN const BYTE iAxisNo, IN const DWORD dwWaitTime);
	const INT	EnableAutoReconnect(IN const BOOL bEnable);

	//------------------------------------------------------------------------------
	//					No-Motion Command Functions
	//------------------------------------------------------------------------------
//	int		DoCmdGetSlaveInfoV6(BYTE iAxisNo, BYTE* pType, LPSTR lpBuff, int nBuffSize);
//	int		DoCmdGetSlaveInfoV8(BYTE iAxisNo, DRIVE_INFO* lpDriveInfo);
//	int		DoCmdGetSlaveInfoEx(BYTE iAxisNo, DRIVE_INFO* lpDriveInfo);
//	int		DoCmdGetMotorInfo(BYTE iAxisNo, BYTE* pType, LPSTR lpBuff, int nBuffSize);

	//------------------------------------------------------------------------------
	//					Parameter Functions
	//------------------------------------------------------------------------------
	const INT	DoCmdSaveAllParam(IN const BYTE iAxisNo);
	const INT	DoCmdSetParameter(IN const BYTE iAxisNo, IN const BYTE iParaNo, IN LONG lParaValue);
	const INT	DoCmdGetParameter(IN const BYTE iAxisNo, IN const BYTE iParaNo, OUT LONG* lParamValue);
	const INT	DoCmdGetROMParam(IN const BYTE iAxisNo, IN const BYTE iParaNo, OUT long* lROMParam);

	//------------------------------------------------------------------------------
	//					IO Functions
	//------------------------------------------------------------------------------
//	int		DoCmdSetIOInput(BYTE iAxisNo, DWORD dwIOSETMask, DWORD dwIOCLRMask);
//	int		DoCmdGetIOInput(BYTE iAxisNo, DWORD* dwIOInput);

//	int		DoCmdSetIOOutput(BYTE iAxisNo, DWORD dwIOSETMask, DWORD dwIOCLRMask);
//	int		DoCmdGetIOOutput(BYTE iAxisNo, DWORD* dwIOOutput);

//	int		DoCmdGetIOAssignMap(BYTE iAxisNo, BYTE iIOPinNo, DWORD* dwIOLogicMask, BYTE* bLevel);
//	int		DoCmdSetIOAssignMap(BYTE iAxisNo, BYTE iIOPinNo, DWORD dwIOLogicMask, BYTE bLevel);

//	int		DoCmdIOAssignMapReadROM(BYTE iAxisNo);

	//------------------------------------------------------------------------------
	//					Servo Driver Control Functions
	//------------------------------------------------------------------------------		
	const INT DoCmdServoEnable( IN const BYTE iAxisNo, IN const BOOL bOnOff);
//	int		DoCmdServoAlarmReset(BYTE iAxisNo);
//	int		DoCmdStepAlarmReset(BYTE iAxisNo, BOOL bReset);

	//------------------------------------------------------------------------------
	//					Read Status and Position
	//------------------------------------------------------------------------------
	const INT DoCmdGetAxisStatus(IN const BYTE iAxisNo, OUT DWORD* dwAxisStatus);
//	int		DoCmdGetIOAxisStatus(BYTE iAxisNo, DWORD* dwInStatus, DWORD* dwOutStatus, DWORD* dwAxisStatus);
//	int		DoCmdGetMotionStatus(BYTE iAxisNo, long* lCmdPos, long* lActPos, long* lPosErr, long* lActVel, WORD* wPosItemNo); /*Question : lPosItemNo는 WORD가 아닌가??*/
	const INT DoCmdGetAllStatus(IN const BYTE iAxisNo, OUT ULONGLONG* dwInStatus, OUT DWORD* dwOutStatus, OUT DWORD* dwAxisStatus, OUT long* lCmdPos, OUT long* lActPos, OUT long* lPosErr, OUT long* lActVel, OUT WORD* wPosItemNo);

//	int		DoCmdSetCommandPos(BYTE iAxisNo, long lCmdPos);
//	int		DoCmdSetActualPos(BYTE iAxisNo, long lActPos);
//	int		DoCmdClearPosition(BYTE iAxisNo);
//	int		DoCmdGetCommandPos(BYTE iAxisNo, long* lCmdPos);
//	int		DoCmdGetActualPos(BYTE iAxisNo, long* lActPos);
//	int		DoCmdGetPosError(BYTE iAxisNo, long* lPosErr);
//	int		DoCmdGetActualVel(BYTE iAxisNo, long* lActVel);

	int		DoCmdGetAlarmType(BYTE iAxisNo, BYTE* pStatus);

//	int		DoCmdGetAllTorqueStatus(BYTE iAxisNo, DWORD* dwInStatus, DWORD* dwOutStatus, DWORD* dwAxisStatus, long* lCmdPos, long* lActPos, long* lPosErr, long* lActVel, WORD* wPosItemNo, WORD* wTorqueValue);
//	int		DoCmdGetTorqueStatus(BYTE iAxisNo, WORD* wTorqueValue);

	//------------------------------------------------------------------
	//					Motion Functions.
	//------------------------------------------------------------------
	const INT DoCmdMoveStop(BYTE iAxisNo);
	const INT DoCmdEmergencyStop(BYTE iAxisNo);

//	int		DoCmdMovePause(BYTE iAxisNo, BOOL bPause);

	const INT DoCmdMoveOriginSingleAxis(IN const BYTE iAxisNo);
//	int		DoCmdMoveSingleAxisAbsPos(BYTE iAxisNo, long lAbsPos, DWORD lVelocity);
//	int		DoCmdMoveSingleAxisIncPos(BYTE iAxisNo, long lIncPos, DWORD lVelocity);
//	int		DoCmdMoveToLimit(BYTE iAxisNo, DWORD lVelocity, int iLimitDir);
	const INT DoCmdMoveVelocity(IN const BYTE iAxisNo, IN const DWORD dwSpeed, IN const BYTE btDirection);

//	int		DoCmdPositionAbsOverride(BYTE iAxisNo, long lOverridePos);
//	int		DoCmdPositionIncOverride(BYTE iAxisNo, long lOverridePos);
//	int		DoCmdVelocityOverride(BYTE iAxisNo, DWORD lVelocity);

//	int		DoCmdSetLinearInfo(BYTE iAxisNo, long lIncPos, DWORD lFeedrate, WORD wAccelTime);
//	int		DoCmdMoveLinearInc(BYTE iAxisNo, DWORD dwSlavesMask);

//	int		DoCmdTriggerPulseOutput(BYTE iAxisNo, BOOL bStartTrigger, long lStartPos, DWORD dwPeriod, DWORD dwPulseTime, BYTE nOutputPin, DWORD dwReserved);
//	int		DoCmdTriggerPulseStatus(BYTE iAxisNo, BYTE* bTriggerStatus);
	
//	int		DoCmdSetTriggerOutputEx(BYTE iAxisNo, BYTE nOutputNo, BYTE bRun, WORD wOnTime, BYTE nTriggerCount, long* arrTriggerPosition);
//	int		DoCmdGetTriggerOutputEx(BYTE iAxisNo, BYTE nOutputNo, BYTE* bRun, WORD* wOnTime, BYTE* nTriggerCount, long* arrTriggerPosition);

//	int		DoCmdMovePush(BYTE iAxisNo, DWORD dwStartSpd, DWORD dwMoveSpd, long lPosition, WORD wAccel, WORD wDecel, WORD wPushRate, DWORD dwPushSpd, long lEndPosition, WORD wPushMode);
//	int		DoCmdGetPushStatus(BYTE iAxisNo, BYTE* nPushStatus);

	//------------------------------------------------------------------
	//					Ex-Motion Functions.
	//------------------------------------------------------------------
//	int		DoCmdMoveSingleAxisAbsPosEx(BYTE iAxisNo, long lAbsPos, DWORD lVelocity, MOTION_OPTION_EX* lpExOption);
//	int		DoCmdMoveSingleAxisIncPosEx(BYTE iAxisNo, long lIncPos, DWORD lVelocity, MOTION_OPTION_EX* lpExOption);
//	int		DoCmdMoveVelocityEx(BYTE iAxisNo, DWORD lVelocity, int iVelDir, VELOCITY_OPTION_EX* lpExOption);

	////------------------------------------------------------------------
	////					All-Motion Functions.
	////------------------------------------------------------------------
	//int		DoCmdAllMoveStop(BYTE iAxisNo);
	//int		DoCmdAllEmergencyStop(BYTE iAxisNo);
	//int		DoCmdAllMoveOriginSingleAxis(BYTE iAxisNo);
	//int		DoCmdAllMoveSingleAxisAbsPos(BYTE iAxisNo, long lAbsPos, DWORD lVelocity);
	//int		DoCmdAllMoveSingleAxisIncPos(BYTE iAxisNo, long lIncPos, DWORD lVelocity);

	//------------------------------------------------------------------
	//					Position Table Functions.
	//------------------------------------------------------------------
	const INT	DoCmdPosTableReadItem(IN const BYTE iAxisNo, IN const WORD wItemNo, OUT LPITEM_NODE lpItem);
	const INT	DoCmdPosTableWriteItem(IN const BYTE iAxisNo, IN const WORD wItemNo, IN const LPITEM_NODE lpItem);
	const INT	DoCmdPosTableWriteROM(IN const BYTE iAxisNo);
	const INT	DoCmdPosTableReadROM(IN const BYTE iAxisNo);
	const INT	DoCmdPosTableRunItem(IN const BYTE iAxisNo, IN const WORD wItemNo);
//	int		DoCmdPosTableIsData(BYTE iAxisNo, LPPOSTABLE_DATAFLAG pBitFlag);
//	int		DoCmdPosTableIsDataEx(BYTE iAxisNo, DWORD dwSectionNo, LPPOSTABLE_DATAFLAG_EX pBitFlag);

//	int		DoCmdPosTableRunOneItem(BYTE iAxisNo, BOOL bNextMove, WORD wItemNo);
//	int		DoCmdPosTableCheckStopMode(BYTE iAxisNo, BYTE* pStopMode);

//	int		DoCmdPosTableReadOneItem(BYTE iAxisNo, WORD wItemNo, WORD wOffset, long* lPosItemVal);
//	int		DoCmdPosTableWriteOneItem(BYTE iAxisNo, WORD wItemNo, WORD wOffset, long lPosItemVal);

	////------------------------------------------------------------------
	////					STEP Motor Functions.
	////------------------------------------------------------------------
	//int		DoCmdSTEPGetCurrent(BYTE iAxisNo, BYTE iMotorIndex, WORD* wCurrent);
	//int		DoCmdSTEPSetCurrent(BYTE iAxisNo, WORD wCurrent);
	//int		DoCmdSTEPVersion(BYTE iAxisNo, LPSTR lpSTEPVer, int nBuffSize);
	//int		DoCmdSTEPGetMotorDB(BYTE iAxisNo, LIST_MOTORDB* lpMotorDBList);
	//int		DoCmdSTEPSetMotorDB(BYTE iAxisNo, BYTE iMotorIndex);

	////------------------------------------------------------------------
	////					STEP Motor Functions.
	////------------------------------------------------------------------
	//int		DoCmdSERVOGetEncoder(BYTE iAxisNo, WORD* wEncoderVal);

	////------------------------------------------------------------------
	////					Calibration Functions.
	////------------------------------------------------------------------
	//int		DoCmdStartCalibration(BYTE iAxisNo);
	//int		DoCmdGetCalibrationResult(BYTE iAxisNo, DWORD* dwResult);
	//int		DoCmdSetCalibrationData(BYTE iAxisNo, BYTE fMotorType, DWORD lMotorPitch, DWORD lEncoderResolution);

	////------------------------------------------------------------------
	////					Gap Control Functions.
	////------------------------------------------------------------------
	//int		DoCmdGapControlEnable(BYTE iAxisNo, WORD wItemNo, long lGapCompSpeed, long lGapAccTime, long lGapDecTime, long lGapStartSpeed);
	//int		DoCmdGapControlDisable(BYTE iAxisNo);
	//int		DoCmdIsGapControlEnable(BYTE iAxisNo, BOOL* bIsEnable, WORD* wCurrentItemNo);

	//int		DoCmdGapControlGetADCValue(BYTE iAxisNo, long* lADCValue);
	//int		DoCmdGapOneResultMonitor(BYTE iAxisNo, BYTE* bUpdated, long* iIndex, long* lGapValue, long* lCmdPos, long* lActPos, long* lCompValue, long* lReserved); 

	//------------------------------------------------------------------
	//					Alarm Type History Functions.
	//------------------------------------------------------------------
//	int		DoCmdGetAlarmLogs(BYTE iAxisNo, ALARM_LOG* pAlarmLog);
//	int		DoCmdResetAlarmLogs(BYTE iAxisNo);

	////------------------------------------------------------------------
	////					I/O Module Functions.
	////------------------------------------------------------------------
//	int		DoCmdGetInput(BYTE iAxisNo, unsigned long* uInput, unsigned long* uLatch);

//	int		DoCmdClearLatch(BYTE iAxisNo, unsigned long uLatchMask);
//	int		DoCmdGetLatchCount(BYTE iAxisNo, unsigned char iInputNo, unsigned long* uCount);
//	int		DoCmdGetLatchCountAll(BYTE iAxisNo, unsigned long** ppuAllCount);
//	int		DoCmdClearLatchCount(BYTE iAxisNo, unsigned long uInputMask);

//	int		DoCmdGetOutput(BYTE iAxisNo, unsigned long* uOutput, unsigned long* uStatus);
//	int		DoCmdSetOutput(BYTE iAxisNo, unsigned long uSetMask, unsigned long uClearMask);

//	int		DoCmdSetTrigger(BYTE iAxisNo, unsigned char uOutputNo, TRIGGER_INFO* pTrigger);
//	int		DoCmdSetRunStop(BYTE iAxisNo, unsigned long uRunMask, unsigned long uStopMask);
//	int		DoCmdGetTriggerCount(BYTE iAxisNo, unsigned char uOutputNo, unsigned long* uCount);
	
//	int		DoCmdGetIOLevel(BYTE iAxisNo, unsigned long* uIOLevel);
//	int		DoCmdSetIOLevel(BYTE iAxisNo, unsigned long uIOLevel);
//	int		DoCmdLoadIOLevel(BYTE iAxisNo);
//	int		DoCmdSaveIOLevel(BYTE iAxisNo);

//	int		DoCmdGetInputFilter(BYTE iAxisNo, unsigned short* filter);
//	int		DoCmdSetInputFilter(BYTE iAxisNo, unsigned short filter);

	////------------------------------------------------------------------
	////					Ez-IO Plus-AD Functions
	////------------------------------------------------------------------
//	int		DoCmdGetAllADResult(BYTE iAxisNo, AD_RESULT* result);
//	int		DoCmdGetADResult(BYTE iAxisNo, BYTE channel, float* adresult);
//	int		DoCmdSetADRange(BYTE iAxisNo, BYTE channel, AD_RANGE range);

	const INT DoCmdGetMotiongateInfo(OUT LPVOID pDeviceInfo, IN const INT cbBuffer);
	const INT DoCmdGetSlaveInfo(IN const BYTE iAxisNo, OUT BYTE* pSlaveType, OUT LPSTR lpszSlaveInfo, IN const INT cbSlaveInfo, IN BOOL bRetry = TRUE);
	const INT DoCmdGetMotiongateSystemStatus(IN const BYTE iAxisNo, OUT BYTE* pSystemStatus, IN const INT cbSystemStatus);
	const INT DoCmdSetMonitoringMode(IN const BOOL bEnable);
	const INT DoCmdChangeMotiongateInfo(IN const CHAR* strName, IN const INT nStrLen, IN const BYTE btMotorCount, IN const BYTE btHomingTimeout);
	const INT DoCmdResetMotionGateAlarm();
	const INT DoCmdChangeSlaveID(IN const BYTE iCurAxisNo, const IN BYTE iNewAxisNo);
	const INT DoCmdRebootMotionGate();
	const INT DoCmdRescanMotiongateSlave();
	const INT DoCmdNotifyBackupDone(IN const WORD wElapse);
	const INT DoCmdGetLifeCounter(IN const BYTE iAxisNo, OUT DWORD* pdwCounter);
	const INT DoCmdGetMotorOrganization(IN const BYTE iAxisNo, OUT DWORD* pdwOrganization);
	const INT DoCmdGetMotionGateAlarmStatus(BYTE* pStatus);
};
