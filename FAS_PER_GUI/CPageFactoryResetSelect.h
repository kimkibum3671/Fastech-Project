#pragma once

#include "CMFCButtonEx.h"
#include "CMFCPropertyPageEx.h"

#include "CStaticImageViewer.h"

// CPageFactoryResetSelect dialog

class CPageFactoryResetSelect : public CMFCPropertyPageEx
{
	DECLARE_DYNAMIC(CPageFactoryResetSelect)

public:
	CPageFactoryResetSelect(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPageFactoryResetSelect();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_FACTORY_RESET_SELECT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedBtnBack();
	afx_msg void OnBnClickedBtnNext();
	afx_msg void OnBnClickedBtnMotor1();
	afx_msg void OnBnClickedBtnMotor2();
	afx_msg void OnBnClickedBtnMotor3();
	afx_msg void OnBnClickedBtnMotor4();
	afx_msg void OnBnClickedBtnMotor5();
	afx_msg void OnBnClickedBtnMotor6();
	afx_msg void OnBnClickedBtnMotor7();
	afx_msg void OnBnClickedBtnMotor8();

public:
	virtual BOOL OnInitDialog();

protected:
	CMFCButtonEx m_btnMotor[8], m_btnBack, m_btnNext;
	CStaticImageViewer	m_wndViewer;

protected:
	void OnMotorSelected(const INT nResID);
};
