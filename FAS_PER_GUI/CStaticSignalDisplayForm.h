#pragma once

/// <summary>
/// Internal Structure
/// </summary>
typedef struct _STRUCT_FORM_SIGNAL_INFO
{
	CString		strText;
	UINT		m_nTooltipType;
	UINT		m_nTooltipID;
	COLORREF	colorBkOn;
	COLORREF	colorBkOff;
	COLORREF	colorTextOn;
	COLORREF	colorTextOff;
}	STRUCT_FORM_SIGNAL_INFO;

class CStaticSignalDisplayForm
{
public:
	CStaticSignalDisplayForm();
	~CStaticSignalDisplayForm();

public:
	static CStaticSignalDisplayForm* GetInstance();

protected:
	typedef CMap<ULONG, ULONG, STRUCT_FORM_SIGNAL_INFO*, STRUCT_FORM_SIGNAL_INFO*> CMapData;
	CMapData	m_mapData;
	COLORREF	m_colorDefaultBkOn, m_colorDefaultBkOff, m_colorDefaultTextOn, m_colorDefaultTextOff;
	CString		m_strTooltipTitle;

protected:
	STRUCT_FORM_SIGNAL_INFO* GetFormStructure(const ULONG index);

public:
	virtual void	RemoveAll();

public:
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

	void	Draw(const ULONG index, CDC* pDC, CRect rect, const BOOL bOn);
	void	DrawOn(const ULONG index, CDC* pDC, CRect rect);
	void	DrawOff(const ULONG index, CDC* pDC, CRect rect);

	const BOOL		SetItemText(const ULONG iBitNo, LPCTSTR lpszText);
	LPCTSTR			GetItemText(const ULONG iBitNo);

	const BOOL		SetBkColor(const ULONG iBitNo, const COLORREF color);
	const BOOL		SetBkColor(const ULONG iBitNo, const COLORREF colorOn, const COLORREF colorOff);
	
	void			GetBkColor(const ULONG iBitNo, COLORREF& colorOn, COLORREF& colorOff);
	const COLORREF	GetBkColorOn(const ULONG iBitNo);
	const COLORREF	GetBkColorOff(const ULONG iBitNo);

	const BOOL		SetTextColor(const ULONG iBitNo, const COLORREF color);
	const BOOL		SetTextColor(const ULONG iBitNo, const COLORREF colorOn, const COLORREF colorOff);
	void			GetTextColor(const ULONG iBitNo, COLORREF& colorOn, COLORREF& colorOff);
	const COLORREF	GetTextColorOn(const ULONG iBitNo);
	const COLORREF	GetTextColorOff(const ULONG iBitNo);

	void			SetTooltipTitle(LPCTSTR lpszTitle);
	LPCTSTR			GetTooltipTitle();

	void			SetTooltipType(const ULONG iBitNo, const UINT nType);
	const UINT		GetTooltipType(const ULONG iBitNo);

	void			SetTooltipID(const ULONG iBitNo, const UINT nResID);
	const UINT		GetTooltipID(const ULONG iBitNo);
};
