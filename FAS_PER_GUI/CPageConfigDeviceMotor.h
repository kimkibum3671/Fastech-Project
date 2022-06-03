#pragma once

#include "CPageConfig.h"

#include "CMFCButtonEx.h"

// CPageConfigDeviceMotor dialog

class CPageConfigDeviceMotor : public CPageConfig
{
	DECLARE_DYNAMIC(CPageConfigDeviceMotor)

public:
	CPageConfigDeviceMotor(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPageConfigDeviceMotor();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_CONFIG_DEVICE_MOTOR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedRadioMotor1();
	afx_msg void OnBnClickedRadioMotor2();
	afx_msg void OnBnClickedRadioMotor3();
	afx_msg void OnBnClickedRadioMotor4();
	afx_msg void OnBnClickedRadioMotor5();
	afx_msg void OnBnClickedRadioMotor6();
	afx_msg void OnBnClickedRadioMotor7();
	afx_msg void OnBnClickedRadioMotor8();
	
protected:
	INT m_nMotorCount;

public:
	void ChangeMotorCount(const INT nCount);
	virtual void OnOK();
};
