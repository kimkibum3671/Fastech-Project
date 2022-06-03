#pragma once

#include "CViewDevice.h"
#include "ColorStaticST.h"
#include "CMFCButtonEx.h"
// CViewBackup form view

class CViewBackup : public CViewDevice
{
	DECLARE_DYNCREATE(CViewBackup)

protected:
	CViewBackup();           // protected constructor used by dynamic creation
	virtual ~CViewBackup();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DEVICE_BACKUP };
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
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnRandom();
	afx_msg void OnBnClickedBtnNominal();
	afx_msg void OnEnChangeEdit();

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

protected:
	CColorStaticST m_wndNameRegister, m_wndNameDevice;
	CString m_strProjectName;

protected:
	void DisplayNames(CDeviceMotionGate* pDevice);
};
