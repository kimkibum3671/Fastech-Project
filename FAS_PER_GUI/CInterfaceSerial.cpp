#include "pch.h"
#include "CInterfaceSerial.h"

#include "CDataBaseManager.h"

#include "../FAS_PR8MG_DLL/FAS_EziMotionPlusR8_MG.h"
#include "../Common_Define/Define_ReturnCode.h"

#ifdef DEBUG
#pragma comment (lib, "../Bin/Debug/FAS_PR8MG_DLL.lib")
#else
#pragma comment (lib, "../Bin/Release/FAS_PR8MG_DLL.lib")
#endif

#ifdef __FAS_NAMESPACE
using namespace FAS_PR8_MG;
#endif

CInterfaceSerial::CInterfaceSerial()
{

}

CInterfaceSerial::~CInterfaceSerial()
{

}

CInterface* CInterfaceSerial::GetInstance()
{
	static CInterfaceSerial _instance;

	return &_instance;
}

const BOOL	CInterfaceSerial::Connect(const DWORD dwAddress, const INT nPortNo)
{
	m_nPortNo = CDataBaseConfig::GetInstance()->GetSerialPort();
	
	m_nLastError = ::FAS_Open(m_nPortNo, CBR_115200);
	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSerial::Close(const DWORD dwAddress)
{
	::FAS_Close(m_nPortNo);

	m_nPortNo = -1;

	return m_nLastError = FMM_OK;
}

const BOOL	CInterfaceSerial::GetMotiongateInfo(IN const INT iID, MOTIONGATE_READ_INFO* pDeviceInfo, const INT cbSize)
{
	m_nLastError = ::FAS_GetMotiongateInfo(m_nPortNo, pDeviceInfo, cbSize);
	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSerial::ScanSlave(IN const INT iID, IN const BYTE iAxisNo, OUT BYTE* pSlaveType, OUT LPSTR lpszSlaveInfo, IN const INT cbSlaveInfo)
{
	m_nLastError = ::FAS_GetSlaveInfoQuick(m_nPortNo, iAxisNo, pSlaveType, lpszSlaveInfo, cbSlaveInfo);
	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSerial::ScanSlaveFromMotiongate(IN const INT iID)
{
	m_nLastError = ::FAS_RescanMotiongateSlave(m_nPortNo);
	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSerial::ChangeMotiongateInfo(IN const INT iID, IN LPCTSTR lpszName, IN const BYTE btMotorCount, IN const BYTE btHomingTimeout)
{
	m_nLastError = ::FAS_ChangeMotiongateInfo(m_nPortNo, CStringA(lpszName), btMotorCount, btHomingTimeout);
	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSerial::RebootMotiongate(IN const INT iID)
{
	m_nLastError = ::FAS_RebootMotionGate(m_nPortNo);
	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSerial::GetMotiongateStatus(IN const INT iID, IN const BYTE iAxisNo, OUT BYTE* pSystemStatus, IN const INT cbSystemStatus)
{
	m_nLastError = ::FAS_GetMotiongateSystemStatus(m_nPortNo, iAxisNo, pSystemStatus, cbSystemStatus);
	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSerial::GetAxisStatus(IN const INT iID, IN const BYTE iAxisNo, OUT DWORD* pdwStatus)
{
	m_nLastError = ::FAS_GetAxisStatus(m_nPortNo, iAxisNo, pdwStatus);
	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSerial::GetAlarmType(IN const INT iID, IN const BYTE iAxisNo, OUT BYTE* pStatus)
{
	m_nLastError = ::FAS_GetAlarmType(m_nPortNo, iAxisNo, pStatus);
	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSerial::ResetMotionGateAlarm(IN const INT iID)
{
	m_nLastError = ::FAS_ResetMotionGateAlarm(m_nPortNo);
	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSerial::SetParameter(IN const INT iID, const INT iAxisNo, const INT nParameterIndex, const LONG lValue)
{
	m_nLastError = ::FAS_SetParameter(m_nPortNo, iAxisNo, nParameterIndex, lValue);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSerial::GetParameter(IN const INT iID, const INT iAxisNo, const INT nParameterIndex, LONG* pValue)
{
	m_nLastError = ::FAS_GetParameter(m_nPortNo, iAxisNo, nParameterIndex, pValue);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSerial::GetFlashParameter(IN const INT iID, const INT iAxisNo, const INT nParameterIndex, LONG* pValue)
{
	m_nLastError = ::FAS_GetROMParameter(m_nPortNo, iAxisNo, nParameterIndex, pValue);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSerial::SetPositionTableItem(IN const INT iID, const INT iAxisNo, const INT nTableIndex, IN LPCVOID pbtItem, IN const INT cbNodeSize)
{
	m_nLastError = ::FAS_PosTableWriteItem(m_nPortNo, iAxisNo, nTableIndex, reinterpret_cast<LPITEM_NODE>(const_cast<LPVOID>(pbtItem)));

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSerial::GetPositionTableItem(IN const INT iID, IN const INT iAxisNo, IN const INT nTableIndex, OUT LPVOID pbtItem, IN const INT cbNodeSize)
{
	m_nLastError = ::FAS_PosTableReadItem(m_nPortNo, iAxisNo, nTableIndex, reinterpret_cast<LPITEM_NODE>(pbtItem));

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSerial::SaveAllParameters(IN const INT iID, IN const INT iAxisNo)
{
	m_nLastError = ::FAS_SaveAllParameters(m_nPortNo, iAxisNo);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSerial::SavePositionTableItems(IN const INT iID, IN const INT iAxisNo)
{
	m_nLastError = ::FAS_PosTableWriteROM(m_nPortNo, iAxisNo);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSerial::LoadPositionTableItems(IN const INT iID, IN const INT iAxisNo)
{
	m_nLastError = ::FAS_PosTableReadROM(m_nPortNo, iAxisNo);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSerial::ChangeSlaveID(IN const INT iID, IN const INT iCurAxisNo, IN const INT iNewAxisNo)
{
	m_nLastError = ::FAS_ChangeSlaveID(m_nPortNo, iCurAxisNo, iNewAxisNo);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSerial::EnableServo(IN const INT iID, IN const BYTE iAxisNo, IN const BOOL bEnable)
{
	m_nLastError = ::FAS_ServoEnable(m_nPortNo, iAxisNo, bEnable);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSerial::MoveStop(IN const INT iID, IN const BYTE iAxisNo)
{
	m_nLastError = ::FAS_MoveStop(m_nPortNo, iAxisNo);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSerial::MoveVelocity(IN const INT iID, IN const BYTE iAxisNo, const INT nSpeed, const BOOL bIncDir)
{
	m_nLastError = ::FAS_MoveVelocity(m_nPortNo, iAxisNo, nSpeed, bIncDir);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSerial::EnableAdministrationMode(IN const INT iID, IN const BOOL bEnable)
{
	m_nLastError = ::FAS_EnterMonitoringMode(m_nPortNo, bEnable);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSerial::ExecutePositionTable(IN const INT iID, IN const BYTE iAxisNo, IN const INT nTableIndex)
{
	m_nLastError = ::FAS_PosTableRunItem(m_nPortNo, iAxisNo, nTableIndex);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSerial::SearchOriginSingle(IN const INT iID, IN const BYTE iAxisNo)
{
	m_nLastError = ::FAS_MoveOriginSingleAxis(m_nPortNo, iAxisNo);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSerial::NotifyBackupDone(IN const INT iID, IN const WORD wElapse)
{
	m_nLastError = ::FAS_NofityBackupDone(m_nPortNo, wElapse);

	return FMM_OK == m_nLastError;
}

const BOOL CInterfaceSerial::GetLifeCounter(IN const INT iID, IN BYTE iAxisNo, OUT DWORD* pdwCounter)
{
	m_nLastError = ::FAS_GetLifeCounter(m_nPortNo, iAxisNo, pdwCounter);

	return FMM_OK == m_nLastError;
}

const BOOL CInterfaceSerial::GetMotorOrganization(IN const INT iID, IN BYTE iAxisNo, OUT DWORD* pdwOrganization)
{
	m_nLastError = ::FAS_GetLifeCounter(m_nPortNo, iAxisNo, pdwOrganization);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSerial::GetMotionGateAlarmStatus(IN const INT iID, OUT BYTE* pStatus)
{
	m_nLastError = ::FAS_MotionGateAlarmStatus(iID, pStatus);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSerial::SetMotionGateAlarmReset(IN const INT iID)
{
	m_nLastError = ::FAS_SetMotionGateAlarmReset(iID);

	return FMM_OK == m_nLastError;
}