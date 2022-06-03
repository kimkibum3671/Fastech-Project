#pragma once


// CPageConfigDeviceTest

class CPageConfigDeviceTest : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CPageConfigDeviceTest)

public:
	CPageConfigDeviceTest();
	virtual ~CPageConfigDeviceTest();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_CONFIG_DEVICE_TEST };
#endif


protected:
	DECLARE_MESSAGE_MAP()


public:
	afx_msg void OnBnClickedButton1();
};


