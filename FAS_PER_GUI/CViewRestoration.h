#pragma once

#include "ColorStaticST.h"

#include "CViewDevice.h"
#include "CMFCButtonEx.h"
#include "CMFCListCtrlEX.h"

// CViewRestoration form view

class CViewRestoration : public CViewDevice
{
	DECLARE_DYNCREATE(CViewRestoration)

protected:
	CViewRestoration();           // protected constructor used by dynamic creation
	virtual ~CViewRestoration();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RESTORATION };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLvnItemchangedListMain(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnBnClickedBtnHoming();
	afx_msg void OnBnClickedBtnStart();

protected:
	typedef enum _TIMER_ID
	{
		TIMER_INITIALIZE = 0,
		TIMER_POLLING,
	}	TIMER_ID;

	typedef enum _TIMER_ELPASE
	{
		ELAPSE_TIMER_INITIALIZE = 1,
		ELAPSE_TIMER_POLLING = 100,
	}	TIMER_ELPASE;

	typedef enum _COMPARE_METHOD
	{
		compare_nothing = 0,
		compare_backup,
		compare_device,
	}	COMPARE_METHOD;

protected:
	CColorStaticST m_wndNameRegister, m_wndNameDevice;
	CString m_strProjectName;
	CMFCButtonEx m_btnEnableCompare;

	BOOL	m_bAutoHoming, m_bAutoRestoration;
	INT		m_nCompareMethod;

	CMFCListCtrlEx m_listCtrl;

protected:
	void InitializeListCtrl();

	void DisplayNames(CDeviceMotionGate* pDevice);

	void ResizeWindows();

	void LoadProjectList(LPCTSTR lpszProjectName);

public:
	virtual void PrepareDestroy();
};
