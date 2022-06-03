// CDlgHomingControlPanel.cpp: 구현 파일
//

#include "pch.h"
#include "afxdialogex.h"
#include "FAS_PER_GUI.h"
#include "CDlgHomingControlPanel.h"


// CDlgHomingControlPanel 대화 상자

IMPLEMENT_DYNAMIC(CDlgHomingControlPanel, CDialogEx)

#define TIMER_PERIOD 50

CDlgHomingControlPanel::CDlgHomingControlPanel(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SYSTEM_HOMING, pParent)
{
	m_pDevice = NULL;
	m_nSelectedNumber = -1;
	m_stMotor[8] = { 0, };
}

CDlgHomingControlPanel::~CDlgHomingControlPanel()
{
	CloseHomingMotion();
}



void CDlgHomingControlPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgHomingControlPanel, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ALARM_RST, &CDlgHomingControlPanel::OnBnClickedMfcbuttonAlarmRst)
	ON_WM_TIMER()
	
	ON_BN_CLICKED(IDC_BUTTON_MOTOR1, &CDlgHomingControlPanel::OnBnClickedMfcbuttonMotor1)
	ON_BN_CLICKED(IDC_BUTTON_MOTOR2, &CDlgHomingControlPanel::OnBnClickedMfcbuttonMotor2)
	ON_BN_CLICKED(IDC_BUTTON_MOTOR3, &CDlgHomingControlPanel::OnBnClickedMfcbuttonMotor3)
	ON_BN_CLICKED(IDC_BUTTON_MOTOR4, &CDlgHomingControlPanel::OnBnClickedMfcbuttonMotor4)
	ON_BN_CLICKED(IDC_BUTTON_MOTOR5, &CDlgHomingControlPanel::OnBnClickedMfcbuttonMotor5)
	ON_BN_CLICKED(IDC_BUTTON_MOTOR6, &CDlgHomingControlPanel::OnBnClickedMfcbuttonMotor6)
	ON_BN_CLICKED(IDC_BUTTON_MOTOR7, &CDlgHomingControlPanel::OnBnClickedMfcbuttonMotor7)
	ON_BN_CLICKED(IDC_BUTTON_MOTOR8, &CDlgHomingControlPanel::OnBnClickedMfcbuttonMotor8)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgHomingControlPanel::OnBnClickedMfcbuttonClose)
	ON_BN_CLICKED(IDC_BUTTON_ALL_START, &CDlgHomingControlPanel::OnBnClickedMfcbuttonAllStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP_ALL, &CDlgHomingControlPanel::OnBnClickedMfcbuttonStopAll)
	ON_BN_CLICKED(IDC_CHECK_SERVO, &CDlgHomingControlPanel::OnBnClickedCheckServo)
	ON_BN_CLICKED(IDC_CHECK2, &CDlgHomingControlPanel::OnBnClickedCheck2)
END_MESSAGE_MAP()


// CDlgHomingControlPanel 메시지 처리기

void CDlgHomingControlPanel::OnBnClickedMfcbuttonClose()
{
	if (TRUE == AskClose())
	{

		for (INT nAxis = 1; nAxis < 9; nAxis++)
		{
			if (m_pDevice->IsMotorConnected(nAxis))
			{
				m_pDevice->MoveStop(nAxis);
			}
		}

		m_pDevice->SetHomingClose(TRUE);
		//m_pDevice->SendHomingDoneMessage(TRUE);

		OnOK();
	}
}

void CDlgHomingControlPanel::SetMotionGate(CDeviceMotionGate* pDevice)
{
	m_pDevice = pDevice;
}

BOOL CDlgHomingControlPanel::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	//OpenHomingMotion();

	InitButtonImage();

	InitControlTextSize();

	SetTimer(TIMER_LOAD_IMAGE, 300, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgHomingControlPanel::InitButtonImage()
{
	/*m_btnReset.SubclassDlgItem(IDC_BUTTON_ALARM_RST, this);
	m_btnReset.SetImage(IDB_HOMING_ON);*/
	
	// CMFCButtonEx m_btnReset, m_btnHoming, m_btnStop, m_btnClose;

	/*
		hRes = Image.Load(L"res/TotalConfig/Restore.png");
		if (hRes != S_OK)
		{
			ASSERT(0);
		}
		hBit = Image.Detach();

		m_restore.SubclassDlgItem(IDC_BUTTON_RESTORE, this);
		m_restore.SetImage(hBit, TRUE);
	*/
	CImage Image;
	HBITMAP hBit;
	HRESULT hRes;

	if (true)
	{
		hRes = Image.Load(L"res/TotalConfig/Close.png");
		if (hRes != S_OK)
		{
			ASSERT(0);
		}
		hBit = Image.Detach();

		m_btnClose.SubclassDlgItem(IDC_BUTTON_CLOSE, this);
		m_btnClose.SetImage(hBit, TRUE);
	}
	
	if (true)
	{
		hRes = Image.Load(L"res/TotalConfig/Homing Stop.png");
		if (hRes != S_OK)
		{
			ASSERT(0);
		}
		hBit = Image.Detach();

		m_btnStop.SubclassDlgItem(IDC_BUTTON_STOP_ALL, this);
		m_btnStop.SetImage(hBit, TRUE);
	}

	if (true)
	{
		hRes = Image.Load(L"res/TotalConfig/Homing.png");
		if (hRes != S_OK)
		{
			ASSERT(0);
		}
		hBit = Image.Detach();

		m_btnHoming.SubclassDlgItem(IDC_BUTTON_ALL_START, this);
		m_btnHoming.SetImage(hBit, TRUE);
	}

	if (true)
	{
		hRes = Image.Load(L"res/TotalConfig/Reset.png");
		if (hRes != S_OK)
		{
			ASSERT(0);
		}
		hBit = Image.Detach();

		m_btnAlarmRst.SubclassDlgItem(IDC_BUTTON_ALARM_RST, this);
		m_btnAlarmRst.SetImage(hBit, TRUE);
	}
	
}


void CDlgHomingControlPanel::InitStaticImage()
{
	CImage Image;
	HBITMAP hBit;
	HRESULT hRes;

	CMFCButton* pButton;

	if (TRUE == IsWindowVisible())
	{
		for (INT nIndex = 1; nIndex <= 8; nIndex++)
		{
			SetMotorStaticImage(nIndex - 1, sts_homing_ready);
		}

		for (INT nAxis = 0; nAxis < 8; nAxis++)
		{
			m_wndColor[nAxis].SubclassDlgItem(IDC_ST_MOTOR1 + nAxis, this);
		}

		m_wndTotalColor.SubclassDlgItem(IDC_STATIC_STATUS, this);

		m_wndViewer.SubclassDlgItem(IDC_STATIC_SYSTEM_IMAGE, this);
		m_wndViewer.LoadMainImage(m_pDevice->GetSystemImage());

	/*	if (1)
		{
			hRes = Image.Load(L"res/TotalConfig/Reset.png");
			if (hRes != S_OK)
			{
				ASSERT(0);
			}
			hBit = Image.Detach();

			pButton = (CMFCButton*)GetDlgItem(IDC_MFCBUTTON_ALARM_RST);
			pButton->m_nFlatStyle = CMFCButton::BUTTONSTYLE_3D;
			pButton->m_bTransparent = TRUE;
			pButton->m_bDrawFocus = FALSE;

			pButton->SetImage(hBit, TRUE);
			pButton->SetWindowText(L"Alarm Reset");
		}

		if (1)
		{
			hRes = Image.Load(L"res/TotalConfig/Homing.png");
			if (hRes != S_OK)
			{
				ASSERT(0);
			}
			hBit = Image.Detach();

			pButton = (CMFCButton*)GetDlgItem(IDC_MFCBUTTON_ALL_START);
			pButton->m_nFlatStyle = CMFCButton::BUTTONSTYLE_3D;
			pButton->m_bTransparent = TRUE;
			pButton->m_bDrawFocus = FALSE;

			pButton->SetImage(hBit, TRUE);
			pButton->SetWindowText(L"Start All");
		}

		if (1)
		{
			hRes = Image.Load(L"res/TotalConfig/Homing Stop.png");
			if (hRes != S_OK)
			{
				ASSERT(0);
			}
			hBit = Image.Detach();

			pButton = (CMFCButton*)GetDlgItem(IDC_MFCBUTTON_STOP_ALL);
			pButton->m_nFlatStyle = CMFCButton::BUTTONSTYLE_3D;
			pButton->m_bTransparent = TRUE;
			pButton->m_bDrawFocus = FALSE;

			pButton->SetImage(hBit, TRUE);
			pButton->SetWindowText(L"Stop All");
		}

		if (1)
		{
			hRes = Image.Load(L"res/TotalConfig/Close.png");
			if (hRes != S_OK)
			{
				ASSERT(0);
			}
			hBit = Image.Detach();

			pButton = (CMFCButton*)GetDlgItem(IDC_BUTTON_CLOSE);
			pButton->m_nFlatStyle = CMFCButton::BUTTONSTYLE_3D;
			pButton->m_bTransparent = TRUE;
			pButton->m_bDrawFocus = FALSE;

			pButton->SetImage(hBit, TRUE);
			pButton->SetWindowText(L"Close");
		}*/
	}
	
}


void CDlgHomingControlPanel::SetMotorStaticImage(INT nIndex, INT nState)
{
	// OninitDialog에서 바로 실행시키면 화면 현시가 안된다.
	CImage image;
	
	switch (nState)
	{
	case sts_homing_ready:
	{
		image.Load(L"res/TotalConfig/homing_ready.bmp");
	}	break;

	case sts_homing_done:
	{
		image.Load(L"res/TotalConfig/homing_done.bmp");
	}	break;

	case sts_homing_processing:
	{
		image.Load(L"res/TotalConfig/homing_progress.bmp");
	}	break;

	case sts_homing_failed:
	{
		image.Load(L"res/TotalConfig/homing_fail.bmp");
	}	break;

	default:
		break;
	}
	
	((CStatic*)GetDlgItem(IDC_STATIC_MOTOR1 + nIndex))->SetBitmap(image);
}





void CDlgHomingControlPanel::OnBnClickedMfcbuttonAlarmRst()
{
	m_pDevice->AlarmReset();
	
	//SetTimer(TIMER_ALARM_RST_BUTTON, 500, NULL);
}


void CDlgHomingControlPanel::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	switch (nIDEvent)
	{
	case TIMER_LOAD_IMAGE:
	{
		KillTimer(nIDEvent);
		
		InitStaticImage();

		//InitButtonImage();

		SetTimer(TIMER_MONITOR_BUTTON, 100, NULL);
	}	break;
	case TIMER_HOMING_MOTOR1:
	case TIMER_HOMING_MOTOR2:
	case TIMER_HOMING_MOTOR3:
	case TIMER_HOMING_MOTOR4:
	case TIMER_HOMING_MOTOR5:
	case TIMER_HOMING_MOTOR6:
	case TIMER_HOMING_MOTOR7:
	case TIMER_HOMING_MOTOR8:
	{
		if (FALSE == m_pDevice->IsActiveHomingAll())
		{
			EraseProgressBarStatus();
			SetHomingAllTime(0);
		}

		HomingRun(nIDEvent);
	}	break;

	case TIMER_HOMING_TOTAL:
	{
		HomingAll();
	}	break;

	case TIMER_MONITOR_BUTTON:
	{
		MonitorBtnStatus();
	}	break;

	case TIMER_ALARM_RST_BUTTON:
	{
		KillTimer(nIDEvent);
		//MotorAlarmReset();
	}	break;

	default:
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CDlgHomingControlPanel::HomingRun(INT nMotorIndex)
{
	INT nAxis = nMotorIndex + 1;
	INT nState = 0;

	if (FALSE == m_stMotor[nMotorIndex].bHoming)
	{
		nState = sts_homing_ready;

		m_pDevice->ActiveHoming();

		m_pDevice->SetInitHomingState(nAxis);

		m_stMotor[nMotorIndex].nStartWaiting = 0;

		if (m_pDevice->IsMotorAlarmDetected(nMotorIndex))
		{ // 시작시 알람 확인..
			nState = sts_homing_failed;

			m_stMotor[nMotorIndex].bHomingDone = FALSE;
			m_stMotor[nMotorIndex].bHomingFail = TRUE;
			m_stMotor[nMotorIndex].bHomingReady = FALSE;
			m_stMotor[nMotorIndex].bHomingStop = FALSE;
		}
		else
		{
			m_pDevice->ExecuteOriginSerach(nAxis);

			if (m_pDevice->GetStatus() == CDeviceMotionGate::state_home_all)
			{ 
				if (TRUE == m_pDevice->IsHoming(nAxis))
				{
					m_stMotor[nMotorIndex].bHoming = TRUE;
				}
			}

			m_stMotor[nMotorIndex].bHomingDone = FALSE;
			m_stMotor[nMotorIndex].bHomingFail = FALSE;
			m_stMotor[nMotorIndex].bHomingReady = FALSE;
			m_stMotor[nMotorIndex].bHomingStop = FALSE;
		}
		
	}
	else
	{
		//// 두번 이상 실행시 CDeviceMotionGate에서는 이전 마지막 상태값을 가지고 있다.
		//if (m_stMotor[nMotorIndex].nStartWaiting < 15)
		//{
		//	m_stMotor[nMotorIndex].nStartWaiting++;
		//	return;
		//}
	
		/*
		if (FALSE == m_pDevice->IsMotorServoOff(nAxis))
		{ // Servo On 확인후 진행..
			if (TRUE == m_pDevice->IsHoming(nAxis))
			{
				nState = sts_homing_processing;
			}
			else
			{ // Waiting은 이쪽으로 들어온다.
				if (m_pDevice->IsHomingOK(nAxis))
				{
					nState = sts_homing_done;
					m_stMotor[nMotorIndex].bHomingDone = TRUE;
				}

				else if (m_pDevice->IsHomingNG(nAxis))
				{
					nState = sts_homing_failed;
					m_stMotor[nMotorIndex].bHomingFail = TRUE;
				}

				//if (nState == sts_homing_processing)
				//{
				//	//m_stMotor[nMotorIndex].bHoming = TRUE;
				//}
				
			}
		}
		*/

		if (TRUE == m_pDevice->IsHoming(nAxis))
		{
			nState = sts_homing_processing;
		}
		else
		{ // Waiting은 이쪽으로 들어온다.
			if (m_pDevice->IsHomingOK(nAxis))
			{
				nState = sts_homing_done;
				m_stMotor[nMotorIndex].bHomingDone = TRUE;
			}

			else if (TRUE == m_pDevice->IsHomingNG(nAxis))
			{
				nState = sts_homing_failed;
				m_stMotor[nMotorIndex].bHomingFail = TRUE;
			}
		}
		
		if (TRUE == m_pDevice->IsMotorAlarmDetected(nAxis))
		{
			nState = sts_homing_failed;
			m_stMotor[nMotorIndex].bHomingFail = TRUE;
		}
	}

	if (nState == sts_homing_done ||
		nState == sts_homing_failed)
	{
		m_stMotor[nMotorIndex].bHoming = FALSE;
		KillTimer(nMotorIndex);
		m_pDevice->SetHomingClose(TRUE);
	}

	if (m_stMotor[nMotorIndex].nStatus != nState)
	{
		//SetMotorStaticImage(nMotorIndex, nState);
		m_stMotor[nMotorIndex].nStatus = nState;
	}
}




void CDlgHomingControlPanel::SelectedMotorIndex(INT nSelected)
{
	m_nSelectedNumber = nSelected;
}


const INT CDlgHomingControlPanel::GetSelectedMotorIndex()
{
	return m_nSelectedNumber;
}


void CDlgHomingControlPanel::OnBnClickedMfcbuttonMotor1()
{
	if (TRUE == AskHoming())
	{
		m_pDevice->SetActiveHomingAll(FALSE);
		SelectedMotorIndex(TIMER_HOMING_MOTOR1);
		SetTimer(GetSelectedMotorIndex(), TIMER_PERIOD, NULL);
	}
}


void CDlgHomingControlPanel::OnBnClickedMfcbuttonMotor2()
{
	if (TRUE == AskHoming())
	{
		m_pDevice->SetActiveHomingAll(FALSE);
		SelectedMotorIndex(TIMER_HOMING_MOTOR2);
		SetTimer(GetSelectedMotorIndex(), TIMER_PERIOD, NULL);
	}
}


void CDlgHomingControlPanel::OnBnClickedMfcbuttonMotor3()
{
	if (TRUE == AskHoming())
	{
		m_pDevice->SetActiveHomingAll(FALSE);
		SelectedMotorIndex(TIMER_HOMING_MOTOR3);
		SetTimer(GetSelectedMotorIndex(), TIMER_PERIOD, NULL);
	}
}


void CDlgHomingControlPanel::OnBnClickedMfcbuttonMotor4()
{
	if (TRUE == AskHoming())
	{
		m_pDevice->SetActiveHomingAll(FALSE);
		SelectedMotorIndex(TIMER_HOMING_MOTOR4);
		SetTimer(GetSelectedMotorIndex(), TIMER_PERIOD, NULL);
	}
}


void CDlgHomingControlPanel::OnBnClickedMfcbuttonMotor5()
{
	if (TRUE == AskHoming())
	{
		m_pDevice->SetActiveHomingAll(FALSE);
		SelectedMotorIndex(TIMER_HOMING_MOTOR5);
		SetTimer(GetSelectedMotorIndex(), TIMER_PERIOD, NULL);
	}
}


void CDlgHomingControlPanel::OnBnClickedMfcbuttonMotor6()
{
	if (TRUE == AskHoming())
	{
		m_pDevice->SetActiveHomingAll(FALSE);
		SelectedMotorIndex(TIMER_HOMING_MOTOR6);
		SetTimer(GetSelectedMotorIndex(), TIMER_PERIOD, NULL);
	}
}


void CDlgHomingControlPanel::OnBnClickedMfcbuttonMotor7()
{
	if (TRUE == AskHoming())
	{
		m_pDevice->SetActiveHomingAll(FALSE);
		SelectedMotorIndex(TIMER_HOMING_MOTOR7);
		SetTimer(GetSelectedMotorIndex(), TIMER_PERIOD, NULL);
	}
}


void CDlgHomingControlPanel::OnBnClickedMfcbuttonMotor8()
{
	if (TRUE == AskHoming())
	{
		m_pDevice->SetActiveHomingAll(FALSE);
		SelectedMotorIndex(TIMER_HOMING_MOTOR8);
		SetTimer(GetSelectedMotorIndex(), TIMER_PERIOD, NULL);
	}
}


void CDlgHomingControlPanel::OnBnClickedMfcbuttonAllStart()
{
	if (TRUE == AskHomingAllStart())
	{
		CloseHomingMotion();
		
		BOOL bAlarmDetected = FALSE;
		for (INT nAxis = 1; nAxis < 9; nAxis++)
		{
			if (m_pDevice->IsMotorConnected(nAxis))
			{
				if (m_pDevice->IsMotorAlarmDetected(nAxis))
				{
					bAlarmDetected |= TRUE;
				}
			}
		}

		if (TRUE == bAlarmDetected)
		{ // Motor가 하나라도 알람이 발생되어 있으면 진행하지 않는다.

			AfxMessageBox(_T("Motor alarm detected."));
			return;
		}
		
		m_pDevice->SetActiveHomingAll(TRUE);
		//m_pDevice->ActiveHoming();
		
		for (INT nAxis = 1; nAxis < 8; nAxis++)
		{
			if (m_pDevice->IsMotorConnected(nAxis))
			{
				//m_pDevice->SetInitHomingState(nAxis);
				SetTimer(nAxis - 1, 100, NULL);
			}
		}

		SetTimer(TIMER_HOMING_TOTAL, 300, NULL);
	}
}


void CDlgHomingControlPanel::OnBnClickedMfcbuttonStopAll()
{
	INT nStatus = m_pDevice->GetStatus();

	/*if (nStatus == CDeviceMotionGate::state_home_all)
	{
		m_pDevice->SetHomingStop(TRUE);
	}
	else
	{
		for (INT nAxis = 1; nAxis < 9; nAxis++)
		{
			if (m_pDevice->IsHoming(nAxis))
			{
				m_pDevice->MoveStop(nAxis);
			}
		}
	}*/

	for (INT nAxis = 1; nAxis < 9; nAxis++)
	{
		if (m_pDevice->IsHoming(nAxis))
		{
			m_pDevice->MoveStop(nAxis);
		}
	}
	
}


void CDlgHomingControlPanel::MonitorBtnStatus()
{
	if (TRUE)
	{// System Homing Check
		BOOL bHoming = m_pDevice->IsSystemHoming();

		BOOL bAlarmDected = FALSE;
		BOOL bServoOff = TRUE;
		
		for (INT nAxis = 1; nAxis < 9; nAxis++)
		{
			if (m_pDevice->IsMotorConnected(nAxis))
			{ // 모두 Off가 되어 있어야 한다.
				bServoOff &= m_pDevice->IsMotorServoOff(nAxis);
			}
		}

		for (INT nAxis = 1; nAxis < 9; nAxis++)
		{
			if (m_pDevice->IsMotorConnected(nAxis))
			{ // 한개라도 알람이 발생어야 처리..
				bAlarmDected |= m_pDevice->IsMotorAlarmDetected(nAxis);
			}
		}
		
		GetDlgItem(IDC_BUTTON_ALARM_RST)->EnableWindow(!bHoming && bAlarmDected);
		GetDlgItem(IDC_BUTTON_ALL_START)->EnableWindow(!bHoming && bServoOff);
		//GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(!bHoming);
	}
	
	for (INT nAxis = 1; nAxis < 9; nAxis++)
	{
		BOOL bConnected = m_pDevice->IsMotorConnected(nAxis);
		
		BOOL bEnableBtn = FALSE;
		
		if (bConnected)
		{
			if ((FALSE == m_pDevice->IsActiveHomingAll()))
			{ // Homing All 모드중에서는 개별 모터 Start가 정지되어 있다 하더라도 Disable 되어야 한다.
				if (m_pDevice->IsMotorStopped(nAxis) == TRUE)
				{
					bEnableBtn = TRUE;
				}
			}
		}
		

		GetDlgItem(IDC_BUTTON_MOTOR1 + (nAxis - 1))->EnableWindow(bEnableBtn);

		GetDlgItem(IDC_ST_MOTOR1 + (nAxis - 1))->EnableWindow(bConnected);
		
		SetMotorStaticImage(nAxis - 1, bConnected ? m_stMotor[nAxis - 1].nStatus : sts_homing_ready);
		
		if (bConnected)
		{
			SetStatusBkColor(nAxis, m_stMotor[nAxis - 1].nStatus);
		}
	}


	if (1)
	{ // Debuging 용도
		BOOL bMotioningCheck = FALSE;
		for (INT nAxis = 1; nAxis < 9; nAxis++)
		{ // 하나라도 동작중인 경우 TRUE
			bMotioningCheck |= m_stMotor[nAxis - 1].bHoming;
		}
		GetDlgItem(IDC_BUTTON_STOP_ALL)->EnableWindow(bMotioningCheck);
	}
	if (1)
	{ // Debuging 용도
		INT nStatus = m_pDevice->GetStatus();
		CString strStatus;
		strStatus.Format(_T("Device Status : %d"), nStatus);
		(GetDlgItem(IDC_STATIC_DEVICE_STATUS))->SetWindowText(strStatus);
	}

	
}


void CDlgHomingControlPanel::SetStatusBkColor(INT nAxis, INT nStatus)
{
	CString str;
	CString strCheck;
	COLORREF bkColor, textColor;
	COLORREF red, white, yellow, black, blue, green, gray, none;
	
	none = RGB(240, 240, 240);
	red = RGB(255, 0, 0);
	white = RGB(255, 255, 255);
	yellow = RGB(255, 255, 0);
	black = RGB(0, 0, 0);
	blue = RGB(75, 137, 220);
	green = RGB(0x00, 0xc8, 0x00);
	gray = RGB(0x32, 0x32, 0x32);

	bkColor = none;
	textColor = black;

	switch (nStatus)
	{
	case sts_homing_ready:
	{
		if (m_pDevice->IsMotorAlarmDetected(nAxis))
		{
			str.Format(_T("[Motor #%d] Alarm"), nAxis);
			bkColor = red;
			textColor = black;
		}
		else
		{
			str.Format(_T("[Motor #%d] Ready"), nAxis);
			bkColor = gray;
			textColor = white;
		}
		
	}	break;
	case sts_homing_failed:
	{
		BOOL bCheck = m_pDevice->IsMotorAlarmDetected(nAxis);

		
		if (m_pDevice->IsMotorAlarmDetected(nAxis))
		{
			str.Format(_T("[Motor #%d] Alarm"), nAxis);
			bkColor = red;
			textColor = black;
		}
		else
		{ // Alarm이 아닌 다른 이유로 원점탐색 실패할 경우 Ready로 넘어감.
			/*str.Format(_T("[Motor #%d] Ready"), nAxis);
			bkColor = none;
			textColor = black;*/

			str.Format(_T("[Motor #%d] Failed"), nAxis);
			bkColor = red;
			textColor = black;
		}
		
	}	break;

	case sts_homing_processing:
	{
		str.Format(_T("[Motor #%d] Homing"), nAxis);
		bkColor = yellow;
		textColor = black;
	}	break;

	case sts_homing_done:
	{
		str.Format(_T("[Motor #%d] Done"), nAxis);
		bkColor = green;
		textColor = black;
	}	break;
	}

	GetDlgItemText(IDC_ST_MOTOR1 + (nAxis - 1), strCheck);

	if (0 != strCheck.Compare(str))
	{
		m_wndColor[nAxis - 1].SetWindowText(str);
		m_wndColor[nAxis - 1].SetBkColor(bkColor);
		m_wndColor[nAxis - 1].SetTextColor(textColor);
	}

}


void CDlgHomingControlPanel::OpenHomingMotion()
{
	m_pDevice->EnableAutoHoming(TRUE);
}


void CDlgHomingControlPanel::CloseHomingMotion()
{// 모두가 꺼져야 HomingMotion 상태 Close
	
	/*
	BOOL bServoOn = FALSE;

	for (INT nAxis = 1; nAxis < 9; nAxis++)
	{ // 하나라도 True이면 bHoming은 True.
		bServoOn |= m_pDevice->IsServoEnabled(nAxis);

		if (nAxis == 8)
		{
			if (FALSE == bServoOn)
			{
				if (TRUE == m_pDevice->IsAutoHomingEnabled())
				{
					m_pDevice->EnableAutoHoming(FALSE); // 모두 정지해 있으면 Homing상태 종료한다.
				}
			}
		}
	}
	*/
	
	/*
	BOOL bClose = FALSE;

	for (INT nAxis = 0; nAxis < 8; nAxis++)
	{
		switch (m_stMotor[nAxis].nStatus)
		{
		case sts_homing_done:
		case sts_homing_failed:
		{
			bClose |= TRUE;
		}	break;

		default:
		{

		}	break;
		}
	}

	if (bClose)
	{
		m_pDevice->EnableAutoHoming(FALSE); // 모두 진행이 완료된 상태일 경우.
	}
	*/
	
	
	// 어차피 Homing은 Restoration이 시작되고 선택하는 것이므로, Motion중일리가 없다.
	m_pDevice->EnableAutoHoming(FALSE); 
	
}


void CDlgHomingControlPanel::MotorAlarmReset()
{ 
	// Motor Alarm Display
	
	for (INT nAxis = 0; nAxis < 8; nAxis++)
	{
		if (FALSE == m_stMotor[nAxis].bHoming)
		{ // 정지 상태 일때.

			BOOL bAxisAlarm = m_pDevice->IsMotorAlarmDetected(nAxis + 1);
			if (TRUE == bAxisAlarm)
			{ // Alarm 발생했을 경우
				m_stMotor[nAxis].nStatus = sts_homing_failed;
			}
			else
			{ // Alarm이 없을경우
				
			}

			SetStatusBkColor(nAxis + 1, m_stMotor[nAxis].nStatus);
		}
	}
}


void CDlgHomingControlPanel::HomingAll()
{
	CString str;
	COLORREF bkColor, textColor;
	COLORREF red, white, yellow, black, blue, green, gray,none;
	
	BOOL bReady = TRUE;
	BOOL bDone = TRUE;

	BOOL bHoming = FALSE;
	BOOL bAlarm = FALSE;
	BOOL bFailed = FALSE;

	//static INT nTick = 0;

	none = RGB(240, 240, 240);
	red = RGB(255, 0, 0);
	white = RGB(255, 255, 255);
	yellow = RGB(255, 255, 0);
	black = RGB(0, 0, 0);
	blue = RGB(75, 137, 220);
	green = RGB(0x00, 0xc8, 0x00);
	gray = RGB(0x32, 0x32, 0x32);

	bkColor = none;
	textColor = black;
	
	for (INT nAxis = 0; nAxis < 8; nAxis++)
	{
		if (m_pDevice->IsMotorConnected(nAxis + 1))
		{
			bReady &= (m_stMotor[nAxis].nStatus == sts_homing_ready); // 모두가 준비상태
			bDone &= (m_stMotor[nAxis].nStatus == sts_homing_done); // 모두가 완료인 상태

			bHoming |= (m_stMotor[nAxis].nStatus == sts_homing_processing); // 한개라도 모션이 진행중일 경우
			bFailed |= (m_stMotor[nAxis].nStatus == sts_homing_failed); // 한개라도 Failed 될 경우.
			bAlarm |= (m_pDevice->IsMotorAlarmDetected(nAxis + 1)); // 한개라도 Alarm 발생했을 경우.
		}
	}

	if (bReady)
	{
		str.Format(_T("Motor all ready."));
		bkColor = gray;
		textColor = white;
	}

	if (bHoming)
	{
		str.Format(_T("Homing..."));
		bkColor = yellow;
		textColor = black;
	}

	if (bDone)
	{
		str.Format(_T("Homing done."));
		bkColor = green;
		textColor = black;
	}

	if (bFailed)
	{
		str.Format(_T("Homing failed."));
		bkColor = red;
		textColor = black;
	}

	if (bAlarm)
	{
		str.Format(_T("Homing failed (Alarm dected)."));
		bkColor = red;
		textColor = black;
	}

	m_wndTotalColor.SetWindowText(str);
	m_wndTotalColor.SetBkColor(bkColor);
	m_wndTotalColor.SetTextColor(textColor);

	if (CDeviceMotionGate::state_home_all == m_pDevice->GetStatus())
	{
		SetHomingAllTime(m_pDevice->GetProgressEndTime());
	}
	

	for (INT nAxis = 0; nAxis < 8; nAxis++)
	{ // 모두가 정지해야만 KillTimer 을 시킨다.
		if (TRUE == m_stMotor[nAxis].bHoming) return;
		
		if (nAxis == 7)
		{// 모두 정지하거나 TimeOut 걸렸을 경우.
			if (TRUE == bDone ||
				TRUE == bFailed ||
         		TRUE == bAlarm)
			{
				m_pDevice->SetActiveHomingAll(FALSE);
				KillTimer(TIMER_HOMING_TOTAL);
			}
		}
	}
}


const BOOL CDlgHomingControlPanel::AskHoming()
{
	BOOL bResult = FALSE;

	CString message("");
	CString mainInstruction("Do you want to Homing run?");
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

const BOOL CDlgHomingControlPanel::AskHomingStop()
{
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

const BOOL CDlgHomingControlPanel::AskHomingAllStart()
{
	BOOL bResult = FALSE;

	CString message("");
	CString mainInstruction("Do you want to Homing all run?");
	CString title(_T("Confirm homing execution behavior"));

	CTaskDialog taskDialog(message, mainInstruction, title, !(TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON));

	taskDialog.AddCommandControl(IDYES, _T("Yes, I want to Homing all run."));
	taskDialog.AddCommandControl(IDNO, _T("No, I don't want to Homing all run."));

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

const BOOL CDlgHomingControlPanel::AskClose()
{
	BOOL bResult = FALSE;

	CString message("If the motor is running, the screen is closed after stopping.");
	CString mainInstruction("Are you sure you want to exit the Homing screen?");
	CString title(_T("Confirm Homing screen exit"));

	CTaskDialog taskDialog(message, mainInstruction, title, !(TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON));

	taskDialog.AddCommandControl(IDYES, _T("Yes, I want to exit the Homing screen"));
	taskDialog.AddCommandControl(IDNO, _T("No, I don't want to exit the Homing screen."));

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


void CDlgHomingControlPanel::InitControlTextSize()
{
	if (true)
	{
		LOGFONT lf = { 0, };
		afxGlobalData.fontRegular.GetLogFont(&lf);

		lf.lfHeight = -15;
		m_BigFont.CreateFontIndirect(&lf);

		GetDlgItem(IDC_STATIC_GROUP_STATUS)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_GROUP_IMAGE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_GROUP_MOTOR_STATUS)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_HOMING_READY)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_HOMMING)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_HOMING_DONE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_HOMING_FAIL)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_STATUS)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_SEC)->SetFont(&m_BigFont);

		for (INT nAxis = 0; nAxis < 8; nAxis++)
		{
			GetDlgItem(IDC_BUTTON_MOTOR1 + nAxis)->SetFont(&m_BigFont);
			GetDlgItem(IDC_ST_MOTOR1 + nAxis)->SetFont(&m_BigFont);
		}

		m_BigFont.Detach();
	}

	/*if (true)
	{
		m_BigFont.CreateFont(FONT_SIZE, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, TEXT_STYLE);

		GetDlgItem(IDC_STATIC_GROUP_STATUS)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_GROUP_IMAGE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_GROUP_MOTOR_STATUS)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_HOMING_READY)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_HOMMING)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_HOMING_DONE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_HOMING_FAIL)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_STATUS)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_SEC)->SetFont(&m_BigFont);

		for (INT nAxis = 0; nAxis < 8; nAxis++)
		{
			GetDlgItem(IDC_BUTTON_MOTOR1 + nAxis)->SetFont(&m_BigFont);
			GetDlgItem(IDC_ST_MOTOR1 + nAxis)->SetFont(&m_BigFont);
		}

		m_BigFont.Detach();
	}*/
}


void CDlgHomingControlPanel::SetHomingAllTime(DWORD dwTime)
{
	CString str;
	CString strStatic;

	GetDlgItemText(IDC_STATIC_SEC, strStatic);
	str.Format(_T("[ %d ] Sec"), dwTime / 10 + (dwTime % 10 > 4 ? 1 : 0));
	
	if (strStatic != str)
	{
		GetDlgItem(IDC_STATIC_SEC)->SetWindowText(str);
	}
}


void CDlgHomingControlPanel::EraseProgressBarStatus()
{
	m_wndTotalColor.SetWindowText(_T(""));
	m_wndTotalColor.SetBkColor(RGB(240, 240, 240));
}


void CDlgHomingControlPanel::OnBnClickedCheckServo()
{
	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_SERVO))->GetCheck();
	for (INT nAxis = 1; nAxis < 9; nAxis++)
	{
		if (m_pDevice->IsMotorConnected(nAxis))
		{
			m_pDevice->ServoControl(nAxis, bCheck);
		}
		
	}
}


void CDlgHomingControlPanel::OnBnClickedCheck2()
{
	for (INT nAxis = 1; nAxis < 9; nAxis++)
	{
		if (m_pDevice->IsMotorConnected(nAxis))
		{
			m_pDevice->CmdMotorMoveStop(nAxis);
		}
	}
}
