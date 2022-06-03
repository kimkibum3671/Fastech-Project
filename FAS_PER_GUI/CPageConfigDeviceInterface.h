#pragma once

#include "CMFCPropertyPageEx.h"

// CPageConfigDeviceInterface dialog

class CPageConfigDeviceInterface : public CMFCPropertyPageEx
{
	DECLARE_DYNAMIC(CPageConfigDeviceInterface)

public:
	CPageConfigDeviceInterface(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPageConfigDeviceInterface();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_CONFIG_DEVICE_GENERAL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

protected:
	INT		m_nInterface;
	INT		m_nSerialPort;
	BOOL	m_bEnableMotionGate;
	BOOL	m_bConfirm;
public:
	afx_msg void OnBnClickedRadioTcp();
	afx_msg void OnBnClickedRadioSerial();
	afx_msg void OnBnClickedBtnScan();
	afx_msg void OnBnClickedRadioMgYes();
	afx_msg void OnBnClickedRadioMgNo();
	afx_msg void OnCbnSelchangeCmbSerial();
	afx_msg void OnCbnSelchangeCmbContinue();

protected:
	void ScanSerialPort(const INT nSelectedPort);
public:
	virtual void OnOK();
};
