#pragma once

#include "CMFCPropertyPageEx.h"
#include "CStaticImageViewer.h"
#include "CMFCButtonEx.h"

// CPageLoginGroup dialog

class CPageLoginGroup : public CMFCPropertyPageEx
{
	DECLARE_DYNAMIC(CPageLoginGroup)

public:
	CPageLoginGroup(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPageLoginGroup();	

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_LOGIN_GROUP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg void OnCbnSelchangeCmbUser();
	afx_msg void OnBnClickedBtnNext();

public:
	virtual BOOL OnInitDialog();

protected:
	INT		m_nSelUser;
	CStaticImageViewer m_wndImage;

	CBitmap m_bmpOperator, m_bmpEngineer, m_bmpAdministrator;

	CMFCButtonEx m_btnNext;

	CFont m_BigFont;
public:
	virtual BOOL OnKillActive();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void InitControlTextSize();
};
