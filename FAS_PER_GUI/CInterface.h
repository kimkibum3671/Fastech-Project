#pragma once

#include "../Common_Define/Define_Structure.h"

class CInterface
{
public:
	CInterface();
	~CInterface();

protected:
	INT m_nLastError;

public:
	const INT GetLastError();

public:
	virtual const INT	GetInterfaceType();

	virtual const BOOL	Connect(const DWORD dwAddress, const INT nPortNo);
	virtual const BOOL	Close(const DWORD dwAddress);
//	virtual const BOOL	GetMotiongateInfo(IN const INT iID, OUT LPSTR lpszDeviceType, IN const INT cbDeviceType, OUT LPSTR lpszDeviceName, IN const INT cbDeviceName);
	virtual const BOOL	GetMotiongateInfo(IN const INT iID, MOTIONGATE_READ_INFO* pDeviceInfo, const INT cbSize);
	virtual const BOOL	ScanSlave(IN const INT iID, IN const BYTE iAxisNo, OUT BYTE* pSlaveType, OUT LPSTR lpszSlaveInfo, IN const INT cbSlaveInfo);
	virtual const BOOL	ScanSlaveFromMotiongate(IN const INT iID);
//	virtual const BOOL	ChangeMotiongateName(IN const INT iID, IN LPCTSTR lpszName);
	virtual const BOOL	ChangeMotiongateInfo(IN const INT iID, IN LPCTSTR lpszName, IN const BYTE btMotorCount, IN const BYTE btHomingTimeout);
	virtual const BOOL	RebootMotiongate(IN const INT iID);
	virtual const BOOL	GetMotiongateStatus(IN const INT iID, IN const BYTE iAxisNo, OUT BYTE* pSystemStatus, IN const INT cbSystemStatus);
	virtual const BOOL	GetAxisStatus(IN const INT iID, IN const BYTE iAxisNo, OUT DWORD* pdwStatus);
	virtual const BOOL	ResetMotionGateAlarm(IN const INT iID);
	virtual const BOOL	SetParameter(IN const INT iID, const INT nMotorIndex, const INT nParameterIndex, const LONG lValue);
	virtual const BOOL	GetParameter(IN const INT iID, const INT nMotorIndex, const INT nParameterIndex, LONG* pValue);
	virtual const BOOL	GetFlashParameter(IN const INT iID, const INT nMotorIndex, const INT nParameterIndex, LONG* pValue);
	virtual const BOOL	SetPositionTableItem(IN const INT iID, const INT nMotorIndex, const INT nTableIndex, IN LPCVOID pbtItem, IN const INT cbNodeSize);
	virtual const BOOL	GetPositionTableItem(IN const INT iID, IN const INT nMotorIndex, IN const INT nTableIndex, OUT LPVOID pbtItem, IN const INT cbNodeSize);
	virtual const BOOL	SaveAllParameters(IN const INT iID, IN const INT nMotorIndex);
	virtual const BOOL	SavePositionTableItems(IN const INT iID, IN const INT nMotorIndex);
	virtual const BOOL	LoadPositionTableItems(IN const INT iID, IN const INT nMotorIndex);
	virtual const BOOL	ChangeSlaveID(IN const INT iID, IN const INT iCurAxisNo, IN const INT iNewAxisNo);
	virtual const BOOL	EnableServo(IN const INT iID, IN const BYTE iAxisNo, IN const BOOL bEnable);
	virtual const BOOL	MoveStop(IN const INT iID, IN const BYTE iAxisNo);
	virtual const BOOL	MoveVelocity(IN const INT iID, IN const BYTE iAxisNo, const INT nSpeed, const BOOL bIncDir);
	virtual const BOOL	EnableAdministrationMode(IN const INT iID, IN const BOOL bEnable);
	virtual const BOOL	ExecutePositionTable(IN const INT iID, IN const BYTE iAxisNo, IN const INT nTableIndex);
	virtual const BOOL	SearchOriginSingle(IN const INT iID, IN const BYTE iAxisNo);
	virtual const BOOL	NotifyBackupDone(IN const INT iID, IN const WORD wElapse);
	virtual const BOOL	GetLifeCounter(IN const INT iID, IN const BYTE iAxisNo, OUT DWORD* pdwCounter);
	virtual const BOOL	GetMotionGateAlarmStatus(IN const INT iID, OUT BYTE* pdwStatus);
	virtual const BOOL	GetAlarmType(IN const INT iID, IN const BYTE iAxisNo, OUT BYTE* pStatus);
	virtual const BOOL	SetMotionGateAlarmReset(IN const INT iID);
	virtual const BOOL	GetMotorOrganization(IN const INT iID, IN const BYTE iAxisNo, OUT DWORD* pdwOrganization);
};
