// CPageLoginPassword.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageLoginPassword.h"
#include "afxdialogex.h"

#include "CSheetLogin.h"

#include "CDataBaseManager.h"

// CPageLoginPassword dialog

IMPLEMENT_DYNAMIC(CPageLoginPassword, CMFCPropertyPageEx)

CPageLoginPassword::CPageLoginPassword(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPageEx(IDP_LOGIN_PASSWORD)
{

}

CPageLoginPassword::~CPageLoginPassword()
{
}

void CPageLoginPassword::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageLoginPassword, CMFCPropertyPage)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_PREV, &CPageLoginPassword::OnBnClickedBtnPrev)
	ON_BN_CLICKED(IDC_BTN_NEXT, &CPageLoginPassword::OnBnClickedBtnNext)
//	ON_EN_CHANGE(IDC_EDIT_PASSWORD, &CPageLoginPassword::OnEnChangeEditPassword)
END_MESSAGE_MAP()


// CPageLoginPassword message handlers
BOOL CPageLoginPassword::OnInitDialog()
{
	CMFCPropertyPageEx::OnInitDialog();

	m_btnPrev.SubclassDlgItem(IDC_BTN_PREV, this);
	m_btnPrev.SetImage(IDB_BACK);

	m_btnNext.SubclassDlgItem(IDC_BTN_NEXT, this);
	m_btnNext.SetImage(IDB_LOGIN);

	m_wndImage.SubclassDlgItem(IDC_ST_IMAGE, this);

	return TRUE;
}


BOOL CPageLoginPassword::OnSetActive()
{
	m_wndImage.LoadMainImage(static_cast<CSheetLogin*>(GetParent())->GetUserImage());
	
	SetDlgItemText(IDC_ST_NAME, CString(" ") + static_cast<CSheetLogin*>(GetParent())->GetUserName());
	SetDlgItemText(IDC_EDIT_PASSWORD, _T(""));
	
	SetTimer(TIMER_SET_DEFAULT_FOCUS, 100, NULL);

	return CMFCPropertyPageEx::OnSetActive();
}


void CPageLoginPassword::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_SET_DEFAULT_FOCUS:
	{
		GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();
		KillTimer(nIDEvent);
	}	break;
	}

	CMFCPropertyPageEx::OnTimer(nIDEvent);
}


void CPageLoginPassword::OnBnClickedBtnPrev()
{
	ActivatePrevPage();
}

void CPageLoginPassword::OnBnClickedBtnNext()
{
	CString strName, strPassword;
	
	GetDlgItemText(IDC_ST_NAME, strName);
	strName.TrimLeft();

	GetDlgItemText(IDC_EDIT_PASSWORD, strPassword);

	CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();

	if (pConfigDB->ChangeUser(strName, strPassword))
	{
		OnOK();
	}
	else
	{
		//	사용자 정보가 없거나 비밀번호가 다르거나
		//	일치하는 회원 정보가 없습니다.
		CString strCaption, strMessage;
		strCaption.LoadString(IDS_CAPTION_ERROR);
		strMessage.LoadString(IDS_FAIL_LOGIN);
		MessageBox(strMessage, strCaption, MB_ICONERROR);
	}
}


BOOL CPageLoginPassword::PreTranslateMessage(MSG* pMsg)
{
	if ( WM_KEYDOWN ==  pMsg->message &&
		VK_RETURN == pMsg->wParam &&
		IDC_EDIT_PASSWORD == ::GetDlgCtrlID(pMsg->hwnd) )
	{
		OnBnClickedBtnNext();
	}

	return CMFCPropertyPageEx::PreTranslateMessage(pMsg);
}
