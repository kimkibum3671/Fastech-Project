#pragma once


// CDlgDeviceList dialog

class CDlgDeviceList : public CDialogEx
{
	DECLARE_DYNCREATE(CDlgDeviceList)

public:
	CDlgDeviceList(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDlgDeviceList();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DEVICE_LIST };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnMod();
	afx_msg void OnBnClickedBtnDel();
	afx_msg void OnBnClickedOk();	
	afx_msg void OnLvnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);

protected:
	CMFCListCtrl m_listCtrl;
	INT m_nCurSel;
};
