#pragma once

#include "CDeviceMotionGate.h"

// CPageConfigDeviceSocket dialog

class CPageConfigDeviceSocket : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CPageConfigDeviceSocket)

public:
	CPageConfigDeviceSocket(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPageConfigDeviceSocket();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_CONFIG_DEVICE_SOCKET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

protected:
	CMFCListCtrl m_listCtrl;
	INT m_nCurSel;

public:
	afx_msg void OnLvnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnMod();
	afx_msg void OnBnClickedBtnDel();

protected:
	CList<ULONGLONG, ULONGLONG> m_lstAddressHistory;

public:
	virtual void OnOK();

public:
	CDeviceMotionGate* ScanAddress(const DWORD dwAddress);
	CDeviceMotionGate* ScanName(LPCTSTR lpszSystemName);
};
