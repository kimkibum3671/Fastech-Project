#pragma once


// CDlgSaveLogData 대화 상자

class CDlgSaveLogData : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSaveLogData)

public:
	CDlgSaveLogData(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgSaveLogData();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SAVE_LOG };
#endif

	typedef struct _OPERATIONLOG_INFORMATION
	{	//	Log DB에 저장할 내용
		INT		nLevel;
		CString strTime;
		CString strDeviceName;
		CString strDeviceIP;
		CString strInform;
	}	OPERATIONLOG_INFORMATION, * PST_OPERATIONLOG_INFORMATION;

	CList<OPERATIONLOG_INFORMATION, OPERATIONLOG_INFORMATION&> m_lstSystemLog;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	 void SetList(LPVOID list);
	 virtual BOOL OnInitDialog();
	 BOOL SaveListData();
	 CProgressCtrl m_Progress;
	 afx_msg void OnTimer(UINT_PTR nIDEvent);
};
