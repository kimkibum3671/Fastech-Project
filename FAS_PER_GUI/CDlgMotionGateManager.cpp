// CDlgMotionGateManager.cpp: 구현 파일
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CDlgMotionGateManager.h"
#include "afxdialogex.h"

#include "COutlookDevice.h"
#include "MainFrm.h"

// CDlgMotionGateManager 대화 상자

IMPLEMENT_DYNAMIC(CDlgMotionGateManager, CDialogEx)

#define MONITORTIMMER 0
#define MONITORTEXT	  1

const int m_nBasic = 100;

CDlgMotionGateManager::CDlgMotionGateManager(CWnd* pParent /*=nullptr*/, int nMotionCnt)
	: CDialogEx(IDD_MOTION_MANAGER, pParent)
{
	m_pParent = pParent;
	m_nCurrentCount = nMotionCnt;
	m_bProgramStart = true;
	m_nVScrolPos = 0;
	m_nHScrolPos = 0;
	m_nPrevCount = 0;
	m_nMotorIndex = 0;

	m_pSelDevice = NULL;
	m_pPrevSelDevice = NULL;

	m_CurMotorStatus = { NULL };
	m_PrevMotorStatus = { NULL };

	m_bChangedWindowRect = FALSE;
	m_bBackupToggle = FALSE;
	m_strBackupDateTime = _T("");
	m_strBackupAutoSec = _T("");
	
	m_bVisibleBackupTime = m_bVisibleBackdupAutoSec = m_bVisibleBtnBackupToggle = FALSE;

	m_bShow = FALSE;

	if (1)
	{
		m_ScreenPos = { 0 };
	}
		
}

CDlgMotionGateManager::~CDlgMotionGateManager()
{	
	if (m_pMotionGateInform == NULL) return;
	
	m_pMotionGateInform->DestroyWindow();
	
	
	//delete m_pMotionGateInform;
}

void CDlgMotionGateManager::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgMotionGateManager, CDialogEx)
	ON_WM_TIMER()
	ON_WM_VSCROLL()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_HSCROLL()
	ON_WM_SIZE()
END_MESSAGE_MAP()


BOOL CDlgMotionGateManager::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CMainFrame* m_pFrame = (CMainFrame*)m_pParent;
	m_nCurrentCount = 1;

	InitScrollObj();
	
	ShowMotionInform();
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

int CDlgMotionGateManager::GetMotionGateCount()
{
	return m_nMotionGateCount;
	//return 1;
}

bool CDlgMotionGateManager::ReMakeInstance()
{
	m_nCurrentCount = GetMotionGateCount();
	return true;
}


bool CDlgMotionGateManager::ShowMotionInform()
{
	CRect rect;
	CRect rectInform;
	
	if (1)
	{
		SetTimer(MONITORTIMMER, 100, NULL);
		SetTimer(MONITORTEXT, 1000, NULL);
	}

	if (1)
	{
		m_pMotionGateInform = new CDlgMotionGateInform;
		m_pMotionGateInform->Create(IDD_MOTION_INFORM, this);
		m_pMotionGateInform->SetMotionID(0); // 의미 X, 화면내 연결된 모든 장치에 대해 ID를 부여해주기 위해 설계된것임.
		m_pMotionGateInform->GetWindowRect(rectInform);
		m_pMotionGateInform->SetWindowPos(NULL, 0, 0, rectInform.Width(), rectInform.Height(), SWP_SHOWWINDOW | SWP_NOZORDER);
	}

	if (1)
	{
		// Scroll
		GetClientRect(rect);
		m_nVPageSize = rect.Height();

		m_nHPageSize = rect.Width();
	}
	
	(GetDlgItem(IDC_STATIC_SIZE_ALARM))->ShowWindow(SW_HIDE);

	m_bShow = TRUE;

	return false;
}

void CDlgMotionGateManager::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case MONITORTIMMER: // 100ms
	{
		if (SetMotionStatus())
		{

		}
		else
		{

		}
	}
	break;

	case MONITORTEXT:
	{
		ShowMotorStatus(m_CurMotorStatus.pDevice);
	}	break;

	default:
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

bool CDlgMotionGateManager::SetMotionStatus()
{
	CString strSimpleSt = _T("");
	CString strProgressSt = _T("");
	CString strMotorError = _T("");
	CString strMotor = _T("Motor");
	DWORD	dwTimeCheck = 0;
	DWORD	dwProgressTime = 0;
	DWORD	dwEndCheckTime = 0;
	BOOL	bEndTime = FALSE;
	BOOL	bChanged = FALSE;
	BOOL	bSelected = FALSE;
	static INT nTick = 0;
	static INT nPrevPos = 0;
	INT nPos = 0, nMin = 0, nMax = 0;
	
	CDeviceList* pDeviceList = CDeviceList::GetInstance();
	CDeviceMotionGate* pDevice = m_pSelDevice;
	bSelected = (pDevice == m_pSelDevice);

	if (m_pPrevSelDevice != m_pSelDevice)
	{
		m_pMotionGateInform->SetDevice(m_pSelDevice);
		m_pPrevSelDevice = m_pSelDevice;
		m_nMotorIndex = 0;
	}

	if (m_pSelDevice == NULL || 0 == pDeviceList->GetAddressCount())
	{// Outlook Device에 IP Address가 아무것도 없을경우
		if (FALSE == m_pMotionGateInform->IsControlEnable())
		{ // 한번만 진행하기 위해 FALSE 일 경우만 진행한다.
			m_pMotionGateInform->SetControlDisable(TRUE);
			//m_pMotionGateInform->SetEnableSystemImage(FALSE);
			m_pMotionGateInform->DisableControl();
			m_pMotionGateInform->SetImage(NULL);
		}
		return FALSE; 
	}
	else 
	{
		m_pMotionGateInform->SetControlDisable(FALSE);
		//m_pMotionGateInform->SetEnableSystemImage(TRUE);
	}
	


	for (INT i = 0; i < pDeviceList->GetAddressCount(); i++)
	{ // 디바이스마다 진행상황(시간) 체크중..
		const DWORD dwAddress = pDeviceList->GetAddressByIndex(i);
		CDeviceMotionGate* pDevice = NULL;
		pDeviceList->GetMotiongateByAddress(dwAddress, pDevice);

		//INT nPos, nMin, nMax;

		if (1)
		{ // Progress Bar 제어 및 Progress Time 제어
#ifdef LIFE_COUNT_TEST
			pDevice->IncreseMotorLifeCount(100); // Life Count Test
#endif // LIFE_COUNT_TEST
			
			pDevice->StartProgressTimer(TRUE);
			pDevice->GetDetailStatus(&nPos, &nMin, &nMax);

			dwProgressTime = pDevice->GetProgressTime();

			if (nMin == nPos || nMax == nPos)
			{ // Progress Bar Stop 경우
				pDevice->StopProgressTimer(TRUE);

				if (nMin == nPos)
				{
				}

				if (nMax == nPos && dwProgressTime != 0) //nMax가 두번 인식되기에 dwProgressTime == 0 으로 저장됨...즉, Timing 문제 때문..
				{
					pDevice->SetProgressEndTime(dwProgressTime);
				}
			}

			else
			{ // Progress Bar 진행일 경우
				if (nPrevPos != nPos)
				{
					pDevice->SetProgressEndTime(dwProgressTime);
					nPrevPos = nPos;
				}
			}

		}
	}

	pDevice->EnterEditMode();
	pDevice->GetAddress(m_pMotionGateInform->m_mgInform.strIP);
	pDevice->GetAutoBackupDatetime(m_pMotionGateInform->m_mgInform.BackupDateTime);
	pDevice->GetSimpleStatus(strSimpleSt);

	if (pDevice->IsRun())
	{ // 지정 MotionGate ProgressBar 최종 제어..
		pDevice->GetDetailStatus(&nPos, &nMin, &nMax);
		dwEndCheckTime = pDevice->GetProgressEndTime();
		strProgressSt += pDevice->GetDetailStatus();
		if (true)
		{ // System Alarm 메시지는 설명부분이 굉장히 길기때문에.. 코드번호만 나타낸다.
			CString strAlarm;
			strAlarm.LoadString(IDS_DETAIL_SYSTEM_ALARM);

			if (-1 != strProgressSt.Find(strAlarm))
			{
				strProgressSt = strProgressSt.Left(strProgressSt.Find(')') + 1);
			}
		}


		//INT nLengthLimit = 19;
		m_pMotionGateInform->ShowProgressTime(dwEndCheckTime);
		m_pMotionGateInform->SetProgressBar(nPos, nMin, nMax);
		m_pMotionGateInform->SetProgressStatus(strProgressSt);


		//Command Test
		/*CString strCmdTest;
		strCmdTest = pDevice->GetCommandTestResult();
		m_pMotionGateInform->SetCommandResultText(strCmdTest);*/

	}

	// BackupSchedule Time, Auto Sec, On_Off Button.
	
	m_pMotionGateInform->SetBackupScheduleDateTime(GetBackupDateTime());
	m_pMotionGateInform->SetBackupAutoSec(GetBackupAutoSec());
	m_pMotionGateInform->SetBackupToggle(GetBackupToggle());
	m_pMotionGateInform->SetBackupTimeControl(m_bVisibleBackdupAutoSec, m_bVisibleBtnBackupToggle /*m_bEnableBtnToggle*/, m_bVisibleBackupTime);

	// Setting MotionGate Informaion GUI
	m_pMotionGateInform->SetDevice(pDevice);
	m_pMotionGateInform->ShowToolTip(pDevice);
	m_pMotionGateInform->ShowButton(pDevice);
	m_pMotionGateInform->SetAddress(pDevice->GetAddress());

	m_pMotionGateInform->m_mgInform.strStatus = strSimpleSt;
	m_pMotionGateInform->m_mgInform.nMotorCnt = pDevice->GetMotorCount();
	m_pMotionGateInform->m_mgInform.dwMotorConnectStatus = pDevice->GetConnectedMotorStatus();
	m_pMotionGateInform->m_mgInform.strSystemName = pDevice->GetSystemName();
	m_pMotionGateInform->m_mgInform.nOpCode = pDevice->GetOpCode();
	m_pMotionGateInform->m_mgInform.bFocus = bSelected;
	m_pMotionGateInform->m_mgInform.bConnect = m_pMotionGateInform->m_bConnected = pDevice->IsMotiongateConnected();
	m_pMotionGateInform->m_mgInform.bMasterConnect = pDevice->GetMasterConnectStatus();

	m_CurMotorStatus.nMotorsAlarmCnt = pDevice->GetMotorsAlarmCount(); // Alarm 발생 모터개수 총량
	m_CurMotorStatus.dwPTLifeCount = m_pMotionGateInform->GetMotorPTtotalLifeCount(pDevice); // 분기값 변화 발생 총량 카운트
	m_CurMotorStatus.dwConnectionNum = m_pMotionGateInform->GetConnectedMotorNum(pDevice); // 연결된 모터 개수
	m_CurMotorStatus.dwAlarmNum = m_pMotionGateInform->GetMotorTotalAlarm(pDevice); // 모터들의 알람 번호 총합
	m_CurMotorStatus.dwPTNum = m_pMotionGateInform->GetMotorPosionTableTotalNum(pDevice); // 모터들의 PT 번호 총합

	if (pDevice->IsMotiongateConnected())
	{ // Life Count
		if (TRUE == pDevice->IsDefaultState() ||
			TRUE == pDevice->IsBackupState())
		{ // 이재우차장 요청 사항 : Backup 중에는 Motor 값 현시.
			m_pMotionGateInform->MotorPositionTableLifeCount(pDevice); // Text 현시
		}
		else
		{
			m_pMotionGateInform->MotorShowDisable();
			m_pMotionGateInform->SetMotorErrorState(_T(""));
			m_pMotionGateInform->SetZeroMotorStatus(_T(""));
		}

		m_PrevMotorStatus.dwPTLifeCount = m_CurMotorStatus.dwPTLifeCount;
	}

	
	
	if (IsStatusChanged() || (nTick == 100))
	{ // 상태 변화 감지.
		
		if (m_bChangedWindowRect) SetScrollBar(m_pMotionGateInform);

		bool bConnected = pDevice->IsMotiongateConnected(); // m_stateMachine
		bool bBusy = pDevice->IsSystemStatusBusy(); // m_stateMachine
		bool bWarning = pDevice->IsSystemStatusWarning(); // m_stSystemStatus.wStatus.bitHoming, m_stSystemStatus.wStatus.bitTeachingPendant
		bool bError = pDevice->IsSystemStatusError(); // m_stateMachine && m_nopcode / CurrentConnected
		bool bMaster = pDevice->GetMasterConnectStatus();
		bool bChangedFocus = bSelected;

		m_pMotionGateInform->ShowMotionInform();
		m_pMotionGateInform->ShowMotionStLed(bConnected, bBusy, bWarning, bError, bMaster, bChangedFocus);
		m_pMotionGateInform->SetImage(pDevice);
		
		if (!bConnected)
		{
			m_pMotionGateInform->MotorShowDisable();
			m_pMotionGateInform->SetMotorErrorState(_T(""));
			m_pMotionGateInform->SetProgressStatus(_T(""));
			m_pMotionGateInform->SetZeroMotorStatus(_T(""));
		}
		else
		{ // 먼저 연결 체크를 해야한다.
			if (TRUE == pDevice->IsDefaultState() ||
				TRUE == pDevice->IsBackupState())
			{// Backup 이거나 Default 상태일 경우만  보여준다.
			 // 이미 이 상황일 경우 위에서 ShowDisable 처리 한다,
				DWORD dwCurAxisConnection = pDevice->GetConnectedMotorStatus();
				m_pMotionGateInform->MotorConnectionStatus(dwCurAxisConnection);
				m_pMotionGateInform->MotorPositionTableStatus(pDevice);
				m_pMotionGateInform->MotorAlarmStatus(pDevice);
			}
		}


		m_pMotionGateInform->m_PrevInform.strIP = m_pMotionGateInform->m_mgInform.strIP;
		m_pMotionGateInform->m_PrevInform.BackupDateTime = m_pMotionGateInform->m_mgInform.BackupDateTime;
		m_pMotionGateInform->m_PrevInform.strStatus = m_pMotionGateInform->m_mgInform.strStatus;
		m_pMotionGateInform->m_PrevInform.nMotorCnt = m_pMotionGateInform->m_mgInform.nMotorCnt;
		m_pMotionGateInform->m_PrevInform.dwMotorConnectStatus = m_pMotionGateInform->m_mgInform.dwMotorConnectStatus;
		m_pMotionGateInform->m_PrevInform.strSystemName = m_pMotionGateInform->m_mgInform.strSystemName;
		m_pMotionGateInform->m_PrevInform.nOpCode = m_pMotionGateInform->m_mgInform.nOpCode;
		m_pMotionGateInform->m_PrevInform.bFocus = m_pMotionGateInform->m_mgInform.bFocus;
		m_pMotionGateInform->m_PrevInform.bConnect = m_pMotionGateInform->m_mgInform.bConnect;
		m_pMotionGateInform->m_PrevInform.bMasterConnect = m_pMotionGateInform->m_mgInform.bMasterConnect;

		m_PrevMotorStatus.pDevice = m_CurMotorStatus.pDevice;
		m_PrevMotorStatus.bLifeCountAlarm = m_CurMotorStatus.bLifeCountAlarm;
		m_PrevMotorStatus.dwConnectionNum = m_CurMotorStatus.dwConnectionNum;
		m_PrevMotorStatus.dwAlarmNum = m_CurMotorStatus.dwAlarmNum;
		m_PrevMotorStatus.dwPTNum = m_CurMotorStatus.dwPTNum;
		m_PrevMotorStatus.nMotorsAlarmCnt = m_CurMotorStatus.nMotorsAlarmCnt;

		nTick = 0;
		//m_PrevMotorStatus.dwPTLifeCount = m_CurMotorStatus.dwPTLifeCount;
	}

	m_bChangedWindowRect = FALSE;
	nTick++;
	//ShowToolTip();
	return false;
}


void CDlgMotionGateManager::ShowMotorStatus(CDeviceMotionGate* pDevice)
{
	CString strMotorStatus = _T("");
	
	if (pDevice == NULL) return; // Device가 아무것도 없을경우

	if (pDevice->IsMotiongateConnected())
	{
		if (!pDevice->IsMotorExist(MOTOR0)) m_pMotionGateInform[0].SetZeroMotorStatus(strMotorStatus);

		for (INT nIndex = m_nMotorIndex; ; nIndex++)
		{
			if (pDevice->IsMotorExist(nIndex) && _T("") != pDevice->GetMotorErrorText(m_nMotorIndex))
			{
				m_nMotorIndex = nIndex;

				if (TRUE == pDevice->IsDefaultState() ||
					TRUE == pDevice->IsBackupState())
				{ // 일반 백업 상태이거나 일반 대기 상태일 경우만 문구 현시해 준다.
					strMotorStatus = pDevice->GetMotorErrorText(m_nMotorIndex);
				}

				if (m_nMotorIndex == MOTOR0) m_pMotionGateInform[0].SetZeroMotorStatus(strMotorStatus);
				else m_pMotionGateInform[0].SetMotorErrorState(strMotorStatus);
				m_nMotorIndex++;
				return;
			}

			if (nIndex == 9)
			{
				m_nMotorIndex = 0;
				return;
			}
		}
	}

	else
	{
		m_pMotionGateInform[0].SetMotorErrorState(strMotorStatus);
		m_pMotionGateInform[0].SetZeroMotorStatus(strMotorStatus);
	}
}

void CDlgMotionGateManager::InitScrollObj()
{

}

bool CDlgMotionGateManager::ShowVertScroll(CDlgMotionGateInform* pInform, int nShow)
{
	m_nShowVertScroll = nShow;

	if (nShow == SW_HIDE)
	{
		// 스크롤바 제어후 다시 창을 늘린다면 마지막 제어 화면에서 멈춤. 
		// 따라서 초기화면으로 돌려줘야하기에 원상태로 돌림. 또한 이함수는 주기적으로 로딩되기때문에 m_nHScrolPos = 0 으로 바로 초기화 해야함.
		ScrollWindow(0, m_nVScrolPos);

		m_nVScrolPos = 0;
	}

	else
	{
		InitPosition(pInform);
	}

	return false;
}

bool CDlgMotionGateManager::ShowHorzScroll(CDlgMotionGateInform* pInform, int nShow)
{
	m_nShowHorzScroll = nShow;

	if (nShow == SW_HIDE)
	{
		// 스크롤바 제어후 다시 창을 늘린다면 마지막 제어 화면에서 멈춤. 
		// 따라서 초기화면으로 돌려줘야하기에 원상태로 돌림. 또한 이함수는 주기적으로 로딩되기때문에 m_nHScrolPos = 0 으로 바로 초기화 해야함.
		ScrollWindow(m_nHScrolPos, 0);
		m_nHScrolPos = 0;
	}

	else
	{
		InitPosition(pInform);
	}

	return TRUE;
}


void CDlgMotionGateManager::OnDestroy()
{
	KillTimer(MONITORTIMMER);
	KillTimer(MONITORTEXT);
	CDialogEx::OnDestroy();
}


void CDlgMotionGateManager::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.
	this->GetWindowRect(m_rectNow);

	if (m_rectPrev != m_rectNow)
	{
		m_rectPrev = m_rectNow;
		m_bChangedWindowRect = TRUE;
	}

	
}


BOOL CDlgMotionGateManager::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	
	
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CDlgMotionGateManager::ShowToolTip()
{
	CMainFrame* pFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	CDeviceMotionGate* pDevice = pFrame->GetSelectedMotiongateCtrl();
}



void CDlgMotionGateManager::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnMouseMove(nFlags, point);
}


BOOL CDlgMotionGateManager::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int delta;
	CRect rect;
	GetWindowRect(rect);

	if (m_nShowVertScroll == SW_SHOW)
	{
		if (zDelta > 0)
		{
			delta = -m_nVPageSize;
		}

		else
		{
			delta = m_nVPageSize;
		}

		int scrollpos = m_nVScrolPos + delta;
		int maxpos = m_nViewHeight - m_nVPageSize;

		if (scrollpos < 0)
			delta = -m_nVScrolPos;
		else
			if (scrollpos > maxpos)
				delta = maxpos - m_nVScrolPos;


		if (delta != 0)
		{
			m_nVScrolPos += delta;
			SetScrollPos(SB_VERT, m_nVScrolPos, TRUE);
			ScrollWindow(0, -delta);
		}
	}

	else if (m_nShowHorzScroll == SW_SHOW)
	{
		if (zDelta > 0)
		{
			delta = -m_nHPageSize;
		}

		else
		{
			delta = m_nHPageSize;
		}

		int scrollpos = m_nHScrolPos + delta;
		int maxpos = m_nViewWidth - m_nHPageSize;

		if (scrollpos < 0)
			delta = -m_nHScrolPos;
		else
			if (scrollpos > maxpos)
				delta = maxpos - m_nHScrolPos;


		if (delta != 0)
		{
			m_nHScrolPos += delta;
			SetScrollPos(SB_HORZ, m_nHScrolPos, TRUE);
			ScrollWindow(-delta, 0);
		}

	}
		
	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}


void CDlgMotionGateManager::SetBackupDateTime(CString strTime)
{
	m_strBackupDateTime = strTime;
}


CString CDlgMotionGateManager::GetBackupDateTime()
{
	return m_strBackupDateTime;
}


void CDlgMotionGateManager::SetBackupAutoSec(CString strSec)
{
	m_strBackupAutoSec = strSec;
}


CString CDlgMotionGateManager::GetBackupAutoSec()
{
	return m_strBackupAutoSec; 
}


void CDlgMotionGateManager::SetBackupToggle(BOOL bEnable)
{
	m_bBackupToggle = bEnable;
}


BOOL CDlgMotionGateManager::GetBackupToggle()
{
	return m_bBackupToggle;
}


void CDlgMotionGateManager::SetVisibleBackupDate(BOOL bBackupAutoSecControl, BOOL bBackupToggle, BOOL bBdakupTime)
{
	m_bVisibleBackdupAutoSec = bBackupAutoSecControl;
	m_bVisibleBackupTime = bBdakupTime;
	m_bVisibleBtnBackupToggle = bBackupToggle;
}

void CDlgMotionGateManager::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int delta;
	CRect rect;
	GetWindowRect(rect);

	switch (nSBCode)
	{
	case SB_LINEUP:
	{
		delta = -m_nBasic;
	}
	break;
	case SB_PAGEUP:
	{
		delta = -m_nVPageSize;
	}
	break;
	case SB_THUMBTRACK:
	{
		delta = static_cast<int>(nPos) - m_nVScrolPos;
	}
	break;
	case SB_PAGEDOWN:
	{
		delta = m_nVPageSize;
	}

	break;
	case SB_LINEDOWN:
	{
		delta = m_nBasic;
	}
	break;

	default:
	{
	}
	return;
	}

	int scrollpos = m_nVScrolPos + delta;
	int maxpos = m_nViewHeight - m_nVPageSize;

	if (scrollpos < 0)
		delta = -m_nVScrolPos;
	else
		if (scrollpos > maxpos)
			delta = maxpos - m_nVScrolPos;


	if (delta != 0)
	{
		m_nVScrolPos += delta;
		SetScrollPos(SB_VERT, m_nVScrolPos, TRUE);
		ScrollWindow(0, -delta);
	}

	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CDlgMotionGateManager::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	
	int delta;
	CRect rect;
	GetWindowRect(rect);

	switch (nSBCode)
	{
	case SB_LINELEFT:
	{
		delta = -m_nBasic;
	}
	break;
	case SB_PAGELEFT:
	{
		delta = -m_nHPageSize;
	}
	break;
	case SB_THUMBTRACK:
	{
		delta = static_cast<int>(nPos) - m_nHScrolPos;
	}
	break;
	case SB_PAGERIGHT:
	{
		delta = m_nHPageSize;
	}

	break;
	case SB_LINERIGHT:
	{
		delta = m_nBasic;
	}
	break;

	default:
	{
	}
	return;
	}

	int scrollpos = m_nHScrolPos + delta;
	int maxpos = m_nViewWidth - m_nHPageSize;

	if (scrollpos < 0)
		delta = -m_nHScrolPos;
	else
		if (scrollpos > maxpos)
			delta = maxpos - m_nHScrolPos;


	if (delta != 0)
	{
		m_nHScrolPos += delta;
		SetScrollPos(SB_HORZ, m_nHScrolPos, TRUE);
		ScrollWindow(-delta, 0);
	}

	

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}





void CDlgMotionGateManager::IncreaseMotorLifeCount(INT nLifeCount) 
{ // Test용
	CDeviceList* pDeviceList = CDeviceList::GetInstance();
	
	for (INT i = 0; i < pDeviceList->GetAddressCount(); i++)
	{
		const DWORD dwAddress = pDeviceList->GetAddressByIndex(i);
		CDeviceMotionGate* pDevice = NULL;
		pDeviceList->GetMotiongateByAddress(dwAddress, pDevice);
		pDevice->IncreseMotorLifeCount(nLifeCount);
	}
	//pDeviceList->GetRegisteredDeviceCount
	// pDevice->IncreseMotorLifeCount(1000);
}


void CDlgMotionGateManager::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	Invalidate(TRUE); // 잔상 제거..
	m_ScreenPos.cx = cx;
	m_ScreenPos.cy = cy;
}


void CDlgMotionGateManager::SetScrollBar(CDlgMotionGateInform* pInform)
{
	if (pInform != NULL)
	{
		CRect rectMotion;
		CRect rectManage;
		GetClientRect(rectManage);

		pInform->GetClientRect(rectMotion);

		m_nViewWidth = rectMotion.Width();
		m_nViewHeight = rectMotion.Height();

		int HScrollMax = 0;
		m_nHPageSize = 0;

		if (m_ScreenPos.cx < m_nViewWidth)
		{
			HScrollMax = m_nViewWidth;
			m_nHPageSize = m_ScreenPos.cx;
			m_nHScrolPos = min(m_nHScrolPos, m_nViewWidth - m_nHPageSize);
		}

		SCROLLINFO si;
		si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
		si.nMin = 0;
		si.nMax = HScrollMax;
		si.nPos = m_nHScrolPos;
		si.nPage = m_nHPageSize;
		SetScrollInfo(SB_HORZ, &si, TRUE);

		int VScrollMax = 0;
		m_nVPageSize = 0;

		if (m_ScreenPos.cy < m_nViewHeight)
		{
			VScrollMax = m_nViewHeight;
			m_nVPageSize = m_ScreenPos.cy;
			m_nVScrolPos = min(m_nVScrolPos, m_nViewHeight - m_nVPageSize);
		}

		si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
		si.nMin = 0;
		si.nMax = VScrollMax;
		si.nPos = m_nVScrolPos;
		si.nPage = m_nVPageSize;
		SetScrollInfo(SB_VERT, &si, TRUE);
		
		ShowHorzScroll(pInform, rectMotion.Width() - rectManage.Width() > 0 ? SW_SHOW : SW_HIDE);
		ShowVertScroll(pInform, rectMotion.Height() - rectManage.Height() > 0 ? SW_SHOW : SW_HIDE);
	}

	
}


const BOOL CDlgMotionGateManager::IsShowState()
{
	return m_bShow;
}


void CDlgMotionGateManager::SetShowState(bool bShow)
{
	m_bShow = bShow;
}


void CDlgMotionGateManager::InitPosition(CDlgMotionGateInform* pInform)
{
	pInform->SetWindowPos(NULL, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOSIZE);
	SetScrollPos(SB_VERT, 0, TRUE);
	ScrollWindow(m_nHScrolPos, 0);

	m_nVScrolPos = 0;
	m_nHScrolPos = 0;
}


void CDlgMotionGateManager::SetDeviceMotionGate(CDeviceMotionGate* pDevice)
{
	m_CurMotorStatus.pDevice = m_pSelDevice = pDevice;
}


const BOOL CDlgMotionGateManager::IsStatusChanged()
{
	BOOL bChange = (m_bChangedWindowRect || // Window 변화 상태 확인
				// MotionGate 상태 변화 확인.
		m_pMotionGateInform->m_PrevInform.bConnect != m_pMotionGateInform->m_mgInform.bConnect ||
		m_pMotionGateInform->m_PrevInform.strIP != m_pMotionGateInform->m_mgInform.strIP ||
		m_pMotionGateInform->m_PrevInform.BackupDateTime != m_pMotionGateInform->m_mgInform.BackupDateTime ||
		m_pMotionGateInform->m_PrevInform.strStatus != m_pMotionGateInform->m_mgInform.strStatus ||
		m_pMotionGateInform->m_PrevInform.nMotorCnt != m_pMotionGateInform->m_mgInform.nMotorCnt ||
		m_pMotionGateInform->m_PrevInform.dwMotorConnectStatus != m_pMotionGateInform->m_mgInform.dwMotorConnectStatus ||
		m_pMotionGateInform->m_PrevInform.strSystemName != m_pMotionGateInform->m_mgInform.strSystemName ||
		m_pMotionGateInform->m_PrevInform.bFocus != m_pMotionGateInform->m_mgInform.bFocus ||
		// Master(PLC) 연결상태 변화 확인.
		m_pMotionGateInform->m_PrevInform.bMasterConnect != m_pMotionGateInform->m_mgInform.bMasterConnect ||
		//Motor 변화 상태 확인.
		m_CurMotorStatus.pDevice != m_PrevMotorStatus.pDevice ||
		m_CurMotorStatus.bLifeCountAlarm != m_PrevMotorStatus.bLifeCountAlarm ||
		//m_CurMotorStatus.dwPTLifeCount	!= m_PrevMotorStatus.dwPTLifeCount ||
		m_CurMotorStatus.dwConnectionNum != m_PrevMotorStatus.dwConnectionNum ||
		m_CurMotorStatus.dwAlarmNum != m_PrevMotorStatus.dwAlarmNum ||
		m_CurMotorStatus.dwPTNum != m_PrevMotorStatus.dwPTNum	||
		m_CurMotorStatus.nMotorsAlarmCnt != m_PrevMotorStatus.nMotorsAlarmCnt);

	return bChange;
}
