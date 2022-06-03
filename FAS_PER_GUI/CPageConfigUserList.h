#pragma once


// CPageConfigUserList dialog

class CPageConfigUserList: public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CPageConfigUserList)

public:
	CPageConfigUserList(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPageConfigUserList();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_CONFIG_USER_LIST };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
