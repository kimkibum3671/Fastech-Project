// CDlgNoticeSystem.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CDlgNoticeSystem.h"
#include "afxdialogex.h"


#include "CDataBaseManager.h"
#include "CDeviceList.h"
#include "CMFCButtonEx.h"

// CDlgNoticeSystem dialog

IMPLEMENT_DYNAMIC(CDlgNoticeSystem, CDialogEx)

CDlgNoticeSystem::CDlgNoticeSystem(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NOTICE_NEW_SYSTEM, pParent)
{
	m_bReload = FALSE;
	m_bStart = FALSE;
}

CDlgNoticeSystem::~CDlgNoticeSystem()
{
	DestroyWindow();
}

void CDlgNoticeSystem::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_BUTTON_TEST, m_btnTest);
	//DDX_Control(pDX, IDC_BUTTON_TEST, m_btnTest);
}


BEGIN_MESSAGE_MAP(CDlgNoticeSystem, CDialogEx)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, &CDlgNoticeSystem::OnLvnItemchangedList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, &CDlgNoticeSystem::OnNMDblclkList)
	ON_BN_CLICKED(IDOK, &CDlgNoticeSystem::OnBnClickedOk)
	ON_BN_CLICKED(IDC_MFCBUTTON_CONFIG, &CDlgNoticeSystem::OnBnClickedMfcbuttonConfig)
END_MESSAGE_MAP()


// CDlgNoticeSystem message handlers


BOOL CDlgNoticeSystem::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_dwSelectedDeviceAddress = 0;

	CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();


	if (m_wndList.SubclassDlgItem(IDC_LIST, this))
	{
		m_wndList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SINGLEROW | LVS_EX_DOUBLEBUFFER);

		INT nColumnIndex = 0;
		//m_wndList.InsertColumn(nColumnIndex++, _T("System Name"), 0, 150);
		m_wndList.InsertColumn(nColumnIndex++, _T("Tooling Name"), 0, 150);

		switch (pConfigDB->GetInterface())
		{
		case CDataBaseConfig::interface_TCP:
		case CDataBaseConfig::interface_UDP:
		{
			m_wndList.InsertColumn(nColumnIndex++, _T("IP Address"), 0, 100);
		}	break;

		case CDataBaseConfig::interface_Serial:
		{
			//	Multi serial port를 지원하면 그 때 고려해 볼 것
		}	break;
		}

		m_wndList.InsertColumn(nColumnIndex++, _T("New"), 0, 100);
		m_wndList.InsertColumn(nColumnIndex++, _T("Motor"), 0, 100);
		//m_wndList.InsertColumn(nColumnIndex++, _T("Homing"), 0, 100);

		SetTimer(TIMER_ADD_DEVICE, 100, NULL);
		
		m_bReload = TRUE;
	}

	m_wndText[INFORM_MOTOR_COUNT].SubclassDlgItem(IDC_STATIC_MOTOR_COUNT, this);
	m_wndText[INFORM_HOMING_TIMEOUT].SubclassDlgItem(IDC_STATIC_HOMING_TIME, this);

	InitControlTextSize();

	return TRUE;
}

const BOOL CDlgNoticeSystem::IsDeviceOK(const DWORD dwAddress)
{
	CDeviceMotionGate* pDevice = NULL;
	if (FALSE == CDeviceList::GetInstance()->GetMotiongateByAddress(dwAddress, pDevice))
	{
		return FALSE;
	}

	const BOOL bResult = pDevice->CompareMotiongateInfo(); // 정보 불일치 판단.

	if (TRUE == bResult)
	{ // 기존에 문제있던 디바이스가 정상화 작업이 진행되었다면 List에서 삭제한다.
		FilterRegisteredAddress(dwAddress);
	}

	if (FALSE == bResult && -1 == FindRegisteredAddress(dwAddress))
	{
		m_lstDevice.AddTail(dwAddress);

		m_bReload = TRUE;
	}

	return bResult;
}

const DWORD CDlgNoticeSystem::GetSelectedDeviceAddress()
{
	return m_dwSelectedDeviceAddress;
}

void CDlgNoticeSystem::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_ADD_DEVICE:
	{
		if (FALSE == m_bReload)
		{
			break;
		}

		INT nStartCount = m_wndList.GetItemCount();
		POSITION pos = m_lstDevice.GetHeadPosition();
		while (pos)
		{
			const DWORD dwAddress = m_lstDevice.GetNext(pos);
			if (nStartCount)
			{
				nStartCount--;
				continue;
			}

			if (true)
			{
				CDeviceMotionGate* pDevice = NULL;
				CDeviceList::GetInstance()->GetMotiongateByAddress(dwAddress, pDevice);
				if (pDevice->CompareMotiongateInfo())
				{
					FilterRegisteredAddress(dwAddress);
				}
				else
				{
					AddListItem(dwAddress);
				}
			}

		}

		SetImage();

		m_bReload = FALSE;
	}	break;
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CDlgNoticeSystem::OnDestroy()
{
	CDialogEx::OnDestroy();
}

const INT CDlgNoticeSystem::FindRegisteredAddress(const DWORD dwAddress)
{
	INT nResult = -1;

	INT nIndex = 0;
	POSITION pos = m_lstDevice.GetHeadPosition();
	while (pos)
	{
		if (m_lstDevice.GetNext(pos) == dwAddress)
		{
			nResult = nIndex;
			break;
		}

		nIndex++;
	}

	return nResult;
}

void CDlgNoticeSystem::AddListItem(const DWORD dwAddress)
{
	CDeviceMotionGate* pDevice = NULL;
	if (FALSE == CDeviceList::GetInstance()->GetMotiongateByAddress(dwAddress, pDevice) ||
		NULL == pDevice)
	{
		return;
	}

	CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();
	
	//const INT nItemRow = m_wndList.InsertItem(m_wndList.GetItemCount(), pDevice->GetSystemName());
	const INT nItemRow = m_wndList.InsertItem(m_wndList.GetItemCount(), pDevice->GetDatabaseSystemName());
	m_wndList.SetItemData(nItemRow, dwAddress);

	INT nItemCol = 1;

	CString strValue;

	switch (pConfigDB->GetInterface())
	{
	case CDataBaseConfig::interface_TCP:
	case CDataBaseConfig::interface_UDP:
	{
		pDevice->GetAddress(strValue);
		m_wndList.SetItemText(nItemRow, nItemCol++, strValue);
	}	break;

	case CDataBaseConfig::interface_Serial:
	{
		//	Multi serial port를 지원하면 그 때 고려해 볼 것s
	}	break;
	}

	INT nMotorCount = pDevice->GetMotorCount();
	switch (nMotorCount)
	{
	case -1:
	{	//	DB에는 이름이 등록되어있으나 Motiongate에서 Motor Count가 없는 경우
		nMotorCount = 0;
		nItemCol++;
	}	break;

	case 0:
	{	//	DB에 등록이 안되어있는 새 제품
		m_wndList.SetItemText(nItemRow, nItemCol++, _T("Yes"));
	}	break;

	default:
	{
		nItemCol++;
	}	break;
	}

	//	Motor Count
	m_wndList.SetItemText(nItemRow, nItemCol++, (nMotorCount == pDevice->GetDatabaseMotorCount()) ? _T("Pass") : _T("Error"));

	INT nHomingTimeout = pDevice->GetHomingTimeout();
	if (-1 == nHomingTimeout)
	{	//	DB에는 이름이 등록되어있으나 Motiongate에서 Motor Count가 없는 경우
		nHomingTimeout = 0;
	}

	//	Homing Timeout
	m_wndList.SetItemText(nItemRow, nItemCol++, (nHomingTimeout == pDevice->GetDatabaseHomingTimeout()) ? _T("Pass") : _T("Error"));
}


void CDlgNoticeSystem::OnLvnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	*pResult = 0;

	POSITION pos = m_wndList.GetFirstSelectedItemPosition();

	ReportSelectedDeviceInfo(pos ? m_wndList.GetNextSelectedItem(pos) : -1);
}

void CDlgNoticeSystem::ReportSelectedDeviceInfo(const INT nIndex)
{
	BOOL bEnable = FALSE;
	CStatic* pStatic = static_cast<CStatic*>(GetDlgItem(IDC_ST_INFORMATION));
	CString strInformation, strIP,
		strDeviceCount(_T("- ")),
		strDeviceHoming(_T("- ")),
		strDatabaseCount(_T("- ")),
		strDatabaseHoming(_T("- ")),
		strSystemName(_T("Configuration"));

	COLORREF colorTxtMotorCnt = RGB(0x00, 0x00, 0x00);
	COLORREF colorTxtHomingTime = RGB(0x00, 0x00, 0x00);

	if (-1 != nIndex)
	{
		const DWORD dwAddress = m_wndList.GetItemData(nIndex);

		CDeviceMotionGate* pDevice = NULL;
		if (FALSE == CDeviceList::GetInstance()->GetMotiongateByAddress(dwAddress, pDevice) ||
			NULL == pDevice)
		{
			//	들어올 수 없는 영역
			return;
		}
		pDevice->GetAddress(strIP);


		//CString strName = pDevice->GetDatabaseSystemName();
		strInformation.Format(_T("Selected Tooling IP : %s"), strIP);

		//strDeviceCount.Format(_T("%d"), pDevice->GetMotorCount()); 기존..
		strDeviceCount.Format(_T("%d"), pDevice->GetMotorCount());
		strDeviceHoming.Format(_T("%d"), pDevice->GetHomingTimeout());
		strDatabaseHoming.Format(_T("%d"), pDevice->GetDatabaseHomingTimeout());
		
		if (pDevice->GetDatabaseMotorCount() == -1) strDatabaseCount.Format(_T("NEW"));
		else strDatabaseCount.Format(_T("%d"), pDevice->GetDatabaseMotorCount());

		//strSystemName.Format(_T("[ %s - %s ] Configuration"), pDevice->GetSystemName(), strIP); 기존..
		strSystemName.Format(_T("[ IP : %s ] Configuration"), strIP);

		m_dwSelectedDeviceAddress = dwAddress;

		if (strDeviceCount != strDatabaseCount)
		{
			colorTxtMotorCnt = RGB(0xff, 0, 0);
		}

		if (strDeviceHoming != strDatabaseHoming)
		{
			colorTxtHomingTime = RGB(0xff, 0, 0);
		}

		bEnable = TRUE;
	}
	else
	{
		strInformation.Format(_T("%d invalid system found."), m_wndList.GetItemCount());

		m_dwSelectedDeviceAddress = 0;
		bEnable = FALSE;
	}

	m_wndText[INFORM_MOTOR_COUNT].SetTextColor(colorTxtMotorCnt);
	m_wndText[INFORM_HOMING_TIMEOUT].SetTextColor(colorTxtHomingTime);

	CString strTemp;
	CWnd* pWnd = NULL;

	pWnd = GetDlgItem(IDC_MFCBUTTON_CONFIG);
	pWnd->SetWindowText(strSystemName);
	pWnd->EnableWindow(bEnable);

	pWnd = GetDlgItem(IDC_ST_INFORMATION);
	pWnd->GetWindowText(strTemp);
	if (strTemp != strInformation)
	{
		pWnd->SetWindowText(strInformation);
	}

	pWnd = GetDlgItem(IDC_ST_DEVICE_COUNT);
	pWnd->GetWindowText(strTemp);
	if (strTemp != strDeviceCount)
	{
		pWnd->SetWindowText(strDeviceCount);
	}

	pWnd = GetDlgItem(IDC_ST_DEVICE_HOMING);
	pWnd->GetWindowText(strTemp);
	if (strTemp != strDeviceHoming)
	{
		pWnd->SetWindowText(strDeviceHoming);
	}

	pWnd = GetDlgItem(IDC_ST_DATABASE_COUNT);
	pWnd->GetWindowText(strTemp);
	if (strTemp != strInformation)
	{
		pWnd->SetWindowText(strDatabaseCount);
	}

	pWnd = GetDlgItem(IDC_ST_DATABASE_HOMING);
	pWnd->GetWindowText(strTemp);
	if (strTemp != strInformation)
	{
		pWnd->SetWindowText(strDatabaseHoming);
	}
}


void CDlgNoticeSystem::OnNMDblclkList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	m_dwSelectedDeviceAddress = 0;

	POSITION pos = m_wndList.GetFirstSelectedItemPosition();
	if (pos)
	{
		const INT nItem = m_wndList.GetNextSelectedItem(pos);
		const DWORD dwAddress = m_wndList.GetItemData(nItem);

		m_dwSelectedDeviceAddress = dwAddress;

		OnOK();
	}
}


void CDlgNoticeSystem::SetImage()
{
	// IDC_STATIC_IMAGE

	CRect rect;
	CImage image;
	CDC* dc;
	CStatic* pToggleBtn = (CStatic*)GetDlgItem(IDC_ST_WARNING_CONFIG);
	dc = pToggleBtn->GetDC();
	pToggleBtn->GetWindowRect(rect);
	image.Load(L"res/TotalConfig/Config.bmp");
	image.StretchBlt(dc->m_hDC, 0, 0, rect.Width(), rect.Height(), SRCCOPY);
	ReleaseDC(dc);

}


void CDlgNoticeSystem::FilterRegisteredAddress(const DWORD dwAddress)
{
	POSITION pos = m_lstDevice.GetHeadPosition();

	while (pos != NULL)
	{
		POSITION posTemp = pos;
		if (m_lstDevice.GetNext(pos) == dwAddress)
		{
			m_lstDevice.RemoveAt(posTemp);
			break;
		}
	}
}


void CDlgNoticeSystem::SetButtonImage(INT nID, CString strFilePath, CString strBtnText)
{
	HRESULT hRes;
	HBITMAP hBit;
	CImage image;
	CMFCButton* pButton;

	hRes = image.Load(L"res/TotalConfig/backup.png");
	if (hRes != S_OK)
	{
		ASSERT(0);
	}
	hBit = image.Detach();

	pButton = (CMFCButton*)GetDlgItem(IDC_MFCBUTTON_CONFIG);
	pButton->m_nFlatStyle = CMFCButton::BUTTONSTYLE_3D;
	pButton->m_bTransparent = TRUE;
	pButton->m_bDrawFocus = FALSE;

	pButton->SetImage(hBit, TRUE);
	pButton->SetWindowText(L"Configuration");
}


//BOOL CDlgNoticeSystem::OnCommand(WPARAM wParam, LPARAM lParam)
//{
//	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
//	UINT id = (UINT)LOWORD(wParam);
//
//	switch (id)
//	{
//	case IDC_MFCBUTTON_CONFIG:
//	{
//		if ( 0 != GetSelectedDeviceAddress())
//		{
//			OnOK();
//		}
//	}	break;
//	default:
//		break;
//	}
//	
//	//IDC_MFCBUTTON_CONFIG
//	//return CDialogEx::OnCommand(wParam, lParam);
//	return TRUE;
//}


void CDlgNoticeSystem::OnBnClickedOk()
{
	CDialogEx::OnCancel();
}


void CDlgNoticeSystem::OnBnClickedMfcbuttonConfig()
{
	if (0 != GetSelectedDeviceAddress())
	{
		//FilterRegisteredAddress(GetSelectedDeviceAddress());
		OnOK();
	}

}


int CDlgNoticeSystem::GetListCount()
{
	return m_lstDevice.GetCount();
}


const BOOL CDlgNoticeSystem::IsErrorDeviceExist()
{
	POSITION pos = m_lstDevice.GetHeadPosition();
	while (pos)
	{
		const DWORD dwAddress = m_lstDevice.GetNext(pos);

		if (true)
		{
			CDeviceMotionGate* pDevice = NULL;
			CDeviceList::GetInstance()->GetMotiongateByAddress(dwAddress, pDevice);
			if (pDevice->CompareMotiongateInfo())
			{
				FilterRegisteredAddress(dwAddress);
			}

		}

	}

	return m_lstDevice.GetCount() == 0 ? FALSE : TRUE;
}


void CDlgNoticeSystem::InitControlTextSize()
{
	LOGFONT lf = { 0, };
	afxGlobalData.fontRegular.GetLogFont(&lf);

	lf.lfHeight = -15;
	m_BigFont.CreateFontIndirect(&lf);

	GetDlgItem(IDC_STATIC_WARNING)->SetFont(&m_BigFont);
	GetDlgItem(IDC_LIST)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_INFORMATION)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_MOTOR_COUNT)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_MEMORY_MOTIONGATE)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_MEMORY_DB)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_DEVICE_COUNT)->SetFont(&m_BigFont);
	GetDlgItem(IDC_ST_DATABASE_COUNT)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_CONFIG_INFORMATION)->SetFont(&m_BigFont);
	GetDlgItem(IDC_MFCBUTTON_CONFIG)->SetFont(&m_BigFont);
	GetDlgItem(IDOK)->SetFont(&m_BigFont);

	m_BigFont.Detach();
	
}
