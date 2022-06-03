#pragma once
#include "MainFrm.h"
#include "CStaticImageViewer.h"

// CPageConfigDeviceControlAuthorize

class CPageConfigDeviceControlAuthorize : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CPageConfigDeviceControlAuthorize)

public:
	CPageConfigDeviceControlAuthorize();
	virtual ~CPageConfigDeviceControlAuthorize();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_CONFIG_DEVICE_CONTROL_AUTHORIZE};
#endif
protected:
	DECLARE_MESSAGE_MAP()
public:
	CPageConfigDeviceControlAuthorize* GetInstance();
	virtual void OnOK();
	BOOL SaveLifeCountData();
	void LoadConfigurationValue();
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	CONFIG_NODE m_Admin, m_Engineer, m_Operator, m_Guest;
	PASSWORD_NODE m_AdminPassword, m_EngineerPassword, m_OperatorPassword, m_GuestPassword;
public:
	void LoadFromDataBase();
	void InitButtonCtrl();
	const BOOL SaveControlDataBase();
	
	CFont m_BigFont;

public:
	CStaticImageViewer m_wndAdminImage[2], m_wndEngineerImage[2], m_wndOperatorImage[2], m_wndGuestImage[2];
	CBitmap m_bmpAdministrator, m_bmpEngineer, m_bmpOperator, m_bmpGuest;
	void GetLoadImage();
	void SetButtonImage(INT nID, BOOL bCheck);
	BOOL ChangeSystemPassword(INT nID);
	void SetAuthorizedPassword(PASSWORD_NODE node);
	BOOL SavePassword();
	const BOOL SavePasswordToDataBase(PASSWORD_NODE node);
	void SetSettingButtonImage();
	void InitControlTextSize();
};



