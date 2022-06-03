#pragma once

#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif

#include <winsock2.h>

#ifndef _WINSOCK2API_
#define _WINSOCK2API_
#endif

#include <stdlib.h>
#pragma comment(lib, "ws2_32.lib")

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

#include "ETHERNET_DEFINE.h"

#define SOCKET_WAITTIME_NORMAL			500
#define SOCKET_WAITTIME_SAVE_PARAM 		1000
#define SOCKET_WAITTIME_SAVE_PT 		5000


class CFSocket
{
public:
	CFSocket(BOOL bTCP, struct in_addr addr, WORD wPort);
	virtual ~CFSocket();

	// Socket
private:
	SOCKET		m_socket;
	SOCKADDR_IN	m_server;
	const BOOL	m_bTCP;
	const WORD	m_wFirstPort;

	HANDLE		m_hCritical;

	// IP Address
public:
	IN_ADDR		GetIPAddress() const;
	BYTE		GetIPAddress(int index);

	BOOL		IsTCP() const	{return m_bTCP;}
	WORD		GetPort() const	{return ntohs(m_server.sin_port);}

private:
//	WORD		GetOtherPort(WORD wPort);

	// Connection
public:
	BOOL		ConnectSocket(BOOL bOtherPort = FALSE);

	BOOL		Reconnect();

private:
	BOOL		_Reconnect();	// Internal function (w/o critical section)

public:
	void		CloseSocket();

	BOOL		IsConnected()	{return (m_socket != INVALID_SOCKET);}

	// Command
private:
	BYTE		m_BuffSend[MAX_BUFFER_SIZE];
	BYTE		m_BuffRecv[MAX_BUFFER_SIZE];
	DWORD		m_cbBuffRecv;

	BYTE		m_nSyncNo;

protected:
	const INT ClearReceiveTCPBuffer();
	const INT ClearReceiveUDPBuffer();

protected:
	const INT	SendUDPPacket(IN const BYTE iAxisNo, IN const BYTE FrameType, IN LPCBYTE lpData, IN const DWORD dwLen, IN const BOOL bRecordLog = TRUE);
	const INT 	SendTCPPacket(IN const BYTE iAxisNo, IN const BYTE FrameType, IN LPCBYTE lpData, IN const DWORD dwLen, IN const BOOL bRecordLog = TRUE, IN const BOOL bReconnect = FALSE);
	const INT	RecvUDPPacket(IN const BYTE iAxisNo, IN const BYTE FrameType, OUT LPBYTE lpData, IN const DWORD dwLen, IN const DWORD dwWaitTime, BOOL bRecordLog = TRUE);
	const INT	RecvTCPPacket(IN const BYTE iAxisNo, IN const BYTE FrameType, OUT LPBYTE lpData, IN const DWORD dwLen, IN const DWORD dwWaitTime, IN const BOOL bRecordLog = TRUE);

public:
	const INT	DoTrySendCommand(IN const BYTE iAxisNo, IN const BYTE byCmd, IN LPCVOID lpIN, IN const DWORD dwINlen, OUT LPVOID lpOUT, IN const DWORD dwOUTlen, IN const DWORD dwWaitTime, IN const BOOL bReconnect = FALSE);
	const INT	DoSendCommand(IN const BYTE iAxisNo, IN const BYTE byCmd, IN LPCVOID lpIN, IN const DWORD dwINlen, OUT LPVOID lpOUT, IN const DWORD dwOUTlen, IN const DWORD dwWaitTime, IN const BOOL bStopRecordLog = FALSE, IN const BOOL bReconnect = FALSE);
	const INT	DoSendCommandNoResp(IN const BYTE iAxisNo, IN const BYTE byCmd, IN LPCVOID lpIN, IN const DWORD dwINlen, IN const BOOL bReconnect = FALSE);

	// Error
private:
	int			m_nLastErr;

public:
	const INT GetLastSockError();
	const INT GetDefinedLastSockError();

	// Options
public:
	static void SetOptionAutoReconnect(BOOL bSET);

#ifdef _PRINT_LOG_MODE
private:
	HANDLE		m_hLogSync;

	FILE*		m_pfileLogMsg;

	// Log기록을 시작한 Year/Month/Day
	int			m_tmLogYear;
	int			m_tmLogDay;

public:
	static void	LogEnable(BOOL bEnable);
	static BOOL	LogSetPath(LPCTSTR lpPath);

	bool		OpenLogFile(struct tm *today);
	void		CloseLogFile();

	void		Trace0Msg(LPCTSTR pszMessage);
	void		TraceMsg(LPCTSTR pszFormat, ...);

	void		TraceSendBuffer(UINT nLen);
	void		TraceRecvBuffer(UINT nLen);
#else
#define Trace0Msg	TRACE0
#define TraceMsg	TRACE
#endif //_PRINT_LOG_MODE

protected:
	const WORD CalcCRC(IN LPCBYTE lpBuff, IN const DWORD dwLen);
	const DWORD BuildPlusR8FramePacket(OUT LPBYTE pBuffer, IN const INT cbSize, IN const BYTE iSlaveNo, IN const BYTE iSyncNo, IN const BYTE FrameType, IN LPCBYTE lpData, IN const DWORD dwLen);
	const INT InterpretReceivedProtocol(IN const BYTE iAxisNo, IN const BYTE FrameType, OUT LPBYTE lpData, IN const DWORD dwLen, IN LPBYTE lpBuffer, IN const DWORD cbBuffer );
};
