#pragma once
class CBalloonMsg
{
protected:
	static CEdit m_wndEdit;

public:
	static const BOOL ShowBalloonMsg(HWND hWnd, LPCWSTR lpszTitle, LPCWSTR lpszText, const INT ttiIcon);
};
