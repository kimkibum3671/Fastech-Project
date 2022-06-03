#pragma once

#include "CPageConfig.h"

#include "CMFCButtonEx.h"

// CPageConfigDeviceOrigin dialog

class CPageConfigDeviceOrigin : public CPageConfig
{
	DECLARE_DYNAMIC(CPageConfigDeviceOrigin)

public:
	CPageConfigDeviceOrigin(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPageConfigDeviceOrigin();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_CONFIG_DEVICE_ORIGIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeEditTimeout();

protected:
	INT m_nHomingTimeout;
public:
	virtual void OnOK();
};
