#include "pch.h"
#include "CInterfaceSocket.h"

#include "CDataBaseManager.h"

#include "..\FAS_PER_DLL\FAS_EziMotionPlusER.h"
#include "..\Common_Define\Define_ReturnCode.h"

#ifdef DEBUG
#pragma comment (lib, "../Bin/Debug/EziMOTIONPlusER.lib")
#else
#pragma comment (lib, "../Bin/Release/EziMOTIONPlusER.lib")
#endif

#ifdef __FAS_NAMESPACE
using namespace FAS_PER;
#endif

CInterfaceSocket::CInterfaceSocket()
{

}

CInterfaceSocket::~CInterfaceSocket()
{

}

CInterface* CInterfaceSocket::GetInstance()
{
	static CInterfaceSocket _instance;

	return &_instance;
}

const BOOL CInterfaceSocket::Connect(const DWORD dwAddress, const INT nPortNo)
{
	m_nLastError = ::FAS_ConnectTCP(dwAddress, HIBYTE(HIWORD(dwAddress)), LOBYTE(HIWORD(dwAddress)), HIBYTE(LOWORD(dwAddress)), LOBYTE(LOWORD(dwAddress)), 10001);

	return FMM_OK == m_nLastError;
}

const BOOL CInterfaceSocket::Close(const DWORD dwAddress)
{
	m_nLastError = ::FAS_Close(dwAddress);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSocket::GetMotiongateInfo(IN const INT iID, MOTIONGATE_READ_INFO* pDeviceInfo, const INT cbSize)
{
	m_nLastError = ::FAS_GetMotiongateInfo(iID, pDeviceInfo, cbSize);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSocket::ScanSlave(IN const INT iID, IN const BYTE iAxisNo, OUT BYTE* pSlaveType, OUT LPSTR lpszSlaveInfo, IN const INT cbSlaveInfo)
{
	m_nLastError = ::FAS_GetSlaveInfoQuick(iID, IN iAxisNo, pSlaveType, lpszSlaveInfo, cbSlaveInfo);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSocket::ScanSlaveFromMotiongate(IN const INT iID)
{
	m_nLastError = ::FAS_RescanMotiongateSlave(iID);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSocket::ChangeMotiongateInfo(IN const INT iID, IN LPCTSTR lpszName, IN const BYTE btMotorCount, IN const BYTE btHomingTimeout)
{
	m_nLastError = ::FAS_ChangeMotiongateInfo(iID, CStringA(lpszName), btMotorCount, btHomingTimeout);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSocket::RebootMotiongate(IN const INT iID)
{
	m_nLastError = ::FAS_RebootMotionGate(iID);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSocket::GetMotiongateStatus(IN const INT iID, IN const BYTE iAxisNo, OUT BYTE* pSystemStatus, IN const INT cbSystemStatus)
{
	m_nLastError = ::FAS_GetMotiongateSystemStatus(iID, iAxisNo, pSystemStatus, cbSystemStatus);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSocket::GetAxisStatus(IN const INT iID, IN const BYTE iAxisNo, OUT DWORD* pdwStatus)
{
	m_nLastError = ::FAS_GetAxisStatus(iID, iAxisNo, pdwStatus);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSocket::ResetMotionGateAlarm(IN const INT iID)
{
	m_nLastError = ::FAS_ResetMotionGateAlarm(iID);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSocket::SetParameter(IN const INT iID, const INT iAxisNo, const INT nParameterIndex, const LONG lValue)
{
	m_nLastError = ::FAS_SetParameter(iID, iAxisNo, nParameterIndex, lValue);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSocket::GetParameter(IN const INT iID, const INT iAxisNo, const INT nParameterIndex, LONG* pValue)
{
	m_nLastError = ::FAS_GetParameter(iID, iAxisNo, nParameterIndex, pValue);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSocket::GetFlashParameter(IN const INT iID, const INT iAxisNo, const INT nParameterIndex, LONG* pValue)
{
	m_nLastError = ::FAS_GetROMParameter(iID, iAxisNo, nParameterIndex, pValue);
	
	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSocket::SetPositionTableItem(IN const INT iID, const INT iAxisNo, const INT nTableIndex, IN LPCVOID pbtItem, IN const INT cbNodeSize)
{
	//	ItemNode의 구조체 크기가 바뀌면 큰일난다.
	m_nLastError = ::FAS_PosTableWriteItem(iID, iAxisNo, nTableIndex, reinterpret_cast<LPITEM_NODE>(const_cast<LPVOID>(pbtItem)) );

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSocket::GetPositionTableItem(IN const INT iID, IN const INT iAxisNo, IN const INT nTableIndex, OUT LPVOID pbtItem, IN const INT cbNodeSize)
{
	//	ItemNode의 구조체 크기가 바뀌면 큰일난다.
	m_nLastError = ::FAS_PosTableReadItem(iID, iAxisNo, nTableIndex, reinterpret_cast<LPITEM_NODE>(pbtItem));

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSocket::SaveAllParameters(IN const INT iID, IN const INT iAxisNo)
{
	m_nLastError = ::FAS_SaveAllParameters(iID, iAxisNo);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSocket::SavePositionTableItems(IN const INT iID, IN const INT iAxisNo)
{
	m_nLastError = ::FAS_PosTableWriteROM(iID, iAxisNo);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSocket::LoadPositionTableItems(IN const INT iID, IN const INT iAxisNo)
{
	m_nLastError = ::FAS_PosTableReadROM(iID, iAxisNo);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSocket::ChangeSlaveID(IN const INT iID, IN const INT iCurAxisNo, IN const INT iNewAxisNo)
{
	m_nLastError = ::FAS_ChangeSlaveID(iID, iCurAxisNo, iNewAxisNo);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSocket::EnableServo(IN const INT iID, IN const BYTE iAxisNo, IN const BOOL bEnable)
{
	m_nLastError = ::FAS_ServoEnable(iID, iAxisNo, bEnable);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSocket::MoveStop(IN const INT iID, IN const BYTE iAxisNo)
{
	m_nLastError = ::FAS_MoveStop(iID, iAxisNo);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSocket::MoveVelocity(IN const INT iID, IN const BYTE iAxisNo, const INT nSpeed, const BOOL bIncDir)
{
	m_nLastError = ::FAS_MoveVelocity(iID, iAxisNo, nSpeed, bIncDir);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSocket::EnableAdministrationMode(IN const INT iID, IN const BOOL bEnable)
{
	m_nLastError = ::FAS_EnterMonitoringMode(iID, bEnable);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSocket::ExecutePositionTable(IN const INT iID, IN const BYTE iAxisNo, IN const INT nTableIndex)
{
	m_nLastError = ::FAS_PosTableRunItem(iID, iAxisNo, nTableIndex);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSocket::SearchOriginSingle(IN const INT iID, IN const BYTE iAxisNo)
{
	m_nLastError = ::FAS_MoveOriginSingleAxis(iID, iAxisNo);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSocket::NotifyBackupDone(IN const INT iID, IN const WORD wElapse)
{
	m_nLastError = ::FAS_NofityBackupDone(iID, wElapse);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSocket::GetLifeCounter(IN const INT iID, IN const BYTE iAxisNo, OUT DWORD* pdwCounter)
{
	m_nLastError = ::FAS_GetLifeCounter(iID, iAxisNo, pdwCounter);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSocket::GetMotorOrganization(IN const INT iID, IN const BYTE iAxisNo, OUT DWORD* pdwOrganization)
{
	m_nLastError = ::FAS_GetMotorOrganization(iID, iAxisNo, pdwOrganization);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSocket::GetMotionGateAlarmStatus(IN const INT iID, OUT BYTE* pStatus)
{
	m_nLastError = ::FAS_GetMotionGateAlarmStatus(iID, pStatus);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSocket::SetMotionGateAlarmReset(IN const INT iID)
{
	m_nLastError = ::FAS_SetMotionGateAlarmReset(iID);

	return FMM_OK == m_nLastError;
}

const BOOL	CInterfaceSocket::GetAlarmType(IN const INT iID, IN const BYTE iAxisNo, OUT BYTE* pStatus)
{
	m_nLastError = ::FAS_GetAlarmType(iID, iAxisNo, pStatus);

	return FMM_OK == m_nLastError;
}
