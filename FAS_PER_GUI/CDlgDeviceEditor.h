#pragma once


// CDlgDeviceEditor dialog

class CDlgDeviceEditor : public CDialogEx
{
	DECLARE_DYNCREATE(CDlgDeviceEditor)

public:
	CDlgDeviceEditor(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDlgDeviceEditor();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DEVICE_EDITOR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnEnChangeEdit();

protected:
	DWORD	m_dwAddress;
	CString	m_strName;
	INT		m_nMotorCount;

protected:
	void CheckButtonStatus();

public:
	void		SetAddress(const DWORD dwAddress);
	const DWORD	GetAddress();

	void		SetName(LPCTSTR lpszName);
	LPCTSTR		GetName();

	void		SetCount(const INT nCount);
	const INT	GetCount();	
};
