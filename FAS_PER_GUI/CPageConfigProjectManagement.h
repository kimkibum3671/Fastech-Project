#pragma once
#include "CMFCListCtrlEX.h"

// CPageConfigProjectManagement

class CPageConfigProjectManagement : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CPageConfigProjectManagement)

public:
	CPageConfigProjectManagement();
	virtual ~CPageConfigProjectManagement();


#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_CONFIG_DEVICE_PROJECT_MANAGE};
#endif

protected:
	DECLARE_MESSAGE_MAP()

protected:
	CMFCListCtrlEx m_listCtrl;
public:
	virtual BOOL OnInitDialog();
	void SetListItem(LPVOID pDeviceMotiongate, const INT nIndex = -1);
	afx_msg void OnLvnItemchangedListDevice(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedBtnDelete();
};


