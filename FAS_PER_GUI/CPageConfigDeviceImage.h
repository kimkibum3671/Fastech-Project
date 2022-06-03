#pragma once

#include "CPageConfig.h"

#include "CMFCButtonEx.h"
#include "CStaticImageViewer.h"

// CPageConfigDeviceImage dialog

class CPageConfigDeviceImage : public CPageConfig
{
	DECLARE_DYNAMIC(CPageConfigDeviceImage)

public:
	CPageConfigDeviceImage(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPageConfigDeviceImage();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_CONFIG_DEVICE_IMAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

protected:
	CStaticImageViewer m_wndViewer;

public:
	afx_msg void OnBnClickedBtnDefault();
	afx_msg void OnBnClickedBtnLoad();
	afx_msg void OnBnClickedBtnSave();

protected:
	CBitmap m_bmpImage;

public:
	virtual void OnOK();
};
