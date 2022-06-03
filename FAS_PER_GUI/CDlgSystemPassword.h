#pragma once
#include "CStaticImageViewer.h"
#include "MainFrm.h"
// CDlgSystemPassword 대화 상자

class CDlgSystemPassword : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSystemPassword)

public:
	CDlgSystemPassword(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgSystemPassword();

	CWnd* m_pParent;
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_CONFIG_DEVICE_PASSWORD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	CString m_strName;
	CString m_strChangedPassword;
	CStaticImageViewer m_wndImage;
	CBitmap m_Image;
	BOOL GetLoadImage(CString strName);
	virtual BOOL OnInitDialog();
	void SetController(CString strName);
	CString GetController();
	const BOOL GetLoadPassword(CString strName);
	void GetChangePassword();
	virtual void OnOK();
	void SetChangedPassword(CString strPassword);
	CString GetChangedPassword();
	void GetLoadChangedPassword(CString strPassword);
	void InitControlTextSize();
	CFont m_BigFont;
};
