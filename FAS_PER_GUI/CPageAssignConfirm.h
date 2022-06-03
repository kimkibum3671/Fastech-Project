#pragma once

#include "ColorStaticST.h"
#include "CMFCButtonEx.h"
#include "CMFCPropertyPageEx.h"

#include "CStaticImageViewer.h"

// CPageAssignConfirm dialog

class CPageAssignConfirm : public CMFCPropertyPageEx
{
	DECLARE_DYNAMIC(CPageAssignConfirm)

public:
	CPageAssignConfirm(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPageAssignConfirm();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_ASSIGN_CONFIRM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedBtnBack();
	afx_msg void OnBnClickedBtnNext();

public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();

protected:
	CColorStaticST		m_wndColor[8];
	CMFCButtonEx		m_btnStart, m_btnPrev;
	CStaticImageViewer	m_wndViewer;
};
