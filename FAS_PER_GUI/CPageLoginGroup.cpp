// CPageLoginGroup.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageLoginGroup.h"
#include "afxdialogex.h"

#include "CDataBaseManager.h"

#include "CSheetLogin.h"

// CPageLoginGroup dialog

IMPLEMENT_DYNAMIC(CPageLoginGroup, CMFCPropertyPage)

CPageLoginGroup::CPageLoginGroup(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPageEx(IDP_LOGIN_GROUP)
{
	m_nSelUser = -1;
}

CPageLoginGroup::~CPageLoginGroup()
{
}

void CPageLoginGroup::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageLoginGroup, CMFCPropertyPage)
	ON_CBN_SELCHANGE(IDC_CMB_USER, &CPageLoginGroup::OnCbnSelchangeCmbUser)
	ON_BN_CLICKED(IDC_BTN_NEXT, &CPageLoginGroup::OnBnClickedBtnNext)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CPageLoginGroup message handlers


BOOL CPageLoginGroup::OnInitDialog()
{
	CMFCPropertyPageEx::OnInitDialog();

	InitControlTextSize();

	m_btnNext.SubclassDlgItem(IDC_BTN_NEXT, this);
//	m_btnNext.SetImage(IDB_NEXT);
	m_btnNext.SetImage(IDB_LOGIN);

	if (m_wndImage.SubclassDlgItem(IDC_ST_IMAGE, this))
	{
		//	Initailize Image
		CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();
		if (FALSE == pConfigDB->LoadUserImage(_T("Operator"), &m_bmpOperator) ||
			FALSE == pConfigDB->LoadUserImage(_T("Engineer"), &m_bmpEngineer) ||
			FALSE == pConfigDB->LoadUserImage(_T("Administrator"), &m_bmpAdministrator))
		{
			
		}
	}

	//	Initialize ComboBox
	CComboBox* pCmbCtrl = static_cast<CComboBox*>(GetDlgItem(IDC_CMB_USER));
	CString strList[] = {
		_T("Operator"),
		_T("Engineer"),
		_T("Administrator"),
		_T(""),
	};

	for (INT nIndex = 0; strList[nIndex].GetLength(); nIndex++)
	{
		pCmbCtrl->AddString(strList[nIndex]);
	}

	if (pCmbCtrl->GetCount())
	{
		pCmbCtrl->SetCurSel(-1 != m_nSelUser ? m_nSelUser : 0);
		
		OnCbnSelchangeCmbUser();
	}

	SetTimer(0, 1, NULL);

	return TRUE;
}


void CPageLoginGroup::OnCbnSelchangeCmbUser()
{
	CBitmap* pImage = NULL;
	switch (static_cast<CComboBox*>(GetDlgItem(IDC_CMB_USER))->GetCurSel())
	{
	case 0:
	{
		pImage = &m_bmpOperator;
	}	break;

	case 1:
	{
		pImage = &m_bmpEngineer;
	}	break;

	case 2:
	{
		pImage = &m_bmpAdministrator;
	}	break;
	}

	if (pImage)
	{
		static_cast<CSheetLogin*>(GetParent())->SetUserImage(pImage);
		m_wndImage.LoadMainImage(pImage);
	}
}


void CPageLoginGroup::OnBnClickedBtnNext()
{
	if (0)
	{
		// Original
		ActivateNextPage();
	}
	else
	{
		CString strName, strPassword;

		GetDlgItemText(IDC_CMB_USER, strName);
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
}


BOOL CPageLoginGroup::OnKillActive()
{
	CString strName;
	GetDlgItemText(IDC_CMB_USER, strName);
	
	static_cast<CSheetLogin*>(GetParent())->SetUserName(strName);

	return CMFCPropertyPageEx::OnKillActive();
}


BOOL CPageLoginGroup::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message &&
		VK_RETURN == pMsg->wParam)
	{
		this->PostMessage(WM_COMMAND, IDC_BTN_NEXT);
	}

	return CMFCPropertyPageEx::PreTranslateMessage(pMsg);
}


void CPageLoginGroup::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 0:
	{
		KillTimer(nIDEvent);
		GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();
	}	break;
	}
	CMFCPropertyPageEx::OnTimer(nIDEvent);
}


void CPageLoginGroup::InitControlTextSize()
{
	if (true)
	{
		LOGFONT lf = { 0, };
		afxGlobalData.fontRegular.GetLogFont(&lf);

		lf.lfHeight = -15;
		m_BigFont.CreateFontIndirect(&lf);

		GetDlgItem(IDC_STATIC_USER)->SetFont(&m_BigFont);
		GetDlgItem(IDC_STATIC_PASSWORD)->SetFont(&m_BigFont);
		GetDlgItem(IDC_CMB_USER)->SetFont(&m_BigFont);
		GetDlgItem(IDC_EDIT_PASSWORD)->SetFont(&m_BigFont);

		m_BigFont.Detach();
	}

	/*if (true)
	{
	m_BigFont.CreateFont(FONT_SIZE, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, TEXT_STYLE);

	GetDlgItem(IDC_STATIC_USER)->SetFont(&m_BigFont);
	GetDlgItem(IDC_STATIC_PASSWORD)->SetFont(&m_BigFont);
	GetDlgItem(IDC_CMB_USER)->SetFont(&m_BigFont);
	GetDlgItem(IDC_EDIT_PASSWORD)->SetFont(&m_BigFont);

	m_BigFont.Detach();
	}*/

	
}
