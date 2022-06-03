#pragma once


// CPageConfigDeviceSerial dialog

class CPageConfigDeviceSerial : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CPageConfigDeviceSerial)

public:
	CPageConfigDeviceSerial(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPageConfigDeviceSerial();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_CONFIG_DEVICE_SERIAL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
