#pragma once
#include "CMFCListCtrlEX.h"

// CDlgSystemHistory 대화 상자

class CDlgSystemHistory : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSystemHistory)

public:
	CDlgSystemHistory(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgSystemHistory();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYSTEM_HISTORY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	typedef struct _OPERATIONLOG_INFORMATION
	{	//	Log DB에 저장할 내용
		INT		nLevel;
		CString strTime;
		CString strDeviceName;
		CString strDeviceIP;
		CString strInform;
	}	OPERATIONLOG_INFORMATION, * PST_OPERATIONLOG_INFORMATION;

	CList<OPERATIONLOG_INFORMATION, OPERATIONLOG_INFORMATION&> m_lstSystemLog;

	CMFCListCtrlEx m_DeviceList;
	CMFCListCtrlEx m_LogList;
	CComboBox* m_pStandType;
	CComboBox* m_pNameType;
	CComboBox* m_pDataType;
	POSITION m_pos;

	CFont m_BigFont;

	afx_msg void OnBnClickedButtonShow();
	virtual BOOL OnInitDialog();
	const BOOL SaveLogList(INT nLevel, CString strTime, CString strDeviceName, CString strDeviceIP, CString strInform, CTime time);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void InitList();
	void InitControl();
	void ShowList();
	void EnableControl(BOOL bEnable);
	afx_msg void OnBnClickedButtonAll();
	CProgressCtrl m_Progress;
	void InitControlTextSize();
};
