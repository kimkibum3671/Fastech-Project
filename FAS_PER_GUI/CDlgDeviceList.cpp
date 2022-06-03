// CDlgDeviceList.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CDlgDeviceList.h"
#include "afxdialogex.h"

#include "CDeviceList.h"
#include "CDlgDeviceEditor.h"

#include "CDataBaseManager.h"

#include "CSheetConfigDeviceSingle.h"

// CDlgDeviceList dialog

IMPLEMENT_DYNAMIC(CDlgDeviceList, CDialogEx)

CDlgDeviceList::CDlgDeviceList(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DEVICE_LIST, pParent)
{

}


CDlgDeviceList::~CDlgDeviceList()
{

}


void CDlgDeviceList::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgDeviceList, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_ADD, &CDlgDeviceList::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_MOD, &CDlgDeviceList::OnBnClickedBtnMod)
	ON_BN_CLICKED(IDC_BTN_DEL, &CDlgDeviceList::OnBnClickedBtnDel)
	ON_BN_CLICKED(IDOK, &CDlgDeviceList::OnBnClickedOk)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, &CDlgDeviceList::OnLvnItemchangedList)
END_MESSAGE_MAP()


// CDlgDeviceList message handlers
BOOL CDlgDeviceList::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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
	}

	return TRUE;
}

void CDlgDeviceList::OnBnClickedBtnAdd()
{
	/*
	CSheetConfigDeviceSingle sheet;

	if (IDOK == sheet.DoModal())
	{
		CDeviceMotionGate* pDevice = sheet.GetMotiongateCtrl();
		if (pDevice)
		{
			CString strTemp;
			pDevice->GetAddress(strTemp);
			const INT nIndex = m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), strTemp);

			m_listCtrl.SetItemText(nIndex, 1, pDevice->GetSystemName() );

			strTemp.Format(_T("%d"), sheet.GetMotorCount());
			m_listCtrl.SetItemText(nIndex, 2, strTemp);

			m_listCtrl.SetItemData(nIndex, reinterpret_cast<DWORD>(pDevice));

			CDeviceList::GetInstance()->RegisterDevice(pDevice);
		}
		else
		{
			MessageBox(_T("You cannot insert that device\n\n") \
				_T("- Same IP Address or device name has been found."),
				_T("Failed to insert"), MB_ICONERROR | MB_OK);
		}
	}
	*/
}

void CDlgDeviceList::OnBnClickedBtnMod()
{
	CSheetConfigDeviceSingle sheet;
	sheet.SetMotiongateCtrl(reinterpret_cast<CDeviceMotionGate*>(m_listCtrl.GetItemData(m_nCurSel)));
	if (IDOK == sheet.DoModal())
	{
		CDeviceMotionGate* pDevice = sheet.GetMotiongateCtrl();
		if (pDevice)
		{
			CString strTemp;	pDevice->GetAddress(strTemp);
			m_listCtrl.SetItemText(m_nCurSel, 0, strTemp);

			m_listCtrl.SetItemText(m_nCurSel, 1, pDevice->GetSystemName() );

			strTemp.Format(_T("%d"), pDevice->GetMotorCount() );
			m_listCtrl.SetItemText(m_nCurSel, 2, strTemp);
		}
	}
}

void CDlgDeviceList::OnBnClickedBtnDel()
{
	if (IDYES == MessageBox(_T("You you sure to delete selected item?"),
		_T("Failed to insert"), MB_ICONERROR | MB_YESNOCANCEL))
	{
		m_listCtrl.DeleteItem(m_nCurSel);
		m_nCurSel = -1;
		m_listCtrl.Invalidate(FALSE);

		GetDlgItem(IDC_BTN_MOD)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_DEL)->EnableWindow(FALSE);
	}
}

void CDlgDeviceList::OnLvnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	m_nCurSel = m_listCtrl.GetNextSelectedItem(pos);

	const BOOL bEnable = -1 != m_nCurSel;

	GetDlgItem(IDC_BTN_MOD)->EnableWindow(bEnable);
	GetDlgItem(IDC_BTN_DEL)->EnableWindow(bEnable);
}

void CDlgDeviceList::OnBnClickedOk()
{	
	/*
	CDeviceList* pDeviceList = CDeviceList::GetInstance();

	pDeviceList->ResetDevicePriority();

	const INT nCount = m_listCtrl.GetItemCount();
	for (INT nIndex = 0; nIndex < nCount; nIndex++)
	{
		CDeviceMotionGate* pDevice = reinterpret_cast<CDeviceMotionGate*>(m_listCtrl.GetItemData(nIndex));
		if (pDevice)
		{
			pDevice->EnableAutoDatabaseUpdate(FALSE);
			pDevice->EnableAutoDatabaseUpdate(TRUE);
		}
	}

	pDeviceList->UpdateDeviceInformation();

	*/
	CDialogEx::OnOK();
}
