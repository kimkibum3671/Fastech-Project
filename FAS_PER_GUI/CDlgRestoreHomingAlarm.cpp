// CDlgRestoreHomingAlarm.cpp: 구현 파일
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CDlgRestoreHomingAlarm.h"
#include "afxdialogex.h"



// CDlgRestoreHomingAlarm 대화 상자

IMPLEMENT_DYNAMIC(CDlgRestoreHomingAlarm, CDialogEx)

CDlgRestoreHomingAlarm::CDlgRestoreHomingAlarm(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DEVICE_RESTORATION, pParent)
{
	m_bWait = TRUE;
}

CDlgRestoreHomingAlarm::~CDlgRestoreHomingAlarm()
{
}

void CDlgRestoreHomingAlarm::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_STATUS, m_progress);
}


BEGIN_MESSAGE_MAP(CDlgRestoreHomingAlarm, CDialogEx)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_MFCBUTTON_HOMING, &CDlgRestoreHomingAlarm::OnBnClickedMfcbuttonHoming)
	ON_BN_CLICKED(IDC_BTN_HOMING, &CDlgRestoreHomingAlarm::OnBnClickedBtnHoming)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CDlgRestoreHomingAlarm::OnBnClickedMfcbuttonExist)
END_MESSAGE_MAP()


// CDlgRestoreHomingAlarm 메시지 처리기


BOOL CDlgRestoreHomingAlarm::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// IDC_ST_PICTURE
	InitDeviceImage();

	InitButtonImage();

	InitControlTextSize();

	SetProcess(sts_restore_wait);

	m_bHomingEnableText = TRUE;
	m_strStatus = _T("");

	
	GetDlgItem(IDC_BTN_HOMING)->EnableWindow(FALSE);
	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
	
	SetTimer(TIMER_RESOTRE_WAIT, 1000, NULL); // Stop 시키고 일정 시간 지나야 한다.
	

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgRestoreHomingAlarm::SetMotionGate(CDeviceMotionGate* pDevice)
{
	m_pDevice = pDevice;
}


void CDlgRestoreHomingAlarm::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_RESOTRE_WAIT:
	{
		if (TRUE == IsServoOff())
		{ // Restoration 이전에 Servo Stop 및 Servo Off 까지 진행되어야 한다.
			StartRestoration();
			SetTimer(TIMER_RESOTRE_PROGRESS, 10, NULL);

			KillTimer(nIDEvent);
		}

	}	break;

	case TIMER_RESOTRE_PROGRESS:
	{
		if (IsRestorationStart())
		{
			MonitoringRetoreStatus();
		}
		
	}	break;

	case TIMER_HOMING_WAIT:
	{
		if (TRUE ==IsHomingReady())
		{
			m_pDevice->EnableAutoHoming(TRUE);
			m_pDevice->ActiveHoming();
			SetTimer(TIMER_HOMING_PROGRESS, 10, NULL);
			
			KillTimer(nIDEvent);
		}
		
	}	break;

	case TIMER_HOMING_PROGRESS:
	{
		MonitoringHomingStatus();
	}	break;


	default:
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CDlgRestoreHomingAlarm::StartRestoration()
{
	m_pDevice->StartRestoration();
}


void CDlgRestoreHomingAlarm::MonitoringRetoreStatus()
{
	CString strAfxMessage;
	CString strStatus, strDetailStatus;
	CString strOK, strNG;
	
	INT nProcess;
	INT nPos, nMin, nMax;
	BOOL bEnable = FALSE;
	BOOL bRestoring = TRUE;

	nProcess = sts_restore_processing;
	
	strOK.LoadString(IDS_DEVICE_RESTORATION_DONE);
	strNG.LoadString(IDS_DEVICE_RESTORATION_FAIL);

	m_pDevice->GetDetailStatus(&nPos, &nMin, &nMax);
	SetProgressBar(nPos, nMin, nMax);

	m_pDevice->GetSimpleStatus(strStatus);
	strDetailStatus = m_pDevice->GetDetailStatus();
	strDetailStatus.Remove('.');
	
	if (m_strStatus != strDetailStatus)
	{
		SetDlgItemText(IDC_ST_STATUS, strDetailStatus);
		m_strStatus = strDetailStatus;
	}
	
	if (strStatus == strOK)
	{ // Restoration OK
		bEnable = TRUE;
		bRestoring = FALSE;
		nProcess = sts_restore_success;

		GetDlgItem(IDC_BTN_HOMING)->EnableWindow(bEnable);
		GetDlgItem(IDCANCEL)->EnableWindow(bEnable);
		strAfxMessage.Format(_T("Restoration success."));
	}

	else if (strStatus == strNG)
	{ // Restoration NG
		bEnable = TRUE;
		bRestoring = FALSE;
		nProcess = sts_restore_fail;

		GetDlgItem(IDC_BTN_HOMING)->EnableWindow(!bEnable);
		GetDlgItem(IDCANCEL)->EnableWindow(bEnable);
		strAfxMessage.Format(_T("Restoration failed."));
	}

	SetProcess(nProcess);

	if (sts_restore_processing != GetProcess())
	{
		
		m_btnExit.SetWindowText(_T("Exit"));
		KillTimer(TIMER_RESOTRE_PROGRESS);
		AfxMessageBox(strAfxMessage);
	}
}

void CDlgRestoreHomingAlarm::MonitoringHomingStatus()
{
	CString strAfxMessage;
	CString strStatus, strDetailStatus;
	CString strOK, strNG;

	INT nProcess;
	INT nPos, nMin, nMax;
	BOOL bEnable = FALSE;
	BOOL bRestoring = TRUE;
	static INT nTick = 0;
	static INT nLogPos = 0;

	nProcess = sts_homing_processing;

	strOK.LoadString(IDS_DEVICE_HOMING_DONE);
	strNG.LoadString(IDS_DEVICE_HOMING_FAIL);

	m_pDevice->GetDetailStatus(&nPos, &nMin, &nMax);
	SetProgressBar(nPos, nMin, nMax);

	m_pDevice->GetSimpleStatus(strStatus);
	strDetailStatus = m_pDevice->GetDetailStatus();

	if (m_strStatus != strDetailStatus)
	{
		SetDlgItemText(IDC_ST_STATUS, strDetailStatus);
		m_strStatus = strDetailStatus;
	}
	
	if (CDeviceMotionGate::state_home_all == m_pDevice->GetStatus())
	{
		bEnable = FALSE;
		nProcess = sts_homing_processing;

		m_btnHome.SetImage(IDB_HOMING_OFF);
		m_btnHome.SetWindowText(_T("Homing Disable"));
		GetDlgItem(IDC_BUTTON_EXIT)->SetWindowText(_T("Stop"));

		GetDlgItem(IDC_BTN_HOMING)->EnableWindow(bEnable);

		if (nLogPos != nPos)
		{
			nTick = 0;
			nLogPos = nPos;
		}
		else
		{
			nTick++;
			if (nTick == 300)
			{
				if (strStatus != strOK)
				{
					strAfxMessage = _T("Homing failed.");
					
					m_pDevice->SetHomingStop(TRUE);
					if (CDeviceMotionGate::state_home_all == m_pDevice->GetStatus())
					{
						m_pDevice->StopSequence();
					}

					m_btnExit.SetWindowText(_T("Exit"));
					KillTimer(TIMER_HOMING_PROGRESS);
					AfxMessageBox(strAfxMessage);
				}
				nTick = 0;
				nLogPos = 0;
			}
		}

		
	}
	else
	{
		if (strStatus == strOK)
		{
			strAfxMessage = _T("Homing success.");
			nProcess = sts_homing_success;
		}
		/*else if (strStatus == strNG)
		{
			strAfxMessage = _T("Homing failed.");
			nProcess = sts_homing_fail;
		}*/
		else
		{
			strAfxMessage = _T("Homing failed.");
			nProcess = sts_homing_fail;

		}
		GetDlgItem(IDC_BUTTON_EXIT)->SetWindowText(_T("Exit"));
	}
	
	SetProcess(nProcess);
	
	if (sts_homing_processing != GetProcess())
	{
		m_btnExit.SetWindowText(_T("Exit"));
		KillTimer(TIMER_HOMING_PROGRESS);
		AfxMessageBox(strAfxMessage);
	}
}

void CDlgRestoreHomingAlarm::SetProgressBar(INT nPos, INT nMin, INT nMax)
{
	m_progress.SetRange(nMin, nMax);
	m_progress.SetPos(nPos);
}


void CDlgRestoreHomingAlarm::InitButtonImage()
{
	if (true)
	{// Exit Button
		m_btnExit.SubclassDlgItem(IDC_BUTTON_EXIT, this);
	}
	
	if (true)
	{ // Homing Button
		m_btnHome.SubclassDlgItem(IDC_BTN_HOMING, this);
		m_btnHome.SetImage(IDB_HOMING_ON);
		m_btnHome.SetWindowText(_T("\t Homing \nStart"));
	}
}


void CDlgRestoreHomingAlarm::OnBnClickedMfcbuttonHoming()
{
	
	
}


const BOOL CDlgRestoreHomingAlarm::IsServoOff()
{
	BOOL bAllServoOff = TRUE;

	CString strStatus, strOK, strNG;

	strOK.LoadString(IDS_DEVICE_RESTORATION_DONE);
	strNG.LoadString(IDS_DEVICE_RESTORATION_FAIL);

	// "Restoration : OK"
	// "Restoration : NG"

	m_pDevice->GetSimpleStatus(strStatus);

	if (true)
	{ 
		for (INT nAxis = 1; nAxis < 9; nAxis++)
		{
			if (m_pDevice->IsMotorConnected(nAxis))
			{
				BOOL bCheck = m_pDevice->IsMotorServoOff(nAxis);

				bAllServoOff &= m_pDevice->IsMotorServoOff(nAxis);
			}
		}
	}
	
	return bAllServoOff;
}


const BOOL CDlgRestoreHomingAlarm::IsWait()
{
	return m_bWait;
}


const BOOL CDlgRestoreHomingAlarm::Waiting()
{
	// TODO: 여기에 구현 코드 추가.
	return false;
}


BOOL CDlgRestoreHomingAlarm::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialogEx::OnCommand(wParam, lParam);
}


void CDlgRestoreHomingAlarm::InitDeviceImage()
{
	m_wndViewer.SubclassDlgItem(IDC_PICTURE_PROJECT, this);
	m_wndViewer.LoadMainImage(m_pDevice->GetSystemImage());
}


void CDlgRestoreHomingAlarm::SetProcess(INT nProcess)
{
	m_bProcess = nProcess;
}


const INT CDlgRestoreHomingAlarm::GetProcess()
{
	return m_bProcess;
}


void CDlgRestoreHomingAlarm::OnBnClickedBtnHoming()
{
	/*BOOL bEnable = !m_bHomingEnableText;

	if (bEnable)
	{
		m_btnHome.SetImage(IDB_HOMING_ON);
		m_btnHome.SetWindowText(_T("Enabled"));
		m_bHomingEnableText = TRUE;
	}
	else
	{
		m_btnHome.SetImage(IDB_HOMING_OFF);
		m_btnHome.SetWindowText(_T("Disabled"));
		m_bHomingEnableText = FALSE;
	}*/

	/*
	BOOL bResult = FALSE;
	bResult = AskHomingAll();

	if (bResult)
	{ // 복원을 원할 경우
		//SetTimer(TIMER_HOMING_WAIT, 10, NULL);
		OnOK();
	}
	else
	{
		
	}
	*/

	OnOK();
}


void CDlgRestoreHomingAlarm::OnBnClickedMfcbuttonExist()
{
	BOOL bResult = FALSE;
	switch (GetProcess())
	{
	case sts_restore_processing:
	{
		bResult = AskRestorationStop();
		if (TRUE == bResult)
		{
			if (CDeviceMotionGate::state_restoration != m_pDevice->GetStatus())
			{// state_restoration(6), 복원이 이미 완료되었을 경우
				AfxMessageBox(_T("The restore has already been completed."));
			}
			else
			{ // 복원중일경우
				
				m_pDevice->StopSequence();
			}

			OnCancel();
		}
		else
		{ // Restore Stop 진행하지 않을 경우
			// Restore Stop을 진행하지 않고 Homing을 진행하고 싶은 경우도 있을수 있으니..다시 화면으로 돌아간다.. 
		}

		
	}	break;

	/*
	case sts_restore_success:
	{
		if (m_bHomingEnableText)
		{ // Homing Button이 Enable 일때
			bResult = AskHomingAll();
			if (bResult)
			{ // 원점탐색을 원할 경우
				OnOK();
			}
			else
			{
				OnCancel();
			}
		}
		else
		{ // Homing Button이 Disable일때
			OnCancel(); 
		}
	}	break;
	*/

	case sts_homing_processing:
	{
		bResult = AskHomingStop();
		if (TRUE == bResult)
		{
			//m_pDevice->SetHomingStop(TRUE);

			if (CDeviceMotionGate::state_home_all != m_pDevice->GetStatus())
			{// state_restoration(6), 복원이 이미 완료되었을 경우
				AfxMessageBox(_T("The Homing has already been completed."));
			}
			else
			{ // Homing중일경우
				m_pDevice->SetHomingStop(TRUE);
			}

			OnCancel();
		}
		else
		{

		}
	}	break;
	
	default:
	{ OnCancel(); }
		break;
	}


}


const BOOL CDlgRestoreHomingAlarm::AskRestorationStop()
{
	BOOL bResult = FALSE;

	CString message("");
	CString mainInstruction("Restore is in progress. Do you want to stop?");
	CString title(_T("Confirm to Restoration stop the system"));

	CTaskDialog taskDialog(message, mainInstruction, title, !(TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON));

	taskDialog.AddCommandControl(IDYES, _T("Yes, I want to stop restore."));
	taskDialog.AddCommandControl(IDNO, _T("No, I don't want to stop restore."));

	taskDialog.SetDialogWidth(350);

	switch (taskDialog.DoModal())
	{
	case IDYES:
	{
		bResult = TRUE;
	}	break;

	case IDNO:
	{
		bResult = FALSE;
	}	break;
	}

	return bResult;
}


const BOOL CDlgRestoreHomingAlarm::AskHomingAll()
{// 사용X
	BOOL bResult = FALSE;

	CString message("");
	CString mainInstruction("the restore is complete. Do you want to Homing run?");
	CString title(_T("Confirm homing execution behavior"));

	CTaskDialog taskDialog(message, mainInstruction, title, !(TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON));

	taskDialog.AddCommandControl(IDYES, _T("Yes, I want to Homing run."));
	taskDialog.AddCommandControl(IDNO, _T("No, I don't want to Homing run."));

	taskDialog.SetDialogWidth(350);

	switch (taskDialog.DoModal())
	{
	case IDYES:
	{
		bResult = TRUE;
	}	break;

	case IDNO:
	{
		bResult = FALSE;
	}	break;
	}

	return bResult;
}

const BOOL CDlgRestoreHomingAlarm::AskHomingStop()
{ // 사용X
	BOOL bResult = FALSE;

	CString message("");
	CString mainInstruction("the Homing is running. Do you want to homing stop?");
	CString title(_T("Confirm homing stop behavior"));

	CTaskDialog taskDialog(message, mainInstruction, title, !(TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON));

	taskDialog.AddCommandControl(IDYES, _T("Yes, I want to Homing stop."));
	taskDialog.AddCommandControl(IDNO, _T("No, I don't want to Homing stop."));

	taskDialog.SetDialogWidth(350);

	switch (taskDialog.DoModal())
	{
	case IDYES:
	{
		bResult = TRUE;
	}	break;

	case IDNO:
	{
		bResult = FALSE;
	}	break;
	}

	return bResult;
}

const BOOL CDlgRestoreHomingAlarm::IsHomingReady()
{ // 사용X
	if (CDeviceMotionGate::state_motion_gate_command == m_pDevice->GetStatus())
	{
		return TRUE;
	}
	
	return FALSE;
}



const BOOL CDlgRestoreHomingAlarm::IsRestorationStart()
{ // 직전에 Restoration 진행되었었는지 체크후 시작시점까지 기다린다.
  // 이후 진행될 MonitoringRetoreStatus() 함수에서 Restoration : OK / Restoration : NG 가 인지되었을 경우 바로 모니터링을 종료시키기 때문.
	CString strStatus, strOK, strNG;
	static INT nLoadCount = 0;

	strOK.LoadString(IDS_DEVICE_RESTORATION_DONE);
	strNG.LoadString(IDS_DEVICE_RESTORATION_FAIL);

	m_pDevice->GetSimpleStatus(strStatus);

	if (0 == strStatus.CompareNoCase(strOK) ||
		0 == strStatus.CompareNoCase(strNG))
	{
		
		if (nLoadCount != 0)
		{ // Restoration 시작 후 완료때 메세지는 MonitoringRetoreStatus()를 진행해 줘야 하기 때문에 TRUE값을 반환한다.
			nLoadCount = 0;
			
			return TRUE;
		}
		else
		{
			return FALSE;
		}
		
	}

	nLoadCount++;

	return TRUE;
}


void CDlgRestoreHomingAlarm::InitControlTextSize()
{
	if (true)
	{
		LOGFONT lf = { 0, };
		afxGlobalData.fontRegular.GetLogFont(&lf);

		lf.lfHeight = -15;
		m_BigFont.CreateFontIndirect(&lf);

		GetDlgItem(IDC_PROGRESS_STATUS)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_STATUS)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_PROGRESS)->SetFont(&m_BigFont);
		
		m_BigFont.Detach();
	}

	/*if (true)
	{
		m_BigFont.CreateFont(FONT_SIZE, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, TEXT_STYLE);

		GetDlgItem(IDC_PROGRESS_STATUS)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_STATUS)->SetFont(&m_BigFont);

		m_BigFont.Detach();
	}*/
	
}
