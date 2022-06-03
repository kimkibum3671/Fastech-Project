#pragma once


// CSheetConfigPLC 대화 상자

class CSheetConfigPLC : public CDialogEx
{
	DECLARE_DYNAMIC(CSheetConfigPLC)

public:
	CSheetConfigPLC(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CSheetConfigPLC();



protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CRichEditCtrl m_edit;
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
