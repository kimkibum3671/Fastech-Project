// CPageConfigDeviceOrigin.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageConfigDeviceOrigin.h"
#include "afxdialogex.h"

#include "CSheetConfigDevice.h"

// CPageConfigDeviceOrigin dialog

IMPLEMENT_DYNAMIC(CPageConfigDeviceOrigin, CPageConfig)

CPageConfigDeviceOrigin::CPageConfigDeviceOrigin(CWnd* pParent /*=nullptr*/)
	: CPageConfig(IDP_CONFIG_DEVICE_ORIGIN)
{
	m_nHomingTimeout = 10;
}

CPageConfigDeviceOrigin::~CPageConfigDeviceOrigin()
{
}

void CPageConfigDeviceOrigin::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageConfigDeviceOrigin, CMFCPropertyPage)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_EDIT_TIMEOUT, &CPageConfigDeviceOrigin::OnEnChangeEditTimeout)
END_MESSAGE_MAP()


// CPageConfigDeviceOrigin message handlers


BOOL CPageConfigDeviceOrigin::OnInitDialog()
{
	CMFCPropertyPageEx::OnInitDialog();

	CSheetConfigDevice* pParent = dynamic_cast<CSheetConfigDevice*>(GetParent());

	SetDlgItemInt(IDC_EDIT_TIMEOUT, pParent->GetEditHomingTimeout(), FALSE);

	return TRUE;
}


void CPageConfigDeviceOrigin::OnEnChangeEditTimeout()
{
	m_nHomingTimeout = GetDlgItemInt(IDC_EDIT_TIMEOUT, FALSE);
}


void CPageConfigDeviceOrigin::OnOK()
{
	CSheetConfigDevice* pParent = dynamic_cast<CSheetConfigDevice*>(GetParent());

	pParent->SetEditHomingTimeout(m_nHomingTimeout);

	CPageConfig::OnOK();
}
