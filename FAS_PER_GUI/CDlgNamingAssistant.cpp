// CDlgNamingAssistant.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CDlgNamingAssistant.h"
#include "afxdialogex.h"

#define UM_LIST_ITEM_CHANGED	WM_APP

// CDlgNamingAssistant dialog

IMPLEMENT_DYNAMIC(CDlgNamingAssistant, CDialogEx)

CDlgNamingAssistant::CDlgNamingAssistant(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DEVICE_NAME, pParent)
{

}

CDlgNamingAssistant::~CDlgNamingAssistant()
{
	m_mapDevice.RemoveAll();
}

void CDlgNamingAssistant::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgNamingAssistant, CDialogEx)
	//	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, &CDlgNamingAssistant::OnLvnItemchangedList)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_CLICK, IDC_LIST, &CDlgNamingAssistant::OnNMClickList)
	ON_MESSAGE(UM_LIST_ITEM_CHANGED, &CDlgNamingAssistant::OnListItemChanged)
	ON_BN_CLICKED(IDOK, &CDlgNamingAssistant::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgNamingAssistant message handlers


BOOL CDlgNamingAssistant::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (1)
	{
		CString strTemp;
		if (strTemp.LoadString(IDS_LIST_DONT_CARE))
		{
			m_arrString.Add(strTemp);
		}
		if (strTemp.LoadString(IDS_LIST_SYNC_SYSTEM))
		{
			m_arrString.Add(strTemp);
		}
		if (strTemp.LoadString(IDS_LIST_SYNC_DEVICE))
		{
			m_arrString.Add(strTemp);
		}
		if (strTemp.LoadString(IDS_LIST_NEW_NAME))
		{
			m_arrString.Add(strTemp);
		}
	}

	if (m_wndList.SubclassDlgItem(IDC_LIST, this))
	{
		m_wndList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SINGLEROW | LVS_EX_DOUBLEBUFFER);
		m_wndList.SetEditLableMessage(UM_LIST_ITEM_CHANGED);

		m_wndList.InsertColumn(0, _T("IP Address"), 0, 100);
		m_wndList.InsertColumn(1, _T("System Name"), 0, 100);
		m_wndList.InsertColumn(2, _T("Device Name"), 0, 100);
		m_wndList.InsertColumn(3, _T("Solution"), 0, 100);
		m_wndList.InsertColumn(4, _T("Description"), 0, 100);

		m_mutexInternal.Lock();
		POSITION pos = m_mapDevice.GetStartPosition();
		while (pos)
		{
			CDeviceMotionGate* pKey, * pDevice;
			m_mapDevice.GetNextAssoc(pos, pKey, pDevice);

			InsertItem(pDevice, &m_wndList);
		}
		m_mutexInternal.Unlock();

		m_wndList.LoadHeaderItemInfo();

		m_wndList.SetWindowPos(&wndBottom, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
	}

	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);

	return TRUE;
}

void CDlgNamingAssistant::AddDevice(CDeviceMotionGate* pDevice)
{
	m_mutexInternal.Lock();

	CDeviceMotionGate* pDevicePtr = NULL;
	if (FALSE == m_mapDevice.Lookup(pDevice, pDevicePtr))
	{
		m_mapDevice.SetAt(pDevice, pDevice);

		if (m_wndList.GetSafeHwnd())
		{
			InsertItem(pDevice, &m_wndList);
		}
	}

	m_mutexInternal.Unlock();
}


void CDlgNamingAssistant::InsertItem(CDeviceMotionGate* pDevice, CListCtrl* pListCtrl)
{
	/*if (NULL == pListCtrl)
	{
		pListCtrl = static_cast<CListCtrl*>(GetDlgItem(IDC_LIST));
	}

	CString strDefault(m_arrString.GetAt(0));

	if (pListCtrl->GetSafeHwnd())
	{
		CString strAddress;	pDevice->GetAddress(strAddress);
		const INT nIndex = pListCtrl->InsertItem(pListCtrl->GetItemCount(), strAddress);

		pListCtrl->SetItemText(nIndex, 1, pDevice->GetSystemName());
		pListCtrl->SetItemText(nIndex, 2, pDevice->GetRealName());
		pListCtrl->SetItemText(nIndex, 3, strDefault);
		pListCtrl->SetItemData(nIndex, reinterpret_cast<DWORD_PTR>(pDevice));
	}*/
}


void CDlgNamingAssistant::OnDestroy()
{
	CDialogEx::OnDestroy();

	m_mapDevice.RemoveAll();

	m_wndList.SaveHeaderItemInfo();
}

void CDlgNamingAssistant::SetMessageInformation(CWnd* pTarget, UINT nMsgID)
{
	m_pWndTarget = pTarget;
	m_nMsgID = nMsgID;
}


void CDlgNamingAssistant::OnNMClickList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	CPoint pt;	GetCursorPos(&pt);
	INT nItem = -1, nColumn = -1;
	if (m_wndList.HitTestEx(pt, &nItem, &nColumn))
	{
		switch (nColumn)
		{
		case 0:
		{	//	IP Address
			m_wndList.SetItemText(nItem, 4, _T(""));
		}	break;

		case 1:	//	Sync to system
		case 2:	//	Sync to device
		{
			m_wndList.SetItemText(nItem, 3, m_arrString.GetAt(nColumn));
			m_wndList.SetItemText(nItem, 4, _T(""));
		}	break;

		case 3:
		{	//	selectable
			ShowComboBox(nItem, nColumn);
		}	break;

		case 4:
		{
			ShowEditBox(nItem, nColumn);
		}	break;
		}
	}
}


void CDlgNamingAssistant::ShowComboBox(const INT nItem, const INT nColumn)
{
	m_wndList.EditLableByComboBox(nItem, nColumn, m_arrString);
}

void CDlgNamingAssistant::ShowEditBox(const INT nItem, const INT nColumn)
{
	m_wndList.EditLableByEdit(nItem, nColumn);
}

LRESULT CDlgNamingAssistant::OnListItemChanged(WPARAM wParam, LPARAM lParam)
{
	CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

	if (3 == pt.x)
	{
		INT nSelect = GetSelectedItemIndex(pt.y, pt.x);

		if (3 == nSelect)
		{
			ShowEditBox(pt.y, 4);
		}
		else
		{
			m_wndList.SetItemText(pt.y, 4, _T(""));
		}
	}

	return 0;
}


void CDlgNamingAssistant::OnBnClickedOk()
{
	//const SHORT shStatus = GetKeyState(VK_RETURN);
	//if (0x8000 & shStatus)
	//{
	//	return;
	//}

	//const INT nCount = m_wndList.GetItemCount();
	//for (INT nIndex = 0; nCount > nIndex; nIndex++)
	//{
	//	BYTE btOption = 0;
	//	CString strName;
	//	switch (GetSelectedItemIndex(nIndex))
	//	{
	//	case 0:		//	Don't care
	//	{

	//	}	break;

	//	case 1:		//	Sync to System
	//	{
	//		strName = m_wndList.GetItemText(nIndex, 1);
	//		btOption = CDeviceMotionGate::option_change_name_register;
	//	}	break;

	//	case 2:	//	Sync to Device
	//	{
	//		strName = m_wndList.GetItemText(nIndex, 2);
	//		btOption =
	//			CDeviceMotionGate::option_change_name_device;
	//	}	break;

	//	case 3:	//	Use new alias
	//	{
	//		strName = m_wndList.GetItemText(nIndex, 4);
	//		btOption =
	//			CDeviceMotionGate::option_change_name_register |
	//			CDeviceMotionGate::option_change_name_device;
	//	}	break;

	//	default:
	//	{

	//	}	break;
	//	}

	//	if (btOption)
	//	{
	//		CDeviceMotionGate* pDevice = reinterpret_cast<CDeviceMotionGate*>(m_wndList.GetItemData(nIndex));
	//		pDevice->ChangeName(strName, CDeviceMotionGate::option_change_name_register);
	//	}
	//}

	CDialogEx::OnOK();
}

const INT CDlgNamingAssistant::GetSelectedItemIndex(const INT nItem, const INT nColumn)
{
	INT nResult = -1;

	CString strText(m_wndList.GetItemText(nItem, nColumn));
	const INT nCount = m_arrString.GetCount();
	for (INT nIndex = 0; nCount > nIndex; nIndex++)
	{
		if (strText == m_arrString.GetAt(nIndex))
		{
			nResult = nIndex;
			break;
		}
	}

	return nResult;
}
