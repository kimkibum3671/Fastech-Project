#include "pch.h"
#include "CDeviceMotionGate.h"

#include "CDeviceList.h"

#include "CDataBaseManager.h"

#include "Define_Message.h"

#include "resource.h"

#include "CInterfaceManager.h"

#include <stdio.h>
#include <time.h>

#define TRY_OVER_COUNT			5
#define TIMER_SERVO_ON			1000
#define TIMER_SERVO_OFF			1000	

#define REBOOT_CONNECTION_COUNT	30

#define SKIP_ADMINISTRATION_MODE

#include "..\Common_Define\Define_ReturnCode.h"

CDeviceMotionGate::CDeviceMotionGate()
{
	Initialize();
}

CDeviceMotionGate::~CDeviceMotionGate()
{
	m_bInformRequest = FALSE;

	if (m_stDatabaseDeviceInfo.bmpImage.GetSafeHandle())
	{
		m_stDatabaseDeviceInfo.bmpImage.DeleteObject();
	}

	if (m_pPLCReqInform != NULL)
	{
		delete m_pPLCReqInform;
	}
}

void CDeviceMotionGate::SendLogMessage(const INT nLevel, LPCTSTR lpszFormat, ...)
{
	if (nLevel > m_nLogLevel)
	{
		return;
	}

	CWnd* pWnd = AfxGetMainWnd();
	if (NULL == pWnd)
	{
		return;
	}

	CString strMessage;
	va_list vl;
	va_start(vl, lpszFormat);
	strMessage.FormatV(lpszFormat, vl);
	va_end(vl);

	CMainFrame::ST_OPERAION_MESSAGE msg = { 0, };
	msg.pDevice = this;
	msg.nLevel = nLevel;
	msg.nMsgID = msg_function_log;

	::SendMessage(pWnd->m_hWnd, CMainFrame::UM_OPERATION_LOG, reinterpret_cast<WPARAM>(&msg), reinterpret_cast<WPARAM>(static_cast<LPCTSTR>(strMessage)));
}

void CDeviceMotionGate::SendLogMessage(const INT nLevel, const INT nMsgID, const LPARAM lParam)
{
	BuildSimpleStatus(nMsgID, lParam);
	BuildDetailProgress(nMsgID, lParam);
	BuildDetailStatus(nMsgID, lParam);

#if defined(LOG_START)
	SaveLogStatus(nMsgID, m_strDetailStatus);
#endif

	if (nLevel > m_nLogLevel)
	{
		return;
	}

	CWnd* pWnd = AfxGetMainWnd();
	if (NULL == pWnd)
	{
		return;
	}

	CMainFrame::ST_OPERAION_MESSAGE msg = { 0, };
	msg.pDevice = this;
	msg.nLevel = nLevel;
	msg.nMsgID = nMsgID;
	m_Msg.bEnableLog = FALSE;
	
	if ((m_Msg.nLevel != msg.nLevel) || (m_Msg.nMsgID != msg.nMsgID))
	{ 
	/*
		Message Filtering!!
		UM_OPERATION_LOG 메세지는 StatusBar의 현재 장치 상태를 현시하기 위한 메세지이다.
		따라서 이또한 지속적으로 StatusBar에 현시해줘야하나 Logging기능은 막아야한다.
		결과적으로 bEnableLog변수를 통하여 로깅에 대한 명령값(TRUE)이 저장하여 메세지를 보낸다,
	*/
		m_Msg.pDevice = msg.pDevice;
		m_Msg.nLevel = msg.nLevel;
		m_Msg.nMsgID = msg.nMsgID;
		m_Msg.bEnableLog = TRUE;
	}

	::SendMessage(pWnd->m_hWnd, CMainFrame::UM_OPERATION_LOG, reinterpret_cast<WPARAM>(&m_Msg), lParam);
}

void CDeviceMotionGate::Initialize(const BOOL bFirstLaunch)
{ // 초기 연결시(bFirstLaunch == TRUE) & Reboot 진행시, 설정 변경시 (bFirstLaunch == FALSE)
	CDataBaseConfig* pConfig = CDataBaseConfig::GetInstance();
	pConfig->LoadNoImage(&m_stDatabaseDeviceInfo.bmpImage);
	
	if (bFirstLaunch)
	{
		m_stDatabaseDeviceInfo.strName.Empty();
		m_stDatabaseDeviceInfo.dwAddress = 0x00000000;
		m_stDatabaseDeviceInfo.nMotorCount = -1; // 기존.. 1
		m_stDatabaseDeviceInfo.nHomingTimeout = 60 * 1000;	//	60 sec
		m_stDatabaseDeviceInfo.nNominalID = -1;
		m_stDatabaseDeviceInfo.nRestoredID = -1;
		m_stDatabaseDeviceInfo.nDeviceID = -1;

		m_timeOperationTimeStamp = CTime::GetCurrentTime();

		m_pThread = NULL;

		m_nOpCode = opCode_stop;

		SetStatusConnectionStop();
		_tcscpy_s(m_strDetailStatus, MAX_PATH, _T("Stopped"));

		m_bAutoRestoration = TRUE;

		m_bRun = FALSE;

		m_stConnectedDeviceInfo.strInformation.Empty();
		m_stConnectedDeviceInfo.strName.Empty();
		m_stConnectedDeviceInfo.nMotorCount = 0;
		m_stConnectedDeviceInfo.nHomingTimeout = 0;
		m_stConnectedDeviceInfo.dwWarningLifeCount = pConfig->GetLifeCountStandardValue();
		m_stConnectedDeviceInfo.dwWarningLifeTickCount = pConfig->GetLifeCountCycleValue();
	}

	m_bEditMode = FALSE;

	m_wPrevAxisConnection =
		m_wScanAxisConnection =
		m_wCurAxisConnection = 0;

	m_stCommand.cmdGate.dwFlag = 0;
	for (INT i = 0; i <= 8; i++)
	{
		m_stCommand.cmdMotor[i].command.dwFlag = 0;
		m_stCommand.cmdMotor[i].timer.servoOnDelay = 0;
		m_stCommand.cmdMotor[i].timer.servoOffDelay = 0;
		m_stCommand.cmdMotor[i].timer.homeFail = 0;
	}

	m_stSystemStatus.btStartMotorConnectionStatus = 0;
	m_stSystemStatus.btCurrentMotorConnectionStatus = 0;
	m_stSystemStatus.wStatus.wValue = 0;
	for (INT i = 0; i <= 8; i++)
	{
		m_stSystemStatus.stMotorStatus[i].wPosInteger = 0;
		m_stSystemStatus.stMotorStatus[i].wPosDecimal = 0;
		m_stSystemStatus.stMotorStatus[i].wStatus.wValue = 0;
		m_stSystemStatus.stMotorStatus[i].wPositionTableNo = 0;
		m_stSystemStatus.stMotorStatus[i].btErrorCode = 0;
		m_stSystemStatus.dwDetailStatus[i].dwValue = 0;
		m_stSystemStatus.opStatus[i].wValue = 0;
		m_stSystemStatus.dwLifeCount[i] = 0;
		m_stSystemStatus.bMotorLifeDetected[i] = false;
		m_stSystemStatus.bMotorLifeCycleDetected[i] = false;
	}

	m_bAutoHoming = FALSE;
	m_bBackupReservation = FALSE;
	m_bAutoBackupError = TRUE;

	m_nLogLevel = msgNormal;

	m_bConnected = FALSE;

	m_nDetailProgressPos = 0;
	m_nDetailProgressMin = 0;
	m_nDetailProgressMax = 100;

	m_bBackupReservation = FALSE;
	m_nPauseBackupCounter = 0;

	m_bDeleted = false;

	if (1)
	{
		COleDateTime now = COleDateTime::GetCurrentTime();
		for (INT nIndex = 0; nIndex < sts_max; nIndex++)
		{
			m_dateTimeStatus[nIndex] = now;
			m_dwStatusFlag[nIndex] = 0;
		}

		SetStatusConnectionStop();
	}

	m_bCheckInform = true;
	m_bInformRequest = FALSE;
	m_bProgressTimer = FALSE;
	m_bEndTimeCheck = FALSE;
	m_dwProgressEndTime = 0;
	m_PrevLog = m_CurLog = { NULL, };
	m_opLogInform.dwAddress = 0;

	m_bLogChanged = FALSE;
	m_strCmdTest = _T("");
	m_bLifeCountAlarm = m_bLifeCountPeriodAlarm = FALSE;
	m_bMotorChangeDetected = FALSE;
	m_bMotorError[9] = { FALSE, };
	m_bKeepStopAutoBackup = FALSE;
	m_bHomingStop = FALSE;
	m_bAllalarm = FALSE;
	m_bActiveHomingAll = FALSE;
	m_bSetHomingClose = FALSE;
	m_bSendDoneMessage = FALSE;
	m_wCurAxisOrganization = m_wOriginAxisOrganization = 0;
}


void CDeviceMotionGate::ResetOperationTimeStamp(CTime time)
{
	m_timeOperationTimeStamp = time;
}


CTime& CDeviceMotionGate::GetOpertionTimeStamp()
{
	
	return m_timeOperationTimeStamp;
}


UINT CDeviceMotionGate::PollingThread(LPVOID pParam)
{
	ULONGLONG dwTickCount = 0;
	ULONGLONG dwNextDelay = 0;

	CDeviceList* pDeviceList = CDeviceList::GetInstance();
	CDeviceMotionGate* pParent = reinterpret_cast<CDeviceMotionGate*>(pParam);

	pDeviceList->IncreaseThreadCount();
	

	INT nMotorNo = 0;
	pParent->SendLogMessage(msgNormal, msg_monitoring_start);

	pParent->SetOpCode(opCode_stop);
	
	while (pDeviceList->IsMonitoringEnabled(pParent->GetAddress()))
	{
		Sleep(1);

		if (dwNextDelay >= (::GetTickCount64() - dwTickCount))
		{
			continue;
		}
		
		dwNextDelay = pParent->DoNextStateMachine();
		dwTickCount = ::GetTickCount64();
	}
	



	pParent->SetOpCode(opCode_stop);

	pParent->SendLogMessage(msgNormal, msg_monitoring_stop);

	pParent->m_pThread = NULL;

	CString strAddress;
	pParent->GetAddress(strAddress);
	pDeviceList->DeleteObject(pParent);

	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (CDataBaseConfig::interface_Serial == pInterface->GetInterfaceType())
	{
		pInterface->Close(pParent->m_stDatabaseDeviceInfo.dwAddress);
	}
	

	return 1;
}


const BOOL	CDeviceMotionGate::StartThread()
{
	if (m_bRun && m_pThread)
	{
		DWORD dwExitCode = 0;
		if (TRUE == ::GetExitCodeThread(m_pThread->m_hThread, &dwExitCode) && STILL_ACTIVE == dwExitCode)
		{
			return TRUE;
		}
	}

	m_bRun = TRUE;
	m_pThread = AfxBeginThread(&CDeviceMotionGate::PollingThread, this);

	return TRUE;
}


const BOOL CDeviceMotionGate::CheckRunState()
{
	BOOL bResult = FALSE;;
	if (m_bRun)
	{
		bResult = TRUE;
	}
	return bResult;
}


void CDeviceMotionGate::StopThread()
{
	m_bRun = FALSE;
}

const BOOL CDeviceMotionGate::IsRun()
{
	return CDeviceList::GetInstance()->IsMonitoringEnabled() && m_bRun;;
}

const BOOL	CDeviceMotionGate::IsRunning()
{
	if (NULL == m_pThread)
	{
		return FALSE;
	}

	DWORD dwExitCode = 0;
	if (FALSE == ::GetExitCodeThread(m_pThread->m_hThread, &dwExitCode))
	{
		return FALSE;
	}

	if (state_stop == m_stateMachine)
	{
		return FALSE;
	}

	return STILL_ACTIVE == dwExitCode;
}


void CDeviceMotionGate::SetAddress(const DWORD dwAddress)
{
	if (m_stDatabaseDeviceInfo.dwAddress)
	{
		if (m_stDatabaseDeviceInfo.dwAddress != dwAddress &&
			CDeviceList::GetInstance()->ChangeAddress(m_stDatabaseDeviceInfo.dwAddress, dwAddress))
		{
			m_stDatabaseDeviceInfo.dwAddress = dwAddress;
		}
	}
	else
	{
		m_stDatabaseDeviceInfo.dwAddress = dwAddress;
	}
}

const DWORD	CDeviceMotionGate::GetAddress()
{
	return m_stDatabaseDeviceInfo.dwAddress;
}

void CDeviceMotionGate::GetAddress(CString& strAddress)
{
	if (0xffffffff == m_stDatabaseDeviceInfo.dwAddress)
	{
		strAddress.Format(_T("COM%d"), CDataBaseConfig::GetInstance()->GetSerialPort());
	}
	else
	{
		strAddress.Format(_T("%3d.%3d.%3d.%3d"),
			HIBYTE(HIWORD(m_stDatabaseDeviceInfo.dwAddress)),
			LOBYTE(HIWORD(m_stDatabaseDeviceInfo.dwAddress)),
			HIBYTE(LOWORD(m_stDatabaseDeviceInfo.dwAddress)),
			LOBYTE(LOWORD(m_stDatabaseDeviceInfo.dwAddress)));
	}	
}

void CDeviceMotionGate::SetEditAddress(const DWORD dwAddress)
{
	m_stDatabaseEditDeviceInfo.dwAddress = dwAddress;
}

const DWORD	CDeviceMotionGate::GetEditAddress()
{
	return m_stDatabaseEditDeviceInfo.dwAddress;
}

void CDeviceMotionGate::SetEditDeviceID(INT nID)
{
	m_stDatabaseEditDeviceInfo.nDeviceID = nID; //여기서 부터 이어서 보기!!
}


const INT CDeviceMotionGate::GetEditDeviceID()
{
	return m_stDatabaseEditDeviceInfo.nDeviceID;
}




void CDeviceMotionGate::GetEditAddress(CString& strAddress)
{
	strAddress.Format(_T("%3d.%3d.%3d.%3d"),
		HIBYTE(HIWORD(m_stDatabaseEditDeviceInfo.dwAddress)),
		LOBYTE(HIWORD(m_stDatabaseEditDeviceInfo.dwAddress)),
		HIBYTE(LOWORD(m_stDatabaseEditDeviceInfo.dwAddress)),
		LOBYTE(LOWORD(m_stDatabaseEditDeviceInfo.dwAddress)));
}

const BOOL CDeviceMotionGate::SetSystemName(LPCTSTR lpszName)
{
	if (CString(lpszName) != m_stConnectedDeviceInfo.strName)
	{
		/*if (m_bAutoDatabaseUpdate)
		{
			CDataBaseProject::GetInstance()->ChangeRegisterName(m_stDatabaseDeviceInfo.strName, lpszName);
		}*/

		m_stConnectedDeviceInfo.strName = lpszName;
	}

	return TRUE;
}


LPCTSTR CDeviceMotionGate::GetSystemName()
{
	return m_stConnectedDeviceInfo.strName;
}


const BOOL CDeviceMotionGate::SetEditSystemName(LPCTSTR lpszName)
{
	m_stDatabaseEditDeviceInfo.strName = lpszName;

	return TRUE;
}


LPCTSTR CDeviceMotionGate::GetEditSystemName()
{
	return m_stDatabaseEditDeviceInfo.strName;
}

LPCTSTR CDeviceMotionGate::GetDatabaseSystemName()
{
	return m_stDatabaseDeviceInfo.strName;
}

void CDeviceMotionGate::SetNominalBackupID(const INT nBackupID)
{
	if (nBackupID != m_stDatabaseDeviceInfo.nNominalID)
	{
		if (m_bAutoDatabaseUpdate)
		{

		}

		m_stDatabaseDeviceInfo.nNominalID = nBackupID;
	}
}


const INT CDeviceMotionGate::GetNominalBackupID()
{
	return m_stDatabaseDeviceInfo.nNominalID;
}


void CDeviceMotionGate::SetRestoredBackupID(const INT nBackupID)
{
	if (nBackupID != m_stDatabaseDeviceInfo.nRestoredID)
	{
		if (m_bAutoDatabaseUpdate)
		{

		}

		m_stDatabaseDeviceInfo.nRestoredID = nBackupID;
	}
}


const INT CDeviceMotionGate::GetRestoredBackupID()
{
	return m_stDatabaseDeviceInfo.nRestoredID;
}


CBitmap* CDeviceMotionGate::SetSystemImage(HBITMAP hBitmap)
{
	if (m_stDatabaseDeviceInfo.bmpImage.GetSafeHandle())
	{
		m_stDatabaseDeviceInfo.bmpImage.Detach();
	}

	m_stDatabaseDeviceInfo.bmpImage.Attach(::CopyImage(hBitmap, IMAGE_BITMAP, 0, 0, 0));

	return &m_stDatabaseDeviceInfo.bmpImage;
}


CBitmap* CDeviceMotionGate::GetSystemImage()
{
	return &m_stDatabaseDeviceInfo.bmpImage;
}


//CBitmap	CDeviceMotionGate::GetSystemDatabaseImage()
//{
//	//CBitmap bitmap = m_stDatabaseDeviceInfo.bmpImage;
//	CBitmap bitmap = 0;
//	return bitmap;
//}


CBitmap* CDeviceMotionGate::GetSystemEditImage()
{
	// TODO: 여기에 구현 코드 추가.
	return &m_stDatabaseEditDeviceInfo.bmpImage;
}


CBitmap* CDeviceMotionGate::SetEditSystemImage(HBITMAP hBitmap)
{
	if (m_stDatabaseEditDeviceInfo.bmpImage.GetSafeHandle())
	{
		m_stDatabaseEditDeviceInfo.bmpImage.Detach();
	}

	m_stDatabaseEditDeviceInfo.bmpImage.Attach(::CopyImage(hBitmap, IMAGE_BITMAP, 0, 0, 0));

	return &m_stDatabaseEditDeviceInfo.bmpImage;
}

CBitmap* CDeviceMotionGate::GetEditSystemImage()
{
	return &m_stDatabaseEditDeviceInfo.bmpImage;
}

void CDeviceMotionGate::SetMotorCount(const INT nCount)
{
	if (m_stConnectedDeviceInfo.nMotorCount != nCount)
	{
		/*if (m_bAutoDatabaseUpdate)
		{

		}*/

		m_stConnectedDeviceInfo.nMotorCount = nCount;
	}
}


const INT CDeviceMotionGate::GetMotorCount()
{
	return m_stConnectedDeviceInfo.nMotorCount;
}

const INT CDeviceMotionGate::GetDatabaseMotorCount()
{
	return m_stDatabaseDeviceInfo.nMotorCount;
}

const INT CDeviceMotionGate::GetDatabaseDeviceID()
{
	return m_stDatabaseDeviceInfo.nDeviceID;
}

void CDeviceMotionGate::SetEditMotorCount(const INT nCount)
{
	m_stDatabaseEditDeviceInfo.nMotorCount = nCount;
}

const INT CDeviceMotionGate::GetEditMotorCount()
{
	return m_stDatabaseEditDeviceInfo.nMotorCount;
}

const DWORD	CDeviceMotionGate::GetStatus()
{
	return m_stateMachine;
}


void CDeviceMotionGate::GetSimpleStatus(CString& strStatus)
{
	CString strTemp;
	GetLastSimpleStatus(strTemp);
	if (strTemp == _T("System OK"))
	{
		INT a = 0;
	}

	strTemp.Remove('.');
	
	strStatus += strTemp;//GetSimpleStatus();
}

void CDeviceMotionGate::SetOpCode(const UINT nOpCode)
{
	m_nOpCode = nOpCode;

	switch (m_nOpCode)
	{
	case opCode_stop:
	{
		//		SetSimpleStatus(_T("Stopped"));
		m_stateMachine = state_stop;
	}	break;
	}
}

const UINT CDeviceMotionGate::GetOpCode()
{
	return m_nOpCode;
}

void CDeviceMotionGate::StopSequence()
{
	m_bAutoHoming = FALSE;

	m_stCommand.cmdGate.bitStopExecution = FALSE;

	switch (m_stateMachine)
	{
	case state_backup:
	{
		if (m_stCommand.cmdGate.bitBackup) SendLogMessage(msgWarning, msg_backup_stop);
		else SendLogMessage(msgWarning, msg_compare_stop);

		m_stateMachine = state_motion_gate_get_system_status_all;
		m_nOpCode = opCode_read_motiongate_status;
	}	break;

	case state_restoration:
	{
		SendLogMessage(msgWarning, msg_restoration_stop);
		m_nOpCode = opCode_exit_monitor_mode;
	}	break;
	case state_factory_reset:
	{
		SendLogMessage(msgWarning, msg_factory_reset_stop);
		m_nOpCode = opCode_exit_monitor_mode;
	}	break;

	case state_assign_motor_id:
	{
		SendLogMessage(msgWarning, msg_initialize_stop);
		m_nOpCode = opCode_exit_monitor_mode;
	}	break;

	case state_home_all:
	{
		SendLogMessage(msgWarning, msg_homing_all_stop);
		m_nOpCode = opCode_exit_monitor_mode;
	}	break;
	}

	CheckAutoBackupSchedule();

	m_nMotorIndex = FindNextMotorIndex();
}

const INT CDeviceMotionGate::DoNextStateMachine()
{
	CDeviceList* pDeviceList = CDeviceList::GetInstance();

	INT nErrorMsgID = -1;

	INT nDelay = 1;

	CheckAutoBackupEnable();
	
	DWORD dwAddress = GetAddress();
	
	switch (m_stateMachine)
	{
	case state_stop:
	{
		if (pDeviceList->IsMonitoringEnabled(GetAddress()))
		{
			m_stateMachine = state_connect;
			m_nOpCode = opCode_connect;
		}
	}	break;

	case state_connect:
	{
		nDelay = OpConnectDevice();

	}	break;

	case state_reboot:
	{
		nDelay = OpRebootDevice();
	}	break;

	case state_motion_gate_get_system_status_all:
	{
		nDelay = OpReadMotiongateStatus();
	}	break;

	case state_motion_gate_command:
	{
		nDelay = OpWriteUserRequest();
	}	break;

	case state_backup:
	{
		nDelay = OpReadParameters();
		//if (1 == nDelay) SendLogMessage(msgWarning, msg_backup_stop);

		if (-1 == nDelay)
		{
			nErrorMsgID = IDS_FORMAT_BACKUP_NG;
			if (m_stCommand.cmdGate.bitBackup) SendLogMessage(msgError, msg_backup_fail);
			else SendLogMessage(msgError, msg_compare_fail);
		}
	}	break;

	case state_restoration:
	{
		nDelay = OpRestoreParameter();

		//if (1 == nDelay) SendLogMessage(msgWarning, msg_restoration_stop);

		if (-1 == nDelay)
		{
			nErrorMsgID = IDS_FORMAT_RESTORATION_NG;
			SendLogMessage(msgError, msg_restoration_fail);
		}
	}	break;

	case state_factory_reset:
	{
		nDelay = OpFactoryReset();
		//if (1 == nDelay) SendLogMessage(msgWarning, msg_factory_reset_stop);

		if (-1 == nDelay)
		{
			nErrorMsgID = IDS_FORMAT_FACTORY_RESET_NG;
			SendLogMessage(msgError, msg_factory_reset_fail);
		}
	}	break;

	case state_assign_motor_id:
	{
		nDelay = OpAssignMotorID();
		
		//if (1 == nDelay) SendLogMessage(msgWarning, msg_initialize_stop);

		if (-1 == nDelay)
		{
			nErrorMsgID = IDS_FORMAT_ASSIGNMENT_ID_NG;
			SendLogMessage(msgError, msg_initialize_fail);
		}
	}	break;

	case state_home_all:
	{
		//nDelay = OpHomingAll();
		nDelay = OpHomingMotion(m_nMotorIndex);
		if (-1 == nDelay)
		{
			nErrorMsgID = IDS_FORMAT_HOME_ALL_NG;
			SendLogMessage(msgError, msg_homing_all_fail);
		}
	}	break;

	case state_change_information:
	{
		nDelay = OpChangeDeviceInformation();
		if (-1 == nDelay)
		{
			nErrorMsgID = IDS_FAIL_CHANGE_NAME;
			// nErrorMsgID = IDS_FORMAT_CHANGE_SYSTEM_INFO_NG; 기존 주석...
			SendLogMessage(msgError, msg_motiongate_change_system_fail);
		}
	}	break;

	default:
	{
		ASSERT(0);
	}	break;
	}

	if (0 < nErrorMsgID)
	{
		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			//	웬만한 함수 호출은 들어오면 안되지만 일부 기능의 Time-out같은 경우는 들어올 수 있다.
			nDelay = 1;
		}	break;

		case FMC_NO_RESPONSE:
		{
			//	처음부터 재연결을 시도한다.
			nDelay = 1000;
			SetConnectionStatus(FALSE);
		}	break;

		default:
		{
			//	연결은 되어있으므로 초기화 단계부터 시작한다.
			nDelay = 1000;
			m_stateMachine = state_connect;
			m_nOpCode = opCode_read_motiongate_info;
		}	break;
		}
	}

	/*
	for (INT nStatus = STS_LOG_NORMAL; nStatus < STS_LOG_MAX; nStatus++)
	{
		switch (nStatus)
		{
		case STS_LOG_NORMAL:	m_CurLog.strNormal = GetCurrentSystemStatus(nStatus); break;
		case STS_LOG_WARNING:	m_CurLog.strWarning = GetCurrentSystemStatus(nStatus); break;
		case STS_LOG_ERROR:	m_CurLog.strError = GetCurrentSystemStatus(nStatus); break;
		default:	break;
		}
	}
	*/
	

	return nDelay;
}


const INT CDeviceMotionGate::OpConnectDevice()
{
	INT nResult = 1000;
	INT nCurMotorCnt = 0;
	
	switch (m_nOpCode)
	{
	case opCode_connect:
	case opCode_disconenct:
	case opCode_no_response:
	case opCode_MotiongateBusy:
	{
		SendLogMessage(msgSequence, msg_motiongate_connecting);

		m_nLastFasResult = DoCmdConnect();

		if (FMM_OK != m_nLastFasResult)
		{
			INT i = 0;
		}

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			CString str;
			GetAddress(str);
			Sleep(1);

			nResult = 1;

			m_nOpCode = opCode_read_motiongate_info;

			this->Initialize(FALSE);

			//	IP Connector와 연결되어있는지 확인하는 과정
			switch (CDataBaseConfig::GetInstance()->GetInterface())
			{
			case CDataBaseConfig::interface_Serial:
			{

			}	break;

			default:
			{
				SetConnectionStatus(TRUE);
				SendLogMessage(msgInternal, msg_motiongate_connected);
			}	break;
			}
		}	break;

		case FMC_NO_RESPONSE:
		{
			m_nOpCode = opCode_no_response;
			SendLogMessage(msgError, msg_motiongate_no_response);

			DoCmdClose();
		}	break;

		case FMC_REFUSED:
		case FMC_SOCKET_ERROR:
		case FMC_NOT_COMPLPETE:
		{
			m_nOpCode = opCode_disconenct;
			SendLogMessage(msgError, msg_motiongate_disconnected);

			DoCmdClose();
		}	break;

		case FMM_INVALID_PORT:
		{
			//	고민이 필요하겠네?
		}
		case FMM_INVALID_SLAVE_NUM:
		case FMP_PACKETERROR:
		{
			m_nOpCode = opCode_disconenct;

			DoCmdClose();
		}	break;

		case FMM_MOTION_GATE_BUSY:
		{
			m_nOpCode = opCode_MotiongateBusy;
			SendLogMessage(msgError, msg_motiongate_busy);

			DoCmdClose();
		}	break;

		default:
		{
			ASSERT(0);
		}	break;
		}
	}	break;

	case opCode_read_motiongate_info:
	{
		m_nLastFasResult = DoCmdReadMotiongateInfo();

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			m_nOpCode = opCode_scan_device;

			m_wScanAxisConnection = 0;
			m_nMotorIndex = 0;

			nResult = 1;

			switch (CDataBaseConfig::GetInstance()->GetInterface())
			{
			case CDataBaseConfig::interface_Serial:
			{
				//	TCP/IP에서는 이미 연결되어있으니까..
				SetConnectionStatus(TRUE);
				SendLogMessage(msgInternal, msg_motiongate_connected);
			}	break;
			}

		}	break;

		case FMC_NO_RESPONSE:
		{
			switch (CDataBaseConfig::GetInstance()->GetInterface())
			{
			case CDataBaseConfig::interface_Serial:
			{
				SendLogMessage(msgError, msg_motiongate_no_response);
			}	break;
			}

			nResult = 1000;
		}	break;

		default:
		{
			DoCmdClose();

			SetConnectionStatus(FALSE);
			nResult = 1000;
		}	break;
		}
	}	break;

	case opCode_scan_device:
	{
		SendLogMessage(msgSequence, msg_scan_slave_progress);

		m_nLastFasResult = DoCmdGetSlaveInfoQuick();

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{ // Motor와의 Connection이 이루어진다면 모터 인덱스를 쉬프트 시킴.
			m_wScanAxisConnection |= (0x1 << m_nMotorIndex);

			nResult = 1;

			m_nOpCode = opCode_read_life_counter;
		}	break;

		case FMC_TIMEOUT_ERROR:
		case FMC_NO_RESPONSE:
		{
			nResult = 1;
			m_nOpCode = opCode_scan_next_device_ng;

		}	break;

		default:
		{
			SetConnectionStatus(FALSE);

			nResult = 1000;
		}	break;
		}

		
	}	break;

	case opCode_read_life_counter:
	{
		SendLogMessage(msgSequence, msg_scan_slave_progress);

		m_nLastFasResult = DoCmdGetLifeCounter(m_nMotorIndex);

		switch (m_nLastFasResult) 
		{
		case FMM_OK:
		{
			nResult = 1;

			m_nOpCode = opCode_scan_next_device_ok;

		}	break;

		case FMC_TIMEOUT_ERROR:
		case FMC_NO_RESPONSE:
		{
			m_nOpCode = opCode_scan_next_device_ng;
		}	break;
		};
	}	break;

	case opCode_scan_next_device_ok:
	case opCode_scan_next_device_ng:
	{
		nResult = ( opCode_scan_next_device_ok == m_nOpCode ) ? 1 : 100;

		m_nMotorIndex++;

		if (8 < m_nMotorIndex)
		{
			CheckAutoBackupSchedule();

			m_stateMachine = state_motion_gate_get_system_status_all;
			m_nOpCode = opCode_read_motiongate_status;

			m_wPrevAxisConnection = m_wCurAxisConnection;
			m_wCurAxisConnection = m_wScanAxisConnection;

			SendLogMessage(msgSequence, msg_reset_detail_progress);

			CWnd* pWnd = AfxGetMainWnd();
			//::SendMessage(pWnd->m_hWnd, CMainFrame::UM_SHOW_DEIVCE_CONFIG, m_stDatabaseDeviceInfo.dwAddress, 0);
			::PostMessage(pWnd->m_hWnd, CMainFrame::UM_SHOW_DEIVCE_CONFIG, m_stDatabaseDeviceInfo.dwAddress, 0);
			
			m_nMotorIndex = FindNextMotorIndex();
		}
		else
		{
			m_nOpCode = opCode_scan_device;
		}
	}	break;
	}
	return nResult;
}

const INT CDeviceMotionGate::OpRebootDevice()
{
	INT nResult = 100;

	switch (m_nOpCode)
	{
	case opCode_enter_monitor_mode:
	{
#ifndef SKIP_ADMINISTRATION_MODE
		m_nLastFasResult = DoCmdEnableAdministrationMode(TRUE);

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			m_nOpCode = opCode_check_monitor_mode_on;
		}	break;

		default:
		{
			return -1;
		}	break;
		}
#else
		m_nOpCode = opCode_reboot_motiongate;
#endif
	}	break;	//	end of 'case opCode_enter_monitor_mode:'

	case opCode_check_monitor_mode_on:
	{
#ifndef SKIP_ADMINISTRATION_MODE
		m_nLastFasResult = DoCmdGetMotiongateSystemStatus(m_nMotorIndex);
		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			if (m_stSystemStatus.wStatus.bitMonitorOn)
			{				
				m_nRetryCount = 0;

				m_nOpCode = opCode_reboot_motiongate;
			}
			else
			{
				m_nRetryCount++;
				if (TRY_OVER_COUNT <= m_nRetryCount)
				{
					/*SetDetailStatus(_T("Failed to enter adminitration mode"), m_nProgressCounter);*/
					return -1;
				}

				m_nOpCode = opCode_enter_monitor_mode;
			}
		}	break;

		default:
		{
			return -1;
		}	break;
		}
#else
		m_nOpCode = opCode_reboot_motiongate;
#endif
	}	break;	// end of 'case opCode_check_monitor_mode_on:'

	case opCode_reboot_motiongate:
	{
		m_nLastFasResult = DoCmdRebootMotiongate();
		m_nDetailProgressPos = 0;

		if (FMM_OK == m_nLastFasResult)
		{
			//AfxMessageBox(_T("[Test] DoCmdRebootMotiongate - Reboot OK."));
			m_nOpCode = opCode_read_motiongate_info;
			nResult = 1000;
		}	break;

		m_nLastFasResult = DoCmdRebootMotiongate();

		switch (1)
		{
		case FMM_OK:
		{
			//AfxMessageBox(_T("[Test] DoCmdRebootMotiongate - Reboot OK."));
			Initialize(FALSE);

			m_nRetryCount = 0;

			m_nOpCode = opCode_read_motiongate_info;
			nResult = 1000;
		}	break;

		default:
		{
			return -1;
		}	break;
		}
	}

	case opCode_read_motiongate_info:
	{
		m_nLastFasResult = DoCmdReadMotiongateInfo();
		m_nDetailProgressPos++;

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			//AfxMessageBox(_T("[Test Reboot] DoCmdReadMotiongateInfo - ReadMotionGate OK."));
			SendLogMessage(msgInternal, msg_scan_slave_start);
			m_stateMachine = state_connect;
			m_nOpCode = opCode_scan_device;

			nResult = 100;

			m_wScanAxisConnection = 0;
			m_nMotorIndex = 0;
		}	break;

		case FMC_REFUSED:	//	IP Converter의 경우 Refuse되는 경우가 발생
		case FMC_TIMEOUT_ERROR:
		case FMC_NO_RESPONSE:
		{
			m_nRetryCount++;

			if (REBOOT_CONNECTION_COUNT <= m_nRetryCount)
			{
				m_stateMachine = state_connect;
				m_nOpCode = opCode_no_response;
			}

		}	break;

		default:
		{
			return -1;
		}	break;
		}
	}	break;
	}

	return nResult;
}

const INT CDeviceMotionGate::OpReadMotiongateStatus()
{
	INT nResult = 50;

	switch (m_nOpCode)
	{
	case opCode_read_motiongate_status:
	{
		switch (DoCmdGetMotorOrganization())
		{
		case FMM_OK:
		{
		
		} break;

		default:
		{
			nResult = 1000;
		}	break;
		}
		
	

		switch (DoCmdGetMotiongateSystemStatus(m_nMotorIndex))
		{
		case FMM_OK:
		{
			m_wPrevAxisConnection = m_wCurAxisConnection;
			m_wCurAxisConnection = (m_wCurAxisConnection & 0xfe01) | (static_cast<WORD>(m_stSystemStatus.btCurrentMotorConnectionStatus << 1));

 			m_nOpCode = opCode_read_life_counter;
		}	break;

		default:
		{
			nResult = 1000;
		}	break;
		}
	}	break;

	case opCode_read_life_counter:
	{
		//	이전 값
		m_stateMachine = state_motion_gate_command;

		const DWORD dwPrevCnt = m_stSystemStatus.dwLifeCount[m_nMotorIndex];
		const BOOL  bPrevAlarm = m_stSystemStatus.bMotorLifeDetected[m_nMotorIndex];
		const DWORD dwStandard = m_stConnectedDeviceInfo.dwWarningLifeCount;
		const DWORD dwCycle = m_stConnectedDeviceInfo.dwWarningLifeTickCount;
		
		switch (DoCmdGetLifeCounter(m_nMotorIndex))
		{
		case FMM_OK:
		{
			DWORD dwCurCnt = m_stSystemStatus.dwLifeCount[m_nMotorIndex];
			DWORD dwOffset = dwCurCnt - dwStandard;
			DWORD dwAlarmCount = dwOffset / dwCycle;
			DWORD dwPrevCycle = dwPrevCnt / dwCycle;
			DWORD dwCurCycle = dwCurCnt / dwCycle;
			
		
			//	수량에 변화가 발생
			if (dwStandard <= dwCurCnt)
			{
				SetPTLifeAlarmDetected(m_nMotorIndex, TRUE);

				if (bPrevAlarm != m_stSystemStatus.bMotorLifeDetected[m_nMotorIndex]) SendLogMessage(msgWarning, msg_life_standard_detected_on, m_nMotorIndex);

				// 반복성 체크
				if (0 < dwAlarmCount)
				{
					SetPTLifeCycleAlarmDetected(m_nMotorIndex, TRUE);
					if (dwPrevCycle != dwCurCycle)
					{
						SendLogMessage(msgWarning, msg_life_cycle_detected_on, m_nMotorIndex);
					}
				}
			}
			else
			{
				SetPTLifeAlarmDetected(m_nMotorIndex, FALSE);
				if (bPrevAlarm != m_stSystemStatus.bMotorLifeDetected[m_nMotorIndex]) SendLogMessage(msgWarning, msg_life_standard_detected_off, m_nMotorIndex);
			}

			if (dwPrevCnt != dwCurCnt)
			{
				
			}
		}	break;

		default:
		{
			nResult = 1000;
		}	break;
		}
	}	break;

	default:
	{
		//	여기에 진합하면 m_nOpCode가 opCode_read_motiongate_status으로 바뀌어 있는지 확인이 필요함
		ASSERT(0);
	}	break;
	}

	return nResult;
}


const INT CDeviceMotionGate::OpWriteUserRequest()
{
	INT nResult = 200;

	DWORD dwCmdCode = this->GetCommand(m_nMotorIndex);
	switch (dwCmdCode)
	{
	case cmd_empty:
	{
		nResult = 1;
	}	break;

	case cmd_enter_monitoring_mode:
	{
		m_nLastFasResult = DoCmdEnableAdministrationMode(TRUE);
	}	break;

	case cmd_exit_monitoring_mode:
	{
		m_nLastFasResult = DoCmdEnableAdministrationMode(FALSE);
	}	break;

	case cmd_alarm_reset:
	{
		m_nLastFasResult = DoCmdResetMotiongateAlarm();
	}	break;

	case cmd_change_information:
	{
		m_nOpCode = opCode_check_monitor_mode_on;
		m_stateMachine = state_change_information;

		m_nRetryCount = 0;

		m_stCommand.cmdGate.bitChangeInformation = FALSE;
		return nResult;
	}	break;

	case cmd_read_motiongate:
	{
		m_nLastFasResult = DoCmdGetMotiongateSystemStatus(m_nMotorIndex);
	}	break;

	case cmd_enable_servo:
	{
		ULONGLONG curTickCount, afterTickCFount;
		curTickCount = ::GetTickCount64();
		
		m_nLastFasResult = DoCmdEnableServo(m_nMotorIndex, TRUE);

		afterTickCFount = ::GetTickCount64();
		
		WORD timeElapse = static_cast<WORD>(afterTickCFount - curTickCount);
		
		TRACE(_T("Motor[#%d] = ServoOn Time Delay = %d \n"), m_nMotorIndex, timeElapse);
		
		m_stCommand.cmdMotor[m_nMotorIndex].command.bitServoOn = TRUE;
	}	break;

	case cmd_disable_servo:
	{
		m_nLastFasResult = DoCmdEnableServo(m_nMotorIndex, FALSE);

		m_stCommand.cmdMotor[m_nMotorIndex].command.bitServoOn = FALSE;
		
	}	break;

	case cmd_jog_cw:
	{
		const INT nSpeed = m_stCommand.cmdMotor[m_nMotorIndex].command.bitJogSpeedHigh ? 10000 : 1000;

		m_nLastFasResult = DoCmdMoveVelocity(m_nMotorIndex, nSpeed, TRUE);

		m_stCommand.cmdMotor[m_nMotorIndex].command.bitJogPlus = FALSE;
	}	break;

	case cmd_jog_ccw:
	{
		const INT nSpeed = m_stCommand.cmdMotor[m_nMotorIndex].command.bitJogSpeedHigh ? 10000 : 1000;
		m_nLastFasResult = DoCmdMoveVelocity(m_nMotorIndex, nSpeed, FALSE);

		m_stCommand.cmdMotor[m_nMotorIndex].command.bitJogMinus = FALSE;
	}	break;

	case cmd_normal_stop:
	{
		DoCmdMoveStop(m_nMotorIndex);

		m_stCommand.cmdMotor[m_nMotorIndex].command.bitStop = FALSE;
	}	break;

	case cmd_emergency_stop:
	{
		m_nLastFasResult = DoCmdEnableServo(m_nMotorIndex, FALSE);
		
		m_stCommand.cmdMotor[m_nMotorIndex].command.bitEStop = FALSE;
		m_stCommand.cmdMotor[m_nMotorIndex].command.bitServoOn = FALSE;
	}	break;

	case cmd_position_table:
	{
		if (m_stSystemStatus.stMotorStatus[m_nMotorIndex].wStatus.bitServoOn &&
			m_stSystemStatus.stMotorStatus[m_nMotorIndex].wStatus.bitInpsotion)
		{
			m_nLastFasResult = DoCmdExecutePositionTable(m_nMotorIndex, m_stCommand.cmdMotor[m_nMotorIndex].command.btIndex);

			m_stCommand.cmdMotor[m_nMotorIndex].command.bitPtOn = FALSE;
		}

	}	break;

	case cmd_origin:
	{
		m_nLastFasResult = DoCmdSearchOrigin(m_nMotorIndex);

		m_stCommand.cmdMotor[m_nMotorIndex].command.bitOrigin = FALSE;
	}	break;

	case cmd_start_read_parameter:	// Backup + Data Compare
	{
		m_stCommand.cmdGate.bitReadParameter = FALSE;

		m_nOpCode = opCode_backup_read_parameter;
		m_stateMachine = state_backup;

		m_nMotorIndex = FindNextMotorIndex();
		m_nParameterIndex = 0;

		if (m_stCommand.cmdGate.bitBackup) this->SendLogMessage(msgSequence, msg_backup_start);
		else { this->SendLogMessage(msgSequence, msg_compare_start); }

		m_dwTickCount = ::GetTickCount();

		return nResult;
	}	break;

	case cmd_start_restoration:
	{
		//	Backup Data 준비
		this->LoadCurrentParameterFromDataBase();

		m_nOpCode = opCode_enter_monitor_mode;
		m_stateMachine = state_restoration;
		m_nRetryCount = 0;

		this->SendLogMessage(msgSequence, msg_restoration_start);

		m_dwTickCount = ::GetTickCount();

		return nResult;
	}	break;

	case cmd_start_factory_reset:
	{
		m_nOpCode = opCode_enter_monitor_mode;
		m_stateMachine = state_factory_reset;
		m_nRetryCount = 0;

		m_dwTickCount = ::GetTickCount();
		return nResult;
	}	break;

	case cmd_start_assign_id:
	{
		m_nOpCode = opCode_enter_monitor_mode;
		m_stateMachine = state_assign_motor_id;
		m_nRetryCount = 0;

		m_dwTickCount = ::GetTickCount();
		return nResult;
	}	break;

	case cmd_rescan_slave:
	{
		m_nLastFasResult = DoCmdScanSlave();

		m_stCommand.cmdGate.bitRescanSlave = FALSE;
	}	break;

	case cmd_restart_system:
	{
		m_nRetryCount = 0;

		m_stateMachine = state_reboot;
		m_nOpCode = opCode_enter_monitor_mode;

		nResult = 1;
		return nResult;
	}	break;
	
	case cmd_motiongate_alarm_reset:
	{
		m_nLastFasResult = DoCmdMotionGateAlarmReset();
		if (FMM_OK != m_nLastFasResult)
		{
			AfxMessageBox(_T("Alarm Reset Fail !!"));
		}
		this->SendLogMessage(msgSequence, msg_motiongate_alarm_reset);
		nResult = 1;
	}	break;

	


	default:
	{
		ASSERT(0);
	}	break;
	}

	m_nMotorIndex = FindNextMotorIndex(m_nMotorIndex + 1);

	m_stateMachine = state_motion_gate_get_system_status_all;
	m_nOpCode = opCode_read_motiongate_status;

	return nResult;
}


const INT CDeviceMotionGate::OpReadParameters()
{
	if (m_stCommand.cmdGate.bitStopExecution)
	{
		StopSequence();
		m_bAutoBackupError = FALSE;

		return 1;
	}

	INT nResult = 100;

	switch (m_nOpCode)
	{
	case opCode_backup_read_parameter:
	{
		m_nLastFasResult = DoCmdGetParameter(m_nMotorIndex, m_nParameterIndex, &m_stMotorInfo[m_nMotorIndex].lParameter[m_nParameterIndex]);

		if (FMM_OK != m_nLastFasResult)
		{
			return -1;
		}

		m_nParameterIndex++;

		if (40 <= m_nParameterIndex)
		{
			for (INT nAxis = 1; nAxis < 9; nAxis++)
			{
				if (TRUE == IsMotorConnected(nAxis))
				{
					if (FMM_OK != DoCmdGetMotiongateSystemStatus(nAxis)) return -1;
					if (FMM_OK != DoCmdGetLifeCounter(nAxis)) return -1;
				}
			}
			

			m_nOpCode = opCode_backup_read_position_table;
			m_nParameterIndex = 0;
		}
	}	break;

	case opCode_backup_read_position_table:
	{
		m_nLastFasResult = DoCmdGetPositionTableItem(m_nMotorIndex, m_nParameterIndex + 1, &m_stMotorInfo[m_nMotorIndex].nodeItem[m_nParameterIndex]);

		m_nParameterIndex++;

		if (32 <= m_nParameterIndex)
		{
			m_nParameterIndex = 0;

			const INT nNextMotorIndex = FindNextMotorIndex(m_nMotorIndex + 1);

			if (m_nMotorIndex < nNextMotorIndex)
			{
				for (INT nAxis = 1; nAxis < 9; nAxis++)
				{
					if (TRUE == IsMotorConnected(nAxis))
					{
						if (FMM_OK != DoCmdGetMotiongateSystemStatus(nAxis)) return -1;
						if (FMM_OK != DoCmdGetLifeCounter(nAxis)) return -1;
					}
				}

				m_nOpCode = opCode_backup_read_parameter;
			}
			else
			{
				m_timeLastReadParameter = COleDateTime::GetCurrentTime();
				
				if (m_stCommand.cmdGate.bitBackup)
				{
					
					if (FALSE == this->SaveCurrentParameterToDataBase(m_stCommand.cmdGate.bitBackupNominal))
					{
						return -1;
					}

					SendLogMessage(msgNormal, msg_backup_done);
				}

				else
				{ // Compare Mode일 경우..
					SendLogMessage(msgNormal, msg_compare_done);
				}

				m_stCommand.cmdGate.bitBackup = FALSE;
				m_stCommand.cmdGate.bitBackupNominal = FALSE;

				m_stateMachine = state_motion_gate_get_system_status_all;
				m_nOpCode = opCode_read_motiongate_status;

				CheckAutoBackupSchedule();
			}

			m_nMotorIndex = nNextMotorIndex;
		}

	}	break;
	}

	return nResult;
}

const INT CDeviceMotionGate::OpRestoreParameter()
{
	if (m_stCommand.cmdGate.bitStopExecution)
	{
		StopSequence();
		return 1; // KKB 추가...
	}

	INT nResult = 100;
	
	switch (m_nOpCode)
	{
	case opCode_enter_monitor_mode:
	{
#ifndef SKIP_ADMINISTRATION_MODE
		m_nLastFasResult = DoCmdEnableAdministrationMode(TRUE);

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			m_nOpCode = opCode_check_monitor_mode_on;
		}	break;

		default:
		{
			return -1;
		}	break;
		}
#else
		m_nMotorIndex = FindNextMotorIndex();
		m_nParameterIndex = 0;

		m_nOpCode = opCode_write_main_parameter;
#endif
	}	break;	//	end of 'case opCode_enter_monitor_mode:'

	case opCode_check_monitor_mode_on:
	{
#ifndef SKIP_ADMINISTRATION_MODE
		m_nLastFasResult = DoCmdGetMotiongateSystemStatus(m_nMotorIndex);
		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			if (m_stSystemStatus.wStatus.bitMonitorOn)
			{
				m_nMotorIndex = FindNextMotorIndex();
				m_nParameterIndex = 0;

				m_nOpCode = opCode_write_main_parameter;
			}
			else
			{
				m_nRetryCount++;
				if (TRY_OVER_COUNT <= m_nRetryCount)
				{
					/*SetDetailStatus(_T("Failed to enter adminitration mode"), m_nProgressCounter);*/
					return -1;
				}

				m_nOpCode = opCode_enter_monitor_mode;
			}
		}	break;

		default:
		{
			return -1;
		}	break;
		}
#else
		m_nMotorIndex = FindNextMotorIndex();
		m_nParameterIndex = 0;

		m_nOpCode = opCode_write_main_parameter;
#endif
	}	break;	// end of 'case opCode_check_monitor_mode_on:'

	case opCode_write_main_parameter:
	{
		m_nLastFasResult = DoCmdSetParameter(m_nMotorIndex, m_nParameterIndex, m_stMotorInfo[m_nMotorIndex].lParameter[m_nParameterIndex]);

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			m_nParameterIndex++;

			if (40 <= m_nParameterIndex)
			{
				m_nOpCode = opCode_write_position_table;
				m_nParameterIndex = 0;
			}
		}	break;

		default:
		{
			return -1;
		}	break;
		}
	}	break;	// end of 'opCode_write_main_parameter:'

	case opCode_write_position_table:
	{
		m_nLastFasResult = DoCmdSetPositionTableItem(m_nMotorIndex, m_nParameterIndex + 1, &m_stMotorInfo[m_nMotorIndex].nodeItem[m_nParameterIndex]);

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			m_nParameterIndex++;

			if (32 <= m_nParameterIndex)
			{
				m_nParameterIndex = 0;

				m_nOpCode = opCode_save_main_parameter;
			}
		}	break;

		default:
		{
			return -1;
		}	break;
		}
	}	break;	// end of 'opCode_write_position_table:'

	case opCode_save_main_parameter:
	{
		m_nLastFasResult = DoCmdSaveAllParameters(m_nMotorIndex);

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			m_nOpCode = opCode_save_position_table;
		}	break;

		default:
		{
			return -1;
		}
		}
	}	break;	// end of 'opCode_save_main_parameter:'

	case opCode_save_position_table:
	{
		m_nLastFasResult = DoCmdSavePositionTableItems(m_nMotorIndex);

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			m_nParameterIndex = 0;
			m_nOpCode = opCode_confirm_main_parameter;
		}	break;

		default:
		{
			return -1;
		}
		}
	}	break;	// end of 'opCode_save_position_table:'

	case opCode_confirm_main_parameter:
	{
		LONG rtnValue = 0;
		m_nLastFasResult = DoCmdGetSavedParameter(m_nMotorIndex, m_nParameterIndex, &rtnValue);

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			if (rtnValue != m_stMotorInfo[m_nMotorIndex].lParameter[m_nParameterIndex])
			{
				return -1;
			}

			m_nParameterIndex++;

			if (40 <= m_nParameterIndex)
			{
				m_nOpCode = opCode_load_position_table;
				m_nParameterIndex = 0;
			}
		}	break;

		default:
		{
			return -1;
		}
		}
	}	break;	// end of 'opCode_confirm_main_parameter:'

	case opCode_load_position_table:
	{
		m_nLastFasResult = DoCmdLoadPositionTableItems(m_nMotorIndex);

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			m_nOpCode = opCode_confirm_position_table;
			m_nParameterIndex = 0;
		}	break;

		default:
		{
			return -1;
		}
		}

	}	break;	// end of 'opCode_load_position_table:'

	case opCode_confirm_position_table:
	{
		m_nLastFasResult = DoCmdComparePositionTableItem(m_nMotorIndex, m_nParameterIndex + 1, &m_stMotorInfo[m_nMotorIndex].nodeItem[m_nParameterIndex]);

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			m_nParameterIndex++;

			if (32 <= m_nParameterIndex)
			{
				m_nParameterIndex = 0;

				const INT nNextMotorIndex = FindNextMotorIndex(m_nMotorIndex + 1);

				if (m_nMotorIndex < nNextMotorIndex)
				{
					m_nOpCode = opCode_write_main_parameter;
				}
				else
				{
					CString strSystemName(GetSystemName());
					CDataBaseProject::GetInstance()->UpdateLatestRestoredID(strSystemName, m_nBackupID);

					if (m_bAutoHoming)
					{
						/*
						* 복원 완료 후 사용자의 선택에 따라 Homing이 진행되므로.. 주석처리 실시.
						m_stateMachine = state_home_all;

#ifndef SKIP_ADMINISTRATION_MODE
						m_nOpCode = opCode_check_monitor_mode_on;
#else
						//m_nOpCode = opCode_notify_backup_complete;
						m_nOpCode = opCode_check_monitor_mode_on;
#endif

						const ULONGLONG ullTimeStamp = ::GetTickCount64();
						for (INT nIndex = 0; nIndex < 9; nIndex++)
						{
							m_stCommand.timeStamp[nIndex] = ullTimeStamp;
						}

						m_nMotorIndex = FindNextMotorIndex();
						m_nParameterIndex = 0;
						m_nRetryCount = 0;

						SendLogMessage(msgSequence, msg_homing_all_start);
						*/
						
					}
					else
					{
						/*SetSimpleStatus(IDS_SIMPLE_RESTORATION_OK);*/
#ifndef SKIP_ADMINISTRATION_MODE
						m_nOpCode = opCode_exit_monitor_mode;
#else
						m_nOpCode = opCode_notify_backup_complete;
#endif
					}

					m_nRetryCount = 0;
				}

				m_nMotorIndex = nNextMotorIndex;
			}
		}	break;

		case FMC_DATA_COMPARE_ERROR:
		{
			return -1;
		}	break;

		default:
		{
			return -1;
		}	break;
		}
	}	break;	// end of 'opCode_confirm_position_table:'

	case opCode_notify_backup_complete:
	{
		m_nLastFasResult = DoCmdNotifyBackupDone();

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			m_stateMachine = state_motion_gate_get_system_status_all;
			m_nOpCode = opCode_read_motiongate_status;

			const DWORD dwTimer = ::GetTickCount() - m_dwTickCount;

			SendLogMessage(msgNormal, msg_restoration_done);
		}	break;

		default:
		{
			return -1;
		}	break;
		}
	}

	default:
	{
		TRACE(_T(""));
	}	break;	// end of 'default:'
	}	//	end of 'switch (m_nOpCode)'

	return nResult;
}


const INT CDeviceMotionGate::OpFactoryReset()
{
	if (m_stCommand.cmdGate.bitStopExecution)
	{
		StopSequence();

		return 1;
	}

	INT nResult = 100;

	switch (m_nOpCode)
	{
	case opCode_enter_monitor_mode:
	{
		SendLogMessage(msgNormal, msg_factory_reset_start);

#ifndef SKIP_ADMINISTRATION_MODE
		m_nLastFasResult = DoCmdEnableAdministrationMode(TRUE);

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			m_nOpCode = opCode_check_monitor_mode_on;
		}	break;

		default:
		{
			return -1;
		}	break;
		}
#else
		m_nOpCode = opCode_write_main_parameter;

		m_nMotorIndex = FindNextMotorIndex();
		m_nParameterIndex = 0;

		CDataBaseProject::GetInstance()->GetDefaultLoadParameterParameter(&m_stMotorInfo[m_stCommand.cmdGate.index]);
#endif
	}	break;	//	end of 'case opCode_enter_monitor_mode:'

	case opCode_check_monitor_mode_on:
	{
#ifndef SKIP_ADMINISTRATION_MODE
		m_nLastFasResult = DoCmdGetMotiongateSystemStatus(m_stCommand.cmdGate.index);
		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			if (m_stSystemStatus.wStatus.bitMonitorOn)
			{
				m_nMotorIndex = FindNextMotorIndex();
				m_nParameterIndex = 0;
				//				m_nProgressCounter = 0;

				CDataBaseProject::GetInstance()->GetDefaultLoadParameterParameter(&m_stMotorInfo[m_stCommand.cmdGate.index]);

				m_nOpCode = opCode_write_main_parameter;
			}
			else
			{
				m_nRetryCount++;
				if (TRY_OVER_COUNT <= m_nRetryCount)
				{
					/*SetDetailStatus(_T("Failed to enter adminitration mode"), m_nProgressCounter);*/
					return -1;
				}

				m_nOpCode = opCode_enter_monitor_mode;
			}
		}	break;

		default:
		{
			return -1;
		}	break;
		}
#else
		m_nOpCode = opCode_write_main_parameter;
#endif
	}	break;	// end of 'case opCode_check_monitor_mode_on:'

	case opCode_write_main_parameter:
	{
		m_nLastFasResult = DoCmdSetParameter(m_stCommand.cmdGate.index, m_nParameterIndex, m_stMotorInfo[m_stCommand.cmdGate.index].lParameter[m_nParameterIndex]);

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			m_nParameterIndex++;

			if (40 <= m_nParameterIndex)
			{
				m_nOpCode = opCode_write_position_table;
				m_nParameterIndex = 0;
			}
		}	break;

		default:
		{
			return -1;
		}	break;
		}
	}	break;	// end of 'opCode_write_main_parameter:'

	case opCode_write_position_table:
	{
		m_nLastFasResult = DoCmdSetPositionTableItem(m_stCommand.cmdGate.index, m_nParameterIndex/* + 1*/, &m_stMotorInfo[m_stCommand.cmdGate.index].nodeItem[m_nParameterIndex]);

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			m_nParameterIndex++;

			if (32 < m_nParameterIndex)
			{
				m_nParameterIndex = 0;

				m_nOpCode = opCode_save_main_parameter;
			}
		}	break;

		default:
		{
			return -1;
		}	break;
		}
	}	break;	// end of 'opCode_write_position_table:'

	case opCode_save_main_parameter:
	{
		m_nLastFasResult = DoCmdSaveAllParameters(m_stCommand.cmdGate.index);

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			m_nOpCode = opCode_save_position_table;
		}	break;

		default:
		{
			return -1;
		}
		}
	}	break;	// end of 'opCode_save_main_parameter:'

	case opCode_save_position_table:
	{
		m_nLastFasResult = DoCmdSavePositionTableItems(m_stCommand.cmdGate.index);

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			m_nParameterIndex = 0;
			m_nOpCode = opCode_confirm_main_parameter;
		}	break;

		default:
		{
			return -1;
		}
		}
	}	break;	// end of 'opCode_save_position_table:'

	case opCode_confirm_main_parameter:
	{
		LONG rtnValue = 0;
		m_nLastFasResult = DoCmdGetSavedParameter(m_stCommand.cmdGate.index, m_nParameterIndex, &rtnValue);

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			if (rtnValue != m_stMotorInfo[m_stCommand.cmdGate.index].lParameter[m_nParameterIndex])
			{
				return -1;
			}

			m_nParameterIndex++;
			//			m_nProgressCounter++;

			if (40 <= m_nParameterIndex)
			{
				m_nOpCode = opCode_load_position_table;
				m_nParameterIndex = 0;
			}
		}	break;

		default:
		{
			return -1;
		}
		}
	}	break;	// end of 'opCode_confirm_main_parameter:'

	case opCode_load_position_table:
	{
		m_nLastFasResult = DoCmdLoadPositionTableItems(m_stCommand.cmdGate.index);

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			m_nOpCode = opCode_confirm_position_table;
			m_nParameterIndex = 0;
		}	break;

		default:
		{
			return -1;
		}
		}

	}	break;	// end of 'opCode_load_position_table:'

	case opCode_confirm_position_table:
	{
		m_nLastFasResult = DoCmdComparePositionTableItem(m_stCommand.cmdGate.index, m_nParameterIndex/* + 1*/, &m_stMotorInfo[m_stCommand.cmdGate.index].nodeItem[m_nParameterIndex]);

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			m_nParameterIndex++;

			if (32 < m_nParameterIndex)
			{
				m_nOpCode = opCode_change_motiongate_id;
			}
		}	break;

		default:
		{
			return -1;
		}
		}
	}	break;	// end of 'opCode_confirm_position_table:'

	case opCode_change_motiongate_id:
	{
		m_nLastFasResult = DoCmdChangeSlaveID(m_stCommand.cmdGate.index, 0);

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			SendLogMessage(msgNormal, msg_factory_reset_done);
			m_nRetryCount = 0;
			m_nOpCode = opCode_reboot_motiongate;
		}	break;

		default:
		{
			return -1;
		}	break;
		}

	}	break;

	case opCode_reboot_motiongate:
	{
		m_nLastFasResult = DoCmdRebootMotiongate();

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			m_nOpCode = opCode_read_motiongate_info;
			nResult = 1000;
		}	break;

		default:
		{
			return -1;
		}	break;
		}

	}	break;

	case opCode_read_motiongate_info:
	{
		INT nResult = 1000;

		m_nLastFasResult = DoCmdReadMotiongateInfo();

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			SendLogMessage(msgInternal, msg_scan_slave_start);
			m_stateMachine = state_connect;
			m_nOpCode = opCode_scan_device;

			nResult = 100;

			m_wScanAxisConnection = 0;
			m_nMotorIndex = 0;
		}	break;

		case FMC_NO_RESPONSE:
		{
			m_nRetryCount++;

			if (REBOOT_CONNECTION_COUNT <= m_nRetryCount)
			{
				return -1;
			}

		}	break;

		default:
		{
			return -1;
		}	break;
		}
	}	break;

	case opCode_exit_monitor_mode:
	{
		m_nLastFasResult = DoCmdEnableAdministrationMode(FALSE);

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			m_nOpCode = opCode_check_monitor_mode_off;
		}	break;

		default:
		{
			return nResult = -1;
		}	break;
		}
	}	break;	// end of 'opCode_exit_monitor_mode:'

	case opCode_check_monitor_mode_off:
	{
		switch (DoCmdGetMotiongateSystemStatus(m_nMotorIndex))
		{
		case FMM_OK:
		{
			if (FALSE == m_stSystemStatus.wStatus.bitMonitorOn)
			{
				m_stateMachine = state_motion_gate_get_system_status_all;
				m_nOpCode = opCode_read_motiongate_status;

				const DWORD dwTimer = ::GetTickCount() - m_dwTickCount;
				TRACE(_T(""));
			}
			else
			{
				m_nRetryCount++;
				if (TRY_OVER_COUNT <= m_nRetryCount )
				{
					return -1;
				}

				m_nOpCode = opCode_enter_monitor_mode;
			}
		}	break;

		default:
		{
			return -1;
		}	break;
		}
	}

	}	//	end of 'switch (m_nOpCode)'

	return nResult;
}

const INT CDeviceMotionGate::OpAssignMotorID()
{
	if (m_stCommand.cmdGate.bitStopExecution)
	{
		StopSequence();

		return 1;
	}

	INT nResult = 100;

	switch (m_nOpCode)
	{
	case opCode_enter_monitor_mode:
	{
		SendLogMessage(msgSequence, msg_initialize_start);

#ifndef SKIP_ADMINISTRATION_MODE
		m_nLastFasResult = DoCmdEnableAdministrationMode(TRUE);

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			m_nOpCode = opCode_check_monitor_mode_on;
		}	break;

		default:
		{
			return -1;
		}	break;
		}
#else
		m_nOpCode = opCode_change_motiongate_id;
#endif
	}	break;	//	end of 'case opCode_enter_monitor_mode:'

	case opCode_check_monitor_mode_on:
	{
#ifndef SKIP_ADMINISTRATION_MODE
		m_nLastFasResult = DoCmdGetMotiongateSystemStatus(m_nMotorIndex);

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			if (m_stSystemStatus.wStatus.bitMonitorOn)
			{
				m_nMotorIndex = FindNextMotorIndex();
				m_nParameterIndex = 0;

				memcpy(&m_stMotorInfo[m_nMotorIndex], &m_stMotorInfo[0], sizeof(MOTOR_PARAMETER_DATA));

				m_nOpCode = opCode_change_motiongate_id;
			}
			else
			{
				m_nRetryCount++;
				if (TRY_OVER_COUNT <= m_nRetryCount )
				{

					return -1;
				}

				m_nOpCode = opCode_enter_monitor_mode;
			}
		}	break;

		default:
		{
			return -1;
		}	break;
		}
#else
		m_nOpCode = opCode_change_motiongate_id;
#endif
	}	break;	// end of 'case opCode_check_monitor_mode_on:'

	case opCode_change_motiongate_id:
	{
		m_nLastFasResult = DoCmdChangeSlaveID(m_nAssignSourceID, m_nAssignTargetID);

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			SendLogMessage(msgNormal, msg_initialize_done);

			m_nRetryCount = 0;
			m_nOpCode = opCode_reboot_motiongate;
		}	break;

		default:
		{
			return -1;
		}	break;
		}
	}	break;

	case opCode_reboot_motiongate:
	{
		m_nLastFasResult = DoCmdRebootMotiongate();

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			m_nOpCode = opCode_read_motiongate_info;
			nResult = 1000;
		}	break;

		default:
		{
			return -1;
		}	break;
		}

	}	break;

	case opCode_read_motiongate_info:
	{
		m_nOpCode = opCode_read_motiongate_info;

		INT nResult = 1000;

		m_nLastFasResult = DoCmdReadMotiongateInfo();

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			SendLogMessage(msgInternal, msg_scan_slave_start);

			m_stateMachine = state_connect;
			m_nOpCode = opCode_scan_device;

			nResult = 100;

			m_wScanAxisConnection = 0;
			m_nMotorIndex = 0;
		}	break;

		case FMC_NO_RESPONSE:
		{
			m_nRetryCount++;

			if (REBOOT_CONNECTION_COUNT <= m_nRetryCount)
			{
				return -1;
			}

		}	break;

		default:
		{
			return -1;
		}	break;
		}
	}	break;
	}
	return nResult;
}

INT CDeviceMotionGate::OpHomingMotion(int nMotor)
{
	for (INT iAxisNo = 1; iAxisNo < 9; iAxisNo++)
	{
		if (TRUE == IsHomingClose())
		{ 
			if (TRUE == IsAllMotorServoOff())
			{ // Homing Close 신호가 입력되었더라도 모든 모터의 Servo Off가 완료되어야 다시 Monitoring모드로 들어간다.
				m_stateMachine = state_motion_gate_get_system_status_all;
				m_nOpCode = opCode_read_motiongate_status;
				SetHomingClose(FALSE);
			
				SendLogMessage(msgSequence, msg_homing_all_finish);
			}
		}

		if (IsMotorConnected(iAxisNo))
		{ // Homing
			
			const ULONGLONG curTickCount = ::GetTickCount64();

			WORD timeElapse = static_cast<WORD>(curTickCount - m_stCommand.timeStamp[iAxisNo]);
			m_stCommand.timeStamp[iAxisNo] = curTickCount;
			
			m_nLastFasResult = DoCmdGetAxisStatus(iAxisNo);
			
			/*if (true)
			{
				CString strStatus;
				if (m_stSystemStatus.dwDetailStatus[iAxisNo].bitMotioning)
				{
					strStatus.Format(_T("Motor[%d] = Motioning. \n"), iAxisNo);
				}
				else
				{
					strStatus.Format(_T("Motor[%d] = Stop. \n"), iAxisNo);
				}
				
				TRACE(strStatus);
			}*/

			if (FMM_OK != m_nLastFasResult)
			{
				return -1;
			}

			const BOOL bBusy = m_stSystemStatus.dwDetailStatus[iAxisNo].bitMotioning;

			//if (s)
			if (TRUE == IsMotorAlarmDetected(iAxisNo))
			{
				m_stCommand.cmdMotor[iAxisNo].command.bitStop = TRUE;
				m_stCommand.cmdMotor[iAxisNo].command.bitOrigin = FALSE;
			}

			if (TRUE == m_stCommand.cmdMotor[iAxisNo].command.bitStop)	// 일반정지 확인
			{
				m_stCommand.cmdMotor[iAxisNo].command.bitOrigin = FALSE;

				m_stCommand.cmdMotor[iAxisNo].command.bitStop = FALSE;
				m_stCommand.cmdMotor[iAxisNo].command.bitJogPlus = FALSE;
				m_stCommand.cmdMotor[iAxisNo].command.bitJogMinus = FALSE;
				m_stCommand.cmdMotor[iAxisNo].command.bitOrigin = FALSE;
				m_stCommand.cmdMotor[iAxisNo].command.bitPtOn = FALSE;

				m_stCommand.cmdMotor[iAxisNo].command.bitServoOn = FALSE;
				m_stCommand.cmdMotor[iAxisNo].command.bitServoOff = TRUE;

				if (TRUE == IsMotorOriginOK(iAxisNo))
				{
					m_stSystemStatus.opStatus[iAxisNo].bitHoming = FALSE;
					m_stSystemStatus.opStatus[iAxisNo].bitHomingOK = TRUE;
					m_stSystemStatus.opStatus[iAxisNo].bitHomingNG = FALSE;
				}
				else
				{
					m_stSystemStatus.opStatus[iAxisNo].bitHoming = FALSE;
					m_stSystemStatus.opStatus[iAxisNo].bitHomingOK = FALSE;
					m_stSystemStatus.opStatus[iAxisNo].bitHomingNG = TRUE;
				}

				m_nLastFasResult = DoCmdMoveStop(iAxisNo);

				if (FMM_OK != m_nLastFasResult)
				{
					return -1;
				}

			}

			if (TRUE == m_stCommand.cmdMotor[iAxisNo].command.bitServoOff)	//	Servo Off
			{
				if (m_stSystemStatus.dwDetailStatus[iAxisNo].bitMotioning)
				{
					m_nLastFasResult = DoCmdMoveStop(iAxisNo);

					if (FMM_OK != m_nLastFasResult)
					{
						return -1;
					}

				}

				m_stCommand.cmdMotor[iAxisNo].timer.homeFail = 0;
				m_stCommand.cmdMotor[iAxisNo].timer.servoOnDelay = 0;
				m_stCommand.cmdMotor[iAxisNo].timer.servoOffDelay = 0;
				m_stCommand.cmdMotor[iAxisNo].command.bitServoOff = FALSE;

				if (m_stSystemStatus.opStatus[iAxisNo].bitHomingOK == TRUE)
				{
					m_stSystemStatus.opStatus[iAxisNo].bitHomingNG == FALSE;

					SendLogMessage(msgSequence, msg_homing_done, iAxisNo);
				}
				else if (m_stSystemStatus.opStatus[iAxisNo].bitHomingNG == TRUE)
				{
					m_stSystemStatus.opStatus[iAxisNo].bitHomingOK == FALSE;

					SendLogMessage(msgSequence, msg_homing_fail, iAxisNo);
				}

				if (FALSE == IsMotorAlarmDetected(iAxisNo))
				{ // Alarm 발생으로 중지하는 경우는 이미 알아서 Servo Off 진행한다.
				  // 만일 Servo Off 된상태에서  DoCmdEnableServo(iAxisNo, FALSE) 진행할 경우 FMP_SERVOONFAIL1 값을 반환 받게 되어 접속이 끊어진다.
					m_nLastFasResult = DoCmdEnableServo(iAxisNo, FALSE);

					if (FMM_OK != m_nLastFasResult)
					{
						return -1;
					}
				}
				
			}

			if (TRUE == m_stCommand.cmdMotor[iAxisNo].command.bitOrigin)
			{ // 해당 축의 Origin 명령이 들어올경우
				SendLogMessage(msgSequence, msg_homing_start, iAxisNo);

				if (FALSE == m_stSystemStatus.dwDetailStatus[iAxisNo].bitServoOn)
				{ // Servo Off 상태인경우.. Servo On 실시.
					m_stCommand.cmdMotor[iAxisNo].timer.servoOnDelay = 100;
					m_stCommand.cmdMotor[iAxisNo].timer.originDelay = 0;

					m_stSystemStatus.opStatus[iAxisNo].bitHoming = FALSE;
					m_stSystemStatus.opStatus[iAxisNo].bitHomingNG = FALSE;
					m_stSystemStatus.opStatus[iAxisNo].bitHomingOK = FALSE;

					m_nLastFasResult = DoCmdEnableServo(iAxisNo, TRUE);

					if (FMM_OK != m_nLastFasResult)
					{
						return -1;
					}
				}
				else
				{ // Servo On 상태일 경우 100ms간의 delay가 필요.
					m_stCommand.cmdMotor[iAxisNo].timer.originDelay += timeElapse;
					//TRACE(_T("Motor[%d] = originDelay (%d)\n"), iAxisNo, m_stCommand.cmdMotor[iAxisNo].timer.originDelay);

					if (timeElapse < m_stCommand.cmdMotor[iAxisNo].timer.servoOnDelay)
					{
						m_stCommand.cmdMotor[iAxisNo].timer.servoOnDelay -= timeElapse;

						return 1;
					}
					else
					{ // Delay Time이 다 지났다면 모션을 감시한다.
						if (true)
						{
							CString strTime;
							WORD homing = static_cast<WORD>(m_stDatabaseDeviceInfo.nHomingTimeout);
							WORD servoOnDelay = m_stCommand.cmdMotor[iAxisNo].timer.servoOnDelay;
							WORD originTime = m_stCommand.cmdMotor[iAxisNo].timer.originDelay;
							//TRACE(_T("(Servo ON!!) Motor #[%d] = HomingTimeout(%d) / servoOnDelay(%d) msec / timeElapse = (%d)msec / OriginDelay (%d)msec \n"), iAxisNo, homing, servoOnDelay, timeElapse, originTime);
						}

						m_stCommand.cmdMotor[iAxisNo].timer.servoOnDelay = 0;
					}

					// Servo On 상태이자 Servo Delay 시간까지 지났을 경우..

					if (FALSE == m_stSystemStatus.dwDetailStatus[iAxisNo].bitOriginReturning &&
						FALSE == m_stSystemStatus.opStatus[iAxisNo].bitHoming &&
						FALSE == bBusy)
					{ // Homing 동작중이 아니며, 아무 동작이 없는 경우. 원점탐색 시작...
						TRACE(_T("(Origin Start!!) Motor[%d] = originDelay (%d)\n"), iAxisNo, m_stCommand.cmdMotor[iAxisNo].timer.originDelay);

						m_stCommand.cmdMotor[iAxisNo].timer.homeFail = 0;

						m_nLastFasResult = DoCmdSearchOrigin(iAxisNo);

						if (FMM_OK != m_nLastFasResult)
						{
							return -1;
						}
					}

				}
			} 

			if (m_stSystemStatus.dwDetailStatus[iAxisNo].bitOriginReturning && TRUE == bBusy)
			{ // Homing wait... or Start..

				if (m_stSystemStatus.opStatus[iAxisNo].bitHomingNG == TRUE)
				{ // Homing 시작 후 Time Out이 될 경우
					m_stCommand.cmdMotor[iAxisNo].command.bitStop = TRUE;

					m_stCommand.cmdMotor[iAxisNo].timer.servoOffDelay = 300;
				}

				else if (0 == m_stCommand.cmdMotor[iAxisNo].timer.homeFail)
				{ // Time 초기화 및 원점탐색 시작.
					m_stCommand.cmdMotor[iAxisNo].command.bitOrigin = FALSE;
					m_stCommand.cmdMotor[iAxisNo].timer.homeFail = static_cast<WORD>(m_stDatabaseDeviceInfo.nHomingTimeout);
					//m_stCommand.cmdMotor[iAxisNo].timer.homeFail = 10000; // Test용...

					m_stSystemStatus.opStatus[iAxisNo].bitHoming = TRUE;
					m_stSystemStatus.opStatus[iAxisNo].bitHomingNG = FALSE;
					m_stSystemStatus.opStatus[iAxisNo].bitHomingOK = FALSE;
				}
			}
			else if (TRUE == m_stSystemStatus.opStatus[iAxisNo].bitHoming)
			{ //[bBusy == FALSE] , 중단(에러에 의한 멈춤 or Origin OK) 될 경우.. 

				m_stCommand.cmdMotor[iAxisNo].timer.homeFail = 0;
				m_stSystemStatus.opStatus[iAxisNo].bitHoming = FALSE;

				if (m_stSystemStatus.dwDetailStatus[iAxisNo].bitOriginComplete == TRUE)
				{ // OK일 경우
					m_stSystemStatus.opStatus[iAxisNo].bitHomingNG = FALSE;
					m_stSystemStatus.opStatus[iAxisNo].bitHomingOK = TRUE;
					
					SendLogMessage(msgWarning, msg_homing_done, iAxisNo);
				}
				else
				{ // NG 일 경우
					BOOL bError = m_stSystemStatus.dwDetailStatus[iAxisNo].bitErrorAll;
					m_stSystemStatus.opStatus[iAxisNo].bitHomingNG = TRUE;
					m_stSystemStatus.opStatus[iAxisNo].bitHomingOK = FALSE;

					SendLogMessage(msgWarning, msg_homing_stop, iAxisNo);
				}

				m_stCommand.cmdMotor[iAxisNo].command.bitStop = TRUE;

				m_stCommand.cmdMotor[iAxisNo].timer.servoOffDelay = 300;

				return 1;
			}

			if (TRUE == bBusy)
			{
				// Homing Time Check and TimeOut 처리.
				if (TRUE == IsActiveHomingAll())
				{
					if (m_stCommand.cmdMotor[iAxisNo].timer.homeFail)
					{ // Homing All일경우 처리..
						if (timeElapse < m_stCommand.cmdMotor[iAxisNo].timer.homeFail)
						{
							m_stCommand.cmdMotor[iAxisNo].timer.homeFail -= timeElapse;

							m_stSystemStatus.opStatus[iAxisNo].bitHoming = TRUE;

							SendLogMessage(msgNormal, msg_homing_all_timer, m_stCommand.cmdMotor[iAxisNo].timer.homeFail);
						}
						else
						{ // Time Over
							m_stCommand.cmdMotor[iAxisNo].timer.homeFail = 0;

							m_stSystemStatus.opStatus[iAxisNo].bitHoming = FALSE;
							m_stSystemStatus.opStatus[iAxisNo].bitHomingOK = FALSE;
							m_stSystemStatus.opStatus[iAxisNo].bitHomingNG = TRUE;

							m_stCommand.cmdMotor[iAxisNo].command.bitStop = TRUE;

							SendLogMessage(msgNormal, msg_homing_all_timer, m_stCommand.cmdMotor[iAxisNo].timer.homeFail);
						}
					}
				}
				else
				{ // 개별 모터 원점탐색 진행..
					m_stCommand.cmdMotor[iAxisNo].timer.homeFail = 0;
					m_stSystemStatus.opStatus[iAxisNo].bitHoming = TRUE;
				}
				
			}
		}
	}
	
}

const INT CDeviceMotionGate::OpHomingAll()
{
	if (m_stCommand.cmdGate.bitStopExecution)
	{
		StopSequence();

		m_nMotorIndex = FindNextMotorIndex();
		m_nOpCode = opCode_homing_stop;
		m_bAutoHoming = TRUE;

		return 1;
	}
	
	const ULONGLONG curTickCount = ::GetTickCount64();
	WORD timeElapse = static_cast<WORD>(curTickCount - m_stCommand.timeStamp[m_nMotorIndex]);
	
#ifdef TUITUTUTU
	if (true)
	{
		CString strCheck;
		CString strShow;

		//m_stCommand.cmdMotor[m_nMotorIndex].timer.homeFail -= timeElapse;
		CString strHomeFail, strCurTickCount, strTimeStamp, strTimeElapse, strHomeFailCalc;
		WORD wHomeCalc = m_stCommand.cmdMotor[m_nMotorIndex].timer.homeFail - timeElapse;
		
		strHomeFail.Format(_T("[strHomeFail : %d]", m_stCommand.cmdMotor[m_nMotorIndex].timer.homeFail));
		strHomeFailCalc.Format(_T("homeFail[%d] - timeElapse[%d] = %d"), m_stCommand.cmdMotor[m_nMotorIndex].timer.homeFail, timeElapse, wHomeCalc);
		strCurTickCount.Format(_T("[strCurTickCount : %d]"), curTickCount);
		strTimeStamp.Format(_T("[strTimeStamp : %d]"), m_stCommand.timeStamp);
		strTimeElapse.Format(_T("[strTimeElapse : %d]"), timeElapse);

		strCheck.Format(_T("%s - %s = %s / [%d sec] /"), strCurTickCount, strTimeStamp, strTimeElapse, /*strHomeFailCalc, */(m_nDetailProgressMax - wHomeCalc) / 1000 );

		
		strHomeFail.Format(_T("[homeFail : %d]"), m_stCommand.cmdMotor[m_nMotorIndex].timer.homeFail);
		strShow.Format(_T("Motor : [%d] %s / %s \r\n"), m_nMotorIndex, strCheck, strHomeFail);

		TRACE(strShow);

	}
#endif

	m_stCommand.timeStamp[m_nMotorIndex] = curTickCount;

	INT nResult = 100;

	switch (m_nOpCode)
	case opCode_enter_monitor_mode:
	{
	{
#ifndef SKIP_ADMINISTRATION_MODE
		m_nLastFasResult = DoCmdEnableAdministrationMode(TRUE);

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			m_nOpCode = opCode_check_monitor_mode_on;
		}	break;

		default:
		{
			return -1;
		}	break;
		}
#else
		m_nOpCode = opCode_homing_servo_on;
		
		m_nMotorIndex = FindNextMotorIndex(1);
		m_nParameterIndex = 0;
#endif
	}	break;	//	end of 'case opCode_enter_monitor_mode:'

	case opCode_check_monitor_mode_on:
	{
#ifndef SKIP_ADMINISTRATION_MODE
		m_nLastFasResult = DoCmdGetMotiongateSystemStatus(m_nMotorIndex);

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			if (m_stSystemStatus.wStatus.bitMonitorOn)
			{
				m_nMotorIndex = FindNextMotorIndex(1);
				m_nParameterIndex = 0;
				m_nRetryCount = 0;

				m_nOpCode = opCode_homing_servo_on;
			}
			else
			{
				m_nRetryCount++;
				if (TRY_OVER_COUNT <= m_nRetryCount )
				{
					return -1;
				}

				m_nOpCode = opCode_enter_monitor_mode;
			}
		}	break;

		default:
		{
			return -1;
		}	break;
		}
#else
		m_nOpCode = opCode_homing_servo_on;

		m_nMotorIndex = FindNextMotorIndex(1);
		m_nParameterIndex = 0;
#endif
	}	break;	// end of 'case opCode_check_monitor_mode_on:'

	case opCode_homing_servo_on:
	{
		switch (DoCmdGetMotiongateSystemStatus(m_nMotorIndex))
		{
		case FMM_OK:
		{
			m_stSystemStatus.opStatus[m_nMotorIndex].bitHoming = FALSE;
			m_stSystemStatus.opStatus[m_nMotorIndex].bitHomingOK = FALSE;
			m_stSystemStatus.opStatus[m_nMotorIndex].bitHomingNG = FALSE;

			m_stCommand.cmdMotor[m_nMotorIndex].timer.homeFail = 0;

			if (FALSE == m_stSystemStatus.stMotorStatus[m_nMotorIndex].wStatus.bitServoOn)
			{
				m_nLastFasResult = DoCmdEnableServo(m_nMotorIndex, TRUE);

				if (FMM_OK != m_nLastFasResult)
				{
					return -1;
				}
				//// KKB
				//else if(FMM_OK == m_nLastFasResult)
				//{
				//	//Sleep(10);
				//	DoCmdGetMotiongateSystemStatus(m_nMotorIndex);
				//}
			}
		}	break;

		default:
		{
			return -1;
		}	break;
		}

		const INT nNextMotorIndex = FindNextMotorIndex(m_nMotorIndex + 1);

		if (m_nMotorIndex < nNextMotorIndex)
		{
			m_nMotorIndex = nNextMotorIndex;
		}
		else
		{
			//	연결된 모든 모터의 Servo On을 확인한다.
			BOOL bResult = TRUE;
			
			for (INT nIndex = 1; nIndex <= 8; nIndex++)
			{
				if (m_stSystemStatus.btCurrentMotorConnectionStatus >> (nIndex - 1) & 0x1)
				{
					if (FALSE == m_stSystemStatus.stMotorStatus[nIndex].wStatus.bitServoOn)
					{
						bResult = FALSE;
						break;
					}

					m_stSystemStatus.opStatus[m_nMotorIndex].bitHoming = FALSE;
				}
			}

			m_nMotorIndex = FindNextMotorIndex(1);
			if (bResult)
			{
				nResult = 1000;
				m_nOpCode = opCode_homing_start;
			}
			else
			{
				m_nRetryCount++;
				if (m_nRetryCount >= TRY_OVER_COUNT)
				{
					/*SetDetailStatus(_T("Failed to servo on all motors"), m_nProgressCounter);*/
					return -1;
				}
			}
		}
	}	break;	// end of 'opCode_homing_servo_on'

	case opCode_homing_start:
	{
		m_nLastFasResult = DoCmdGetMotiongateSystemStatus(m_nMotorIndex);

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			if (m_stSystemStatus.stMotorStatus[m_nMotorIndex].wStatus.bitServoOn)
			{
				if (FALSE == m_stSystemStatus.opStatus[m_nMotorIndex].bitHoming &&
					FALSE == m_stSystemStatus.stMotorStatus[m_nMotorIndex].wStatus.bitMovingBackward &&
					FALSE == m_stSystemStatus.stMotorStatus[m_nMotorIndex].wStatus.bitMovingForward)
				{
					m_nLastFasResult = DoCmdSearchOrigin(m_nMotorIndex);
					
					if (FMM_OK != m_nLastFasResult)
					{
						return -1;
					}

					m_stSystemStatus.opStatus[m_nMotorIndex].bitHoming = TRUE;
					m_stSystemStatus.opStatus[m_nMotorIndex].bitHomingOK = FALSE;
					m_stSystemStatus.opStatus[m_nMotorIndex].bitHomingNG = FALSE;

					m_stCommand.cmdMotor[m_nMotorIndex].timer.homeFail = static_cast<WORD>(m_stDatabaseDeviceInfo.nHomingTimeout);
				}
			}
			else
			{
				//	Servo On이 안되었을리가...?
			}
		}	break;

		default:
		{
			return -1;
		}	break;
		}

		const INT nNextMotorIndex = FindNextMotorIndex(m_nMotorIndex + 1);
		if (m_nMotorIndex < nNextMotorIndex)
		{
			m_nMotorIndex = nNextMotorIndex;
		}
		else
		{
			//	연결된 모든 모터의 Homing On을 확인한다.
			BOOL bResult = TRUE;
			for (INT nIndex = 1; nIndex <= 8; nIndex++)
			{
				if (m_stSystemStatus.btCurrentMotorConnectionStatus >> (nIndex - 1) & 0x1)
				{
					if (FALSE == m_stSystemStatus.stMotorStatus[nIndex].wStatus.bitOriginOK &&
						FALSE == m_stSystemStatus.stMotorStatus[nIndex].wStatus.bitOriginRun)
					{
						bResult = FALSE;
						break;
					}
				}
			}

			m_nMotorIndex = FindNextMotorIndex(1);
			if (bResult)
			{
				m_nOpCode = opCode_homing_wait;
			}
		}
	}	break;	// end of 'opCode_homing_wait'

	case opCode_homing_wait:
	{
		switch (DoCmdGetMotiongateSystemStatus(m_nMotorIndex))
		{
		case FMM_OK:
		{
			if (m_stSystemStatus.stMotorStatus[m_nMotorIndex].wStatus.bitServoOn && m_stSystemStatus.stMotorStatus[m_nMotorIndex].wStatus.bitOriginOK)
			{
				m_nLastFasResult = DoCmdEnableServo(m_nMotorIndex, FALSE);

				if (FMM_OK != m_nLastFasResult)
				{
					return -1;
				}
			}
		}	break;

		default:
		{
			return -1;
		}	break;
		}

		switch (DoCmdGetAxisStatus(m_nMotorIndex))
		{
		case FMM_OK:
		{
			if (m_stSystemStatus.dwDetailStatus[m_nMotorIndex].bitMotioning == FALSE &&
				m_stSystemStatus.dwDetailStatus[m_nMotorIndex].bitServoOn == TRUE &&
				m_stSystemStatus.dwDetailStatus[m_nMotorIndex].bitOriginComplete == FALSE)
			{
				m_nLastFasResult = DoCmdEnableServo(m_nMotorIndex, FALSE);

				if (FMM_OK != m_nLastFasResult)
				{
					return -1;
				}
			}

			if (m_stSystemStatus.dwDetailStatus[m_nMotorIndex].bitErrorAll)
			{
				m_nLastFasResult = DoCmdEnableServo(m_nMotorIndex, FALSE);

				m_stSystemStatus.opStatus[m_nMotorIndex].bitHoming = FALSE;
				m_stSystemStatus.opStatus[m_nMotorIndex].bitHomingOK = FALSE;
				m_stSystemStatus.opStatus[m_nMotorIndex].bitHomingNG = TRUE;

				if (FMM_OK != m_nLastFasResult)
				{
					return -1;
				}
			}

			if (TRUE == m_stSystemStatus.stMotorStatus[m_nMotorIndex].wStatus.bitOriginOK)
			{
				m_stSystemStatus.opStatus[m_nMotorIndex].bitHoming = FALSE;
				m_stSystemStatus.opStatus[m_nMotorIndex].bitHomingOK = TRUE;
				m_stSystemStatus.opStatus[m_nMotorIndex].bitHomingNG = FALSE;
			}
		}	break;
		
		default:
		{
			return -1;
		}	break;
		
		}

		const INT nNextMotorIndex = FindNextMotorIndex(m_nMotorIndex + 1);
		if (m_nMotorIndex < nNextMotorIndex)
		{
			m_nMotorIndex = nNextMotorIndex;
		}
		else
		{
			//	연결된 모든 모터의 Homing On을 확인한다.
			BOOL bResult = TRUE, bStop = FALSE;
			DWORD dwCurrentMotorAlarmStatus = 0x00;
			for (INT nIndex = 1; nIndex <= 8; nIndex++)
			{

				if (m_stCommand.cmdMotor[nIndex].timer.homeFail <= timeElapse)
				{ // 60초 종료..
					TRACE(_T("m_stCommand.cmdMotor[%d].timer.homeFail = %d; \r\n"), nIndex);
					m_stCommand.cmdMotor[nIndex].timer.homeFail = 0;
					bStop = TRUE;
					SendLogMessage(msgNormal, msg_homing_all_timer, m_stCommand.cmdMotor[nIndex].timer.homeFail);
				}
				else if (m_stCommand.cmdMotor[nIndex].timer.homeFail)
				{
					m_stCommand.cmdMotor[nIndex].timer.homeFail -= timeElapse;
		
					//TRACE(_T("Motor : %d, %d TickCount : %d, Elapse : %d, Fail : %d\n"), GetTickCount(), timeElapse, m_stCommand.cmdMotor[m_nMotorIndex].timer.homeFail);

					SendLogMessage(msgNormal, msg_homing_all_timer, m_stCommand.cmdMotor[nIndex].timer.homeFail);
				}

				if (m_stSystemStatus.btCurrentMotorConnectionStatus >> (nIndex - 1) & 0x1)
				{
					/*if (TRUE == m_stSystemStatus.stMotorStatus[m_nMotorIndex].wStatus.bitServoOn ||
						FALSE == m_stSystemStatus.stMotorStatus[m_nMotorIndex].wStatus.bitOriginOK)
					{
						bResult = FALSE;
						break;
					}*/

					if (TRUE == m_stSystemStatus.dwDetailStatus[nIndex].bitErrorAll)
					{// Error Alarm Motor Index 검출.
						dwCurrentMotorAlarmStatus |= (0x01 << (nIndex - 1));
						if (dwCurrentMotorAlarmStatus == m_stSystemStatus.btCurrentMotorConnectionStatus)
						{ // 연결된 모든 모터가 Error Alarm이 켜져있다면..
							bResult = TRUE;
							break;
						}
					}

					if (TRUE == m_stSystemStatus.stMotorStatus[nIndex].wStatus.bitServoOn ||
						FALSE == m_stSystemStatus.stMotorStatus[nIndex].wStatus.bitOriginOK)
					{ // 모터가 연결된 상태에서 계속 진행중일 경우..
						bResult = FALSE;
						break;
					}


				}

				
			}

			m_nMotorIndex = FindNextMotorIndex(1);

			if (bResult)
			{ 
				m_nOpCode = opCode_exit_monitor_mode;
			}
			else if (bStop)
			{ // TimeOut
				m_nOpCode = opCode_homing_stop;
			}
			//else if (bAllalarm)
			//{ // all alarm
			//	m_nOpCode = opCode_homing_stop;
			//}
			else if (TRUE == IsUserHomingStop())
			{
				m_nOpCode = opCode_homing_stop;
			}
			
			
		}
	}	break;	// end of 'opCode_homing_servo_off'

	case opCode_homing_stop:
	{
		switch (DoCmdGetMotiongateSystemStatus(m_nMotorIndex))
		{
		case FMM_OK:
		{
			if (m_stSystemStatus.stMotorStatus[m_nMotorIndex].wStatus.bitMovingBackward ||
				m_stSystemStatus.stMotorStatus[m_nMotorIndex].wStatus.bitMovingForward)
			{
				m_nLastFasResult = DoCmdMoveStop(m_nMotorIndex);
				if (FMM_OK != m_nLastFasResult)
				{
					return -1;
				}
			}
			else if (m_stSystemStatus.stMotorStatus[m_nMotorIndex].wStatus.bitServoOn)
			{
				m_nLastFasResult = DoCmdEnableServo(m_nMotorIndex, FALSE);
				if (FMM_OK != m_nLastFasResult)
				{
					return -1;
				}
				if (FMM_OK == m_nLastFasResult)
				{
					DoCmdGetMotiongateSystemStatus(m_nMotorIndex);
				}

				m_stSystemStatus.opStatus[m_nMotorIndex].bitHoming = FALSE;

				if (m_stSystemStatus.stMotorStatus[m_nMotorIndex].wStatus.bitOriginOK)
				{
					m_stSystemStatus.opStatus[m_nMotorIndex].bitHomingOK = TRUE;
					m_stSystemStatus.opStatus[m_nMotorIndex].bitHomingNG = FALSE;
				}
				else if (m_stSystemStatus.stMotorStatus[m_nMotorIndex].wStatus.bitOriginRun)
				{
					m_stSystemStatus.opStatus[m_nMotorIndex].bitHomingOK = FALSE;
					m_stSystemStatus.opStatus[m_nMotorIndex].bitHomingNG = TRUE;
				}
				else
				{
					//	오면 안되는 영역
					m_stSystemStatus.stMotorStatus[m_nMotorIndex].wStatus;
					m_stSystemStatus.opStatus[m_nMotorIndex].bitHomingOK = FALSE;
					m_stSystemStatus.opStatus[m_nMotorIndex].bitHomingNG = TRUE;
				}
			}

		}	break;

		default:
		{
			return -1;
		}	break;
		}

		const INT nNextMotorIndex = FindNextMotorIndex(m_nMotorIndex + 1);
		if (m_nMotorIndex < nNextMotorIndex)
		{
			m_nMotorIndex = nNextMotorIndex;
		}
		else
		{
			//	연결된 모든 모터의 Homing On을 확인한다.
			BOOL bResult = TRUE;
			for (INT nIndex = 1; nIndex <= 8; nIndex++)
			{
				if (m_stSystemStatus.btCurrentMotorConnectionStatus >> (nIndex - 1) & 0x1)
				{
					if (m_stSystemStatus.stMotorStatus[nIndex].wStatus.bitServoOn)
					{
						bResult = FALSE;
						break;
					}
				}
			}

			m_nMotorIndex = FindNextMotorIndex(1);

			if (bResult)
			{
				m_nOpCode = opCode_exit_monitor_mode;
			}
		}
	}	break;	// end of 'opCode_homing_stable'

	case opCode_exit_monitor_mode:
	{
		/*SetDetailStatus(_T("Exiting Administration Mode"), m_nProgressCounter);*/

#ifndef SKIP_ADMINISTRATION_MODE
		m_nLastFasResult = DoCmdEnableAdministrationMode(FALSE);

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			m_nOpCode = opCode_check_monitor_mode_off;

		}	break;

		default:
		{
			return nResult = -1;
		}	break;
		}
#else
		m_nOpCode = opCode_check_monitor_mode_off;
#endif
	}	break;	// end of 'opCode_exit_monitor_mode:'

	case opCode_check_monitor_mode_off:
	{
		/*SetDetailStatus(_T("Checking Administration Mode"), m_nProgressCounter);*/

#ifndef SKIP_ADMINISTRATION_MODE
		switch (DoCmdGetMotiongateSystemStatus(m_nMotorIndex))
		{
		case FMM_OK:
		{
			if (FALSE == m_stSystemStatus.wStatus.bitMonitorOn)
			{
				const DWORD dwTimer = ::GetTickCount() - m_dwTickCount;

				BOOL bResult = TRUE;
				for (INT nIndex = 1; nIndex <= 8; nIndex++)
				{
					if (m_stSystemStatus.btCurrentMotorConnectionStatus >> (nIndex - 1) & 0x1)
					{
						if (m_stSystemStatus.opStatus[nIndex].bitHomingNG)
						{
							bResult = FALSE;
							break;
						}
					}
				}

				m_nMotorIndex = FindNextMotorIndex(1);
				m_stateMachine = state_motion_gate_get_system_status_all;
				m_nOpCode = opCode_read_motiongate_status;

				CString strResult;
				INT nLevel = msgNormal;
				CString strMessgage;
				if (bResult)
				{
					SendLogMessage(msgNormal, msg_homing_all_done);
				}
				else
				{
					SendLogMessage(msgError, msg_homing_all_fail);
					return -1;
				}
			}
			else
			{
				m_nRetryCount++;
				if (TRY_OVER_COUNT <= m_nRetryCount)
				{
					/*SetDetailStatus(_T("Failed to exit adminitration mode"), m_nProgressCounter);*/
					return -1;
				}

				m_nOpCode = opCode_enter_monitor_mode;
			}
		}	break;

		default:
		{
			return -1;
		}	break;
		}
#else
		m_nMotorIndex = FindNextMotorIndex(1);
		m_stateMachine = state_motion_gate_get_system_status_all;
		m_nOpCode = opCode_read_motiongate_status;
		m_bAutoHoming = FALSE;

		BOOL bResult = TRUE;
		BOOL bAlarm = FALSE;
		for (INT nIndex = 1; nIndex <= 8; nIndex++)
		{
			if (m_stSystemStatus.btCurrentMotorConnectionStatus >> (nIndex - 1) & 0x1)
			{
				m_stSystemStatus.opStatus[nIndex].bitHoming = FALSE;

				if (m_stSystemStatus.opStatus[nIndex].bitHomingNG)
				{// 한개라도 NG이면 Fail을 띄워야 한다.
					bResult &= FALSE;

					m_stSystemStatus.opStatus[nIndex].bitHomingOK = FALSE;
					m_stSystemStatus.opStatus[nIndex].bitHomingNG = TRUE;
					//break;
				}
				
				if (TRUE == m_stSystemStatus.dwDetailStatus[nIndex].bitErrorAll)
				{ // 하나라도 Error 이 감지된다면 Fail을 띄워야한다.

					m_stSystemStatus.opStatus[nIndex].bitHomingOK = FALSE;
					m_stSystemStatus.opStatus[nIndex].bitHomingNG = TRUE;

					bAlarm = TRUE;
				}

				if (m_stSystemStatus.stMotorStatus[nIndex].wStatus.bitOriginOK)
				{
					
					m_stSystemStatus.opStatus[nIndex].bitHomingOK = TRUE;
					m_stSystemStatus.opStatus[nIndex].bitHomingNG = FALSE;
				}

			}
		}

		BOOL bUserStop = IsUserHomingStop();
		if (bUserStop)
		{ // KKB
			SendLogMessage(msgNormal, msg_homing_all_stop);
			SetHomingStop(FALSE);
			SetActiveHomingAll(FALSE);
		}
		else if (!bResult || bAlarm)
		{
			SendLogMessage(msgError, msg_homing_all_fail);
			SetActiveHomingAll(FALSE);
			return -1;
		}
		else if (bResult)
		{
			SendLogMessage(msgNormal, msg_homing_all_done);
			SetActiveHomingAll(FALSE);
		}
#endif
	}	break;	// end of 'opCode_check_monitor_mode_off:'

	default:
	{
		TRACE(_T(""));
	}	break;	// end of 'default:'
	}	//	end of 'switch (m_nOpCode)'

	return nResult;
}

const INT CDeviceMotionGate::OpChangeDeviceInformation()
{
	INT nResult = 100;

	switch (m_nOpCode)
	{
	case opCode_check_monitor_mode_on:
	{
#ifndef SKIP_ADMINISTRATION_MODE
		m_nLastFasResult = DoCmdGetMotiongateSystemStatus(m_nMotorIndex);

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			if (m_stSystemStatus.wStatus.bitMonitorOn)
			{
				m_nOpCode = opCode_change_device_info;
			}
			else
			{
				m_nRetryCount++;
				if (TRY_OVER_COUNT <= m_nRetryCount)
				{
					return -1;
				}

				m_nOpCode = opCode_enter_monitor_mode;
			}
		}	break;

		default:
		{
			return -1;
		}	break;
		}
#else
		m_nOpCode = opCode_change_device_info;
#endif
	}	break;	// end of 'case opCode_check_monitor_mode_on:'

	case opCode_enter_monitor_mode:
	{
#ifndef SKIP_ADMINISTRATION_MODE
		m_nLastFasResult = DoCmdEnableAdministrationMode(TRUE);

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			m_nOpCode = opCode_check_monitor_mode_on;
		}	break;

		default:
		{
			return -1;
		}	break;
		}
#else
		m_nOpCode = opCode_check_monitor_mode_on;
#endif
	}	break;	//	end of 'case opCode_enter_monitor_mode:'

	case opCode_change_device_info:
	{
		m_nLastFasResult = DoCmdChangeMotiongateInformation();

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			m_nOpCode = opCode_reboot_motiongate;
		}	break;

		default:
		{
			return -1;
		}	break;
		}
	}	break;	//	end of 'case opCode_change_device_name:'

	case opCode_reboot_motiongate:
	{
		m_nLastFasResult = DoCmdRebootMotiongate();
		m_nDetailProgressPos = 0;

		if (FMM_OK == m_nLastFasResult)
		{
			//			Initialize(FALSE);

			m_nOpCode = opCode_read_motiongate_info;
			nResult = 1000;
		}	break;

		m_nLastFasResult = DoCmdRebootMotiongate();

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			Initialize(FALSE);

			m_nRetryCount = 0;

			m_nOpCode = opCode_read_motiongate_info;
			nResult = 1000;
		}	break;

		default:
		{
			return -1;
		}	break;
		}
	}

	case opCode_read_motiongate_info:
	{
		m_nLastFasResult = DoCmdReadMotiongateInfo();
		m_nDetailProgressPos++;

		switch (m_nLastFasResult)
		{
		case FMM_OK:
		{
			SendLogMessage(msgInternal, msg_scan_slave_start);
			m_stateMachine = state_connect;
			m_nOpCode = opCode_scan_device;

			nResult = 100;

			m_wScanAxisConnection = 0;
			m_nMotorIndex = 0;
		}	break;

		case FMC_REFUSED:	//	IP Converter의 경우 Refuse되는 경우가 발생
		case FMC_TIMEOUT_ERROR:
		case FMC_NO_RESPONSE:
		{
			m_nRetryCount++;

			if (REBOOT_CONNECTION_COUNT <= m_nRetryCount)
			{
				m_stateMachine = state_connect;
				m_nOpCode = opCode_no_response;
			}

		}	break;

		default:
		{
			return -1;
		}	break;
		}
	}	break;
	}

	//case opCode_read_motiongate_info:
	//{
	//	m_nLastFasResult = DoCmdReadMotiongateInfo();
	//	switch (m_nLastFasResult)
	//	{
	//	case FMM_OK:
	//	{
	//		m_nOpCode = opCode_exit_monitor_mode;

	//		m_nRetryCount = 0;
	//	}	break;

	//	default:
	//	{
	//		return -1;
	//	}	break;
	//	}
	//}	break;		//	end of 'case opCode_read_motiongate_info:'

	//case opCode_exit_monitor_mode:
	//{
	//	m_nLastFasResult = DoCmdEnableAdministrationMode(FALSE);

	//	switch (m_nLastFasResult)
	//	{
	//	case FMM_OK:
	//	{
	//		m_nOpCode = opCode_check_monitor_mode_off;
	//	}	break;

	//	default:
	//	{
	//		return -1;
	//	}	break;
	//	}
	//}	break;	//	end of 'case opCode_exit_monitor_mode:'

	//case opCode_check_monitor_mode_off:
	//{
	//	m_nLastFasResult = DoCmdGetMotiongateSystemStatus(m_nMotorIndex);

	//	switch (m_nLastFasResult)
	//	{
	//	case FMM_OK:
	//	{
	//		if (FALSE == m_stSystemStatus.wStatus.bitMonitorOn)
	//		{
	//			SendLogMessage(msgNormal, msg_motiongate_change_system_done);

	//			m_stateMachine = state_motion_gate_get_system_status_all;
	//			m_nMotorIndex = FindNextMotorIndex(1);
	//		}
	//		else
	//		{
	//			m_nRetryCount++;
	//			if (TRY_OVER_COUNT < m_nRetryCount)
	//			{
	//				return -1;
	//			}

	//			m_nOpCode = opCode_exit_monitor_mode;
	//		}
	//	}	break;	//	end of 'case opCode_check_monitor_mode_off:'

	//	default:
	//	{
	//		return -1;
	//	}	break;
	//	}
	//}	break;
	//}

	return nResult;
}

const DWORD CDeviceMotionGate::GetMotiongateStatus()
{
	return m_stSystemStatus.wStatus.wValue;
}


const DWORD CDeviceMotionGate::GetConnectedMotorStatus()
{
	return m_wCurAxisConnection;
}


const INT CDeviceMotionGate::GetConnectedMotorCount()
{
	INT nResult = 0;
	for (INT nIndex = 0; nIndex < 16; nIndex++)
	{
		if ((m_wCurAxisConnection >> nIndex) & 0x1)
		{
			nResult++;
		}
	}

	return nResult;
}

const INT CDeviceMotionGate::GetContinuouslyConnectedMotorCount(const INT nExceptMotorIndex)
{
	INT nResult = 0;

	for (INT nIndex = 1; nIndex < 16; nIndex++)
	{
		if (nExceptMotorIndex == nIndex ||
			(m_wCurAxisConnection >> nIndex) & 0x1)
		{
			nResult++;
		}
		else
		{
			break;
		}
	}

	return nResult;
}


const BOOL CDeviceMotionGate::IsMotorConnectionContinuity(const INT nExceptMotorIndex)
{
	return GetMotorCount() == GetContinuouslyConnectedMotorCount(nExceptMotorIndex); // 0번 이외의 연결된 모터 개수!
}


const DWORD CDeviceMotionGate::GetMotorStatus(const INT nAxisNo)
{
	return m_stSystemStatus.stMotorStatus[nAxisNo].wStatus.wValue;
}


const DOUBLE CDeviceMotionGate::GetMotorCurPosition(const INT nAxisNo)
{
	return static_cast<DOUBLE>(m_stSystemStatus.stMotorStatus[nAxisNo].wPosInteger) + static_cast<DOUBLE>(m_stSystemStatus.stMotorStatus[nAxisNo].wPosDecimal) / 100;
}


const DWORD	CDeviceMotionGate::GetMotorCurPositionTable(const INT nAxisNo)
{
	return m_stSystemStatus.stMotorStatus[nAxisNo].wPositionTableNo;
}


const DWORD CDeviceMotionGate::GetMotorErrorCode(const INT nAxisNo)
{ 
	// return m_stSystemStatus.stMotorStatus[nAxisNo].btErrorCode; 기존..
	return m_stSystemStatus.btMotorAlarm[nAxisNo];
}


const DWORD CDeviceMotionGate::GetMotorLifeCount(const INT nAxisNo)
{
	return m_stSystemStatus.dwLifeCount[nAxisNo];
}


const DWORD CDeviceMotionGate::GetCommand(const INT iAxisNo)
{
	if (m_bBackupReservation && m_bAutoRestoration)
	{ 
		if (FALSE == IsSystemStatusError() && FALSE == IsBusy())
		{ // 에러 발생 및 Busy가 아닌조건에서만 AutoBackup 기능을 진행한다.
			SetAutoBackupDeviceError(FALSE);
			
			COleDateTime now = COleDateTime::GetCurrentTime();
			m_timespanAutoBackup -= (now - m_timeAutoBackupTickTock);
			m_timeAutoBackupTickTock = now;

			if (m_nPauseBackupCounter && m_timespanAutoBackup.GetTotalSeconds() < 60.0f)
			{
				m_timespanAutoBackup = COleDateTimeSpan(0, 0, 1, 0);
			}
			else if (m_timespanAutoBackup.GetTotalSeconds() == 0.0f)
			{
				CDataBaseProject* pProjectDB = CDataBaseProject::GetInstance();

				CString strAddress;	GetAddress(strAddress);
				m_strBackupName.Format(_T("%s_Auto%d"), GetSystemName(), pProjectDB->GetProjectBackupListCount(strAddress));

				m_bBackupReservation = FALSE;
				m_stCommand.cmdGate.bitBackup = TRUE;
				return cmd_start_read_parameter;
			}
		}
		else
		{ // 만일 AutoBackup이 진행된다면 현재 진행 상황을 변수(SetAutoBackupDeviceError(TRUE))에 저장하고 AutoBackup진행을 중지시킨다,.
			EnableAutoRestoration(FALSE);
			SetAutoBackupDeviceError(TRUE);
		}
	}

	else if (FALSE == IsAutoRestorationEnabled())
	{ // 특정 사유로 인해 AutoBackup이 진행된 상태에서 SystemError or Busy or KeepStop 상태를 판단한다.
		if (TRUE == IsSystemStatusError() || TRUE == IsBusy() ||  TRUE == IsKeepStopAutoBackup())
		{ // SystemError or Busy or KeepStop 일경우 그냥 넘어간다.

		}
		else
		{ // 시스템 에러로 인하여 AutoBackup이 중지되었으며, 시스템 정상화가 이뤄졌다면 다시 AutoBackup을 진행한다.
			if (TRUE == IsAutoBackupError())
			{
				EnableAutoRestoration(TRUE);
				SetAutoBackupDeviceError(FALSE);
			}
		}
	}
	if (true)
	{
		if (m_stCommand.cmdGate.bitReadParameter)
		{
			m_stCommand.cmdGate.bitReadParameter = FALSE;
			return cmd_start_read_parameter;
		}

		if (m_stCommand.cmdGate.bitAssginID)
		{
			m_stCommand.cmdGate.bitAssginID = FALSE;
			return cmd_start_assign_id;
		}

		if (m_stCommand.cmdGate.bitRestoration)
		{
			m_stCommand.cmdGate.bitRestoration = FALSE;
			return cmd_start_restoration;
		}

		if (m_stCommand.cmdGate.bitFactoryReset)
		{
			m_stCommand.cmdGate.bitFactoryReset = FALSE;
			return cmd_start_factory_reset;
		}

		if (m_stCommand.cmdGate.bitChangeInformation)
		{
			CString strName = GetSystemName();
			m_stCommand.cmdGate.bitChangeInformation = FALSE;
			return cmd_change_information;
		}

		if (m_stCommand.cmdGate.bitReboot)
		{
			m_stCommand.cmdGate.bitReboot = FALSE;
			return cmd_restart_system;
		}

		if (m_stCommand.cmdGate.bitMotionAlarmReset)
		{
			m_stCommand.cmdGate.bitMotionAlarmReset = FALSE;
			return cmd_motiongate_alarm_reset;
		}

		if (m_stCommand.cmdMotor[iAxisNo].command.bitEStop)	// 긴급정지 확인
		{
			//	긴급정지 작동 순서
			//	Stop + Servo Off // Stop 이후에 Servo Off

			m_stCommand.cmdMotor[iAxisNo].command.bitEStop = FALSE;
			m_stCommand.cmdMotor[iAxisNo].command.bitJogPlus = FALSE;
			m_stCommand.cmdMotor[iAxisNo].command.bitJogMinus = FALSE;
			m_stCommand.cmdMotor[iAxisNo].command.bitOrigin = FALSE;
			m_stCommand.cmdMotor[iAxisNo].command.bitPtOn = FALSE;

			m_stCommand.cmdMotor[iAxisNo].command.bitStop = TRUE;
			m_stCommand.cmdMotor[iAxisNo].command.bitServoOn = FALSE;

			return cmd_emergency_stop;
		}
	}
	

	if (TRUE == IsAutoHomingEnabled())
	{ // Homing
		const ULONGLONG curTickCount = ::GetTickCount64();
		/*WORD timeElapse = static_cast<WORD>(curTickCount - m_stCommand.timeStamp[m_nMotorIndex]);
		m_stCommand.timeStamp[m_nMotorIndex] = curTickCount;*/
		WORD timeElapse = static_cast<WORD>(curTickCount - m_stCommand.timeStamp[iAxisNo]);
		m_stCommand.timeStamp[iAxisNo] = curTickCount;

		DoCmdGetAxisStatus(iAxisNo);

		const BOOL bBusy = m_stSystemStatus.stMotorStatus[iAxisNo].wStatus.bitMovingForward ||
			m_stSystemStatus.stMotorStatus[iAxisNo].wStatus.bitMovingBackward;
		
		//if (s)
		if ( TRUE== IsMotorAlarmDetected(iAxisNo) ||
			sts_error_motor_none != m_stSystemStatus.btMotorAlarm[iAxisNo])
		{
			m_stCommand.cmdMotor[iAxisNo].command.bitStop = TRUE;
			m_stCommand.cmdMotor[iAxisNo].command.bitOrigin = FALSE;
			m_stSystemStatus.opStatus[iAxisNo].bitHomingNG = TRUE;
		}

		if (TRUE == m_stCommand.cmdMotor[iAxisNo].command.bitStop)	// 일반정지 확인
		{
			m_stCommand.cmdMotor[iAxisNo].command.bitOrigin = FALSE;

			m_stCommand.cmdMotor[iAxisNo].command.bitStop = FALSE;
			m_stCommand.cmdMotor[iAxisNo].command.bitJogPlus = FALSE;
			m_stCommand.cmdMotor[iAxisNo].command.bitJogMinus = FALSE;
			m_stCommand.cmdMotor[iAxisNo].command.bitOrigin = FALSE;
			m_stCommand.cmdMotor[iAxisNo].command.bitPtOn = FALSE;

			m_stCommand.cmdMotor[iAxisNo].command.bitServoOn = FALSE;
			m_stCommand.cmdMotor[iAxisNo].command.bitServoOff = TRUE;

			//DoCmdEnableServo(iAxisNo, FALSE);
			return cmd_normal_stop;
		}

		if (TRUE == m_stCommand.cmdMotor[iAxisNo].command.bitServoOff)	//	Servo Off
		{
			if (m_stSystemStatus.stMotorStatus[iAxisNo].wStatus.bitMovingBackward ||
				m_stSystemStatus.stMotorStatus[iAxisNo].wStatus.bitMovingForward)
			{
				return cmd_normal_stop;
			}

			m_stCommand.cmdMotor[iAxisNo].timer.homeFail = 0;
			m_stCommand.cmdMotor[iAxisNo].timer.servoOnDelay = 0;
			m_stCommand.cmdMotor[iAxisNo].timer.servoOffDelay = 0;
			m_stCommand.cmdMotor[iAxisNo].command.bitServoOff = FALSE;

			if (m_stSystemStatus.opStatus[iAxisNo].bitHomingOK == TRUE)
			{
				SendLogMessage(msgSequence, msg_homing_done, iAxisNo);
			}
			else if (m_stSystemStatus.opStatus[iAxisNo].bitHomingNG == TRUE)
			{
				SendLogMessage(msgSequence, msg_homing_fail, iAxisNo);
			}

			return cmd_disable_servo;
		}

		if (TRUE == m_stCommand.cmdMotor[iAxisNo].command.bitOrigin)
		{ // 해당 축의 Origin 명령이 들어올경우
			SendLogMessage(msgSequence, msg_homing_start, iAxisNo);

			if (FALSE == m_stSystemStatus.stMotorStatus[iAxisNo].wStatus.bitServoOn)
			{ // Servo Off 상태인경우.. Servo On 실시.
				m_stCommand.cmdMotor[iAxisNo].timer.servoOnDelay = 1000;
				m_stCommand.cmdMotor[iAxisNo].timer.originDelay = 0;

				m_stSystemStatus.opStatus[iAxisNo].bitHoming = FALSE;
				m_stSystemStatus.opStatus[iAxisNo].bitHomingNG = FALSE;
				m_stSystemStatus.opStatus[iAxisNo].bitHomingOK = FALSE;
							
				return cmd_enable_servo;
			}
			else
			{ // Servo On 상태일 경우 1초간의 delay가 필요.
				m_stCommand.cmdMotor[iAxisNo].timer.originDelay += timeElapse;
				TRACE(_T("Motor[%d] = originDelay (%d)\n"), iAxisNo, m_stCommand.cmdMotor[iAxisNo].timer.originDelay);
				
				if (timeElapse < m_stCommand.cmdMotor[iAxisNo].timer.servoOnDelay)
				{
					m_stCommand.cmdMotor[iAxisNo].timer.servoOnDelay -= timeElapse;
					
					return cmd_empty;
				}
				else
				{ // Delay Time이 다 지났다면 모션을 감시한다.
					if (true)
					{
						CString strTime;
						WORD homing = static_cast<WORD>(m_stDatabaseDeviceInfo.nHomingTimeout);
						WORD servoOnDelay = m_stCommand.cmdMotor[iAxisNo].timer.servoOnDelay;
						WORD originTime = m_stCommand.cmdMotor[iAxisNo].timer.originDelay;
						TRACE(_T("(Servo ON!!) Motor #[%d] = HomingTimeout(%d) / servoOnDelay(%d) msec / timeElapse = (%d)msec / OriginDelay (%d)msec \n"), iAxisNo, homing, servoOnDelay, timeElapse, originTime);
					}

					m_stCommand.cmdMotor[iAxisNo].timer.servoOnDelay = 0;
				}

			}		

			// Servo On 상태이자 Servo Delay 시간까지 지났을 경우..

			if (FALSE == m_stSystemStatus.stMotorStatus[iAxisNo].wStatus.bitOriginRun &&
				//TRUE == m_stSystemStatus.stMotorStatus[iAxisNo].wStatus.bitServoOn &&
				FALSE == bBusy)
			{ // Homing 동작중이 아니며, 아무 동작이 없는 경우. 원점탐색 시작...
				TRACE(_T("(Origin Start!!) Motor[%d] = originDelay (%d)\n"), iAxisNo, m_stCommand.cmdMotor[iAxisNo].timer.originDelay);
				
				m_stCommand.cmdMotor[iAxisNo].timer.homeFail = 0;
				
				return cmd_origin;
			}
		}

		if (m_stSystemStatus.stMotorStatus[iAxisNo].wStatus.bitOriginRun &&  TRUE == bBusy)
		{ // Homing wait... or Start..
			
			if (m_stSystemStatus.opStatus[iAxisNo].bitHomingNG == TRUE)
			{ // Homing 시작 후 Time Out이 될 경우
				m_stCommand.cmdMotor[iAxisNo].command.bitStop = TRUE;

				m_stCommand.cmdMotor[iAxisNo].timer.servoOffDelay = 300;
			}

			else if (0 == m_stCommand.cmdMotor[iAxisNo].timer.homeFail)
			{ // Time 초기화 및 원점탐색 시작.
				m_stCommand.cmdMotor[iAxisNo].command.bitOrigin = FALSE;
				m_stCommand.cmdMotor[iAxisNo].timer.homeFail = static_cast<WORD>(m_stDatabaseDeviceInfo.nHomingTimeout);
				// m_stCommand.cmdMotor[iAxisNo].timer.homeFail = 10000; Test용...

				m_stSystemStatus.opStatus[iAxisNo].bitHoming = TRUE;
				m_stSystemStatus.opStatus[iAxisNo].bitHomingNG = FALSE;
				m_stSystemStatus.opStatus[iAxisNo].bitHomingOK = FALSE;
			}
		}
		else if (TRUE == m_stSystemStatus.opStatus[iAxisNo].bitHoming)
		{ //[bBusy == FALSE] , 중단(에러에 의한 멈춤 or Origin OK) 될 경우.. 

			m_stCommand.cmdMotor[iAxisNo].timer.homeFail = 0;
			m_stSystemStatus.opStatus[iAxisNo].bitHoming = FALSE;
			
			if (m_stSystemStatus.stMotorStatus[iAxisNo].wStatus.bitOriginOK == TRUE)
			{ // OK일 경우
				m_stSystemStatus.opStatus[iAxisNo].bitHoming = FALSE;
				m_stSystemStatus.opStatus[iAxisNo].bitHomingNG = FALSE;
				m_stSystemStatus.opStatus[iAxisNo].bitHomingOK = TRUE;
			}
			else
			{ // NG 일 경우
				m_stSystemStatus.opStatus[iAxisNo].bitHoming = FALSE;
				m_stSystemStatus.opStatus[iAxisNo].bitHomingNG = TRUE;
				m_stSystemStatus.opStatus[iAxisNo].bitHomingOK = FALSE;
				
				SendLogMessage(msgWarning, msg_homing_stop, iAxisNo);
			}

			m_stCommand.cmdMotor[iAxisNo].command.bitStop = TRUE;

			m_stCommand.cmdMotor[iAxisNo].timer.servoOffDelay = 300;

			return cmd_empty;
		}

		if (TRUE == bBusy)
		{ 
			/*
			// Homing Time Check and TimeOut 처리.
			if (m_stCommand.cmdMotor[iAxisNo].timer.homeFail)
			{
				if (timeElapse < m_stCommand.cmdMotor[iAxisNo].timer.homeFail)
				{
					m_stCommand.cmdMotor[iAxisNo].timer.homeFail -= timeElapse;
				
					m_stSystemStatus.opStatus[iAxisNo].bitHoming = TRUE;
				}
				else
				{ // Time Over
					m_stCommand.cmdMotor[iAxisNo].timer.homeFail = 0;
					
					m_stSystemStatus.opStatus[iAxisNo].bitHoming = FALSE;
					m_stSystemStatus.opStatus[iAxisNo].bitHomingOK = FALSE;
					m_stSystemStatus.opStatus[iAxisNo].bitHomingNG = TRUE;
				}
			}
			*/

			m_stSystemStatus.opStatus[iAxisNo].bitHoming = TRUE;
		}

		/*
		if (TRUE == m_stSystemStatus.opStatus[iAxisNo].bitHomingNG ||
			TRUE == m_stSystemStatus.opStatus[iAxisNo].bitHomingOK)
		{   // 원점탐색이 성공하였을경우 모터는 정지하였지만, 로직적으로 Servo Off를 위해 Stop 부분을 진행해야한다.
			// TimeOut으로 원점 탐색이 실패하였을 경우 모터를 정지 시킨다.
			m_stSystemStatus.opStatus[iAxisNo].bitHomingNG = FALSE;
			m_stCommand.cmdMotor[iAxisNo].command.bitStop = TRUE;

			m_stCommand.cmdMotor[iAxisNo].timer.servoOffDelay = 1000;
		}
		*/

		if (m_stCommand.cmdMotor[iAxisNo].timer.servoOffDelay)
		{
			if (timeElapse < m_stCommand.cmdMotor[iAxisNo].timer.servoOnDelay)
			{
				m_stCommand.cmdMotor[iAxisNo].timer.servoOffDelay -= timeElapse;

				TRACE(_T("Motor[%d] = ServoOffDelay (%d) msec \n"), m_stCommand.cmdMotor[iAxisNo].timer.servoOffDelay);

				return cmd_empty;
			}
			else
			{ // Delay Time이 다 지났다면 Servo Off를 진행한다..
				m_stCommand.cmdMotor[iAxisNo].timer.servoOnDelay = 0;
				m_stCommand.cmdMotor[iAxisNo].command.bitServoOn = FALSE;
				m_stCommand.cmdMotor[iAxisNo].command.bitServoOff = TRUE;
			}
		}
		/*
		if (TRUE == m_stCommand.cmdMotor[iAxisNo].command.bitStop)	// 일반정지 확인
		{
			m_stCommand.cmdMotor[iAxisNo].command.bitOrigin = FALSE;
			
			m_stCommand.cmdMotor[iAxisNo].command.bitStop = FALSE;
			m_stCommand.cmdMotor[iAxisNo].command.bitJogPlus = FALSE;
			m_stCommand.cmdMotor[iAxisNo].command.bitJogMinus = FALSE;
			m_stCommand.cmdMotor[iAxisNo].command.bitOrigin = FALSE;
			m_stCommand.cmdMotor[iAxisNo].command.bitPtOn = FALSE;

			m_stCommand.cmdMotor[iAxisNo].command.bitServoOn = FALSE;
			m_stCommand.cmdMotor[iAxisNo].command.bitServoOff = TRUE;
			return cmd_normal_stop;
		}

		if (TRUE == m_stCommand.cmdMotor[iAxisNo].command.bitServoOff)	//	Servo Off
		{
			if (m_stSystemStatus.stMotorStatus[iAxisNo].wStatus.bitMovingBackward ||
				m_stSystemStatus.stMotorStatus[iAxisNo].wStatus.bitMovingForward)
			{
				return cmd_normal_stop;
			}

			m_stCommand.cmdMotor[iAxisNo].timer.homeFail = 0;
			m_stCommand.cmdMotor[iAxisNo].timer.servoOnDelay = 0;
			m_stCommand.cmdMotor[iAxisNo].timer.servoOffDelay = 0;
			m_stCommand.cmdMotor[iAxisNo].command.bitServoOff = FALSE;

			if (m_stSystemStatus.opStatus[iAxisNo].bitHomingOK == TRUE)
			{
				SendLogMessage(msgSequence, msg_homing_done, iAxisNo);
			}
			else if (m_stSystemStatus.opStatus[iAxisNo].bitHomingNG == TRUE)
			{
				SendLogMessage(msgSequence, msg_homing_fail, iAxisNo);
			}

			return cmd_disable_servo;
		}
		*/
		
		
	}

	if (m_stCommand.cmdMotor[iAxisNo].command.bitStop)	// 일반정지 확인
	{
		m_stCommand.cmdMotor[iAxisNo].command.bitStop = FALSE;
		m_stCommand.cmdMotor[iAxisNo].command.bitOrigin = FALSE;
		m_stCommand.cmdMotor[iAxisNo].command.bitJogPlus = FALSE;
		m_stCommand.cmdMotor[iAxisNo].command.bitJogMinus = FALSE;
		m_stCommand.cmdMotor[iAxisNo].command.bitOrigin = FALSE;
		m_stCommand.cmdMotor[iAxisNo].command.bitPtOn = FALSE;

		m_stCommand.cmdMotor[iAxisNo].command.bitServoOn = FALSE;
		m_stCommand.cmdMotor[iAxisNo].command.bitServoOff = TRUE;
	
		return cmd_normal_stop;
	}

	if (m_stCommand.cmdMotor[iAxisNo].command.bitServoOff)
	{
		if (m_stSystemStatus.stMotorStatus[iAxisNo].wStatus.bitMovingBackward ||
			m_stSystemStatus.stMotorStatus[iAxisNo].wStatus.bitMovingForward)
		{
			return cmd_normal_stop;
		}

		m_stCommand.cmdMotor[iAxisNo].command.bitServoOff = FALSE;

		m_stCommand.cmdMotor[iAxisNo].timer.homeFail = 0;
		m_stCommand.cmdMotor[iAxisNo].timer.servoOnDelay = 0;
		m_stCommand.cmdMotor[iAxisNo].timer.servoOffDelay = 0;
		
		return cmd_disable_servo;
	}

	
	///////////////////////////////////////////////////////
	return cmd_empty;

	//	수동 제어

	//	장치 제어 시작
	const ULONGLONG curTickCount = ::GetTickCount64();
	WORD timeElapse = static_cast<WORD>(curTickCount - m_stCommand.timeStamp[m_nMotorIndex]);
	m_stCommand.timeStamp[m_nMotorIndex] = curTickCount;

	const BOOL bBusy = m_stSystemStatus.stMotorStatus[iAxisNo].wStatus.bitMovingForward ||
		m_stSystemStatus.stMotorStatus[iAxisNo].wStatus.bitMovingBackward;

	//	Homing & Homing Result Status
	if (m_stSystemStatus.stMotorStatus[iAxisNo].wStatus.bitOriginRun && bBusy)
	{
		if (0 == m_stCommand.cmdMotor[iAxisNo].timer.homeFail)
		{
			m_stCommand.cmdMotor[iAxisNo].command.bitOrigin = FALSE;

			m_stCommand.cmdMotor[iAxisNo].timer.homeFail = static_cast<WORD>(m_stDatabaseDeviceInfo.nHomingTimeout);

			m_stSystemStatus.opStatus[iAxisNo].bitHoming = TRUE;
			m_stSystemStatus.opStatus[iAxisNo].bitHomingOK = FALSE;
			m_stSystemStatus.opStatus[iAxisNo].bitHomingNG = FALSE;
		}
	}
	else if (m_stSystemStatus.opStatus[iAxisNo].bitHoming)
	{
		m_stCommand.cmdMotor[iAxisNo].timer.homeFail = 0;

		m_stSystemStatus.opStatus[iAxisNo].bitHoming = FALSE;
		if (m_stSystemStatus.stMotorStatus[iAxisNo].wStatus.bitOriginOK)
		{
			m_stSystemStatus.opStatus[iAxisNo].bitHomingOK = TRUE;
			m_stSystemStatus.opStatus[iAxisNo].bitHomingNG = FALSE;
		}
		else
		{
			m_stSystemStatus.opStatus[iAxisNo].bitHomingOK = FALSE;
			m_stSystemStatus.opStatus[iAxisNo].bitHomingNG = TRUE;
		}
	}

	if (m_stSystemStatus.stMotorStatus[iAxisNo].wStatus.bitServoOn)
	{
		//	Servo On delay 처리
		if (timeElapse < m_stCommand.cmdMotor[iAxisNo].timer.servoOnDelay)
		{
			m_stCommand.cmdMotor[iAxisNo].timer.servoOnDelay -= timeElapse;

			return cmd_empty;
		}
		else
		{
			m_stCommand.cmdMotor[iAxisNo].timer.servoOnDelay = 0;
		}
	}
	else if (0 == m_stCommand.cmdMotor[iAxisNo].timer.servoOnDelay)
	{
		m_stCommand.cmdMotor[iAxisNo].timer.servoOnDelay = TIMER_SERVO_ON;
	}

	if (FALSE == m_stSystemStatus.wStatus.bitMonitorOn)
	{
		//	관리자 모드가 비활성화되면 기존 명령은 전부 비활성화
		for (INT nIndex = 0; nIndex < 9; nIndex++)
		{
			m_stCommand.cmdMotor[nIndex].command.dwFlag = 0x0;
		}

		if (m_stCommand.cmdGate.bitMonitoringMode)
		{
			return cmd_enter_monitoring_mode;
		}
		else
		{
			return cmd_empty;
		}
	}

#ifndef SKIP_ADMINISTRATION_MODE
	if (FALSE == m_stCommand.cmdGate.bitMonitoringMode)
	{
		BOOL bExit = TRUE;
		for (INT i = 0; i < 9; i++)
		{
			if (m_stSystemStatus.stMotorStatus[i].wStatus.bitMovingForward ||
				m_stSystemStatus.stMotorStatus[i].wStatus.bitMovingBackward)
			{
				bExit = FALSE;
				break;
			}
		}

		if (bExit)
		{
			return cmd_exit_monitoring_mode;
		}
	}
#endif

	if (m_stCommand.cmdMotor[iAxisNo].command.bitEStop)	// 긴급정지 확인
	{
		//	긴급정지 작동 순서
		//	Stop + Servo Off // Stop 이후에 Servo Off

		m_stCommand.cmdMotor[iAxisNo].command.bitEStop = FALSE;
		m_stCommand.cmdMotor[iAxisNo].command.bitJogPlus = FALSE;
		m_stCommand.cmdMotor[iAxisNo].command.bitJogMinus = FALSE;
		m_stCommand.cmdMotor[iAxisNo].command.bitOrigin = FALSE;
		m_stCommand.cmdMotor[iAxisNo].command.bitPtOn = FALSE;

		m_stCommand.cmdMotor[iAxisNo].command.bitStop = TRUE;
		m_stCommand.cmdMotor[iAxisNo].command.bitServoOn = FALSE;

		//		return cmd_emergency_stop;
	}

	if (m_stCommand.cmdMotor[iAxisNo].command.bitStop)	// 일반정지 확인
	{
		m_stCommand.cmdMotor[iAxisNo].command.bitStop = FALSE;
		m_stCommand.cmdMotor[iAxisNo].command.bitJogPlus = FALSE;
		m_stCommand.cmdMotor[iAxisNo].command.bitJogMinus = FALSE;
		m_stCommand.cmdMotor[iAxisNo].command.bitOrigin = FALSE;
		m_stCommand.cmdMotor[iAxisNo].command.bitPtOn = FALSE;

		return cmd_normal_stop;
	}

	if (m_stCommand.cmdGate.bitRescanSlave)
	{
		m_stCommand.cmdGate.bitRescanSlave = FALSE;
		return cmd_rescan_slave;
	}

	if (m_stCommand.cmdGate.bitAlarmReset)
	{
		m_stCommand.cmdGate.bitAlarmReset = FALSE;
		return cmd_alarm_reset;
	}

	if (FALSE == m_stSystemStatus.stMotorStatus[iAxisNo].wStatus.bitServoOn)
	{
		//	다른 제어 비트가 살아있다면 Servo On 시켜야 한다.
		if (m_stCommand.cmdMotor[iAxisNo].command.bitJogPlus ||
			m_stCommand.cmdMotor[iAxisNo].command.bitJogMinus ||
			m_stCommand.cmdMotor[iAxisNo].command.bitOrigin ||
			m_stCommand.cmdMotor[iAxisNo].command.bitPtOn)
		{
			m_stCommand.cmdMotor[iAxisNo].command.bitServoOn = TRUE;
		}

		if (m_stCommand.cmdMotor[iAxisNo].command.bitServoOn)
		{
			return cmd_enable_servo;;
		}

		return cmd_empty;
	}

	//	Delay 처리
	if (m_stCommand.cmdMotor[iAxisNo].command.bitServoOn)
	{
		if (bBusy)
		{
			//	Home Fail 처리
			if (m_stSystemStatus.stMotorStatus[iAxisNo].wStatus.bitOriginRun)
			{
				if (m_stCommand.cmdMotor[iAxisNo].timer.homeFail)
				{
					if (timeElapse < m_stCommand.cmdMotor[iAxisNo].timer.homeFail)
					{
						m_stCommand.cmdMotor[iAxisNo].timer.homeFail -= timeElapse;
					}
					else
					{
						m_stCommand.cmdMotor[iAxisNo].command.bitServoOn = FALSE;

						m_stSystemStatus.opStatus[iAxisNo].bitHomingOK = FALSE;
						m_stSystemStatus.opStatus[iAxisNo].bitHomingNG = TRUE;
					}
				}
			}

			//	Servo OFF Delay 처리
			m_stCommand.cmdMotor[iAxisNo].timer.servoOffDelay = TIMER_SERVO_OFF;
		}
		else
		{
			//	Servo ON
			if (m_stCommand.cmdMotor[iAxisNo].timer.servoOnDelay)
			{

			}
			else if (m_stCommand.cmdMotor[iAxisNo].timer.servoOffDelay)
			{
				//	Servo OFF Delay 처리
				if (timeElapse < m_stCommand.cmdMotor[iAxisNo].timer.servoOffDelay)
				{
					m_stCommand.cmdMotor[iAxisNo].timer.servoOffDelay -= timeElapse;
				}
				else
				{
					m_stCommand.cmdMotor[iAxisNo].timer.servoOffDelay = 0;
					m_stCommand.cmdMotor[iAxisNo].command.bitServoOn = FALSE;
				}
			}
		}
	}

	if (FALSE == m_stCommand.cmdMotor[iAxisNo].command.bitServoOn)	//	Servo Off
	{
		if (m_stSystemStatus.stMotorStatus[iAxisNo].wStatus.bitMovingBackward ||
			m_stSystemStatus.stMotorStatus[iAxisNo].wStatus.bitMovingForward)
		{
			return cmd_normal_stop;
		}

		m_stCommand.cmdMotor[iAxisNo].timer.homeFail = 0;
		m_stCommand.cmdMotor[iAxisNo].timer.servoOnDelay = 0;
		m_stCommand.cmdMotor[iAxisNo].timer.servoOffDelay = 0;

		return cmd_disable_servo;
	}

	if (FALSE == m_stCommand.cmdMotor[iAxisNo].command.bitServoOn)	// Servo Off라면 이후 제어 명령은 실행 불요
	{
		return cmd_empty;
	}

	if (bBusy)	//	기동 신호 확인
	{
		m_stCommand.cmdMotor[iAxisNo].command.bitJogPlus = FALSE;
		m_stCommand.cmdMotor[iAxisNo].command.bitJogMinus = FALSE;
		m_stCommand.cmdMotor[iAxisNo].command.bitPtOn = FALSE;
	}

	if (m_stCommand.cmdMotor[iAxisNo].command.bitJogPlus)	//	JOG_P 확인
	{
		return cmd_jog_cw;
	}

	if (m_stCommand.cmdMotor[iAxisNo].command.bitJogMinus)	//	JOG_N 확인
	{
		return cmd_jog_ccw;
	}

	if (m_stCommand.cmdMotor[iAxisNo].command.bitOrigin)	//	ORGIN 확인
	{
		return cmd_origin;
	}

	if (m_stCommand.cmdMotor[iAxisNo].command.bitPtOn)		//	PT 운전 확인
	{
		return cmd_position_table;
	}

	return cmd_empty;
}


const INT CDeviceMotionGate::FindNextMotorIndex(const INT nIndex)
{
	INT nCurIndex = nIndex;

	for (; nCurIndex < 9; nCurIndex++)
	{
		if ((m_wCurAxisConnection >> nCurIndex) & 0x1)
		{
			return nCurIndex;
		}
	}

	for (nCurIndex = 0; nCurIndex < nIndex; nCurIndex++)
	{
		if ((m_wCurAxisConnection >> nCurIndex) & 0x1)
		{
			return nCurIndex;
		}
	}

	return -1;
}


const BOOL CDeviceMotionGate::SaveCurrentParameterToDataBase(const BOOL bNominalData)
{
	CDataBaseProject* pProjectDB = CDataBaseProject::GetInstance();
	CString strSystemName(GetSystemName());	const INT nBackupID = pProjectDB->CreateBackupID(strSystemName, m_strBackupName);

	if (pProjectDB->OpenDataBase())
	{
		pProjectDB->Begin();

		if (bNominalData)
		{
			pProjectDB->UpdateNominalBackupID(strSystemName, nBackupID);
		}

		INT nMotorID = 0;
		for (;;)
		{
			INT nNextMotorID = FindNextMotorIndex(nMotorID + 1);
			if (nMotorID >= nNextMotorID)
			{
				break;
			}

			nMotorID = nNextMotorID;
			const INT nMotorIndex = nNextMotorID;

			for (INT i = 0; i < 40; i++)
			{
				const INT nIndex = i;
				pProjectDB->WriteBackupData(strSystemName, nBackupID, nMotorID, nIndex, m_stMotorInfo[nMotorIndex].lParameter[i]);
			}

			for (INT i = 0; i < 32; i++)
			{
				for (INT y = 0; y < 32; y++)
				{
					const INT nIndex = (i + 1) * 1000 + y;
					pProjectDB->WriteBackupData(strSystemName, nBackupID, nMotorID, nIndex, m_stMotorInfo[nMotorIndex].nodeItem[i].wBuffer[y]);
				}
			}
		}

		pProjectDB->Commit();
		pProjectDB->CloseDataBase();
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::LoadCurrentParameterFromDataBase()
{
	CDataBaseProject* pProjectDB = CDataBaseProject::GetInstance();

	CString strSystemName(GetSystemName());
	if (FALSE == pProjectDB->LoadMotorParameter(strSystemName, m_nBackupID) ||
		FALSE == pProjectDB->SeekToBegin())
	{
		return FALSE;
	}

	while (CDataBaseRecordData* pData = pProjectDB->SeekToNext())
	{
		INT nMotorID = -1, nParamIndex = -1, nParamValue = -1;
		if (pData->GetIntergerData(_T("Motor_ID"), nMotorID) &&
			pData->GetIntergerData(_T("Parameter_Index"), nParamIndex) &&
			pData->GetIntergerData(_T("Parameter_Value"), nParamValue))
		{
			const INT nTableIndex = nParamIndex / 1000;
			const INT nIndex = nParamIndex % 1000;

			switch (nTableIndex)
			{
			case 0:
			{
				m_stMotorInfo[nMotorID].lParameter[nIndex] = static_cast<LONG>(nParamValue);
			}	break;

			default:
			{
				m_stMotorInfo[nMotorID].nodeItem[nTableIndex - 1].wBuffer[nIndex] = static_cast<WORD>(nParamValue);
			}	break;
			}
		}
		else
		{
			return FALSE;
		}
	}

	if (pProjectDB->GetBackupName(strSystemName, m_nBackupID))
	{
		pProjectDB->SeekToBegin();
		if (CDataBaseRecordData* pData = pProjectDB->SeekToNext())
		{
			pData->GetTextData(_T("Backup_Name"), m_strBackupName);
		}
	}

	return TRUE;
}
//
//const BOOL CDeviceMotionGate::SetSimpleStatus(LPCTSTR lpszStatus)
//{
//	if (0 == _tcscmp(m_strSimpleStatus, lpszStatus))
//	{
//		return TRUE;
//	}
//
//	InsertLog(CMainFrame::msgNormal, lpszStatus);
//
//	m_mutexSimpleStatus.Lock();
//
//	const INT nStrLen = _tcslen(lpszStatus);
//
//	m_strSimpleStatus[nStrLen] = NULL;
//	_tcscpy_s(m_strSimpleStatus, MAX_PATH, lpszStatus);
//
//	m_mutexSimpleStatus.Unlock();
//
//	return TRUE;
//}
//
//const BOOL CDeviceMotionGate::SetSimpleStatus(const INT nStringID)
//{
//	CString strMessage;
//	if (FALSE == strMessage.LoadString(nStringID))
//	{
//		return FALSE;
//	}
//
//	return SetSimpleStatus(strMessage);
//}
//

//LPCTSTR CDeviceMotionGate::GetSimpleStatus()
//{
//	return m_strSimpleStatus;
//}

//
//const BOOL CDeviceMotionGate::SetDetailStatus(LPCTSTR lpszStatus, const INT nPosition, const INT nMin, const INT nMax)
//{
//	if (0 == _tcscmp(m_strDetailStatus, lpszStatus))
//	{
//		return TRUE;
//	}
//
//	InsertLog(CMainFrame::msgSequence, lpszStatus);
//
//	m_mutexDetailStatus.Lock();
//
//	const INT nStrLen = _tcslen(lpszStatus);
//
//	m_strDetailStatus[nStrLen] = NULL;
//	_tcscpy_s(m_strDetailStatus, MAX_PATH, lpszStatus);
//
//	m_mutexDetailStatus.Unlock();
//
//	m_nDetailProgressPos = nPosition;
//
//	if (nMax != nMin)
//	{
//		m_nDetailProgressMin = nMin;
//		m_nDetailProgressMax = nMax;
//	}
//
//	return TRUE;
//}
//
//const BOOL CDeviceMotionGate::SetDetailStatus(const INT nStringID, const INT nPosition, const INT nMin, const INT nMax)
//{
//	CString strMessage;
//	if (FALSE == strMessage.LoadString(nStringID))
//	{
//		return FALSE;
//	}
//
//	return this->SetDetailStatus(strMessage, nPosition, nMin, nMax);
//}
//
//LPCTSTR CDeviceMotionGate::GetDetailStatus(INT* pPos, INT* pMin, INT* pMax)
//{
//	if (pPos)
//	{
//		*pPos = m_nDetailProgressPos;
//	}
//
//	if (pMax)
//	{
//		*pMax = m_nDetailProgressMax;
//	}
//
//	if (pMin)
//	{
//		*pMin = m_nDetailProgressMin;
//	}
//
//	return m_strDetailStatus;
//}

const BOOL CDeviceMotionGate::IsMotiongateConnected()
{
	switch (m_stateMachine)
	{
	case state_stop:
	case state_connect:
	{
		return FALSE;
	}	break;
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::IsDefaultState()
{
	switch (m_stateMachine)
	{
	case state_motion_gate_get_system_status_all:
	case state_motion_gate_command:
	{
		return TRUE;
	}	break;
	}

	return FALSE;
}

const BOOL CDeviceMotionGate::IsMotiongateControllable(const BOOL bCheckGrant, const BOOL bCheckCondition)
{
	BOOL bResult = FALSE;

	switch (m_stateMachine)
	{
	case state_stop:
	case state_connect:
	{
		//	초기화 단계라서 제어 불가
	}	break;

	case state_backup:
	case state_restoration:
	case state_factory_reset:
	case state_assign_motor_id:
	case state_home_all:
	{
		// 이미 동작하고 있는 상태이므로 불가
	}	break;

	default:
	{
		if (bCheckCondition)
		{
			//	Todo : 
			//	DB와 이름 비교
			//	DB와 모터 수량 비교
			//	DB와 Timeout 비교
			//m_stSystemStatus.dwDetailStatus[m_nMotorIndex].bitErrorAll
			if (m_stConnectedDeviceInfo.strName == m_stDatabaseDeviceInfo.strName)
			{
				if (bCheckGrant)
				{
					if (m_stSystemStatus.wStatus.bitNetworkOn)
					{
						if (m_stSystemStatus.wStatus.bitMonitorGrant)
						{
							bResult = TRUE;
						}
					}
					else
					{
						bResult = TRUE;
					}
				}
				else
				{
					bResult = TRUE;
				}
			}
		}
		else if (bCheckGrant)
		{
			if (m_stSystemStatus.wStatus.bitMonitorGrant)
			{
				bResult = TRUE;
			}
		}
		else
		{
			bResult = TRUE;
		}
	}	break;
	}

	return bResult;
}


//const BOOL CDeviceMotionGate::IsDeviceNameValid()
//{
//	return m_stDatabaseDeviceInfo.strName == m_strRealName;
//}

const BOOL CDeviceMotionGate::IsDeviceInformationValid()
{
	return TRUE;
}


const BOOL CDeviceMotionGate::EnableMonitoringMode(const BOOL bEnable)
{
	m_stCommand.cmdGate.bitMonitoringMode = bEnable;

	return TRUE;
}

const BOOL CDeviceMotionGate::IsMonitoringModeEnabled()
{
	return m_stCommand.cmdGate.bitMonitoringMode;
}

const BOOL CDeviceMotionGate::IsMonitoringModeActivated()
{
	return m_stSystemStatus.wStatus.bitMonitorOn;
}


const BOOL CDeviceMotionGate::IsBareMotorConnected() // 0번 모터의 연결 유무
{
	return (m_wCurAxisConnection & 0x1) ? TRUE : FALSE;
}

const BOOL CDeviceMotionGate::IsInvalidMotorConnected()
{
	return m_stSystemStatus.btStartMotorConnectionStatus != m_stSystemStatus.btCurrentMotorConnectionStatus;
}


const BOOL CDeviceMotionGate::IsChangedMotorConnection()
{
	return (m_byMotorChange ^ m_stSystemStatus.btCurrentMotorConnectionStatus != 0);
}


const BOOL CDeviceMotionGate::IsMotorScanned(const INT nAxisNo)
{
	return (m_wScanAxisConnection >> nAxisNo) & 0x1 ? TRUE : FALSE;
}

const BOOL CDeviceMotionGate::IsMotorConnected(const INT nAxisNo)
{
	return ((m_wCurAxisConnection >> nAxisNo) & 0x01) ? TRUE : FALSE;
}


const DWORD CDeviceMotionGate::EnableServo(const DWORD dwIndex)
{
	for (INT nIndex = 0; nIndex < 9; nIndex++)
	{
		m_stCommand.cmdMotor[nIndex].command.bitServoOn = ((dwIndex >> nIndex) & 0x01) ? TRUE : FALSE;
	}

	return 0;
}


const DWORD CDeviceMotionGate::EnableServo(const INT nAxisNo, const BOOL bEnable)
{
	for (INT nIndex = 0; nIndex < 9; nIndex++)
	{
		m_stCommand.cmdMotor[nAxisNo].command.bitServoOn = bEnable ? TRUE : FALSE;
	}

	return 0;
}


const BOOL CDeviceMotionGate::IsServoEnabled(const INT nAxisNo)
{
	return m_stSystemStatus.stMotorStatus[nAxisNo].wStatus.bitServoOn;
}


const BOOL CDeviceMotionGate::JogPlus(const DWORD dwIndex, const BOOL bHighSpeed)
{
	for (INT nIndex = 0; nIndex < 9; nIndex++)
	{
		m_stCommand.cmdMotor[nIndex].command.bitJogPlus = ((dwIndex >> nIndex) & 0x01) ? TRUE : FALSE;
		m_stCommand.cmdMotor[nIndex].command.bitJogSpeedHigh = bHighSpeed;
	}

	return TRUE;
}


const BOOL CDeviceMotionGate::JogPlus(const INT nAxisNo, const BOOL bHighSpeed)
{
	m_stCommand.cmdMotor[nAxisNo].command.bitJogPlus = TRUE;
	m_stCommand.cmdMotor[nAxisNo].command.bitJogSpeedHigh = bHighSpeed;

	return TRUE;
}


const BOOL CDeviceMotionGate::JogMinus(const DWORD dwIndex, const BOOL bHighSpeed)
{
	for (INT nIndex = 0; nIndex < 9; nIndex++)
	{
		m_stCommand.cmdMotor[nIndex].command.bitJogMinus = ((dwIndex >> nIndex) & 0x01) ? TRUE : FALSE;
		m_stCommand.cmdMotor[nIndex].command.bitJogSpeedHigh = bHighSpeed;
	}

	return TRUE;
}


const BOOL CDeviceMotionGate::JogMinus(const INT nAxisNo, const BOOL bHighSpeed)
{
	for (INT nIndex = 0; nIndex < 9; nIndex++)
	{
		m_stCommand.cmdMotor[nAxisNo].command.bitJogMinus = TRUE;
		m_stCommand.cmdMotor[nAxisNo].command.bitJogSpeedHigh = bHighSpeed;
	}

	return TRUE;
}


const BOOL CDeviceMotionGate::EmergencyStopIndex(const DWORD dwIndex)
{
	for (INT nIndex = 0; nIndex < 9; nIndex++)
	{
		m_stCommand.cmdMotor[nIndex].command.bitEStop = ((dwIndex >> nIndex) & 0x01) ? TRUE : FALSE;
	}

	return TRUE;
}

const BOOL	CDeviceMotionGate::EmergencyStop(const INT nAxisNo)
{
	m_stCommand.cmdMotor[nAxisNo].command.bitEStop = TRUE;

	return TRUE;
}

const BOOL CDeviceMotionGate::MoveStopIndex(const DWORD dwIndex)
{
	for (INT nIndex = 0; nIndex < 9; nIndex++)
	{
		m_stCommand.cmdMotor[nIndex].command.bitStop = ((dwIndex >> nIndex) & 0x01) ? TRUE : FALSE;
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::MoveStop(const INT nAxisNo)
{
	
	//DoCmdMoveStop(nAxisNo);
	m_stCommand.cmdMotor[nAxisNo].command.bitStop = TRUE;
	
	//m_stCommand.cmdMotor[nAxisNo].command.bitEStop = TRUE;
	
	return TRUE;
}

const BOOL CDeviceMotionGate::ExecutePositionTableIndex(const DWORD dwIndex, const INT nTableNo)
{
	for (INT nIndex = 0; nIndex < 9; nIndex++)
	{
		BOOL bExecute = ((dwIndex >> nIndex) & 0x01) ? TRUE : FALSE;
		if (bExecute)
		{
			m_stCommand.cmdMotor[nIndex].command.btIndex = nTableNo;
			m_stCommand.cmdMotor[nIndex].command.bitPtOn = bExecute;
		}
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::ExecutePositionTablex(const INT nAxisNo, const INT nTableNo)
{
	m_stCommand.cmdMotor[nAxisNo].command.btIndex = nTableNo;
	m_stCommand.cmdMotor[nAxisNo].command.bitPtOn = TRUE;

	return TRUE;
}


const BOOL CDeviceMotionGate::ExecuteOriginSerachIndex(const DWORD dwIndex)
{
	for (INT nIndex = 0; nIndex < 9; nIndex++)
	{
		m_stCommand.cmdMotor[nIndex].command.bitOrigin = ((dwIndex >> nIndex) & 0x01) ? TRUE : FALSE;
	}

	return TRUE;
}


const BOOL CDeviceMotionGate::ExecuteOriginSerach(const INT nAxisNo)
{
	m_stCommand.cmdMotor[nAxisNo].command.bitOrigin = TRUE;
	
	return TRUE;
}


const BOOL CDeviceMotionGate::IsMotiongateAlarmEnabled()
{
	return m_stSystemStatus.wStatus.bitAlarm;
}

const BOOL CDeviceMotionGate::AlarmResetIndex(const DWORD dwIndex)
{
	for (INT nIndex = 0; nIndex < 9; nIndex++)
	{
		m_stCommand.cmdMotor[nIndex].command.bitOrigin = ((dwIndex >> nIndex) & 0x01) ? TRUE : FALSE;
	}

	return TRUE;
}


const BOOL CDeviceMotionGate::AlarmReset(const INT nAxisNo)
{
	m_stCommand.cmdMotor[nAxisNo].command.bitOrigin = TRUE;

	return TRUE;
}


const BOOL CDeviceMotionGate::AlarmReset()
{
	m_nLastFasResult = DoCmdMotionGateAlarmReset();
	if (FMM_OK != m_nLastFasResult)
	{
		AfxMessageBox(_T("Alarm Reset Fail !!"));
	}
	
	this->SendLogMessage(msgSequence, msg_motiongate_alarm_reset);
	
	return m_nLastFasResult == FMM_OK;
}

const BOOL CDeviceMotionGate::RescanSlave()
{
	m_stCommand.cmdGate.bitAlarmReset = TRUE;

	return TRUE;
}

const BOOL CDeviceMotionGate::IsHoming(const INT nAxisNo)
{
	//return m_stSystemStatus.opStatus[nAxisNo].bitHoming;
	return m_stSystemStatus.dwDetailStatus[nAxisNo].bitOriginReturning;
}

const BOOL CDeviceMotionGate::IsHomingOK(const INT nAxisNo)
{
	//return m_stSystemStatus.opStatus[nAxisNo].bitHomingOK;
	return m_stSystemStatus.dwDetailStatus[nAxisNo].bitOriginComplete;
}

const BOOL CDeviceMotionGate::IsHomingNG(const INT nAxisNo)
{
	return m_stSystemStatus.opStatus[nAxisNo].bitHomingNG;
	
	/*BOOL bSearching = m_stSystemStatus.dwDetailStatus[nAxisNo].bitOriginReturning;
	BOOL bOriginOK = m_stSystemStatus.dwDetailStatus[nAxisNo].bitOriginComplete;
	return (FALSE == bSearching) && (FALSE == bOriginOK);*/
}

const BOOL CDeviceMotionGate::StopExecution()
{
	BOOL bResult = FALSE;
	switch (m_stateMachine)
	{
	case state_backup:
	case state_restoration:
	case state_factory_reset:
	case state_assign_motor_id:
	case state_home_all:
	{
		m_stCommand.cmdGate.bitStopExecution = TRUE;
		bResult = TRUE;
	}	break;
	}

	return bResult;
}

const BOOL CDeviceMotionGate::StartBackup()
{
	if (FALSE == IsMotiongateControllable(FALSE))
	{
		return FALSE;
	}

	m_stCommand.cmdGate.bitReadParameter = TRUE;
	m_stCommand.cmdGate.bitBackup = TRUE;

	return TRUE;
}

const BOOL CDeviceMotionGate::StartReadParameter()
{
	if (FALSE == IsMotiongateControllable())
	{
		return FALSE;
	}

	m_stCommand.cmdGate.bitReadParameter = TRUE;
	m_stCommand.cmdGate.bitBackup = FALSE;

	return TRUE;
}

void CDeviceMotionGate::SetProjectName(LPCTSTR lpszProjectName)
{
	m_strProjectName = lpszProjectName;
}

LPCTSTR CDeviceMotionGate::GetProjectName()
{
	return m_strProjectName;
}

//const INT CDeviceMotionGate::ChangeName(LPCTSTR lpszName, const BYTE btOption)
//{
//	//	중복 이름을 검색하는 무언가가 필요하다.
//
//	SetProjectName(lpszName);
//
//	m_stCommand.cmdGate.index = btOption;
//	m_stCommand.cmdGate.bitChangeInformation = TRUE;
//
//	return 0;
//}


void CDeviceMotionGate::SetBackupName(LPCTSTR lpszBackupName)
{
	m_strBackupName = lpszBackupName;
}

LPCTSTR CDeviceMotionGate::GetBackupName()
{
	return m_strBackupName;
}

void CDeviceMotionGate::SetNominalBackup(const BOOL bActivate)
{
	m_stCommand.cmdGate.bitBackupNominal = bActivate;
}

const BOOL CDeviceMotionGate::IsBusy()
{
	switch (m_stateMachine)
	{
	case state_backup:
	case state_restoration:
	case state_home_all:
	case state_factory_reset:
	case state_assign_motor_id:
	{
		return TRUE;
	}	break;
	}

	return FALSE;
}

const BOOL CDeviceMotionGate::IsGrantedToStop()
{
	BOOL bResult = FALSE;

	CDataBaseConfig* pConfig = CDataBaseConfig::GetInstance();
	switch (m_stateMachine)
	{
	case state_backup:
	{
		bResult = pConfig->IsDeviceBackupGranted();
	}	break;

	case state_restoration:
	case state_home_all:
	{
		bResult = pConfig->IsDeviceRestorationGranted();
	}	break;

	case state_factory_reset:
	{
		bResult = pConfig->IsDeviceFactoryResetGranted();
	}	break;
	case state_assign_motor_id:
	{
		bResult = pConfig->IsDeviceIdAssignmentGranted();
	}	break;
	}

	return bResult;
}

void CDeviceMotionGate::SelectBackupID(const INT nBackupID)
{
	m_nBackupID = nBackupID;
}


const INT CDeviceMotionGate::GetSelectedBackupID()
{
	return m_nBackupID;
}

const BOOL CDeviceMotionGate::StartRestoration()
{
	m_stCommand.cmdGate.bitRestoration = TRUE;

	return TRUE;
}

void CDeviceMotionGate::EnableAutoHoming(const BOOL bEnable)
{
	m_bAutoHoming = bEnable;
}

const BOOL CDeviceMotionGate::IsAutoHomingEnabled()
{
	return m_bAutoHoming;
}

void CDeviceMotionGate::EnableAutoRestoration(const BOOL bEnable)
{
	if (m_bAutoRestoration != bEnable)
	{
		if (bEnable)
		{
			CheckAutoBackupSchedule();
		}

		m_bAutoRestoration = bEnable;
	}
}


void CDeviceMotionGate::ResetMotionGateAlarm()
{
	m_stCommand.cmdGate.bitMotionAlarmReset = TRUE;
}


const BOOL CDeviceMotionGate::IsAutoRestorationEnabled()
{
	return m_bAutoRestoration;
}

const BOOL CDeviceMotionGate::StartFactoryReset(const INT nMotorIndex)
{
	// 실행 조건을 나중에 여기서 한번 더 체크

	m_stCommand.cmdGate.index = static_cast<BYTE>(nMotorIndex);
	m_stCommand.cmdGate.bitFactoryReset = TRUE;

	return TRUE;
}

void CDeviceMotionGate::SelectAssignID(const INT nTargetID, const INT nSourceID)
{
	m_nAssignTargetID = nTargetID;
	m_nAssignSourceID = nSourceID;
}

const BOOL CDeviceMotionGate::StartAssignID()
{
	// 실행 조건을 나중에 여기서 한번 더 체크

	m_stCommand.cmdGate.bitAssginID = TRUE;

	return TRUE;
}

void CDeviceMotionGate::SetHomingTimeout(const INT nTime, const BOOL bSecond)
{
	const INT nMilisecondTimeout = bSecond ? nTime * 1000 : nTime;

	if (m_stConnectedDeviceInfo.nHomingTimeout != nMilisecondTimeout)
	{
		/*if (m_bAutoDatabaseUpdate)
		{
			CDataBaseProject::GetInstance()->SetHomingTimeout(m_stDatabaseDeviceInfo.strName, nMilisecondTimeout);
		}*/

		m_stConnectedDeviceInfo.nHomingTimeout = nMilisecondTimeout;
	}
}

const INT CDeviceMotionGate::GetHomingTimeout(const BOOL bSecond)
{
	return bSecond ? m_stConnectedDeviceInfo.nHomingTimeout / 1000 : m_stDatabaseDeviceInfo.nHomingTimeout;
}

const INT CDeviceMotionGate::GetDatabaseHomingTimeout(const BOOL bSecond)
{
	return bSecond ? m_stDatabaseDeviceInfo.nHomingTimeout / 1000 : m_stConnectedDeviceInfo.nHomingTimeout;
}

void CDeviceMotionGate::SetEditHomingTimeout(const INT nTime, const BOOL bSecond)
{
	m_stDatabaseEditDeviceInfo.nHomingTimeout = bSecond ? nTime * 1000 : nTime;
}

const INT CDeviceMotionGate::GetEditHomingTimeout(const BOOL bSecond)
{
	return bSecond ? m_stDatabaseEditDeviceInfo.nHomingTimeout / 1000 : m_stDatabaseEditDeviceInfo.nHomingTimeout;
}

const BOOL CDeviceMotionGate::GetAutoBackupDatetime(COleDateTime& dateTime)
{
	if (FALSE == m_bBackupReservation)
	{
		return FALSE;
	}

	dateTime = m_timeAutoBackup;

	return TRUE;
}

const INT CDeviceMotionGate::GetLastFasResult()
{
	return m_nLastFasResult;
}

const INT CDeviceMotionGate::DoCmdConnect()
{
	INT nFasResult = FMM_OK;
	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->Connect(m_stDatabaseDeviceInfo.dwAddress, 10002))
	{
		nFasResult = pInterface->GetLastError();
	}

	SendLogMessage(msgAll, _T("Connect(0x%x, 10001) : %d"), m_stDatabaseDeviceInfo.dwAddress, nFasResult);
	SetCommandTestResult(_T("DoCmdConnect()"), nFasResult);
	return nFasResult;
}


const INT CDeviceMotionGate::DoCmdMotionGateAlarmReset()
{
	INT nFasResult = FMM_OK;
	CInterface* pInterface = CInterfaceManager::GetInterface();

	if (FALSE == pInterface->SetMotionGateAlarmReset(m_stDatabaseDeviceInfo.dwAddress))
	{
		nFasResult = pInterface->GetLastError();
	}

	return nFasResult;
}


const INT CDeviceMotionGate::DoCmdClose()
{
	INT nFasResult = FMM_OK;
	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->Close(m_stDatabaseDeviceInfo.dwAddress))
	{
		nFasResult = pInterface->GetLastError();
	}

	m_stDatabaseDeviceInfo.strName.Empty();

	m_stConnectedDeviceInfo.strInformation.Empty();
	m_stConnectedDeviceInfo.strName.Empty();

	SendLogMessage(msgAll, _T("Close(0x%x, 10001) : %d"), m_stDatabaseDeviceInfo.dwAddress, nFasResult);
	SetCommandTestResult(_T("DoCmdClose()"), nFasResult);
	return nFasResult;
}

const INT CDeviceMotionGate::DoCmdScanSlave()
{
	INT nFasResult = FMM_OK;
	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->ScanSlaveFromMotiongate(m_stDatabaseDeviceInfo.dwAddress))
	{
		nFasResult = pInterface->GetLastError();
	}

	SendLogMessage(msgAll, _T("ScanSlaveFromMotiongate(0x%x) : %d"), m_stDatabaseDeviceInfo.dwAddress, m_nLastFasResult);
	SetCommandTestResult(_T("DoCmdScanSlave()"), nFasResult);

	return nFasResult;
}

const INT CDeviceMotionGate::DoCmdGetSlaveInfoQuick()
{
	const INT cbBuffer = MAX_PATH;
	CHAR strSlaveInfo[cbBuffer] = { 0, };

	BYTE btSlaveInfo = 0;

	INT nResult = FMM_OK;
	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->ScanSlave(m_stDatabaseDeviceInfo.dwAddress, m_nMotorIndex, &btSlaveInfo, strSlaveInfo, cbBuffer))
	{
		nResult = pInterface->GetLastError();
	}

	SendLogMessage(msgAll, _T("::ScanSlave(0x%x, %d, 0x%x, 0x%x, %d) : %d"), m_stDatabaseDeviceInfo.dwAddress, m_nMotorIndex, &btSlaveInfo, strSlaveInfo, cbBuffer, nResult);
	SetCommandTestResult(_T("DoCmdGetSlaveInfoQuick()"), nResult);
	return nResult;
}

const INT CDeviceMotionGate::DoCmdGetLifeCounter(const INT nMotorIndex )
{
	// 기존
	DWORD dwCounter = 0;
	INT nResult = FMM_OK;
	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (pInterface->GetLifeCounter(m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, &dwCounter))
	{
		// 기존
		m_stSystemStatus.dwLifeCount[nMotorIndex] = dwCounter;

#ifdef LIFE_COUNT_TEST
		m_stSystemStatus.dwLifeCount[m_nMotorIndex] = m_stSystemStatus.dwLifeCount[m_nMotorIndex];
#endif // LIFE_COUNT_TEST

		// Test용
		
	}
	else
	{
		nResult = pInterface->GetLastError();
	}

	SendLogMessage(msgAll, _T("::GetLifeCounter(0x%x, %x) : %d"), m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, nResult);

	return nResult;
}

const INT CDeviceMotionGate::DoCmdGetAxisStatus(const INT nMotorIndex)
{
	union
	{
		DWORD dwValue;

		struct
		{
			BYTE bitErrorAll : 1;
			BYTE bitHardwarePositiveLimit : 1;
			BYTE bitHardwareNegativeLimit : 1;
			BYTE bitSoftwarePositiveLimit : 1;
			BYTE bitSoftwareNegativeLimit : 1;
			BYTE bitReserved1 : 1;
			BYTE bitReserved2 : 1;
			BYTE bitErrorPositionOverflow : 1;
			BYTE bitErrorOverCurrent : 1;
			BYTE bitErrorOverSpeed : 1;
			BYTE bitErrorPositionTracking : 1;
			BYTE bitErrorOverLoad : 1;
			BYTE bitErrorOverHeat : 1;
			BYTE bitErrorBackEMF : 1;
			BYTE bitErrorMotorPower : 1;
			BYTE bitErrorInposition : 1;
			BYTE bitEmergencyStop : 1;
			BYTE bitSlowStop : 1;
			BYTE bitOriginReturning : 1;
			BYTE bitInposition : 1;
			BYTE bitServoOn : 1;
			BYTE bitAlarmReset : 1;
			BYTE bitPositionTableStop : 1;
			BYTE bitOriginSensor : 1;
			BYTE bitPhaseZ : 1;
			BYTE bitOriginComplete : 1;
			BYTE bitMotionDirection : 1;	// High = Directoin -
			BYTE bitMotioning : 1;
			BYTE bitMotionPause : 1;
			BYTE bitMotionAccel : 1;
			BYTE bitMotionDecel : 1;
			BYTE bitMotionConst : 1;
		};
	}	motor_status;

	motor_status.dwValue = 0;

	INT nFasResult = FMM_OK;
	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->GetAxisStatus(m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, &motor_status.dwValue))
	{
		nFasResult = pInterface->GetLastError();
	}
	SendLogMessage(msgAll, _T("::FAS_GetAxisStatus(0x%x, %d, 0x%x) : %d"), m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, &motor_status.dwValue, nFasResult);

	SetCommandTestResult(_T("DoCmdGetAxisStatus()"), nFasResult);
	
	switch (nFasResult)
	{
	/* 기존
	
	case FMM_OK:
	{
		m_stSystemStatus.stMotorStatus[nMotorIndex].wStatus.bitAlarm = motor_status.bitErrorAll;
		m_stSystemStatus.stMotorStatus[nMotorIndex].wStatus.bitInpsotion = motor_status.bitInposition;
		m_stSystemStatus.stMotorStatus[nMotorIndex].wStatus.bitMovingBackward = motor_status.bitMotionDirection && motor_status.bitMotioning;
		m_stSystemStatus.stMotorStatus[nMotorIndex].wStatus.bitMovingForward = motor_status.bitMotioning;
		m_stSystemStatus.stMotorStatus[nMotorIndex].wStatus.bitOriginOK = motor_status.bitOriginComplete;
		m_stSystemStatus.stMotorStatus[nMotorIndex].wStatus.bitOriginRun = motor_status.bitOriginReturning;
		m_stSystemStatus.stMotorStatus[nMotorIndex].wStatus.bitPtError = 0;	// 이 부분은 고민이 필요하다
		m_stSystemStatus.stMotorStatus[nMotorIndex].wStatus.bitServoOn = motor_status.bitServoOn;
		m_stSystemStatus.stMotorStatus[nMotorIndex].wStatus.bitSoftwareLimitMinus = motor_status.bitSoftwareNegativeLimit;
		m_stSystemStatus.stMotorStatus[nMotorIndex].wStatus.bitSoftwareLimitPlus = motor_status.bitSoftwarePositiveLimit;
	}	break;
	
	*/
	

	case FMM_OK:
	{
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitErrorAll= motor_status.bitErrorAll;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitHardwarePositiveLimit = motor_status.bitHardwarePositiveLimit;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitHardwareNegativeLimit = motor_status.bitHardwareNegativeLimit;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitSoftwarePositiveLimit = motor_status.bitSoftwarePositiveLimit;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitSoftwareNegativeLimit = motor_status.bitSoftwareNegativeLimit;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitReserved1 = motor_status.bitReserved1;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitReserved2 = motor_status.bitReserved2;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitErrorPositionOverflow = motor_status.bitErrorPositionOverflow;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitErrorOverCurrent = motor_status.bitErrorOverCurrent;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitErrorOverSpeed = motor_status.bitErrorOverSpeed;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitErrorPositionTracking = motor_status.bitErrorPositionTracking;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitErrorOverLoad = motor_status.bitErrorOverLoad;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitErrorOverHeat = motor_status.bitErrorOverHeat;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitErrorBackEMF = motor_status.bitErrorBackEMF;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitErrorMotorPower = motor_status.bitErrorMotorPower;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitErrorInposition = motor_status.bitErrorInposition;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitEmergencyStop = motor_status.bitEmergencyStop;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitSlowStop = motor_status.bitSlowStop;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitOriginReturning = motor_status.bitOriginReturning;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitInposition = motor_status.bitInposition;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitServoOn = motor_status.bitServoOn;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitAlarmReset = motor_status.bitAlarmReset;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitPositionTableStop = motor_status.bitPositionTableStop;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitOriginSensor = motor_status.bitOriginSensor;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitPhaseZ = motor_status.bitPhaseZ;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitOriginComplete = motor_status.bitOriginComplete;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitMotionDirection = motor_status.bitMotionDirection;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitMotioning = motor_status.bitMotioning;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitMotionPause = motor_status.bitMotionPause;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitMotionAccel = motor_status.bitMotionAccel;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitMotionDecel = motor_status.bitMotionDecel;
		m_stSystemStatus.dwDetailStatus[nMotorIndex].bitMotionConst = motor_status.bitMotionConst;
		
		/*if (1 == motor_status.bitErrorAll)
		{
			TRACE(_T("Motor Error!!\n"));
		}
		else
		{
			TRACE(_T("Motor Not Error!!\n"));
		}*/

		//TRACE(_T("Motor[%d] / Motioning.. (%d) \r\n"), nMotorIndex, motor_status.bitMotioning);
		//TRACE(_T("Motor[%d] / Alarm.. (%d) \r\n"), nMotorIndex, motor_status.bitErrorAll);
	}	break;
	}

	return nFasResult;
}

//const INT CDeviceMotionGate::DoCmdGetStatusAll(const INT nMotorIndex)
//{
//	ULONGLONG inStatus = 0;
//	DWORD outStatus = 0;
//	DWORD axisStatus = 0;
//	LONG lCmdPos = 0, lActPos = 0, lPosErr = 0, lActVel = 0;
//	WORD posItemNo = 0;
//
//	const INT nFasResult = ::FAS_GetAllStatus(m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, &inStatus, &outStatus, &axisStatus, &lCmdPos, &lActPos, &lPosErr, &lActVel, &posItemNo);
//	SendLogMessage(msgAll, _T("::FAS_GetAllStatus(0x%x, %d, 0x%x, 0x%x, 0x%x, %d, %d, %d, %d, %d) : %d"), m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, inStatus, outStatus, axisStatus, lCmdPos, lActPos, lPosErr, lActVel, posItemNo, nFasResult);
//
//	switch (nFasResult)
//	{
//	case FMM_OK:
//	{
//
//	}	break;
//	}
//
//	return nFasResult;
//}

void CDeviceMotionGate::SetConnectionStatus(const BOOL bConnected)
{
	if (m_bConnected != bConnected)
	{
		CString strMessage;
		if (bConnected)
		{

		}
		else
		{
			DoCmdClose();

			m_stateMachine = state_connect;
			m_nOpCode = opCode_disconenct;
		}

		m_bConnected = bConnected;
	}
}

BOOL CDeviceMotionGate::GetMotionGateConnectedStatus()
{
	return m_bConnected;
}

const INT CDeviceMotionGate::DoCmdGetMotiongateSystemStatus(const INT nMotorIndex)
{
	switch (m_stateMachine)
	{
	case state_restoration:			break;
	case state_factory_reset:		break;
	case state_assign_motor_id:		break;
	case state_home_all:			break;
	case state_change_information:	break;
	case state_backup:				break;
	default:		
	{
		if (FALSE == IsAutoHomingEnabled())
		{ // 일반 Homing때 자꾸 거친다.
			SendLogMessage(msgSequence, msg_motiongate_read_status);
		}
		
	}	break;
		
	}

	
	SYSTEM_STATUS stSystem = { 0 };

	INT nFasResult = FMM_OK;
	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->GetMotiongateStatus(m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, reinterpret_cast<PBYTE>(&stSystem), sizeof(SYSTEM_STATUS)))
	{
		nFasResult = pInterface->GetLastError();
	}
	
	SetCommandTestResult(_T("DoCmdGetMotiongateSystemStatus()"), nFasResult);
	SendLogMessage(msgAll, _T("GetMotiongateStatus(0x%x, %d, 0x%x, %d) : %d"), m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, reinterpret_cast<PBYTE>(&stSystem), sizeof(SYSTEM_STATUS), nFasResult);

	
	switch (nFasResult)
	{
	case FMM_OK:
	{
		
		memcpy(&m_stSystemStatus.stMotorStatus[nMotorIndex], &stSystem.stMotorStatus, sizeof(AXIS_INFORMAION));
		
		BOOL bErrorDected;
		if (m_stSystemStatus.stMotorStatus[nMotorIndex].btErrorCode != 0)
		{
			bErrorDected = TRUE;
		}
		
		
		//////////////////////////
		
		if (m_wCurAxisOrganization != 0)
		{
			if (FMM_OK == nFasResult)
			{
				nFasResult = DoCmdGetAxisStatus(nMotorIndex);
				switch (nFasResult)
				{
				case FMM_OK:
				{
					/*CString str = GetSystemName();

					m_stSystemStatus.stMotorStatus[nMotorIndex].btAlarmCode = byStatus;*/

				}	break;

				case FMC_DISCONNECTED:
				case FMC_NO_RESPONSE:
				{
					SetConnectionStatus(FALSE);
				}	break;

				default:
				{

				}	break;
				}
			}

			if (FMM_OK == nFasResult)
			{
				nFasResult = DoCmdGetAlarmType(m_stDatabaseDeviceInfo.dwAddress, nMotorIndex);

				switch (nFasResult)
				{
				case FMM_OK:
				{

				}	break;

				case FMC_DISCONNECTED:
				case FMC_NO_RESPONSE:
				{
					SetConnectionStatus(FALSE);
				}	break;

				default:
				{

				}	break;
				}
			}
		}
		

		if (stSystem.wStatus.bitAlarm)
		{
			nFasResult = DoCmdGetMotionGateAlarmStatus(m_stDatabaseDeviceInfo.dwAddress);
			switch (nFasResult)
			{
			case FMM_OK:
			{
				
			}	break;

			case FMC_DISCONNECTED:
			case FMC_NO_RESPONSE:
			{
				SetConnectionStatus(FALSE);
			}	break;

			default:
			{

			}	break;
			}
		}

		/////////////////////////
		
		
		if (m_stSystemStatus.wStatus.wValue != stSystem.wStatus.wValue ||
			m_stSystemStatus.btStartMotorConnectionStatus != stSystem.btStartMotorConnectionStatus ||
			m_stSystemStatus.btCurrentMotorConnectionStatus != stSystem.btCurrentMotorConnectionStatus)
		{
		
			if (m_wCurAxisOrganization != 0)
			{
				BYTE btPrevConnectionStatus = stSystem.btCurrentMotorConnectionStatus;
				BYTE btCurrentConnectionStatus = m_stSystemStatus.btCurrentMotorConnectionStatus;
				
				if (btPrevConnectionStatus != btCurrentConnectionStatus)
				{
					BOOL bError = FALSE;
					for (INT nIndex = 0; nIndex < 8; nIndex++)
					{ // 상태 변화 감지..
						BOOL bChange = (((btPrevConnectionStatus ^ btCurrentConnectionStatus) >> nIndex) & 0x01);
						if (bChange)
						{
							if ((btCurrentConnectionStatus >> nIndex) & 0x01)
							{
								bError = TRUE;
							}
						}

						SetStatusNoResponseMotor(nIndex + 1, bError);
					}
				}
			}
			else
			{
				SendLogMessage(msgError, msg_motiongate_no_motor_response);
			}
								
			m_stSystemStatus.wStatus.wValue = stSystem.wStatus.wValue;
			m_stSystemStatus.btStartMotorConnectionStatus = stSystem.btStartMotorConnectionStatus;
			m_stSystemStatus.btCurrentMotorConnectionStatus = stSystem.btCurrentMotorConnectionStatus;
					
		}
	}	break;

	case FMC_DISCONNECTED:
	case FMC_NO_RESPONSE:
	{
		SetConnectionStatus(FALSE);
	}	break;

	default:
	{

	}	break;
	}

	return nFasResult;
}

const INT CDeviceMotionGate::DoCmdResetMotiongateAlarm()
{
	INT nResult = FMM_OK;

	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->ResetMotionGateAlarm(m_stDatabaseDeviceInfo.dwAddress))
	{
		nResult = pInterface->GetLastError();
	}
	SetCommandTestResult(_T("DoCmdResetMotiongateAlarm()"), nResult);
	SendLogMessage(msgAll, _T("ResetMotionGateAlarm(0x%x) : %d"), m_stDatabaseDeviceInfo.dwAddress, m_nLastFasResult);

	return nResult;
}

const INT CDeviceMotionGate::GetMotorsTotalStatus(const INT nMotorIndex)
{
	switch (this->GetStatus())
	{
	case state_stop:
	{
		return motor_status_unknown;
	}	break;

	case state_connect:
	{
		INT nResult = motor_status_unknown;
		switch (m_nOpCode)
		{
		case opCode_scan_device:
		{
			nResult = motor_status_initialize;
		}	break;
		}

		return nResult;
	}	break;

	case CDeviceMotionGate::state_motion_gate_get_system_status_all:
	case CDeviceMotionGate::state_motion_gate_command:
	{
		if (0 == nMotorIndex)
		{
			return motor_status_caution;
		}
		else
		{
			if (FALSE == IsMotorConnected(nMotorIndex))
			{
				return motor_status_unknown;
			}

			else if (m_stSystemStatus.dwDetailStatus[nMotorIndex].bitErrorAll)
			{
				return motor_status_error;
			}

			else if (m_stSystemStatus.dwDetailStatus[nMotorIndex].bitOriginReturning)
			{
				return motor_status_caution;
			}

			/*else if (m_stSystemStatus.stMotorStatus[nMotorIndex].wStatus.bitAlarm)
			{
				return motor_status_error;
			}

			else if (m_stSystemStatus.opStatus[nMotorIndex].bitHoming)
			{
				return motor_status_caution;
			}

			else if (m_stSystemStatus.opStatus[nMotorIndex].bitHomingNG)
			{
				return motor_status_error;
			}*/
		
		}
	}	break;

	case CDeviceMotionGate::state_backup:
	case CDeviceMotionGate::state_restoration:
	case CDeviceMotionGate::state_factory_reset:
	case CDeviceMotionGate::state_assign_motor_id:
	case CDeviceMotionGate::state_home_all:
	{
		return motor_status_caution;
	}	break;
	}

	return motor_status_ok;
}

const INT CDeviceMotionGate::DoCmdMoveStop(const INT nMotorIndex)
{
	INT nFasResult = FMM_OK;
	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->MoveStop(m_stDatabaseDeviceInfo.dwAddress, nMotorIndex))
	{
		nFasResult = pInterface->GetLastError();
	}
	SetCommandTestResult(_T("DoCmdMoveStop()"), nFasResult);
	SendLogMessage(msgAll, _T("::FAS_MoveStop(0x%x, %d) : %d"), m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, nFasResult);

	return nFasResult;
}

const INT CDeviceMotionGate::DoCmdEnableServo(const INT nMotorIndex, const BOOL bEnable)
{
	INT nFasResult = FMM_OK;
	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->EnableServo(m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, bEnable))
	{
		nFasResult = pInterface->GetLastError();
	}

	SendLogMessage(msgAll, _T("ServoEnable(0x%x, %d, %d) : %d"), m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, bEnable, nFasResult);
	SetCommandTestResult(_T("DoCmdEnableServo()"), nFasResult);
	return nFasResult;
}

const INT CDeviceMotionGate::DoCmdMoveVelocity(const INT nMotorIndex, const INT nSpeed, const BOOL bIncDirection)
{
	INT nFasResult = FMM_OK;
	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->MoveVelocity(m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, nSpeed, bIncDirection))
	{
		nFasResult = pInterface->GetLastError();
	}
	SetCommandTestResult(_T("DoCmdMoveVelocity()"), nFasResult);
	SendLogMessage(msgAll, _T("MoveVelocity(0x%x, %d, %d, %d) : %d"), m_stDatabaseDeviceInfo.dwAddress, m_nMotorIndex, nSpeed, TRUE, nFasResult);

	return nFasResult;
}

const INT CDeviceMotionGate::DoCmdEnableAdministrationMode(const BOOL bEnable)
{
	switch (m_stateMachine)
	{
	case state_restoration:			break;
	case state_factory_reset:		break;
	case state_assign_motor_id:		break;
	case state_home_all:			break;
	case state_change_information:	break;
	default:						SendLogMessage(msgSequence, msg_motiongate_enter_admin);
	}

	INT nFasResult = FMM_OK;
	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->EnableAdministrationMode(m_stDatabaseDeviceInfo.dwAddress, bEnable))
	{
		nFasResult = pInterface->GetLastError();
	}
	SetCommandTestResult(_T("DoCmdEnableAdministrationMode()"), nFasResult);
	SendLogMessage(msgAll, _T("EnterAdministrationMode(0x%x, %d) : %d"), m_stDatabaseDeviceInfo.dwAddress, bEnable, nFasResult);

	return nFasResult;
}

const INT CDeviceMotionGate::DoCmdChangeMotiongateInformation()
{
	switch (m_stateMachine)
	{
	case state_restoration:			break;
	case state_factory_reset:		break;
	case state_assign_motor_id:		break;
	case state_home_all:			break;
	default:						SendLogMessage(msgSequence, msg_motiongate_change_system_info);
	}

	INT nFasResult = FMM_OK;
	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->ChangeMotiongateInfo(
		m_stDatabaseDeviceInfo.dwAddress,
		m_stDatabaseDeviceInfo.strName,
		m_stDatabaseDeviceInfo.nMotorCount,
		m_stDatabaseDeviceInfo.nHomingTimeout / 1000)
		)
	{
		nFasResult = pInterface->GetLastError();
	}
	
	SetCommandTestResult(_T("DoCmdChangeMotiongateInformation()"), nFasResult);
	
	SendLogMessage(msgAll, _T("ChangeMotiongateInfo(0x%x, %s, %d, %d) : %d"),
		m_stDatabaseDeviceInfo.dwAddress,
		m_stDatabaseDeviceInfo.strName,
		m_stDatabaseDeviceInfo.nMotorCount,
		m_stDatabaseDeviceInfo.nHomingTimeout / 1000);

	return nFasResult;
}
//
//const INT CDeviceMotionGate::DoCmdChangeMotiongateName(LPCTSTR lpszDeviceName)
//{
//	switch (m_stateMachine)
//	{
//	case state_restoration:			break;
//	case state_factory_reset:		break;
//	case state_assign_motor_id:		break;
//	case state_home_all:			break;
//	default:						SendLogMessage(msgSequence, msg_motiongate_change_system_info);
//	}
//
//	INT nFasResult = FMM_OK;
//	CInterface* pInterface = CInterfaceManager::GetInterface();
//	if (FALSE == pInterface->ChangeMotiongateName(m_stDatabaseDeviceInfo.dwAddress, lpszDeviceName))
//	{
//		nFasResult = pInterface->GetLastError();
//	}
//
//	SendLogMessage(msgAll, _T("ChangeMotiongateName(0x%x, %s) : %d"), m_stDatabaseDeviceInfo.dwAddress, lpszDeviceName, nFasResult);
//
//	return nFasResult;
//}

const INT CDeviceMotionGate::DoCmdReadMotiongateInfo()
{
	switch (m_stateMachine)
	{
		//	Reboot
	case state_factory_reset:		SendLogMessage(msgInternal, msg_motiongate_reconnecting); break;
	case state_assign_motor_id:		SendLogMessage(msgInternal, msg_motiongate_reconnecting); break;
		//	Nop
	case state_restoration:			break;
	case state_home_all:			break;
	case state_change_information:	break;
		//	Default
	default:
	{
		switch(CDataBaseConfig::GetInstance()->GetInterface())
		{
		case CDataBaseConfig::interface_Serial:	break;
		default:								SendLogMessage(msgSequence, msg_motiongate_read_info);	break;
		}
	}	break;
	}

	MOTIONGATE_READ_INFO stMotiongate;
	const INT cbBuffer = sizeof(MOTIONGATE_READ_INFO);
	::ZeroMemory(&stMotiongate, cbBuffer);

	INT nFasResult = FMM_OK;
	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->GetMotiongateInfo(m_stDatabaseDeviceInfo.dwAddress, &stMotiongate, cbBuffer))
	{
		nFasResult = pInterface->GetLastError();
	}

	SendLogMessage(msgAll, _T("GetMotiongateInfo(0x%x, %08x, %d) : %d"), m_stDatabaseDeviceInfo.dwAddress, &stMotiongate, cbBuffer, nFasResult);
	
	SetCommandTestResult(_T("DoCmdReadMotiongateInfo()"), nFasResult);

	switch (nFasResult)
	{
	case FMM_OK:
	{
		if (1)
		{
			const INT nStrLength = 40;
			CHAR chText[nStrLength + 1];	memcpy(chText, stMotiongate.chMotiongateInfomation, nStrLength);
			chText[nStrLength] = '\0';
			
			m_stConnectedDeviceInfo.strInformation = CStringA(chText);
		}

		if (1)
		{
			const INT nStrLength = 20;
			CHAR chText[nStrLength + 1];	memcpy(chText, stMotiongate.chMotiongateName, nStrLength);
			chText[nStrLength] = '\0';
			
			m_stConnectedDeviceInfo.strName = CStringA(chText);
			
			SetProjectName(m_stConnectedDeviceInfo.strName);
		}
		
		m_stConnectedDeviceInfo.nMotorCount		= stMotiongate.btMotorCount;
		m_stConnectedDeviceInfo.nHomingTimeout	= static_cast<INT>(stMotiongate.btHomingTimeout) * 1000;

		if (m_stConnectedDeviceInfo.strName.GetLength())
		{	//	Device 정보를 Load한다.
			CDataBaseProject* pProjectData = CDataBaseProject::GetInstance();
			if (pProjectData->OpenDataBase())
			{
				BOOL bLoadDatabase = FALSE;
				if (pProjectData->GetDeviceList(m_stConnectedDeviceInfo.strName)) // db파일(Project Table)에 해당 프로젝트 이름이 존재하는지 확인.
				{
					bLoadDatabase = LoadFromDatabase(pProjectData->SeekToNext());
					// db 파일에 원하는 값이 한개라도 없다면 false를 반환받는다.
				}

				pProjectData->CloseDataBase();

				// KKB, 현재 디바이스에 연결된 ID를 지정한다.
				if (1)
				{
					CDeviceList* pList = CDeviceList::GetInstance();
					m_stConnectedDeviceInfo.nDeviceID = m_stDatabaseDeviceInfo.nDeviceID; // db에서 읽어온 ID를 m_stConnectedDeviceInfo에 저장.
					pList->RemoveDeviceIDKey(m_stDatabaseDeviceInfo.dwAddress);
					pList->AddDeviceID(m_stDatabaseDeviceInfo.dwAddress, m_stDatabaseDeviceInfo.nDeviceID);
				}

				if (bLoadDatabase)
				{
					//	DB에는 이름이 등록되어 있는데 모터 수량과 원점 탐색 결과가 비어있으면
					if (0 == m_stConnectedDeviceInfo.nMotorCount)
					{
						m_stConnectedDeviceInfo.nMotorCount = -1;
					}

					if (0 == m_stConnectedDeviceInfo.nHomingTimeout)
					{
						m_stConnectedDeviceInfo.nHomingTimeout = -1;
					}
				}
				else
				{
					const INT nPriority = pProjectData->GetMaxPriority() + 1;
					m_stDatabaseDeviceInfo.strName = m_stConnectedDeviceInfo.strName;
					

					pProjectData->InsertProject(
						m_stDatabaseDeviceInfo.strName,
						m_stDatabaseDeviceInfo.nDeviceID,
						nPriority,
						m_stDatabaseDeviceInfo.nMotorCount,
						m_stDatabaseDeviceInfo.nHomingTimeout,
						m_stDatabaseDeviceInfo.nNominalID,
						m_stDatabaseDeviceInfo.nRestoredID,
						NULL);
				}
			}
		}
		else
		{	//	공장초기화 상태의 Device

		}

	}	break;
	}

	return nFasResult;
}

const INT CDeviceMotionGate::DoCmdSetParameter(const INT nMotorIndex, const INT nParameterIndex, const LONG lValue)
{
	INT nFasResult = FMM_OK;
	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->SetParameter(m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, nParameterIndex, lValue))
	{
		nFasResult = pInterface->GetLastError();
	}

	SendLogMessage(msgSequence, msg_write_parameter, nMotorIndex);
	SendLogMessage(msgAll, _T("SerParameter(0x%x, %d, %d, %d) : %d"), m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, m_nParameterIndex, lValue, nFasResult);
	
	SetCommandTestResult(_T("DoCmdSetParameter()"), nFasResult);
	
	return nFasResult;
}

const INT CDeviceMotionGate::DoCmdGetParameter(const INT nMotorIndex, const INT nParameterIndex, LONG* pValue)
{
	INT nFasResult = FMM_OK;
	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->GetParameter(m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, nParameterIndex, pValue))
	{
		nFasResult = pInterface->GetLastError();
	}

	SendLogMessage(msgSequence, msg_read_parameter, nMotorIndex);
	SendLogMessage(msgAll, _T("::FAS_GetParameter(0x%x, %d, %d, %d) : %d"), m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, nParameterIndex, *pValue, nFasResult);
	
	SetCommandTestResult(_T("DoCmdGetParameter()"), nFasResult);
	
	return nFasResult;
}

const INT CDeviceMotionGate::DoCmdGetSavedParameter(const INT nMotorIndex, const INT nParameterIndex, LONG* pValue)
{
	INT nFasResult = FMM_OK;
	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->GetFlashParameter(m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, nParameterIndex, pValue))
	{
		nFasResult = pInterface->GetLastError();
	}

	SendLogMessage(msgSequence, msg_compare_parameter, nMotorIndex);
	SendLogMessage(msgAll, _T("::FAS_GetROMParameter(0x%x, %d, %d, %d) : %d"), m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, nParameterIndex, *pValue, nFasResult);
	SetCommandTestResult(_T("DoCmdGetSavedParameter()"), nFasResult);
	return nFasResult;
}

const INT CDeviceMotionGate::DoCmdExecutePositionTable(const INT nMotorIndex, const INT nTableIndex)
{
	INT nFasResult = FMM_OK;
	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->ExecutePositionTable(m_stDatabaseDeviceInfo.dwAddress, m_nMotorIndex, nTableIndex))
	{
		nFasResult = pInterface->GetLastError();
	}

	SendLogMessage(msgAll, _T("::FAS_PosTableRunItem(0x%x, %d, %d) : %d"), m_stDatabaseDeviceInfo.dwAddress, m_nMotorIndex, m_stCommand.cmdMotor[m_nMotorIndex].command.btIndex, m_nLastFasResult);
	SetCommandTestResult(_T("DoCmdExecutePositionTable()"), nFasResult);
	return nFasResult;
}

const INT CDeviceMotionGate::DoCmdGetPositionTableItem(const INT nMotorIndex, const INT nTableIndex, ITEM_NODE* pItem)
{
	INT nFasResult = FMM_OK;
	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->GetPositionTableItem(m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, nTableIndex, pItem, sizeof(ITEM_NODE)))
	{
		nFasResult = pInterface->GetLastError();
	}

	SendLogMessage(msgSequence, msg_read_position, nMotorIndex);
	SendLogMessage(msgAll, _T("GetPositionTableItem(0x%x, %d, %d, 0x%x) : %d"), m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, nTableIndex, &pItem, nFasResult);
	SetCommandTestResult(_T("DoCmdGetPositionTableItem()"), nFasResult);
	return nFasResult;
}

const INT CDeviceMotionGate::DoCmdSetPositionTableItem(const INT nMotorIndex, const INT nTableIndex, ITEM_NODE* pItem)
{
	INT nFasResult = FMM_OK;
	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->SetPositionTableItem(m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, nTableIndex, pItem, sizeof(ITEM_NODE)))
	{
		nFasResult = pInterface->GetLastError();
	}

	SendLogMessage(msgSequence, msg_write_position, nMotorIndex);
	SendLogMessage(msgAll, _T("::FAS_PosTableWriteItem(0x%x, %d, %d, 0x%x) : %d"), m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, nTableIndex, pItem, nFasResult);
	SetCommandTestResult(_T("DoCmdSetPositionTableItem()"), nFasResult);
	return nFasResult;
}

const INT CDeviceMotionGate::DoCmdComparePositionTableItem(const INT nMotorIndex, const INT nTableIndex, ITEM_NODE* pItem)
{
	ITEM_NODE item = { 0, };

	INT nFasResult = FMM_OK;
	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->GetPositionTableItem(m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, nTableIndex, &item, sizeof(ITEM_NODE)))
	{
		nFasResult = pInterface->GetLastError();
	}

	SendLogMessage(msgSequence, msg_compare_position, nMotorIndex);
	SendLogMessage(msgAll, _T("::FAS_PosTableReadItem(0x%x, %d, %d, 0x%x) : %d"), m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, nTableIndex, &pItem, nFasResult);
	SetCommandTestResult(_T("DoCmdComparePositionTableItem()"), nFasResult);
	switch (nFasResult)
	{
	case FMM_OK:
	{
		if (item.lPosition != pItem->lPosition ||
			item.dwStartSpd != pItem->dwStartSpd ||
			item.dwMoveSpd != pItem->dwMoveSpd ||
			item.wAccelRate != pItem->wAccelRate ||
			item.wDecelRate != pItem->wDecelRate ||
			item.wCommand != pItem->wCommand ||
			item.wWaitTime != pItem->wWaitTime ||
			item.wContinuous != pItem->wContinuous ||
			item.wBranch != pItem->wBranch ||
			item.wCond_branch0 != pItem->wCond_branch0 ||
			item.wCond_branch1 != pItem->wCond_branch1 ||
			item.wCond_branch2 != pItem->wCond_branch2 ||
			item.wLoopCount != pItem->wLoopCount ||
			item.wBranchAfterLoop != pItem->wBranchAfterLoop ||
			item.wPTSet != pItem->wPTSet ||
			item.wLoopCountCLR != pItem->wLoopCountCLR ||
			item.bCheckInpos != pItem->bCheckInpos)
		{
			nFasResult = FMC_DATA_COMPARE_ERROR;
		}
	}	break;
	}

	return nFasResult;
}

const INT CDeviceMotionGate::DoCmdSaveAllParameters(const INT nMotorIndex)
{
	INT nFasResult = FMM_OK;
	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->SaveAllParameters(m_stDatabaseDeviceInfo.dwAddress, nMotorIndex))
	{
		nFasResult = pInterface->GetLastError();
	}

	SendLogMessage(msgSequence, msg_save_parameter, nMotorIndex);
	SendLogMessage(msgAll, _T("SaveAllParameters(0x%x, %d) : %d"), m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, nFasResult);
	SetCommandTestResult(_T("DoCmdSaveAllParameters()"), nFasResult);
	return nFasResult;
}

const INT CDeviceMotionGate::DoCmdSavePositionTableItems(const INT nMotorIndex)
{
	INT nFasResult = FMM_OK;
	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->SavePositionTableItems(m_stDatabaseDeviceInfo.dwAddress, nMotorIndex))
	{
		nFasResult = pInterface->GetLastError();
	}

	SendLogMessage(msgSequence, msg_save_position, nMotorIndex);
	SendLogMessage(msgAll, _T("SavePositionTables(0x%x, %d) : %d"), m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, nFasResult);
	SetCommandTestResult(_T("DoCmdSavePositionTableItems()"), nFasResult);
	return nFasResult;
}

const INT CDeviceMotionGate::DoCmdLoadPositionTableItems(const INT nMotorIndex)
{
	INT nFasResult = FMM_OK;
	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->LoadPositionTableItems(m_stDatabaseDeviceInfo.dwAddress, nMotorIndex))
	{
		nFasResult = pInterface->GetLastError();
	}

	SendLogMessage(msgSequence, msg_load_position, nMotorIndex);
	SendLogMessage(msgAll, _T("::LoadPositionTableItems(0x%x, %d) : %d"), m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, nFasResult);
	SetCommandTestResult(_T("DoCmdLoadPositionTableItems()"), nFasResult);
	return nFasResult;
}

const INT CDeviceMotionGate::DoCmdChangeSlaveID(const INT nCurMotorIndex, const INT nNewMotorIndex)
{
	INT nFasResult = FMM_OK;
	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->ChangeSlaveID(m_stDatabaseDeviceInfo.dwAddress, nCurMotorIndex, nNewMotorIndex))
	{
		nFasResult = pInterface->GetLastError();
	}

	SendLogMessage(msgSequence, msg_change_motor_id);
	SendLogMessage(msgAll, _T("::FAS_ChangeSlaveID(0x%x, %d, %d) : %d"), m_stDatabaseDeviceInfo.dwAddress, nCurMotorIndex, nNewMotorIndex, nFasResult);
	SetCommandTestResult(_T("DoCmdChangeSlaveID()"), nFasResult);
	return nFasResult;
}

const INT CDeviceMotionGate::DoCmdRebootMotiongate()
{
	INT nFasResult = FMM_OK;
	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->RebootMotiongate(m_stDatabaseDeviceInfo.dwAddress))
	{
		nFasResult = pInterface->GetLastError();
	}

	SendLogMessage(msgSequence, msg_motiongate_reboot);
	SendLogMessage(msgAll, _T("::FAS_RebootMotionGate(0x%x) : %d"), m_stDatabaseDeviceInfo.dwAddress, nFasResult);

	SetCommandTestResult(_T("DoCmdRebootMotiongate()"), nFasResult);
	
	return nFasResult;
}

const INT CDeviceMotionGate::DoCmdSearchOrigin(const INT nMotorIndex)
{
	INT nFasResult = FMM_OK;

	Sleep(10);

	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->SearchOriginSingle(m_stDatabaseDeviceInfo.dwAddress, nMotorIndex))
	{
		nFasResult = pInterface->GetLastError();
	}
	else
	{
		//SendLogMessage(msgSequence, msg_homing_start);
	}

	SendLogMessage(msgAll, _T("::FAS_MoveOriginSingleAxis(0x%x, %d) : %d"), m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, nFasResult);
	SetCommandTestResult(_T("DoCmdSearchOrigin()"), nFasResult);
	return nFasResult;
}

const INT CDeviceMotionGate::DoCmdNotifyBackupDone()
{
	INT nFasResult = FMM_OK;

	WORD wValue = 1000;

	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->NotifyBackupDone(m_stDatabaseDeviceInfo.dwAddress, wValue))
	{
		nFasResult = pInterface->GetLastError();
	}
	else
	{
		//	SendLogMessage(msgSequence, msg_homing_start);
	}
	SetCommandTestResult(_T("DoCmdNotifyBackupDone()"), nFasResult);
	SendLogMessage(msgAll, _T("::FAS_NofityBackupDone(0x%x, %d) : %d"), m_stDatabaseDeviceInfo.dwAddress, wValue, nFasResult);

	return nFasResult;
}

//
//void CDeviceMotionGate::SetMotiongateCommandOption(const BYTE btOption)
//{
//	m_stCommand.cmdGate.index = btOption;
//}
//
//const BYTE CDeviceMotionGate::GetMotiongateCommandOption()
//{
//	return m_stCommand.cmdGate.index;
//}

void CDeviceMotionGate::BuildSimpleStatus(const INT nMsgID, const LPARAM lParam)
{
	//CString strStatus;

	//switch (nMsgID)
	//{
	//case msg_monitoring_start:			strStatus.LoadString(IDS_DEVICE_MONITORING_START);		break;
	//case msg_monitoring_stop:			strStatus.LoadString(IDS_DEVICE_MONITORING_STOP);		break;
	//case msg_motiongate_connected:		strStatus.LoadString(IDS_DEVICE_CONNECTED);				break;
	//case msg_motiongate_disconnected:	strStatus.LoadString(IDS_DEVICE_DISCONNECTED);			break;
	//case msg_motiongate_no_response:	strStatus.LoadString(IDS_DEVICE_NO_RESPONSE);			break;
	//case msg_motiongate_reboot:			strStatus.LoadString(IDS_DEVICE_REBOOT);				break;
	//case msg_motiongate_read_info:		strStatus.LoadString(IDS_DEVICE_INFORMATION);			break;
	//case msg_scan_slave_progress:		strStatus.LoadString(IDS_DEVICE_SCAN);					break;
	//case msg_backup_start:				strStatus.LoadString(IDS_DEVICE_BACKUP_START);			break;
	//case msg_backup_done:				strStatus.LoadString(IDS_DEVICE_BACKUP_DONE);			break;
	//case msg_backup_fail:				strStatus.LoadString(IDS_DEVICE_BACKUP_FAIL);			break;
	//case msg_backup_stop:				strStatus.LoadString(IDS_DEVICE_BACKUP_STOP);			break;
	//case msg_restoration_start:			strStatus.LoadString(IDS_DEVICE_RESTORATION_START);		break;
	//case msg_restoration_done:			strStatus.LoadString(IDS_DEVICE_RESTORATION_DONE);		break;
	//case msg_restoration_fail:			strStatus.LoadString(IDS_DEVICE_RESTORATION_FAIL);		break;
	//case msg_restoration_stop:			strStatus.LoadString(IDS_DEVICE_RESTORATION_STOP);		break;
	//case msg_factory_reset_start:		strStatus.LoadString(IDS_DEVICE_RESET_START);			break;
	//case msg_factory_reset_done:		strStatus.LoadString(IDS_DEVICE_RESET_DONE);			break;
	//case msg_factory_reset_fail:		strStatus.LoadString(IDS_DEVICE_RESET_FAIL);			break;
	//case msg_factory_reset_stop:		strStatus.LoadString(IDS_DEVICE_RESET_STOP);			break;
	//case msg_initialize_start:			strStatus.LoadString(IDS_DEVICE_INIT_START);			break;
	//case msg_initialize_done:			strStatus.LoadString(IDS_DEVICE_INIT_DONE);				break;
	//case msg_initialize_fail:			strStatus.LoadString(IDS_DEVICE_INIT_FAIL);				break;
	//case msg_initialize_stop:			strStatus.LoadString(IDS_DEVICE_INIT_STOP);				break;
	//case msg_homing_all_start:			strStatus.LoadString(IDS_DEVICE_HOMING_START);			break;
	//case msg_homing_all_done:			strStatus.LoadString(IDS_DEVICE_HOMING_DONE);			break;
	//case msg_homing_all_fail:			strStatus.LoadString(IDS_DEVICE_HOMING_FAIL);			break;
	//case msg_homing_all_stop:			strStatus.LoadString(IDS_DEVICE_HOMING_STOP);			break;
	//default:	return;
	//case msg_motiongate_check_status:
	//{
	//	//	Status를 확인해서 표시
	//	if (m_stSystemStatus.wStatus.bitAlarm)
	//	{
	//		strStatus.LoadString(IDS_DEVICE_SYSTEM_ALARM);
	//	}
	//	else if (m_stSystemStatus.btStartMotorConnectionStatus != m_stSystemStatus.btCurrentMotorConnectionStatus)
	//	{
	//		strStatus.LoadString(IDS_DEVICE_SYSTEM_CONNECTION);
	//	}
	//	else
	//	{
	//		strStatus.LoadString(IDS_DEVICE_SYSTEM_OK);
	//	}
	//}	break;
	//}

	//if (0 == _tcscmp(m_strSimpleStatus, strStatus))
	//{
	//	return;
	//}

	//const INT nStrLen = strStatus.GetLength();

	//m_strSimpleStatus[nStrLen] = NULL;
	//_tcscpy_s(m_strSimpleStatus, MAX_PATH, strStatus);

	switch (nMsgID)
	{
	case msg_monitoring_start:			SetStatusConnectionStart();				break;
	case msg_monitoring_stop:			SetStatusConnectionStop();				break;
	case msg_motiongate_connected:		SetStatusConnectionConnected();			break;
	case msg_motiongate_disconnected:	SetStatusConnectionDisconnected();		break;
	case msg_motiongate_busy:			SetStatusConnectionBusy();				break;
	case msg_motiongate_reboot:			SetStatusConnectionReboot();			break;
	case msg_motiongate_read_info:		SetStatusConnectionCheckMotiongate();	break;
	case msg_scan_slave_progress:		SetStatusConnectionCheckSlave();		break;
	case msg_reset_detail_progress:		SetStatusConnectionSystemReady();		break;
	case msg_compare_start:				SetStatusCompareStart();				break;
	case msg_compare_stop:				SetStatusCompareStop();					break;
	case msg_compare_fail:				SetStatusCompareBad();					break;
	case msg_compare_done:				SetStatusCompareEnd();					break;
	case msg_backup_start:				SetStatusBackupRun();					break;
	case msg_backup_done:				SetStatusBackupGood();					break;
	case msg_backup_fail:				SetStatusBackupBad();					break;
	case msg_backup_stop:				SetStatusBackupStop();					break;
	case msg_restoration_start:			SetStatusRestoreRun();					break;
	case msg_restoration_done:			SetStatusRestoreGood();					break;
	case msg_restoration_fail:			SetStatusRestoreBad();					break;
	case msg_restoration_stop:			SetStatusRestoreStop();					break;
	case msg_factory_reset_start:		SetStatusResetRun();					break;
	case msg_factory_reset_done:		SetStatusResetGood();					break;
	case msg_factory_reset_fail:		SetStatusResetBad();					break;
	case msg_factory_reset_stop:		SetStatusResetStop();					break;
	case msg_initialize_start:			SetStatusAssignRun();					break;
	case msg_initialize_done:			SetStatusAssignGood();					break;
	case msg_initialize_fail:			SetStatusAssignBad();					break;
	case msg_initialize_stop:			SetStatusAssignStop();					break;
	case msg_homing_all_start:			SetStatusAllHomeRun();					break;
	case msg_homing_all_done:			SetStatusAllHomeGood();					break;
	case msg_homing_all_fail:			SetStatusAllHomeBad();					break;
	case msg_homing_all_stop:			SetStatusAllHomeStop();					break;
	case msg_homing_all_finish:			SetStatusAllHomeFinish();				break;
	case msg_motiongate_no_response:	SetStatusNoResponseMotiongate();		break;
	case msg_motiongate_no_motor_response: SetStatusNoMotorExist();				break;
	case msg_motiongate_check_status:
	{
		if (m_stSystemStatus.wStatus.bitTeachingPendant)
		{
			SetStatusConnectionBusy();
		}
		else
		{
			SetStatusConnectionSystemReady();
		}

	}	break;
	case msg_motiongate_read_status:
	{
		if (m_wCurAxisOrganization != 0)
		{
			for (INT nIndex = 1; nIndex < 9; nIndex++)
			{
				SetStatusErrorMotor(nIndex, m_stSystemStatus.dwDetailStatus[nIndex].bitErrorAll ? TRUE : FALSE);
			}
		}
		/*else
		{
			SetStatusNoMotorExist();
		}*/
		
	}	break;

	default:	break;
	}
} 


void CDeviceMotionGate::SaveLogStatus(const INT nMsgID, const CString strDetail)
{
	SaveLogMotionGateStatus(nMsgID, strDetail); // _INTERNAL_MESSAGE_ENEMERATION 관련 데이터
	SaveSubSystemStatus(nMsgID);
	//BuildSubSystemStatusWarning();
	SaveLogMotorStatus();

}

void CDeviceMotionGate::SaveLogMotionGateStatus(const INT nMsgID, CString strDetail)
{
	// KKB
	if (m_PrevLog.strNormal != strDetail && strDetail != L"")
	{
		if (TRUE)
		{
			CString strFilter = strDetail;

			INT	nIndex = 0;
			INT nState = 0;

			switch (nMsgID) // 뒤 숫자들 제거..
			{
			case msg_motiongate_read_status:
			case msg_reset_detail_progress:
			case msg_scan_slave_progress:
			case msg_read_parameter:
			case msg_compare_parameter:
			case msg_read_position:
			case msg_write_position:
			case msg_compare_position:
			case msg_write_parameter:
			case msg_homing_all_timer:
			{
				nIndex = strFilter.Find('(');
				if (-1 != nIndex)
				{
					CString strStandard;
					strStandard.LoadString(IDS_DETAIL_SYSTEM_ALARM);
					if (-1 != strStandard.Find(strFilter.Left(nIndex))) break;
					strFilter = strFilter.Left(nIndex);
				}

			}	break;
			}

			m_CurLog.strNormal = strFilter;
			if (m_CurLog.strNormal != m_PrevLog.strNormal && m_CurLog.strNormal != L"")
			{
				switch (nMsgID)
				{
					//Warining
				case msg_monitoring_stop:
				case msg_backup_stop:
				case msg_compare_stop:
				case msg_restoration_stop:
				case msg_factory_reset_stop:
				case msg_initialize_stop:
				case msg_homing_all_stop:
				case msg_life_standard_detected_on:
				case msg_life_standard_detected_off:
				case msg_life_cycle_detected_on:
				case msg_life_cycle_detected_off:
				case msg_motiongate_check_status:
				case msg_motiongate_read_status: // KKB
				{
					if (strFilter == _T("System Status OK")) break;


					if (nMsgID == msg_motiongate_read_status)
					{
						CString strAlarm;
 						strAlarm.LoadString(IDS_DETAIL_SYSTEM_ALARM);

						nIndex = strFilter.Find(strAlarm);
						if (-1 == nIndex) break;
					}

					m_PrevLog.strWarning = m_CurLog.strWarning = strFilter;
					nState = STS_LOG_WARNING;
				}	break;

				// Error
				case msg_motiongate_change_system_fail:
				case msg_backup_fail:
				case msg_restoration_fail:
				case msg_factory_reset_fail:
				case msg_initialize_fail:
				case msg_compare_fail:
				case msg_homing_all_fail:
				{
					m_PrevLog.strDeviceError = m_CurLog.strDeviceError = strFilter;
					nState = STS_LOG_ERROR;
				}	break;

				default:
				{
					nState = STS_LOG_NORMAL;
				}	break;
				}

				if (1)
				{ // 최종 db 저장 및 Operation Log 현시..
					CString str;
					//GetAddress(str);
					//TRACE(str + _T(" : ") + m_CurLog.strNormal + _T("\r\n"));
					
					//m_CurLog.strNormal.Replace(_T("..."), _T("."));

					InsertLog(nState, m_CurLog.strNormal); // To DataBase

					if (m_CurLog.strNormal != L"")
					{
						m_opLogInform.dwAddress = GetAddress();
						CWnd* pWnd = AfxGetMainWnd();
						::SendMessage(pWnd->m_hWnd, CMainFrame::UM_SEND_LOGMESSAGE_FOR_OPERATION_LOG, reinterpret_cast<WPARAM>(&m_opLogInform), 0);
					}

					m_PrevLog.strNormal = m_CurLog.strNormal;
				}

			}

		}
	}
	

}


void CDeviceMotionGate::BuildSubSystemStatusWarning()
{
	m_CurLog.strWarning = GetCurrentSystemStatus(STS_LOG_WARNING);
	if ((m_CurLog.strWarning != m_PrevLog.strWarning) && m_CurLog.strWarning != L"")
	{
		if (1)
		{ // 최종 db 저장 및 Operation Log 현시..
			InsertLog(STS_LOG_WARNING, m_CurLog.strWarning); // To DataBase

			if (m_CurLog.strNormal != L"")
			{
				m_opLogInform.dwAddress = GetAddress();
				CWnd* pWnd = AfxGetMainWnd();
				::SendMessage(pWnd->m_hWnd, CMainFrame::UM_SEND_LOGMESSAGE_FOR_OPERATION_LOG, reinterpret_cast<WPARAM>(&m_opLogInform), 0);
			}
		}

		m_PrevLog.strWarning = m_CurLog.strWarning;
	}
}




void CDeviceMotionGate::SaveSubSystemStatus(const INT nMsgID)
{
	CString strAddress;
	GetAddress(strAddress);
	CString str;
	int nState = STS_LOG_ERROR;
	//static INT nIndex = 0;
	if (nMsgID == msg_motiongate_read_status)
	{
		if (GetMotionGateConnectedStatus())
		{
			//if (IsInvalidMotorConnected())
			//{	//	동작중 모터개수의 변경이 감지되었다.
			//	str.Format(_T("Detecting the change in the number of motors."));
			//}
			//else
			//{
			//	const INT nMotorCount = GetContinuouslyConnectedMotorCount();
			//	if (nMotorCount != m_stConnectedDeviceInfo.nMotorCount || // 동작중 모션게이트의 모터 개수와 Run 시작시 인식된 모터개수가 상이할때
			//		nMotorCount != m_stDatabaseDeviceInfo.nMotorCount) // 동작중 모션게이트의 모터 개수와 db에 설정되더있는 모터개수가 상이할때..
			//	{
			//		if (nMotorCount != m_stConnectedDeviceInfo.nMotorCount)
			//		{
			//			str.Format(_T("Detection of Inconsistency with the initial number of motors."));
			//		}
			//		
			//		else
			//		{
			//			str.Format(_T("Detection of inconsistency with the number of database setting motors."));
			//		}
			//		
			//	}
			//}
			
			if (IsInvalidMotorConnected())
			{	//	동작중 초기 연결 종류대비 현재 모터 종류 변경이 감지되었다.
				str.Format(_T("The motor composition has been changed")); // or The motor connection has changed.
				m_bMotorChangeDetected = TRUE;
			}
			
			/*
				if (IsChangedMotorConnection())
			{
				BYTE motor = m_byMotorChange ^ m_stSystemStatus.btCurrentMotorConnectionStatus;
				for (; nIndex < 7; )
				{
					if ((motor >> nIndex) & 0x01)
					{
						nIndex++;
						m_bMotorChangeDetected = TRUE;
						BOOL bOn = (m_stSystemStatus.btCurrentMotorConnectionStatus >> nIndex) & 0x01;
						if (bOn)
						{
							str.Format(_T("[No.%d] motor connection detected."), nIndex + 1);
							break;
						}

						else
						{
							str.Format(_T("[No.%d] motor disconnection detected."), nIndex + 1);
							break;
						}
					}

					else
					{
						nIndex++;
					}
				}

				if (nIndex == 7)
				{
					m_byMotorChange = m_stSystemStatus.btCurrentMotorConnectionStatus;
					nIndex = 0;
				}
				
			}
			*/
		
			
			else if (GetContinuouslyConnectedMotorCount() != m_stConnectedDeviceInfo.nMotorCount ||
					 GetContinuouslyConnectedMotorCount() != m_stDatabaseDeviceInfo.nMotorCount)
			{
				m_bMotorChangeDetected = TRUE;
				if (GetContinuouslyConnectedMotorCount() != m_stDatabaseDeviceInfo.nMotorCount)
				{ // 설정 모터 개수와 초기 연결 모터 개수가 달라짐.
					str.Format(_T("Detection of inconsistency with the number of database setting motors."));
				}

				else
				{ //초기 연결 모터개수와 현재 모터 연결 개수가 달라짐.
					str.Format(_T("Detection of Inconsistency with the initial number of motors."));
				}
			}


			else if(m_bMotorChangeDetected)
			{
				m_bMotorChangeDetected = FALSE;
				str.Format(_T("Recovered the system.")); 
				nState = STS_LOG_NORMAL;
			}

		}
	}

	//m_CurLog.strDeviceError = GetCurrentSystemStatus(STS_LOG_ERROR); 기존..
	m_CurLog.strDeviceError = str;
	
	if ((m_CurLog.strDeviceError != m_PrevLog.strDeviceError) && m_CurLog.strDeviceError != L"")
	{
		if (1)
		{ // 최종 db 저장 및 Operation Log 현시..
			InsertLog(nState, m_CurLog.strDeviceError); // To DataBase

			if (m_CurLog.strNormal != L"")
			{
				m_opLogInform.dwAddress = GetAddress();
				CWnd* pWnd = AfxGetMainWnd();
				::SendMessage(pWnd->m_hWnd, CMainFrame::UM_SEND_LOGMESSAGE_FOR_OPERATION_LOG, reinterpret_cast<WPARAM>(&m_opLogInform), 0);
			}
		}

		m_PrevLog.strDeviceError = m_CurLog.strDeviceError;
	}
}

void CDeviceMotionGate::SaveLogMotorStatus()
{
	CString strMotorError;

	for (INT nMotor = 0; nMotor < 9; nMotor++)
	{
		GetMotorErrorStatus(strMotorError, nMotor);
		BOOL bFouund = (-1 != strMotorError.Find(_T("Found")));
		BOOL bReady = (-1 != strMotorError.Find(_T("Ready")));
		BOOL bBusy = (-1 != strMotorError.Find(_T("Busy")));

		if ((-1 == strMotorError.Find(_T("Busy"))) && (-1 == strMotorError.Find(_T("No_Error"))))
		{
			m_CurLog.strMotorError[nMotor] = strMotorError;

			if ((m_CurLog.strMotorError[nMotor] != m_PrevLog.strMotorError[nMotor]) && m_CurLog.strMotorError[nMotor] != L"")
			{
				if (1)
				{ // 최종 db 저장 및 Operation Log 현시..
					INT nState = STS_LOG_NORMAL;
					if (-1 == strMotorError.Find(_T("Recovered"))) nState = STS_LOG_ERROR;

					InsertLog(nState, m_CurLog.strMotorError[nMotor]); // To DataBase, m_opLogInform 값을 저장해줌..

					if (m_CurLog.strNormal != L"")
					{
						m_opLogInform.dwAddress = GetAddress();
						CWnd* pWnd = AfxGetMainWnd();
						::SendMessage(pWnd->m_hWnd, CMainFrame::UM_SEND_LOGMESSAGE_FOR_OPERATION_LOG, reinterpret_cast<WPARAM>(&m_opLogInform), 0);
					}
				}

				m_PrevLog.strMotorError[nMotor] = m_CurLog.strMotorError[nMotor];
			}
		}
		else
		{
			
		}
	}
	
}
void CDeviceMotionGate::BuildDetailStatus(const INT nMsgID, const LPARAM lParam)
{
	CString strStatus, strFormat, strDetail;
	INT nParam = static_cast<INT>(lParam);
	switch (nMsgID)
	{
	case msg_program_start:					strDetail.LoadString(IDS_DETAIL_PROGRAM_START);				break;
	case msg_program_end:					strDetail.LoadString(IDS_DETAIL_PROGRAM_END);				break;
	case msg_monitoring_start:				strDetail.LoadString(IDS_DETAIL_THREAD_START);				break;
	case msg_monitoring_stop:				strDetail.LoadString(IDS_DETAIL_THREAD_END);				break;
	//case msg_motiongate_connecting:			strDetail.LoadString(IDS_DETAIL_CONNECTING);				break;
	case msg_motiongate_connected:			strDetail.LoadString(IDS_DETAIL_CONNECTED);					break;
	case msg_motiongate_reconnecting:		strDetail.LoadString(IDS_DETAIL_RECONNECT);					break; // KKB 추가..
	case msg_motiongate_disconnected:		strDetail.LoadString(IDS_DETAIL_DISCONNECTED);				break; // KKB 추가..
	case msg_motiongate_no_response:		strDetail.LoadString(IDS_DETAIL_NO_RESPONSE);				break;
	case msg_motiongate_reboot:				strDetail.LoadString(IDS_DETAIL_REBOOT);					break;
	case msg_motiongate_read_info:			strDetail.LoadString(IDS_DETAIL_GET_MOTIONGATE_INFO);		break;
	//case msg_motiongate_read_status:		strDetail.LoadString(IDS_DETAIL_MOTION_READ_STATUS);		break; // KKB 추가..
	/*
	case msg_motiongate_read_status:
	{
		if (IsInvalidMotorConnected())
		{	//	동작중 초기 연결 종류대비 현재 모터 종류 변경이 감지되었다.
			strDetail.Format(_T("The motor composition has been changed")); // or The motor connection has changed.
			m_bMotorChangeDetected = TRUE;
		}

		else if (GetContinuouslyConnectedMotorCount() != m_stConnectedDeviceInfo.nMotorCount ||
			GetContinuouslyConnectedMotorCount() != m_stDatabaseDeviceInfo.nMotorCount)
		{
			m_bMotorChangeDetected = TRUE;
			if (GetContinuouslyConnectedMotorCount() != m_stDatabaseDeviceInfo.nMotorCount)
			{ // 설정 모터 개수와 초기 연결 모터 개수가 달라짐.

				strDetail.Format(_T("Detection of inconsistency with the number of database setting motors."));
			}

			else
			{ //초기 연결 모터개수와 현재 모터 연결 개수가 달라짐.
				strDetail.Format(_T("Detection of Inconsistency with the initial number of motors."));
			}
		}


		else if (m_bMotorChangeDetected)
		{
			m_bMotorChangeDetected = FALSE;
			strDetail.Format(_T("Normalizing the system."));
		}
	}	break;
	*/
	case msg_motiongate_enter_admin:			strDetail.LoadString(IDS_DETAIL_ENTER_ADMIN);				break;
	case msg_motiongate_confirm_admin_on:		strDetail.LoadString(IDS_DETAIL_CHECK_ADMIN_ON);			break;
	case msg_motiongate_exit_admin:				strDetail.LoadString(IDS_DETAIL_EXIT_ADMIN);				break;
	case msg_motiongate_confirm_admin_off:		strDetail.LoadString(IDS_DETAIL_CHECK_ADMIN_OFF);			break;
	case msg_motiongate_change_system_info:		strDetail.LoadString(IDS_DETAIL_CHANGE_NAME);				break;
	case msg_motiongate_change_system_done:		strDetail.LoadString(IDS_DETAIL_CHANGE_NAME_DONE);			break;
	case msg_motiongate_change_system_fail:		strDetail.LoadString(IDS_DETAIL_CHANGE_SYSTEM_FAIL);		break;
	case msg_motiongate_alarm_reset:			strDetail.LoadString(IDS_DETAIL_ALARM_RESET);				break;
	case msg_scan_slave_start:				strDetail.LoadString(IDS_DETAIL_SCAN_START);				break;
	case msg_scan_slave_progress:			strFormat.LoadString(IDS_DETAIL_SCAN_SLAVE);				strDetail.Format(strFormat, m_nMotorIndex);						break;
	case msg_read_parameter:				strFormat.LoadString(IDS_DETAIL_READ_PARAMETER);			strDetail.Format(strFormat, lParam, m_nParameterIndex);	break;
	case msg_write_parameter:				strFormat.LoadString(IDS_DETAIL_WRITE_PARAMETER);			strDetail.Format(strFormat, lParam, m_nParameterIndex);	break;
	case msg_save_parameter:				strDetail.LoadString(IDS_DETAIL_SAVE_PARAMETER);			break;
	case msg_compare_parameter:				strFormat.LoadString(IDS_DETAIL_COMPARE_PARAMETER);			strDetail.Format(strFormat, lParam, m_nParameterIndex);	break;
	case msg_read_position:					strFormat.LoadString(IDS_DETAIL_READ_POSITION_TABLE);		strDetail.Format(strFormat, lParam, m_nParameterIndex);	break;
	case msg_write_position:				strFormat.LoadString(IDS_DETAIL_WRITE_POSITION_TABLE);		strDetail.Format(strFormat, lParam, m_nParameterIndex);	break;
	case msg_save_position:					strDetail.LoadString(IDS_DETAIL_SAVE_POSITION_TABLE);		break;
	case msg_load_position:					strDetail.LoadString(IDS_DETAIL_LOAD_POSITION_TABLE);		break;
	case msg_compare_position:				strFormat.LoadString(IDS_DETAIL_COMPARE_POSITION_TABLE);	strDetail.Format(strFormat, lParam, m_nParameterIndex);	break;
	case msg_compare_start:					strDetail.LoadString(IDS_DETAIL_LOAD_COMPARE_START);		break;
	case msg_compare_stop:					strDetail.LoadString(IDS_DETAIL_LOAD_COMPARE_STOP);		break;
	case msg_compare_fail:					strDetail.LoadString(IDS_DETAIL_LOAD_COMPARE_FAIL);		break;
	case msg_compare_done:					strDetail.LoadString(IDS_DETAIL_LOAD_COMPARE_END);		break;
	case msg_backup_start:					strDetail.LoadString(IDS_DETAIL_BACKUP_START);				break;
	case msg_backup_done:					strDetail.LoadString(IDS_DETAIL_BACKUP_DONE);				break;
	case msg_backup_fail:					strDetail.LoadString(IDS_DETAIL_BACKUP_FAIL);				break;
	case msg_backup_stop:					strDetail.LoadString(IDS_DETAIL_BACKUP_STOP);				break;
	case msg_change_motor_id:				strDetail.LoadString(IDS_DETAIL_CHANGE_ID);					break;
	case msg_restoration_start:				strDetail.LoadString(IDS_DETAIL_RESTORATION_START);			break;
	case msg_restoration_done:				strDetail.LoadString(IDS_DETAIL_RESTORATION_DONE);			break;
	case msg_restoration_fail:				strDetail.LoadString(IDS_DETAIL_RESTORATION_FAIL);			break;
	case msg_restoration_stop:				strDetail.LoadString(IDS_DETAIL_RESTORATION_STOP);			break;
	case msg_factory_reset_start:			strDetail.LoadString(IDS_DETAIL_RESET_START);				break;
	case msg_factory_reset_done:			strDetail.LoadString(IDS_DETAIL_RESET_DONE);				break;
	case msg_factory_reset_fail:			strDetail.LoadString(IDS_DETAIL_RESET_FAIL);				break;
	case msg_factory_reset_stop:			strDetail.LoadString(IDS_DETAIL_RESET_STOP);				break;
	case msg_initialize_start:				strDetail.LoadString(IDS_DETAIL_INIT_START);				break;
	case msg_initialize_done:				strDetail.LoadString(IDS_DETAIL_INIT_DONE);					break;
	case msg_initialize_fail:				strDetail.LoadString(IDS_DETAIL_INIT_FAIL);					break;
	case msg_initialize_stop:				strDetail.LoadString(IDS_DETAIL_INIT_STOP);					break;
	case msg_homing_start:
	case msg_homing_done:
	case msg_homing_fail:
	case msg_homing_stop:
	case msg_homing_timer:					break; // 아래 모터 상세 정보 있음..
	case msg_homing_all_start:				break;// strDetail.LoadString(IDS_DETAIL_HOMING_ALL_START);			break;
	case msg_homing_all_done:				strDetail.LoadString(IDS_DETAIL_HOMING_ALL_DONE);			break;
	case msg_homing_all_fail:				strDetail.LoadString(IDS_DETAIL_HOMING_ALL_FAIL);			break;
	case msg_homing_all_stop:				strDetail.LoadString(IDS_DETAIL_HOMING_ALL_STOP);			break;
	//case msg_homing_all_timer:			break;//	strFormat.LoadString(IDS_DETAIL_HOMING_ALL_TIMER);			strDetail.Format(strFormat, m_nDetailProgressPos / 1000);	break;
	case msg_life_standard_detected_on:		strFormat.LoadString(IDS_STATUS_LIFE_STAND);				strDetail.Format(strFormat, nParam); break;
	case msg_life_standard_detected_off:	strFormat.LoadString(IDS_STATUS_LIFE_STAND_OFF);			strDetail.Format(strFormat, nParam); break;
	case msg_life_cycle_detected_on:		strFormat.LoadString(IDS_STATUS_LIFE_CYCLE);				strDetail.Format(strFormat, nParam, GetMotorLifeCount(nParam)); break;
	case msg_life_cycle_detected_off:		strFormat.LoadString(IDS_STATUS_LIFE_CYCLE_OFF);			strDetail.Format(strFormat, nParam); break;
	case msg_motiongate_read_status: // KKB 추가..
	case msg_motiongate_check_status:
	{
		INT nStrID = IDS_DETAIL_SYSTEM_OK;
		
		CString strBitAlarm;
		BOOL bBitAlarm = FALSE;
		BOOL bHoming = FALSE;
		//	Status를 확인해서 표시
 		CString str = GetSystemName();
		if (m_stSystemStatus.wStatus.bitTeachingPendant)
		{
			nStrID = IDS_DETAIL_ACCESS_DENIED;
		}
		/*else if (m_stSystemStatus.btStartMotorConnectionStatus != m_stSystemStatus.btCurrentMotorConnectionStatus)
		{
			nStrID = IDS_DETAIL_SYSTEM_CONNECTION;
		}*/
		else if (m_stSystemStatus.wStatus.bitAlarm)
		{
			nStrID = IDS_DETAIL_SYSTEM_ALARM;
			bBitAlarm = TRUE;

			BYTE byResult = m_stSystemStatus.btSystemAlarmStatus;
			switch (byResult)
			{
			case motion_status_push_detection_fail :
			{
				strBitAlarm.Format(_T("(%d) \r\n: Error of not catching the target."), byResult);
			} break;

			case motion_status_eot_not_match :
			{
				strBitAlarm.Format(_T("(%d) \r\n: A valid device ID connection stored in the master controller is required."), byResult);
			}	break;

			case motion_status_teaching_pendant_connected :
			{
				strBitAlarm.Format(_T("(%d) \r\n: Teaching pendant connection is detected."), byResult);
			}	break;

			case motion_status_motor_disconnected :
			{
				strBitAlarm.Format(_T("(%d) \r\n: the motors communication status has unstable connection or has been disconnected."), byResult);
			}	break;

			case motion_status_motor_port_disconnected:
			{
				strBitAlarm.Format(_T("(%d) \r\n: monitoring port disconnected."), byResult);
			}	break;

			default:
			{
				return;
			}
				break;
			}

		}

		else if (m_stSystemStatus.wStatus.bitHoming)
		{
			bHoming = TRUE;
		}

		strDetail.LoadString(nStrID);
		
		if (bBitAlarm) strDetail += strBitAlarm;
		if (bHoming) return;
	}	break;

	default:								return;
	}
	
	//SaveLogMessage(nMsgID, strDetail);
	

	if (true)
	{ // Motor Log 저장
		BOOL bMotorLog = TRUE;

		switch (nMsgID)
		{ // 현재 들어온 메세지가 각 Motor의 Status값들인지 확인.
		case msg_read_parameter:				strFormat.LoadString(IDS_DETAIL_READ_PARAMETER);			strDetail.Format(strFormat, lParam, m_nParameterIndex);	break;
		case msg_write_parameter:				strFormat.LoadString(IDS_DETAIL_WRITE_PARAMETER);			strDetail.Format(strFormat, lParam, m_nParameterIndex);	break;
		case msg_compare_parameter:				strFormat.LoadString(IDS_DETAIL_COMPARE_PARAMETER);			strDetail.Format(strFormat, lParam, m_nParameterIndex);	break;
		case msg_read_position:					strFormat.LoadString(IDS_DETAIL_READ_POSITION_TABLE);		strDetail.Format(strFormat, lParam, m_nParameterIndex);	break;
		case msg_write_position:				strFormat.LoadString(IDS_DETAIL_WRITE_POSITION_TABLE);		strDetail.Format(strFormat, lParam, m_nParameterIndex);	break;
		case msg_compare_position:				strFormat.LoadString(IDS_DETAIL_COMPARE_POSITION_TABLE);	strDetail.Format(strFormat, lParam, m_nParameterIndex);	break;

		case msg_homing_start:					strFormat.LoadString(IDS_DETAIL_HOMING_START);				strDetail.Format(strFormat, nParam); break;
		case msg_homing_done:					strFormat.LoadString(IDS_DETAIL_HOMING_DONE);				strDetail.Format(strFormat, nParam); break;
		case msg_homing_fail:					strFormat.LoadString(IDS_DETAIL_HOMING_FAIL);				strDetail.Format(strFormat, nParam); break;
		case msg_homing_stop:					strFormat.LoadString(IDS_DETAIL_HOMING_STOP);				strDetail.Format(strFormat, nParam); break;
		case msg_homing_timer:					strFormat.LoadString(IDS_DETAIL_HOMING_TIMER);				strDetail.Format(strFormat, nParam); break;

		case msg_life_standard_detected_on:		strFormat.LoadString(IDS_STATUS_LIFE_STAND);				strDetail.Format(strFormat, nParam); break;
		case msg_life_standard_detected_off:	strFormat.LoadString(IDS_STATUS_LIFE_STAND_OFF);			strDetail.Format(strFormat, nParam); break;
		case msg_life_cycle_detected_on:		strFormat.LoadString(IDS_STATUS_LIFE_CYCLE);				strDetail.Format(strFormat, nParam, GetMotorLifeCount(nParam)); break;
		case msg_life_cycle_detected_off:		strFormat.LoadString(IDS_STATUS_LIFE_CYCLE_OFF);			strDetail.Format(strFormat, nParam); break;
		
		default: { bMotorLog = FALSE; }	break;
		}

		if (bMotorLog)
		{
			if (m_stSystemStatus.stMotorLog[nParam] != strDetail)
			{
				m_stSystemStatus.stMotorLog[nParam] = strDetail;
			}
			else
			{
				return;
			}
		}
	}
	

	if (0 == _tcscmp(m_strDetailStatus, strDetail))
	{
		return;
	}

	const INT nStrLen = strDetail.GetLength();

	m_strDetailStatus[nStrLen] = NULL;
	_tcscpy_s(m_strDetailStatus, MAX_PATH, strDetail);
}



void CDeviceMotionGate::BuildDetailProgress(const INT nMsgID, const LPARAM lParam)
{
	switch (nMsgID)
	{
	case msg_motiongate_enter_admin:
	case msg_motiongate_exit_admin:
	case msg_motiongate_change_system_info:
	case msg_motiongate_reconnecting:
	case msg_change_motor_id:
	case msg_scan_slave_progress:
	case msg_read_parameter:
	case msg_write_parameter:
	case msg_save_parameter:
	case msg_compare_parameter:
	case msg_read_position:
	case msg_write_position:
	case msg_save_position:
	case msg_load_position:
	case msg_compare_position:
	{
		m_nDetailProgressPos++;
	}	break;

	case msg_homing_all_timer:
	{
		const INT nCurPos = m_nDetailProgressMax - static_cast<INT>(lParam);
		if (m_nDetailProgressPos < nCurPos)
		{
			m_nDetailProgressPos = nCurPos;
		}

	}	break;

	case msg_motiongate_connected:
	case msg_scan_slave_start:
	{
		m_nDetailProgressPos = 0;
		m_nDetailProgressMin = 0;
		m_nDetailProgressMax = 9;
	}	break;

	case msg_compare_start:
	case msg_backup_start:
	{
		m_nDetailProgressPos = 0;
		m_nDetailProgressMin = 0;
		m_nDetailProgressMax = GetConnectedMotorCount() * (40 + 32);
	}	break;

	case msg_restoration_start:
	{
		m_nDetailProgressPos = 0;
		m_nDetailProgressMin = 0;
		m_nDetailProgressMax = GetConnectedMotorCount() * ((40 + 32) * 2 + 3);
	}	break;

	case msg_factory_reset_start:
	{
		m_nDetailProgressPos = 0;
		m_nDetailProgressMin = 0;
		m_nDetailProgressMax = 1 * ((40 + 32) * 2 + 3);
	}	break;

	case msg_initialize_start:
	{
		m_nDetailProgressPos = 0;
		m_nDetailProgressMin = 0;
		m_nDetailProgressMax = 3;
	}	break;

	case msg_motiongate_reboot:
	{
		m_nDetailProgressPos = 0;
		m_nDetailProgressMin = 0;
		m_nDetailProgressMax = REBOOT_CONNECTION_COUNT;
	}	break;

	case msg_homing_all_start:
	{
		m_nDetailProgressPos = 0;
		m_nDetailProgressMin = 0;
		m_nDetailProgressMax = 10000;
		CDataBaseProject::GetInstance()->GetHomingTimeout(m_stDatabaseDeviceInfo.strName, m_nDetailProgressMax);
	}	break;

	case msg_reset_detail_progress:
	case msg_backup_done:
	case msg_restoration_done:
	case msg_factory_reset_done:
	case msg_initialize_done:
	case msg_homing_all_done:
	{
		m_nDetailProgressPos = 0;
		m_nDetailProgressMin = 0;
		/* 7
		//기존
		m_nDetailProgressMax = 100; 
		*/
		m_nDetailProgressMax = GetConnectedMotorCount()* (40 + 32);
	}	break;
	}
}

LPCTSTR CDeviceMotionGate::GetDetailStatus(INT* pPos, INT* pMin, INT* pMax)
{
	if (pPos)
	{
		*pPos = m_nDetailProgressPos;
	}

	if (pMax)
	{
		*pMax = m_nDetailProgressMax;
	}

	if (pMin)
	{
		*pMin = m_nDetailProgressMin;
	}

	return m_strDetailStatus;
}


void CDeviceMotionGate::CheckAutoBackupSchedule()
{
	CDataBaseProject* pProjectDB = CDataBaseProject::GetInstance();

	CString strSystemName(GetSystemName());
	//	CString strAddress;	GetAddress(strAddress);
	/*if (pProjectDB->IsDataBaseOpened())
	{
		pProjectDB->Lock();
		pProjectDB->DeletePassedBackupReservationData(strSystemName);
		pProjectDB->UnLock();
	}

	else
	{
		pProjectDB->DeletePassedBackupReservationData(strSystemName);
	}*/
	
	//pProjectDB->DeletePassedBackupReservationData(strSystemName);
	
	pProjectDB->Lock();
	pProjectDB->DeletePassedBackupReservationData(strSystemName);
	pProjectDB->UnLock();
	

	//	Get Reservation Date time
	COleDateTime timeSingleTime, timeRepeatTime;

	const BOOL bSingle = pProjectDB->GetSingleReservationTime(strSystemName, timeSingleTime);

	//	Get Repeat Time
	const BOOL bRepeat = pProjectDB->GetRepeatReservationTime(strSystemName, timeRepeatTime);
	if (bRepeat)
	{
		CString strTemp(timeRepeatTime.Format(_T("%c")));
		INT i = 0;
	}

	m_bBackupReservation = bSingle || bRepeat;

	if (m_bBackupReservation)
	{
		if (bSingle && bRepeat)
		{
			m_timeAutoBackup = timeSingleTime < timeRepeatTime ? timeSingleTime : timeRepeatTime;
		}
		else if (bSingle)
		{
			m_timeAutoBackup = timeSingleTime;
		}
		else //	if (bRepeat)
		{
			m_timeAutoBackup = timeRepeatTime;
		}

		CString strTemp(m_timeAutoBackup.Format(_T("%c")));

		m_timeAutoBackupTickTock = COleDateTime::GetCurrentTime();
		m_timespanAutoBackup = m_timeAutoBackup - m_timeAutoBackupTickTock;
		if (m_timespanAutoBackup.GetTotalSeconds() <= 0)
		{
			m_timespanAutoBackup = COleDateTimeSpan(0, 0, 1, 0);
		}
	}
}



const BOOL CDeviceMotionGate::IsAutoBackupScheduleExist()
{
	return m_bBackupReservation;
}

void CDeviceMotionGate::PauseAutoBackup(const BOOL bPause)
{
	if (bPause)
	{
		m_nPauseBackupCounter++;
	}
	else
	{
		m_nPauseBackupCounter--;
	}
}

const BOOL CDeviceMotionGate::GetParameter(const INT nMotorIndex, MOTOR_PARAMETER_DATA* pParameter)
{
	if (0 > nMotorIndex || 8 < m_nMotorIndex)
	{
		return FALSE;
	}

	memcpy(pParameter, &m_stMotorInfo[nMotorIndex], sizeof(MOTOR_PARAMETER_DATA));

	return TRUE;
}

COleDateTime& CDeviceMotionGate::GetLastReadParameter()
{
	return m_timeLastReadParameter;
}

const DOUBLE CDeviceMotionGate::GetAutoBackupLeftSeconds()
{
	return m_timespanAutoBackup.GetTotalSeconds();
}

const BOOL CDeviceMotionGate::SetStatusConnectionStop()
{
	const DWORD dwFlag = sts_flag_connection_stop;
	if (m_dwStatusFlag[sts_connection] != dwFlag)
	{
		m_dateTimeStatus[sts_connection] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_connection] = dwFlag;
	}

	return TRUE;
}

const BOOL	CDeviceMotionGate::SetStatusConnectionStart()
{
	const DWORD dwFlag = sts_flag_connection_start;
	if (m_dwStatusFlag[sts_connection] != dwFlag)
	{
		m_dateTimeStatus[sts_connection] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_connection] = dwFlag;
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::SetStatusConnectionDisconnected()
{
	const DWORD dwFlag = sts_flag_connection_disconnected;
	if (m_dwStatusFlag[sts_connection] != dwFlag)
	{
		m_dateTimeStatus[sts_connection] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_connection] = dwFlag;
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::SetStatusConnectionConnected()
{
	const DWORD dwFlag = sts_flag_connection_connected;
	if (m_dwStatusFlag[sts_connection] != dwFlag)
	{
		m_dateTimeStatus[sts_connection] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_connection] = dwFlag;
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::SetStatusConnectionBusy()
{
	const DWORD dwFlag = sts_flag_connection_off_line;
	if (m_dwStatusFlag[sts_connection] != dwFlag)
	{
		if (state_motion_gate_get_system_status_all == m_stateMachine)
		{
			m_stateMachine = state_connect;
			m_nOpCode = opCode_MotiongateBusy;
		}

		m_dateTimeStatus[sts_connection] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_connection] = dwFlag;
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::SetStatusConnectionCheckMotiongate()
{
	const DWORD dwFlag = sts_flag_connection_check_motiongate;
	if (m_dwStatusFlag[sts_connection] != dwFlag)
	{
		m_dateTimeStatus[sts_connection] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_connection] = dwFlag;
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::SetStatusConnectionCheckSlave()
{
	const DWORD dwFlag = sts_flag_connection_check_slave;
	if (m_dwStatusFlag[sts_connection] != dwFlag)
	{
		m_dateTimeStatus[sts_connection] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_connection] = dwFlag;
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::SetStatusConnectionSystemReady()
{
	const DWORD dwFlag = sts_flag_connection_system_ready;
	if (m_dwStatusFlag[sts_connection] != dwFlag)
	{

		/*if (TRUE == IsMotionGateMotorExist())
		{
			m_dateTimeStatus[sts_connection] = COleDateTime::GetCurrentTime();
			m_dwStatusFlag[sts_connection] = dwFlag;


			SetStatusNoResponseMotiongate(FALSE);

			for (INT nIndex = 1; nIndex < 9; nIndex++)
			{
				SetStatusNoResponseMotor(nIndex, FALSE);
			}
		}
		else
		{
			SetStatusNoMotorExist();
		}*/
		m_dateTimeStatus[sts_connection] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_connection] = dwFlag;


		SetStatusNoResponseMotiongate(FALSE);

		for (INT nIndex = 1; nIndex < 9; nIndex++)
		{
			SetStatusNoResponseMotor(nIndex, FALSE);
		}
		
	}
	
	return TRUE;
}

const BOOL CDeviceMotionGate::SetStatusConnectionReboot()
{
	const DWORD dwFlag = sts_flag_connection_reboot;
	if (m_dwStatusFlag[sts_connection] != dwFlag)
	{
		m_dateTimeStatus[sts_connection] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_connection] = dwFlag;
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::SetStatusAlarmMotor0(const BOOL bEnable)
{
	const DWORD dwFlag = sts_flag_alarm_motor0;
	const BOOL bAlarm = m_dwStatusFlag[sts_alarm] & dwFlag ? TRUE : FALSE;
	if (bAlarm != bEnable)
	{
		m_dwStatusFlag[sts_alarm] &= ~dwFlag;

		if (bEnable)
		{
			m_dwStatusFlag[sts_alarm] |= dwFlag;
		}
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::SetStatusAlarmMotorCount(const BOOL bEnable)
{
	const DWORD dwFlag = sts_flag_alarm_motor_count;
	const BOOL bAlarm = m_dwStatusFlag[sts_alarm] & dwFlag ? TRUE : FALSE;
	if (bAlarm != bEnable)
	{
		m_dwStatusFlag[sts_alarm] &= ~dwFlag;

		if (bEnable)
		{
			m_dwStatusFlag[sts_alarm] |= dwFlag;
		}
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::SetStatusAlarmAllHoming(const BOOL bEnable)
{
	const DWORD dwFlag = sts_flag_alarm_all_homing;
	const BOOL bAlarm = m_dwStatusFlag[sts_alarm] & dwFlag ? TRUE : FALSE;
	if (bAlarm != bEnable)
	{
		m_dwStatusFlag[sts_alarm] &= ~dwFlag;

		if (bEnable)
		{
			m_dwStatusFlag[sts_alarm] |= dwFlag;
		}
	}

	return TRUE;
}


const BOOL CDeviceMotionGate::SetStatusNoResponseMotiongate(const BOOL bError)
{
	const DWORD dwFlag = sts_flag_error_no_response_motion_gate;
	const BOOL bErrorFlag = (m_dwStatusFlag[sts_error] & dwFlag) ? TRUE : FALSE;
	if (bError != bErrorFlag)
	{
		m_dateTimeStatus[sts_error] = COleDateTime::GetCurrentTime();

		m_dwStatusFlag[sts_error] &= ~dwFlag;

		if (bError)
		{
			m_dwStatusFlag[sts_error] |= dwFlag;
		}
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::SetStatusNoMotorExist(const BOOL bError)
{
	const DWORD dwFlag = sts_flag_error_no_motor;
	const BOOL bErrorFlag = (m_dwStatusFlag[sts_error] & dwFlag) ? TRUE : FALSE;
	/*if (bError != bErrorFlag)
	{
		m_dateTimeStatus[sts_error] = COleDateTime::GetCurrentTime();

		m_dwStatusFlag[sts_error] &= ~dwFlag;

		if (bError)
		{
			m_dwStatusFlag[sts_error] |= dwFlag;
		}
	}*/
	if (TRUE)
	{
		m_dateTimeStatus[sts_error] = COleDateTime::GetCurrentTime();

		m_dwStatusFlag[sts_error] &= ~dwFlag;

		if (TRUE)
		{
			m_dwStatusFlag[sts_error] |= dwFlag;
		}
	}

	return TRUE;
}


const BOOL CDeviceMotionGate::SetStatusNoResponseMotor(const INT nMotorID, const BOOL bError)
{
	if (0 > nMotorID || 8 < nMotorID)
	{
		return FALSE;
	}
	
	const DWORD dwFlag = sts_flag_error_no_response_motor0 << nMotorID;
	const BOOL bErrorFlag = (m_dwStatusFlag[sts_error] & dwFlag) ? TRUE : FALSE;
	if (bError != bErrorFlag)
	{
		m_dateTimeStatus[sts_error] = COleDateTime::GetCurrentTime();

		m_dwStatusFlag[sts_error] &= ~dwFlag;

		if (bError)
		{
			m_dwStatusFlag[sts_error] |= dwFlag;
		}
	}

	return TRUE;
}


const BOOL CDeviceMotionGate::SetStatusErrorMotiongate(const BOOL bError)
{
	const DWORD dwFlag = sts_flag_error_motiongate;
	const BOOL bErrorFlag = (m_dwStatusFlag[sts_error] & dwFlag) ? TRUE : FALSE;
	if (bError != bErrorFlag)
	{
		m_dateTimeStatus[sts_error] = COleDateTime::GetCurrentTime();

		m_dwStatusFlag[sts_error] &= ~dwFlag;

		if (bError)
		{
			m_dwStatusFlag[sts_error] |= dwFlag;
		}
	}

	return TRUE;
}


const BOOL CDeviceMotionGate::SetStatusErrorMotor(const INT nMotorID, const BOOL bError)
{
	if (0 > nMotorID || 8 < nMotorID)
	{
		return FALSE;
	}

	const DWORD dwFlag = sts_flag_error_motor0 << nMotorID;
	const BOOL bErrorFlag = (m_dwStatusFlag[sts_error] & dwFlag) ? TRUE : FALSE;

	/*if (bError)
	{
		CString str;
		str.Format(_T("[%s] Motor : %d / Error Detected!! \r\n"), GetSystemName(), nMotorID);
		TRACE(str);
	}*/

	if (bError != bErrorFlag)
	{
		m_dateTimeStatus[sts_error] = COleDateTime::GetCurrentTime();

		m_dwStatusFlag[sts_error] &= ~dwFlag;

		if (bError)
		{
			m_dwStatusFlag[sts_error] |= dwFlag;
		}
	}

	return TRUE;
}


const BOOL CDeviceMotionGate::SetStatusBackupRun()
{
	const DWORD dwFlag = sts_flag_backup_run;
	if (FALSE == (m_dwStatusFlag[sts_backup] & dwFlag))
	{
		m_dateTimeStatus[sts_backup] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_backup] = dwFlag;
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::SetStatusCompareStart()
{
	const DWORD dwFlag = sts_flag_comapare_start;
	if (FALSE == (m_dwStatusFlag[sts_backup] & dwFlag))
	{
		m_dateTimeStatus[sts_backup] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_backup] = dwFlag;
	}

	return TRUE;
}


const BOOL CDeviceMotionGate::SetStatusCompareEnd()
{
	const DWORD dwFlag = sts_flag_comapare_end;
	if (FALSE == (m_dwStatusFlag[sts_backup] & dwFlag))
	{
		m_dateTimeStatus[sts_backup] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_backup] = dwFlag;
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::SetStatusCompareStop()
{
	const DWORD dwFlag = sts_flag_comapare_stop;
	if (FALSE == (m_dwStatusFlag[sts_backup] & dwFlag))
	{
		m_dateTimeStatus[sts_backup] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_backup] = dwFlag;
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::SetStatusCompareBad()
{
	const DWORD dwFlag = sts_flag_comapare_bad;
	if (FALSE == (m_dwStatusFlag[sts_backup] & dwFlag))
	{
		m_dateTimeStatus[sts_backup] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_backup] = dwFlag;
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::SetStatusBackupStop()
{
	const DWORD dwFlag = sts_flag_backup_stop;
	if (FALSE == (m_dwStatusFlag[sts_backup] & dwFlag))
	{
		m_dateTimeStatus[sts_backup] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_backup] = dwFlag;
	}

	return TRUE;
}


const BOOL CDeviceMotionGate::SetStatusBackupGood()
{
	const DWORD dwFlag = sts_flag_backup_good;
	if (FALSE == (m_dwStatusFlag[sts_backup] & dwFlag))
	{
		m_dateTimeStatus[sts_backup] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_backup] = dwFlag;
	}

	return TRUE;
}


const BOOL CDeviceMotionGate::SetStatusBackupBad()
{
	const DWORD dwFlag = sts_flag_backup_bad;
	if (FALSE == (m_dwStatusFlag[sts_backup] & dwFlag))
	{
		m_dateTimeStatus[sts_backup] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_backup] = dwFlag;
	}

	return TRUE;
}


const BOOL CDeviceMotionGate::SetStatusRestoreRun()
{
	const DWORD dwFlag = sts_flag_restore_run;
	if (FALSE == (m_dwStatusFlag[sts_restore] & dwFlag))
	{
		m_dateTimeStatus[sts_restore] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_restore] = dwFlag;
	}

	return TRUE;
}


const BOOL CDeviceMotionGate::SetStatusRestoreStop()
{
	const DWORD dwFlag = sts_flag_restore_stop;
	if (FALSE == (m_dwStatusFlag[sts_restore] & dwFlag))
	{
		m_dateTimeStatus[sts_restore] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_restore] = dwFlag;
	}

	return TRUE;
}


const BOOL CDeviceMotionGate::SetStatusRestoreGood()
{
	const DWORD dwFlag = sts_flag_restore_good;
	if (FALSE == (m_dwStatusFlag[sts_restore] & dwFlag))
	{
		m_dateTimeStatus[sts_restore] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_restore] = dwFlag;
	}

	return TRUE;
}


const BOOL CDeviceMotionGate::SetStatusRestoreBad()
{
	const DWORD dwFlag = sts_flag_restore_bad;
	if (FALSE == (m_dwStatusFlag[sts_restore] & dwFlag))
	{
		m_dateTimeStatus[sts_restore] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_restore] = dwFlag;
	}

	return TRUE;
}


const BOOL CDeviceMotionGate::SetStatusAllHomeRun()
{
	const DWORD dwFlag = sts_flag_restore_all_home_run;
	if (FALSE == (m_dwStatusFlag[sts_restore] & dwFlag))
	{
		m_dateTimeStatus[sts_restore] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_restore] = dwFlag;
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::SetStatusAllHomeStop()
{
	const DWORD dwFlag = sts_flag_restore_all_home_stop;
	if (FALSE == (m_dwStatusFlag[sts_restore] & dwFlag))
	{
		m_dateTimeStatus[sts_restore] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_restore] = dwFlag;
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::SetStatusAllHomeGood()
{
	const DWORD dwFlag = sts_flag_restore_all_home_good;
	if (FALSE == (m_dwStatusFlag[sts_restore] & dwFlag))
	{
		m_dateTimeStatus[sts_restore] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_restore] = dwFlag;
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::SetStatusAllHomeFinish()
{
	const DWORD dwFlag = sts_flag_restore_all_home_finish;
	if (FALSE == (m_dwStatusFlag[sts_restore] & dwFlag))
	{
		m_dateTimeStatus[sts_restore] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_restore] = dwFlag;
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::SetStatusAllHomeBad()
{
	const DWORD dwFlag = sts_flag_restore_all_home_bad;
	if (FALSE == (m_dwStatusFlag[sts_restore] & dwFlag))
	{
		m_dateTimeStatus[sts_restore] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_restore] = dwFlag;
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::SetStatusResetRun()
{
	const DWORD dwFlag = sts_flag_reset_run;
	if (FALSE == (m_dwStatusFlag[sts_reset] & dwFlag))
	{
		m_dateTimeStatus[sts_reset] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_reset] = dwFlag;
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::SetStatusResetStop()
{
	const DWORD dwFlag = sts_flag_reset_stop;
	if (FALSE == (m_dwStatusFlag[sts_reset] & dwFlag))
	{
		m_dateTimeStatus[sts_reset] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_reset] = dwFlag;
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::SetStatusResetGood()
{
	const DWORD dwFlag = sts_flag_reset_good;
	if (FALSE == (m_dwStatusFlag[sts_reset] & dwFlag))
	{
		m_dateTimeStatus[sts_reset] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_reset] = dwFlag;
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::SetStatusResetBad()
{
	const DWORD dwFlag = sts_flag_reset_bad;
	if (FALSE == (m_dwStatusFlag[sts_reset] & dwFlag))
	{
		m_dateTimeStatus[sts_reset] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_reset] = dwFlag;
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::SetStatusAssignRun()
{
	const DWORD dwFlag = sts_flag_assign_run;
	if (FALSE == (m_dwStatusFlag[sts_assign] & dwFlag))
	{
		m_dateTimeStatus[sts_assign] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_assign] = dwFlag;
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::SetStatusAssignStop()
{
	const DWORD dwFlag = sts_flag_assign_stop;
	if (FALSE == (m_dwStatusFlag[sts_assign] & dwFlag))
	{
		m_dateTimeStatus[sts_assign] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_assign] = dwFlag;
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::SetStatusAssignGood()
{
	const DWORD dwFlag = sts_flag_assign_good;
	if (FALSE == (m_dwStatusFlag[sts_assign] & dwFlag))
	{
		m_dateTimeStatus[sts_assign] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_assign] = dwFlag;
	}

	return TRUE;
}

const BOOL CDeviceMotionGate::SetStatusAssignBad()
{
	const DWORD dwFlag = sts_flag_assign_bad;
	if (FALSE == (m_dwStatusFlag[sts_assign] & dwFlag))
	{
		m_dateTimeStatus[sts_assign] = COleDateTime::GetCurrentTime();
		m_dwStatusFlag[sts_assign] = dwFlag;
	}

	return TRUE;
}

const INT CDeviceMotionGate::GetLastSimpleStatus()
{
	INT nResult = 0;

	for (INT nIndex = 1; nIndex < sts_max; nIndex++)
	{
		if (m_dateTimeStatus[nResult] < m_dateTimeStatus[nIndex])
		{
			switch (nIndex)
			{
			case sts_alarm:
			case sts_error:
			{
				if (m_dwStatusFlag[nIndex])
				{
					nResult = nIndex;
				}
			}	break;

			case sts_connection:
			case sts_backup:
			case sts_restore:
			case sts_reset:
			case sts_assign:
			default:
			{
				// KKB 추가
				// Compare 모드는 완료시 연결 상태의 값을 출력해준다, 다만 에러가 존재했을 경우 에러값을 나타내준다.
				if (nIndex == sts_backup && (m_dwStatusFlag[nIndex] == sts_flag_comapare_end || m_dwStatusFlag[nIndex] == sts_flag_comapare_stop))
				{
					nResult = sts_connection;
					if (m_dwStatusFlag[sts_error] != 0)
					{
						nResult = sts_error;
					}
				}
					
				else nResult = nIndex;
			}	break;
			}
		}
	}

	return nResult;
}

const DWORD	CDeviceMotionGate::GetLastSimpleStatusFlag(const INT nSimpleStatus)
{
	return m_dwStatusFlag[nSimpleStatus];
}

void CDeviceMotionGate::GetLastSimpleStatus(const INT nSimpleStatus, CString& strStatus)
{
	const DWORD dwStatusFlag = ScanFastStatus(m_dwStatusFlag[nSimpleStatus]);

	INT nStringID = -1;

	switch (nSimpleStatus)
	{
	case sts_connection:
	{
		switch (dwStatusFlag)
		{
		case sts_flag_connection_stop:				nStringID = IDS_DEVICE_MONITORING_STOP;		break;
		case sts_flag_connection_start:				nStringID = IDS_DEVICE_MONITORING_START;	break;
		case sts_flag_connection_disconnected:		nStringID = IDS_DEVICE_DISCONNECTED;		break;
		case sts_flag_connection_connected:			nStringID = IDS_DEVICE_CONNECTED;			break;
		case sts_flag_connection_off_line:			nStringID = IDS_DEVICE_OFFLINE;				break;
		case sts_flag_connection_check_motiongate:	nStringID = IDS_DEVICE_INFORMATION;			break;
		case sts_flag_connection_check_slave:		nStringID = IDS_DEVICE_SCAN;				break;
		case sts_flag_connection_system_ready:		nStringID = IDS_DEVICE_SYSTEM_OK;			break;
		case sts_flag_connection_reboot:			nStringID = IDS_DEVICE_REBOOT;				break;
		}
	}	break;

	case sts_error:
	{
		switch (dwStatusFlag)
		{
		case sts_flag_error_no_response_motion_gate:	nStringID = IDS_DEVICE_NO_RESPONSE;			break;
		case sts_flag_error_no_response_motor0:			nStringID = IDS_DEVICE_NO_RESPONSE_MOTOR0;			break;
		case sts_flag_error_no_response_motor1:			nStringID = IDS_DEVICE_NO_RESPONSE_MOTOR1;			break;
		case sts_flag_error_no_response_motor2:			nStringID = IDS_DEVICE_NO_RESPONSE_MOTOR2;			break;
		case sts_flag_error_no_response_motor3:			nStringID = IDS_DEVICE_NO_RESPONSE_MOTOR3;			break;
		case sts_flag_error_no_response_motor4:			nStringID = IDS_DEVICE_NO_RESPONSE_MOTOR4;			break;
		case sts_flag_error_no_response_motor5:			nStringID = IDS_DEVICE_NO_RESPONSE_MOTOR5;			break;
		case sts_flag_error_no_response_motor6:			nStringID = IDS_DEVICE_NO_RESPONSE_MOTOR6;			break;
		case sts_flag_error_no_response_motor7:			nStringID = IDS_DEVICE_NO_RESPONSE_MOTOR7;			break;
		case sts_flag_error_no_response_motor8:			nStringID = IDS_DEVICE_NO_RESPONSE_MOTOR8;			break;
		case sts_flag_error_motiongate:					nStringID = IDS_DEVICE_ERROR_MOTIONGATE;	break;
		case sts_flag_error_motor0:						nStringID = IDS_DEVICE_ERROR_MOTOR0;		break;
		case sts_flag_error_motor1:						nStringID = IDS_DEVICE_ERROR_MOTOR1;		break;
		case sts_flag_error_motor2:						nStringID = IDS_DEVICE_ERROR_MOTOR2;		break;
		case sts_flag_error_motor3:						nStringID = IDS_DEVICE_ERROR_MOTOR3;		break;
		case sts_flag_error_motor4:						nStringID = IDS_DEVICE_ERROR_MOTOR4;		break;
		case sts_flag_error_motor5:						nStringID = IDS_DEVICE_ERROR_MOTOR5;		break;
		case sts_flag_error_motor6:						nStringID = IDS_DEVICE_ERROR_MOTOR6;		break;
		case sts_flag_error_motor7:						nStringID = IDS_DEVICE_ERROR_MOTOR7;		break;
		case sts_flag_error_motor8:						nStringID = IDS_DEVICE_ERROR_MOTOR8;		break;
		case sts_flag_error_no_motor:					nStringID = IDS_DEVICE_ERROR_NO_MOTOR;		break;
			
		}
	}	break;

	case sts_alarm:
	{
		switch (dwStatusFlag)
		{
		case sts_flag_alarm_motor0:			break;
		case sts_flag_alarm_motor_count:	break;
		case sts_flag_alarm_restoration:	break;
		case sts_flag_alarm_all_homing:		break;
		}
	}	break;

	case sts_backup:
	{
		switch (dwStatusFlag)
		{
		case sts_flag_backup_run:		nStringID = IDS_DEVICE_BACKUP_START;				break;
		case sts_flag_backup_stop:		nStringID = IDS_DEVICE_BACKUP_STOP;					break;
		case sts_flag_backup_good:		nStringID = IDS_DEVICE_BACKUP_DONE;					break;
		case sts_flag_backup_bad:		nStringID = IDS_DEVICE_BACKUP_FAIL;					break;
		case sts_flag_comapare_start:	nStringID = IDS_DEVICE_COMPARE_START;			break;
		case sts_flag_comapare_stop:	nStringID = IDS_DEVICE_COMPARE_STOP;			break;
		case sts_flag_comapare_bad:		nStringID = IDS_DEVICE_COMPARE_FAIL;			break;
		case sts_flag_comapare_end:		nStringID = IDS_DEVICE_COMPARE_DONE;			break;
		}
	}	break;

	case sts_restore:
	{
		switch (dwStatusFlag)
		{
		case sts_flag_restore_run:				nStringID = IDS_DEVICE_RESTORATION_START;	break;
		case sts_flag_restore_stop:				nStringID = IDS_DEVICE_RESTORATION_STOP;	break;
		case sts_flag_restore_good:				nStringID = IDS_DEVICE_RESTORATION_DONE;	break;
		case sts_flag_restore_bad:				nStringID = IDS_DEVICE_RESTORATION_FAIL;	break;
		case sts_flag_restore_all_home_run:		nStringID = IDS_DEVICE_HOMING_START;		break;
		case sts_flag_restore_all_home_stop:	nStringID = IDS_DEVICE_HOMING_STOP;			break;
		case sts_flag_restore_all_home_good:	nStringID = IDS_DEVICE_HOMING_DONE;			break;
		case sts_flag_restore_all_home_bad:		nStringID = IDS_DEVICE_HOMING_FAIL;			break;
		case sts_flag_restore_all_home_finish:	nStringID = IDS_DEVICE_HOMING_FINISH;		break;
		}
	}	break;

	case sts_reset:
	{
		switch (dwStatusFlag)
		{
		case sts_flag_reset_run:	nStringID = IDS_DEVICE_RESET_START;	break;
		case sts_flag_reset_stop:	nStringID = IDS_DEVICE_RESET_STOP;	break;
		case sts_flag_reset_good:	nStringID = IDS_DEVICE_RESET_DONE;	break;
		case sts_flag_reset_bad:	nStringID = IDS_DEVICE_RESET_FAIL;	break;
		}
	}	break;

	case sts_assign:
	{
		switch (dwStatusFlag)
		{
		case sts_flag_assign_run:	nStringID = IDS_DEVICE_INIT_START;	break;
		case sts_flag_assign_stop:	nStringID = IDS_DEVICE_INIT_STOP;	break;
		case sts_flag_assign_good:	nStringID = IDS_DEVICE_INIT_DONE;	break;
		case sts_flag_assign_bad:	nStringID = IDS_DEVICE_INIT_FAIL;	break;
		}
	}	break;
	}

	if (-1 != nStringID)
	{
		strStatus.LoadString(nStringID);
	}
}

void CDeviceMotionGate::GetLastSimpleStatus(CString& strStatus)
{
	GetLastSimpleStatus(GetLastSimpleStatus(), strStatus);
}

const DWORD CDeviceMotionGate::ScanFastStatus(const DWORD dwFlag)
{
	DWORD dwResult = 0x1;

	while (dwResult)
	{
		if (dwResult & dwFlag)	break;
		dwResult <<= 1;
	}

	return dwResult;
}

const BOOL CDeviceMotionGate::IsTeachingPendantOnLine()
{
	return m_stSystemStatus.wStatus.bitTeachingPendant;
}

void CDeviceMotionGate::EnableAutoDatabaseUpdate(const BOOL bEnable)
{
	if (m_bAutoDatabaseUpdate != bEnable)
	{
		m_bAutoDatabaseUpdate = bEnable;
	}
}

const BOOL CDeviceMotionGate::IsAutoDatabaseUpdateEnabled()
{
	return m_bAutoDatabaseUpdate;
}

void CDeviceMotionGate::EnterEditMode()
{
	if (m_bEditMode)
	{
		return;
	}

	m_stDatabaseEditDeviceInfo.strName = m_stDatabaseDeviceInfo.strName;	
	m_stDatabaseEditDeviceInfo.nDeviceID = m_stDatabaseDeviceInfo.nDeviceID;
	m_stDatabaseEditDeviceInfo.dwAddress = m_stDatabaseDeviceInfo.dwAddress;

	m_stDatabaseEditDeviceInfo.nMotorCount = m_stDatabaseDeviceInfo.nMotorCount;
	m_stDatabaseEditDeviceInfo.nHomingTimeout = m_stDatabaseDeviceInfo.nHomingTimeout;
	//KKB 
	m_stDatabaseEditDeviceInfo.nNominalID = m_stDatabaseDeviceInfo.nNominalID;

	if (m_stDatabaseEditDeviceInfo.bmpImage.GetSafeHandle())
	{
		m_stDatabaseEditDeviceInfo.bmpImage.Detach();
	}

	if (m_stDatabaseDeviceInfo.bmpImage.GetSafeHandle())
	{
		m_stDatabaseEditDeviceInfo.bmpImage.Attach(::CopyImage(m_stDatabaseDeviceInfo.bmpImage, IMAGE_BITMAP, 0, 0, 0));
	}

	m_bEditMode = TRUE;
}

void CDeviceMotionGate::ExitEditMode(const BOOL bApplyEditProperty)
{
	if (FALSE == m_bEditMode)
	{
		return;
	}

	//	Apply Edit Mode
	if (bApplyEditProperty)
	{
		m_strProjectName = m_stDatabaseDeviceInfo.strName;	//	임시로 이름을 저장
		m_stDatabaseDeviceInfo.strName = m_stDatabaseEditDeviceInfo.strName;
		m_stDatabaseDeviceInfo.nDeviceID = m_stDatabaseEditDeviceInfo.nDeviceID;
		m_stDatabaseDeviceInfo.dwAddress = m_stDatabaseEditDeviceInfo.dwAddress;

		m_stDatabaseDeviceInfo.nMotorCount = m_stDatabaseEditDeviceInfo.nMotorCount;
		m_stDatabaseDeviceInfo.nHomingTimeout = m_stDatabaseEditDeviceInfo.nHomingTimeout;

		//KKB
		m_stDatabaseDeviceInfo.nNominalID = m_stDatabaseEditDeviceInfo.nNominalID;
		if (m_stDatabaseDeviceInfo.bmpImage.GetSafeHandle())
		{
			m_stDatabaseDeviceInfo.bmpImage.Detach();
		}

		if (m_stDatabaseEditDeviceInfo.bmpImage.GetSafeHandle())
		{
			m_stDatabaseDeviceInfo.bmpImage.Attach(::CopyImage(m_stDatabaseEditDeviceInfo.bmpImage, IMAGE_BITMAP, 0, 0, 0));
		}
	}

	m_bEditMode = FALSE;
}

const BOOL CDeviceMotionGate::DoMGFactoryReset(bool bAct)
{
	m_stDatabaseDeviceInfo.strName = L"";
	m_stDatabaseDeviceInfo.nMotorCount = 0;
	return TRUE;
}



CBitmap* CDeviceMotionGate::LoadImageFromDatabase(LPVOID pDatabaseRecordData, CString strProjectName)
{
	CDataBaseRecordData* pRecord = static_cast<CDataBaseRecordData*>(pDatabaseRecordData);
	CString strDatabaseName;
	CBitmap* pBitmap = GetEditSystemImage();

	if (pRecord->GetTextData(_T("Project_Name"), strDatabaseName))
	{
		if (strDatabaseName == strProjectName)
		{
			if (FALSE == pRecord->GetImageData(_T("Image"), pBitmap))
			{
				CDataBaseConfig::GetInstance()->GetInstance()->LoadNoImage(pBitmap);
			}
		}
	}

	if (0 == strProjectName.GetLength())
	{
		CDataBaseConfig::GetInstance()->GetInstance()->LoadNoImage(pBitmap);
	}

	return pBitmap;
}

const BOOL	CDeviceMotionGate::LoadFromDatabase(LPVOID pDatabaseRecordData)
{
	CDataBaseRecordData* pRecord = static_cast<CDataBaseRecordData*>(pDatabaseRecordData);

	if (m_stDatabaseDeviceInfo.bmpImage.GetSafeHandle())
	{
		m_stDatabaseDeviceInfo.bmpImage.Detach();
	}

	if (FALSE == pRecord->GetTextData(_T("Project_Name"), m_stDatabaseDeviceInfo.strName) ||
		FALSE == pRecord->GetIntergerData(_T("Motor_Count"), m_stDatabaseDeviceInfo.nMotorCount) ||
		FALSE == pRecord->GetIntergerData(_T("Homing_Timeout"), m_stDatabaseDeviceInfo.nHomingTimeout) ||
		FALSE == pRecord->GetIntergerData(_T("Nominal_ID"), m_stDatabaseDeviceInfo.nNominalID) ||
		FALSE == pRecord->GetIntergerData(_T("Restored_ID"), m_stDatabaseDeviceInfo.nRestoredID) ||
		FALSE == pRecord->GetIntergerData(_T("Device_ID"), m_stDatabaseDeviceInfo.nDeviceID))
	{
		return FALSE;
	}

	if (FALSE == pRecord->GetImageData(_T("Image"), &m_stDatabaseDeviceInfo.bmpImage))
	{
		CDataBaseConfig::GetInstance()->GetInstance()->LoadNoImage(&m_stDatabaseDeviceInfo.bmpImage);
	}

	if (FALSE == pRecord->GetIntergerData(_T("Delete"), m_bDeleted))
	{
		m_bDeleted = false;
	} 

	return TRUE;
}

const BOOL	CDeviceMotionGate::SaveToDatabase( const INT nPriority )
{
	return CDataBaseProject::GetInstance()->InsertProject(
		m_stDatabaseDeviceInfo.strName,
		m_stDatabaseDeviceInfo.nDeviceID,
		nPriority,
		m_stDatabaseDeviceInfo.nMotorCount,
		m_stDatabaseDeviceInfo.nHomingTimeout,
		m_stDatabaseDeviceInfo.nNominalID,
		m_stDatabaseDeviceInfo.nRestoredID,
		&m_stDatabaseDeviceInfo.bmpImage
	);
}


const BOOL CDeviceMotionGate::UpdateDeleteValue(BOOL bDelete)
{
	return CDataBaseProject::GetInstance()->SetDeleteValueProjcetTable(	m_stDatabaseDeviceInfo.strName,	false);
}


const BOOL CDeviceMotionGate::UpdateRestoreBackupListTable()
{
	return CDataBaseProject::GetInstance()->UpdateRestoreBackupListTable(m_stDatabaseDeviceInfo.strName, false, true);
}

const BOOL CDeviceMotionGate::CopyList(CString strOrigin, CString strTarget)
{
	return CDataBaseProject::GetInstance()->CopyList(strOrigin, strTarget, m_stDatabaseDeviceInfo.strName);
}


const BOOL	CDeviceMotionGate::UpdateDatabase()
{
	return CDataBaseProject::GetInstance()->UpdateProject(
		m_strProjectName,			//	임시로 저장한 이름
		m_stDatabaseDeviceInfo.strName,
		m_stDatabaseDeviceInfo.nMotorCount,
		m_stDatabaseDeviceInfo.nHomingTimeout,
		&m_stDatabaseDeviceInfo.bmpImage,
		m_stDatabaseDeviceInfo.nDeviceID
	);
}


const BOOL CDeviceMotionGate::UpdateChangeProject()
{
	return CDataBaseProject::GetInstance()->UpdateChangeProject(
		m_strProjectName,			//	임시로 저장한 이름
		m_stDatabaseEditDeviceInfo.strName,
		m_stDatabaseEditDeviceInfo.nMotorCount,
		m_stDatabaseEditDeviceInfo.nHomingTimeout,
		&m_stDatabaseEditDeviceInfo.bmpImage,
		m_stDatabaseEditDeviceInfo.nDeviceID
	);
}

const BOOL CDeviceMotionGate::IsDataBaseExist(CString strName)
{
	BOOL bResult = FALSE;
	CDataBaseProject* pProject = CDataBaseProject::GetInstance();

	if (pProject->OpenDataBase())
	{
		if (pProject->GetDeviceList())
		{
			while (CDataBaseRecordData* pRecord = pProject->SeekToNext())
			{
				CDeviceMotionGate* pDevice = new CDeviceMotionGate;
				if (pDevice->LoadFromDatabase(pRecord))
				{
					if (pDevice->GetDatabaseSystemName() == strName)
					{
						bResult = TRUE;

						delete pDevice;
						break;
					}
				}
				else
				{
					//	뭔가 문제가 있다.
					delete pDevice;
					ASSERT(0);
				}
			}
		}
	}

	pProject->CloseDataBase();

	return bResult;
}

const BOOL CDeviceMotionGate::InsertProjectDatabase()
{
	CDataBaseProject* pProjectData = CDataBaseProject::GetInstance();

	const INT nPriority = pProjectData->GetMaxPriority() + 1;

	BOOL bResult = pProjectData->InsertProject(
		m_stConnectedDeviceInfo.strName,
		m_stConnectedDeviceInfo.nDeviceID,
		nPriority,
		m_stConnectedDeviceInfo.nMotorCount,
		m_stConnectedDeviceInfo.nHomingTimeout,
		m_stDatabaseDeviceInfo.nNominalID,
		m_stDatabaseDeviceInfo.nRestoredID,
		NULL);

	return bResult;
}

const BOOL CDeviceMotionGate::InsertNewProjectDatabase()
{
	CDataBaseProject* pProjectData = CDataBaseProject::GetInstance();

	const INT nPriority = pProjectData->GetMaxPriority() + 1;

	BOOL bResult = pProjectData->InsertProject(
		m_stDatabaseEditDeviceInfo.strName,
		m_stDatabaseEditDeviceInfo.nDeviceID,
		nPriority,
		m_stDatabaseEditDeviceInfo.nMotorCount,
		m_stDatabaseEditDeviceInfo.nHomingTimeout,
		m_stDatabaseEditDeviceInfo.nNominalID,
		m_stDatabaseEditDeviceInfo.nRestoredID,
		&m_stDatabaseEditDeviceInfo.bmpImage);

	return bResult;
}

const BOOL CDeviceMotionGate::RebootSystem()
{
	switch (m_stateMachine)
	{
	case state_change_information:
	case state_motion_gate_get_system_status_all:
	case state_motion_gate_command:
	{
		m_stCommand.cmdGate.bitReboot = TRUE;
		return TRUE;
	}	break;
	}

	return FALSE;
}

const BOOL CDeviceMotionGate::Copy(CDeviceMotionGate* pTarget, CDeviceMotionGate* pSource)
{
	if (NULL == pTarget)
	{
		//	Validate도 필요할까?
		return FALSE;
	}

	if (NULL == pSource)
	{
		//	Validate도 필요할까?
		return FALSE;
	}

	if (1)
	{	//	Copy m_stDatabaseDeviceInfo.bmpImage
		if (pTarget->m_stDatabaseDeviceInfo.bmpImage.GetSafeHandle())
		{
			pTarget->m_stDatabaseDeviceInfo.bmpImage.Detach();
		}

		if (HGDIOBJ hBitmap = pSource->m_stDatabaseDeviceInfo.bmpImage.GetSafeHandle())
		{
			pTarget->m_stDatabaseDeviceInfo.bmpImage.Attach(CopyImage(hBitmap, IMAGE_BITMAP, 0, 0, 0));
		}
	}

	if (1)
	{	//	Copy Another part of m_stDatabaseDeviceInfo
		pTarget->m_stDatabaseDeviceInfo.dwAddress = pSource->m_stDatabaseDeviceInfo.dwAddress;
		pTarget->m_stDatabaseDeviceInfo.nHomingTimeout = pSource->m_stDatabaseDeviceInfo.nHomingTimeout;
		pTarget->m_stDatabaseDeviceInfo.nMotorCount = pSource->m_stDatabaseDeviceInfo.nMotorCount;
		pTarget->m_stDatabaseDeviceInfo.nNominalID = pSource->m_stDatabaseDeviceInfo.nNominalID;
		pTarget->m_stDatabaseDeviceInfo.nRestoredID = pSource->m_stDatabaseDeviceInfo.nRestoredID;
		pTarget->m_stDatabaseDeviceInfo.strName = pSource->m_stDatabaseDeviceInfo.strName;
	}
	
	return TRUE;
}

CDeviceMotionGate* CDeviceMotionGate::Clone()
{
	CDeviceMotionGate* pResult = new CDeviceMotionGate;

	if (FALSE == Copy(pResult, this))
	{
		delete pResult;
		return NULL;
	}

	return pResult;
}

const BOOL CDeviceMotionGate::ApplyProperty(CDeviceMotionGate* pSource)
{
	//	사용 중인 데이터가 이미 변경되기 때문에 에러나면 절대 안된다...
	return Copy(this, pSource);
}

const BOOL CDeviceMotionGate::CompareMotiongateInfo()
{
	if (m_stConnectedDeviceInfo.strName != m_stDatabaseDeviceInfo.strName ||
		m_stConnectedDeviceInfo.nMotorCount != m_stDatabaseDeviceInfo.nMotorCount ||
		m_stConnectedDeviceInfo.nHomingTimeout != m_stDatabaseDeviceInfo.nHomingTimeout)
	{
		return FALSE;
	}

	return TRUE;
}


const BOOL CDeviceMotionGate::CompareMotionGateInfoEtc()
{
	INT nMotorReal = GetConnectedMotorCount();
	INT nMotor = m_stDatabaseEditDeviceInfo.nMotorCount;
	INT nHoming = m_stDatabaseEditDeviceInfo.nHomingTimeout;
	if (m_stConnectedDeviceInfo.nMotorCount != m_stDatabaseDeviceInfo.nMotorCount ||
		m_stConnectedDeviceInfo.nHomingTimeout != m_stDatabaseDeviceInfo.nHomingTimeout)
	{
		return FALSE;
	}

	return TRUE;
}


const BOOL CDeviceMotionGate::CompareNoticeInfo()
{
	//INT nMotor = GetConnectedMotorCount();
	INT nMotor = GetMotorCount();
	INT nHoming = m_stDatabaseEditDeviceInfo.nHomingTimeout;

	if (m_stConnectedDeviceInfo.strName != m_stDatabaseDeviceInfo.strName)
	{ // 모터 개수를 비교하기 전에 현재 장치와의 이름이 다르다면 그냥 진행..
		return TRUE;
	}

	if (nMotor != m_stDatabaseDeviceInfo.nMotorCount)
	{
		/*
		* // MotionGate Setting 에서 MotorCount값을 db에 저장된 MotorCount에 맞출경우..
		if (GetEditMotorCount() != m_stDatabaseDeviceInfo.nMotorCount)
		{ 
			return FALSE;
		}
		*/
		return FALSE;
	}
	return TRUE;
}

const BOOL CDeviceMotionGate::MatchToDevice()
{
	BOOL bResult = FALSE;
	if (m_stDatabaseEditDeviceInfo.strName == m_stDatabaseDeviceInfo.strName)
	{
		if (m_stDatabaseEditDeviceInfo.nHomingTimeout == m_stDatabaseDeviceInfo.nHomingTimeout)
		{
			if (m_stDatabaseEditDeviceInfo.nMotorCount == m_stDatabaseDeviceInfo.nMotorCount)
			{ // Edit Ctrl의 모터개수를 db의 모터개수에 맞출경우..
				bResult = TRUE;
			}
		}
	}
	
	return bResult;
}


const BOOL CDeviceMotionGate::ChangeSystemInformation()
{
	switch (m_stateMachine)
	{
	case state_change_information:
	case state_motion_gate_get_system_status_all:
	case state_motion_gate_command:
	{
		m_stCommand.cmdGate.bitChangeInformation = TRUE;
		return TRUE;
	}	break;
	}

	return FALSE;
}

const BOOL CDeviceMotionGate::IsSystemStatusBusy()
{
	BOOL bBusy = FALSE;

	switch ( m_stateMachine )
	{
	case state_backup:
	case state_restoration:
	case state_factory_reset:
	case state_assign_motor_id:
	case state_home_all:
	case state_change_information:
		bBusy = TRUE;
		break;
	}

	return bBusy;
}

const BOOL CDeviceMotionGate::IsSystemStatusWarning()
{
	BOOL bWarning = FALSE;

	if ( m_stSystemStatus.wStatus.bitHoming)
	{	// 원점 탐색을 수행하고 있다면
		bWarning = TRUE;
	}
	else if (m_stSystemStatus.wStatus.bitTeachingPendant)
	{	// Teaching Pendant가 연결되어 있다면
		bWarning = TRUE;
	}

	return bWarning;
}

const BOOL CDeviceMotionGate::IsSystemStatusError()
{
	BOOL bError = FALSE;

	CString strAddr;
	GetAddress(strAddr);
	int nOpCode = m_nOpCode;
	switch (m_stateMachine)
	{
	case state_stop:
	{

	}	break;

	case state_reboot:
	{
		bError = TRUE;
	}	break;

	case state_connect:
	{
		switch (m_nOpCode)
		{
			case opCode_connect:
			case opCode_disconenct:
			case opCode_no_response:
			case opCode_scan_device:
			case opCode_read_motiongate_status:
			case opCode_standby_motiongate:
			{

			}	break;

			

			default:
			{
				bError = TRUE;
			}	break;
		}
	}	break;

	default:
	{
		if (m_stSystemStatus.btStartMotorConnectionStatus != m_stSystemStatus.btCurrentMotorConnectionStatus)
		{	//	초기 Motor 연결 상황과 현재 Motor 연결 상황이 다를 때
			bError = TRUE;
		}
		else
		{
			const INT nMotorCount = GetContinuouslyConnectedMotorCount();
			if (nMotorCount != m_stConnectedDeviceInfo.nMotorCount  || 
				nMotorCount != m_stDatabaseDeviceInfo.nMotorCount)
			{
				bError = TRUE;
			}
		}
		
		if (FALSE == bError)
		{ // 위 조건에서 모두 정상인경우만 체크.. 어차피 위에 걸리면 Error으로 반환함
			for (INT nMotor = 1; nMotor < 9; nMotor++)
			{
				BOOL bConnected = IsMotorConnected(nMotor);
				if (bConnected)
				{
					if (sts_error_motor_none != GetMotorErrorCode(nMotor))
					{
						bError = TRUE;
					}
					else
					{ // 정상인 경우
						
					}
				}
			}
		}

		

	}	break;
	}

	if (bError)
	{
		//TRACE(L"[%s] m_stateMachine : %d / m_nOpCode : %d \n", strAddr, m_stateMachine, m_nOpCode);
	}

	return bError;
}


ITEM_NODE CDeviceMotionGate::GetPLCReqNodeItem(const INT nPTNo, const INT nMotorNo)
{
	return m_stMotorInfo[nMotorNo].nodeItem[nPTNo];
}


WORD CDeviceMotionGate::GetModBusResPonseCmd(const INT nMotorNo, const INT nPTNo, const INT nCmd, const LONG lPos)
{
	WORD wResult = 0;

	switch (nCmd)
	{
	case POS_LO:
	{
		wResult = LOWORD((DWORD)(ConvertPulseToMillimeter(&m_stMotorInfo[nMotorNo], lPos) * 100));
	}	break;

	case POS_HI:
	{
		wResult = HIWORD((DWORD)(ConvertPulseToMillimeter(&m_stMotorInfo[nMotorNo], lPos) * 100));
	}	break;

	case SPEED:
	{
		wResult = WORD(ConvertPulseToMillimeter(&m_stMotorInfo[nMotorNo], lPos));
	}	break;
	default:
		break;
	}


	return wResult;
}


DOUBLE CDeviceMotionGate::ConvertPulseToMillimeter(MOTOR_PARAMETER_DATA* pstParameter, const LONG lPosition)
{
	DOUBLE dbDistance = 0;
	CString str;
	const DOUBLE dbResolution[] = {
			500,	//	index = 0
			500,	//	index = 1
			1000,	//	index = 2
			1600,	//	index = 3
			2000,	//	index = 4
			3600,	//	index = 5
			5000,	//	index = 6
			6400,	//	index = 7
			7200,	//	index = 8
			10000	//	index = 9
	};

	if ((dbResolution[pstParameter->lParameter[0]] * pstParameter->lParameter[32]) != 0)
	{
		dbDistance = static_cast<DOUBLE>(lPosition) * pstParameter->lParameter[31] / (dbResolution[pstParameter->lParameter[0]] * pstParameter->lParameter[32]);
	}

	return dbDistance;
}


BYTE CDeviceMotionGate::GetMotionGateErrorCode(BYTE byPTNo)
{
	BYTE byStatus = GetSystemStatus();
	BYTE byWarningStatus = GetSystemStatusWarning();
	BYTE byErrorStatus = GetSystemStatusError();
	

	if (byErrorStatus != 0)
	{
		return byErrorStatus;
	}

	else if (byWarningStatus != 0)
	{
		return byWarningStatus;
	}

	else
	{
		return byStatus;
	}

	return 0;
}


BYTE CDeviceMotionGate::GetSystemStatusError()
{
	BYTE byResult = 0;

	CString strAddr;
	GetAddress(strAddr);

	if (m_nLastFasResult == FMC_NO_RESPONSE || m_nLastFasResult == FMC_REFUSED)
	{
		return byResult = (m_nLastFasResult == FMC_NO_RESPONSE ? sts_error_no_response : sts_error_disconnect);
	}

	switch (m_stateMachine)
	{
	
	case state_reboot:
	{
	}	break;

	case state_connect:
	{
		switch (m_nOpCode)
		{
		case opCode_stop:
		{
			byResult = sts_error_stop_motion_while_try_connect;
		}	break;

		case opCode_MotiongateBusy:
		{
			byResult = sts_error_motion_busy_while_try_connect;
		}	break;

		case opCode_read_motiongate_info:
		{
			byResult = sts_error_read_motion_while_try_connect;
		}	break;

		case opCode_no_response:
		{
			byResult = sts_error_no_response;
		} break;

		case opCode_disconenct:
		{
			byResult = sts_error_disconnect;
		}	break;

		case opCode_backup_read_parameter :
		case opCode_backup_read_position_table:
		case opCode_enter_monitor_mode:
		case opCode_exit_monitor_mode:
		case opCode_check_monitor_mode_on:
		case opCode_check_monitor_mode_off:
		case opCode_check_device_name:
		case opCode_change_device_info:
		case opCode_write_main_parameter:
		case opCode_write_position_table:
		case opCode_save_main_parameter:
		case opCode_save_position_table:
		case opCode_confirm_main_parameter:
		case opCode_load_position_table:
		case opCode_confirm_position_table:
		case opCode_change_motiongate_id:
		case opCode_reboot_motiongate:
		case opCode_homing_servo_on:
		case opCode_homing_start:
		case opCode_homing_wait:
		case opCode_homing_stop:
		{
			byResult = 0x80 + (m_nOpCode - 6);
		}	break;

		}
	}	break;

	default:
	{
		if (1 > (m_stConnectedDeviceInfo.strInformation.Trim()).GetLength() )
		{
			byResult = sts_error_system_no_name;
		}
		
		if (m_stSystemStatus.btStartMotorConnectionStatus != m_stSystemStatus.btCurrentMotorConnectionStatus)
		{	//	초기 Motor 연결 상황과 현재 Motor 연결 상황이 다를 때
			byResult = sts_error_discord_current_init_motor_status_while_try_connect;
		}
		else
		{
			const INT nMotorCount = GetContinuouslyConnectedMotorCount();
			if (nMotorCount != m_stConnectedDeviceInfo.nMotorCount)
			{
				byResult = sts_error_discord_current_connected_motor_count_while_try_connect;
			}

			if(nMotorCount != m_stDatabaseDeviceInfo.nMotorCount)
			{
				byResult = sts_error_discord_motorcount_in_db_while_try_connect;
			}
		}
		
	}	break;
	}




	return byResult;
}


BYTE CDeviceMotionGate::GetSystemStatusWarning()
{
	BYTE byResult = 0;

	if (m_stSystemStatus.wStatus.bitHoming)
	{	// 원점 탐색을 수행하고 있다면
		byResult = sts_warning_homing;
	}
	else if (m_stSystemStatus.wStatus.bitTeachingPendant)
	{	// Teaching Pendant가 연결되어 있다면
		byResult = sts_warning_teaching_pendant;
	}

	return byResult;
}



BYTE CDeviceMotionGate::GetSystemStatus()
{	
	BYTE byResult = 0;
	// '0' 값을 Normal로 사용할 예정이기에 m_stateMachine + 1을 진행해준다.

	switch (m_stateMachine)
	{
	
	/*case state_stop:
	{

	}
			state_connect,

	case state_reboot:
	{
		byResult = sts_status_reboot;
	}	break;*/
	case state_stop:
	{
		byResult = sts_status_stop;
	}	break;
	
	case state_connect:
	case state_motion_gate_get_system_status_all:
	case state_motion_gate_command:
	{
		byResult = sts_status_normal;
	} break;

	case state_backup:
	case state_restoration:
	case state_factory_reset:
	case state_assign_motor_id:
	case state_home_all:
	case state_change_information:
	{
		byResult = m_stateMachine - 1;
	}	break;

	default:
	{
		byResult = m_stateMachine + 1;
	}
		break;
	}

	return byResult;
}


BYTE CDeviceMotionGate::GetMotorError(INT nAxis, INT nPTNum)
{
	BYTE bySystemStatusErrorCode = 0;
	if (0 < nAxis && 9 > nAxis) bySystemStatusErrorCode = static_cast<BYTE>(GetMotorErrorCode(nAxis));
	
	if (bySystemStatusErrorCode)
	{
		switch (bySystemStatusErrorCode)
		{
		case sts_error_motor_overcurrent:
		case sts_error_motor_overspeed:
		case sts_error_motor_position_tracking:
		case sts_error_motor_overload:
		case sts_error_motor_overheat:
		case sts_error_motor_over_regeratived_voltage:
		case sts_error_motor_connection:
		case sts_error_motor_encoder_connection:
		case sts_error_motor_motor_voltate_error:
		case sts_error_motor_inposition_error:
		case sts_error_motor_drive_system_error:
		case sts_error_motor_drive_rom_error:
		case sts_error_motor_drive_voltage_error:
		case sts_error_motor_position_overflow:
		case sts_error_motor_no_clam_load:
		{
			return bySystemStatusErrorCode;
		}	break;
		
		default:
			break;
		}
	}

	else
	{
		if (31 > nPTNum && 9 > nAxis)
		{
			/* nodeItem[0] 은 실제적으로 nodeItem[1] 을 갖고있기에,	nodeItem[0 ~ 30] / pt 1 ~ 31까지만 데이터를 갖고있다.
			
			*/
			// 기존..? 무슨로직 ?bySystemStatusErrorCode = m_stMotorInfo[nAxis].nodeItem[nPTNum].lPosition < 1 ? sts_error_access_pt_not_exist : bySystemStatusErrorCode;
		}

		if (30 < nPTNum)
		{
			//bySystemStatusErrorCode = sts_error_access_pt_out_of_range;
		}

		if ( 1 > nAxis || 8 < nAxis)
		{
			bySystemStatusErrorCode = sts_error_access_motor_out_of_range;
		}

		if (9 > nAxis)
		{
			bySystemStatusErrorCode = ((m_wCurAxisConnection >> nAxis) & 0x01) == 0 ? sts_error_access_motor_not_exist : bySystemStatusErrorCode;
		}
	}

	return bySystemStatusErrorCode;
}


const INT CDeviceMotionGate::GetDeletedStatus()
{
	return m_bDeleted;
}


void CDeviceMotionGate::MotionGateInformRequest(BOOL bRun)
{
	m_bInformRequest = bRun;
}


const BOOL CDeviceMotionGate::GetMotionGateInformRequest()
{
	return m_bInformRequest;
}


void CDeviceMotionGate::SetRun(BOOL bRun)
{
	m_bRun = bRun;
}


void CDeviceMotionGate::StartProgressTimer(BOOL bStart)
{
	if (!m_bProgressTimer)
	{
		m_bProgressTimer = TRUE;
		m_dwProgressStartTickCount = ::GetTickCount64();
	}
}


void CDeviceMotionGate::StopProgressTimer(BOOL bStart)
{
	if (m_bProgressTimer)
	{
		m_bProgressTimer = FALSE;
		//m_dwProgressStopTickCount = ::GetTickCount64();
	}
}


DWORD CDeviceMotionGate::GetProgressTime()
{
	m_dwProgressStopTickCount = ::GetTickCount64();
	return (m_dwProgressStopTickCount - m_dwProgressStartTickCount) / 100;
}


void CDeviceMotionGate::SetEndTimeCheck(BOOL bEnd)
{
	m_bEndTimeCheck = bEnd;
}


BOOL CDeviceMotionGate::GetEndTimeCheck()
{
	return m_bEndTimeCheck;
}


void CDeviceMotionGate::SetProgressEndTime(DWORD dwTime)
{
	m_dwProgressEndTime = dwTime;
}


DWORD CDeviceMotionGate::GetProgressEndTime()
{
	return m_dwProgressEndTime;
}


void CDeviceMotionGate::GetMotorErrorStatus(CString& strError, INT nIndex)
{
	switch (GetMotorErrorCode(nIndex))
	{ // 실제 db에 저장..
	case sts_error_motor_overcurrent:
	{strError.Format(_T("Motor[%d] - overcurrent"), nIndex); m_bMotorError[nIndex] = TRUE; }	break;

	case sts_error_motor_overspeed:
	{strError.Format(_T("Motor[%d] - overspeed"), nIndex); m_bMotorError[nIndex] = TRUE; }	break;

	case sts_error_motor_position_tracking:
	{strError.Format(_T("Motor[%d] - position tracking"), nIndex); m_bMotorError[nIndex] = TRUE; }	break;

	case sts_error_motor_overload:
	{strError.Format(_T("Motor [%d] - overload"), nIndex); m_bMotorError[nIndex] = TRUE; }	break;

	case sts_error_motor_overheat:
	{strError.Format(_T("Motor[%d] - overheat"), nIndex); m_bMotorError[nIndex] = TRUE; }	break;

	case sts_error_motor_over_regeratived_voltage:
	{strError.Format(_T("Motor[%d] - over regeratived voltage"), nIndex); m_bMotorError[nIndex] = TRUE; }	break;

	case sts_error_motor_connection:
	{strError.Format(_T("Motor[%d] - connection error"), nIndex); m_bMotorError[nIndex] = TRUE; }	break;

	case sts_error_motor_encoder_connection:
	{strError.Format(_T("Motor[%d] - encoder connection error"), nIndex); m_bMotorError[nIndex] = TRUE; }	break;

	case sts_error_motor_motor_voltate_error:
	{strError.Format(_T("Motor[%d] - voltate error"), nIndex); m_bMotorError[nIndex] = TRUE; }	break;

	case sts_error_motor_inposition_error:
	{strError.Format(_T("Motor[%d] - inposition error"), nIndex); m_bMotorError[nIndex] = TRUE; }	break;

	case sts_error_motor_drive_system_error:
	{strError.Format(_T("Motor[%d] - drive system error"), nIndex); m_bMotorError[nIndex] = TRUE; }	break;

	case sts_error_motor_drive_rom_error: // 12
	{strError.Format(_T("Motor[%d] - drive rom error"), nIndex); m_bMotorError[nIndex] = TRUE; }	break;

	case sts_error_motor_drive_voltage_error: // = 14 
	{strError.Format(_T("Motor[%d] - drive voltage error"), nIndex); m_bMotorError[nIndex] = TRUE; }	break;

	case sts_error_motor_position_overflow:
	{strError.Format(_T("Motor[%d] - position overflow"), nIndex); m_bMotorError[nIndex] = TRUE; }	break;

	case sts_error_motor_position_change:
	{strError.Format(_T("Motor[%d] - style error"), nIndex); m_bMotorError[nIndex] = TRUE; }	break;

	case sts_error_motor_break_short:
	{strError.Format(_T("Motor[%d] - break error"), nIndex); m_bMotorError[nIndex] = TRUE; }	break;

	case sts_error_motor_struct_postion_change:
	{strError.Format(_T("Motor[%d] - Struct position error"), nIndex); m_bMotorError[nIndex] = TRUE; }	break;

	case sts_error_motor_no_clam_load: // 20
	{strError.Format(_T("Motor[%d] - no clam load"), nIndex); m_bMotorError[nIndex] = TRUE; }	break;

	case sts_error_access_motor_not_exist:
	{strError.Format(_T("Motor[%d] - not exist"), nIndex); m_bMotorError[nIndex] = TRUE; }	break;

	case sts_error_access_motor_out_of_range: // 모터 접근 범위 초과
	{strError.Format(_T("Motor[%d] - out of range"), nIndex); m_bMotorError[nIndex] = TRUE; }	break;

	case sts_error_access_pt_not_exist:
	{strError.Format(_T("Motor[%d] - access PT('0') not exist"), nIndex); m_bMotorError[nIndex] = TRUE; }	break;

	case sts_error_access_pt_out_of_range: // 24
	{strError.Format(_T("Motor[%d] - access PT num out of range"), nIndex); m_bMotorError[nIndex] = TRUE; }	break;
	default:
	{
		if (IsMotorBusy(nIndex))
		{
			strError.Format(_T("Motor[%d] - Busy"), nIndex);
		}
		else
		{
			if (m_bMotorError[nIndex])
			{
				m_bMotorError[nIndex] = FALSE;
				strError.Format(_T("Motor[%d] - Recovered."), nIndex);
			}
			else
			{
				strError.Format(_T("Motor[%d] - No_Error"), nIndex);
			}
			
		}
	}

	break;
	}
}

CString CDeviceMotionGate::GetMotorErrorText(INT nIndex)
{ // Imform UI 에 나타내기 위한 함수..
	CString strError = _T("");
	INT nError;
	
	switch (nError = GetMotorErrorCode(nIndex))
	{
	case sts_error_motor_overcurrent:
	{strError.Format(_T("Motor[%d] - overcurrent"), nIndex); }	break;

	case sts_error_motor_overspeed:
	{strError.Format(_T("Motor[%d] - overspeed"), nIndex); }	break;

	case sts_error_motor_position_tracking:
	{strError.Format(_T("Motor[%d] - position tracking"), nIndex); }	break;

	case sts_error_motor_overload:
	{strError.Format(_T("Motor [%d] - overload"), nIndex); }	break;

	case sts_error_motor_overheat:
	{strError.Format(_T("Motor[%d] - overheat"), nIndex); }	break;

	case sts_error_motor_over_regeratived_voltage:
	{strError.Format(_T("Motor[%d] - over regeratived voltage"), nIndex); }	break;

	case sts_error_motor_connection:
	{strError.Format(_T("Motor[%d] - connection error"), nIndex); }	break;

	case sts_error_motor_encoder_connection:
	{strError.Format(_T("Motor[%d] - encoder connection error"), nIndex); }	break;

	case sts_error_motor_motor_voltate_error:
	{strError.Format(_T("Motor[%d] - voltate error"), nIndex); }	break;

	case sts_error_motor_inposition_error:
	{strError.Format(_T("Motor[%d] - inposition error"), nIndex); }	break;

	case sts_error_motor_drive_system_error:
	{strError.Format(_T("Motor[%d] - drive system error"), nIndex); }	break;

	case sts_error_motor_drive_rom_error: // 12
	{strError.Format(_T("Motor[%d] - drive rom error"), nIndex); }	break;

	case sts_error_motor_drive_voltage_error: // = 14 
	{strError.Format(_T("Motor[%d] - drive voltage error"), nIndex); }	break;

	case sts_error_motor_position_overflow:
	{strError.Format(_T("Motor[%d] - position overflow"), nIndex); }	break;

	case sts_error_motor_position_change:
	{strError.Format(_T("Motor[%d] - style error"), nIndex); }	break;

	case sts_error_motor_break_short:
	{strError.Format(_T("Motor[%d] - break error"), nIndex); }	break;

	case sts_error_motor_struct_postion_change:
	{strError.Format(_T("Motor[%d] - Struct position error"), nIndex, nError); }	break;

	case sts_error_motor_no_clam_load: // 20
	{strError.Format(_T("Motor[%d] - no clam load"), nIndex); }	break;

	case sts_error_access_motor_not_exist:
	{strError.Format(_T("Motor[%d] - not exist"), nIndex); }	break;

	case sts_error_access_motor_out_of_range: // 모터 접근 범위 초과
	{strError.Format(_T("Motor[%d] - out of range"), nIndex); }	break;

	case sts_error_access_pt_not_exist:
	{strError.Format(_T("Motor[%d] - access PT('0') not exist"), nIndex); }	break;

	case sts_error_access_pt_out_of_range: // 24
	{strError.Format(_T("Motor[%d] - access PT num out of range"), nIndex); }	break;
	default:
	{
		if (0 == nIndex)
		{ // 0번 모터만 Text 알림을 띄운다. 나머지 모터는 에러상태만 띄운다.
			IsMotorBusy(nIndex) ? strError.Format(_T("Motor[%d] - Busy"), nIndex) : strError.Format(_T("Motor[%d] - No_Error"), nIndex);
		}
	}

	break;
	}

	return strError;
}



BOOL CDeviceMotionGate::IsMotorExist(INT nAxis)
{
	BOOL bResult = (m_wCurAxisConnection >> nAxis) & 0x01 ? TRUE : FALSE;
	return bResult;
}


void CDeviceMotionGate::InsertLog(INT nLevel, CString strInform)
{
	CWnd* pWnd = AfxGetMainWnd();
	CDataBaseLog* pBase = CDataBaseLog::GetInstance();
	
	OPERATIONLOG_INFORMATION logInform;
	CString strStatus;
	CString strName;  strName = GetSystemName();
	CString strIP;  GetAddress(strIP);

	if (1)
	{
		CTime time = CTime::GetCurrentTime();
		logInform.time = time;
		logInform.strTime.Format(_T("%04d-%02d-%02d  %02d:%02d:%02d"),
								 time.GetYear(), time.GetMonth(), time.GetDay(), 
								 time.GetHour(), time.GetMinute(), time.GetSecond());
		logInform.nLevel = nLevel;
		logInform.strDeviceIP = strIP;
		logInform.strDeviceName = strName == L"" ? _T("Unknown.") : strName;
		logInform.strInform = strInform;
	}

	if (strInform == L"")
	{
		return;
	}

	pBase->InsertLogData(nLevel, strInform, strName, strIP);
	m_opLogInform = logInform;

}

CString CDeviceMotionGate::GetCurrentSystemStatus(INT nLevel)
{
	CString str = NULL;

	if (STS_LOG_NORMAL == nLevel)
	{
		switch (m_stateMachine)
		{
		case state_stop: str.Format(_T("Stop")); break;
		case state_connect: str.Format(_T("Try connecting")); break;
		case state_backup: str.Format(_T("Backup")); break;
		case state_restoration: str.Format(_T("Restoration")); break;
		case state_factory_reset: str.Format(_T("Reset")); break;
		case state_assign_motor_id: str.Format(_T("Motor ID Assign")); break;
		case state_home_all: str.Format(_T("Home All")); break;
		case state_change_information: str.Format(_T("Change Information")); break;
		default: break;
		}

		if (1)
		{
			(m_bConnected == 1  ? str.Format(_T("Connect success")) : str);
		}
	}

	if (STS_LOG_WARNING == nLevel)
	{// IsSystemStatusWarning 참조..
		if (m_stSystemStatus.wStatus.bitHoming)
		{	// 원점 탐색을 수행하고 있다면
			str.Format(_T("Homing"));
		}
		else if (m_stSystemStatus.wStatus.bitTeachingPendant)
		{	// Teaching Pendant가 연결되어 있다면
			str.Format(_T("Connecting Teaching Pendant"));
		}

		/*
		if (1) // Test
		{
			switch (m_nOpCode)
			{
			case opCode_stop: str.Format(_T(" Device stop "));	break;
			case opCode_disconenct: str.Format(_T(" Device disconnected "));	break;
			case opCode_no_response: str.Format(_T(" Convertor no response "));	break;
			case opCode_scan_device: str.Format(_T(" Scan device "));	break;
			case opCode_read_motiongate_status: str.Format(_T(" Read motiongate "));	break;
			case opCode_MotiongateBusy: str.Format(_T(" Device busy "));	break;			//	Teaching Pendant가 연결되었을 때
			case opCode_read_motiongate_info: str.Format(_T(" Read MotionGate Information "));	break;
			case opCode_backup_read_parameter: str.Format(_T(" Backup read paramete "));	break;
			case opCode_backup_read_position_table: str.Format(_T(" Backup read PositionTable "));	break;
			case opCode_enter_monitor_mode: str.Format(_T(" Enter monitor mode "));	break;
			case opCode_exit_monitor_mode: str.Format(_T(" Exit monitor mode "));	break;
			case opCode_check_monitor_mode_on: str.Format(_T(" Monitor mode on "));	break;
			case opCode_check_monitor_mode_off: str.Format(_T(" Monitor mode off "));	break;
			case opCode_notify_backup_complete: str.Format(_T(" Backup complete "));	break;
			case opCode_check_device_name: str.Format(_T(" Check device Name"));	break;
			case opCode_change_device_info: str.Format(_T(" Change device inform "));	break;
			case opCode_write_main_parameter: str.Format(_T(" Write main parameter "));	break;
			case opCode_write_position_table: str.Format(_T(" Write position table "));	break;
			case opCode_save_main_parameter: str.Format(_T(" Save main parameter "));	break;
			case opCode_save_position_table: str.Format(_T(" Save position table "));	break;
			case opCode_confirm_main_parameter: str.Format(_T(" Confirm main parameter "));	break;
			case opCode_load_position_table: str.Format(_T(" Load position table "));	break;
			case opCode_confirm_position_table: str.Format(_T(" Confirm position table "));	break;
			case opCode_change_motiongate_id: str.Format(_T(" Change motiongate ID "));	break;
			case opCode_reboot_motiongate: str.Format(_T(" Reboot motiongate "));	break;
			case opCode_homing_servo_on: str.Format(_T(" Homing Servo on "));	break;
			case opCode_homing_start: str.Format(_T(" Homing Start "));	break;
			case opCode_homing_wait: str.Format(_T(" Homing Wait "));	break;
			case opCode_homing_stop: str.Format(_T(" Homing Stop "));	break;
			}
		}
		*/
		
	}

	if (STS_LOG_ERROR == nLevel)
	{
		switch (m_stateMachine)
		{
		case state_reboot:
		{
			str.Format(_T(" Motiongate reboot"));	break;
		}	break;

		case state_connect:
		{
			switch (m_nOpCode)
			{
			case opCode_stop: str.Format(_T(" Device stop while connecting MotionGate"));	break;
			case opCode_MotiongateBusy: str.Format(_T(" Device busy while connecting MotionGate"));	break;			//	Teaching Pendant가 연결되었을 때
			case opCode_read_motiongate_info: str.Format(_T("Read MotionGate Information while connecting MotionGate"));	break;
			case opCode_backup_read_parameter: str.Format(_T(" Backup read paramete while connecting MotionGate"));	break;
			case opCode_backup_read_position_table: str.Format(_T(" Backup read PositionTable while connecting MotionGate"));	break;
			case opCode_enter_monitor_mode: str.Format(_T(" Enter monitor mode while connecting MotionGate"));	break;
			case opCode_exit_monitor_mode: str.Format(_T(" Exit monitor mode while connecting MotionGate"));	break;
			case opCode_check_monitor_mode_on: str.Format(_T(" Monitor mode on while connecting MotionGate"));	break;
			case opCode_check_monitor_mode_off: str.Format(_T(" Monitor mode off while connecting MotionGate"));	break;
			case opCode_notify_backup_complete: str.Format(_T(" Backup complete while connecting MotionGate"));	break;
			case opCode_check_device_name: str.Format(_T(" Check device Namewhile connecting MotionGate"));	break;
			case opCode_change_device_info: str.Format(_T(" Change device inform while connecting MotionGate"));	break;
			case opCode_write_main_parameter: str.Format(_T(" Write main parameter while connecting MotionGate"));	break;
			case opCode_write_position_table: str.Format(_T(" Write position table while connecting MotionGate"));	break;
			case opCode_save_main_parameter: str.Format(_T(" Save main parameter while connecting MotionGate"));	break;
			case opCode_save_position_table: str.Format(_T(" Save position table while connecting MotionGate"));	break;
			case opCode_confirm_main_parameter: str.Format(_T(" Confirm main parameter while connecting MotionGate"));	break;
			case opCode_load_position_table: str.Format(_T(" Load position table while connecting MotionGate"));	break;
			case opCode_confirm_position_table: str.Format(_T(" Confirm position table while connecting MotionGate"));	break;
			case opCode_change_motiongate_id: str.Format(_T(" Change motiongate ID while connecting MotionGate"));	break;
			case opCode_reboot_motiongate: str.Format(_T(" Reboot motiongate while connecting MotionGate"));	break;
			case opCode_homing_servo_on: str.Format(_T(" Homing Servo on while connecting MotionGate"));	break;
			case opCode_homing_start: str.Format(_T(" Homing Start while connecting MotionGate"));	break;
			case opCode_homing_wait: str.Format(_T(" Homing Wait while connecting MotionGate"));	break;
			case opCode_homing_stop: str.Format(_T(" Homing Stop while connecting MotionGate"));	break;
			}
		}	break;

		default:
		{
			if (IsInvalidMotorConnected())
			{	//	초기 Motor 연결 상황과 현재 Motor 연결 상황이 다를 때
				//str.Format(_T("Not equal initial mottor connection status and current motor connection status"));
				str.Format(_T("Inital motor connection status ≠ Current motor connection status"));
			}
			else
			{
				const INT nMotorCount = GetContinuouslyConnectedMotorCount();
				if (nMotorCount != m_stConnectedDeviceInfo.nMotorCount ||
					nMotorCount != m_stDatabaseDeviceInfo.nMotorCount)
				{
					//str.Format(_T("The actual number of motors and the number of storage motors are different."));
					str.Format(_T("Device Motor Count ≠ Configuration Motor Count"));
				}
			}
		}	break;

		}
	}

	return str == L"" ? NULL : str;
}


/*
CString CDeviceMotionGate::GetCurrentSystemStatus(INT nLevel)
{
	CString str = NULL;

	if (STS_LOG_NORMAL == nLevel)
	{
		switch (m_stateMachine)
		{
		case state_stop: str.Format(_T("Stop")); break;
		case state_motion_gate_get_system_status_all:
			str.Format(_T("state_motion_gate_get_system_status_all")); break;
		case state_motion_gate_command :
			str.Format(_T("state_motion_gate_command")); break;
		case state_reboot: str.Format(_T("Reboot")); break;
		case state_connect: str.Format(_T("Try connect")); break;
		case state_backup: str.Format(_T("Backup")); break;
		case state_restoration: str.Format(_T("Restoration")); break;
		case state_factory_reset: str.Format(_T("Reset")); break;
		case state_assign_motor_id: str.Format(_T("Motor ID Assign")); break;
		case state_home_all: str.Format(_T("Home All")); break;
		case state_change_information: str.Format(_T("Change Information")); break;
		default: break;
		}
	}

	if (STS_LOG_WARNING == nLevel)
	{
		switch (m_nOpCode)
		{
		case opCode_connect : str.Format(_T(" Device connect (Detail State)"));	break;
		case opCode_standby_motiongate : str.Format(_T(" Device standby"));	break;
		case opCode_stop: str.Format(_T(" Device stop (Detail State)"));	break;
		case opCode_disconenct: str.Format(_T(" Device disconnected (Detail State)"));	break;
		case opCode_no_response: str.Format(_T(" Convertor no response (Detail State)"));	break;
		case opCode_scan_device: str.Format(_T(" Scan device (Detail State)"));	break;
		case opCode_read_motiongate_status: str.Format(_T(" Read motiongate (Detail State)"));	break;
		case opCode_MotiongateBusy: str.Format(_T(" Device busy (Detail State)"));	break;			//	Teaching Pendant가 연결되었을 때
		case opCode_read_motiongate_info: str.Format(_T(" Read MotionGate Information (Detail State)"));	break;
		case opCode_backup_read_parameter: str.Format(_T(" Backup read paramete (Detail State)"));	break;
		case opCode_backup_read_position_table: str.Format(_T(" Backup read PositionTable (Detail State)"));	break;
		case opCode_enter_monitor_mode: str.Format(_T(" Enter monitor mode (Detail State)"));	break;
		case opCode_exit_monitor_mode: str.Format(_T(" Exit monitor mode (Detail State)"));	break;
		case opCode_check_monitor_mode_on: str.Format(_T(" Monitor mode on (Detail State)"));	break;
		case opCode_check_monitor_mode_off: str.Format(_T(" Monitor mode off (Detail State)"));	break;
		case opCode_notify_backup_complete: str.Format(_T(" Backup complete (Detail State)"));	break;
		case opCode_check_device_name: str.Format(_T(" Check device Name(Detail State)"));	break;
		case opCode_change_device_info: str.Format(_T(" Change device inform (Detail State)"));	break;
		case opCode_write_main_parameter: str.Format(_T(" Write main parameter (Detail State)"));	break;
		case opCode_write_position_table: str.Format(_T(" Write position table (Detail State)"));	break;
		case opCode_save_main_parameter: str.Format(_T(" Save main parameter (Detail State)"));	break;
		case opCode_save_position_table: str.Format(_T(" Save position table (Detail State)"));	break;
		case opCode_confirm_main_parameter: str.Format(_T(" Confirm main parameter (Detail State)"));	break;
		case opCode_load_position_table: str.Format(_T(" Load position table (Detail State)"));	break;
		case opCode_confirm_position_table: str.Format(_T(" Confirm position table (Detail State)"));	break;
		case opCode_change_motiongate_id: str.Format(_T(" Change motiongate ID (Detail State)"));	break;
		case opCode_reboot_motiongate: str.Format(_T(" Reboot motiongate (Detail State)"));	break;
		case opCode_homing_servo_on: str.Format(_T(" Homing Servo on (Detail State)"));	break;
		case opCode_homing_start: str.Format(_T(" Homing Start (Detail State)"));	break;
		case opCode_homing_wait: str.Format(_T(" Homing Wait (Detail State)"));	break;
		case opCode_homing_stop: str.Format(_T(" Homing Stop (Detail State)"));	break;
		}

	}

	if (STS_LOG_ERROR == nLevel)
	{
		switch (m_stateMachine)
		{
			case state_reboot:
			{
				str.Format(_T(" Motiongate reboot"));	break;
			}	break;

			case state_connect:
			{
				switch (m_nOpCode)
				{
				case opCode_stop: str.Format(_T(" Device stop When MotionGate Connected"));	break;
				case opCode_MotiongateBusy: str.Format(_T(" Device busy When MotionGate Connected"));	break;			//	Teaching Pendant가 연결되었을 때
				case opCode_read_motiongate_info: str.Format(_T(" Read MotionGate Information When MotionGate Connected"));	break;
				case opCode_backup_read_parameter: str.Format(_T(" Backup read paramete When MotionGate Connected"));	break;
				case opCode_backup_read_position_table: str.Format(_T(" Backup read PositionTable When MotionGate Connected"));	break;
				case opCode_enter_monitor_mode: str.Format(_T(" Enter monitor mode When MotionGate Connected"));	break;
				case opCode_exit_monitor_mode: str.Format(_T(" Exit monitor mode When MotionGate Connected"));	break;
				case opCode_check_monitor_mode_on: str.Format(_T(" Monitor mode on When MotionGate Connected"));	break;
				case opCode_check_monitor_mode_off: str.Format(_T(" Monitor mode off When MotionGate Connected"));	break;
				case opCode_notify_backup_complete: str.Format(_T(" Backup complete When MotionGate Connected"));	break;
				case opCode_check_device_name: str.Format(_T(" Check device NameWhen MotionGate Connected"));	break;
				case opCode_change_device_info: str.Format(_T(" Change device inform When MotionGate Connected"));	break;
				case opCode_write_main_parameter: str.Format(_T(" Write main parameter When MotionGate Connected"));	break;
				case opCode_write_position_table: str.Format(_T(" Write position table When MotionGate Connected"));	break;
				case opCode_save_main_parameter: str.Format(_T(" Save main parameter When MotionGate Connected"));	break;
				case opCode_save_position_table: str.Format(_T(" Save position table When MotionGate Connected"));	break;
				case opCode_confirm_main_parameter: str.Format(_T(" Confirm main parameter When MotionGate Connected"));	break;
				case opCode_load_position_table: str.Format(_T(" Load position table When MotionGate Connected"));	break;
				case opCode_confirm_position_table: str.Format(_T(" Confirm position table When MotionGate Connected"));	break;
				case opCode_change_motiongate_id: str.Format(_T(" Change motiongate ID When MotionGate Connected"));	break;
				case opCode_reboot_motiongate: str.Format(_T(" Reboot motiongate When MotionGate Connected"));	break;
				case opCode_homing_servo_on: str.Format(_T(" Homing Servo on When MotionGate Connected"));	break;
				case opCode_homing_start: str.Format(_T(" Homing Start When MotionGate Connected"));	break;
				case opCode_homing_wait: str.Format(_T(" Homing Wait When MotionGate Connected"));	break;
				case opCode_homing_stop: str.Format(_T(" Homing Stop When MotionGate Connected"));	break;
				}
			}	break;

			default:
			{
				if (IsInvalidMotorConnected())
				{	//	초기 Motor 연결 상황과 현재 Motor 연결 상황이 다를 때
					str.Format(_T("Not equal initial mottor connection status and current motor connection status"));
				}
				else
				{
					const INT nMotorCount = GetContinuouslyConnectedMotorCount();
					if (nMotorCount != m_stConnectedDeviceInfo.nMotorCount ||
						nMotorCount != m_stDatabaseDeviceInfo.nMotorCount)
					{
						str.Format(_T("The actual number of motors and the number of storage motors are different."));
					}
				}
			}	break;

		}
	}

	return str == L"" ? NULL : str;
}
*/



BOOL CDeviceMotionGate::LogDataChanged()
{
	if (m_CurLog.strNormal == _T("Connect"))
	{
		INT a = 0;
	}
	BOOL bCheck1 = (m_CurLog.strNormal != L"" || m_CurLog.strWarning != L"" || m_CurLog.strDeviceError != L"");
	bool bCheck2 = (m_CurLog.strNormal != m_PrevLog.strNormal || 
					m_CurLog.strWarning != m_PrevLog.strWarning || 
					m_CurLog.strDeviceError != m_PrevLog.strDeviceError ||
					m_CurLog.strMotorError != m_PrevLog.strMotorError);

	if (bCheck1 && bCheck2)
	{
		return TRUE;
	}
	return FALSE;
}


BOOL CDeviceMotionGate::SetOperationLogAll(INT nCursel)
{
	CDataBaseLog* pBase = CDataBaseLog::GetInstance();
	CString strInform, strID, strIP, strTime;
	if (pBase->OpenDataBase())
	{
		if (pBase->GetLogList(_T("Program Executed")))
		{
			if (pBase->SeekToBegin()) // 마지막 Time 체크..
			{
				//while (CDataBaseRecordData* pRecord = pBase->SeekToNext())
				//{
				//	//pBase->GetLogInformationAll(strInform, strIP, strID, strTime);
				//}

			}
			
		}
	}
	return TRUE;
}





const BOOL CDeviceMotionGate::IsMotorBusy(INT nAxis)
{
	const BOOL bBusy = m_stSystemStatus.stMotorStatus[nAxis].wStatus.bitMovingForward ||
		m_stSystemStatus.stMotorStatus[nAxis].wStatus.bitMovingBackward ||
		m_stSystemStatus.stMotorStatus[nAxis].wStatus.bitOriginRun;

	return bBusy;
}




void CDeviceMotionGate::ServoControl(INT nIndex, BOOL bEnable)
{
	DoCmdEnableServo(nIndex, bEnable);
}


void CDeviceMotionGate::InitMotorState()
{// 만일 Motor에러가 발생한 상태에서 ID 변경시 이전모터 상태 이력 가지고 있음..
	for (INT i = 0; i <= 8; i++)
	{
		m_stSystemStatus.stMotorStatus[i].wPosInteger = 0;
		m_stSystemStatus.stMotorStatus[i].wPosDecimal = 0;
		m_stSystemStatus.stMotorStatus[i].wStatus.wValue = 0;
		m_stSystemStatus.stMotorStatus[i].wPositionTableNo = 0;
		m_stSystemStatus.stMotorStatus[i].btErrorCode = 0;
		m_stSystemStatus.opStatus[i].wValue = 0;
		m_stSystemStatus.dwLifeCount[i] = 0;
		m_stSystemStatus.bMotorLifeDetected[i] = false;
		m_stSystemStatus.bMotorLifeCycleDetected[i] = false;
	}
}


void CDeviceMotionGate::SetCommandTestResult(CString strCommand, INT nResult)
{
	CString str;
	
	str.Format(_T(" : %s (%d)"), nResult != FMM_OK ? _T("No FMM_OK") : _T("FMM_OK"), nResult);
	m_strCmdTest = strCommand + str;
}


CString CDeviceMotionGate::GetCommandTestResult()
{
	return m_strCmdTest;
}


void CDeviceMotionGate::ActiveOperationLog(INT nState, CString strMessage)
{
	InsertLog(nState, strMessage);
	if (strMessage != _T(""))
	{
		m_opLogInform.dwAddress = GetAddress();
		CWnd* pWnd = AfxGetMainWnd();
		::SendMessage(pWnd->m_hWnd, CMainFrame::UM_SEND_LOGMESSAGE_FOR_OPERATION_LOG, reinterpret_cast<WPARAM>(&m_opLogInform), 0);
	}
}


const BOOL CDeviceMotionGate::GetMasterConnectStatus()
{
	return m_stSystemStatus.wStatus.bitNetworkOn == 1 ? TRUE : FALSE;
}



void CDeviceMotionGate::SetPTLifeAlarmDetected(INT nAxis, BOOL bAlarm)
{
	m_stSystemStatus.bMotorLifeDetected[nAxis] = bAlarm;
}


BOOL CDeviceMotionGate::GetPTLifeAlarmDetected(INT nAxis)
{
	return m_stSystemStatus.bMotorLifeDetected[nAxis];
}


void CDeviceMotionGate::SetPTLifeCycleAlarmDetected(INT nAxis, BOOL bAlarm)
{
	m_stSystemStatus.bMotorLifeCycleDetected[nAxis] = bAlarm;
}


const BOOL CDeviceMotionGate::GetPTLifeCycleAlarmDetected(INT nAxis)
{
	return m_stSystemStatus.bMotorLifeCycleDetected[nAxis];
}


void CDeviceMotionGate::SetPTLifePeriodAlarmDetected(INT nAxis, INT nPortion, BOOL bAlarm)
{
	/*m_bLifeCountPeriodAlarm = bAlarm;*/
}


DWORD CDeviceMotionGate::GetLifePeriodAlarmCount(INT nAxis)
{
	DWORD nPortion = 0;
	if (m_stSystemStatus.bMotorLifeDetected[nAxis]) // 우선 기준값이 넘어야한다.
	{
		//nPortion = GetMotorLifeCount(nAxis) / GetLifeCountCycle(); // 몫만 변함.
		nPortion = GetMotorLifeCount(nAxis);
	}
	
	return nPortion;
}


void CDeviceMotionGate::IncreseMotorLifeCount(INT nValue)
{
	for (INT nAxis = 0; nAxis < 9; nAxis++)
	{
		if ((m_wCurAxisConnection >> nAxis)& 0x01)
		{
			m_stSystemStatus.dwLifeCount[nAxis] += nValue;
		}
	}
}


void CDeviceMotionGate::SetLifeCountStandard(DWORD dwValue)
{
	m_stConnectedDeviceInfo.dwWarningLifeCount = dwValue;
	
}

void CDeviceMotionGate::SetLifeCountCycle(DWORD dwValue)
{
	m_stConnectedDeviceInfo.dwWarningLifeTickCount = dwValue;
}


DWORD CDeviceMotionGate::GetLifeCountCycle()
{
	return m_stConnectedDeviceInfo.dwWarningLifeTickCount;
}


DWORD CDeviceMotionGate::GetLifeCountStandard()
{
	return m_stConnectedDeviceInfo.dwWarningLifeCount;
}


INT CDeviceMotionGate::CmdConnectionClose()
{
	INT nFasResult = DoCmdClose();
	return nFasResult;
}




CString CDeviceMotionGate::GetLastNomalStatus()
{
	return m_PrevLog.strNormal;
}


CString CDeviceMotionGate::GetLastWarningStatus()
{
	return m_PrevLog.strWarning;
}


CString CDeviceMotionGate::GetLastDeviceErrorStatus()
{
	return m_PrevLog.strDeviceError;
}


CString CDeviceMotionGate::GetLastMotorErrorStatus(INT index)
{
	return m_PrevLog.strMotorError[index];
}


INT CDeviceMotionGate::GetBackupState()
{
	return m_stCommand.cmdGate.bitBackup;
}

/*
const BOOL CDataBaseConfig::LoadNoImage(CBitmap* pBitmap)
{
	if (NULL == pBitmap)
	{
		return FALSE;
	}

	if (pBitmap->GetSafeHandle())
	{
		pBitmap->Detach();
	}

	pBitmap->Attach(::CopyImage(m_bmpNoImage, IMAGE_BITMAP, 0, 0, 0));

	return TRUE;
}
*/


BOOL CDeviceMotionGate::GetSystemImageBitmap(CBitmap* pBitmap)
{
	if (NULL == pBitmap)
	{
		return FALSE;
	}

	if (pBitmap->GetSafeHandle())
	{
		pBitmap->Detach();
	}
	CBitmap* pBitmapTest = GetSystemImage();
	pBitmap->Attach(::CopyImage(m_stDatabaseDeviceInfo.bmpImage, IMAGE_BITMAP, 0, 0, 0));
}


void CDeviceMotionGate::SetAutoBackupReservation(BOOL bEnable)
{
	m_bBackupReservation = bEnable;
}

const BOOL CDeviceMotionGate::IsBackupState()
{
	return state_backup == GetStatus();
}




const BOOL CDeviceMotionGate::CompareChangeDatabase()
{
	if (m_stDatabaseEditDeviceInfo.strName != m_stDatabaseDeviceInfo.strName ||
		m_stDatabaseEditDeviceInfo.nMotorCount != m_stDatabaseDeviceInfo.nMotorCount ||
		m_stDatabaseEditDeviceInfo.nHomingTimeout != m_stDatabaseDeviceInfo.nHomingTimeout)
	{
		return FALSE;
	}
	return TRUE;
}

const BOOL CDeviceMotionGate::CompareNameDatabase()
{
	if (m_stDatabaseEditDeviceInfo.strName != m_stDatabaseDeviceInfo.strName)
	{ // 이름이 다르면 모터수량은 무시해야한다.
		return TRUE;
	}
	return FALSE;
}


const BOOL CDeviceMotionGate::IsStableMotorStruct()
{
	
	INT nMotorCount = GetMotorCount() + 1;
	DWORD dwConnected = GetConnectedMotorStatus();

	for (INT nIndex = 1; nIndex < nMotorCount; nIndex++)
	{
		if (FALSE == IsMotorConnected(nIndex))  return FALSE;
	}

	return TRUE;
}


const BOOL CDeviceMotionGate::IsAutoBackupError()
{
	return m_bAutoBackupError;
}


void CDeviceMotionGate::SetAutoBackupDeviceError(const BOOL bEnable)
{
	m_bAutoBackupError = bEnable;
}


void CDeviceMotionGate::ActiveHoming()
{
	//BOOL bEnable = FALSE;
	BOOL bEnable = TRUE;

	static INT nTick;

	if (/*m_bActiveHomingAll &&*/ bEnable)
	{
		m_stateMachine = state_home_all;

#ifndef SKIP_ADMINISTRATION_MODE
		m_nOpCode = opCode_check_monitor_mode_on;
#else
		//m_nOpCode = opCode_notify_backup_complete;
		m_nOpCode = opCode_check_monitor_mode_on;
#endif

		const ULONGLONG ullTimeStamp = ::GetTickCount64();
		for (INT nIndex = 0; nIndex < 9; nIndex++)
		{
			m_stCommand.timeStamp[nIndex] = ullTimeStamp;
		}

		m_nMotorIndex = FindNextMotorIndex();
		m_nParameterIndex = 0;
		m_nRetryCount = 0;

		SendLogMessage(msgSequence, msg_homing_all_start);
	}
}


void CDeviceMotionGate::KeepStopAutoBackup(BOOL bStop)
{
	m_bKeepStopAutoBackup = bStop;
}


const BOOL CDeviceMotionGate::IsKeepStopAutoBackup()
{
	return m_bKeepStopAutoBackup;
}


void CDeviceMotionGate::CheckAutoBackupEnable()
{
	if (IsSystemStatusError() || 
		IsBusy() || 
		IsKeepStopAutoBackup())
	{
		EnableAutoRestoration(FALSE);
		SetAutoBackupDeviceError(TRUE);
	}
}


void CDeviceMotionGate::SetHomingStop(BOOL bStop)
{
	m_bHomingStop = bStop;
}


const BOOL CDeviceMotionGate::IsUserHomingStop()
{
	return m_bHomingStop;
}


void CDeviceMotionGate::InitDatabase()
{
	m_stDatabaseDeviceInfo.nMotorCount = -1;
	m_stDatabaseDeviceInfo.nHomingTimeout = 6000;
	m_stDatabaseDeviceInfo.nNominalID = -1;
	m_stDatabaseDeviceInfo.nRestoredID = -1;
	m_stDatabaseDeviceInfo.nDeviceID = -1;
	m_stDatabaseDeviceInfo.bDeleted = FALSE;
}

void CDeviceMotionGate::InitEditData()
{
	m_stDatabaseEditDeviceInfo.nMotorCount = -1;
	m_stDatabaseEditDeviceInfo.nHomingTimeout = 6000;
	m_stDatabaseEditDeviceInfo.nNominalID = -1;
	m_stDatabaseEditDeviceInfo.nRestoredID = -1;
	m_stDatabaseEditDeviceInfo.nDeviceID = -1;
	m_stDatabaseEditDeviceInfo.bDeleted = FALSE;
}




const BOOL CDeviceMotionGate::IsAllalarm()
{
	return m_bAllalarm;
}


const BOOL CDeviceMotionGate::GetMotorsAlarmCount()
{
	INT nCount = 0;
	for (INT nIndex = 0; nIndex <= 8; nIndex++)
	{
		if (m_stSystemStatus.btCurrentMotorConnectionStatus >> (nIndex - 1) & 0x1)
		{
			if (TRUE == m_stSystemStatus.dwDetailStatus[nIndex].bitErrorAll)
			{
				nCount++;
			}
		}
	}
	
	return nCount;
}


INT CDeviceMotionGate::DoCmdGetAlarmType(DWORD dwAddress, INT nMotorIndex)
{
	CInterface* pInterface = CInterfaceManager::GetInterface();
	BYTE byStatus;
	INT nFasResult = FMM_OK;

	if (FALSE == pInterface->GetAlarmType(m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, &byStatus))
	{// GetAxisStatus(m_stDatabaseDeviceInfo.dwAddress, nMotorIndex, &motor_status.dwValue))
		nFasResult = pInterface->GetLastError(); // nFasResult 이걸 같이가도 되나?
	}

	switch (nFasResult)
	{
	case FMM_OK:
	{
		m_stSystemStatus.btMotorAlarm[nMotorIndex] = byStatus;

	}	break;
	
	default:
	{

	}	break;

	}
	
	return nFasResult; // 명령의 LastError이 날라간다.
}


INT CDeviceMotionGate::DoCmdGetMotionGateAlarmStatus(DWORD dwAddress)
{
	CInterface* pInterface = CInterfaceManager::GetInterface();
	BYTE byStatus;
	INT nFasResult = FMM_OK;

	if (FALSE == pInterface->GetMotionGateAlarmStatus(m_stDatabaseDeviceInfo.dwAddress, &byStatus))
	{
		nFasResult = pInterface->GetLastError(); // nFasResult 이걸 같이가도 되나?
	}

	switch (nFasResult)
	{
	case FMM_OK:
	{
		m_stSystemStatus.btSystemAlarmStatus = byStatus;
	}	break;

	case FMC_DISCONNECTED:
	case FMC_NO_RESPONSE:
	{
		SetConnectionStatus(FALSE);
	}	break;

	default:
	{

	}	break;
	}

	return nFasResult; // 명령의 LastError이 날라간다.
}


const BOOL CDeviceMotionGate::IsSystemHoming()
{
	BOOL bSystemHoming = FALSE;
	for (INT nAxis = 1; nAxis < 9; nAxis++)
	{
		if (TRUE == IsMotorConnected(nAxis))
		{ // 모터 한개라도 Homing 중이면..
			bSystemHoming |= m_stSystemStatus.dwDetailStatus[nAxis].bitOriginReturning;
		}
	}
	
	return bSystemHoming;
}


const BOOL CDeviceMotionGate::IsMotorAlarmDetected(INT nMotorIndex)
{
	return m_stSystemStatus.dwDetailStatus[nMotorIndex].bitErrorAll == 1;
}

const BOOL CDeviceMotionGate::IsMotorOriginOK(INT nIndex)
{
	return m_stSystemStatus.dwDetailStatus[nIndex].bitOriginComplete == 1;
}


const BOOL CDeviceMotionGate::IsMotorServoOff(INT nMotorIndex)
{
	//m_stSystemStatus.stMotorStatus[nMotorIndex].wStatus.bitServoOn == 1;
	return m_stSystemStatus.dwDetailStatus[nMotorIndex].bitServoOn == 0;
}





const INT CDeviceMotionGate::DoCmdGetMotorOrganization()
{
	DWORD dwOrganization = 0;
	INT nFasResult = FMM_OK;
	
	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->GetMotorOrganization(m_stDatabaseDeviceInfo.dwAddress, 0xfa, &dwOrganization))
	{
		nFasResult = pInterface->GetLastError();
	}
	
	m_wOriginAxisOrganization = LOWORD(dwOrganization);
	m_wCurAxisOrganization = HIWORD(dwOrganization);

	return nFasResult;
}


DWORD CDeviceMotionGate::GetOriginAxisOrganization()
{
	return m_wOriginAxisOrganization;
}


DWORD CDeviceMotionGate::GetCurAxisOrganization()
{
	// TODO: 여기에 구현 코드 추가.
	return m_wCurAxisOrganization;
}


BOOL CDeviceMotionGate::IsActiveHomingAll()
{
	return m_bActiveHomingAll;
}


void CDeviceMotionGate::SetActiveHomingAll(const BOOL bActive)
{
	m_bActiveHomingAll = bActive;
}


void CDeviceMotionGate::SetInitHomingState(INT nAxis)
{
	m_stSystemStatus.opStatus[nAxis].bitHoming = FALSE;
	m_stSystemStatus.opStatus[nAxis].bitHomingNG = FALSE;
	m_stSystemStatus.opStatus[nAxis].bitHomingOK = FALSE;
}


BOOL CDeviceMotionGate::IsMotorStopped(INT nAxis)
{
	return m_stSystemStatus.dwDetailStatus[nAxis].bitMotioning == 0;
}


INT CDeviceMotionGate::CmdMotorMoveStop(INT nAxis)
{
	m_nLastFasResult = DoCmdMoveStop(nAxis);

	if (FMM_OK != m_nLastFasResult)
	{
		return -1;
	}
}


void CDeviceMotionGate::SetHomingClose(BOOL bClose)
{
	m_bSetHomingClose = bClose;
}


const BOOL CDeviceMotionGate::IsHomingClose()
{
	return m_bSetHomingClose;
}


const BOOL CDeviceMotionGate::IsAllMotorServoOff()
{
	BOOL bServoOff = TRUE;
	
	for (INT nAxis = 1; nAxis < 9; nAxis++)
	{ // 전체 모터 ServoOff 확인..
		bServoOff &= (FALSE == m_stSystemStatus.dwDetailStatus[nAxis].bitServoOn);
	}
	
	return bServoOff; // 반대를 출력..
}

void CDeviceMotionGate::SendHomingDoneMessage(BOOL bClose)
{
	m_bSendDoneMessage = bClose;
}

const BOOL CDeviceMotionGate::IsMotionGateMotorExist()
{
	return m_wCurAxisOrganization != 0;
}
