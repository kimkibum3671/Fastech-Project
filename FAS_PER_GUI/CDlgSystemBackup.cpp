// CDlgSystemBackup.cpp: 구현 파일
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CDlgSystemBackup.h"
#include "afxdialogex.h"
#include "CDeviceList.h"

#define TIMER_BACKUP 1
#define TIMER_DEVICE_BACKUP 2

// CDlgSystemBackup 대화 상자

IMPLEMENT_DYNAMIC(CDlgSystemBackup, CDialogEx)

CDlgSystemBackup::CDlgSystemBackup(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SYSTEM_BACKUP, pParent)
{
	m_bDoNext = FALSE;
	m_bIsRun = FALSE;
	m_process = process_Initate;
	m_device = {0,};
}

CDlgSystemBackup::~CDlgSystemBackup()
{
	
}

void CDlgSystemBackup::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_BACKUP, m_progress);
	DDX_Control(pDX, IDC_EDIT_TIME_FILTER, m_EditTimer);
	DDX_Control(pDX, IDC_SPIN_TIMER, m_spTimer);
	DDX_Control(pDX, IDC_EDIT_COMMENT, m_editComment);
}


BEGIN_MESSAGE_MAP(CDlgSystemBackup, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_BACKUP, &CDlgSystemBackup::OnBnClickedButtonBackup)
	ON_BN_CLICKED(IDOK, &CDlgSystemBackup::OnBnClickedOk)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_ALL, &CDlgSystemBackup::OnBnClickedCheckAll)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TIMER, &CDlgSystemBackup::OnDeltaposSpinTimer)
	ON_BN_CLICKED(IDC_BUTTON_LIST_UPDATE, &CDlgSystemBackup::OnBnClickedButtonListSelectALL)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_BACKUP_ENABLE, &CDlgSystemBackup::OnLvnItemchangedListBackupEnable)
END_MESSAGE_MAP()


// CDlgSystemBackup 메시지 처리기


BOOL CDlgSystemBackup::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rect;
	CString strComment = _T("Batch Backup");
	
	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_SYSTEM_BATCH));
	this->SetIcon(hIcon, FALSE);

	// m_listCtrl : 백업 가능 장치 리스트
	m_listCtrl.SubclassDlgItem(IDC_LIST_BACKUP_ENABLE, this);
	m_listCtrl.GetWindowRect(&rect);
	m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER | LVS_EX_CHECKBOXES);
	m_listCtrl.InsertColumn(0, TEXT("[Check] Tooling System"));
	m_listCtrl.SetColumnWidth(0, 400);


	// m_listCtrlDisable : 백업 불가 장치 리스트
	m_listCtrlDisable.SubclassDlgItem(IDC_LIST_BACKUP_DISABLE, this);
	m_listCtrlDisable.GetWindowRect(&rect);
	m_listCtrlDisable.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER /*| LVS_EX_CHECKBOXES*/);
	m_listCtrlDisable.InsertColumn(0, TEXT("[Disable] Tooling System"));
	m_listCtrlDisable.SetColumnWidth(0, 200);

	m_listCtrlDisable.InsertColumn(1, TEXT("State"));
	m_listCtrlDisable.SetColumnWidth(1, 250);
	//m_listCtrlDisable.EnableWindow(FALSE);

	m_EditTimer.SetWindowText(_T("10"));

	m_spTimer.SetRange(0, 60);
	m_spTimer.SetPos(10);

	m_editComment.SetWindowText(strComment);

	InitalizeListControl();

	InitControlTextSize();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgSystemBackup::InitalizeListControl()
{
	BACKUP_LIST device;
	
	CDeviceList* pDeviceList = CDeviceList::GetInstance();
	CDeviceMotionGate* pDevice;
	
	m_list.RemoveAll();
	m_listCtrl.RemoveAll();
	m_listCtrlDisable.RemoveAll();

	for (INT i = 0; i < pDeviceList->GetAddressCount(); i++)
	{
		const DWORD dwAddress = pDeviceList->GetAddressByIndex(i);
		pDeviceList->GetMotiongateByAddress(dwAddress, pDevice);
		 
		BOOL bState = pDevice->GetMotionGateConnectedStatus();
		CString str = pDevice->GetSystemName();

		if ( TRUE == pDevice->GetMotionGateConnectedStatus())
		{
			INT iItem = 0;
			DWORD rgb = 0;
			CString str;
			BOOL bBackup = FALSE;
			
			switch (pDevice->GetStatus())
			{
			case state_motion_gate_get_system_status_all:
			case state_motion_gate_command:
			{
				bBackup = TRUE;
				str.Format(_T("Idle"));
				
				if ( TRUE == pDevice->IsSystemStatusError())
				{
					bBackup = FALSE;
					str.Format(_T("System Error"));
					rgb = RGB(0xff, 0, 0);
				}

			}	break;

			case state_stop:
			{
				str.Format(_T("Stop"));
				rgb = RGB(0xff, 0, 0);
			}	break;
			case state_connect:
			{
				str.Format(_T("Try connecting"));
				rgb = RGB(0xff, 0, 0);
			}	break;
			case state_reboot:
			{
				str.Format(_T("Reboot"));
				rgb = RGB(0xff, 0, 0);
			}	break;
			case state_backup:
			{
				str.Format(_T("Backup"));
				rgb = RGB(0xff, 0, 0);
			}	break;
			case state_restoration:
			{
				str.Format(_T("Restoration"));
				rgb = RGB(0xff, 0, 0);
			}	break;
			case state_factory_reset:
			{
				str.Format(_T("Factory reset"));
				rgb = RGB(0xff, 0, 0);
			}	break;
			case state_assign_motor_id:
			{
				str.Format(_T("Assign motor id"));
				rgb = RGB(0xff, 0, 0);
			}	break;
			case state_home_all:
			{
				str.Format(_T("Homming"));
				rgb = RGB(0xff, 0, 0);
			}	break;
			case state_change_information:
			{
				str.Format(_T("Change information"));
				rgb = RGB(0xff, 0, 0);
			}	break;

			default:
				break;
			}
			
			if (bBackup && IsEnableBackupScheduleTime(pDevice))
			{
				INT index = m_listCtrl.GetItemCount();
				m_listCtrl.InsertItem(index, pDevice->GetSystemName());
				m_listCtrl.SetItemText(index, 1, str);
			}

			else
			{
				if ((!IsEnableBackupScheduleTime(pDevice)) && (bBackup))
				{
					INT min = (pDevice->GetAutoBackupLeftSeconds() / 60);
					str.Format(_T("Auto Backup Schecdule In %d Min"), min);
					
					rgb = RGB(0xff, 0, 0);
				}

				INT index = m_listCtrlDisable.GetItemCount();
				m_listCtrlDisable.InsertItem(index, pDevice->GetSystemName());
				m_listCtrlDisable.SetItemText(index, 1, str);

				for (INT nCol = 0; nCol < 2; nCol++) m_listCtrlDisable.SetCellBkColor(index, nCol, rgb);
			}

			device.pDevice = pDevice;
			device.strName = pDevice->GetSystemName();
			device.bBackup = bBackup;
			device.bBackupStart = FALSE;
			
			m_list.AddTail(device);
			m_listCtrl.SetItemData(i,(DWORD_PTR)&device);
		}

	} 
	
	if (0 == m_listCtrl.GetItemCount()) m_listCtrl.EnableWindow(FALSE);

	if (0 == m_listCtrlDisable.GetItemCount()) m_listCtrlDisable.EnableWindow(FALSE);

}


void CDlgSystemBackup::OnBnClickedButton1()
{
	m_listCtrl.Invalidate(FALSE);
}


void CDlgSystemBackup::OnBnClickedButtonBackup()
{
	INT nMax = 0;
	m_progress.SetPos(0);

	for (INT i = 0; i < m_listCtrl.GetItemCount(); i++)
	{
		if (m_listCtrl.GetCheck(i)) nMax++;
	}
	
	m_progress.SetRange(0, nMax);
	m_bIsRun = TRUE;
	SetTimer(TIMER_BACKUP, 100 , NULL);
}


void CDlgSystemBackup::OnBnClickedOk()
{

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}





void CDlgSystemBackup::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_BACKUP:
	{
		ExecuteBackup();
	}	break;
	default:
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CDlgSystemBackup::EnableControlWindow(BOOL bEnable)
{
	GetDlgItem(IDC_EDIT_COMMENT)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_BACKUP)->EnableWindow(bEnable);
	GetDlgItem(IDC_LIST_BACKUP_ENABLE)->EnableWindow(bEnable);
}


void CDlgSystemBackup::OnBnClickedCheckAll()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_ALL);
	INT nCheck = pBtn->GetState();
	if (pBtn->GetState() & 0x01)
	{
		for (INT i = 0; i < m_listCtrl.GetItemCount(); i++)
		{
			m_listCtrl.SetCheck(i);
		}
	}
}


const BOOL CDlgSystemBackup::IsEnableBackupScheduleTime(CDeviceMotionGate* pDevice)
{
	BOOL bEnable = FALSE;
	CString strEidtTimer;
	DOUBLE dbLeftSecond = 0;
	INT nLeftMin = 0;
	INT nConfigMin = 0;
	
	CString strName = pDevice->GetSystemName();
	
	if (!pDevice->IsAutoRestorationEnabled()) return TRUE;
	if (!pDevice->IsAutoBackupScheduleExist()) return TRUE;

	// m_EditTimer는 10분으로 Default...
	m_EditTimer.GetWindowText(strEidtTimer);
	
	dbLeftSecond = pDevice->GetAutoBackupLeftSeconds();
	nLeftMin = static_cast<INT>(dbLeftSecond) / 60;
	nConfigMin = _ttoi(strEidtTimer);

	if (nLeftMin >= nConfigMin) bEnable = TRUE;

	return bEnable;
}


void CDlgSystemBackup::OnDeltaposSpinTimer(NMHDR* pNMHDR, LRESULT* pResult)
{// 사용 X
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	int iVal = pNMUpDown->iPos + pNMUpDown->iDelta;

	if ((0 <= iVal) && ( 60 >= iVal))
	{
		CString strValue;
		strValue.Format(_T("%d \n"), iVal);
		m_EditTimer.SetWindowText(strValue);
	}
	
	*pResult = 0;
}


void CDlgSystemBackup::AddFailDevice(BACKUP_LIST device)
{
	CDeviceMotionGate* pDevice = device.pDevice;
	if (TRUE == pDevice->GetMotionGateConnectedStatus())
	{ // 연결 상태에서 백업 시간이 설정 시간보다 작을경우
		DOUBLE dbLeftSecond = 0;
		INT nLeftMin = 0;
		CString str;
		
		dbLeftSecond = pDevice->GetAutoBackupLeftSeconds();
		nLeftMin = static_cast<INT>(dbLeftSecond) / 60;
		str.Format(_T("Auto Backup Schecdule In %d Min..."), nLeftMin);

		INT index = m_listCtrlDisable.GetItemCount();
		m_listCtrlDisable.InsertItem(index, device.strName);
		m_listCtrlDisable.SetItemText(index, 1, str);

		for (INT nCol = 0; nCol < 2; nCol++) m_listCtrlDisable.SetCellBkColor(index, nCol, RGB(0xff, 0, 0));
	}

	else
	{
		INT iItem = 0;
		DWORD rgb = 0;
		CString str;
		BOOL bBackup = FALSE;

		switch (pDevice->GetStatus())
		{
		case state_motion_gate_get_system_status_all:
		case state_motion_gate_command:
		{
			bBackup = TRUE;
		}	break;

		case state_stop:
		{
			str.Format(_T("Stop..."));
			rgb = RGB(0xff, 0, 0);
		}	break;
		case state_connect:
		{
			str.Format(_T("[Disconnect] Try connecting..."));
			rgb = RGB(0xff, 0, 0);
		}	break;
		case state_reboot:
		{
			str.Format(_T("Reboot..."));
			rgb = RGB(0xff, 0, 0);
		}	break;
		case state_backup:
		{
			str.Format(_T("Backup..."));
			rgb = RGB(0xff, 0, 0);
		}	break;
		case state_restoration:
		{
			str.Format(_T("Restoration..."));
			rgb = RGB(0xff, 0, 0);
		}	break;
		case state_factory_reset:
		{
			str.Format(_T("Factory reset..."));
			rgb = RGB(0xff, 0, 0);
		}	break;
		case state_assign_motor_id:
		{
			str.Format(_T("Assign motor id..."));
			rgb = RGB(0xff, 0, 0);
		}	break;
		case state_home_all:
		{
			str.Format(_T("Homming..."));
			rgb = RGB(0xff, 0, 0);
		}	break;
		case state_change_information:
		{
			str.Format(_T("Change information..."));
			rgb = RGB(0xff, 0, 0);
		}	break;

		default:
			break;
		}

		if (!bBackup)
		{
			INT index = m_listCtrlDisable.GetItemCount();
			m_listCtrlDisable.InsertItem(index, device.strName);
			m_listCtrlDisable.SetItemText(index, 1, str);

			for (INT nCol = 0; nCol < 2; nCol++) m_listCtrlDisable.SetCellBkColor(index, nCol, RGB(0xff, 0, 0));
		}
		
	}
}


void CDlgSystemBackup::OnBnClickedButtonListSelectALL()
{
	//InitalizeListControl();
	for (INT i = 0; i < m_listCtrl.GetItemCount(); i++)
	{
		m_listCtrl.SetCheck(i);
	}
}


void CDlgSystemBackup::OnLvnItemchangedListBackupEnable(NMHDR* pNMHDR, LRESULT* pResult)
{ // 사용 X
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	for (INT i = 0; i < m_listCtrl.GetItemCount(); i++)
	{
		if (m_listCtrl.GetCheck(i)) ((CButton*)GetDlgItem(IDC_CHECK_ALL))->SetCheck(TRUE);
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK_ALL))->SetCheck(FALSE);
			break;
		}
	}

}


void CDlgSystemBackup::ExecuteBackup()
{
	CDeviceMotionGate* pDevice = NULL;
	
	if (m_device.pDevice != NULL)
	{
		pDevice = m_device.pDevice;
	}

	CString strState,strName;
	static INT nIndex = 0;

	switch (m_process)
	{
	case process_Initate:
	{
		if (nIndex == m_listCtrl.GetItemCount())
		{ // Backup 가능한 장치가 ListControl에 없거나, Backup 대상 마지막 장치까지 완료하였을 경우..
			KillTimer(TIMER_BACKUP);
			
			m_bIsRun = FALSE;
			m_process = process_Initate;
			if (nIndex != 0) AfxMessageBox(_T("System Backup done"));
			nIndex = 0;
		}
		else
		{
			m_process = process_searching;
		}
		
	}	break;

	case process_searching:
	{
		POSITION pos = m_list.GetHeadPosition();
		BOOL bDetected = FALSE;
		while (pos)
		{ // Backup 대상 객체를 가져온다.
			m_device = m_list.GetNext(pos);
			strName = m_listCtrl.GetItemText(nIndex, 0);
			if ((m_device.strName == strName) && (m_listCtrl.GetCheck(nIndex)))
			{ // CheckBox가 활성화 되어있고, 리스트에 등록된 장치 이름이 백업 대상 장치 이름과 맞을경우..
				pDevice = m_device.pDevice;
				m_process = process_checking;
				bDetected = TRUE;
				break;
			}
		}

		if (bDetected)
		{ // Backup 대상 찾지 못했을경우 다시 처음과정으로 돌아감
			m_process = process_checking;
		}

		else
		{ // Backup 대상 찾았을 경우
			nIndex++;
			m_process = process_Initate;
		}
	}	break;

	case process_checking:
	{// 한번더 체크
		if ((!pDevice->GetBackupState() || // 백업 상태가 아니거나
			!pDevice->IsMotiongateConnected()) && // 연결이 끊어졌거나
			IsEnableBackupScheduleTime(pDevice)) // AutoBackup 시간이 10분이내이거나
		{ 
			CString strComment;
			CString strEdit;
			CString strCheck = m_listCtrl.GetItemText(nIndex, 0);
			m_editComment.GetWindowText(strEdit);

			EnableControlWindow(FALSE);
			strComment.Format(_T("%s_%s"), pDevice->GetSystemName(), strEdit);
			pDevice->SetProjectName(pDevice->GetSystemName());
			pDevice->SetBackupName(strComment);
			pDevice->StartBackup();

			m_process = process_backup;
		}

		else
		{
			m_process = process_Initate;
		}

		nIndex++;

	}	break;

	case process_backup:
	{// 백업 동작중..
		BOOL bSetPos = FALSE;

		strState.Format(_T("[%s] : "), m_device.strName);
		strState += (pDevice->GetDetailStatus());
		strState.Remove('.');
		SetDlgItemText(IDC_ST_PROGRESS, strState);

		if (!(IsEnableBackupScheduleTime(pDevice)))
		{
			AddFailDevice(m_device);
			m_progress.SetPos(nIndex);

			SetDlgItemText(IDC_ST_PROGRESS, _T("Auto Backup Schecdule In %d Min", pDevice->GetAutoBackupLeftSeconds() / 60));
			m_process = process_Initate;
			bSetPos = TRUE;
		}

		else if (!(pDevice->IsMotiongateConnected()))
		{
			AddFailDevice(m_device);
			m_process = process_Initate;
			bSetPos = TRUE;
		}

		else if (!pDevice->GetBackupState())
		{
			m_process = process_done;
			bSetPos = TRUE;
		}

		if (bSetPos)
		{
			m_progress.SetPos(nIndex);
		}
		
	}	break;

	case process_done:
	{
		m_process = process_Initate;
	}	break;

	default:
		break;
	}


}

void CDlgSystemBackup::InitControlTextSize()
{
	if (true)
	{
		LOGFONT lf = { 0, };
		afxGlobalData.fontRegular.GetLogFont(&lf);

		lf.lfHeight = -15;
		m_BigFont.CreateFontIndirect(&lf);

		GetDlgItem(IDC_STATIC_COMMENT)->SetFont(&m_BigFont);
		GetDlgItem(IDC_EDIT_COMMENT)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_TIME_FILTER)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BUTTON_LIST_UPDATE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_LIST_BACKUP_ENABLE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_TEXT)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_PROGRESS)->SetFont(&m_BigFont);
		GetDlgItem(IDC_PROGRESS_BACKUP)->SetFont(&m_BigFont);
		GetDlgItem(IDC_LIST_BACKUP_DISABLE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BUTTON_BACKUP)->SetFont(&m_BigFont);

		m_BigFont.Detach();
	}

	/*if (true)
	{
		m_BigFont.CreateFont(FONT_SIZE, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, TEXT_STYLE);
		GetDlgItem(IDC_STATIC_COMMENT)->SetFont(&m_BigFont);
		GetDlgItem(IDC_EDIT_COMMENT)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_TIME_FILTER)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BUTTON_LIST_UPDATE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_LIST_BACKUP_ENABLE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_TEXT)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_PROGRESS)->SetFont(&m_BigFont);
		GetDlgItem(IDC_PROGRESS_BACKUP)->SetFont(&m_BigFont);
		GetDlgItem(IDC_LIST_BACKUP_DISABLE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BUTTON_BACKUP)->SetFont(&m_BigFont);

		m_BigFont.Detach();
	}*/

}
