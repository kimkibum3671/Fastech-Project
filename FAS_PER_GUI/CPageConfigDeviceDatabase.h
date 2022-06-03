#pragma once

#include "CMFCListCtrlEX.h"

// CPageConfigDeviceDatabase dialog

class CPageConfigDeviceDatabase : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CPageConfigDeviceDatabase)

public:
	CPageConfigDeviceDatabase(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPageConfigDeviceDatabase();
	
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_CONFIG_DEVICE_DATABASE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg void OnDestroy();

public:
	virtual BOOL OnInitDialog();

protected:
	CMFCListCtrlEx m_listCtrl;
	CList<CString, CString&> m_BackupDelList;
	CFont m_BigFont;

protected:
	void		SetListItem(LPVOID pDeviceMotiongate, const INT nIndex = -1);
	const BOOL	IsValidSystemName(LPCTSTR lpszSystemName, LPVOID pDeviceMotiongate = NULL);
	void		ShowInvalidSystemName();
	void		ShowEditPropertySheet(LPVOID pDeviceMotiongate, const INT nItem, const BOOL bInsert = FALSE);

public:
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnMod();
	afx_msg void OnLvnItemchangedListDevice(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedBtnDel();
	virtual void OnOK();
	afx_msg void OnNMDblclkListDevice(NMHDR* pNMHDR, LRESULT* pResult);
	const BOOL IsValidDeviceID(const INT nDeviceID, LPVOID pDeviceMotiongate);
	void InitControlTextSize();
};
