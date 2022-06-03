// CPageAssignConfirm.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageAssignConfirm.h"
#include "afxdialogex.h"

#include "CSheetAssignID.h"

#include "CDeviceList.h"

// CPageAssignConfirm dialog

IMPLEMENT_DYNAMIC(CPageAssignConfirm, CMFCPropertyPageEx)

CPageAssignConfirm::CPageAssignConfirm(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPageEx(IDP_ASSIGN_CONFIRM)
{

}

CPageAssignConfirm::~CPageAssignConfirm()
{
}

void CPageAssignConfirm::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageAssignConfirm, CMFCPropertyPage)
	ON_BN_CLICKED(IDC_BTN_BACK, &CPageAssignConfirm::OnBnClickedBtnBack)
	ON_BN_CLICKED(IDC_BTN_NEXT, &CPageAssignConfirm::OnBnClickedBtnNext)
END_MESSAGE_MAP()


// CPageAssignConfirm message handlers
BOOL CPageAssignConfirm::OnInitDialog()
{
	CMFCPropertyPageEx::OnInitDialog();

	if (1)
	{
		COLORREF m_colorBkName = RGB(0x0a, 0x0a, 0x50);
		COLORREF m_colorTxtName = RGB(0xc8, 0xc8, 0xc8);

		const INT nResID[] = {
			IDC_ST_ADDRESS_CAPTION,
			IDC_ST_NAME_CAPTION,
			IDC_ST_COUNT_CAPTION,
			IDC_ST_MOTOR_CAPTION,
			NULL
		};

		for (INT nIndex = 0; nResID[nIndex]; nIndex++)
		{
			m_wndColor[nIndex].SubclassDlgItem(nResID[nIndex], this);
			m_wndColor[nIndex].SetBkColor(m_colorBkName);
			m_wndColor[nIndex].SetTextColor(m_colorTxtName);
		}
	}

	if (1)
	{
		COLORREF m_colorBkName = RGB(0xf0, 0xf0, 0xf0);
		COLORREF m_colorTxtName = RGB(0x00, 0x00, 0x00);

		const INT nResID[] = {
			IDC_ST_ADDRESS,
			IDC_ST_NAME,
			IDC_ST_COUNT,
			IDC_ST_MOTOR,
			NULL,
		};

		for (INT nIndex = 0; nResID[nIndex]; nIndex++)
		{
			const INT nRelIndex = nIndex + 4;
			m_wndColor[nRelIndex].SubclassDlgItem(nResID[nIndex], this);
			m_wndColor[nRelIndex].SetBkColor(nResID[nIndex] == IDC_ST_MOTOR ? RGB(0xc8, 0xc8, 0x00) : m_colorBkName);
			m_wndColor[nRelIndex].SetTextColor(m_colorTxtName);
		}
	}

	m_btnPrev.SubclassDlgItem(IDC_BTN_BACK, this);
	m_btnPrev.SetImage(IDB_BACK);

	m_btnStart.SubclassDlgItem(IDC_BTN_NEXT, this);
	m_btnStart.SetImage(IDB_START);	

	return TRUE;
}


void CPageAssignConfirm::OnBnClickedBtnBack()
{
	ActivatePrevPage();
}


void CPageAssignConfirm::OnBnClickedBtnNext()
{
	OnOK();
}

BOOL CPageAssignConfirm::OnSetActive()
{
	CSheetAssignID* pParent = static_cast<CSheetAssignID*>(GetParent());

	CDeviceMotionGate* pDevice = NULL;
	if (CDeviceList::GetInstance()->GetMotiongateByAddress(pParent->GetAddress(), pDevice))
	{
		CString strAddress;
		pDevice->GetAddress(strAddress);
		SetDlgItemText(IDC_ST_ADDRESS, strAddress);

		SetDlgItemText(IDC_ST_NAME, pDevice->GetSystemName());

		CString strMotorCount;
		strMotorCount.Format(_T("%d"), pDevice->GetMotorCount());
		SetDlgItemText(IDC_ST_COUNT, strMotorCount);

		if (NULL == m_wndViewer.GetSafeHwnd())
		{
			m_wndViewer.SubclassDlgItem(IDC_ST_PROJECT, this);
			m_wndViewer.LoadMainImage(pDevice->GetSystemImage());
		}
	}

	SetDlgItemInt(IDC_ST_MOTOR, pParent->GetSelectedMotorID());

	return CMFCPropertyPage::OnSetActive();
}
