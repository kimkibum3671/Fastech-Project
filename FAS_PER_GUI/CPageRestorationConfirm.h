#pragma once

#include "ColorStaticST.h"
#include "CMFCButtonEx.h"
#include "CMFCPropertyPageEx.h"

// CPageRestorationConfirm dialog

class CPageRestorationConfirm : public CMFCPropertyPageEx
{
	DECLARE_DYNAMIC(CPageRestorationConfirm)

public:
	CPageRestorationConfirm(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPageRestorationConfirm();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_RESTORATION_CONFIRM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	BOOL m_bCompareType;
	
	virtual BOOL OnSetActive();
	virtual BOOL OnInitDialog();

protected:
	CColorStaticST	m_wndCaption[7];
	CColorStaticST	m_wndText[7];

	CMFCButtonEx	m_btnBack, m_btnStart, m_btnHome;
	CFont m_BigFont;
public:
	afx_msg void OnBnClickedBtnBack();
	afx_msg void OnBnClickedBtnNext();
	afx_msg void OnBnClickedBtnHoming();
	const BOOL LoadBackupFileMotorIndex(LPVOID pDatabaseRecordData, INT& nIndex);
	void InitControlTextSize();
};
