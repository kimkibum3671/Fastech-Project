// CMFCPropertyPageEx.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CMFCPropertyPageEx.h"
#include "afxdialogex.h"

// CMFCPropertyPageEx dialog

IMPLEMENT_DYNAMIC(CMFCPropertyPageEx, CMFCPropertyPage)

CMFCPropertyPageEx::CMFCPropertyPageEx() :
	CMFCPropertyPage()
{
	
}

CMFCPropertyPageEx::CMFCPropertyPageEx(UINT nIDTemplate, UINT nIDCaption) :
	CMFCPropertyPage(nIDTemplate, nIDCaption)
{
	
}

CMFCPropertyPageEx::CMFCPropertyPageEx(LPCTSTR lpszTemplateName, UINT nIDCaption) :
	CMFCPropertyPage(lpszTemplateName, nIDCaption)
{
	
}

CMFCPropertyPageEx::~CMFCPropertyPageEx()
{
}

void CMFCPropertyPageEx::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMFCPropertyPageEx, CMFCPropertyPage)
#ifdef USE_WIZARD_BUTTON
	ON_BN_CLICKED(IDC_BTN_BACK, &CMFCPropertyPageEx::OnBnClickedBtnBack)
	ON_BN_CLICKED(IDC_BTN_NEXT, &CMFCPropertyPageEx::OnBnClickedBtnNext)
#endif
END_MESSAGE_MAP()


// CMFCPropertyPageEx message handlers
CMFCPropertySheetEx* CMFCPropertyPageEx::GetParent()
{
	return static_cast<CMFCPropertySheetEx*>(CMFCPropertyPage::GetParent());
}

void CMFCPropertyPageEx::SubclassWizardButton(const INT nPrevImgID, const INT nNextImgID)
{
	if (IDC_STATIC != nPrevImgID)
	{
		CWnd* pWnd = GetDlgItem(INTERNAL_BTN_PREV);
		if (pWnd->GetSafeHwnd() &&
			m_btnPrev.SubclassDlgItem(INTERNAL_BTN_PREV, this) )
		{
			m_btnPrev.SetImageEx(nPrevImgID);
		}
	}

	if (IDC_STATIC != nNextImgID)
	{
		CWnd* pWnd = GetDlgItem(INTERNAL_BTN_NEXT);
		if (pWnd->GetSafeHwnd() &&
			m_btnNext.SubclassDlgItem(INTERNAL_BTN_NEXT, this))
		{
			m_btnNext.SetImageEx(nNextImgID);
		}
	}
}

const INT CMFCPropertyPageEx::GetCurrentPageIndex()
{
	return GetParent()->GetActiveIndex();
}

const INT CMFCPropertyPageEx::GetPageCount()
{
	return GetParent()->GetPageCount();
}

const BOOL CMFCPropertyPageEx::ActivatePage(const INT nPageIndex)
{
	return GetParent()->SetActivePage(nPageIndex);
}

const BOOL CMFCPropertyPageEx::ActivatePrevPage()
{
	return GetParent()->SetActivePage(GetCurrentPageIndex() - 1);
}

const BOOL CMFCPropertyPageEx::ActivateNextPage()
{
	return GetParent()->SetActiveSafePage(GetCurrentPageIndex() + 1);
}

void CMFCPropertyPageEx::EndDialog(const INT nEndID)
{
	GetParent()->EndDialog(nEndID);
}

void CMFCPropertyPageEx::OnOK()
{
	GetParent()->EndDialog(IDOK);
}

void CMFCPropertyPageEx::OnCancel()
{
	GetParent()->EndDialog(IDCANCEL);
}

void CMFCPropertyPageEx::OnBnClickedBtnBack()
{
	ActivatePrevPage();
}

void CMFCPropertyPageEx::OnBnClickedBtnNext()
{
	ActivateNextPage();
}



void CMFCPropertyPageEx::EnablePrevButton(const BOOL bEnable)
{
	CWnd* pWnd = GetDlgItem(INTERNAL_BTN_PREV);
	if (pWnd->GetSafeHwnd() &&
		pWnd->IsWindowEnabled() != bEnable)
	{
		pWnd->EnableWindow(bEnable);
	}
}

void CMFCPropertyPageEx::EnableNextButton(const BOOL bEnable)
{
	CWnd* pWnd = GetDlgItem(INTERNAL_BTN_NEXT);
	if (pWnd->GetSafeHwnd() &&
		pWnd->IsWindowEnabled() != bEnable)
	{
		pWnd->EnableWindow(bEnable);
	}
}



