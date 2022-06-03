// CDlgSystemHistory.cpp: 구현 파일
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CDlgSystemHistory.h"
#include "afxdialogex.h"

#include "CDeviceList.h"
#include "CDeviceMotionGate.h"

// CDlgSystemHistory 대화 상자
#define TIMER_SHOW 0

IMPLEMENT_DYNAMIC(CDlgSystemHistory, CDialogEx)

CDlgSystemHistory::CDlgSystemHistory(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SYSTEM_HISTORY, pParent)
{
	
}

CDlgSystemHistory::~CDlgSystemHistory()
{
	KillTimer(TIMER_SHOW);
}

void CDlgSystemHistory::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	/*DDX_Control(pDX, IDC_LIST_PROJECT, m_DeviceList);
	DDX_Control(pDX, IDC_LIST_LOG, m_LogList);*/
	DDX_Control(pDX, IDC_PROGRESS_STATUS, m_Progress);
}


BEGIN_MESSAGE_MAP(CDlgSystemHistory, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SHOW, &CDlgSystemHistory::OnBnClickedButtonShow)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_ALL, &CDlgSystemHistory::OnBnClickedButtonAll)
END_MESSAGE_MAP()


// CDlgSystemHistory 메시지 처리기


void CDlgSystemHistory::OnBnClickedButtonShow()
{
	m_pos = m_lstSystemLog.GetHeadPosition();
	m_LogList.RemoveAll();
	BOOL bCheck = FALSE;
	for (INT i = 0; i < m_DeviceList.GetItemCount(); i++)
	{
		if (m_DeviceList.GetCheck(i))
		{
			bCheck |= TRUE;
		}
	}

	if (bCheck)
	{
		m_Progress.SetRange(0, m_lstSystemLog.GetCount());
		EnableControl(FALSE);
		SetTimer(TIMER_SHOW, 10, NULL);
	}
	
}


BOOL CDlgSystemHistory::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_SYSTEM_HISTORY));
	this->SetIcon(hIcon, FALSE);

	InitControl();
	
	InitList();

	InitControlTextSize();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


const BOOL CDlgSystemHistory::SaveLogList(INT nLevel, CString strTime, CString strDeviceName, CString strDeviceIP, CString strInform, CTime time)
{
	OPERATIONLOG_INFORMATION inform;

	inform.nLevel = nLevel;
	inform.strTime = strTime;
	inform.strDeviceName = strDeviceName;
	inform.strDeviceIP = strDeviceIP;
	inform.strInform = strInform;

	m_lstSystemLog.AddTail(inform); // List에 저장시킨다.

	return 0;
}


void CDlgSystemHistory::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_SHOW:
	{
		ShowList();
	}	break;

	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CDlgSystemHistory::InitList()
{
	CDeviceList* pDeviceList = CDeviceList::GetInstance();
	CDeviceMotionGate* pDevice = NULL;
	INT iItem = 0;
	CString strAddress, strName;

	for (INT i = 0; i < pDeviceList->GetAddressCount(); i++)
	{
		const DWORD dwAddress = pDeviceList->GetAddressByIndex(i);
		pDeviceList->GetMotiongateByAddress(dwAddress, pDevice);
		
		if (pDevice->GetMotionGateConnectedStatus())
		{
			pDevice->GetAddress(strAddress);
			strName = pDevice->GetSystemName();
			iItem = m_DeviceList.GetItemCount();
			m_DeviceList.InsertItem(iItem, strAddress);
			m_DeviceList.SetItemText(iItem, 1, strName);
		}
	}

	//GetDlgItem(IDC_BUTTON_SHOW)->EnableWindow(iItem == 0 ? FALSE : TRUE);
}


void CDlgSystemHistory::InitControl()
{
	m_DeviceList.SubclassDlgItem(IDC_LIST_PROJECT, this);
	m_DeviceList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER | LVS_EX_CHECKBOXES);
	m_DeviceList.InsertColumn(0, TEXT("IP Address"));
	m_DeviceList.InsertColumn(1, TEXT("Tooling Name"));

	m_DeviceList.SetColumnWidth(0, 150);
	m_DeviceList.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

	//////////////////////////////////////////////////////////////////////////////

	m_LogList.SubclassDlgItem(IDC_LIST_LOG, this);
	m_LogList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
	m_LogList.InsertColumn(0, TEXT("Time Stamp"));
	m_LogList.InsertColumn(1, TEXT("Tooling"));
	m_LogList.InsertColumn(2, TEXT("Information"));

	m_LogList.SetColumnWidth(0, 130);
	m_LogList.SetColumnWidth(1, 120);
	m_LogList.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);

	//////////////////////////////////////////////////////////////////////////////

	m_pStandType = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_STANDARD_TYPE));
	m_pNameType = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_NAME_TYPE));
	m_pDataType = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_DATA_TYPE));
	
	m_pStandType->AddString(_T("ALL"));
	m_pStandType->AddString(_T("SELECT"));
	m_pStandType->SetCurSel(1);
	
	m_pDataType->AddString(_T("ALL"));
	m_pDataType->AddString(_T("WARNING"));
	m_pDataType->AddString(_T("ERROR"));
	m_pDataType->SetCurSel(0);

	m_pNameType->AddString(_T("NAME"));
	m_pNameType->AddString(_T("IP"));
	m_pNameType->AddString(_T("NAME + IP"));
	m_pNameType->SetCurSel(2);
}


void CDlgSystemHistory::ShowList()
{
	OPERATIONLOG_INFORMATION log;
	
	CString strDataType;
	CString strNameType;
	
	static INT nCount = 0;
	INT nIndex = 0;
	
	log = m_lstSystemLog.GetNext(m_pos);
	m_Progress.SetPos(++nCount);

	if (log.strInform == _T("Program Executed"))
	{// 어떤 장치의 선택이던지 첫 프로그램 시작 메세지는 현시한다.
		if (m_pDataType->GetCurSel() == 0)
		{// Normal일 경우..
			strNameType = log.strDeviceName;
			strDataType = log.strInform;

			m_LogList.InsertItem(nIndex, log.strTime);
			m_LogList.SetItemText(nIndex, 1, strNameType);
			m_LogList.SetItemText(nIndex, 2, strDataType);
		}

		return;
	}

	for (INT i = 0; i < m_DeviceList.GetItemCount(); i++)
	{
		if (m_DeviceList.GetCheck(i))
		{
			if (m_DeviceList.GetItemText(i,0) == log.strDeviceIP)
			{
				switch (m_pNameType->GetCurSel())
				{
				case 0: // Device Name
				{strNameType = log.strDeviceName; }	break;
				case 1: // Device IP
				{strNameType = log.strDeviceIP; }	break;
				case 2: // Merge
				{
					if (log.strDeviceName == L"" || log.strDeviceName == _T("Unknown."))
					{ // DeviceName이 존재하지 않는다면 System Name으로 현시해준다.
						strNameType = log.strDeviceIP;
					}
					else
					{
						strNameType = log.strDeviceName;
					}
					
				}	break;

				default:
					break;
				}
				
				switch (m_pDataType->GetCurSel())
				{
				case 0: // Normal
				{
					strDataType = log.strInform;
				}	break;
				case 1: // Warning
				{
					if (log.nLevel == STS_LOG_WARNING) strDataType = log.strInform;
				}	break;
				case 2: // Error
				{
					if (log.nLevel == STS_LOG_ERROR) strDataType = log.strInform;
				}	break;
				default:
				break;
				}
				
				if (strNameType != _T("") && strDataType != _T(""))
				{
					m_LogList.InsertItem(nIndex, log.strTime);
					m_LogList.SetItemText(nIndex, 1, strNameType);
					m_LogList.SetItemText(nIndex, 2, strDataType);
				}
			}
		}
	}

	if (m_pos == NULL)
	{
		if (true)
		{
			m_DeviceList.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
			m_DeviceList.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
			m_LogList.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
			m_LogList.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);
			m_LogList.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
		}
		
		nCount = 0;
		KillTimer(TIMER_SHOW);
		EnableControl(TRUE);
		return;
	}
}


void CDlgSystemHistory::EnableControl(BOOL bEnable)
{
	GetDlgItem(IDC_COMBO_DATA_TYPE)->EnableWindow(bEnable);
	GetDlgItem(IDC_COMBO_NAME_TYPE)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_ALL)->EnableWindow(bEnable);
	GetDlgItem(IDC_LIST_PROJECT)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_SHOW)->EnableWindow(bEnable);
}


void CDlgSystemHistory::OnBnClickedButtonAll()
{
	for (INT i = 0; i < m_DeviceList.GetItemCount(); i++) m_DeviceList.SetCheck(i);
}


void CDlgSystemHistory::InitControlTextSize()
{
	if (true)
	{
		LOGFONT lf = { 0, };
		afxGlobalData.fontRegular.GetLogFont(&lf);

		lf.lfHeight = -15;
		m_BigFont.CreateFontIndirect(&lf);

		GetDlgItem(IDC_BUTTON_ALL)->SetFont(&m_BigFont);
		GetDlgItem(IDC_LIST_PROJECT)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_NAME_TYPE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_COMBO_NAME_TYPE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_LOG_TYPE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_COMBO_DATA_TYPE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BUTTON_SHOW)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_PROGRESS)->SetFont(&m_BigFont);
		GetDlgItem(IDC_PROGRESS_STATUS)->SetFont(&m_BigFont);
		GetDlgItem(IDC_LIST_LOG)->SetFont(&m_BigFont);
		GetDlgItem(IDOK)->SetFont(&m_BigFont);

		m_BigFont.Detach();
	}

	/*if (true)
	{
		m_BigFont.CreateFont(FONT_SIZE, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, TEXT_STYLE);

		GetDlgItem(IDC_BUTTON_ALL)->SetFont(&m_BigFont);
		GetDlgItem(IDC_LIST_PROJECT)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_NAME_TYPE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_COMBO_NAME_TYPE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_LOG_TYPE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_COMBO_DATA_TYPE)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BUTTON_SHOW)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_PROGRESS)->SetFont(&m_BigFont);
		GetDlgItem(IDC_PROGRESS_STATUS)->SetFont(&m_BigFont);
		GetDlgItem(IDC_LIST_LOG)->SetFont(&m_BigFont);
		GetDlgItem(IDOK)->SetFont(&m_BigFont);

		m_BigFont.Detach();
	}*/
	
}
