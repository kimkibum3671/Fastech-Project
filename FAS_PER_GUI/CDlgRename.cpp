// CDlgRename.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CDlgRename.h"
#include "afxdialogex.h"


// CDlgRename dialog

IMPLEMENT_DYNAMIC(CDlgRename, CDialogEx)

CDlgRename::CDlgRename(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RENAME, pParent)
{
	m_pDevice = NULL;
}

CDlgRename::~CDlgRename()
{
}

void CDlgRename::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgRename, CDialogEx)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CDlgRename::OnBnClickedOk)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CDlgRename message handlers
void CDlgRename::SetTargetDevice(CDeviceMotionGate* pDevice)
{
	m_pDevice = pDevice;
}


BOOL CDlgRename::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (NULL == m_pDevice)
	{
		ASSERT(0);
		return FALSE;
	}

	CString strTemp(m_pDevice->GetSystemName());
	SetDlgItemText(IDC_EDIT_REGISTER, strTemp);
	SetDlgItemText(IDC_EDIT_NEW, strTemp);
//	SetDlgItemText(IDC_EDIT_READ, m_pDevice->GetRealName());

	SetTimer(TIMER_INIT, 0, NULL);

	return TRUE;
}


void CDlgRename::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_INIT:
	{
		KillTimer(nIDEvent);

		GetDlgItem(IDC_EDIT_NEW)->SetFocus();
	}	break;
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CDlgRename::OnBnClickedOk()
{
	//	1.		동록된 이름이 중복이 있는지 확인을 한다.
	//	1.A.	중복된 이름이 있는지 확다면 중복임을 알려주고 계속 진행할 것 인지 묻는다.
	//	2.		이름 변경을 지시한다.

	CString strName;
	GetDlgItemText(IDC_EDIT_NEW, strName);

//	m_pDevice->SetRealName(strName);

	CDialogEx::OnOK();
}


HBRUSH CDlgRename::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (IDC_EDIT_NEW == pWnd->GetDlgCtrlID())
	{
		CString strTemp;
		pWnd->GetWindowText(strTemp);

		const INT nStrLen = strTemp.GetLength();

		COLORREF color = -1;

		if (20 == nStrLen)
		{
			color = RGB(0xff, 0xff, 0x00);
		}
		else if (0 == nStrLen || 20 < nStrLen)
		{
			color = RGB(0xff, 0x00, 0x00);
		}

		if (color != static_cast<COLORREF>(-1))
		{
			pDC->SetBkColor(color);
			hbr = CreateSolidBrush(color);
		}
	}

	return hbr;
}
