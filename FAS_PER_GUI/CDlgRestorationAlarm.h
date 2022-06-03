#pragma once


// CDlgRestorationAlarm 대화 상자

class CDlgRestorationAlarm : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRestorationAlarm)

public:
	CDlgRestorationAlarm(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgRestorationAlarm();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RESTORATION_ALARM };
#endif
public:
	typedef struct _RESOTRE_ALARM
	{
		INT nErrorCount;
		CString strIndexError;
		CString strCountError;
		CString strEndPointError;
		CString strDBError;
		CString strError[4];
	}	RESOTRE_ALARM;

	RESOTRE_ALARM m_alarm;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	void SetRestorationEnableStatus(CString strIndexError, CString strCountError, CString strEndPointError, CString strDbError);
	virtual BOOL OnInitDialog();
};
