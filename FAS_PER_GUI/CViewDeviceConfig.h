#pragma once

#include "CViewDevice.h"
#include "ColorStaticST.h"
#include "CMFCButtonEx.h"

#include "CStaticImageViewer.h"

// CViewDeviceConfig form view

class CViewDeviceConfig : public CViewDevice
{
	DECLARE_DYNCREATE(CViewDeviceConfig)

protected:
	CViewDeviceConfig();           // protected constructor used by dynamic creation
	virtual ~CViewDeviceConfig();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DEVICE_CONFIG };
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
	
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedRadioRegister();
	afx_msg void OnBnClickedRadioDevice();
	afx_msg void OnBnClickedRadioNew();
	afx_msg void OnBnClickedBtnHomingTimeout();
	afx_msg void OnBnClickedBtnExecute();
	afx_msg void OnEnChangeEditNew();

	virtual void OnInitialUpdate();

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
	CColorStaticST		m_wndColor[6];
	CStaticImageViewer	m_wndViewer;
};
