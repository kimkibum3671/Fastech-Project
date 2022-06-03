#include "pch.h"
#include "CModbusServer.h"

#include <WinSock2.h>

CModbusServer::CModbusServer()
{
	WSADATA wsa = { 0 , };
	WSAStartup(MAKEWORD(2, 2), &wsa);
}

CModbusServer::~CModbusServer()
{
	WSACleanup();

	POSITION pos = m_mapListen.GetStartPosition();
	while (pos)
	{
		INT nPortNo = 0;
		PLISTEN_INFO pListenInfo = NULL;

		m_mapListen.GetNextAssoc(pos, nPortNo, pListenInfo);

		CloseServer(nPortNo);
		
		m_mapListen.RemoveKey(pListenInfo->nPortNo);

		while (m_mapListen.Lookup(nPortNo, pListenInfo))
		{
			Sleep(1);
		}
	}

	m_mapListen.RemoveAll();
}

CModbusServer* CModbusServer::GetInstance()
{
	static CModbusServer _instance;

	return &_instance;
}

const BOOL CModbusServer::RemoveServer(PLISTEN_INFO pListenInfo)
{
	POSITION pos = pListenInfo->lstAcceptedClient.GetHeadPosition();
	while (pos)
	{
		//	client의 연결을 해제한다.
		CModbusAcceptedClient* pClist = pListenInfo->lstAcceptedClient.GetNext(pos);
		pClist->Close();
	}

	while (pListenInfo->lstAcceptedClient.GetCount())
	{
		//	모든 client의 연결이 해제된 것을 확인한다.
		Sleep(1);
	}

	m_mapListen.RemoveKey(pListenInfo->nPortNo);

	delete pListenInfo;

	return TRUE;
}

UINT CModbusServer::ThreadListen(LPVOID pParam)
{
	UINT uResult = 0;

	PLISTEN_INFO pListenInfo = reinterpret_cast<PLISTEN_INFO>(pParam);

	SOCKADDR_IN client_addr = { 0, };
	INT nAddrLen = sizeof(client_addr);

	while (pListenInfo->bRun)
	{
		SOCKET client_sock = accept(pListenInfo->sockListen, reinterpret_cast<SOCKADDR*>(&client_addr), &nAddrLen);
		if (INVALID_SOCKET == client_sock)
		{
			uResult = 1;
			break;
		}
		
		pListenInfo->pParent->OnAccept(pListenInfo, client_sock, client_addr);
	}

	closesocket(pListenInfo->sockListen);

	pListenInfo->pParent->RemoveServer(pListenInfo);

	return uResult;
}

const BOOL CModbusServer::Listen(const INT nPortNo)
{
	PLISTEN_INFO pListen = NULL;

	if (m_mapListen.Lookup(nPortNo, pListen))
	{
		return TRUE;
	}

	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == listen_sock)
	{
		return FALSE;
	}

	SOCKADDR_IN server_addr = { 0, };
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(nPortNo);
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (SOCKET_ERROR == bind(listen_sock, reinterpret_cast<SOCKADDR*>(&server_addr), sizeof(server_addr)))
	{
		return FALSE;
	}

	if (SOCKET_ERROR == listen(listen_sock, SOMAXCONN))
	{
		return FALSE;
	}

	pListen = new LISTEN_INFO;
	pListen->pParent = this;
	pListen->bRun = TRUE;
	pListen->nPortNo = nPortNo;
	pListen->sockListen = listen_sock;
	
	m_mapListen.SetAt(nPortNo, pListen);

	pListen->pListenThread = AfxBeginThread(&CModbusServer::ThreadListen, pListen);

	return TRUE;
}

const BOOL CModbusServer::CloseServer(const INT nPortNo)
{
	PLISTEN_INFO pListen = NULL;
	if (FALSE == m_mapListen.Lookup(nPortNo, pListen))
	{
		return FALSE;
	}

	pListen->bRun = FALSE;
	shutdown(pListen->sockListen, SD_BOTH);

	return TRUE;
}

void CModbusServer::OnAccept(PLISTEN_INFO pListenInfo, SOCKET sockClient, SOCKADDR_IN& client_address)
{
	pListenInfo->m_mutex.Lock();
	
	CModbusAcceptedClient* pClient = new CModbusAcceptedClient(pListenInfo->nPortNo, sockClient, client_address);
	pListenInfo->lstAcceptedClient.AddTail(pClient);

	pListenInfo->m_mutex.Unlock();
}

void CModbusServer::OnClientClose(CModbusAcceptedClient* pClient)
{
	PLISTEN_INFO pListenInfo = NULL;
	if (m_mapListen.Lookup(pClient->GetPortNo(), pListenInfo) )
	{
		pListenInfo->m_mutex.Lock();

		POSITION pos = pListenInfo->lstAcceptedClient.GetHeadPosition();
		while( pos )
		{
			if (pClient == pListenInfo->lstAcceptedClient.GetAt(pos))
			{
				//	연결해제 된 client를 제거하고 메모리를 반환한다.
				pListenInfo->lstAcceptedClient.RemoveAt(pos);
				delete pClient;
				break;
			}
		}

		pListenInfo->m_mutex.Unlock();
	}
}
