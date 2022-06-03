#pragma once

#include "CMFCListCtrlEx.h"
#include "CDeviceMotionGate.h"
#include "ColorStaticST.h"
// CDlgNoticeSystem dialog



class CDlgNoticeSystem : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgNoticeSystem)

public:
	CDlgNoticeSystem(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDlgNoticeSystem();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NOTICE_NEW_SYSTEM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	virtual BOOL OnInitDialog();

protected:
	CMFCListCtrlEx m_wndList;

public:
	const BOOL IsDeviceOK(const DWORD dwAddress);
	const DWORD GetSelectedDeviceAddress();


protected:
	CList<DWORD, DWORD> m_lstDevice;
	BOOL m_bReload;
	DWORD m_dwSelectedDeviceAddress;
	BOOL m_bStart;
protected:
	const INT FindRegisteredAddress(const DWORD dwAddress);
	void AddListItem(const DWORD dwAddress);
	void ReportSelectedDeviceInfo(const INT nIndex);

protected:
	typedef enum _TIMER_ID
	{
		TIMER_ADD_DEVICE = 0,
	}	TIMER_ID;

	typedef enum _INFORMATION_ID
	{
		INFORM_MOTOR_COUNT = 0,
		INFORM_HOMING_TIMEOUT,
		INFORM_MAX
	} INFORMATION_ID;
	
	CColorStaticST m_wndText[INFORM_MAX];

public:
	afx_msg void OnLvnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	void SetImage();
	void FilterRegisteredAddress(const DWORD dwAddress);
	void SetButtonImage(INT nID, CString strFilePath, CString strBtnText);
	BOOL IsProgramStart() { return m_bStart; }
//	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedMfcbuttonConfig();
	//CBitmapButton m_btnTest;
	//CBitmapButton m_btnTest;
	int GetListCount();
	const BOOL IsErrorDeviceExist();
	void InitControlTextSize();
	CFont m_BigFont;
};
