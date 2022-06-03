#pragma once

#include "CMFCPropertyPageEx.h"

#include "CMFCButtonEx.h"
#include "CMFCListCtrlEX.h"
#include "CMFCPropertyPageEx.h"
#include "CDlgRestorationAlarm.h"
#include "ColorStaticST.h"
// CPageRestorationSelectedMethod

class CPageRestorationSelectedMethod : public CMFCPropertyPageEx
{
	DECLARE_DYNAMIC(CPageRestorationSelectedMethod)

public:
	CPageRestorationSelectedMethod();
	virtual ~CPageRestorationSelectedMethod();
	BOOL OnInitDialog();
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_RESTORATION_SELECT_METHOD};
#endif

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnBack();
	afx_msg void OnBnClickedBtnNext();
	afx_msg void OnBnClickedSelMethod1();
	afx_msg void OnBnClickedBtnMainNominal();
	afx_msg void OnBnClickedBtnRecent();
	afx_msg void OnLvnItemchangedListMain(NMHDR* pNMHDR, LRESULT* pResult);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

private:
	CDlgRestorationAlarm m_RestorationAlarm;
protected:
	CMFCButtonEx m_btnPrev, m_btnNext, m_btnMethod1, m_btnMethod2, m_btnMethod3;
	INT m_nSelMethod, m_nRestoredID, m_nNominalID;
	CMFCListCtrlEx m_wndList;

	bool m_bToggle;
	bool m_bRestoreEnable;

protected:
	void InitializeListCtrl();
	void LoadProjectList();
	void ChangeProjectInfo(const INT nMethod);
	void SetNominalData(const INT nSelectIndex);

public:
	typedef struct _RESOTRE_ALARM
	{
		INT nErrorCount;
		CString strIndexError;
		CString strCountError;
		CString strEndPointError;
		CString strDBError;
		CString strError[100];
	}	RESOTRE_ALARM;

	RESOTRE_ALARM m_alarm;

	CFont m_BigFont;

public:
	bool GetCheckState(int nIcon);
	const BOOL CheckEnableRestoration(CDeviceMotionGate* pDevice, INT nBackupID);
	const BOOL LoadBackupFileMotorIndex(LPVOID pDatabaseRecordData, INT& nIndex);
	void SetRestorationEnableStatus(CString strIndexError, CString strCountError, CString strEndPointError, CString strDbError);
	void ShowDisableAlarm();
	void SetRestorationAlarmStatus(CString strIndexError, CString strCountError, CString strEndPointError, CString strDbError);


	void SetIndexError(CString strError);
	void InitAlarmStruct();
	void InitControlTextSize();
};


