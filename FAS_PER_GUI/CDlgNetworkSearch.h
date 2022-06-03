#pragma once

#include "MainFrm.h"
// CDlgNetworkSearch 대화 상자

class CDlgNetworkSearch : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgNetworkSearch)

public:
	CDlgNetworkSearch(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgNetworkSearch();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NETWORK_SEARCH };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
protected:
	CWnd* m_pParent;

public:
	typedef struct _NETWORK_INFORM
	{
		DWORD dwAddress;
		MOTIONGATE_READ_INFO info;
	}	NETWORK_INFORM;

	CList<NETWORK_INFORM, NETWORK_INFORM> m_list;
	
	typedef struct _NETWORK_SAVE
	{
		DWORD dwAddress;
		CString strAddress;
	}	NETWORK_SAVE;

	
	CList<NETWORK_SAVE, NETWORK_SAVE> m_save;
	virtual BOOL OnInitDialog();
	//const BOOL SearchingNetwork();
	const BOOL SearchingNetwork(BOOL bStop);
	const INT ConnectDevice(DWORD dwAddress);
	const INT GetMotionGateInform(DWORD dwAddress, MOTIONGATE_READ_INFO& info);
	const BOOL SearchingDeviceInform(DWORD dwAddress);
	const DWORD GetAddress(CString lpszAddress);
	const INT CloseDevice(DWORD dwAddress);
	CString ConvertSystemName(MOTIONGATE_READ_INFO info);
	CString GetAddressString(DWORD dwAddress);

	afx_msg void OnBnClickedButtonSearch();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
//	afx_msg void OnNMClickListNetwork(NMHDR* pNMHDR, LRESULT* pResult);
//	afx_msg void OnLvnItemchangedListNetwork(NMHDR* pNMHDR, LRESULT* pResult);
	const BOOL CheckState(INT nIndex);

	CProgressCtrl m_Progress;
	CMFCListCtrlEx m_listCtrl;
	CString m_strStartAddress;
	CString m_strEndAddress;
	BOOL m_bStop;
	BOOL m_bOutlookSearch;
	INT m_nID;

	void ShowButton(BOOL bEnable);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void SetStartAddress(CString strAddress);
	void SetEndAddress(CString strAddress);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void SetIPControl();
	afx_msg void OnBnClickedButtonStop();
	const BOOL IsSearchingStop();
	afx_msg void OnBnClickedButtonRun();
	afx_msg void OnBnClickedCheckAll();
	void SetOutlookSearch(BOOL bOutlook);
	const BOOL IsOutlookSearch();
	const BOOL IsIPOverlap(DWORD dwAddress);
	afx_msg void OnBnClickedButtonAll();
	afx_msg void OnIpnFieldchangedIpaddressStart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnIpnFieldchangedIpaddressEnd(NMHDR* pNMHDR, LRESULT* pResult);
	void InitControlTextSize();
	CFont m_BigFont;
};
