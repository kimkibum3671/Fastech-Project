#pragma once

#include "CMFCListCtrlEX.h"
#include "CDeviceList.h"
// CDlgNamingAssistant dialog

class CDlgNamingAssistant : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgNamingAssistant)

public:
	CDlgNamingAssistant(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDlgNamingAssistant();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DEVICE_NAME };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg void OnDestroy();
	afx_msg void OnNMClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnListItemChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedOk();

public:
	virtual BOOL OnInitDialog();

protected:
	CMap<CDeviceMotionGate*, CDeviceMotionGate*, CDeviceMotionGate*, CDeviceMotionGate*>	m_mapDevice;
	CMFCListCtrlEx				m_wndList;
	CMutex	m_mutexInternal;
	CStringArray m_arrString;

protected:
	void InsertItem(CDeviceMotionGate* pDevice, CListCtrl* pListCtrl = NULL);
	void ShowComboBox(const INT nItem, const INT nColumn);
	void ShowEditBox(const INT nItem, const INT nColumn);

	const INT GetSelectedItemIndex(const INT nItem, const INT nColumn = 3);

public:
	void AddDevice(CDeviceMotionGate* pDevice);

protected:
	CWnd*	m_pWndTarget;
	UINT	m_nMsgID;

public:
	void SetMessageInformation(CWnd* pTarget, UINT nMsgID);
};
