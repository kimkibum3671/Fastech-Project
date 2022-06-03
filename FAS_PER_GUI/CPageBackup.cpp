// CPageBackup.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageBackup.h"
#include "afxdialogex.h"

#include "CSheetBackup.h"
#include "CDataBaseManager.h"
// CPageBackup dialog

IMPLEMENT_DYNAMIC(CPageBackup, CMFCPropertyPageEx)

CPageBackup::CPageBackup(CWnd* pParent /*=nullptr*/)
	: CMFCPropertyPageEx(IDP_BACKUP)
{
	m_bNominal = FALSE;
}

CPageBackup::~CPageBackup()
{
	
}

void CPageBackup::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageBackup, CMFCPropertyPage)
	ON_BN_CLICKED(IDC_BTN_NEXT, &CPageBackup::OnBnClickedBtnNext)
	ON_BN_CLICKED(IDC_BTN_BACK, &CPageBackup::OnBnClickedBtnBack)
	ON_BN_CLICKED(IDC_SEL_NOMINAL, &CPageBackup::OnBnClickedSelNominal)
	ON_BN_CLICKED(IDC_SEL_GENERAL, &CPageBackup::OnBnClickedSelGeneral)
	ON_WM_TIMER()
	ON_WM_SETFOCUS()
	ON_EN_CHANGE(IDC_EDIT_COMMENT, &CPageBackup::OnEnChangeEditComment)
END_MESSAGE_MAP()


// CPageBackup message handlers
void CPageBackup::OnBnClickedBtnBack()
{
	KillTimer(1);
	ActivatePrevPage();
}


void CPageBackup::OnBnClickedBtnNext()
{
	CSheetBackup* pParent = static_cast<CSheetBackup*>(GetParent());
	
	CString strComment;
	GetDlgItemText(IDC_EDIT_COMMENT, strComment);
	pParent->SetComment(strComment);
	pParent->EnableNominalBackup(m_bNominal);
	if (m_bNominal)
	{
		if (AskBackUpNominalMode())
		{
			OnOK();
		}
	}
	else
	{
		OnOK();
	}
}


BOOL CPageBackup::OnInitDialog()
{
	CMFCPropertyPage::OnInitDialog();

	m_btnPrev.SubclassDlgItem(IDC_BTN_BACK, this);
	m_btnPrev.SetImage(IDB_BACK);

	m_btnNext.SubclassDlgItem(IDC_BTN_NEXT, this);
	m_btnNext.SetImage(IDB_START);
	
	m_btnNominal.SubclassDlgItem(IDC_SEL_NOMINAL, this);
	m_btnNominal.SetImage(IDB_FLAG_OFF);

	m_btnGenernal.SubclassDlgItem(IDC_SEL_GENERAL, this);
	m_btnGenernal.SetImage(IDB_FLAG_ON);

	m_bNominal = FALSE;	

	InitControlTextSize();
	
	SetTimer(0, 100, NULL);

	(GetDlgItem(IDC_SEL_NOMINAL))->EnableWindow(IsGrantedNominal());
	return TRUE;
}


void CPageBackup::OnBnClickedSelNominal()
{
	if (FALSE == m_bNominal)
	{
		m_bNominal = TRUE;
		m_btnNominal.SetImage(IDB_FLAG_ON);
		m_btnNominal.Invalidate(FALSE);
		m_btnGenernal.SetImage(IDB_FLAG_OFF);
		m_btnGenernal.Invalidate(FALSE);
		GetDlgItem(IDC_EDIT_COMMENT)->SetFocus();
		SetTimer(1, 100, NULL);
	}
}


void CPageBackup::OnBnClickedSelGeneral()
{
	if (m_bNominal)
	{
		m_bNominal = FALSE;
		m_btnNominal.SetImage(IDB_FLAG_OFF);
		m_btnNominal.Invalidate(FALSE);
		m_btnGenernal.SetImage(IDB_FLAG_ON);
		m_btnGenernal.Invalidate(FALSE);
		GetDlgItem(IDC_EDIT_COMMENT)->SetFocus();
		m_btnNext.EnableWindow(TRUE);
	}
}


void CPageBackup::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 0:
	{
		KillTimer(nIDEvent);
		GetDlgItem(IDC_EDIT_COMMENT)->SetFocus();
	}	break;

	case 1:
	{
		KillTimer(nIDEvent);

		CString strComment;
		GetDlgItemText(IDC_EDIT_COMMENT, strComment);
		if (0 == strComment.GetLength())
		{
			m_btnNext.EnableWindow(FALSE);
		}
		else
		{
			m_btnNext.EnableWindow(TRUE);
		}
		
	}
	}

	CMFCPropertyPage::OnTimer(nIDEvent);
}


void CPageBackup::OnSetFocus(CWnd* pOldWnd)
{
	CMFCPropertyPage::OnSetFocus(pOldWnd);

	SetTimer(0, 100, NULL);
}


void CPageBackup::OnEnChangeEditComment()
{
	CString strComment;
	GetDlgItemText(IDC_EDIT_COMMENT, strComment);
	if (0 == strComment.GetLength())
	{
		m_btnNext.EnableWindow(FALSE);
	}
	else
	{
		m_btnNext.EnableWindow(TRUE);
	}

	/*CString strComment;
	GetDlgItemText(IDC_EDIT_COMMENT, strComment);

	GetDlgItem(IDC_BTN_NEXT)->EnableWindow(strComment.GetLength() ? TRUE : FALSE);*/
}


const BOOL CPageBackup::IsGrantedNominal()
{
	CString str;
	CDataBaseConfig* pConfig = CDataBaseConfig::GetInstance();
	ASSERT(pConfig);
	str = pConfig->GetOperatorName();
	return (str == L"Engineer" || str == L"Administrator") ? TRUE : FALSE;
}


const BOOL CPageBackup::AskBackUpNominalMode()
{
	BOOL bResult = FALSE;

	CString message("");
	CString mainInstruction("Would you want to backup this system to 'Nominal mode'?");
	CString title(_T("Confirm to BackUp the system"));

	CTaskDialog taskDialog(message, mainInstruction, title, !(TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON));

	taskDialog.AddCommandControl(IDYES, _T("Yes, Proceed with the 'Nominal Mode' backup."));
	taskDialog.AddCommandControl(IDNO, _T("No, Cancel backup mode"));

	taskDialog.SetDialogWidth(350);

	switch (taskDialog.DoModal())
	{
	case IDYES:
	{
		bResult = TRUE;
	}	break;

	case IDNO:
	{
		bResult = FALSE;
	}	break;
	}

	return bResult;
}


void CPageBackup::InitControlTextSize()
{
	LOGFONT lf = { 0, };
	afxGlobalData.fontRegular.GetLogFont(&lf);

	lf.lfHeight = -15;
	m_BigFont.CreateFontIndirect(&lf);

	GetDlgItem(IDC_STATIC_COMMENT)->SetFont(&m_BigFont);
	GetDlgItem(IDC_EDIT_COMMENT)->SetFont(&m_BigFont);
	

	m_BigFont.Detach();
}
