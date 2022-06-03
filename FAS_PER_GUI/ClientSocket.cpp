// ClientSocket.cpp: 구현 파일
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "ClientSocket.h"


// ClientSocket

ClientSocket::ClientSocket()
{
	CAsyncSocket::CAsyncSocket();
	m_hWnd = NULL;
	m_nID = -1;
}

ClientSocket::~ClientSocket()
{
}


// ClientSocket 멤버 함수


void ClientSocket::OnAccept(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CAsyncSocket::OnAccept(nErrorCode);
}


void ClientSocket::OnConnect(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
		//TRACE(L"Cpanel Connect : %d \r\n",m_nID);
	if (nErrorCode != 0) TRACE(L"OnConnect Error Code = %d at %d \r\n", nErrorCode, m_nID);
	if (m_hWnd != NULL)	SendMessage(m_hWnd, WM_SCONNECT, (WPARAM)m_nID, (LPARAM)nErrorCode);	//this);
	CAsyncSocket::OnConnect(nErrorCode);
}


void ClientSocket::OnReceive(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (nErrorCode != 0)	TRACE(L"OnConnect error code = %d at %d.\r\n", nErrorCode, m_nID);
	if (m_hWnd != NULL)	SendMessage(m_hWnd, WM_SRECEIVE, (WPARAM)m_nID, (LPARAM)nErrorCode);	//this);
	CAsyncSocket::OnReceive(nErrorCode);
}


void ClientSocket::OnClose(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (nErrorCode != 0) TRACE(L"ClientSocket Close Error Code = %d at %d \r\n", nErrorCode, m_nID);
	if (m_hWnd != NULL) SendMessage(m_hWnd, WM_SCLOSE, (WPARAM)m_nID, (LPARAM)nErrorCode);
	CAsyncSocket::OnClose(nErrorCode);
}



// ClientSocket 멤버 함수
