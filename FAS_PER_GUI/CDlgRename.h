#pragma once

#include "CDeviceList.h"

// CDlgRename dialog

class CDlgRename : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRename)

public:
	CDlgRename(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDlgRename();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RENAME };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	CDeviceMotionGate* m_pDevice;

public:
	void SetTargetDevice(CDeviceMotionGate* pDevice );
	virtual BOOL OnInitDialog();

protected:
	typedef enum _TIMER_ID
	{
		TIMER_INIT = 0,
	}	TIMER_ID;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOk();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
