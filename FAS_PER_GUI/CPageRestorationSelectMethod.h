#pragma once


// CPageRestorationSelectMethod 대화 상자

class CPageRestorationSelectMethod : public CDialogEx
{
	DECLARE_DYNAMIC(CPageRestorationSelectMethod)

public:
	CPageRestorationSelectMethod(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CPageRestorationSelectMethod();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDP_RESTORATION_SELECT_METHOD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
