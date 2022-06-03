// CPageLoginUserID.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageLoginUserID.h"
#include "afxdialogex.h"

#include "CSheetLogin.h"

// CPageLoginUserID dialog

IMPLEMENT_DYNAMIC(CPageLoginUserID, CMFCPropertyPageEx)

CPageLoginUserID::CPageLoginUserID(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPageEx(IDP_LOGIN_USER)
{
	m_nSelUser = -1;
}

CPageLoginUserID::~CPageLoginUserID()
{
}

void CPageLoginUserID::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageLoginUserID, CMFCPropertyPage)
	ON_CBN_SELCHANGE(IDC_CMB_USER, &CPageLoginUserID::OnCbnSelchangeCmbUser)
	ON_BN_CLICKED(IDC_BTN_NEXT, &CPageLoginUserID::OnBnClickedBtnNext)
END_MESSAGE_MAP()


// CPageLoginUserID message handlers
BOOL CPageLoginUserID::OnInitDialog()
{
	CMFCPropertyPageEx::OnInitDialog();

	CComboBox* pCmbCtrl = static_cast<CComboBox*>(GetDlgItem(IDC_CMB_USER));

	if (pCmbCtrl->GetCount())
	{
		pCmbCtrl->SetCurSel(-1 != m_nSelUser ? m_nSelUser : 0);
	}

	return TRUE;
}

void CPageLoginUserID::OnCbnSelchangeCmbUser()
{
	int i = 0;
}


void CPageLoginUserID::OnBnClickedBtnNext()
{
	TRACE(_T(""));
}


BOOL CPageLoginUserID::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message &&
		VK_RETURN == pMsg->wParam )
	{
		this->PostMessage( WM_COMMAND, IDC_BTN_NEXT);
	}

	return CMFCPropertyPageEx::PreTranslateMessage(pMsg);
}
