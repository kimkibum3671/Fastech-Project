#pragma once

#include "CMFCPropertySheetEx.h"

#define USE_WIZARD_BUTTON

#ifdef USE_WIZARD_BUTTON
#include "CMFCButtonEx.h"
#define INTERNAL_BTN_PREV	IDC_BTN_BACK
#define INTERNAL_BTN_NEXT	IDC_BTN_NEXT
#endif
// CMFCPropertyPageEx dialog

class CMFCPropertyPageEx : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CMFCPropertyPageEx)

public:
	CMFCPropertyPageEx();
	CMFCPropertyPageEx(UINT nIDTemplate, UINT nIDCaption = 0);
	CMFCPropertyPageEx(LPCTSTR lpszTemplateName, UINT nIDCaption = 0);

	virtual ~CMFCPropertyPageEx();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CMFCPropertySheetEx* GetParent();

	const INT	GetCurrentPageIndex();
	const INT	GetPageCount();
	const BOOL	ActivatePage( const INT nPageIndex );
	const BOOL 	ActivatePrevPage();
	const BOOL 	ActivateNextPage();
	
	void		EndDialog(const INT nEndID);
	void		OnOK();
	void		OnCancel();	

#ifdef USE_WIZARD_BUTTON
protected:
	CMFCButtonEx m_btnPrev, m_btnNext;

public:
	void		SubclassWizardButton( const INT nPrevID = IDC_STATIC, const INT nNextID = IDC_STATIC);

	virtual void OnBnClickedBtnBack();
	virtual void OnBnClickedBtnNext();

	void EnablePrevButton(const BOOL bEnable = TRUE);
	void EnableNextButton(const BOOL bEnable = TRUE);
#endif
};
