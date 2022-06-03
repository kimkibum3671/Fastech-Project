// CPageFactoryResetConfirm.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageFactoryResetConfirm.h"
#include "afxdialogex.h"

#include "CSheetFactoryReset.h"

#include "CDeviceList.h"

// CPageFactoryResetConfirm dialog

IMPLEMENT_DYNAMIC(CPageFactoryResetConfirm, CMFCPropertyPageEx)

CPageFactoryResetConfirm::CPageFactoryResetConfirm(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPageEx(IDP_FACTORY_RESET_CONFIRM)
{

}

CPageFactoryResetConfirm::~CPageFactoryResetConfirm()
{
}

void CPageFactoryResetConfirm::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageFactoryResetConfirm, CMFCPropertyPage)
	ON_BN_CLICKED(IDC_BTN_BACK, &CPageFactoryResetConfirm::OnBnClickedBtnBack)
	ON_BN_CLICKED(IDC_BTN_NEXT, &CPageFactoryResetConfirm::OnBnClickedBtnNext)
END_MESSAGE_MAP()


// CPageFactoryResetConfirm message handlers


BOOL CPageFactoryResetConfirm::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

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

	InitControlTextSize();

	return TRUE;
}


BOOL CPageFactoryResetConfirm::OnSetActive()
{
	CSheetFactoryReset* pParent = static_cast<CSheetFactoryReset*>(GetParent());

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


void CPageFactoryResetConfirm::OnBnClickedBtnBack()
{
	ActivatePrevPage();
}


void CPageFactoryResetConfirm::OnBnClickedBtnNext()
{
	OnOK();
}


void CPageFactoryResetConfirm::InitControlTextSize()
{
	if (true)
	{
		LOGFONT lf = { 0, };
		afxGlobalData.fontRegular.GetLogFont(&lf);

		lf.lfHeight = -15;
		m_BigFont.CreateFontIndirect(&lf);

		GetDlgItem(IDC_STATIC_GROUP)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_ADDRESS_CAPTION)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_ADDRESS)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_NAME_CAPTION)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_NAME)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_COUNT_CAPTION)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_COUNT)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_MOTOR_CAPTION)->SetFont(&m_BigFont);
		GetDlgItem(IDC_ST_MOTOR)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_BACK)->SetFont(&m_BigFont);
		GetDlgItem(IDC_BTN_NEXT)->SetFont(&m_BigFont);
		

		m_BigFont.Detach();
	}
}
