#include "pch.h"
#include "CInterface.h"

#include "CDataBaseManager.h"

#include "CInterfaceSocket.h"
#include "CInterfaceSerial.h"

#include "..\Common_Define\Define_ReturnCode.h"

CInterface::CInterface()
{
	
}

CInterface::~CInterface()
{

}

const INT CInterface::GetLastError()
{
	return m_nLastError;
}

const INT CInterface::GetInterfaceType()
{
	return CDataBaseConfig::GetInstance()->GetInterface();
}

const BOOL CInterface::Connect(const DWORD dwAddress, const INT nPortNo)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}

const BOOL CInterface::Close(const DWORD dwAddress)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}

const BOOL	CInterface::GetMotiongateInfo(IN const INT iID, MOTIONGATE_READ_INFO* pDeviceInfo, const INT cbSize)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}

const BOOL CInterface::ScanSlave(IN const INT iID, IN const BYTE iAxisNo, OUT BYTE* pSlaveType, OUT LPSTR lpszSlaveInfo, IN const INT cbSlaveInfo)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}

const BOOL	CInterface::ScanSlaveFromMotiongate(IN const INT iID)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}

const BOOL	CInterface::GetMotiongateStatus(IN const INT iID, IN const BYTE iAxisNo, OUT BYTE* pSystemStatus, IN const INT cbSystemStatus)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}

const BOOL CInterface::GetAxisStatus(IN const INT iID, IN const BYTE iAxisNo, OUT DWORD* pdwStatus)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}

const BOOL	CInterface::ResetMotionGateAlarm(IN const INT iID)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}

const BOOL	CInterface::SetParameter(IN const INT iID, const INT nMotorIndex, const INT nParameterIndex, const LONG lValue)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}

const BOOL	CInterface::GetParameter(IN const INT iID, const INT nMotorIndex, const INT nParameterIndex, LONG* pValue)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}

const BOOL	CInterface::GetFlashParameter(IN const INT iID, const INT nMotorIndex, const INT nParameterIndex, LONG* pValue)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}

const BOOL	CInterface::SetPositionTableItem(IN const INT iID, const INT nMotorIndex, const INT nTableIndex, IN LPCVOID pbtItem, IN const INT cbNodeSize)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}

const BOOL CInterface::GetPositionTableItem(IN const INT iID, IN const INT nMotorIndex, IN const INT nTableIndex, OUT LPVOID pbtItem, IN const INT cbNodeSize)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}

const BOOL	CInterface::SaveAllParameters(IN const INT iID, IN const INT nMotorIndex)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}

const BOOL	CInterface::SavePositionTableItems(IN const INT iID, IN const INT nMotorIndex)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}

const BOOL CInterface::LoadPositionTableItems(IN const INT iID, IN const INT nMotorIndex)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}

const BOOL CInterface::ChangeSlaveID(IN const INT iID, IN const INT iCurAxisNo, IN const INT iNewAxisNo)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}

const BOOL CInterface::EnableServo(IN const INT iID, IN const BYTE iAxisNo, IN const BOOL bEnable)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}

const BOOL	CInterface::MoveStop(IN const INT iID, IN const BYTE iAxisNo)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}

const BOOL CInterface::MoveVelocity(IN const INT iID, IN const BYTE iAxisNo, const INT nSpeed, const BOOL bIncDir)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}

const BOOL	CInterface::EnableAdministrationMode(IN const INT iID, IN const BOOL bEnable)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}

const BOOL	CInterface::ChangeMotiongateInfo(IN const INT iID, IN LPCTSTR lpszName, IN const BYTE btMotorCount, IN const BYTE btHomingTimeout)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}


const BOOL	CInterface::RebootMotiongate(IN const INT iID)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}

const BOOL	CInterface::ExecutePositionTable(IN const INT iID, IN const BYTE iAxisNo, IN const INT nTableIndex)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}

const BOOL	CInterface::SearchOriginSingle(IN const INT iID, IN const BYTE iAxisNo)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}

const BOOL	CInterface::NotifyBackupDone(IN const INT iID, IN const WORD wElapse)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}

const BOOL	CInterface::GetLifeCounter(IN const INT iID, IN const BYTE iAxisNo, OUT DWORD* pdwCounter)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}

const BOOL	CInterface::GetMotorOrganization(IN const INT iID, IN const BYTE iAxisNo, OUT DWORD* pdwOrganization)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}

const BOOL	CInterface::GetMotionGateAlarmStatus(IN const INT iID, OUT BYTE* pdwStatus)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}

const BOOL	CInterface::SetMotionGateAlarmReset(IN const INT iID)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}

const BOOL CInterface::GetAlarmType(IN const INT iID, IN const BYTE iAxisNo, OUT BYTE* pStatus)
{
	m_nLastError = FMM_INVALID_DEVICE;
	return FALSE;
}