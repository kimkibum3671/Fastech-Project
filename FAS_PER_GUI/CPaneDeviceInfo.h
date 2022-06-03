#pragma once

#include "CDeviceList.h"

// CPaneDeviceInfo

class CPaneDeviceInfo : public CDockablePane
{
	DECLARE_DYNAMIC(CPaneDeviceInfo)

public:
	CPaneDeviceInfo();
	virtual ~CPaneDeviceInfo();

public:
	DECLARE_MESSAGE_MAP()

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

protected:
	CMFCToolBar m_toolbar;
	CTreeCtrl m_wndTree;
	UINT m_nScanIndex;
	HTREEITEM m_hSelItem;

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
	const UINT GetDeviceCount( const BOOL bLock = TRUE);
	void GetDeviceStatus(CDeviceMotionGate* pDevice, CString& strStatus);
	void DisplayMotiongateStatus();
	void ReloadConnectedMotorStatus(CDeviceMotionGate* pDevice, const HTREEITEM hMotiongate);
	const INT DisplayConnectedMotorInformation(CDeviceMotionGate* pDevice, const HTREEITEM hParent, const HTREEITEM hMotiongate);
	const INT DisplayConnectedMotorStatus(CDeviceMotionGate* pDevice, const HTREEITEM hMotorCount);

public:
	void LoadDeviceInformation();
	const ULONGLONG GetSelectedItemData();
	
	afx_msg void OnUpdateOperationRun(CCmdUI* pCmdUI);
	
	void PopupDeviceControlMenu(CPoint pt);
	
	const BOOL GetSystemControlMenu(CMenu* pMenu);
	const BOOL GetDeviceControlMenu(CMenu* pMenu);
};
