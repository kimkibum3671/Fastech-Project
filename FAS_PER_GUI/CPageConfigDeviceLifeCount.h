#pragma once


// CPageConfigDeviceLifeCount

class CPageConfigDeviceLifeCount : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CPageConfigDeviceLifeCount)

public:
	CPageConfigDeviceLifeCount();
	virtual ~CPageConfigDeviceLifeCount();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_CONFIG_DEVICE_LIFE_COUNT};
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	void LoadConfigurationValue();
	const BOOL SaveLifeCountData();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	void InitControlTextSize();
	CFont m_BigFont;
};


