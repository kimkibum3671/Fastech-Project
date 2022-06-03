#pragma once


// CDlgSystemOption dialog

class CDlgSystemOption : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSystemOption)

public:
	CDlgSystemOption(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDlgSystemOption();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYSTEM_OPTION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
