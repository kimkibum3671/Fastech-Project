// CPageConfigDeviceSocket.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageConfigDeviceSocket.h"
#include "afxdialogex.h"

#include "CDeviceList.h"
#include "CDataBaseManager.h"

#include "CSheetConfigDeviceSingle.h"

// CPageConfigDeviceSocket dialog

#define MAKE_HISTORY(x, y)	( (ULONGLONG(x) & 0xffffffff) | (ULONGLONG(y) & 0xffffffff) << 32 )
#define LO_HISTORY(x)		( (x) & 0xffffffff )
#define HI_HISTORY(x)		( ( (x) >> 32 ) & 0xffffffff )

IMPLEMENT_DYNAMIC(CPageConfigDeviceSocket, CMFCPropertyPage)

CPageConfigDeviceSocket::CPageConfigDeviceSocket(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPage(IDP_CONFIG_DEVICE_SOCKET)
{
	
}

CPageConfigDeviceSocket::~CPageConfigDeviceSocket()
{
}

void CPageConfigDeviceSocket::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageConfigDeviceSocket, CMFCPropertyPage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, &CPageConfigDeviceSocket::OnLvnItemchangedList)
	ON_BN_CLICKED(IDC_BTN_ADD, &CPageConfigDeviceSocket::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_MOD, &CPageConfigDeviceSocket::OnBnClickedBtnMod)
	ON_BN_CLICKED(IDC_BTN_DEL, &CPageConfigDeviceSocket::OnBnClickedBtnDel)
END_MESSAGE_MAP()


// CPageConfigDeviceSocket message handlers


BOOL CPageConfigDeviceSocket::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	if (1)	// Initialize ListCtrl
	{
		CDeviceList* pDeviceList = CDeviceList::GetInstance();

		m_listCtrl.SubclassDlgItem(IDC_LIST, this);
		m_listCtrl.ModifyStyle(0, LVS_SINGLESEL);
		m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
		HRESULT result = ::SetWindowTheme(m_listCtrl.m_hWnd, _T("Explorer"), NULL);

		m_listCtrl.InsertColumn(0, _T("IP Address"));
		m_listCtrl.InsertColumn(1, _T("Device Name"));
		m_listCtrl.InsertColumn(2, _T("Motor Count"));

		const INT nCount = pDeviceList->GetRegisteredDeviceCount();
		for (INT nIndex = 0; nIndex < nCount; nIndex++)
		{
			CDeviceMotionGate* pDevice = pDeviceList->GetMotiongateByPriority(nIndex);
			if (NULL == pDevice)
			{
				continue;
			}

			CString strTemp;
			pDevice->GetAddress(strTemp);
			INT nItemIndex = m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), strTemp);

			m_listCtrl.SetItemText(nItemIndex, 1, pDevice->GetSystemName());

			strTemp.Format(_T("%d"), pDevice->GetMotorCount());
			m_listCtrl.SetItemText(nItemIndex, 2, strTemp);

			m_listCtrl.SetItemData(nItemIndex, reinterpret_cast<DWORD_PTR>(pDevice));
		}

		INT nWidth1, nWidth2;
		m_listCtrl.SetColumnWidth(0, LVSCW_AUTOSIZE);
		nWidth1 = m_listCtrl.GetColumnWidth(0);
		m_listCtrl.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
		nWidth2 = m_listCtrl.GetColumnWidth(0);
		m_listCtrl.SetColumnWidth(0, (nWidth1 > nWidth2 ? nWidth1 : nWidth2) + 50);

		m_listCtrl.SetColumnWidth(1, LVSCW_AUTOSIZE);
		nWidth1 = m_listCtrl.GetColumnWidth(1);
		m_listCtrl.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
		nWidth2 = m_listCtrl.GetColumnWidth(1);
		m_listCtrl.SetColumnWidth(1, (nWidth1 > nWidth2 ? nWidth1 : nWidth2) + 50);

		m_listCtrl.SetColumnWidth(2, LVSCW_AUTOSIZE);
		nWidth1 = m_listCtrl.GetColumnWidth(1);
		m_listCtrl.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);
		nWidth2 = m_listCtrl.GetColumnWidth(2);
		m_listCtrl.SetColumnWidth(2, (nWidth1 > nWidth2 ? nWidth1 : nWidth2) - 30);

		m_nCurSel = -1;
	}

	return TRUE;
}



void CPageConfigDeviceSocket::OnLvnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	*pResult = 0;

	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	m_nCurSel = m_listCtrl.GetNextSelectedItem(pos);

	const BOOL bEnable = -1 != m_nCurSel;

	GetDlgItem(IDC_BTN_MOD)->EnableWindow(bEnable);
	GetDlgItem(IDC_BTN_DEL)->EnableWindow(bEnable);
}


void CPageConfigDeviceSocket::OnBnClickedBtnAdd()
{
	CWnd* pParent = GetParent();
	pParent->ShowWindow(SW_HIDE);
	
	CDeviceMotionGate* pDevice = new CDeviceMotionGate;
	pDevice->EnterEditMode();

	CSheetConfigDeviceSingle sheet;
	sheet.SetMotiongateCtrl(pDevice);
	const INT nResult = sheet.DoModal();

	pParent->ShowWindow(SW_SHOWNORMAL);

	if (IDOK == nResult)
	{
		CString strTemp;
		pDevice->GetEditAddress(strTemp);
		const INT nIndex = m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), strTemp);

		m_listCtrl.SetItemText(nIndex, 1, pDevice->GetEditSystemName());

		strTemp.Format(_T("%d"), pDevice->GetEditMotorCount());
		m_listCtrl.SetItemText(nIndex, 2, strTemp);

		m_listCtrl.SetItemData(nIndex, reinterpret_cast<DWORD>(pDevice));

		CDeviceList::GetInstance()->RegisterDevice(pDevice);
	}
	else
	{
		delete pDevice;
	}
}


void CPageConfigDeviceSocket::OnBnClickedBtnMod()
{
	CWnd* pParent = GetParent();
	pParent->ShowWindow(SW_HIDE);

	CSheetConfigDeviceSingle sheet;
	CDeviceMotionGate* pDevice = reinterpret_cast<CDeviceMotionGate*>(m_listCtrl.GetItemData(m_nCurSel));
	pDevice->EnterEditMode();

	sheet.SetMotiongateCtrl(pDevice);
	const INT nResult = sheet.DoModal();
	
	pParent->ShowWindow(SW_SHOWNORMAL);

	if (IDOK == nResult)
	{
		const DWORD dwCurAddress = pDevice->GetAddress();
		const DWORD dwNewAddress = pDevice->GetEditAddress();

		if (dwCurAddress != dwNewAddress)
		{
			const ULONGLONG history = MAKE_HISTORY(dwCurAddress, dwNewAddress);
			m_lstAddressHistory.AddTail(history);
		}

		CString strTemp;

		pDevice->GetEditAddress(strTemp);
		m_listCtrl.SetItemText(m_nCurSel, 0, strTemp);

		m_listCtrl.SetItemText(m_nCurSel, 1, pDevice->GetEditSystemName());

		strTemp.Format(_T("%d"), pDevice->GetEditMotorCount());
		m_listCtrl.SetItemText(m_nCurSel, 2, strTemp);
	}
	else
	{
		pDevice->ExitEditMode(FALSE);
	}
}


void CPageConfigDeviceSocket::OnBnClickedBtnDel()
{
	CString message("");
	CString mainInstruction("You you sure to delete a selected item?");
	CString title("Confirm to action");

	CTaskDialog taskDialog(message, mainInstruction, title, TDCBF_CANCEL_BUTTON);

	taskDialog.AddCommandControl(IDYES, L"Yes.");
	taskDialog.AddCommandControl(IDNO, L"No.");

	taskDialog.SetMainIcon(IDI_QUESTION);	

	// Change the default command button control
	taskDialog.SetDefaultCommandControl(IDYES);

//	if (taskDialog.IsCommandControlEnabled(303))
//	{
//		taskDialog.SetCommandControlOptions(303, FALSE);
//	}

	if (IDYES == taskDialog.DoModal())
	{
		m_listCtrl.DeleteItem(m_nCurSel);
		m_nCurSel = -1;
		m_listCtrl.Invalidate(FALSE);

		GetDlgItem(IDC_BTN_MOD)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_DEL)->EnableWindow(FALSE);
	}
}


void CPageConfigDeviceSocket::OnOK()
{
	CDeviceList* pDeviceList = CDeviceList::GetInstance();
	CDataBaseProject* pProjectDB = CDataBaseProject::GetInstance();

	CMap<CDeviceMotionGate*, CDeviceMotionGate*, CDeviceMotionGate*, CDeviceMotionGate*> mapRunningDevice;

	if (1)
	{
		// IP가 변경되는 Device 정지
		POSITION pos = m_lstAddressHistory.GetHeadPosition();
		while (pos)
		{
			ULONGLONG address = m_lstAddressHistory.GetNext(pos);
			CDeviceMotionGate* pDevice = NULL;
			if (pDeviceList->GetMotiongateByAddress(LO_HISTORY(address), pDevice) &&
				pDevice->IsRunning() )
			{
				pDevice->StopThread();
				
				//	굳이 중복을 검사할 필요가 있을까?
				mapRunningDevice.SetAt(pDevice, pDevice);
			}
		}
	}

	if (1)
	{
		//	IP가 변경되는 Device 정지 확인
		BOOL bRun = TRUE;
		while (bRun)
		{
			Sleep(1);

			bRun = FALSE;

			CDeviceMotionGate* pDevice = NULL;
			POSITION pos = mapRunningDevice.GetStartPosition();
			while (pos)
			{
				mapRunningDevice.GetNextAssoc(pos, pDevice, pDevice);
				if( pDevice->IsMotiongateConnected())
				{
					bRun = TRUE;
					break;
				}
			}
		}
	}

	if (1)
	{
		//	IP 및 자료구조 변경
		POSITION pos = m_lstAddressHistory.GetHeadPosition();
		while (pos)
		{
			ULONGLONG address = m_lstAddressHistory.GetNext(pos);
			const DWORD dwCurAddress = LO_HISTORY(address);
			const DWORD dwNewAddress = HI_HISTORY(address);

			if (pDeviceList->ChangeAddress(dwCurAddress, dwNewAddress))
			{
				if (FALSE == pProjectDB->ChangeIpAddress(dwCurAddress, dwNewAddress))
				{
					//	뭔가 이상하다.
					break;
				}
			}
		}
	}
	
	if (1)
	{
		CDeviceList* pDeviceList = CDeviceList::GetInstance();

		pDeviceList->ResetDevicePriority();

		const INT nCount = m_listCtrl.GetItemCount();
		for (INT nIndex = 0; nIndex < nCount; nIndex++)
		{
			CDeviceMotionGate* pDevice = reinterpret_cast<CDeviceMotionGate*>(m_listCtrl.GetItemData(nIndex));
			if (pDevice)
			{
				pDevice->EnableAutoDatabaseUpdate(FALSE);
				pDevice->ExitEditMode();
//				pDevice->SetPriority(nIndex);
				pDevice->EnableAutoDatabaseUpdate(TRUE);
			}
		}

		pDeviceList->UpdateDeviceInformation();
	}

	if (1)
	{

		CDeviceMotionGate* pDevice = NULL;
		POSITION pos = mapRunningDevice.GetStartPosition();
		while (pos)
		{
			mapRunningDevice.GetNextAssoc(pos, pDevice, pDevice);
			pDevice->StartThread();
		}
	}

	mapRunningDevice.RemoveAll();

	CMFCPropertyPage::OnOK();
}

CDeviceMotionGate* CPageConfigDeviceSocket::ScanAddress(const DWORD dwAddress)
{
	CDataBaseProject* pProjectDB = CDataBaseProject::GetInstance();

	const INT nCount = m_listCtrl.GetItemCount();
	for (INT nIndex = 0; nIndex < nCount; nIndex++)
	{
		CString strAddress(m_listCtrl.GetItemText(nIndex, 0));
		const DWORD dwCurAddrss = pProjectDB->StringToAddress(strAddress);
		if (dwCurAddrss == dwAddress)
		{
			return reinterpret_cast<CDeviceMotionGate*>(m_listCtrl.GetItemData(nIndex));
		}
	}

	return NULL;
}

CDeviceMotionGate* CPageConfigDeviceSocket::ScanName(LPCTSTR lpszSystemName)
{
	CDataBaseProject* pProjectDB = CDataBaseProject::GetInstance();

	const INT nCount = m_listCtrl.GetItemCount();
	for (INT nIndex = 0; nIndex < nCount; nIndex++)
	{
		if (m_listCtrl.GetItemText(nIndex, 1) == lpszSystemName)
		{
			return reinterpret_cast<CDeviceMotionGate*>(m_listCtrl.GetItemData(nIndex));
		}
	}

	return NULL;
}
