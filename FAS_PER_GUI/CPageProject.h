#pragma once

#include "ColorStaticST.h"
#include "CMFCButtonEx.h"
#include "CMFCPropertyPageEx.h"

#include "CStaticImageViewer.h"

// CPageProject dialog

class CPageProject : public CMFCPropertyPageEx
{
	DECLARE_DYNAMIC(CPageProject)

public:
	CPageProject(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPageProject();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_PROJECT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedBtnNext();

	virtual BOOL OnInitDialog();

protected:
	CColorStaticST		m_wndColor[6];
	CMFCButtonEx		m_btnNext;
	CStaticImageViewer	m_wndViewer;
	CFont m_BigFont;

	BOOL	m_bSystemCtrl;
public:
	void SetSystemControl(BOOL bCtrl);
	BOOL IsSystemCtrl();
	void LoadSystemControlImage(CString strName);
	void InitControlTextSize();
};
