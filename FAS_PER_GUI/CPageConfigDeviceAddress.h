#pragma once

#include "CPageConfig.h"

#include "CMFCButtonEx.h"

// CPageConfigDeviceAddress dialog

class CPageConfigDeviceAddress : public CPageConfig
{
	DECLARE_DYNAMIC(CPageConfigDeviceAddress)

public:
	CPageConfigDeviceAddress(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPageConfigDeviceAddress();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_CONFIG_DEVICE_ADDRESS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg void OnEnChangeEditSystem();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();

protected:
	typedef enum _TIMER_ID
	{
		TIMER_SHOW_WIZARD_BUTTON = 0,
	}	TIMER_ID;

protected:
	CString	m_strSystemName;
	CString m_strDeviceID;
	INT		m_nCheckName;

protected:
	void ShowWizardButtonOK();
	void CreateNamingWizard();

public:
	BOOL m_bOverlapDeviceID;

public:
	const INT CheckSystemName();
	afx_msg void OnBnClickedBtnGenerate();
	afx_msg void OnEnChangeEditDeviceId();
};
