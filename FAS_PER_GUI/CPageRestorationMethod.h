#pragma once

#include "CMFCButtonEx.h"
#include "CMFCListCtrlEX.h"
#include "CMFCPropertyPageEx.h"
#include "ColorStaticST.h"

#include <vector>

using namespace std;

class CPageRestorationMethod : public CMFCPropertyPageEx
{
	DECLARE_DYNAMIC(CPageRestorationMethod)

public:
	CPageRestorationMethod(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPageRestorationMethod();
	virtual BOOL OnInitDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_RESTORATION_METHOD };
#endif

public:
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnBack();
	afx_msg void OnBnClickedBtnNext();
	afx_msg void OnBnClickedSelMethod1();
	afx_msg void OnBnClickedSelMethod2();
	afx_msg void OnBnClickedSelMethod3();
	afx_msg void OnBnClickedBtnMainNominal();
	afx_msg void OnBnClickedBtnRecent();
	afx_msg void OnLvnItemchangedListMain(NMHDR* pNMHDR, LRESULT* pResult);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	CMFCButtonEx m_btnPrev, m_btnNext, m_btnMethod1, m_btnMethod2, m_btnMethod3;
	
	INT m_nSelMethod, m_nRestoredID, m_nNominalID;
	CMFCListCtrlEx m_wndList;

	CFont m_BigFont;

public:
	CColorStaticST m_wndColor[6];
	typedef struct BACKUP_STRUCT
	{
		INT nBackupID;
		CString strTimeStamp;
		CString strComment;
	};

	BACKUP_STRUCT m_NominalData, m_SelectedData;

	CDeviceMotionGate* m_pDevice;

protected:
	void InitializeListCtrl();
	void LoadProjectList();
	void ChangeProjectInfo(const INT nMethod);
	void SetNominalData(const INT nSelectIndex);
public:
	typedef enum _TIMER_ID
	{
		TIMER_NEXT_BUTTON = 0,
	}TIMER_ID;

	typedef enum _SELECTED_METHOD
	{
		sts_selected_files = 1, // 파일끼리 비교
		sts_selected_device // 장치와 파일과의 비교
	}SELECTED_METHOD;

	typedef struct _RESOTRE_ALARM
	{
		INT nErrorCount;
		CString strError[8];
		DWORD dwNominal;
		DWORD dwSelect;
		/*CString strIndexError;
		CString strCountError;
		CString strEndPointError;
		CString strDBError;
		DWORD dwDevice;*/
	}	RESOTRE_ALARM;

	RESOTRE_ALARM m_alarm;
	BOOL m_bEnableCompare;
	BOOL m_bRequestFromRibbon;
	void InitComapreButton();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	const BOOL CheckingComparing();
	void InitializeStaticControl();
	void ChangeTextInfo(INT nMethod);
	const BOOL IsEnableCompareCurrentSystem(CDeviceMotionGate* pDevice, INT nBackupID);
	const BOOL LoadBackupFileMotorIndex(LPVOID pDatabaseRecordData, INT& nIndex);
	void InitAlarmStruct();
	void SetIndexError(CString strError);
	void ShowDisableAlarm();
	const BOOL IsEnableCompareFiles(CString strName, INT nNominalID, INT nSelectedID);
	void SetNominalID(INT nBackupID);
	void SetDevice(CDeviceMotionGate* pDevice);
	const BOOL IsRequestFromRibbon();
	void SetRequestRibbon(BOOL bRequest = FALSE);
	void InitControlTextSize();
};