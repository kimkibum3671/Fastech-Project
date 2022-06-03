#pragma once


// CMFCRibbonBarEx

class CMFCRibbonBarEx : public CMFCRibbonBar
{
	DECLARE_DYNAMIC(CMFCRibbonBarEx)

public:
	CMFCRibbonBarEx();
	virtual ~CMFCRibbonBarEx();

protected:
	DECLARE_MESSAGE_MAP()

public:
	void DeleteQAT();
	
	const BOOL SetLabelText(const UINT uiCmdID, LPCTSTR lpszText);
	const BOOL SetButtonText(const UINT uiCmdID, LPCTSTR lpszText, const INT nImageIndex = -1);
	const BOOL SetEditText(const UINT uiCmdID, LPCTSTR lpszText);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

	virtual BOOL OnShowRibbonContextMenu(CWnd* pWnd, int x, int y, CMFCRibbonBaseElement* pHit);
};
