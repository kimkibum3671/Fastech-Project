#pragma once

#include "CMFCListCtrlEX.h"
#include "CMFCButtonEx.h"
#include "ColorStaticST.h"

// CDlgManageBackup dialog

class CDlgManageBackup : public CDialogEx
{
	DECLARE_DYNCREATE(CDlgManageBackup)

public:
	CDlgManageBackup(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDlgManageBackup();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MANAGE_BACKUP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnRecent();
	afx_msg void OnBnClickedBtnMainNominal();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnChangeNominal();
	afx_msg void OnBnClickedBtnDelete();

protected:
	DWORD m_dwAddress;

	CMFCButtonEx m_btnPrev, m_btnNext;
	CMFCListCtrlEx m_wndList;

	CColorStaticST m_wndCaption[2], m_wndText[2];

	INT m_nCurNominalID, m_nNewNominalID, m_nRestoredID, m_nItemCount;

	CList<INT, INT> m_lstItemID;

	BOOL m_bSystemControl;
	CString m_strSystemName;

	CFont m_BigFont;

protected:
	void InitializeListCtrl();
	void LoadProjectList();
	void SetNominalData( const INT nSelectIndex );

public:
	afx_msg void OnLvnItemchangedListMain(NMHDR* pNMHDR, LRESULT* pResult);
	
	void		SetAddress(const DWORD dwAddress);
	const DWORD	GetAddress();
	
	POSITION	GetDeleteItemStartPosition();
	const INT	GetNextDeleteItem(POSITION& position);

	const BOOL	IsNominalSelectionChanged();
	const INT	GetSelectedNominalID();

	const BOOL	IsExistLastestBackupID();
	
	void SetSystemControl(BOOL bCtrl);
	void SetSystemControlDeviceName(CString str);
	void InitControlTextSize();
	BOOL IsSystemControl();
	CString GetSystemControlDeviceName();
	
};
