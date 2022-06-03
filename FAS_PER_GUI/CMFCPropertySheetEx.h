#pragma once

#include "CDeviceMotionGate.h"

// CMFCPropertySheetEx

class CMFCPropertySheetEx : public CMFCPropertySheet
{
	DECLARE_DYNAMIC(CMFCPropertySheetEx)

public:
	CMFCPropertySheetEx();
	virtual ~CMFCPropertySheetEx();

protected:
	DECLARE_MESSAGE_MAP()
	
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	CPoint	m_ptStart;
	CSize	m_offset;

public:
	void RecalculateWizardButton();

protected:
	DWORD			m_dwAddress;

public:
	void			SetAddress(const DWORD dwAddress);
	const DWORD		GetAddress();

protected:
	CString	m_strCaption;

public:
	void			SetCaption(LPCTSTR lpszCaption);
	LPCTSTR			GetCaption();

public:
	virtual CDeviceMotionGate* GetMotiongateCtrl();

public:
	virtual const BOOL	SetActiveSafePage(const INT nPage);
	virtual void		AddSubPage(CMFCPropertyPage* pPage);
	virtual void		RemoveSubPages(const INT nDelimeter);

public:
	const BOOL	SetImageList(const INT nResID, const INT cx = 32, const COLORREF crMask = RGB(0xc0, 0xc0, 0xc0));

public:
	const BOOL	ShowWizardButton(const INT nResID, const INT nCmdShow = SW_SHOWNORMAL);
	const BOOL	DestroyWizardButton(const INT nResID);

protected:
	BOOL	m_bShowWizardButton;

public:
	void		ShowWizardButtons(const BOOL bShow = TRUE);
	const BOOL	IsWizardButtonsVisible();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual HRESULT accDoDefaultAction(VARIANT varChild);
	void InitTabTextSize();
	CFont m_BigFont;
	
};
