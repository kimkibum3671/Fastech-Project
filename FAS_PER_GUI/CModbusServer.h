#pragma once

#include "CModbusAcceptedClient.h"

class CModbusServer
{
protected:
	CModbusServer();

public:
	~CModbusServer();

public:
	static CModbusServer* GetInstance();

protected:
	typedef struct _LISTEN_INFO
	{
		CModbusServer*	pParent;
		CMutex			m_mutex;
		BOOL			bRun;
		CWinThread*		pListenThread;
		INT				nPortNo;
		SOCKET			sockListen;
		CList<CModbusAcceptedClient*, CModbusAcceptedClient*>	lstAcceptedClient;
	}	LISTEN_INFO,	*PLISTEN_INFO;

protected:
	CMap<INT, INT, PLISTEN_INFO, PLISTEN_INFO >			m_mapListen;

protected:
	static UINT ThreadListen(LPVOID pParam);

protected:
	const BOOL RemoveServer(PLISTEN_INFO pListenInfo);

public:
	const BOOL Listen(const INT nPortNo);
	const BOOL CloseServer(const INT nPortNo);
	
public:
	virtual void OnAccept(PLISTEN_INFO pListenInfo, SOCKET sockClient, SOCKADDR_IN& client_address);
	virtual void OnClientClose(CModbusAcceptedClient* pClient);
};
