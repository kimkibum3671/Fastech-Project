#pragma once

#include "CDeviceList.h"

// COutlookDevice

class COutlookDevice : public CWnd
{
	DECLARE_DYNAMIC(COutlookDevice)

public:
	COutlookDevice();
	virtual ~COutlookDevice();

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTvnSelChangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnUpdateChkName(CCmdUI* pCmdUI);
	afx_msg void OnChkName();
	afx_msg void OnUpdateChkAddress(CCmdUI* pCmdUI);
	afx_msg void OnChkAddress();
	afx_msg void OnNMClickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMRClickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCmdSystemRun();
	afx_msg void OnCmdSystemStop();
	afx_msg void OnCmdSystemReboot();
	afx_msg void OnCmdSystemShedule();
	afx_msg void OnCmdSystemAlarmReset();
	afx_msg void OnCmdDeviceConfig();
	afx_msg void OnCmdDeviceStop();
	afx_msg void OnCmdDeviceBackup();
	afx_msg void OnCmdDeviceRestoration();
	afx_msg void OnCmdDeviceAssignment();
	afx_msg void OnCmdDeviceFactoryReset();
	afx_msg void OnCmdDeviceOperation();

protected:
	CMFCToolBar m_toolbar;
	CTreeCtrl m_wndTree;
	HTREEITEM m_hSelItem;

public:
	virtual BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nID = (UINT)-1);	
protected:
	typedef enum _TIMER_ID
	{
		TIMER_POLLING = 0,
		TIMER_POPUP_TREE_MENU,
	}	TIMER_ID;

public:
	typedef enum _TREE_ITEM_TYPE
	{
		typeMotionGate = 0,
		typeConnection,
		typeMotor,
	}	TREE_ITEM_TYPE;

	typedef struct _INFO_MOTIONGATE
	{	//	for Device Count
		DWORD	dwDeviceAddress;
		DWORD	reserved;
	}	INFO_MOTIONGATE;

	typedef struct _INFO_CONNECTON
	{	//	for Device Count
		DWORD	dwDeviceAddress;

		struct
		{
			union
			{
				DWORD orgValue;
				struct
				{
					BYTE	btItemType;
					BYTE	status;
					WORD	wConnection;
				};
			};

		};
	}	INFO_CONNECTON;

	typedef struct _INFO_MOTOR
	{	//	for Motor Status
		DWORD	dwDeviceAddress;

		struct
		{
			union
			{
				DWORD orgValue;
				struct
				{
					BYTE	btItemType;
					BYTE	btDeviceID;
					WORD	wStatus;
				};
			};
		};

	}	INFO_MOTOR;

	typedef union _TREE_ITEM_DATA_FLAG
	{	//	for GetSelectedItemData()
		ULONGLONG orgValue;
		INFO_MOTIONGATE	info_motiongate;
		INFO_CONNECTON info_connection;
		INFO_MOTOR info_motor;
	}	TREE_ITEM_DATA_FLAG;

protected:
	const UINT GetDeviceCount(const BOOL bLock = TRUE);
	void GetDeviceStatus(CDeviceMotionGate* pDevice, CString& strStatus, BOOL bFocusName = FALSE, BOOL bFocusIP = FALSE);
	void DisplayMotiongateStatus();
	void ReloadConnectedMotorStatus(CDeviceMotionGate* pDevice, const HTREEITEM hMotiongate);
	const INT DisplayConnectedMotorInformation(CDeviceMotionGate* pDevice, const HTREEITEM hParent, const HTREEITEM hMotiongate);
	const INT DisplayConnectedMotorStatus(CDeviceMotionGate* pDevice, const HTREEITEM hMotorCount);
	void SendCommandMessageToMainFrame(WPARAM wParam = NULL, LPARAM lParam = NULL);

public:
	void LoadDeviceInformation();
	const ULONGLONG GetSelectedItemData();	
	void PopupDeviceControlMenu(CPoint pt);
	const BOOL GetSystemControlMenu(CMenu* pMenu);
	const BOOL GetDeviceControlMenu(CMenu* pMenu);
//	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	public:
		BOOL m_bFocusName;
		BOOL m_bFocusIP;
		BOOL m_bClicked;
		void SetFocusName(/*BOOL bFocus*/);
		void SetFocusIP(/*BOOL bFocus*/);
		void SetFocusNone();
		virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
		void SetIPSearching();
		afx_msg void OnCmdDeviceReset();
		const BOOL IsTreeClicked();
		void SetTreeClicked(BOOL bClicked);
};
