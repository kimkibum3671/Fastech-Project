#pragma once
#include "CDeviceMotionGate.h"
#include "CStaticImageViewer.h"

// CDlgDeviceTotalConfiguration 대화 상자

class CDlgDeviceTotalConfiguration : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDeviceTotalConfiguration)

public:
	CDlgDeviceTotalConfiguration(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgDeviceTotalConfiguration();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONFIG_DEVICE_TOTAL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	CDeviceMotionGate* m_pDevice;
public:
	void SetMotiongateCtrl(CDeviceMotionGate* pDevice);
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void ShowWizardButtonOK();
	
	/*CPageConfigDeviceAddress 부분*/
protected :
	CString m_strSystemName;
	CString m_strDeviceID;
	CString m_strTitle;
	INT		m_nCHeckName;
	BOOL	m_bProgramStart;
	INT		m_nSelectedMethod;
	CList<CString, CString&> m_configList;

	typedef enum _TIMER_ID
	{
		TIMER_SHOW_WIZARD_BUTTON = 0,
	} TIMER_ID;


public:
	BOOL m_bOverlapDeviceID;

	void CreateNamingWizard();
	void OnBtnClickedGenerate();
	afx_msg void OnEnChangeEditSystem();
	afx_msg void OnEnChangeEditDeviceId();
	
	/*CPageConfigDeviceMotor 부분*/
protected:
	INT m_nMotorCount;
public:
	void CreateConboMotorNum();
	afx_msg void OnCbnSelchangeComboMotor();

	/*CPageConfigDeviceOrigin 부분*/
protected:
	INT m_nHomingTimeout;
public:
	afx_msg void OnEnChangeEditTimeout();

	/*CPageConfigDeviceImage 부분*/
protected:
	CStaticImageViewer m_wndViewer;
	CBitmap m_bmpImage;
public:
	HBITMAP GetEditSystemImage();
	void OnBtnClickedDefault();
	void OnBtnClickedLoad();
	void OnBtnClickedSave();
	void ShowNamingWizard(INT nShow, BOOL bProgramStart = FALSE);

public:
	BOOL m_bTotalConfig;
	BOOL m_bNoticeConfig;
	void SetTotalConfig(BOOL bTotal);
	const BOOL IsTotalConfig();
	void EnableGenerateBtn();
	void SetTitleText(CString str);
	CString GetTitleText();
	void SetConfigMethod(INT nState);
	INT GetConfigMethod();
	void SetControlEnableWindow(BOOL bEnable);
	const BOOL IsNoticeConfig();
	void SetNoticeConfig(BOOL bNotice = FALSE);
	const BOOL IsInformChange(CString strEditName);
	const BOOL IsChangeDeviceInformation();
	const BOOL IsDeviceExist(CString strName);
	void SetTotalConfigurationList(CString strName);
	const BOOL IsExistInTotalConfigurationList(CString strName);
	INT GetEditMotorIndex();
	afx_msg void OnBnClickedOk();
	void InitControlTextSize();
	CFont m_BigFont;
};
