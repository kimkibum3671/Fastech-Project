#pragma once

#include "CStaticSignalDisplayForm.h"

// CStaticSignalDisplay
class CStaticSignalDisplay : public CStatic
{
	DECLARE_DYNAMIC(CStaticSignalDisplay)

public:
	CStaticSignalDisplay();
	virtual ~CStaticSignalDisplay();

public:
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

protected:
	DECLARE_MESSAGE_MAP()

protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	CSize m_sizeCell;
	CMap<ULONG, ULONG, BOOL, BOOL> m_mapSignal;
	CStaticSignalDisplayForm* m_pFormCtrl;
	CToolTipCtrl m_tipCtrl;
	INT m_nFormID;
	
	CPoint m_ptMouse;
	INT m_nPointedItem;

protected:
	CStaticSignalDisplayForm* GetFormCtrl();

	typedef enum _TIMER_ID
	{
		TIMER_POLLING = 0,
	}	TIMER_ID;

protected:
	void TrackMousePos();
	const INT CalcSelectedItem(CPoint ptMouse, CRect rtWindow);

public:
	void RemoveAll();

	void			SetFormID( const INT nFormID = -1);
	const INT		GetFormID();

	const BOOL		SetDefaultBkColor(const COLORREF color);
	const BOOL		SetDefaultBkColor(const COLORREF colorOn, const COLORREF colorOff);
	
	void			GetDefaultBkColor(COLORREF& colorOn, COLORREF& colorOff);
	const COLORREF	GetDefaultBkColorOn();
	const COLORREF	GetDefaultBkColorOff();

	const BOOL		SetDefaultTextColor(const COLORREF color);
	const BOOL		SetDefaultTextColor(const COLORREF colorOn, const COLORREF colorOff);
	
	void			GetDefaultTextColor(COLORREF& colorOn, COLORREF& colorOff);
	const COLORREF	GetDefaultTextColorOn();
	const COLORREF	GetDefaultTextColorOff();

	const BOOL		SetSize(const INT cx, const INT cy);
	CSize			GetSize();

	const BOOL		SetItemText(const ULONG index, LPCTSTR lpszText);
	LPCTSTR			GetItemText(const ULONG index);

	const BOOL		SetBkColor(const ULONG index, const COLORREF color);
	const BOOL		SetBkColor(const ULONG index, const COLORREF colorOn, const COLORREF colorOff);
	
	void			GetBkColor(const ULONG index, COLORREF& colorOn, COLORREF& colorOff);
	const COLORREF	GetBkColorOn(const ULONG index);
	const COLORREF	GetBkColorOff(const ULONG index);

	const BOOL		SetTextColor(const ULONG index, const COLORREF color);
	const BOOL		SetTextColor(const ULONG index, const COLORREF colorOn, const COLORREF colorOff);
	
	void			GetTextColor(const ULONG index, COLORREF& colorOn, COLORREF& colorOff);
	const COLORREF	GetTextColorOn(const ULONG index);
	const COLORREF	GetTextColorOff(const ULONG index);

	const BOOL		EnableSignal(const ULONG index, const BOOL bEnable);
	const BOOL		IsSignalEnabled(const ULONG index);
};
