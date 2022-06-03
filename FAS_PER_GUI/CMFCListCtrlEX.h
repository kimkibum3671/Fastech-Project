#pragma once


// CMFCListCtrlEx

class CMFCListCtrlEx : public CMFCListCtrl
{
	DECLARE_DYNAMIC(CMFCListCtrlEx)

public:
	CMFCListCtrlEx();
	virtual ~CMFCListCtrlEx();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnEnKillfocusEdit();
	afx_msg void OnCbnKillfocusCombo();
	afx_msg void OnCbnSelchangeCombo();

public:
	virtual void Sort(int iColumn, BOOL bAscending = TRUE, BOOL bAdd = FALSE);
	virtual int OnCompareItems(LPARAM lParam1, LPARAM lParam2, int iColumn);
	virtual COLORREF OnGetCellTextColor(int /*nRow*/, int /*nColum*/);
	virtual COLORREF OnGetCellBkColor(int /*nRow*/, int /*nColum*/);
	virtual HFONT OnGetCellFont(int /*nRow*/, int /*nColum*/, DWORD /*dwData*/ = 0);

protected:
	CMap<DWORD, DWORD, COLORREF, COLORREF> m_mapColoring;
	CWnd*	m_pEdit;
	CPoint	m_ptEdit;
	UINT	m_nMsgID;

protected:
	void	GetFilename(CString& strFilename);
	CRect	GetItemRectEx(const INT nItem, const INT nColumn);

public:
	void			SaveHeaderItemInfo();
	const BOOL		LoadHeaderItemInfo();

	void			SelectSingleItem(const INT nIndex);

	const BOOL		RemoveAll();

	const BOOL		SetItemBkColor(const INT nIndex, const COLORREF color);
	const BOOL		GetItemBkColor(const INT nIndex, COLORREF& color);
	const BOOL		RemoveItemBkColor(const INT nIndex);

	const BOOL		SetCellBkColor(const INT nItemIndex, const INT nCellIndex, const COLORREF color);
	const BOOL		GetCellBkColor(const INT nItemIndex, const INT nCellIndex, COLORREF& color);
	const BOOL		RemoveCellBkColor(const INT nItemIndex, const INT nCellIndex);

	const BOOL		HitTestEx(CPoint pt, INT* pnItemIndex = NULL, INT* pnColumn = NULL);

	void			EditLableByEdit(const INT nItem, const INT nColumn);
	void			EditLableByComboBox(const INT nItem, const INT nColumn, CStringArray& arrString);

	void			SetEditLableMessage(const UINT nMsgID);
};
