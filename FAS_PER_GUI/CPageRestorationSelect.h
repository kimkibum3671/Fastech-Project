#pragma once

#include "CMFCPropertyPageEx.h"

#include "CMFCListCtrlEX.h"
#include "CMFCButtonEx.h"
#include "ColorStaticST.h"

// CPageRestorationSelect dialog

class CPageRestorationSelect : public CMFCPropertyPageEx
{
	DECLARE_DYNAMIC(CPageRestorationSelect)

public:
	CPageRestorationSelect(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPageRestorationSelect();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_RESTORATION_SELECT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg void OnDestroy();
	afx_msg void OnLvnItemchangedListMain(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedBtnBack();
	afx_msg void OnBnClickedBtnNext();	
	afx_msg void OnBnClickedBtnMainNominal();
	afx_msg void OnBnClickedBtnRecent();

public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();

protected:
	CMFCButtonEx m_btnPrev, m_btnNext;
	CMFCListCtrlEx m_wndList;

	CColorStaticST m_wndCaption[3], m_wndText[3];

	INT m_nNominalID, m_nRestoredID, m_nFirstSelectedID;

protected:
	void InitializeListCtrl();
	void LoadProjectList();
	void DisplaySelectedBackupInfo();
	void SetNominalData(const INT nSelectIndex);
};
