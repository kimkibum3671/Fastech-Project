﻿// CDlgMotionGateInform.cpp: 구현 파일
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CDlgMotionGateInform.h"
#include "CDataBaseManager.h"
#include "MainFrm.h"
#include "afxdialogex.h"
#include "CDlgHomingControlPanel.h"

BOOL CALLBACK EnumChildProcess(HWND hWnd, LPARAM lParam);

// CDlgMotionGateInform 대화 상자

IMPLEMENT_DYNAMIC(CDlgMotionGateInform, CDialogEx)

#define COLOR_NONE			RGB(240, 240, 240)
#define	COLOR_RED			RGB(255, 0, 0)
#define COLOR_DARKRED		RGB(200, 0, 0)
#define	COLOR_LIGHTRED		RGB(219, 68, 85)
#define	COLOR_GREEN			RGB(0, 255, 0)
#define	COLOR_YELLOW		RGB(255, 255, 0)
#define COLOR_ORANGE		RGB(255, 165, 0)
#define COLOR_ORIGINAL_BLUE RGB(0, 0, 255)
#define COLOR_BLUE			RGB(75, 137, 220)
#define COLOR_BLACK			RGB(0, 0, 0)
#define COLOR_WHITE			RGB(255, 255, 255)

#define COLOR_CONNECT       RGB(0, 0, 153) // COLOR_ORANGE
#define COLOR_BUSY			RGB(0x80, 0x80, 0x00) //RGB(153, 153, 255)
#define COLOR_WARNING		RGB(102, 204, 051) // COLOR_GREEN
#define COLOR_ERROR			RGB(255, 0, 0)
#define COLOR_LINE			RGB(150, 150, 150)

#define BLINK_SPEED_SLOW 500
#define BLINK_SPEED_NORMAL 300
#define BLINK_SPEED_FAST 200
#define BLINK_SPEED_VERYFAST 100

 
//#define STRING_CONNECT	_T("                         ONLINE")
//#define STRING_DISCONNECT	_T("                         OFFLINE")
#define STRING_CONNECT	_T("ONLINE")
#define STRING_DISCONNECT	_T("OFFLINE")
#define STRING_PROGRESS	_T("PROGRESS")
#define STRING_WARNING	_T("WARNING")
#define STRING_ERROR	_T("ERROR")
#define STRING_MASTER	_T("MASTER CONNECT")

CDlgMotionGateInform::CDlgMotionGateInform(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MOTION_INFORM, pParent)
{
	m_dwAddress = 0;
	m_bConnected = false;
	m_pDevice = NULL;
	m_bRun = false;
	m_bDisable = FALSE;
	//m_ColorStCount = SHOWSTATUS;
}

CDlgMotionGateInform::~CDlgMotionGateInform()
{
	delete m_tooltip;
}

void CDlgMotionGateInform::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_DEV0, m_MotionBox);
	//DDX_Control(pDX, IDC_MFCBUTTON_BACKUP, m_btnBackup);
	DDX_Control(pDX, IDC_PROGRESS_DEVICE, m_Progress);
}


BEGIN_MESSAGE_MAP(CDlgMotionGateInform, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_STN_CLICKED(IDC_STATIC_TOGGLE_BTN, &CDlgMotionGateInform::OnStnClickedStaticToggle)
	ON_BN_CLICKED(IDC_MFCBUTTON_ALARM_RST, &CDlgMotionGateInform::OnBnClickedMfcbuttonAlarmRst)
	ON_BN_CLICKED(IDC_BUTTON_MOTOR_RESET, &CDlgMotionGateInform::OnBnClickedMfcbuttonMotorReset)
	ON_BN_CLICKED(IDC_BUTTON_BACKUP, &CDlgMotionGateInform::OnBnClickedMfcbuttonBackup)
	ON_BN_CLICKED(IDC_BUTTON_RESTORE, &CDlgMotionGateInform::OnBnClickedMfcbuttonRestore)
	ON_BN_CLICKED(IDC_BUTTON_COMPARE, &CDlgMotionGateInform::OnBnClickedMfcbuttonCompare)
	ON_BN_CLICKED(IDC_BUTTON_CONFIG, &CDlgMotionGateInform::OnBnClickedMfcbuttonConfig)
	ON_BN_CLICKED(IDC_BUTTON_BACKUP_SCHEDULE, &CDlgMotionGateInform::OnBnClickedMfcbuttonBackupSchedule)
	ON_BN_CLICKED(IDC_MFCBUTTON_MG_RESET, &CDlgMotionGateInform::OnBnClickedMfcbuttonMgReset)
	ON_BN_CLICKED(IDC_MFCBUTTON_REBOOT, &CDlgMotionGateInform::OnBnClickedMfcbuttonReboot)
	ON_BN_CLICKED(IDC_BUTTON_ASSIGN, &CDlgMotionGateInform::OnBnClickedMfcbuttonAssign)
	ON_BN_CLICKED(IDC_BUTTON_BACKUP_MANAGE, &CDlgMotionGateInform::OnBnClickedMfcbuttonBackupManage)
	ON_BN_CLICKED(IDC_BUTTON_RUN, &CDlgMotionGateInform::OnBnClickedMfcbuttonRun)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CDlgMotionGateInform::OnBnClickedMfcbuttonStop)
	ON_BN_CLICKED(IDC_CHECK_SERVO_CONTROL, &CDlgMotionGateInform::OnBnClickedCheckServoControl)
	ON_BN_CLICKED(IDC_BUTTON_HOMING, &CDlgMotionGateInform::OnBnClickedButtonHoming)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CDlgMotionGateInform 메시지 처리기


void CDlgMotionGateInform::ShowMotionInform()
{
	CString str;
	str.Format(L"MotionGate #%d", m_nID);
	//((CStatic*)GetDlgItem(IDC_STATIC_GROUP))->SetWindowTextW(str);

	for (int i = 0; i < MGSTATE_MAX; i++)
	{
		switch (i)
		{
		case MGSTATE_IP:
			str = m_mgInform.strIP;
			break;
		case MGSTATE_NAME:
			str = m_mgInform.strSystemName;
			break;
		case MGSTATE_STATE:
			str = m_mgInform.strStatus;
			break;
		case MGSTATE_MOTOR_COUNT:
			str.Format(L"%d", m_mgInform.nMotorCnt);
			break;
		case MGSTATE_CONNECT:
			str.Format(L"%s", ShowOpCode(m_mgInform.nOpCode));
			break;
		/*case MGSTATE_BOXCONTROL:
			str.Format(L"");
			break;*/
		default:
			break;
		}
		
		((CStatic*)GetDlgItem(IDC_STATIC_DEV0 + i))->SetWindowTextW(str);
	}
	//((CStatic*)GetDlgItem(IDC_STATIC_GROUP))->SetWindowTextW(L"MotionGate");
	// TODO: 여기에 구현 코드 추가.
}


CString CDlgMotionGateInform::ShowOpCode(int nOpCode)
{
	CString str;

	switch (nOpCode)
	{
	case opCode_stop :
		str.Format(L"Device Stop");
		break;
	case opCode_connect:
		str.Format(L"Device Connect");
		break;
	case opCode_disconenct:
		str.Format(L"Device DisConnect");
		break;
	case opCode_no_response:
		str.Format(L"Device No Response");
		break;
	case opCode_MotiongateBusy:
		str.Format(L"Device Busy");
		break;
	case opCode_scan_device:
		str.Format(L"Device Scan");
		break;
	case opCode_read_motiongate_status:
		str.Format(L"Read MotionGate");
		break;
	case opCode_standby_motiongate:
		str.Format(L"Stanby MotionGate");
		break;
	case opCode_backup_read_parameter:
		str.Format(L"Read Parameter [Backup]");
		break;
	case opCode_backup_read_position_table:
		str.Format(L"Read PositionTable [Backup]");
		break;
	case opCode_enter_monitor_mode:
		str.Format(L"Enter Monitor Mode");
		break;
	case opCode_exit_monitor_mode:
		str.Format(L"Exit Monitor Mode");
		break;
	case opCode_check_monitor_mode_on:
		str.Format(L"Monitor Mode On");
		break;
	case opCode_check_monitor_mode_off:
		str.Format(L"Monitor Mode Off");
		break;
	case opCode_check_device_name:
		str.Format(L"Check Device Name");
		break;
	//case opCode_change_device_name:
	//	str.Format(L"Change Device Name");
	//	break;
	//case opCode_change_register_name:
	//	str.Format(L"Change Register Name");
	//	break;
	case opCode_write_main_parameter:
		str.Format(L"Write Parameter");
		break;
	case opCode_write_position_table:
		str.Format(L"Write Position Table");
		break;

	default:
		str.Format(L"");
		break;
	}
	
	return str;
}


HBRUSH CDlgMotionGateInform::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	// SetWindowTextW 호출시 이벤트 발생!
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	
	//if (bFocus)
	//{
	//	((CStatic*)GetDlgItem(IDC_STATC_BACKGROUND))->ShowWindow(SW_SHOW);
	//	m_ColorFocus.SetBkColor(COLOR_BLUE);
	//}
	//else
	//{
	//	((CStatic*)GetDlgItem(IDC_STATC_BACKGROUND))->ShowWindow(SW_HIDE);
	//	//m_ColorFocus.SetBkColor(COLOR_NONE);
	//}
	
	//if (m_mgInform.bFocus)
	//{
	//	//m_ColorFocus.ShowWindow(SW_SHOW);
	//	//m_ColorFocus.SetBkColor(COLOR_BLUE);
	//}
	//else
	//{
	//	//m_ColorFocus.SetBkColor(COLOR_NONE);
	//	//m_ColorFocus.ShowWindow(SW_HIDE);
	//}
	//bool bDrawText = false;
	//int nCtrlID = pWnd->GetDlgCtrlID() - IDC_STATIC_DEV0;
	//switch (nCtrlID)
	//{
	//case MGSTATE_IP: 
	//case MGSTATE_STATE:
	//case MGSTATE_NAME:
	//case MGSTATE_BOXCONTROL:
	//case MGSTATE_MOTOR_COUNT:
	//{
	//	bDrawText = true;
	//}
	//default:
	//	break;
	//}
	//if (bDrawText)
	//{
	//	if (m_bConnected)
	//	{
	//		pDC->SetTextColor(RGB(75, 137, 220)); // Blue
	//		pDC->SetTextColor(RGB(0, 0, 0));
	//	}
	//	else
	//	{
	//		pDC->SetTextColor(RGB(0, 0, 0));
	//	}
	//}
	
	
	return hbr;
}


int CDlgMotionGateInform::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	m_tooltip.Create(this);

	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}


BOOL CDlgMotionGateInform::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	CString strMotorNum;
	
	HBITMAP hBitImg = NULL;
	
	if (1)
	{
		m_ColorStatic[STATUS_CONNECTED].SubclassDlgItem(IDC_ST_CONNECT, this);
		m_ColorStatic[STATUS_BUSY].SubclassDlgItem(IDC_ST_BUSY, this);
		m_ColorStatic[STATUS_WARNNING].SubclassDlgItem(IDC_ST_WARNING, this);
		m_ColorStatic[STATUS_ERROR].SubclassDlgItem(IDC_ST_ERROR, this);
		m_ColorStatic[STATUS_MASTER].SubclassDlgItem(IDC_ST_MASTER_CONNECT, this);
	}
	
	

	for (INT nMotor = MOTOR0; nMotor < MOTOR_MAX; nMotor++)
	{
		strMotorNum.Format(_T(" %02d"), nMotor);

		m_MotorConnectColorStatic[nMotor].SubclassDlgItem(IDC_STATIC_MOTOR0_STATUS + nMotor, this);
		m_MotorAlarmColorStatic[nMotor].SubclassDlgItem(IDC_STATIC_MOTOR0_ALARM + nMotor, this);
		m_MotorPTColorStatic[nMotor].SubclassDlgItem(IDC_STATIC_MOTOR0_PT + nMotor, this);
		m_MotorPTLifeColorStatic[nMotor].SubclassDlgItem(IDC_STATIC_MOTOR0_PT_LIFE + nMotor, this);
		m_MotorReadyColorStatic[nMotor].SubclassDlgItem(IDC_STATIC_MOTOR0_READY + nMotor, this);

		m_MotorConnectColorStatic[nMotor].SetWindowText(_T(" "));
		m_MotorAlarmColorStatic[nMotor].SetWindowText(_T(" "));
		m_MotorPTColorStatic[nMotor].SetWindowText(_T(" "));
		m_MotorPTLifeColorStatic[nMotor].SetWindowText(_T(" "));
		m_MotorReadyColorStatic[nMotor].SetWindowText(_T(""));
	}


	InitButtonSetting();
	

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


bool CDlgMotionGateInform::ShowMotionStLed(bool bConnected, bool bBusy, bool bWarning, bool bError, bool bMaster, bool bFocus)
{
	CStatic* pStaticBk = (CStatic*)GetDlgItem(IDC_STATC_BACKGROUND);

	if (bConnected)
	{
		m_ColorStatic[STATUS_CONNECTED].SetWindowTextW(STRING_CONNECT);
		m_ColorStatic[STATUS_CONNECTED].SetTextColor(COLOR_WHITE);
		m_ColorStatic[STATUS_CONNECTED].SetBkColor(COLOR_CONNECT);
		m_ColorStatic[STATUS_CONNECTED].EnableWindow(TRUE);
	}
	else
	{
		m_ColorStatic[STATUS_CONNECTED].SetWindowTextW(STRING_DISCONNECT);
		m_ColorStatic[STATUS_CONNECTED].SetTextColor(COLOR_WHITE);
		m_ColorStatic[STATUS_CONNECTED].SetBkColor(COLOR_RED);
		m_ColorStatic[STATUS_CONNECTED].EnableWindow(TRUE);
	}

	if (bBusy)
	{
		m_ColorStatic[STATUS_BUSY].SetWindowTextW(STRING_PROGRESS);
		m_ColorStatic[STATUS_BUSY].SetTextColor(COLOR_WHITE);
		m_ColorStatic[STATUS_BUSY].SetBkColor(COLOR_BUSY);
		m_ColorStatic[STATUS_BUSY].EnableWindow(TRUE);
	}
	else
	{
		m_ColorStatic[STATUS_BUSY].SetWindowTextW(STRING_PROGRESS);
		m_ColorStatic[STATUS_BUSY].SetBkColor(COLOR_NONE);
		m_ColorStatic[STATUS_BUSY].EnableWindow(FALSE);
	}

	if (bWarning)
	{
		m_ColorStatic[STATUS_WARNNING].SetBkColor(COLOR_WARNING);
		m_ColorStatic[STATUS_WARNNING].SetWindowTextW(STRING_WARNING);
		m_ColorStatic[STATUS_WARNNING].SetTextColor(COLOR_WHITE);
		m_ColorStatic[STATUS_WARNNING].EnableWindow(TRUE);
	}
	else
	{

		m_ColorStatic[STATUS_WARNNING].SetWindowTextW(STRING_WARNING);
		m_ColorStatic[STATUS_WARNNING].SetBkColor(COLOR_NONE);
		m_ColorStatic[STATUS_WARNNING].EnableWindow(FALSE);
	}

	if (bError)
	{
		m_ColorStatic[STATUS_ERROR].SetBkColor(COLOR_DARKRED);
		m_ColorStatic[STATUS_ERROR].SetWindowTextW(STRING_ERROR);
		m_ColorStatic[STATUS_ERROR].SetTextColor(COLOR_WHITE);
		m_ColorStatic[STATUS_ERROR].EnableWindow(TRUE);
	}
	else
	{
		m_ColorStatic[STATUS_ERROR].SetWindowTextW(STRING_ERROR);
		m_ColorStatic[STATUS_ERROR].SetBkColor(COLOR_NONE);
		m_ColorStatic[STATUS_ERROR].EnableWindow(FALSE);
	}

	if (bMaster)
	{
		m_ColorStatic[STATUS_MASTER].SetBkColor(COLOR_ORANGE);
		m_ColorStatic[STATUS_MASTER].SetWindowTextW(STRING_MASTER);
		m_ColorStatic[STATUS_MASTER].SetTextColor(COLOR_WHITE);
		m_ColorStatic[STATUS_MASTER].EnableWindow(TRUE);
	}
	else
	{
		m_ColorStatic[STATUS_MASTER].SetWindowTextW(STRING_MASTER);
		m_ColorStatic[STATUS_MASTER].SetBkColor(COLOR_NONE);
		m_ColorStatic[STATUS_MASTER].EnableWindow(FALSE);
	}


	if (bFocus)
	{
		pStaticBk->ShowWindow(SW_HIDE);

		/*m_ColorFocus.SetBkColor(COLOR_BLUE);
		pStaticBk->ShowWindow(SW_SHOW);*/
	}
	else
	{
		pStaticBk->ShowWindow(SW_HIDE);
	}



	return false;
}


bool CDlgMotionGateInform::SetFocus(bool bChanged)
{
	if (bChanged)
	{
		((CStatic*)GetDlgItem(IDC_STATC_BACKGROUND))->ShowWindow(SW_SHOW);
		m_ColorFocus.SetBkColor(COLOR_BLUE);

	}
	else
	{
		//m_ColorFocus.SetBkColor(COLOR_NONE);
		((CStatic*)GetDlgItem(IDC_STATC_BACKGROUND))->ShowWindow(SW_HIDE);
		
	}

	return false;
}

/*
BOOL CDlgMotionGateInform::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UINT id = (UINT)LOWORD(wParam);

	switch (id)
	{
	case IDC_MFCBUTTON_BACKUP:
	{
		SendCmdMsgToMainFrame(ID_INFORM_BACKUP);
	} break;

	case IDC_MFCBUTTON_RESTORE:
	{
		SendCmdMsgToMainFrame(ID_INFORM_RESTORATION);
	}	break;

	case IDC_MFCBUTTON_COMPARE:
	{
		SendCmdMsgToMainFrame(ID_INFORM_COMPARE);
	}	break;

	case IDC_MFCBUTTON_CONFIG:
	{
		SendCmdMsgToMainFrame(ID_INFORM_CONFIG);
	}	break;


	case IDC_MFCBUTTON_BACKUP_SCHEDULE:
	{
		SendCmdMsgToMainFrame(ID_INFORM_BACKUP_SCHEDULE);
	}	break;

	//case IDC_MFCBUTTON_MG_RESET:
	//{
	//	SendCmdMsgToMainFrame(ID_INFORM_FACTORY_RESET);
	//}	break;

	case IDC_MFCBUTTON_REBOOT:
	{
		SendCmdMsgToMainFrame(ID_INFORM_REBOOT);
	}	break;

	case IDC_MFCBUTTON_MOTOR_RESET:
	{
		SendCmdMsgToMainFrame(ID_INFORM_MOTOR_RESET);
	}	break;

	case IDC_MFCBUTTON_ASSIGN:
	{
		SendCmdMsgToMainFrame(ID_INFORM_ASSIGN);
	}	break;

	case IDC_MFCBUTTON_BACKUP_MANAGE:
	{
		SendCmdMsgToMainFrame(ID_INFORM_BACKUP_MANAGE);
	}	break;

	case IDC_MFCBUTTON_ALARM_RST:
	{
		SendCmdMsgToMainFrame(ID_INFORM_ALARM_RST);
		HomingTest();
	}	break;

	case IDC_MFCBUTTON_RUN:
	{
		DoDeviceMonitoring(TRUE);
	}	break;

	case IDC_MFCBUTTON_STOP:
	{
		DoDeviceMonitoring(FALSE);
	}	break;

	case IDC_STATIC_TOGGLE_BTN:
	{
		SendCmdMsgToMainFrame(ID_INFORM_BACKUP_TOGGLE);
	}	break;

	case IDC_CHECK_SERVO_CONTROL:
	{
		BOOL bCheck = ((CButton*)GetDlgItem(id))->GetCheck();
		ServoControl(bCheck);
	}	break;


	default:
		break;
	}

	((CMainFrame*)AfxGetMainWnd())->SetSelectedAddress(m_dwAddress);
	return TRUE;
}
*/



void CDlgMotionGateInform::DoDeviceMonitoring(BOOL bRun)
{
	if (CheckControlGranted())
	{
		if (bRun)
		{
			CDeviceList* pList = CDeviceList::GetInstance();
			if (pList->IsMonitoringEnabled())
			{
				m_pDevice->StartThread();
			}

			else
			{
				if (AskSystemRunState())
				{
					SendCmdMsgToMainFrame(ID_INFORM_SYSTEM_RUN);
				}
			}

		}
		else
		{
			m_pDevice->StopExecution();
			m_pDevice->StopSequence();
			m_pDevice->StopThread();
			m_pDevice->CmdConnectionClose();
		}
	}
	
}

void CDlgMotionGateInform::SetBackupToggle(BOOL bEnable)
{ // m_bBackupToggle 값이 bEnable 으로 들어온다.
	CImage Image;
	CBitmap bmp;
	HBITMAP hBit;
	HRESULT hRes;
	//CMFCButton* pButton;
	CBitmapButton* pButton;
	CString strStatus, strCompare;

	BOOL bCheck = m_pDevice->IsAutoBackupScheduleExist();

	strStatus = bEnable ? _T("ON") : _T("OFF");
	GetDlgItemText(IDC_STATIC_BACKUP_BUTTON, strCompare);
	if (strStatus != strCompare)
	{
		CString str = strStatus;
	}

	//if (strStatus == strCompare) return; // 이전 상태와 비교하여 Blinking 현상 방지
	SetDlgItemText(IDC_STATIC_BACKUP_BUTTON, strStatus);
	if (m_pDevice != NULL)
	{
		if (!bEnable)
		{
			INT a = 0;
		}
		//m_pDevice->SetAutoBackupReservation(bEnable);
	}
	

	CRect rect;
	CImage image;
	CDC* dc;
	CStatic* pToggleBtn = (CStatic*)GetDlgItem(IDC_STATIC_TOGGLE_BTN);
	dc = pToggleBtn->GetDC();
	pToggleBtn->GetWindowRect(rect);
	image.Load(bEnable ? L"res/TotalConfig/switch_on.bmp" : L"res/TotalConfig/switch_off.bmp");
	image.StretchBlt(dc->m_hDC, 0, 0, rect.Width(), rect.Height(), SRCCOPY);
	ReleaseDC(dc);
}

void CDlgMotionGateInform::InitButtonImage()
{
	CImage Image;
	HBITMAP hBit;
	HRESULT hRes;
	static int nCnt = 0;

	//CMFCButton* pButton; 
	CMFCButtonEx* pButton;
	
	if (1)
	{ // Sevo On Test용
		INT nShow = SW_HIDE;
		//(GetDlgItem(IDC_CHECK_SERVO_CONTROL))->ShowWindow(SW_SHOW);
		(GetDlgItem(IDC_MFCBUTTON_REBOOT))->ShowWindow(nShow);
		(GetDlgItem(IDC_STATIC_CMD_RESULT))->ShowWindow(nShow);
	}

	if (1)
	{
		hRes = Image.Load(L"res/TotalConfig/backup.png");
		if (hRes != S_OK)
		{
			ASSERT(0);
		} 
		hBit = Image.Detach();
		
		pButton = (CMFCButtonEx*)GetDlgItem(IDC_MFCBUTTON_BACKUP);
		pButton->m_nFlatStyle = CMFCButtonEx::BUTTONSTYLE_3D;
		pButton->m_bTransparent = TRUE;
		pButton->m_bDrawFocus = FALSE;
		pButton->SetImage(hBit, TRUE);
		//pButton->m_bFontChange = FALSE;
		pButton->SetWindowText(L"Backup");
	}

	if (1)
	{
		hRes = Image.Load(L"res/TotalConfig/Restore.png");
		if (hRes != S_OK)
		{
			ASSERT(0);
		}
		hBit = Image.Detach();

		pButton = (CMFCButtonEx*)GetDlgItem(IDC_MFCBUTTON_RESTORE);
		pButton->m_nFlatStyle = CMFCButtonEx::BUTTONSTYLE_3D;
		pButton->m_bTransparent = TRUE;
		pButton->m_bDrawFocus = FALSE;

		pButton->SetImage(hBit, TRUE);
		pButton->SetWindowText(L"Restore");
	}
	
	if (1)
	{
		hRes = Image.Load(L"res/TotalConfig/Compare.png");
		if (hRes != S_OK)
		{
			ASSERT(0);
		}
		hBit = Image.Detach();

		pButton = (CMFCButtonEx*)GetDlgItem(IDC_MFCBUTTON_COMPARE);
		pButton->m_nFlatStyle = CMFCButtonEx::BUTTONSTYLE_3D;
		pButton->m_bTransparent = TRUE;
		pButton->m_bDrawFocus = FALSE;

		pButton->SetImage(hBit, TRUE);
		pButton->SetWindowText(L"Compare");
	}

	if (true)
	{
		hRes = Image.Load(L"res/TotalConfig/ID Assign_Ex.png");
		if (hRes != S_OK)
		{
			ASSERT(0);
		}
		hBit = Image.Detach();

		pButton = (CMFCButtonEx*)GetDlgItem(IDC_MFCBUTTON_ASSIGN);
		pButton->m_nFlatStyle = CMFCButtonEx::BUTTONSTYLE_3D;
		pButton->m_bTransparent = TRUE;
		pButton->m_bDrawFocus = FALSE;

		pButton->SetImage(hBit, TRUE);
		pButton->SetWindowText(L"ID Assign");
	}

	if (1)
	{
		/*CFont m_BigFont;
		m_BigFont.CreateFont(50, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, TEXT_STYLE);*/
		
		hRes = Image.Load(L"res/TotalConfig/Reset.png");
		if (hRes != S_OK)
		{
			ASSERT(0);
		}
		hBit = Image.Detach();

		pButton = (CMFCButtonEx*)GetDlgItem(IDC_MFCBUTTON_MOTOR_RESET);
		pButton->m_nFlatStyle = CMFCButtonEx::BUTTONSTYLE_3D;
		pButton->m_bTransparent = TRUE;
		pButton->m_bDrawFocus = FALSE;

		pButton->SetImage(hBit, TRUE);
		pButton->SetWindowText(L" Factory Reset");
		pButton->SetFont(&m_BigFont, 1);
	}

	if (1)
	{
		hRes = Image.Load(L"res/TotalConfig/Reset.png");
		if (hRes != S_OK)
		{
			ASSERT(0);
		}
		hBit = Image.Detach();

		pButton = (CMFCButtonEx*)GetDlgItem(IDC_MFCBUTTON_MG_RESET);
		pButton->m_nFlatStyle = CMFCButtonEx::BUTTONSTYLE_3D;
		pButton->m_bTransparent = TRUE;
		pButton->m_bDrawFocus = FALSE;

		pButton->SetImage(hBit, TRUE);
		pButton->SetWindowText(L" Reset");
	}

	if (1)
	{
		hRes = Image.Load(L"res/TotalConfig/reboot.png");
		if (hRes != S_OK)
		{
			ASSERT(0);
		}
		hBit = Image.Detach();

		pButton = (CMFCButtonEx*)GetDlgItem(IDC_MFCBUTTON_REBOOT);
		pButton->m_nFlatStyle = CMFCButtonEx::BUTTONSTYLE_3D;
		pButton->m_bTransparent = TRUE;
		pButton->m_bDrawFocus = FALSE;

		pButton->SetImage(hBit, TRUE);
		pButton->SetWindowText(L" Reboot");
	}

	if (1)
	{
		hRes = Image.Load(L"res/TotalConfig/Config.png");
		if (hRes != S_OK)
		{
			ASSERT(0);
		}
		hBit = Image.Detach();

		pButton = (CMFCButtonEx*)GetDlgItem(IDC_MFCBUTTON_CONFIG);
		pButton->m_nFlatStyle = CMFCButtonEx::BUTTONSTYLE_3D;
		pButton->m_bTransparent = TRUE;
		pButton->m_bDrawFocus = FALSE;

		pButton->SetImage(hBit, TRUE);
		pButton->SetWindowText(L"Configuration");
	}

	if (1)
	{
		hRes = Image.Load(L"res/TotalConfig/Schedule.png");
		if (hRes != S_OK)
		{
			ASSERT(0);
		}
		hBit = Image.Detach();

		pButton = (CMFCButtonEx*)GetDlgItem(IDC_MFCBUTTON_BACKUP_SCHEDULE);
		pButton->m_nFlatStyle = CMFCButtonEx::BUTTONSTYLE_3D;
		pButton->m_bTransparent = TRUE;
		pButton->m_bDrawFocus = FALSE;

		pButton->SetImage(hBit, TRUE);
		pButton->SetWindowText(L"Backup Schedule");
	}

	if (1)
	{
		hRes = Image.Load(L"res/TotalConfig/BackupManage.png");
		if (hRes != S_OK)
		{
			ASSERT(0);
		}
		hBit = Image.Detach();

		pButton = (CMFCButtonEx*)GetDlgItem(IDC_MFCBUTTON_BACKUP_MANAGE);
		pButton->m_nFlatStyle = CMFCButtonEx::BUTTONSTYLE_3D;
		pButton->m_bTransparent = TRUE;
		pButton->m_bDrawFocus = FALSE;

		pButton->SetImage(hBit, TRUE);
		pButton->SetWindowText(L"Backup Manage");
	}

	if (1)
	{
		hRes = Image.Load(L"res/TotalConfig/Run.png");
		if (hRes != S_OK)
		{
			ASSERT(0);
		}
		hBit = Image.Detach();

		pButton = (CMFCButtonEx*)GetDlgItem(IDC_MFCBUTTON_RUN);
		pButton->m_nFlatStyle = CMFCButtonEx::BUTTONSTYLE_3D;
		pButton->m_bTransparent = TRUE;
		pButton->m_bDrawFocus = FALSE;

		pButton->SetImage(hBit, TRUE);
		pButton->SetWindowText(L"Run");
	}

	if (1)
	{
		hRes = Image.Load(L"res/TotalConfig/Stop.png");
		if (hRes != S_OK)
		{
			ASSERT(0);
		}
		hBit = Image.Detach();

		pButton = (CMFCButtonEx*)GetDlgItem(IDC_MFCBUTTON_STOP);
		pButton->m_nFlatStyle = CMFCButtonEx::BUTTONSTYLE_3D;
		pButton->m_bTransparent = TRUE;
		pButton->m_bDrawFocus = FALSE;

		pButton->SetImage(hBit, TRUE);
		pButton->SetWindowText(L"Stop");
	}

	if (1)
	{
		hRes = Image.Load(L"res/TotalConfig/Stop.png");
		if (hRes != S_OK)
		{
			ASSERT(0);
		}
		hBit = Image.Detach();

		pButton = (CMFCButtonEx*)GetDlgItem(IDC_MFCBUTTON_STOP);
		pButton->m_nFlatStyle = CMFCButtonEx::BUTTONSTYLE_3D;
		pButton->m_bTransparent = TRUE;
		pButton->m_bDrawFocus = FALSE;

		pButton->SetImage(hBit, TRUE);
		pButton->SetWindowText(L"Stop");
	}

	if (1)
	{
		//SetBackupToggle(FALSE);
	}

	if (1)
	{
		m_wndViewer.SubclassDlgItem(IDC_ST_IMAGE, this);
	}

	if (1)
	{
		BOOL bEnable = FALSE;
		GetDlgItem(IDC_MFCBUTTON_BACKUP)->EnableWindow(bEnable);
		GetDlgItem(IDC_MFCBUTTON_RESTORE)->EnableWindow(bEnable);
		GetDlgItem(IDC_MFCBUTTON_COMPARE)->EnableWindow(bEnable);
		GetDlgItem(IDC_MFCBUTTON_CONFIG)->EnableWindow(bEnable);
		GetDlgItem(IDC_MFCBUTTON_BACKUP_SCHEDULE)->EnableWindow(bEnable);
		GetDlgItem(IDC_MFCBUTTON_MG_RESET)->EnableWindow(bEnable);
		GetDlgItem(IDC_MFCBUTTON_REBOOT)->EnableWindow(bEnable);
		GetDlgItem(IDC_MFCBUTTON_MOTOR_RESET)->EnableWindow(bEnable);
		GetDlgItem(IDC_MFCBUTTON_ASSIGN)->EnableWindow(bEnable);
		GetDlgItem(IDC_MFCBUTTON_BACKUP_MANAGE)->EnableWindow(bEnable);
		GetDlgItem(IDC_MFCBUTTON_RUN)->EnableWindow(bEnable);
		GetDlgItem(IDC_MFCBUTTON_STOP)->EnableWindow(bEnable);
		//GetDlgItem(IDC_CHECK_SERVO_CONTROL)->EnableWindow(bEnable);
	}
}


void CDlgMotionGateInform::OnLButtonDown(UINT nFlags, CPoint point)
{
	((CMainFrame*)AfxGetMainWnd())->SetSelectedAddress(m_dwAddress);
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CDlgMotionGateInform::SetAddress(DWORD dwAddress)
{
	m_dwAddress = dwAddress;
}


DWORD CDlgMotionGateInform::GetAddress()
{
	return m_dwAddress;
}


void CDlgMotionGateInform::SendCmdMsgToMainFrame(WPARAM wParam, LPARAM lParam)
{
	AfxGetMainWnd()->SendMessage(WM_COMMAND, wParam, lParam);
}


void CDlgMotionGateInform::SetProgressBar(INT nNewPos, INT nNewMin, INT nNewMax)
{
	m_Progress.SetRange(nNewMin, nNewMax);
	m_Progress.SetPos(nNewPos);
}


void CDlgMotionGateInform::SetProgressStatus(CString strStatus)
{
	(GetDlgItem(IDC_STATIC_DEV5))->SetWindowText(strStatus);
}


const BOOL CDlgMotionGateInform::SetDevice(CDeviceMotionGate* pDevice)
{
	m_pDevice = pDevice;

	if (NULL == m_pDevice)
	{
		ASSERT(0);
		return false;
	}

	return true;
}


void CDlgMotionGateInform::ShowButton(CDeviceMotionGate* pDevice)
{
//if (pDevice->IsMotiongateConnected())
if (pDevice)
{
	CButton* pButton;
	BOOL bEnable = FALSE;
	BOOL bMaster = pDevice->GetMasterConnectStatus();

	CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();
	AUTHORIZE_CONTROL_INFO authorize = pConfigDB->GetAuthorizeState();
	
	BOOL bAdmin = pConfigDB->IsAdministrator();
	BOOL bEngineer = pConfigDB->IsEngineer();
	BOOL bOperator = pConfigDB->IsOperator();
	BOOL bDefault = pConfigDB->IsDefaultOperator();
	
	BOOL bGrantBackup = pConfigDB->IsDeviceBackupGranted();
	BOOL bGrantBackupManage = pConfigDB->IsDeviceBackupManageGranted();
	BOOL bGrantRestore = pConfigDB->IsDeviceRestorationGranted();
	BOOL bGrantCompare = pConfigDB->IsDeviceCompareGranted();
	BOOL bGrantConfig = pConfigDB->IsDeviceConfigGranted();
	BOOL bGrantBackupSchedule = pConfigDB->IsDeviceBackupScheduleGranted();
	BOOL bGrantReset = pConfigDB->IsDeviceFactoryResetGranted();
	BOOL bGrantAssign = pConfigDB->IsDeviceIdAssignmentGranted();
	BOOL bGrantIdReset = pConfigDB->IsDeviceIdResetGranted();
	

	BOOL bGrantControl = pConfigDB->IsControlGranted();
	BOOL bIsMotorExist = pDevice->IsMotionGateMotorExist();
	BOOL bErrorDetected = pDevice->IsSystemStatusError(); 
	BOOL bBusy = pDevice->IsBusy();

	BOOL bAnyOne	   = bAdmin || bEngineer || bOperator || bDefault;
	BOOL bLowAuthority = bAdmin || bEngineer || bOperator;
	BOOL bMidAuthority = bAdmin || bEngineer;
	BOOL bHighAuthority = bAdmin;
	

	if (1) // Run
	{
		pButton = (CButton*)GetDlgItem(IDC_BUTTON_RUN);
		bEnable = pDevice->IsRun() && bAnyOne;
		pButton->EnableWindow(!bEnable);
		
	}

	if (1) // Stop
	{
		pButton = (CButton*)GetDlgItem(IDC_BUTTON_STOP);
		bEnable = pDevice->IsRun() && bAnyOne;
		pButton->EnableWindow(bEnable);
	}

	if (1) // Backup
	{
		pButton = (CButton*)GetDlgItem(IDC_BUTTON_BACKUP);
		bEnable = FALSE;
		if (pDevice->IsMotiongateControllable(FALSE))
		{
			if (pDevice->IsMotorConnectionContinuity())
			{
				if (bGrantBackup && bIsMotorExist)
				{
					bEnable = TRUE;
				}
			}
		}

		if (FALSE == bIsMotorExist ||
			//TRUE == bErrorDetected ||
			TRUE == bBusy)
		{
			bEnable = FALSE;
		}

		pButton->EnableWindow(bEnable);
	}

	if (1) // Backup_Manage
	{
		bEnable = FALSE;
		pButton = (CButton*)GetDlgItem(IDC_BUTTON_BACKUP_MANAGE);
		// 기존.. bEnable = ((pDevice->IsMotiongateControllable() && pDevice->IsMotorConnectionContinuity() && (bMidAuthority || bMaster))/*Run 상태에서의 조건*/ || (pDevice->GetMotionGateConnectedStatus() && bMaster) /*Stop 상태에서의 조건*/);
		if (pDevice->IsMotiongateControllable())
		{
			if (pDevice->IsMotorConnectionContinuity())
			{
				if (/*bMidAuthority*/bGrantBackupManage || bMaster)
				{
					bEnable = TRUE;
				}
			}
		}

		else
		{
			if (pDevice->GetMotionGateConnectedStatus() && bMaster)
			{
				bEnable = TRUE;
			}
		}

		if (FALSE == bIsMotorExist ||
			//TRUE == bErrorDetected ||
			TRUE == bBusy)
		{
			bEnable = FALSE;
		}

		pButton->EnableWindow(bEnable);
	}

	if (1) // Restore
	{
		pButton = (CButton*)GetDlgItem(IDC_BUTTON_RESTORE);
		//bEnable = (pDevice->IsMotiongateControllable() && pDevice->IsMotorConnectionContinuity() && /*bMidAuthority*/ bGrantRestore);
		bEnable = FALSE;
		if (pDevice->IsMotiongateControllable())
		{
			if (pDevice->IsMotorConnectionContinuity())
			{
				if (bGrantRestore && bIsMotorExist)
				{
					bEnable = TRUE;
				}
			}
		}

		if (FALSE == bIsMotorExist ||
			//TRUE == bErrorDetected ||
			TRUE == bBusy)
		{
			bEnable = FALSE;
		}

		pButton->EnableWindow(bEnable);
	}

	if (1) // Compare
	{
		pButton = (CButton*)GetDlgItem(IDC_BUTTON_COMPARE);
		//bEnable = pDevice->IsMotiongateControllable();
		bEnable = FALSE;

		// 기존..
		//bEnable = ((pDevice->IsMotiongateControllable() && pDevice->IsMotorConnectionContinuity() && (bMidAuthority || bMaster))/*Run 상태에서의 조건*/ || (pDevice->GetMotionGateConnectedStatus() && bMaster) /*Stop 상태에서의 조건*/);
		
		if (pDevice->IsMotiongateControllable())
		{  
			if (pDevice->IsMotorConnectionContinuity())
			{
				if (/*bMidAuthority*/bGrantCompare || bMaster)
				{
					bEnable = TRUE;
				}
			}
		}

		else
		{ 
			if (pDevice->GetMotionGateConnectedStatus() && bMaster)
			{
				bEnable = TRUE;
			}
		}
		

		if (FALSE == bIsMotorExist ||
			//TRUE == bErrorDetected ||
			TRUE == bBusy)
		{
			bEnable = FALSE;
		}

		pButton->EnableWindow(bEnable);
	}

	if (1) // Config
	{
		pButton = (CButton*)GetDlgItem(IDC_BUTTON_CONFIG);
		bEnable = FALSE;
		if (pDevice->IsMotiongateControllable(FALSE, FALSE))
		{
			if (bGrantConfig)
			{
				if (!bMaster)
				{
					bEnable = TRUE;
				}
				
			}
		}

	/*	MotionGate Reset을 진행할 수 있기 때문에.. Setting 값은 열어놓는다./
		if (FALSE == bIsMotorExist)
		{ 
			bEnable = FALSE;
		}*/
		if (TRUE == bBusy)
		{
			bEnable = FALSE;
		}
		

		pButton->EnableWindow(bEnable);
	}

	if (1) // Backup Schedule
	{
		pButton = (CButton*)GetDlgItem(IDC_BUTTON_BACKUP_SCHEDULE);
		bEnable = FALSE;
		if (pDevice->IsMotiongateControllable(FALSE, FALSE))
		{
			if (bGrantBackupSchedule)
			{
				bEnable = TRUE;
			}
		}

		/*if (FALSE == bIsMotorExist)
		{
			bEnable = FALSE;
		}*/

		if (TRUE == bBusy)
		{
			bEnable = FALSE;
		}

		if (_T("") == static_cast<CString>(pDevice->GetSystemName()))
		{ // 이재우 차장 예외 처리 추가.. 모션게이트 이름이 존재하지 않을경우 스케쥴 진행X
			bEnable = FALSE;
		}

		pButton->EnableWindow(bEnable);
	}

	if (1) // ID Assign
	{
		pButton = (CButton*)GetDlgItem(IDC_BUTTON_ASSIGN);
		bEnable = FALSE;
		if (pDevice->IsMotiongateControllable())
		{
			if (pDevice->IsBareMotorConnected())
			{
				if (bGrantAssign)
				{
					bEnable = TRUE;
				}
			}
		}

		if (FALSE == bIsMotorExist)
		{
			bEnable = FALSE;
		}

		if (TRUE == bBusy)
		{
			bEnable = FALSE;
		}

		pButton->EnableWindow(bEnable);
	}

	if (1) // Motor Factory Reset
	{
		pButton = (CButton*)GetDlgItem(IDC_BUTTON_MOTOR_RESET);
		bEnable = FALSE;
		if (pDevice->IsMotiongateControllable())
		{
			if (FALSE == pDevice->IsBareMotorConnected())
			{
				if (bGrantIdReset)
				{
					bEnable = TRUE;
				}
			}
		}

		if (FALSE == bIsMotorExist)
		{
			bEnable = FALSE;
		}

		if (TRUE == bBusy)
		{
			bEnable = FALSE;
		}

		pButton->EnableWindow(bEnable);
	}

	if (1) // Reboot
	{ // 사용 안함..
		pButton = (CButton*)GetDlgItem(IDC_MFCBUTTON_REBOOT);
		bEnable = FALSE;
		if (pDevice->IsMotiongateControllable())
		{
			if (pDevice->IsMotorConnectionContinuity())
			{
				bEnable = TRUE;
			}
		}

		if (FALSE == bIsMotorExist ||
			TRUE == bErrorDetected)
		{
			bEnable = FALSE;
		}

		if (TRUE == bBusy)
		{
			bEnable = FALSE;
		}

		pButton->EnableWindow(bEnable);
	}

	if (1) // MotionGate Factory Reset
	{
		pButton = (CButton*)GetDlgItem(IDC_MFCBUTTON_MG_RESET);
		bEnable = FALSE;
		if (pDevice->IsMotiongateControllable())
		{
			if (FALSE == pDevice->IsBareMotorConnected())
			{
				if (bGrantReset)
				{
					bEnable = TRUE;
				}
			}
		}

		if (FALSE == bIsMotorExist)
		{
			bEnable = FALSE;
		}

		if (TRUE == bBusy)
		{
			bEnable = FALSE;
		}

		pButton->EnableWindow(bEnable);
	}

}

}


const BOOL CDlgMotionGateInform::AskSystemRunState()
{
	BOOL bResult = FALSE;

	CString message("");
	CString mainInstruction("If you want to proceed with the 'RUN' mode, Please system run first.");
	CString title(_T("Confirm to run system"));

	CTaskDialog taskDialog(message, mainInstruction, title, !(TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON));

	taskDialog.AddCommandControl(IDYES, _T("Yes, Proceed with the 'System Run'"));
	taskDialog.AddCommandControl(IDNO, _T("No, Cancel 'System Run'"));

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
	


const BOOL CDlgMotionGateInform::CheckControlGranted()
{
	const BOOL bCheck = ((CMainFrame*)AfxGetMainWnd())->CheckControlGranted();
	return bCheck;
}


void CDlgMotionGateInform::ShowProgressTime(DWORD dwTime)
{
	CString str;
	str.Format(_T("[ %d.%d Sec]"), dwTime / 10, dwTime % 10);
	((CStatic*)GetDlgItem(IDC_STATIC_DEV6))->SetWindowText(str);
}


void CDlgMotionGateInform::SetProgressEndTime(DWORD dwTime)
{
	// TODO: 여기에 구현 코드 추가.
}


void CDlgMotionGateInform::SetMotorErrorState(CString str)
{
	SetDlgItemText(IDC_STATIC_DEV7, str);
}

void CDlgMotionGateInform::SetZeroMotorStatus(CString strStatus)
{
	SetDlgItemText(IDC_STATIC_MOTOR0_INFORM, strStatus);
}

void CDlgMotionGateInform::MotorConnectionStatus(DWORD dwMotorConnection)
{
	CStatic* pStatic = NULL;
	BOOL bConnect = false;
	for (INT nIndex = MOTOR0; nIndex < MOTOR_MAX; nIndex++)
	{
		bConnect = (dwMotorConnection >> nIndex) & 0x01 ? TRUE : FALSE;
		if (1)
		{// Motor Connection
			if (nIndex == MOTOR0) GetDlgItem(IDC_STATIC_GROUP3)->EnableWindow(bConnect);
			
			m_MotorConnectColorStatic[nIndex].EnableWindow(bConnect);
			m_MotorConnectColorStatic[nIndex].SetTextColor(bConnect ? COLOR_BLACK : COLOR_NONE);
			m_MotorConnectColorStatic[nIndex].SetBkColor(bConnect ? COLOR_YELLOW : COLOR_NONE);
		}

		
		if (1)
		{ // Motor Error
			m_MotorAlarmColorStatic[nIndex].EnableWindow(bConnect);
			m_MotorAlarmColorStatic[nIndex].SetTextColor(bConnect ? COLOR_BLACK : COLOR_NONE);
			m_MotorAlarmColorStatic[nIndex].SetBkColor(bConnect ? COLOR_YELLOW : COLOR_NONE);
		}

		if (1)
		{ // Motor Style Count
			m_MotorPTColorStatic[nIndex].EnableWindow(bConnect);
			m_MotorPTColorStatic[nIndex].SetTextColor(bConnect ? COLOR_BLACK : COLOR_NONE);
			m_MotorPTColorStatic[nIndex].SetBkColor(bConnect ? COLOR_YELLOW : COLOR_NONE);
		}

		if (1)
		{ // Motor Style 
			m_MotorPTLifeColorStatic[nIndex].EnableWindow(bConnect);
			//m_MotorPTLifeColorStatic[nIndex].SetTextColor(bConnect ? COLOR_BLACK : COLOR_NONE);
			//m_MotorPTLifeColorStatic[nIndex].SetBkColor(bConnect ? COLOR_YELLOW : COLOR_NONE);
		}

		if (1)
		{ // Motor Ready
			m_MotorReadyColorStatic[nIndex].EnableWindow(bConnect);
			m_MotorReadyColorStatic[nIndex].SetTextColor(bConnect ? COLOR_BLACK : COLOR_NONE);
			m_MotorReadyColorStatic[nIndex].SetBkColor(bConnect ? COLOR_YELLOW : COLOR_NONE);
		}

		if (1)
		{
			(GetDlgItem(IDC_STATIC_TEXT_MOTOR0 + nIndex))->EnableWindow(bConnect ? TRUE : FALSE);
		}
	}
}



void CDlgMotionGateInform::MotorLifeCountStatus(DWORD dwMotorConnection)
{
	BOOL bConnect = false;
	BOOL bEnable = false;
	for (INT nIndex = MOTOR0; nIndex < MOTOR_MAX; nIndex++)
	{
		bConnect = (dwMotorConnection >> nIndex) & 0x01 ? TRUE : FALSE;
		if (bConnect)
		{
			m_MotorPTLifeColorStatic[nIndex].EnableWindow(bConnect);
			m_MotorPTLifeColorStatic[nIndex].SetTextColor(bConnect ? COLOR_BLACK : COLOR_NONE);
			m_MotorPTLifeColorStatic[nIndex].SetBkColor(bConnect ? COLOR_YELLOW : COLOR_NONE);
		}
	}
	
}


void CDlgMotionGateInform::MotorAlarmStatus(CDeviceMotionGate* pDevice)
{
	// Ready Status + Error Status
	DWORD	dwErrorCode;
	CString strErrorCode;
	COLORREF ckBkColor = COLOR_NONE;
	BOOL	bConnect = FALSE;
	BOOL	bReady = FALSE;
	BOOL	bBusy = FALSE;
	BOOL	bError = FALSE;

	for (INT nMotor = MOTOR0; nMotor < MOTOR_MAX; nMotor++)
	{
		bConnect = pDevice->IsMotorConnected(nMotor);

		if (bConnect)
		{
			dwErrorCode = pDevice->GetMotorErrorCode(nMotor);
			switch (dwErrorCode)
			{ // DoCmdGetMotiongateSystemStatus 에서 btErrorCode를 읽는다.
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
			case sts_error_motor_drive_rom_error: // 12
			case sts_error_motor_drive_voltage_error: // = 14
			case sts_error_motor_position_overflow:
			case sts_error_motor_position_change:
			case sts_error_motor_break_short:
			case sts_error_motor_struct_postion_change:
			case sts_error_motor_no_clam_load: // 20
			case sts_error_access_motor_not_exist:
			case sts_error_access_motor_out_of_range: // 모터 접근 범위 초과
			case sts_error_access_pt_not_exist:
			case sts_error_access_pt_out_of_range: // 24
			{
				strErrorCode.Format(_T(" %02d"), dwErrorCode);
				bError = TRUE;
				
				bBusy = FALSE;
				bReady = FALSE;
				ckBkColor = COLOR_NONE;

			}	break;
			default:
			{
				bError = FALSE;
				strErrorCode.Format(_T(""));

				bBusy = pDevice->IsMotorBusy(nMotor);
				bReady = !bBusy;

				if (bReady) ckBkColor = COLOR_GREEN;
				if (bBusy)	ckBkColor = COLOR_YELLOW;
			}	break;

			}

			m_MotorAlarmColorStatic[nMotor].SetTextColor(bError ? COLOR_NONE : COLOR_ORIGINAL_BLUE);
			m_MotorAlarmColorStatic[nMotor].SetBkColor(bError ? COLOR_RED : COLOR_NONE);
			m_MotorAlarmColorStatic[nMotor].SetWindowText(strErrorCode);

			m_MotorReadyColorStatic[nMotor].SetBkColor(bError ? COLOR_NONE : ckBkColor);
		}
	}
}


// Motor Alarm KKB
void CDlgMotionGateInform::MotorErrorStatus(CDeviceMotionGate* pDevice)
{
	DWORD	dwErrorCode;
	CString strErrorCode;
	BOOL	bConnect = FALSE;
	BOOL	bError = FALSE;

	for (INT nMotor = MOTOR0; nMotor < MOTOR_MAX; nMotor++)
	{
		bConnect = pDevice->IsMotorConnected(nMotor);

		if (bConnect)
		{
			dwErrorCode = pDevice->GetMotorErrorCode(nMotor);
			switch (dwErrorCode)
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
			case sts_error_motor_drive_rom_error: // 12
			case sts_error_motor_drive_voltage_error: // = 14
			case sts_error_motor_position_overflow:
			case sts_error_motor_position_change :
			case sts_error_motor_break_short:
			case sts_error_motor_struct_postion_change:
			case sts_error_motor_no_clam_load: // 20
			case sts_error_access_motor_not_exist:
			case sts_error_access_motor_out_of_range: // 모터 접근 범위 초과
			case sts_error_access_pt_not_exist:
			case sts_error_access_pt_out_of_range: // 24
			{  
				strErrorCode.Format(_T(" %02d"), dwErrorCode);
				bError = TRUE; 
			}	break;
			default: 
			{ 
				strErrorCode.Format(_T(""));
				bError = FALSE; 


			}	break;
			
			}
			m_MotorAlarmColorStatic[nMotor].SetTextColor(bError ? COLOR_NONE : COLOR_ORIGINAL_BLUE);
			m_MotorAlarmColorStatic[nMotor].SetBkColor(bError ? COLOR_RED : COLOR_NONE);
			m_MotorAlarmColorStatic[nMotor].SetWindowText(strErrorCode);


		}
	}
}


void CDlgMotionGateInform::MotorReadyStatus(CDeviceMotionGate* pDevice)
{
	DWORD	dwErrorCode;
	BOOL	bConnect = FALSE;
	BOOL	bReady = FALSE;
	BOOL	bBusy = FALSE;
	COLORREF ckBkColor = COLOR_NONE;

	for (INT nMotor = MOTOR0; nMotor < MOTOR_MAX; nMotor++)
	{
		bConnect = pDevice->IsMotorConnected(nMotor);

		if (bConnect)
		{
			dwErrorCode = pDevice->GetMotorErrorCode(nMotor);
			switch (dwErrorCode)
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
			case sts_error_motor_drive_rom_error: // 12
			case sts_error_motor_drive_voltage_error: // = 14
			case sts_error_motor_position_overflow:
			case sts_error_motor_position_change:
			case sts_error_motor_break_short:
			case sts_error_motor_struct_postion_change:
			case sts_error_motor_no_clam_load: // 20
			case sts_error_access_motor_not_exist:
			case sts_error_access_motor_out_of_range: // 모터 접근 범위 초과
			case sts_error_access_pt_not_exist:
			case sts_error_access_pt_out_of_range: // 24
			{
				ckBkColor = COLOR_NONE;
			}	break;
			default:
			{
				bBusy = pDevice->IsMotorBusy(nMotor);
				bReady = !bBusy;

				if (bReady) ckBkColor = COLOR_GREEN;
				if (bBusy)	ckBkColor = COLOR_RED;
			}	break;

			}
			m_MotorReadyColorStatic[nMotor].SetBkColor(ckBkColor);
		}
	}
}



void CDlgMotionGateInform::MotorPositionTableLifeCount(CDeviceMotionGate* pDevice)
{
	COLORREF colorBk = COLOR_NONE, colorTxt = COLOR_NONE;
	
	for (INT nMotor = MOTOR0; nMotor < MOTOR_MAX; nMotor++)
	{
		colorBk = COLOR_NONE;

		if (pDevice->IsMotorConnected(nMotor))
		{
			colorBk = COLOR_GREEN;
			colorTxt = COLOR_BLACK;
			if (pDevice->GetPTLifeAlarmDetected(nMotor))
			{
				colorBk = COLOR_ORANGE;

				if (pDevice->GetPTLifeCycleAlarmDetected(nMotor))
				{
					colorBk = COLOR_RED;
				}
			}

		}

		m_MotorPTLifeColorStatic[nMotor].SetTextColor(colorTxt);
		m_MotorPTLifeColorStatic[nMotor].SetBkColor(colorBk);
	}
}


void CDlgMotionGateInform::MotorPositionTableStatus(CDeviceMotionGate* pDevice)
{
	CString strPosNum;
	WORD wPos = 0;
	BOOL bOverFlow = FALSE;
	BOOL bConnect = FALSE;
	for (INT nMotor = MOTOR0; nMotor < MOTOR_MAX; nMotor++)
	{
		strPosNum = _T("");
		bConnect = pDevice->IsMotorConnected(nMotor);
		if (bConnect)
		{
			wPos = pDevice->GetMotorCurPositionTable(nMotor);

			if (wPos < 33 && 0 < wPos)
			{
				strPosNum.Format(_T(" %02d"), wPos);
			}
			else
			{
				//m_MotorPTColorStatic[nMotor].SetBkColor(COLOR_RED);
				strPosNum.Format(_T("???"));
			}

			m_MotorPTColorStatic[nMotor].SetWindowTextW(strPosNum);
		}

		else
		{
			m_MotorPTColorStatic[nMotor].SetWindowTextW(strPosNum);
		}
	}
	
}


void CDlgMotionGateInform::MotorShowDisable()
{
	for (INT nIndex = MOTOR0; nIndex < MOTOR_MAX; nIndex++)
	{
		if (1)
		{
			m_MotorConnectColorStatic[nIndex].EnableWindow(FALSE);
			m_MotorConnectColorStatic[nIndex].SetWindowTextW(_T(" "));
			m_MotorConnectColorStatic[nIndex].SetTextColor(COLOR_NONE);
			m_MotorConnectColorStatic[nIndex].SetBkColor(COLOR_NONE);
		}

		if (1)
		{
			m_MotorAlarmColorStatic[nIndex].EnableWindow(FALSE);
			m_MotorAlarmColorStatic[nIndex].SetWindowTextW(_T(" "));
			m_MotorAlarmColorStatic[nIndex].SetTextColor(COLOR_NONE);
			m_MotorAlarmColorStatic[nIndex].SetBkColor(COLOR_NONE);
		}

		if (1)
		{
			m_MotorPTColorStatic[nIndex].EnableWindow(FALSE);
			m_MotorPTColorStatic[nIndex].SetWindowTextW(_T(" "));
			m_MotorPTColorStatic[nIndex].SetTextColor(COLOR_NONE);
			m_MotorPTColorStatic[nIndex].SetBkColor(COLOR_NONE);
		}

		if (1)
		{
			m_MotorPTLifeColorStatic[nIndex].EnableWindow(FALSE);
			m_MotorPTLifeColorStatic[nIndex].SetWindowTextW(_T(" "));
			m_MotorPTLifeColorStatic[nIndex].SetTextColor(COLOR_NONE);
			m_MotorPTLifeColorStatic[nIndex].SetBkColor(COLOR_NONE);
		}

		if (1)
		{
			m_MotorReadyColorStatic[nIndex].EnableWindow();
			m_MotorReadyColorStatic[nIndex].SetWindowTextW(_T(" "));
			m_MotorReadyColorStatic[nIndex].SetTextColor(COLOR_NONE);
			m_MotorReadyColorStatic[nIndex].SetBkColor(COLOR_NONE);
		}
				
	}
}


DWORD CDlgMotionGateInform::GetConnectedMotorNum(CDeviceMotionGate* pDevice)
{
	return pDevice->GetConnectedMotorStatus();;
}


DWORD CDlgMotionGateInform::GetMotorTotalAlarm(CDeviceMotionGate* pDevice)
{
	DWORD	dwErrorCode = 0;
	DWORD	dwErrorSum = 0;

	for (INT nMotor = MOTOR0; nMotor < MOTOR_MAX; nMotor++)
	{
		dwErrorCode = pDevice->GetMotorErrorCode(nMotor);

		switch (dwErrorCode)
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
		case sts_error_motor_drive_rom_error: // 12
		case sts_error_motor_drive_voltage_error: // = 14
		case sts_error_motor_position_overflow:
		case sts_error_motor_no_clam_load: // 20
		case sts_error_access_motor_not_exist:
		case sts_error_access_motor_out_of_range: // 모터 접근 범위 초과
		case sts_error_access_pt_not_exist:
		case sts_error_access_pt_out_of_range: // 24
		{ dwErrorSum += dwErrorCode; }	break;
		default: { dwErrorSum += 0; }	break;
		}
	}
	return dwErrorSum;
}


DWORD CDlgMotionGateInform::GetMotorPosionTableTotalNum(CDeviceMotionGate* pDevice)
{
	DWORD dwPos = 0;
	for (INT nMotor = MOTOR0; nMotor < MOTOR_MAX; nMotor++)
	{
		dwPos += pDevice->GetMotorCurPositionTable(nMotor);
	}
	return dwPos;
}

DWORD CDlgMotionGateInform::GetMotorPTtotalLifeCount(CDeviceMotionGate* pDevice)
{
	DWORD dwPortion = 0;
	DWORD dwAxis = pDevice->GetConnectedMotorStatus();
	for (INT nAxis = MOTOR0; nAxis < MOTOR_MAX; nAxis++)
	{
		if ((dwAxis >> nAxis & 0x01))
		{
			dwPortion += pDevice->GetLifePeriodAlarmCount(nAxis);
		}
	}

	return dwPortion;
}



BOOL CDlgMotionGateInform::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	m_tooltip.RelayEvent((LPMSG)pMsg);
	
	m_stTooltip = { 0 };

	if (pMsg->message == WM_MOUSEMOVE)
	{
		CRect rcAlarm[MOTOR_MAX];
		CRect rcStyle[MOTOR_MAX];
		CRect rcLife[MOTOR_MAX];

		BOOL bCheckStatus = FALSE;
		for (INT i = 0; i < MOTOR_MAX; i++)
		{
			this->GetDlgItem(IDC_STATIC_MOTOR0_ALARM + i)->GetWindowRect(&rcAlarm[i]);
			this->GetDlgItem(IDC_STATIC_MOTOR0_PT + i)->GetWindowRect(&rcStyle[i]);
			this->GetDlgItem(IDC_STATIC_MOTOR0_PT_LIFE + i)->GetWindowRect(&rcLife[i]);
		}

		for (INT i = 0; i < MOTOR_MAX; i++)
		{
			if (PtInRect(&rcAlarm[i], pMsg->pt)) m_stTooltip.bMotorError[i] = TRUE;
			if (PtInRect(&rcStyle[i], pMsg->pt)) m_stTooltip.bMotorStyle[i] = TRUE;
			if (PtInRect(&rcLife[i], pMsg->pt)) m_stTooltip.bMotorLife[i] = TRUE;
		}

	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CDlgMotionGateInform::ShowToolTip(CDeviceMotionGate* pDevice)
{
	CString strError, strStyleNum, strLifeCount;
	CStatic* pStatic;

	if (pDevice->IsMotiongateConnected())
	{
		for (INT nMotor = MOTOR0; nMotor < MOTOR_MAX; nMotor++)
		{
			if (nMotor != m_stTooltip.nMotor)
			{
				if (m_stTooltip.bMotorError[nMotor]) // Cursor ON
				{ // Motor Error
					pDevice->GetMotorErrorStatus(strError, nMotor);
					if (m_stDeviceTooltip.strError[nMotor] != strError)
					{
						m_stDeviceTooltip.strError[nMotor] = strError;
						m_tooltip.SetMaxTipWidth(m_stDeviceTooltip.strError[nMotor].GetLength() * 100);
						pStatic = ((CStatic*)GetDlgItem(IDC_STATIC_MOTOR0_ALARM + nMotor));
						m_tooltip.AddTool(pStatic, m_stDeviceTooltip.strError[nMotor]);
					}
				}

				if (m_stTooltip.bMotorStyle[nMotor]) // Cursor ON
				{ // Motor Style Num 
					DWORD dwStyleNum = pDevice->GetMotorCurPositionTable(nMotor);

					if (dwStyleNum < 40 && dwStyleNum != 0)
					{
						strStyleNum.Format(_T("Motor[%d] : Style - No.%d"), nMotor, dwStyleNum);
					}
					else
					{
						strStyleNum.Format(_T("Motor[%d] : PositionTable - Not Defined"), nMotor);
					}

					if (m_stDeviceTooltip.strStyleNum[nMotor] != strStyleNum)
					{
						m_stDeviceTooltip.strStyleNum[nMotor] = strStyleNum;

						pStatic = ((CStatic*)GetDlgItem(IDC_STATIC_MOTOR0_PT + nMotor));
						m_tooltip.SetMaxTipWidth(m_stDeviceTooltip.strStyleNum[nMotor].GetLength() * 100);
						m_tooltip.AddTool(pStatic, m_stDeviceTooltip.strStyleNum[nMotor]);
					}
				}

				if (m_stTooltip.bMotorLife[nMotor]) // Cursor ON
				{ // Motor Life Count
					DWORD dwLifeCount = pDevice->GetMotorLifeCount(nMotor);
					strLifeCount.Format(_T("Motor[%d] : Life Count - %d"), nMotor, dwLifeCount);

					if (m_stDeviceTooltip.strLifeCount[nMotor] != strLifeCount)
					{
						m_stDeviceTooltip.strLifeCount[nMotor] = strLifeCount;

						pStatic = ((CStatic*)GetDlgItem(IDC_STATIC_MOTOR0_PT_LIFE + nMotor));
						m_tooltip.SetMaxTipWidth(m_stDeviceTooltip.strLifeCount[nMotor].GetLength() * 100);
						m_tooltip.AddTool(pStatic, m_stDeviceTooltip.strLifeCount[nMotor]);
					}
					
				}
			}

			m_stTooltip.nMotor = nMotor;
		}
	}
}


HBITMAP CDlgMotionGateInform::GetEditSystemImage(CDeviceMotionGate* pDevice)
{
	ASSERT(pDevice);
	CString strSystemName;
	strSystemName = pDevice->GetSystemName();
	CBitmap* pBitmap = pDevice->GetEditSystemImage();

	CDataBaseProject* pProject = CDataBaseProject::GetInstance();
	if (pProject->OpenDataBase())
	{
		if (pProject->GetDeviceList())
		{
			while (CDataBaseRecordData* pRecord = pProject->SeekToNext())
			{
				pBitmap = pDevice->LoadImageFromDatabase(pRecord, strSystemName);
			}
		}
	}
	pProject->CloseDataBase();

	return *pBitmap;
}


//const BOOL CDlgMotionGateInform::LoadMainImage(CBitmap* pBitmap)
//{
//	if (NULL == pBitmap->GetSafeHandle())
//	{
//		return FALSE;
//	}
//
//	BITMAP bitmap = { 0, };
//	pBitmap->GetBitmap(&bitmap);
//
//	if (NULL != m_bmpImage.GetSafeHandle())
//	{
//		m_bmpMain.DeleteObject();
//	}
//
//	m_bmpMain.Attach(::CopyImage(static_cast<HBITMAP>(*pBitmap), IMAGE_BITMAP, 0, 0, 0));
//
//	Invalidate(FALSE);
//
//	return TRUE;
//}


const BOOL CDlgMotionGateInform::SetImage(CDeviceMotionGate* pDevice)
{
	if (pDevice != NULL)
	{
		m_wndViewer.LoadMainImage(pDevice->GetSystemImage());
	}
	else
	{
		CDataBaseConfig* base = CDataBaseConfig::GetInstance();
		
		//HBITMAP bitmap = base->GetNoImage();
		CBitmap* bitmap = CBitmap::FromHandle(base->GetNoImage());
		//HGDIOBJ bitmap = ::GetCurrentObject(base->GetNoImage())
		//CBitmap* bitmap = reinterpret_cast<CBitmap*>(base->GetSerialSystemImage());
		//CBitmap* pBitmap = ;
		m_wndViewer.LoadMainImage(bitmap);
	}

	
	
	return TRUE;
}



void CDlgMotionGateInform::ServoControl(BOOL bServoOn)
{
	CDeviceMotionGate* pDevice = m_pDevice;
	if (NULL == pDevice)
	{
		return;
	}

	DWORD dwIndex = 0x00000000;
	/*DWORD dwCheck;
	for (INT nIndex = 1; nIndex <= 8; nIndex++)
	{
		if (pDevice->IsServoEnabled(nIndex))
		{
			dwIndex |= (0x1 << nIndex);
		}
	}
	
	dwCheck = dwIndex;
	BOOL bCheck = true;*/
	//pDevice->m_stCommand.cmdMotor[nIndex].command.bitServoOn
	dwIndex = pDevice->GetConnectedMotorStatus();
	for (INT i = 1; i < 9; i++)
	{
		if ((dwIndex >> i) & 0x01)
		{
			pDevice->ServoControl(i, bServoOn);
		}
	}
	//dwIndex |= GetSelectedMotorIndex();

	pDevice->EnableServo(dwIndex);
}



void CDlgMotionGateInform::SetCommandResultText(CString str)
{
	SetDlgItemText(IDC_STATIC_CMD_RESULT, str);
}


void CDlgMotionGateInform::SetBackupScheduleDateTime(CString strTime)
{
	CString strCompare;
	GetDlgItemText(IDC_STATIC_BACKUP_SCHEDULE, strCompare);
	if (strCompare != strTime) // 변화되는 조건만 확인.. Blinking 방지..
	{
		SetDlgItemText(IDC_STATIC_BACKUP_SCHEDULE, strTime);
	}
}


void CDlgMotionGateInform::SetBackupAutoSec(CString strSec)
{
	CString strCompare;
	GetDlgItemText(IDC_STATIC_BACKUP_AUTOSEC, strCompare);
	if (strCompare != strSec) // 변화되는 조건만 확인.. Blinking 방지..
	{
		SetDlgItemText(IDC_STATIC_BACKUP_AUTOSEC, strSec);
	}
}


void CDlgMotionGateInform::SetBackupTimeControl(BOOL bBackupAutoSecControl, BOOL bBackupToggle, BOOL bBdakupTime)
{
	(GetDlgItem(IDC_STATIC_BACKUP_AUTOSEC))->EnableWindow(bBackupAutoSecControl);
	//(GetDlgItem(IDC_STATIC_TOGGLE_BTN))->EnableWindow(bBackupToggle);
	(GetDlgItem(IDC_STATIC_BACKUP_BUTTON))->EnableWindow(bBackupToggle);
	(GetDlgItem(IDC_STATIC_BACKUP_SCHEDULE))->EnableWindow(bBdakupTime);
}


void CDlgMotionGateInform::OnBnClickedBtnToggleBakcup()
{
	CDeviceMotionGate* pDevice = m_pDevice;
	if (NULL == pDevice)
	{
		return;
	}

	pDevice->EnableAutoRestoration(!pDevice->IsAutoRestorationEnabled());
}


void CDlgMotionGateInform::OnStnClickedStaticToggleBtn()
{
	SendCmdMsgToMainFrame(ID_INFORM_BACKUP_TOGGLE);
}


void CDlgMotionGateInform::OnStnDblclickStaticToggleBtn()
{
	BOOL bCheck = TRUE;
}


void CDlgMotionGateInform::OnStnClickedStaticToggle()
{
	SendCmdMsgToMainFrame(ID_INFORM_BACKUP_TOGGLE);
}


const BOOL CDlgMotionGateInform::DisableControl()
{
	CString str;
	BOOL bEnable = FALSE;
	
	MotorShowDisable();

	SetMotorErrorState(str);

	if (true)
	{
		(GetDlgItem(IDC_STATIC_BACKUP_AUTOSEC))->EnableWindow(bEnable);
		(GetDlgItem(IDC_STATIC_BACKUP_BUTTON))->EnableWindow(bEnable);
		(GetDlgItem(IDC_STATIC_BACKUP_SCHEDULE))->EnableWindow(bEnable);

		(GetDlgItem(IDC_STATIC_BACKUP_AUTOSEC))->SetWindowText(str);
		(GetDlgItem(IDC_STATIC_BACKUP_BUTTON))->SetWindowText(str);
		(GetDlgItem(IDC_STATIC_BACKUP_SCHEDULE))->SetWindowText(str);
	}

	for (INT i = 0; i < MOTOR_MAX; i++)
	{
		(GetDlgItem(IDC_STATIC_TEXT_MOTOR0 + i))->EnableWindow(bEnable);
	}

	for (INT i = 0; i < MGSTATE_MAX; i++)
	{
		((CStatic*)GetDlgItem(IDC_STATIC_DEV0 + i))->SetWindowTextW(_T(""));
	}

	
	if (1)
	{ // Status Led Initialize
		m_ColorStatic[STATUS_CONNECTED].EnableWindow(bEnable);
		m_ColorStatic[STATUS_BUSY].EnableWindow(bEnable);
		m_ColorStatic[STATUS_WARNNING].EnableWindow(bEnable);
		m_ColorStatic[STATUS_ERROR].EnableWindow(bEnable);
		m_ColorStatic[STATUS_MASTER].EnableWindow(bEnable);

		m_ColorStatic[STATUS_CONNECTED].SetBkColor(COLOR_NONE);
		m_ColorStatic[STATUS_BUSY].SetBkColor(COLOR_NONE);
		m_ColorStatic[STATUS_WARNNING].SetBkColor(COLOR_NONE);
		m_ColorStatic[STATUS_ERROR].SetBkColor(COLOR_NONE);
		m_ColorStatic[STATUS_MASTER].SetBkColor(COLOR_NONE);

	}
	
	
	if (1)
	{ // Button Initialize
		BOOL bEnable = FALSE;
		GetDlgItem(IDC_BUTTON_BACKUP)->EnableWindow(bEnable);
		GetDlgItem(IDC_BUTTON_RESTORE)->EnableWindow(bEnable);
		GetDlgItem(IDC_BUTTON_COMPARE)->EnableWindow(bEnable);
		GetDlgItem(IDC_BUTTON_CONFIG)->EnableWindow(bEnable);
		GetDlgItem(IDC_BUTTON_BACKUP_SCHEDULE)->EnableWindow(bEnable);
		GetDlgItem(IDC_MFCBUTTON_MG_RESET)->EnableWindow(bEnable);
		GetDlgItem(IDC_MFCBUTTON_REBOOT)->EnableWindow(bEnable);
		GetDlgItem(IDC_BUTTON_MOTOR_RESET)->EnableWindow(bEnable);
		GetDlgItem(IDC_BUTTON_ASSIGN)->EnableWindow(bEnable);
		GetDlgItem(IDC_BUTTON_BACKUP_MANAGE)->EnableWindow(bEnable);
		GetDlgItem(IDC_BUTTON_RUN)->EnableWindow(bEnable);
		GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(bEnable);
		//GetDlgItem(IDC_CHECK_SERVO_CONTROL)->EnableWindow(bEnable);
	}

	//((CStatic*)GetDlgItem(IDC_STATIC_DEV6))->SetWindowText(str);

	

	return 0;
}


const BOOL CDlgMotionGateInform::IsControlEnable()
{
	return m_bDisable;
}


void CDlgMotionGateInform::SetControlDisable(BOOL bDisable)
{
	m_bDisable = bDisable;
}


void CDlgMotionGateInform::SetEnableSystemImage(BOOL bEnable)
{
	if (m_wndViewer.GetSafeHwnd() == NULL)
	{
		DWORD dwError = GetLastError();
		CString str, strName, strIP;
		strName = m_pDevice->GetSystemName();
		m_pDevice->GetAddress(strIP);
		str.Format(_T("[%s / %s] '%d' Last Error"), strIP, strName, dwError);
		AfxMessageBox(str);

		return;
	}

	//m_wndViewer.EnableWindow(bEnable);
}


const INT CDlgMotionGateInform::GetProgressPosition()
{
	return m_Progress.GetPos();
}


void CDlgMotionGateInform::OnBnClickedMfcbuttonAlarmRst()
{
	/*SendCmdMsgToMainFrame(ID_INFORM_ALARM_RST);
	HomingTest();*/
	
	INT n = 0;
	
	for ( ; ; )
	{
		if (CDeviceMotionGate::state_motion_gate_command == m_pDevice->GetStatus())
		{
			m_pDevice->EnableAutoHoming(TRUE);
			m_pDevice->ActiveHoming();
			break;
		}
	}
}


void CDlgMotionGateInform::HomingTest()
{
	INT n = 0;

	for (; ; )
	{
		if (CDeviceMotionGate::state_motion_gate_command == m_pDevice->GetStatus())
		{
			m_pDevice->EnableAutoHoming(TRUE);
			m_pDevice->ActiveHoming();
			break;
		}
	}
}


void CDlgMotionGateInform::OnBnClickedMfcbuttonMotorReset()
{
	SendCmdMsgToMainFrame(ID_INFORM_MOTOR_RESET);
}


void CDlgMotionGateInform::OnBnClickedMfcbuttonBackup()
{
	SendCmdMsgToMainFrame(ID_INFORM_BACKUP);
}


void CDlgMotionGateInform::OnBnClickedMfcbuttonRestore()
{
	SendCmdMsgToMainFrame(ID_INFORM_RESTORATION);
}


void CDlgMotionGateInform::OnBnClickedMfcbuttonCompare()
{
	SendCmdMsgToMainFrame(ID_INFORM_COMPARE);
}


void CDlgMotionGateInform::OnBnClickedMfcbuttonConfig()
{
	SendCmdMsgToMainFrame(ID_INFORM_CONFIG);
}


void CDlgMotionGateInform::OnBnClickedMfcbuttonBackupSchedule()
{
	SendCmdMsgToMainFrame(ID_INFORM_BACKUP_SCHEDULE);
}


void CDlgMotionGateInform::OnBnClickedMfcbuttonMgReset()
{
	SendCmdMsgToMainFrame(ID_INFORM_FACTORY_RESET);
}


void CDlgMotionGateInform::OnBnClickedMfcbuttonReboot()
{
	SendCmdMsgToMainFrame(ID_INFORM_REBOOT);
}


void CDlgMotionGateInform::OnBnClickedMfcbuttonAssign()
{
	SendCmdMsgToMainFrame(ID_INFORM_ASSIGN);
}


void CDlgMotionGateInform::OnBnClickedMfcbuttonBackupManage()
{
	SendCmdMsgToMainFrame(ID_INFORM_BACKUP_MANAGE);
}


void CDlgMotionGateInform::OnBnClickedMfcbuttonRun()
{
	DoDeviceMonitoring(TRUE);
}


void CDlgMotionGateInform::OnBnClickedMfcbuttonStop()
{
	DoDeviceMonitoring(FALSE);
}


void CDlgMotionGateInform::OnBnClickedCheckServoControl()
{
	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_SERVO_CONTROL))->GetCheck();
	ServoControl(bCheck);
}


void CDlgMotionGateInform::OnBnClickedButtonHoming()
{
	CDlgHomingControlPanel homing;
	homing.SetMotionGate(m_pDevice);

	if (IDOK == homing.DoModal())
	{

	}
}


void CDlgMotionGateInform::InitButtonSetting()
{
	/*HWND hWnd = ::GetActiveWindow();
	HDC hDC = ::GetDC(hWnd);
	HDC hMemDC;
	*/
	CClientDC dc(this);
	
	INT width, height;
	INT nRate = 10;
	//m_backup, m_backupManage, m_restore, m_compare, m_backupSchedule, m_assign, m_reset;
	HRESULT hRes;
	CImage Image;
	HBITMAP hBit;
	
	if (1)
	{
		m_wndViewer.SubclassDlgItem(IDC_ST_IMAGE, this);
	}

	if (1)
	{
		hRes = Image.Load(L"res/TotalConfig/Run.png");

		width = Image.GetWidth();
		height = Image.GetHeight();
		//hMemDC = CreateCompatibleDC(hDC);
		if (hRes != S_OK)
		{
			ASSERT(0);
		}
		Image.StretchBlt(dc, 0, 0, width * nRate, height * nRate, SRCCOPY);
		hBit = Image.Detach();

		m_run.SubclassDlgItem(IDC_BUTTON_RUN, this);
		m_run.SetImage(hBit, TRUE);
	}

	if (1)
	{
		hRes = Image.Load(L"res/TotalConfig/Stop.png");
		if (hRes != S_OK)
		{
			ASSERT(0);
		}
		hBit = Image.Detach();

		m_stop.SubclassDlgItem(IDC_BUTTON_STOP, this);
		m_stop.SetImage(hBit, TRUE);
	}

	if (true)
	{
		hRes = Image.Load(L"res/TotalConfig/backup.png");
		if (hRes != S_OK)
		{
			ASSERT(0);
		}
		hBit = Image.Detach();

		m_backup.SubclassDlgItem(IDC_BUTTON_BACKUP, this);
		m_backup.SetImage(hBit, TRUE);
	}

	if (true)
	{
		hRes = Image.Load(L"res/TotalConfig/BackupManage.png");
		if (hRes != S_OK)
		{
			ASSERT(0);
		}
		hBit = Image.Detach();

		m_backupManage.SubclassDlgItem(IDC_BUTTON_BACKUP_MANAGE, this);
		m_backupManage.SetImage(hBit, TRUE);
	}

	if (true)
	{
		hRes = Image.Load(L"res/TotalConfig/Restore.png");
		if (hRes != S_OK)
		{
			ASSERT(0);
		}
		hBit = Image.Detach();

		m_restore.SubclassDlgItem(IDC_BUTTON_RESTORE, this);
		m_restore.SetImage(hBit, TRUE);
	}

	if (true)
	{
		hRes = Image.Load(L"res/TotalConfig/Compare.png");
		if (hRes != S_OK)
		{
			ASSERT(0);
		}
		hBit = Image.Detach();

		m_compare.SubclassDlgItem(IDC_BUTTON_COMPARE, this);
		m_compare.SetImage(hBit, TRUE);
	}

	if (true)
	{
		hRes = Image.Load(L"res/TotalConfig/Config.png");
		if (hRes != S_OK)
		{
			ASSERT(0);
		}
		hBit = Image.Detach();

		m_config.SubclassDlgItem(IDC_BUTTON_CONFIG, this);
		m_config.SetImage(hBit, TRUE);
	}

	if (true)
	{
		hRes = Image.Load(L"res/TotalConfig/Schedule.png");
		if (hRes != S_OK)
		{
			ASSERT(0);
		}
		hBit = Image.Detach();

		m_backupSchedule.SubclassDlgItem(IDC_BUTTON_BACKUP_SCHEDULE, this);
		m_backupSchedule.SetImage(hBit, TRUE);
	}

	if (true)
	{
		hRes = Image.Load(L"res/TotalConfig/ID Assign_Ex.png");
		if (hRes != S_OK)
		{
			ASSERT(0);
		}
		hBit = Image.Detach();

		m_assign.SubclassDlgItem(IDC_BUTTON_ASSIGN, this);
		m_assign.SetImage(hBit, TRUE);
	}

	if (true)
	{
		hRes = Image.Load(L"res/TotalConfig/Reset.png");
		if (hRes != S_OK)
		{
			ASSERT(0);
		}
		hBit = Image.Detach();

		m_reset.SubclassDlgItem(IDC_BUTTON_MOTOR_RESET, this);
		m_reset.SetImage(hBit, TRUE);
	}

	
}


void CDlgMotionGateInform::InitControlTextSize()
{
	GetDlgItem(IDC_STATIC_IP)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_NAME)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_STATUS)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_COUNT)->SetFont(&m_BigFont);

	for (int i = 0; i < MGSTATE_MAX; i++)
	{
		GetDlgItem(IDC_STATIC_DEV0 + i)->SetFont(&m_BigFont);
	}

	GetDlgItem(IDC_STATIC_DEV5)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_DEV6)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_DEV7)->SetFont(&m_BigFont);

	GetDlgItem(IDC_ST_CONNECT)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_WARNING)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_BUSY)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_ERROR)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_MASTER_CONNECT)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_PROGRESS)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_SEC)->SetFont(&m_BigFont);
	GetDlgItem(IDC_PROGRESS_DEVICE)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_GROUP_OPERATION)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_GROUP_CONFIG)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_AUTOBACKUP)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_TIME)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_BACKUP_SCHEDULE)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_BACKUP_AUTOSEC)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_BACKUP_BUTTON)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_MOTOR_ALARM)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_GROUP_CONNECT)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_GROUP_READY)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_GROUP_ERROR)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_GROUP_STYLE)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_GROUP_COUNT)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_GROUP2)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_GROUP_MOTOR)->SetFont(&m_BigFont);

	for (INT nIndex = MOTOR0; nIndex < MOTOR_MAX; nIndex++)
	{
		GetDlgItem(IDC_STATIC_TEXT_MOTOR0 + nIndex)->SetFont(&m_BigFont);
	}

	for (INT nIndex = MOTOR0; nIndex < MOTOR_MAX; nIndex++)
	{
		GetDlgItem(IDC_STATIC_MOTOR0_ALARM + nIndex)->SetFont(&m_BigFont);
	}

	for (INT nIndex = MOTOR0; nIndex < MOTOR_MAX; nIndex++)
	{
		GetDlgItem(IDC_STATIC_MOTOR0_PT + nIndex)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_MOTOR0_PT + nIndex)->SetWindowText(_T("???"));
	}
}

LPVOID CDlgMotionGateInform::GetBigFont()
{
	return &m_BigFont;
}

void CDlgMotionGateInform::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.
	m_BigFont.Detach();

	LOGFONT lf = { 0, };
	afxGlobalData.fontRegular.GetLogFont(&lf);
	lf.lfHeight *= 1.5;
	
	m_BigFont.CreateFontIndirect(&lf);

	//InitControlTextSize();
	
	EnumChildWindows(this->m_hWnd, EnumChildProcess, reinterpret_cast<LPARAM>(this));
	
	//m_BigFont.Detach();
}

BOOL CALLBACK EnumChildProcess(HWND hWnd, LPARAM lParam)
{
	CDlgMotionGateInform* pInform = reinterpret_cast<CDlgMotionGateInform*>(lParam);
	
	HFONT hFont = reinterpret_cast<HFONT>(pInform->GetBigFont());

	INT nID = GetDlgCtrlID(hWnd);

	switch (nID)
	{
	case IDC_STATIC_DEV0:
	{
		//SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, NULL);
	} break;

	default:
		break;
	}

	return TRUE;
}