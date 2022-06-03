// CDlgNetworkSearch.cpp: 구현 파일
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CDlgNetworkSearch.h"
#include "afxdialogex.h"
#include "CDeviceMotionGate.h"
#include "CInterfaceManager.h"
#include "CPageConfigNetworkGeneral.h"
#include <ctime>
#include "..\Common_Define\Define_ReturnCode.h"
// CDlgNetworkSearch 대화 상자

#define TIMER_SEARCH 1
#define TIMER_SEARCHING 2

IMPLEMENT_DYNAMIC(CDlgNetworkSearch, CDialogEx)

CDlgNetworkSearch::CDlgNetworkSearch(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NETWORK_SEARCH, pParent)
{
	m_pParent = pParent;
	m_bStop = FALSE;
	m_bOutlookSearch = FALSE;
}

CDlgNetworkSearch::~CDlgNetworkSearch()
{
}

void CDlgNetworkSearch::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_NETWORK, m_Progress);
}


BEGIN_MESSAGE_MAP(CDlgNetworkSearch, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CDlgNetworkSearch::OnBnClickedButtonSearch)
	ON_BN_CLICKED(IDC_BUTTON_OK, &CDlgNetworkSearch::OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CDlgNetworkSearch::OnBnClickedButtonCancel)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CDlgNetworkSearch::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_RUN, &CDlgNetworkSearch::OnBnClickedButtonRun)
	ON_BN_CLICKED(IDC_CHECK_ALL, &CDlgNetworkSearch::OnBnClickedCheckAll)
	ON_BN_CLICKED(IDC_BUTTON_ALL, &CDlgNetworkSearch::OnBnClickedButtonAll)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS_START, &CDlgNetworkSearch::OnIpnFieldchangedIpaddressStart)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS_END, &CDlgNetworkSearch::OnIpnFieldchangedIpaddressEnd)
END_MESSAGE_MAP()


// CDlgNetworkSearch 메시지 처리기


BOOL CDlgNetworkSearch::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (1)
	{
		// List Control 작업

		CRect rect;
		INT iItem = 0;
		m_listCtrl.SubclassDlgItem(IDC_LIST_NETWORK, this);
		m_listCtrl.GetWindowRect(&rect);
		m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER | LVS_EX_CHECKBOXES);
		 
		/*
		SetExtendedStyle : List Control의 확장 스타일을 나타낸다.

		LVS_EX_FULLROWSELECT : 아이템 클릭시 포커스 표시.
		LVS_EX_GRIDLINES : 아이템 경계선 표시
		LVS_EX_DOUBLEBUFFER : 더블버퍼링(깜빡임 방지) 활성화
		*/


		m_listCtrl.InsertColumn(0, TEXT("[Check] IP Address"));
		m_listCtrl.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
			
	}

	m_nID = 0;

	if (!m_bOutlookSearch)
	{ // Ribbon 설정창의 Network 검색 화면에서 들어온경우..
		GetDlgItem(IDC_IPADDRESS_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_IPADDRESS_END)->EnableWindow(FALSE);
		ShowButton(FALSE);
		SetIPControl();
		SetTimer(TIMER_SEARCHING, 500, NULL);
	}

	InitControlTextSize();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


const INT CDlgNetworkSearch::ConnectDevice(DWORD dwAddress)
{
	INT nFasResult = FMM_OK;
	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->Connect(dwAddress, 10002))
	{
		nFasResult = pInterface->GetLastError();
	}

	return nFasResult;
}


const INT CDlgNetworkSearch::GetMotionGateInform(DWORD dwAddress, MOTIONGATE_READ_INFO& info)
{
	MOTIONGATE_READ_INFO stMotiongate;
	const INT cbBuffer = sizeof(MOTIONGATE_READ_INFO);
	::ZeroMemory(&stMotiongate, cbBuffer);

	INT nFasResult = FMM_OK;
	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->GetMotiongateInfo(dwAddress, &info, cbBuffer))
	{
		nFasResult = pInterface->GetLastError();
	}
	
	return nFasResult;
}


const BOOL CDlgNetworkSearch::SearchingDeviceInform(DWORD dwAddress)
{
	BOOL bResult = FALSE;
	INT item = 0;
	NETWORK_INFORM stMotiongate;

	if (FMM_OK == ConnectDevice(dwAddress))
	{
		Sleep(1);

		if (FMM_OK == GetMotionGateInform(dwAddress, stMotiongate.info))
		{
			Sleep(1);

			if (FMM_OK == CloseDevice(dwAddress))
			{// List Control에만 List를 등록하면 되기에 바로 Close 진행한다.
				stMotiongate.dwAddress = dwAddress;
				m_list.AddTail(stMotiongate);
				bResult = TRUE;
			}
		}

		else
		{
			CloseDevice(dwAddress);
			bResult = FALSE;
		}
	}

	return bResult;
}

const DWORD CDlgNetworkSearch::GetAddress(CString lpszAddress)
{
	CString strValue(lpszAddress);

	const BYTE octet1 = _ttoi(strValue);
	strValue.Delete(0, strValue.Find(_T('.')) + 1);
	const BYTE octet2 = _ttoi(strValue);
	strValue.Delete(0, strValue.Find(_T('.')) + 1);
	const BYTE octet3 = _ttoi(strValue);
	strValue.Delete(0, strValue.Find(_T('.')) + 1);
	const BYTE octet4 = _ttoi(strValue);
	

	return static_cast<DWORD>(MAKELONG(MAKEWORD(octet4, octet3), MAKEWORD(octet2, octet1)));
}


void CDlgNetworkSearch::OnBnClickedButtonSearch()
{
	/*ShowButton(FALSE);
	SearchingNetwork();
	ShowButton(TRUE);*/
}


const INT CDlgNetworkSearch::CloseDevice(DWORD dwAddress)
{
	INT nFasResult = FMM_OK;
	CInterface* pInterface = CInterfaceManager::GetInterface();
	if (FALSE == pInterface->Close(dwAddress))
	{
		nFasResult = pInterface->GetLastError();
	}

	return nFasResult;
}


CString CDlgNetworkSearch::ConvertSystemName(MOTIONGATE_READ_INFO info)
{
	CString strName;
	const INT nStrLength = 20;
	CHAR chText[nStrLength + 1];	memcpy(chText, info.chMotiongateName, nStrLength);
	chText[nStrLength] = '\0';

	strName = CStringA(info.chMotiongateName);

	return strName;
}


CString CDlgNetworkSearch::GetAddressString(DWORD dwAddress)
{
	CString strAddress;

	strAddress.Format(_T("%03d.%03d.%03d.%03d"),
		HIBYTE(HIWORD(dwAddress)),
		LOBYTE(HIWORD(dwAddress)),
		HIBYTE(LOWORD(dwAddress)),
		LOBYTE(LOWORD(dwAddress)));

	return strAddress;
}


void CDlgNetworkSearch::OnBnClickedButtonOk()
{
	if (IsOutlookSearch())
	{ // Outlook Device에서 IP Searching 기능 호출할때...
		static INT nAdd = 0;
		CDeviceList* pDeviceList = CDeviceList::GetInstance();
		INT nListCnt = pDeviceList->GetAddressCount();
		// GetAddress
		for (INT nIndex = 0; nIndex < m_listCtrl.GetItemCount(); nIndex++)
		{
			if (m_listCtrl.GetCheck(nIndex))
			{
				DWORD dwAddress = GetAddress(m_listCtrl.GetItemText(nIndex, 0));

				if (!IsIPOverlap(dwAddress))
				{ 
					CString str = m_listCtrl.GetItemText(nIndex, 0);
					pDeviceList->AddAddress(nListCnt + nAdd, dwAddress);
					nAdd++;
				}
			}
		}
		pDeviceList->RebuildAddressIndex(TRUE); // List에 디바이스 재정리한다. 새로운것 추가하고.. 없는것 삭제하고..
		nAdd = 0;
	}
	else
	{ // System Configuration 화면에서 IP Searching 기능 호출할때...
		CPageConfigNetworkGeneral* pParent = (CPageConfigNetworkGeneral*)m_pParent;

		ASSERT(pParent);

		for (INT i = 0; i < m_listCtrl.GetItemCount(); i++)
		{
			if (m_listCtrl.GetCheck(i))
			{
				pParent->SetNetworkSearchingAddress(m_listCtrl.GetItemText(i, 0)); // List 
			}
		}
	}
	
	CDialogEx::OnOK();
}


void CDlgNetworkSearch::OnBnClickedButtonCancel()
{
	CDialogEx::OnCancel();
}

const BOOL CDlgNetworkSearch::CheckState(INT nIndex)
{ // 사용X
	CString str = m_listCtrl.GetItemText(nIndex, 0);
	UINT state = m_listCtrl.GetItemState(nIndex, LVIS_STATEIMAGEMASK);
	BOOL bCheck = (BOOL)((state >> 12) & 0x01);

	return bCheck;
}


void CDlgNetworkSearch::ShowButton(BOOL bEnable)
{
	GetDlgItem(IDC_BUTTON_OK)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_CANCEL)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_RUN)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(!bEnable);
	GetDlgItem(IDC_BUTTON_SEARCH)->EnableWindow(bEnable);
	
	GetDlgItem(IDC_LIST_NETWORK)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_ALL)->EnableWindow(bEnable);
}


BOOL CDlgNetworkSearch::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UINT id = (UINT)LOWORD(wParam);

	switch (id)
	{
	case IDC_BUTTON_SEARCH:
	{
		ShowButton(FALSE);
	} break;

	default:
		break;
	}

	return CDialogEx::OnCommand(wParam, lParam);
}


void CDlgNetworkSearch::SetStartAddress(CString strAddress)
{
	m_strStartAddress = strAddress;
}


void CDlgNetworkSearch::SetEndAddress(CString strAddress)
{
	m_strEndAddress = strAddress;
}


void CDlgNetworkSearch::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_SEARCH:
	{
		//SearchingNetwork();
		KillTimer(TIMER_SEARCH);
	}	break;
		
	case TIMER_SEARCHING:
	{
		SearchingNetwork(IsSearchingStop());
	}	break;

	}

	CDialogEx::OnTimer(nIDEvent);
}


void CDlgNetworkSearch::SetIPControl()
{
	if (1)
	{
		CIPAddressCtrl* pCtrl = (CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_START);

		CString strValue(m_strStartAddress);

		const BYTE octet1 = _ttoi(strValue);
		strValue.Delete(0, strValue.Find(_T('.')) + 1);
		const BYTE octet2 = _ttoi(strValue);
		strValue.Delete(0, strValue.Find(_T('.')) + 1);
		const BYTE octet3 = _ttoi(strValue);
		strValue.Delete(0, strValue.Find(_T('.')) + 1);
		const BYTE octet4 = _ttoi(strValue);

		pCtrl->SetAddress(octet1, octet2, octet3, octet4);
	}

	if (1)
	{
		CIPAddressCtrl* pCtrl = (CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_END);

		CString strValue(m_strEndAddress);

		const BYTE octet1 = _ttoi(strValue);
		strValue.Delete(0, strValue.Find(_T('.')) + 1);
		const BYTE octet2 = _ttoi(strValue);
		strValue.Delete(0, strValue.Find(_T('.')) + 1);
		const BYTE octet3 = _ttoi(strValue);
		strValue.Delete(0, strValue.Find(_T('.')) + 1);
		const BYTE octet4 = _ttoi(strValue);

		pCtrl->SetAddress(octet1, octet2, octet3, octet4);
	}
}


void CDlgNetworkSearch::OnBnClickedButtonStop()
{
	m_bStop = TRUE;
}


const BOOL CDlgNetworkSearch::SearchingNetwork(BOOL bStop)
{

	clock_t delay = 5 * 100;

	GetDlgItemText(IDC_IPADDRESS_START, m_strStartAddress);
	GetDlgItemText(IDC_IPADDRESS_END, m_strEndAddress);

	DWORD dwStartAddress = GetAddress(m_strStartAddress);
	DWORD dwEndAddress = GetAddress(m_strEndAddress);
	INT nRange = dwEndAddress - dwStartAddress; // 탐색 개수를 구한다.(마지막주소 - 시작주소)

	if (bStop || (m_nID > nRange))
	{// 강제 정지 명령이 확인되었거나, 탐색 범위만틈 검색이 진행되었을 경우
		KillTimer(TIMER_SEARCHING);
		if (m_nID > nRange) m_nID = 0; 

		SetDlgItemText(IDC_STATIC_INFORM, m_bStop ? _T("Searching Stop.") : _T("Searching Complete."));

		ShowButton(TRUE);

		return TRUE;
	}

	if (1)
	{
		m_Progress.SetRange(0, nRange);

		Sleep(10);

		m_Progress.SetPos(m_nID + 1);

		SearchingDeviceInform(dwStartAddress + m_nID);
		SetDlgItemText(IDC_STATIC_INFORM, _T("Network IP : [") + GetAddressString(dwStartAddress + m_nID) + _T("]  Searching..."));
		
		if (1)
		{
			m_listCtrl.RemoveAll();

			NETWORK_INFORM netInfo;
			POSITION pos = m_list.GetHeadPosition();
			INT iItem = 0;

			while (pos)
			{
				netInfo = m_list.GetNext(pos);

				iItem = m_listCtrl.InsertItem(iItem, GetAddressString(netInfo.dwAddress));
				m_listCtrl.SetItemText(iItem, 1, CString(netInfo.info.chMotiongateName));

				iItem++;
			}

		}

		m_nID++;
	}

	return TRUE;
}

const BOOL CDlgNetworkSearch::IsSearchingStop()
{
	return m_bStop;
}


void CDlgNetworkSearch::OnBnClickedButtonRun()
{
	m_bStop = FALSE;

	// 만약 탐색 완료된 상태에서 다시 Run 할경우 리시트를 지우고 다시 시작한다.
	if (0 == m_nID)
	{
		m_list.RemoveAll();
		m_listCtrl.RemoveAll();
	}

	ShowButton(FALSE);

	if (IsOutlookSearch())
	{
		GetDlgItem(IDC_IPADDRESS_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_IPADDRESS_END)->EnableWindow(FALSE);
	}
	SetTimer(TIMER_SEARCHING, 500, NULL);
}


void CDlgNetworkSearch::OnBnClickedCheckAll()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK_ALL);

	if (pBtn->GetState())
	{
		for (INT i = 0; i < m_listCtrl.GetItemCount(); i++)
		{
			m_listCtrl.SetCheck(i);
		}
	}
}


void CDlgNetworkSearch::SetOutlookSearch(BOOL bOutlook)
{
	m_bOutlookSearch = bOutlook;
}


const BOOL CDlgNetworkSearch::IsOutlookSearch()
{
	return m_bOutlookSearch;
}


const BOOL CDlgNetworkSearch::IsIPOverlap(DWORD dwAddress)
{
	BOOL bOverLap = FALSE;
	CDeviceList* pDeviceList = CDeviceList::GetInstance();

	for (INT i = 0; i < pDeviceList->GetAddressCount(); i++)
	{
		const DWORD dwListAddress = pDeviceList->GetAddressByIndex(i);
		if (dwListAddress == dwAddress) bOverLap = TRUE;
	}
	
	return bOverLap;
}


void CDlgNetworkSearch::OnBnClickedButtonAll()
{
	for (INT i = 0; i < m_listCtrl.GetItemCount(); i++)
	{
		m_listCtrl.SetCheck(i);
	}
}


void CDlgNetworkSearch::OnIpnFieldchangedIpaddressStart(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	*pResult = 0;

	BYTE ipFirst, ipSecond, ipThird, ipForth;

	CIPAddressCtrl* pCtrl = (CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_END);

	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_START))->GetAddress(ipFirst, ipSecond, ipThird, ipForth);

	if (0 != ipFirst)
	{
		if (0 != ipSecond)
		{
			pCtrl->SetAddress(ipFirst, ipSecond, ipThird, ipForth + 1);
			GetDlgItem(IDC_BUTTON_RUN)->EnableWindow(TRUE);
			return;
		}

		pCtrl->SetAddress(ipFirst, 0, 0, 0);
		GetDlgItem(IDC_BUTTON_RUN)->EnableWindow(FALSE);
		return;
	}
}


void CDlgNetworkSearch::OnIpnFieldchangedIpaddressEnd(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	
	*pResult = 0;

	BYTE ipFirst, ipSecond, ipThird, ipForth;
	BYTE ipStandFirst, ipStandSecond, ipStandThird, ipStandForth;
	BOOL bEnable = FALSE;
	
	//CButton* pBtn = (CButton*)GetDlgItem(IDC_BUTTON_RUN);
	CIPAddressCtrl* pCtrl = (CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_START);
	
	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_END))->GetAddress(ipFirst, ipSecond, ipThird, ipForth);
	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_START))->GetAddress(ipStandFirst, ipStandSecond, ipStandThird, ipStandForth);

	if (ipFirst == 0 || ipSecond == 0 || ipForth == 0) bEnable = FALSE;

	if (ipFirst == ipStandFirst)
	{
		if (ipSecond == ipStandSecond)
		{
			if (ipThird == ipStandThird)
			{
				if (!(ipForth < ipStandForth))
				{
					bEnable = TRUE;
				}
			}
		}
	}

	GetDlgItem(IDC_BUTTON_RUN)->EnableWindow(bEnable);

}


void CDlgNetworkSearch::InitControlTextSize()
{
	LOGFONT lf = { 0, };
	afxGlobalData.fontRegular.GetLogFont(&lf);

	lf.lfHeight = -15;
	m_BigFont.CreateFontIndirect(&lf);

	GetDlgItem(IDC_IPADDRESS_START)->SetFont(&m_BigFont);
	GetDlgItem(IDC_IPADDRESS_END)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_WAVE)->SetFont(&m_BigFont);
	GetDlgItem(IDC_BUTTON_RUN)->SetFont(&m_BigFont);
	GetDlgItem(IDC_BUTTON_STOP)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_INFORM)->SetFont(&m_BigFont);
	GetDlgItem(IDC_PROGRESS_NETWORK)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_LIST)->SetFont(&m_BigFont);
	GetDlgItem(IDC_BUTTON_ALL)->SetFont(&m_BigFont);
	GetDlgItem(IDC_LIST_NETWORK)->SetFont(&m_BigFont);
	GetDlgItem(IDC_BUTTON_OK)->SetFont(&m_BigFont);
	GetDlgItem(IDC_BUTTON_CANCEL)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_SEARCHING)->SetFont(&m_BigFont);
	
	m_BigFont.Detach();

}
