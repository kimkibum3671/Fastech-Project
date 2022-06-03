#pragma once
#include "ReferD.h"
// CMFCButtonEx

#define RGBA(r,g,b, a)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16))|(((DWORD)(BYTE)(a))<<24))
#define GetAValue(rgba)      (LOBYTE((rgba)>>24))

class CMFCButtonEx : public CMFCButton
{
	DECLARE_DYNAMIC(CMFCButtonEx)

public:
	CMFCButtonEx();
	virtual ~CMFCButtonEx();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	BOOL m_bEnableWindowTheme;
	INT m_nBmpResId, m_uiBmpHotResId, m_uiBmpDsblResID;

public:
	BOOL m_bFontChange;
	virtual void OnDrawBorder(CDC* pDC, CRect& rectClient, UINT uiState);
	
	void SetImageEx(const UINT uiBmpResId, const UINT uiBmpHotResId = 0, const UINT uiBmpDsblResID = 0);
	void GetImageEx(UINT* pUiBmpResId, UINT* pUiBmpHotResId = NULL, UINT* pUiBmpDsblResID = NULL);
	virtual void OnDraw(CDC* pDC, const CRect& rect, UINT uiState);
	virtual void OnDrawText(CDC* pDC, const CRect& rect, const CString& strText, UINT uiDTFlags, UINT uiState);
	//afx_msg void OnPaint();
};
