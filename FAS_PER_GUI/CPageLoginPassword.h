#pragma once

#include "CMFCPropertyPageEx.h"

#include "CStaticImageViewer.h"
#include "CMFCButtonEx.h"

// CPageLoginPassword dialog

class CPageLoginPassword : public CMFCPropertyPageEx
{
	DECLARE_DYNAMIC(CPageLoginPassword)

public:
	CPageLoginPassword(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPageLoginPassword();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_LOGIN_PASSWORD };
#endif

protected:
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();	
	
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnBnClickedBtnPrev();
	afx_msg void OnBnClickedBtnNext();

protected:
	CStaticImageViewer	m_wndImage;
	CMFCButtonEx		m_btnPrev, m_btnNext;

protected:
	typedef enum _TIMER_ID
	{
		TIMER_SET_DEFAULT_FOCUS = 0,
	}	TIMER_ID;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
//	afx_msg void OnEnChangeEditPassword();
};
