// CMFCListCtrlEx.cpp: 구현 파일
//

#include "pch.h"
#include "CMFCListCtrlEx.h"

#include "resource.h"

// CMFCListCtrlEx

#define RESOURCE_ID_EDIT		IDC_EDIT
#define RESOURCE_ID_COMBO_BOX	IDC_COMBO

IMPLEMENT_DYNAMIC(CMFCListCtrlEx, CMFCListCtrl)

CMFCListCtrlEx::CMFCListCtrlEx()
{
	m_pEdit = NULL;
}

CMFCListCtrlEx::~CMFCListCtrlEx()
{

}


BEGIN_MESSAGE_MAP(CMFCListCtrlEx, CMFCListCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()

	ON_EN_KILLFOCUS(RESOURCE_ID_EDIT, &CMFCListCtrlEx::OnEnKillfocusEdit)
	ON_CBN_KILLFOCUS(RESOURCE_ID_COMBO_BOX, &CMFCListCtrlEx::OnCbnKillfocusCombo)
	ON_CBN_SELCHANGE(RESOURCE_ID_COMBO_BOX, &CMFCListCtrlEx::OnCbnSelchangeCombo)
END_MESSAGE_MAP()

// CMFCListCtrlEx 메시지 처리기
int CMFCListCtrlEx::OnCompareItems(LPARAM lParam1, LPARAM lParam2, int iColumn)
{
	//	오름차순과 내림차순을 알아야 한다.

	return(lParam1 < lParam2 ? -1 : 1);
}

COLORREF CMFCListCtrlEx::OnGetCellTextColor(int nRow, int nColum)
{
	COLORREF colorResult = 0;

	if (FALSE == IsWindowEnabled())
	{
		colorResult = ::GetSysColor(COLOR_GRAYTEXT);
	}
	else
	{
		colorResult = CMFCListCtrl::OnGetCellTextColor(nRow, nColum);
	}

	return colorResult;
}

COLORREF CMFCListCtrlEx::OnGetCellBkColor(int nRow, int nColum)
{
	COLORREF colorResult = 0;

	if (FALSE == IsWindowEnabled())
	{
		colorResult = ::GetSysColor(COLOR_BTNFACE);
	}
	else if (FALSE == GetItemBkColor(nRow, colorResult) &&
		FALSE == GetCellBkColor(nRow, nColum, colorResult))
	{
		colorResult = GetBkColor();
	}

	return colorResult;
}

HFONT CMFCListCtrlEx::OnGetCellFont(int nRow, int nColum, DWORD dwData)
{
	//	사용 안함?
	HFONT hResult = NULL;
	if (NULL == (dwData & 0xff000000))
		return hResult;

	return hResult;
}

void CMFCListCtrlEx::Sort(int iColumn, BOOL bAscending, BOOL bAdd)
{
	return;
}


void CMFCListCtrlEx::GetFilename(CString& strFilename)
{
	const INT nBufferSize = MAX_PATH;
	strFilename.ReleaseBufferSetLength(::GetModuleFileName(NULL, strFilename.GetBufferSetLength(nBufferSize), nBufferSize));

	const INT nIndex = strFilename.ReverseFind(_T('\\')) + 1;
	strFilename.Delete(nIndex, strFilename.GetLength() + nIndex);

	strFilename += _T("ListCtrl.cfg");
}


void CMFCListCtrlEx::SaveHeaderItemInfo()
{
	const INT nCtrlID = this->GetDlgCtrlID();
	const INT nParentCtrlID = GetParent()->GetDlgCtrlID();

	CString strFilename;
	this->GetFilename(strFilename);

	CString strApp;
	strApp.Format(_T("%04X_%04X"), nParentCtrlID, nCtrlID);

	const INT nCount = m_wndHeader.GetItemCount();
	for (INT nIndex = 0; nIndex < nCount; nIndex++)
	{
		CString strKey, strValue;
		strKey.Format(_T("%d"), nIndex);
		strValue.Format(_T("%d"), GetColumnWidth(nIndex));
		::WritePrivateProfileString(strApp, strKey, strValue, strFilename);
	}
}

const BOOL CMFCListCtrlEx::LoadHeaderItemInfo()
{
	const INT nCtrlID = this->GetDlgCtrlID();
	const INT nParentCtrlID = GetParent()->GetDlgCtrlID();

	CString strFilename;
	this->GetFilename(strFilename);

	CString strApp;
	strApp.Format(_T("%04X_%04X"), nParentCtrlID, nCtrlID);

	const INT nCount = m_wndHeader.GetItemCount();
	for (INT nIndex = 0; nIndex < nCount; nIndex++)
	{
		CString strKey;
		strKey.Format(_T("%d"), nIndex);
		const INT nWidth = ::GetPrivateProfileInt(strApp, strKey, -1, strFilename);
		if (-1 != nWidth)
		{
			this->SetColumnWidth(nIndex, nWidth);
		}
	}

	Invalidate(FALSE);

	return TRUE;
}

void CMFCListCtrlEx::SelectSingleItem(const INT nIndex)
{
	const INT nCount = this->GetItemCount();
	for (INT i = 0; i < nCount; i++)
	{
		if (this->GetItemState(i, LVIS_SELECTED))
		{
			if (nIndex != i)
			{
				this->SetItemState(i, 0, LVIS_SELECTED);
			}
		}
		else if (nIndex == i)
		{
			this->SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
		}
	}
}

const BOOL CMFCListCtrlEx::RemoveAll()
{
	m_mapColoring.RemoveAll();
	this->DeleteAllItems();

	if (GetSafeHwnd())
	{
		Invalidate(FALSE);
	}

	return TRUE;
}

const BOOL CMFCListCtrlEx::SetItemBkColor(const INT nIndex, const COLORREF color)
{
	if (0 > nIndex || MAXWORD < nIndex)
	{
		return FALSE;
	}

	m_mapColoring.SetAt(nIndex, color);

	return TRUE;
}

const BOOL CMFCListCtrlEx::GetItemBkColor(const INT nIndex, COLORREF& color)
{
	if (0 > nIndex || MAXWORD < nIndex)
	{
		return FALSE;
	}

	return m_mapColoring.Lookup(nIndex, color);
}

const BOOL CMFCListCtrlEx::RemoveItemBkColor(const INT nIndex)
{
	if (0 > nIndex || MAXWORD < nIndex)
	{
		return FALSE;
	}

	return m_mapColoring.RemoveKey(nIndex);
}


const BOOL CMFCListCtrlEx::SetCellBkColor(const INT nItemIndex, const INT nCellIndex, const COLORREF color)
{
	if ((0 > nItemIndex || MAXWORD < nItemIndex) ||
		(0 > nCellIndex || MAXWORD < nCellIndex))
	{
		return FALSE;
	}

	m_mapColoring.SetAt(MAKELONG(nItemIndex, nCellIndex), color);

	return TRUE;
}

const BOOL CMFCListCtrlEx::GetCellBkColor(const INT nItemIndex, const INT nCellIndex, COLORREF& color)
{
	if ((0 > nItemIndex || MAXWORD < nItemIndex) ||
		(0 > nCellIndex || MAXWORD < nCellIndex))
	{
		return FALSE;
	}

	return m_mapColoring.Lookup(MAKELONG(nItemIndex, nCellIndex), color);
}

const BOOL CMFCListCtrlEx::RemoveCellBkColor(const INT nItemIndex, const INT nCellIndex)
{
	if ((0 > nItemIndex || MAXWORD < nItemIndex) ||
		(0 > nCellIndex || MAXWORD < nCellIndex))
	{
		return FALSE;
	}

	return m_mapColoring.RemoveKey(MAKELONG(nItemIndex, nCellIndex));
}

const BOOL CMFCListCtrlEx::HitTestEx(CPoint pt, INT* pnItemIndex, INT* pnColumn)
{
	if ((GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT)
	{	//	Report 형식이 아니므로
		return FALSE;
	}

	ScreenToClient(&pt);

	const INT nRow = HitTest(pt);
	if (-1 == nRow)
	{
		return FALSE;
	}

	INT nStart = 0;
	INT nCount = GetHeaderCtrl().GetItemCount();
	for (INT nColumn = 0; nColumn < nCount; nColumn++)
	{
		const INT nWidth = GetColumnWidth(nColumn);
		if (nStart< pt.x && (nStart + nWidth) > pt.x)
		{
			if (pnItemIndex)
			{
				*pnItemIndex = nRow;
			}

			if (pnColumn)
			{
				*pnColumn = nColumn;
			}

			return TRUE;
		}

		nStart += nWidth;
	}

	//	Seek Column

	return FALSE;
}
//
//void CMFCListCtrlEx::EditLabelEx(const INT nItem, const INT nColumn, CWnd* pWnd)
//{
//	if (NULL == pWnd || NULL == pWnd->GetSafeHwnd())
//	{
//		return;
//	}
//
//	CRect rect;
//	GetItemRect(nItem, &rect, LVIR_BOUNDS);
//	rect.right = 0;
//	INT nCount = GetHeaderCtrl().GetItemCount();
//	for (INT nIndex = 0; nIndex <= nColumn; nIndex++)
//	{
//		rect.left = rect.right;
//		const INT nWidth = GetColumnWidth(nIndex);
//		rect.right += nWidth;
//	}
//
//	ClientToScreen(rect);
//	GetParent()->ScreenToClient(rect);
//	
//	pWnd->ShowWindow(SW_SHOW);
//	pWnd->SetWindowPos(&wndTop, rect.left, rect.top, rect.Width(), rect.Height(), 0);
//	pWnd->SetFocus();
//}

CRect CMFCListCtrlEx::GetItemRectEx(const INT nItem, const INT nColumn)
{
	CRect rect;
	GetItemRect(nItem, &rect, LVIR_BOUNDS);
	rect.left = rect.right = 0;
	INT nCount = GetHeaderCtrl().GetItemCount();
	for (INT nIndex = 0; nIndex <= nColumn; nIndex++)
	{
		rect.left = rect.right;
		const INT nWidth = GetColumnWidth(nIndex);
		rect.right += nWidth;
	}

	return rect;
}

void CMFCListCtrlEx::EditLableByEdit(const INT nItem, const INT nColumn)
{
	CRect rect(GetItemRectEx(nItem, nColumn));

	if (m_pEdit && m_pEdit->GetSafeHwnd())
	{
		//	문제가 있다.
		return;
	}

	if (rect.Width())
	{
		m_pEdit = new CEdit;
		CEdit* pEdit = static_cast<CEdit*>(m_pEdit);
		const DWORD dwStyle = WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_BORDER;
		if (pEdit->Create(dwStyle, rect, this, RESOURCE_ID_EDIT) && pEdit->ShowWindow(SW_SHOWNORMAL))
		{
			m_ptEdit = CPoint(nItem, nColumn);
			pEdit->SetFont(&afxGlobalData.fontRegular);
			SetWindowTheme(pEdit->m_hWnd, _T("Explorer"), _T(""));
			pEdit->SetWindowText(GetItemText(nItem, nColumn));
			pEdit->SetSel(0, -1);
			pEdit->SetFocus();
		}
	}
}

void CMFCListCtrlEx::EditLableByComboBox(const INT nItem, const INT nColumn, CStringArray& arrString)
{
	CRect rect(GetItemRectEx(nItem, nColumn));

	if (m_pEdit && m_pEdit->GetSafeHwnd())
	{
		//	문제가 있다.
		return;
	}

	if (rect.Width())
	{
		m_pEdit = new CComboBox;
		CComboBox* pCmbCtrl = static_cast<CComboBox*>(m_pEdit);
		const DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER;
		if (pCmbCtrl->Create(dwStyle, rect, this, RESOURCE_ID_COMBO_BOX))
		{
			m_ptEdit = CPoint(nItem, nColumn);
			SetWindowTheme(pCmbCtrl->m_hWnd, _T("Explorer"), _T(""));
			pCmbCtrl->SetFont(&afxGlobalData.fontRegular);

			const INT nCount = arrString.GetCount();
			CString strText(GetItemText(nItem, nColumn));

			INT nSelect = -1;
			for (INT nIndex = 0; nCount > nIndex; nIndex++)
			{
				CString strItem(arrString.GetAt(nIndex));
				const INT nCurIndex = pCmbCtrl->AddString(strItem);

				if (strText == strItem)
				{
					nSelect = nCurIndex;
				}
			}

			if (nCount)
			{
				pCmbCtrl->SetCurSel(nCount > nSelect ? nSelect : 0);

				pCmbCtrl->ShowDropDown();
			}

			if (pCmbCtrl->ShowWindow(SW_SHOWNORMAL))
			{
				pCmbCtrl->SetFocus();
			}
		}
	}
}


int CMFCListCtrlEx::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMFCListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pEdit = NULL;
	m_ptEdit = CPoint(-1, -1);
	m_nMsgID = -1;

	return 0;
}


void CMFCListCtrlEx::OnDestroy()
{
	if (m_pEdit && m_pEdit->GetSafeHwnd())
	{
		m_pEdit->DestroyWindow();
	}

	CMFCListCtrl::OnDestroy();
}


void CMFCListCtrlEx::OnEnKillfocusEdit()
{
	CString strText;
	m_pEdit->GetWindowText(strText);
	SetItemText(m_ptEdit.x, m_ptEdit.y, strText);

	m_pEdit->DestroyWindow();
	m_pEdit = NULL;

	if (static_cast<UINT>(-1) != m_nMsgID)
	{
		GetParent()->PostMessage(m_nMsgID, NULL, MAKELPARAM(m_ptEdit.x, m_ptEdit.y));
	}
}


void CMFCListCtrlEx::OnCbnKillfocusCombo()
{
	if (m_pEdit->GetSafeHwnd())
	{
		m_pEdit->DestroyWindow();
		m_pEdit = NULL;
	}
}


void CMFCListCtrlEx::OnCbnSelchangeCombo()
{
	CComboBox* pCmbCtrl = static_cast<CComboBox*>(m_pEdit);
	CString strText;
	pCmbCtrl->GetLBText(pCmbCtrl->GetCurSel(), strText);
	SetItemText(m_ptEdit.x, m_ptEdit.y, strText);

	m_pEdit->DestroyWindow();

	if (static_cast<UINT>(-1) != m_nMsgID)
	{
		GetParent()->PostMessage(m_nMsgID, NULL, MAKELPARAM(m_ptEdit.y, m_ptEdit.x));
	}
}

void CMFCListCtrlEx::SetEditLableMessage(const UINT nMsgID)
{
	m_nMsgID = nMsgID;
}
