// CPageConfigNetworkGeneral.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageConfigNetworkGeneral.h"
#include "afxdialogex.h"

#include "CDataBaseManager.h"

#include "CDeviceList.h"

#include "CDlgNetworkSearch.h"

// CPageConfigNetworkGeneral dialog

IMPLEMENT_DYNAMIC(CPageConfigNetworkGeneral, CMFCPropertyPage)

CPageConfigNetworkGeneral::CPageConfigNetworkGeneral(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPage(IDP_CONFIG_NETWORK_GENERAL)
{
	/*for (int i = 0; i < MAX_IP; i++)
	{
		m_protocol[i].pSock = NULL;
		m_protocol[i].bConnect = false;
		m_protocol[i].bInitial = false;
		m_protocol[i].nRi = 0;
		m_protocol[i].nConCycle = 0;
		ZeroMemory(m_protocol[i].sRbuf, SIZE_SOCKRECVBUFFER);
		ZeroMemory(m_protocol[i].sSbuf, SIZE_SOCKSENDBUFFER);
	}*/
}

CPageConfigNetworkGeneral::~CPageConfigNetworkGeneral()
{
}

void CPageConfigNetworkGeneral::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageConfigNetworkGeneral, CMFCPropertyPage)
	ON_WM_DESTROY()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_NETWORK, &CPageConfigNetworkGeneral::OnLvnItemchangedListNetwork)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_NETWORK, &CPageConfigNetworkGeneral::OnLvnEndlabeleditListNetwork)
	ON_BN_CLICKED(IDC_BTN_ADD, &CPageConfigNetworkGeneral::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_MOD, &CPageConfigNetworkGeneral::OnBnClickedBtnMod)
	ON_BN_CLICKED(IDC_BTN_DEL, &CPageConfigNetworkGeneral::OnBnClickedBtnDel)
	ON_BN_CLICKED(IDC_BTN_UP, &CPageConfigNetworkGeneral::OnBnClickedBtnUp)
	ON_BN_CLICKED(IDC_BTN_DOWN, &CPageConfigNetworkGeneral::OnBnClickedBtnDown)
	ON_BN_CLICKED(IDC_BTN_MOD_ID, &CPageConfigNetworkGeneral::OnBnClickedBtnModDevID)
	ON_BN_CLICKED(IDC_BUTTON_SEARCHING, &CPageConfigNetworkGeneral::OnBnClickedButtonSearching)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS_START, &CPageConfigNetworkGeneral::OnIpnFieldchangedIpaddressStart)
	ON_EN_CHANGE(IDC_EDIT_OCTET3, &CPageConfigNetworkGeneral::OnEnChangeEditOctet3)
END_MESSAGE_MAP()


// CPageConfigNetworkGeneral message handlers


BOOL CPageConfigNetworkGeneral::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	if (1)	// Initialize ListCtrl
	{
		m_listCtrl.SubclassDlgItem(IDC_LIST_NETWORK, this);
		m_listCtrl.ModifyStyle(0, LVS_SINGLESEL);
		m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
		HRESULT result = ::SetWindowTheme(m_listCtrl.m_hWnd, _T("Explorer"), NULL);

		m_listCtrl.InsertColumn(0, _T("IP Address"));
		//m_listCtrl.InsertColumn(1, _T("Device ID"));
		
		//	Load HeadSize
		INT nWidth1, nWidth2;
		m_listCtrl.SetColumnWidth(0, LVSCW_AUTOSIZE);
		nWidth1 = m_listCtrl.GetColumnWidth(0);
		m_listCtrl.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
		nWidth2 = m_listCtrl.GetColumnWidth(0);
		m_listCtrl.SetColumnWidth(0, max(nWidth1, nWidth2) - 30);
		

		CDataBaseProject* pProjectDB = CDataBaseProject::GetInstance();

		CDeviceList* pDeviceList = CDeviceList::GetInstance();
		const INT nCount = pDeviceList->GetAddressCount(TRUE);
		for (INT nIndex = 0; nIndex < nCount; nIndex++)
		{
			const DWORD dwAddress = pDeviceList->GetAddressByIndex(nIndex, TRUE);
			CString strAddress;
			pProjectDB->GetStringAddress(strAddress, dwAddress);
			m_listCtrl.InsertItem(nIndex, strAddress);
			m_listCtrl.SetItemData(nIndex, dwAddress);
		}

		m_listCtrl.LoadHeaderItemInfo();
	}	

	if (1)
	{
		
		bool bSystemRun = CDeviceList::GetInstance()->IsStartMonitoring();
		
		GetDlgItem(IDC_BUTTON_SEARCHING)->EnableWindow(FALSE);
		GetDlgItem(IDC_IPADDRESS_END)->EnableWindow(FALSE);
		GetDlgItem(IDC_IPADDRESS_START)->EnableWindow(!bSystemRun);
		GetDlgItem(IDC_EDIT_OCTET3)->EnableWindow(!bSystemRun);
		
		m_stColor.SubclassDlgItem(IDC_STATIC_ALARM, this);
		m_stColor.SetWindowText(_T("※ If you want to use IP searching operation, please stop system monitoring."));
		m_stColor.SetTextColor(RGB(255, 0, 0));
		m_stColor.SetBkColor(RGB(255, 255, 255));
		m_stColor.SetBlinkTextColors(RGB(255, 0, 0), RGB(0, 0, 255));
		m_stColor.StartTextBlink(TRUE, CColorStaticST::ST_FLS_FAST);
		m_stColor.ShowWindow(bSystemRun);
		/*if (bSystemRun)
		{
			m_stColor.SubclassDlgItem(IDC_STATIC_ALARM, this);
			m_stColor.SetTextColor(RGB(255, 0, 0));
			m_stColor.SetBlinkTextColors(RGB(255, 0, 0), RGB(255, 255, 255));
			m_stColor.StartTextBlink(TRUE, CColorStaticST::ST_FLS_NORMAL);
		}
		else
		{
			SetDlgItemText(IDC_STATIC_ALARM, _T(""));
		}*/


		//SetDlgItemText(IDC_STATIC_ALARM, bSystemRun ? _T("※ If you want to use IP searching operation, please stop system monitoring") : _T(""));

		SetDlgItemText(IDC_EDIT_IP, _T("        .       .       ."));
	}

	InitControlTextSize();

	return TRUE;
}


void CPageConfigNetworkGeneral::OnDestroy()
{
	m_listCtrl.SaveHeaderItemInfo();

	CMFCPropertyPage::OnDestroy();
}


void CPageConfigNetworkGeneral::OnLvnItemchangedListNetwork(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	*pResult = 0;
	 
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	const INT nIndex = pos ? m_listCtrl.GetNextSelectedItem(pos) : -1;

	if (1)
	{
		const BOOL bEnable = ( -1 != nIndex );
		GetDlgItem(IDC_BTN_MOD)->EnableWindow(bEnable);
		GetDlgItem(IDC_BTN_DEL)->EnableWindow(bEnable);
	}

	if (1)
	{
		BOOL bEnable = ( 0 < nIndex );
		GetDlgItem(IDC_BTN_UP)->EnableWindow(bEnable);
	}
	
	if (1)
	{
		BOOL bEnable = (-1 != nIndex) && (nIndex < (m_listCtrl.GetItemCount() - 1));
		GetDlgItem(IDC_BTN_DOWN)->EnableWindow(bEnable);
	}
}


void CPageConfigNetworkGeneral::OnLvnEndlabeleditListNetwork(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVDISPINFO* pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	*pResult = 0;

	CString strTemp(pDispInfo->item.pszText);
	CDataBaseProject* pProject = CDataBaseProject::GetInstance();
	const DWORD dwAddress = pProject->StringToAddress(strTemp);

	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	const INT nIndex = m_listCtrl.GetNextSelectedItem(pos);

	BOOL bValid = TRUE;

	if (0 == (dwAddress & 0xff000000) ||
		0 == (dwAddress & 0x000000ff) )
	{
		//	1. Check valid address
		bValid = FALSE;
	}
	else
	{
		const INT nItemCount = m_listCtrl.GetItemCount();
		for (INT nSearchIndex = 0; nSearchIndex < nItemCount; nSearchIndex++)
		{
			if (nIndex == nSearchIndex)
			{
				continue;
			}

			if( m_listCtrl.GetItemData(nSearchIndex) == dwAddress  )
			{
				//	2. Check already used address
				bValid = FALSE;
			}
		}
	}

	if (bValid && FALSE == m_bEsc)
	{
		pProject->GetStringAddress(strTemp, dwAddress);
		m_listCtrl.SetItemText(nIndex, 0, strTemp);
		m_listCtrl.SetItemData(nIndex, dwAddress);
	}
	else
	{
		strTemp = m_listCtrl.GetItemText(nIndex, 0);
		strTemp.Trim();
		if (strTemp.IsEmpty())
		{
			m_listCtrl.DeleteItem(nIndex);
		}
	}

	m_listCtrl.ModifyStyle(LVS_EDITLABELS, 0);
}

void CPageConfigNetworkGeneral::OnBnClickedBtnAdd()
{
	const INT nItemIndex = m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), _T(""));
	m_listCtrl.SelectSingleItem(nItemIndex);
	m_listCtrl.ModifyStyle(0, LVS_EDITLABELS);
	m_listCtrl.SetFocus();
	m_listCtrl.EditLabel(nItemIndex);
}


BOOL CPageConfigNetworkGeneral::PreTranslateMessage(MSG* pMsg)
{
	switch( pMsg->message)
	{
	case WM_KEYDOWN:
	{
		m_bEsc = FALSE;

		switch (pMsg->wParam)
		{
		case VK_RETURN:
		{
			CWnd* pWnd = GetFocus();

			if (1 == pWnd->GetDlgCtrlID())
			{
				switch (pWnd->GetParent()->GetDlgCtrlID())
				{
				case IDC_LIST_NETWORK:
				{
					m_listCtrl.SetFocus();
					return TRUE;
				}	break;
				}
				
			}
		}	break;

		case VK_ESCAPE:
		{
			m_bEsc = TRUE;
		}	break;
		}
	}	break;
	}

	return CMFCPropertyPage::PreTranslateMessage(pMsg);
}


void CPageConfigNetworkGeneral::OnBnClickedBtnMod()
{
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	const INT nIndex = m_listCtrl.GetNextSelectedItem(pos);
	m_listCtrl.SetFocus();
	m_listCtrl.ModifyStyle(0, LVS_EDITLABELS);
	m_listCtrl.EditLabel(nIndex);
	//m_listCtrl.EditLableByEdit(nIndex, 1);
}


void CPageConfigNetworkGeneral::OnBnClickedBtnDel()
{
	//	Check to delete
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	const INT nIndex = m_listCtrl.GetNextSelectedItem(pos);
	m_listCtrl.DeleteItem(nIndex);
}


void CPageConfigNetworkGeneral::OnOK()
{
	//	Change Address & Device ID
	CDeviceMotionGate* pDevice;
	DWORD	dwAddress;
	BOOL	bFindDevice = FALSE;
	CDeviceList* pDeviceList = CDeviceList::GetInstance();
	
	if (pDeviceList->IsMonitoringEnabled())
	{ // 만일 Network가 연결된 상태에서 해당 Device 삭제시 중지후 삭제한다.
		for (INT n = 0; n < pDeviceList->GetAddressCount(); n++)
		{
			bFindDevice = FALSE;
			CString strAddress;
			CString str;
			DWORD dwAddress = pDeviceList->GetAddressByIndex(n);
			
			pDeviceList->GetMotiongateByAddress(dwAddress, pDevice);
						
			if (pDevice->IsRun())
			{
				for (INT nIndex = 0; nIndex < m_listCtrl.GetItemCount(); nIndex++)
				{
					dwAddress = m_listCtrl.GetItemData(nIndex);
					if (pDevice->GetAddress() == dwAddress)
					{
						bFindDevice = TRUE;
					}
				}

				if (!bFindDevice)
				{ // 만약 삭제된 디바이스 항목을 찾는다면 중지시킨다.
					if (pDevice->IsRun())
					{
						pDevice->StopThread();
						pDevice->CmdConnectionClose();
					}
				}
			}
		
		} 
	}

	pDeviceList->ResetAddressIndex();

	const INT nCount = m_listCtrl.GetItemCount();
	for (INT nIndex = 0; nIndex < nCount; nIndex++)
	{
		pDeviceList->AddAddress(nIndex, m_listCtrl.GetItemData(nIndex) );
	//	pDeviceList->AddDeviceID(m_listCtrl.GetItemData(nIndex), _ttoi(m_listCtrl.GetItemText(nIndex, 1)));
	}


	pDeviceList->RebuildAddressIndex( TRUE );

	CMFCPropertyPage::OnOK();
}


void CPageConfigNetworkGeneral::OnBnClickedBtnUp()
{
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	const INT nIndex = m_listCtrl.GetNextSelectedItem(pos);
	
	ExchangeItemData(nIndex, nIndex - 1);
}

void CPageConfigNetworkGeneral::OnBnClickedBtnDown()
{
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	const INT nIndex = m_listCtrl.GetNextSelectedItem(pos);
	const INT nTempIndex = nIndex - 1;

	ExchangeItemData(nIndex, nIndex + 1);
}

void CPageConfigNetworkGeneral::ExchangeItemData(const INT nIdxItem1, const INT nIdxItem2)
{
	CString strItem1 = m_listCtrl.GetItemText(nIdxItem1, 0);
	const DWORD dwItem1 = m_listCtrl.GetItemData(nIdxItem1);

	CString strItem2 = m_listCtrl.GetItemText(nIdxItem2, 0);
	const DWORD dwItem2 = m_listCtrl.GetItemData(nIdxItem2);

	m_listCtrl.SetItemText(nIdxItem1, 0, strItem2);
	m_listCtrl.SetItemData(nIdxItem1, dwItem2);

	m_listCtrl.SetItemText(nIdxItem2, 0, strItem1);
	m_listCtrl.SetItemData(nIdxItem2, dwItem1);

	m_listCtrl.SelectSingleItem(nIdxItem2);
}


void CPageConfigNetworkGeneral::OnBnClickedBtnModDevID()
{
	//POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	//const INT nIndex = m_listCtrl.GetNextSelectedItem(pos);
	//m_listCtrl.SetFocus();
	//m_listCtrl.ModifyStyle(0, LVS_EDITLABELS);
	////m_listCtrl.EditLabel(nIndex,1);
	//m_listCtrl.EditLableByEdit(nIndex, 1);

	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	const INT nIndex = m_listCtrl.GetNextSelectedItem(pos);
	m_listCtrl.SetFocus();
	m_listCtrl.ModifyStyle(0, LVS_EDITLABELS);
	m_listCtrl.EditLableByEdit(nIndex, 1);
}


void CPageConfigNetworkGeneral::OnBnClickedButtonSearching()
{
	CString message("");
	CString mainInstruction("Are you sure to want to proceed with IP search?");
	CString title(_T("Confirm to Searching Network Address "));

	CTaskDialog taskDialog(message, mainInstruction, title, !(TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON));

	taskDialog.AddCommandControl(IDYES, _T("Yes"));
	taskDialog.AddCommandControl(IDNO, _T("No"));

	const HICON icon = AfxGetApp()->LoadStandardIcon(IDI_QUESTION);

	taskDialog.SetMainIcon(icon);

	taskDialog.SetDialogWidth(350);

	CDlgNetworkSearch dlg(this);
	CString strAddress;

	GetDlgItemText(IDC_IPADDRESS_START, strAddress);
	dlg.SetStartAddress(strAddress);

	GetDlgItemText(IDC_IPADDRESS_END, strAddress);
	dlg.SetEndAddress(strAddress);

	switch (taskDialog.DoModal())
	{
	case IDYES:
	{
		if (IDOK == dlg.DoModal())
		{

		}
	}	break;

	case IDNO:
	{
		
	}	break;
	}

	
}


const BOOL CPageConfigNetworkGeneral::SetNetworkSearchingAddress(CString strAddress)
{
	INT nIndex = m_listCtrl.GetItemCount();
	DWORD dwAddress = StringToAddress(strAddress);
	BOOL bContinue = TRUE;
	for (INT i = 0; i < m_listCtrl.GetItemCount(); i++)
	{
		if (strAddress == m_listCtrl.GetItemText(i, 0)) bContinue = FALSE;
	}
	
	if (bContinue)
	{
		m_listCtrl.InsertItem(nIndex, strAddress);
		m_listCtrl.SetItemData(nIndex, dwAddress);
	}

	return TRUE;
}


const DWORD CPageConfigNetworkGeneral::StringToAddress(CString strValue)
{
	const BYTE octet1 = _ttoi(strValue);
	strValue.Delete(0, strValue.Find(_T('.')) + 1);
	const BYTE octet2 = _ttoi(strValue);
	strValue.Delete(0, strValue.Find(_T('.')) + 1);
	const BYTE octet3 = _ttoi(strValue);
	strValue.Delete(0, strValue.Find(_T('.')) + 1);
	const BYTE octet4 = _ttoi(strValue);

	return static_cast<DWORD>(MAKELONG(MAKEWORD(octet4, octet3), MAKEWORD(octet2, octet1)));
}


void CPageConfigNetworkGeneral::OnIpnFieldchangedIpaddressStart(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	*pResult = 0;
	
	
	BYTE ipFirst, ipSecond, ipThird, ipForth;
	
	CEdit* pEdit[4]; 
	CString strAddress(_T("        .       .       ."));
	CString strOctet[4];

	for (INT i = 0; i < 4; i++)
	{
		pEdit[i] = (CEdit*)GetDlgItem(IDC_EDIT_OCTET0 + i);
	}

	CIPAddressCtrl* pCtrl = (CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_END);
	
	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_START))->GetAddress(ipFirst, ipSecond, ipThird, ipForth);

	if (true)
	{
		strOctet[0].Format(_T("%d"), ipFirst);
		strOctet[1].Format(_T("%d"), ipSecond);
		strOctet[2].Format(_T("%d"), ipThird);
		strOctet[3].Format(_T("%d"), 255);
	}

	if (0 != ipFirst)
	{
		if (0 != ipSecond)
		{
			CString strOctedThird;

			pEdit[1]->SetWindowText(strOctet[1]);
			pEdit[2]->SetWindowText(strOctet[2]);

			pCtrl->SetAddress(ipFirst, ipSecond, ipThird, 255);
			GetDlgItemText(IDC_EDIT_OCTET3, strOctedThird);

			if (strOctedThird.GetLength() != 0)
			{
				GetDlgItem(IDC_BUTTON_SEARCHING)->EnableWindow(TRUE);
			}
			strAddress.Format(_T(" %3d.    %3d.    %3d"), ipFirst, ipSecond, ipThird);
			SetDlgItemText(IDC_EDIT_IP, strAddress);
			return;
		} 

		pEdit[0]->SetWindowText(strOctet[0]);

		pCtrl->SetAddress(ipFirst, 0, 0, 0);
		strAddress.Format(_T("    %3d.       .       ."), ipFirst);
		
		GetDlgItem(IDC_BUTTON_SEARCHING)->EnableWindow(FALSE);
		
		SetDlgItemText(IDC_EDIT_IP, strAddress);
		return;
	}

	SetDlgItemText(IDC_EDIT_IP, strAddress);
}


void CPageConfigNetworkGeneral::OnEnChangeEditOctet3()
{
	CString strOctedThird;
	INT nOctedThird;
	BOOL bEnable = TRUE;
	BYTE ipFirst, ipSecond, ipThird, ipForth;
	CIPAddressCtrl* pCtrl = (CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_END);

	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_START))->GetAddress(ipFirst, ipSecond, ipThird, ipForth);
	
	GetDlgItemText(IDC_EDIT_OCTET3, strOctedThird);
	nOctedThird = _ttoi(strOctedThird);

	if (0 != ipFirst)
	{
		if (0 != ipSecond)
		{
			if (((INT)ipForth > nOctedThird) || 255 < (nOctedThird)) bEnable = FALSE;
			else
			{
				pCtrl->SetAddress(ipFirst, ipSecond, ipThird, nOctedThird);
			}
			GetDlgItem(IDC_BUTTON_SEARCHING)->EnableWindow(bEnable);
		}
	}


}


void CPageConfigNetworkGeneral::InitControlTextSize()
{
	if (true)
	{
		LOGFONT lf = { 0, };
		afxGlobalData.fontRegular.GetLogFont(&lf);

		lf.lfHeight = -15;
		m_BigFont.CreateFontIndirect(&lf);

		GetDlgItem(IDC_STATIC_SEARCHING)->SetFont(&m_BigFont);
		GetDlgItem(IDC_IPADDRESS_START)->SetFont(&m_BigFont);
		GetDlgItem(IDC_EDIT_IP)->SetFont(&m_BigFont);
		GetDlgItem(IDC_EDIT_OCTET3)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_SETTING)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_UP)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_DOWN)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_ADD)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_MOD)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_DEL)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_LIST)->SetFont(&m_BigFont);
		GetDlgItem(IDC_LIST_NETWORK)->SetFont(&m_BigFont);

		m_BigFont.Detach();
	}

	/*if (true)
	{
	m_BigFont.CreateFont(FONT_SIZE, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, TEXT_STYLE);

	GetDlgItem(IDC_STATIC_SEARCHING)->SetFont(&m_BigFont);
	GetDlgItem(IDC_IPADDRESS_START)->SetFont(&m_BigFont);
	GetDlgItem(IDC_EDIT_IP)->SetFont(&m_BigFont);
	GetDlgItem(IDC_EDIT_OCTET3)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_SETTING)->SetFont(&m_BigFont);
	GetDlgItem(IDC_BTN_UP)->SetFont(&m_BigFont);
	GetDlgItem(IDC_BTN_DOWN)->SetFont(&m_BigFont);
	GetDlgItem(IDC_BTN_ADD)->SetFont(&m_BigFont);
	GetDlgItem(IDC_BTN_MOD)->SetFont(&m_BigFont);
	GetDlgItem(IDC_BTN_DEL)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_LIST)->SetFont(&m_BigFont);
	GetDlgItem(IDC_LIST_NETWORK)->SetFont(&m_BigFont);

	m_BigFont.Detach();
	}*/

	
}
