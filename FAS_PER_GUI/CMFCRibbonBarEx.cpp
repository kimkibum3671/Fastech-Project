// CMFCRibbonBarEx.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CMFCRibbonBarEx.h"

// CMFCRibbonBarEx

IMPLEMENT_DYNAMIC(CMFCRibbonBarEx, CMFCRibbonBar)

CMFCRibbonBarEx::CMFCRibbonBarEx()
{

}

CMFCRibbonBarEx::~CMFCRibbonBarEx()
{

}


BEGIN_MESSAGE_MAP(CMFCRibbonBarEx, CMFCRibbonBar)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()


// CMFCRibbonBarEx message handlers
void CMFCRibbonBarEx::DeleteQAT()
{
	m_QAToolbar.RemoveAll();
}

const BOOL CMFCRibbonBarEx::SetLabelText(const UINT uiCmdID, LPCTSTR lpszText)
{
	CMFCRibbonLabel* pLabel = static_cast<CMFCRibbonLabel*>(this->FindByID(uiCmdID));
	if (NULL == pLabel)
	{
		return FALSE;
	}

	if (_tcscmp(lpszText, pLabel->GetText() ) )
	{
		pLabel->SetText(lpszText);
		CRect rect(pLabel->GetRect());
		rect.right = rect.left + 400;
		pLabel->SetRect(rect);
		pLabel->Redraw();
	}

	return TRUE;
}

const BOOL CMFCRibbonBarEx::SetButtonText(const UINT uiCmdID, LPCTSTR lpszText, const INT nImageIndex)
{
	CMFCRibbonButton* pButton = static_cast<CMFCRibbonButton*>(this->FindByID(uiCmdID));
	if (NULL == pButton)
	{
		return FALSE;
	}

	BOOL bRedraw = FALSE;
	if (_tcscmp(lpszText, pButton->GetText()))
	{
		pButton->SetText(lpszText);
		bRedraw = TRUE;
	}

	if (-1 != nImageIndex)
	{
		const BOOL bLargeImage = pButton->IsLargeImage();
		if (pButton->GetImageIndex(bLargeImage) != nImageIndex)
		{
			pButton->SetImageIndex(nImageIndex, bLargeImage);
			bRedraw = TRUE;
		}
	}

	if (bRedraw)
	{
		pButton->Redraw();
	}

	return TRUE;
}

const BOOL CMFCRibbonBarEx::SetEditText(const UINT uiCmdID, LPCTSTR lpszText )
{
	CMFCRibbonEdit* pEdit = static_cast<CMFCRibbonEdit*>(this->FindByID(uiCmdID));

	if (NULL == pEdit)
	{
		return FALSE;
	}

	if (_tcscmp(lpszText, pEdit->GetEditText()))
	{
		pEdit->SetEditText(lpszText);
	}	

	return TRUE;
}


void CMFCRibbonBarEx::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CMFCRibbonBaseElement* pHit = HitTest(point);
	if (pHit && pHit->IsKindOf(RUNTIME_CLASS(CMFCRibbonApplicationButton)))
	{
		return;
	}

	CMFCRibbonBar::OnLButtonDblClk(nFlags, point);
}


void CMFCRibbonBarEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	CMFCRibbonBaseElement* pHit = HitTest(point);
	if (pHit && pHit->IsKindOf(RUNTIME_CLASS(CMFCRibbonApplicationButton)))
	{
		return;
	}

	CMFCRibbonBar::OnLButtonDown(nFlags, point);
}


void CMFCRibbonBarEx::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CMFCRibbonBar::OnRButtonDown(nFlags, point);
}

BOOL CMFCRibbonBarEx::OnShowRibbonContextMenu(CWnd* pWnd, int x, int y, CMFCRibbonBaseElement* pHit)
{
	// 이벤트 발생시 상속된 클래스내의 OnShowRibbonContextMenu 함수를  실행시키지않고 여기서 함수로 실행시켜 아무것도 현시하지 않는다.
	return TRUE;
}