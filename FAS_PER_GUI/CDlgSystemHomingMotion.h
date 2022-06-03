#pragma once

#include "CStaticImageViewer.h"

// CDlgSystemHomingMotion 대화 상자

class CDlgSystemHomingMotion : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSystemHomingMotion)

public:
	CDlgSystemHomingMotion(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgSystemHomingMotion();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYTEM_HOMING_MOTION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedMfcbutton1();
};
