#pragma once
#include "CMFCListCtrlEX.h"
#include "CDeviceMotionGate.h"
#include "CStaticImageViewer.h"
// CDlgSystemManage 대화 상자

class CDlgSystemManage : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSystemManage)

public:
	CDlgSystemManage(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgSystemManage();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYSTEM_MAMAGE_BACKUP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	CMFCListCtrlEx m_listCtrl;

public:
	virtual BOOL OnInitDialog();
	
	typedef struct _BACKUP_LIST
	{
		CString strName;
		INT nListCnt;
	}	BACKUP_LIST;

	typedef struct _BACKUP_SUB_LIST
	{
		CString strName;
		CString strNominalName;
		CString strNominalTime;
	}	BACKUP_SUB_LIST;

	CList<BACKUP_LIST, BACKUP_LIST&> m_listMain;
	CList<BACKUP_SUB_LIST, BACKUP_SUB_LIST&> m_listSub;
	CStaticImageViewer m_wndViewer;
	CBitmap m_bmpImage;
	BOOL m_bCompare;
	CFont m_BigFont;

	const BOOL LoadFromDatabase(LPVOID pDatabaseRecordData, CString& strName, INT& nBackupCount);
	const BOOL LoadFromDatabaseDetail(LPVOID pDatabaseRecordData, CString& strProjectName, CString& strNominalName, CString& strNominalTime);
	//CListCtrl m_listCtrl;
	void LoadProjectList();
	void ShowListCtrl();
	CString GetSystemName();
	CString m_strSelectedName;
	afx_msg void OnNMClickListMain(NMHDR* pNMHDR, LRESULT* pResult);
	void SetSelectedName(CString strName);
	void UpdateText(CString strName, CString strCnt);
	void LoadProjectImage();
	const BOOL SetDeviceImage(CDeviceMotionGate* pDevice);
	HBITMAP GetEditSystemImage(CDeviceMotionGate* pDevice);
	CString m_strTitle;
	void SetTitle(CString strTitle);
	afx_msg void OnBnClickedOk();
	
	void SetCompareMode(const BOOL bCompare);
	const BOOL IsCompareMode();
	void RequestFromRibbon(const BOOL bRibbon);
	void InitControlTextSize();
};
