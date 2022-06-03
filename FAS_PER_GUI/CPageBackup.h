#pragma once

#include "CMFCButtonEx.h"
#include "CMFCPropertyPageEx.h"

// CPageBackup dialog

class CPageBackup : public CMFCPropertyPageEx
{
	DECLARE_DYNAMIC(CPageBackup)

public:
	CPageBackup(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPageBackup();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_BACKUP	};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

public:
	afx_msg void OnBnClickedBtnNext();
	afx_msg void OnBnClickedBtnBack();

protected:
	CMFCButtonEx	m_btnPrev, m_btnNext, m_btnNominal, m_btnGenernal;
	BOOL			m_bNominal;

public:
	afx_msg void OnBnClickedSelNominal();
	afx_msg void OnBnClickedSelGeneral();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnEnChangeEditComment();
	const BOOL IsGrantedNominal();
	const BOOL AskBackUpNominalMode();
	void InitControlTextSize();

	CFont m_BigFont;
};
