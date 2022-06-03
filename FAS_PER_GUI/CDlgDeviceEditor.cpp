// CDlgDeviceEditor.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CDlgDeviceEditor.h"
#include "afxdialogex.h"


// CDlgDeviceEditor dialog

IMPLEMENT_DYNAMIC(CDlgDeviceEditor, CDialogEx)

CDlgDeviceEditor::CDlgDeviceEditor(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DEIVCE_EDITOR, pParent)
{
	m_dwAddress = -1;
	m_nMotorCount = 0;
}

CDlgDeviceEditor::~CDlgDeviceEditor()
{
}

void CDlgDeviceEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgDeviceEditor, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgDeviceEditor::OnBnClickedOk)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_EDIT, &CDlgDeviceEditor::OnEnChangeEdit)
END_MESSAGE_MAP()


// CDlgDeviceEditor message handlers
void CDlgDeviceEditor::SetAddress(const DWORD dwAddress)
{
	m_dwAddress = dwAddress;
}

const DWORD	CDlgDeviceEditor::GetAddress()
{
	return m_dwAddress;
}

void CDlgDeviceEditor::SetName(LPCTSTR lpszName)
{
	m_strName = lpszName;
}

LPCTSTR CDlgDeviceEditor::GetName()
{
	return m_strName;
}

void CDlgDeviceEditor::SetCount(const INT nCount)
{
	m_nMotorCount = nCount;
}

const INT CDlgDeviceEditor::GetCount()
{
	return m_nMotorCount;
}

BOOL CDlgDeviceEditor::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (static_cast<DWORD>(-1) != m_dwAddress)
	{
		static_cast<CIPAddressCtrl*>(GetDlgItem(IDC_IPADDRESS))->SetAddress(m_dwAddress);
	}

	if (m_strName.GetLength())
	{
		SetDlgItemText(IDC_EDIT, m_strName);
	}

	if (1)
	{
		CString strTemp;
		CComboBox* pCmbCtrl = static_cast<CComboBox*>(GetDlgItem(IDC_CMB_COUNT));
		for (INT nIndex = 0; nIndex < 8; nIndex++)
		{
			strTemp.Format(_T("%d"), nIndex + 1);
			pCmbCtrl->AddString(strTemp);
		}

		pCmbCtrl->SetCurSel(m_nMotorCount ? m_nMotorCount - 1 : m_nMotorCount);
	}

	return TRUE;
}


BOOL CDlgDeviceEditor::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_CHAR:
	{
		if (IDC_IPADDRESS == ::GetDlgCtrlID(pMsg->hwnd))
		{
			const BOOL bResult = CDialogEx::PreTranslateMessage(pMsg);

			CheckButtonStatus();

			return bResult;
		}
	}	break;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CDlgDeviceEditor::OnBnClickedOk()
{
	m_nMotorCount = static_cast<CComboBox*>(GetDlgItem(IDC_CMB_COUNT))->GetCurSel() + 1;

	CDialogEx::OnOK();
}


HBRUSH CDlgDeviceEditor::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if ( IDC_EDIT == pWnd->GetDlgCtrlID())
	{
		CString strTemp;
		pWnd->GetWindowText(strTemp);

		const INT nStrLen = strTemp.GetLength();

		COLORREF color = -1;

		if (20 == nStrLen)
		{
			color = RGB(0xff, 0xff, 0x00);
		}
		else if( 0 == nStrLen || 20 < nStrLen )
		{
			color = RGB(0xff, 0x00, 0x00);
		}

		if (color != static_cast<COLORREF>(-1) )
		{
			pDC->SetBkColor(color);
			hbr = CreateSolidBrush(color);
		}
	}

	return hbr;
}


void CDlgDeviceEditor::OnEnChangeEdit()
{
	GetDlgItemText(IDC_EDIT, m_strName);

	CheckButtonStatus();
}

void CDlgDeviceEditor::CheckButtonStatus()
{
	CString strTemp;
	GetDlgItemText(IDC_EDIT, strTemp);

	static_cast<CIPAddressCtrl*>(GetDlgItem(IDC_IPADDRESS))->GetAddress(m_dwAddress);
	const BOOL bEnable = \
		(0xff000000 & m_dwAddress) &&
		(0x000000ff & m_dwAddress) &&
		(0 < strTemp.GetLength()) &&
		(20 >= strTemp.GetLength());

	GetDlgItem(IDOK)->EnableWindow(bEnable);
}
