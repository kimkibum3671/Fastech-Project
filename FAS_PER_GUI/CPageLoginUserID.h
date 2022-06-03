#pragma once

#include "CMFCPropertyPageEx.h"

// CPageLoginUserID dialog

class CPageLoginUserID : public CMFCPropertyPageEx
{
	DECLARE_DYNAMIC(CPageLoginUserID)

public:
	CPageLoginUserID(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPageLoginUserID();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_LOGIN_USER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg void OnCbnSelchangeCmbUser();

	virtual BOOL OnInitDialog();

protected:
	INT m_nSelUser;

	CBitmap m_bmpUnKnown;

	
public:
	afx_msg void OnBnClickedBtnNext();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
