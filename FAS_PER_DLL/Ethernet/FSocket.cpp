#include "FSocket.h"

#include <stdio.h>
#include <tchar.h>
#include <mmsystem.h>
#include <time.h>
#include <share.h>
#include <shlwapi.h>

#pragma comment(lib, "winmm.lib")

//	for Debuging
#include <cassert>
#ifdef _DEBUG
#define ASSERT(x)	assert(x)
#else
#define ASSERT(x)
#endif

#include "../../Common_Define/Define_ReturnCode.h"

//------------------------------------------------------------------
//                 Frame Protocol Defines.
//------------------------------------------------------------------
static const BYTE	ASCII_NODE = (0xAA);
static const BYTE	ASCII_NODESTART = (0xCC);
static const BYTE	ASCII_NODEEND = (0xEE);

// static function
SOCKADDR_IN MakeIPaddr(struct in_addr addr, WORD wPort)
{
	SOCKADDR_IN server;

	server.sin_family = AF_INET;
	server.sin_addr = addr;
	server.sin_port = htons(wPort);

	return server;
}

// Log variables
volatile BOOL	local_bWriteLogs = TRUE;	// Connect �Ǿ� Log�� ����� ��.
TCHAR			local_chLogFilePath[_MAX_PATH * 2] = _T("");
BOOL			local_bAutoReconnect = TRUE;

CFSocket::CFSocket(BOOL bTCP, struct in_addr addr, WORD wPort)
	: m_bTCP(bTCP), m_server(MakeIPaddr(addr, wPort)), m_wFirstPort(wPort)
{
	m_socket = INVALID_SOCKET;

	m_nLastErr = 0;

	if (bTCP)
	{
		//	�� TCP�� �̸��� �������� �ʾ���?
		// TCP
		m_hCritical = CreateMutex(NULL, FALSE, NULL);
	}
	else
	{
		// UDP
		TCHAR chMutexName[24];
		_stprintf_s(chMutexName, 24, _T("FASTECH_PE_0x%08X"), GetIPAddress().s_addr);
		m_hCritical = CreateMutex(NULL, FALSE, chMutexName);
	}

	m_nSyncNo = 0xD0;

#ifdef _PRINT_LOG_MODE
	m_hLogSync = CreateMutex(NULL, FALSE, NULL);
	m_pfileLogMsg = NULL;
#endif //_PRINT_LOG_MODE
}

CFSocket::~CFSocket()
{
	if (m_socket != INVALID_SOCKET)
		CloseSocket();

	if (m_hCritical != NULL)
	{
		CloseHandle(m_hCritical);
		m_hCritical = NULL;
	}

#ifdef _PRINT_LOG_MODE
	WaitForSingleObject(m_hLogSync, INFINITE);
	CloseLogFile();
	ReleaseMutex(m_hLogSync);

	CloseHandle(m_hLogSync);
#endif //_PRINT_LOG_MODE
}

IN_ADDR CFSocket::GetIPAddress() const
{
	return m_server.sin_addr;
}

BYTE CFSocket::GetIPAddress(int index)
{
	BYTE chVal = 0;

	switch (index)
	{
	case 0:	chVal = GetIPAddress().s_net;	break;
	case 1:	chVal = GetIPAddress().s_host;	break;
	case 2:	chVal = GetIPAddress().s_lh;	break;
	case 3:	chVal = GetIPAddress().s_impno;	break;
	}

	return chVal;
}

//WORD CFSocket::GetOtherPort(WORD wPort)
//{
//	if (IsTCP())
//	{
//		switch (wPort)
//		{
//		case TCP_PORT_NO1:	return TCP_PORT_NO2;	break;
//		default:			return TCP_PORT_NO1;	break;
//		}
//	}
//	else
//	{
//		switch (wPort)
//		{
//		case UDP_PORT_NO1:	return UDP_PORT_NO2;	break;
//		default:			return UDP_PORT_NO1;	break;
//		}
//	}
//}

BOOL CFSocket::ConnectSocket(BOOL bOtherPort)
{
	m_cbBuffRecv = 0;

	//	���⼭�� bOtherPort�� ������� �ʴ´�.
	
	SOCKET _socket;

	if (m_socket != INVALID_SOCKET)
		CloseSocket();

	if (IsTCP())
	{
		SOCKADDR_IN server;
		BOOL bConnected = FALSE;

		server.sin_family = m_server.sin_family;
		server.sin_addr = m_server.sin_addr;
		server.sin_port = m_server.sin_port;

		_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (_socket != INVALID_SOCKET)
		{
			//	Non-Block�������� �����.
			ULONG iMode = 1;
			if (SOCKET_ERROR == ioctlsocket(_socket, FIONBIO, &iMode))
			{
				m_nLastErr = WSAGetLastError();

				shutdown(_socket, SD_BOTH);
				closesocket(_socket);

				return FMC_SOCKET_ERROR;
			}

			connect(_socket, (struct sockaddr*)&server, sizeof(server));

			fd_set Write;
			FD_ZERO(&Write);
			FD_SET(_socket, &Write);

			TIMEVAL Timeout;
			Timeout.tv_sec = 1;
			Timeout.tv_usec = 0;

			const int ntemp = select(0, NULL, &Write, NULL, &Timeout);
			if (0 < ntemp)
			{
				m_server.sin_family = server.sin_family;
				m_server.sin_addr = server.sin_addr;
				m_server.sin_port = server.sin_port;
			}
			else
			{				
				// WSAGetLastError()�δ� ������ �ľ��� �� ����.
				int nlen = sizeof(m_nLastErr);
				getsockopt(_socket, SOL_SOCKET, SO_ERROR, (char*)&m_nLastErr, &nlen);

				shutdown(_socket, SD_BOTH);
				closesocket(_socket);

				return (WSAECONNREFUSED == m_nLastErr) ? FMC_REFUSED : FMC_SOCKET_ERROR;
			}
		}
	}
	else
	{
		_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (_socket == INVALID_SOCKET)
		{
			m_nLastErr = WSAGetLastError();
			WSASetLastError(0);

			return FMC_SOCKET_ERROR;
		}
	}

	// Connected
	m_socket = _socket;

	return FMM_OK;
}

BOOL CFSocket::Reconnect()
{
	BOOL bRtn = FALSE;

	WaitForSingleObject(m_hCritical, INFINITE);

	bRtn = _Reconnect();

	ReleaseMutex(m_hCritical);

	return bRtn;
}

BOOL CFSocket::_Reconnect()
{
	// Close
	if (m_socket != INVALID_SOCKET)
	{
		if (m_bTCP)
			shutdown(m_socket, SD_BOTH);

		closesocket(m_socket);
	}

	m_socket = INVALID_SOCKET;

	// Reconnect
	return ConnectSocket(TRUE);
}

void CFSocket::CloseSocket()
{
	WaitForSingleObject(m_hCritical, INFINITE);

	if (m_socket != INVALID_SOCKET)
	{
		if (m_bTCP)
			shutdown(m_socket, SD_BOTH);

		closesocket(m_socket);

		m_socket = INVALID_SOCKET;
	}

	ReleaseMutex(m_hCritical);
}

/// <summary>
///	Clear received data of TCP socket
/// </summary>
/// <returns></returns>
const INT CFSocket::ClearReceiveTCPBuffer()
{
	memset(m_BuffRecv, NULL, sizeof(m_BuffRecv));
	m_cbBuffRecv = 0;

	ULONG dbSize = -1;
	if (SOCKET_ERROR == ioctlsocket(m_socket, FIONREAD, &dbSize))
	{
		return FMC_SOCKET_ERROR;
	}

	if (0 < dbSize)
	{
		PCHAR pBuffer = new CHAR[dbSize];
		const int nRead = recv(m_socket, pBuffer, dbSize, 0);
		delete pBuffer;
		if (SOCKET_ERROR == nRead )
		{
			return FMC_SOCKET_ERROR;
		}
	}

	return FMM_OK;
}

const INT CFSocket::ClearReceiveUDPBuffer()
{
	memset(m_BuffRecv, NULL, sizeof(m_BuffRecv));
	m_cbBuffRecv = 0;

	ULONG dbSize = -1;
	if (SOCKET_ERROR == ioctlsocket(m_socket, FIONREAD, &dbSize))
	{
		return FMC_SOCKET_ERROR;
	}

	if (0 < dbSize)
	{
		SOCKADDR_IN sender = { 0 };
		int SenderAddrSize = sizeof(sender);

		PCHAR pBuffer = new CHAR[dbSize];
//		const int nRead = recvfrom(m_socket, pBuffer, dbSize, 0, (sockaddr*)&sender, &SenderAddrSize);
		const int nRead = recvfrom(m_socket, pBuffer, dbSize, 0, NULL, NULL);
		delete pBuffer;
		if (SOCKET_ERROR == nRead)
		{
			return FMC_SOCKET_ERROR;
		}
	}

	return FMM_OK;
}

const INT CFSocket::SendUDPPacket(IN const BYTE iAxisNo, IN const BYTE FrameType, IN LPCBYTE lpData, IN const DWORD dwLen, IN const BOOL bRecordLog)
{
	int byLen = (dwLen + SENDOFFSET_DATA);	// (Header, Length, Sync, CMD) + dwLen

	// Make Packet -----------------------------------------------------
	m_BuffSend[SENDOFFSET_HEADER] = PACKET_HEADER;
	m_BuffSend[SENDOFFSET_LENGTH] = (unsigned char)(dwLen + (SENDOFFSET_CMD - SENDOFFSET_LENGTH));	// Sync, CMD
	m_BuffSend[SENDOFFSET_SYNC] = m_nSyncNo;
#ifdef INSERT_AXIS_IN_PACKET
	m_BuffSend[SENDOFFSET_AXIS] = iAxisNo;
#endif
	m_BuffSend[SENDOFFSET_CMD] = FrameType;
	if (dwLen > 0)
		memcpy(&(m_BuffSend[SENDOFFSET_DATA]), lpData, dwLen);

	// Send Packet ----------------------------------------------------
	if (sendto(m_socket, (const char*)m_BuffSend, byLen, 0, (SOCKADDR*)&m_server, sizeof(m_server)) == SOCKET_ERROR)
	{
		if (bRecordLog)
		{
			m_nLastErr = WSAGetLastError();
			WSASetLastError(0);
		}

		// Close Socket
		CloseSocket();

		return FMC_SOCKET_ERROR;
	}

	return FMM_OK;
}

const INT CFSocket::SendTCPPacket(IN const BYTE iAxisNo, IN const BYTE byCmd, IN LPCBYTE lpData, IN const DWORD dwLen, IN const BOOL bRecordLog, IN const BOOL bReconnect)
{
	const DWORD cbCurSize = BuildPlusR8FramePacket(m_BuffSend, MAX_BUFFER_SIZE, iAxisNo, m_nSyncNo, byCmd, lpData, dwLen);

	if (cbCurSize)
	{
		// Send Packet ----------------------------------------------------
		if (FMM_OK != ClearReceiveTCPBuffer() ||
			SOCKET_ERROR == send(m_socket, (const char*)m_BuffSend, cbCurSize, 0))
		{
			if (bRecordLog)
			{
				m_nLastErr = WSAGetLastError();
				WSASetLastError(0);
			}

			// Close Socket
			CloseSocket();

			return FMC_SOCKET_ERROR;
		}

		if (0)
		{
			FILE* pFile = NULL;
			if (0 == fopen_s(&pFile, "d:\\PacketData.txt", "a+"))
			{
				char strText[1024] = { 0, };
				for (DWORD dwIndex = 0; dwIndex < cbCurSize; dwIndex++)
				{
					char strTemp[255] = { 0, };
					sprintf_s(strTemp, 255, "%02X ", m_BuffSend[dwIndex]);
					strcat_s(strText, 1024, strTemp);
				}
				strcat_s(strText, 1024, "\n");
				fprintf(pFile, strText);
				fclose(pFile);
			}
		}
	}
	return FMM_OK;

	//
	//	int byLen = (dwLen + SENDOFFSET_DATA);	// (Header, Length, Sync, CMD) + dwLen
	//
	//	// Make Packet -----------------------------------------------------
	//	m_BuffSend[SENDOFFSET_HEADER] = PACKET_HEADER;
	//	m_BuffSend[SENDOFFSET_LENGTH] = (unsigned char)(dwLen + (SENDOFFSET_CMD - SENDOFFSET_LENGTH));	// Sync, CMD
	//	m_BuffSend[SENDOFFSET_SYNC] = m_nSyncNo;
	//#ifdef INSERT_AXIS_IN_PACKET
	//	m_BuffSend[SENDOFFSET_AXIS] = iAxisNo;
	//#endif
	//	m_BuffSend[SENDOFFSET_CMD] = FrameType;
	//	if (dwLen > 0)
	//		memcpy(&(m_BuffSend[SENDOFFSET_DATA]), lpData, dwLen);
	//
	//	// Send Packet ----------------------------------------------------
	//	if (send(m_socket, (const char*)m_BuffSend, byLen, 0) == SOCKET_ERROR)
	//	{
	//		if (bRecordLog)
	//		{  
	//			m_nLastErr = WSAGetLastError();
	//			WSASetLastError(0);
	//		}
	//
	//		// Close Socket
	//		CloseSocket();
	//
	//		return FALSE;
	//	}
	//
	//	return TRUE;
}

const INT CFSocket::RecvUDPPacket(BYTE iAxisNo, BYTE FrameType, LPBYTE lpData, DWORD dwLen, DWORD dwWaitTime, BOOL bRecordLog)
{
	int nRead = 0;
	SOCKADDR_IN sender = { 0 };
	int SenderAddrSize = sizeof(sender);
	DWORD dwStartTime = timeGetTime();

	do
	{
		// Packet
		nRead = recvfrom(m_socket, (char*)m_BuffRecv, MAX_BUFFER_SIZE, 0, (SOCKADDR*)&sender, &SenderAddrSize);
		if (nRead == SOCKET_ERROR)
		{
			// Socket error
			m_nLastErr = WSAGetLastError();
			WSASetLastError(0);

			if (m_nLastErr == WSAETIMEDOUT)
			{
				//				DWORD dwElapTime = timeGetTime() - dwStartTime;

				if ((timeGetTime() - dwStartTime) > dwWaitTime)
				{
					if (bRecordLog)
					{

					}
					return FMC_TIMEOUT_ERROR;
				}
				else
				{
					continue;
				}
			}
			else
			{
				if (bRecordLog)
				{

				}

				// Close Socket
				CloseSocket();
				return FMC_DISCONNECTED;
			}
		}

		if (m_BuffRecv[RECVOFFSET_HEADER] != PACKET_HEADER)
		{
			// Reset
			continue;
		}

		if (nRead < (m_BuffRecv[RECVOFFSET_LENGTH] + RECVOFFSET_SYNC))
		{
			// Not enough
			if (bRecordLog)
			{
#ifdef _PRINT_LOG_MODE
				// ���� ������ ���
				TraceRecvBuffer(nRead);
#endif //_PRINT_LOG_MODE
			}
			return FMC_RECVPACKET_ERROR;
		}

		if (sender.sin_addr.s_addr != GetIPAddress().s_addr)
		{
			// Not mine.
			//if (bRecordLog)
			//	TraceMsg(_T("CSocket::RecvPacket(Axis %d, FrameType 0x%02X) : Not from Target Device : (address : %d.%d.%d.%d)\n"), iAxisNo, FrameType, sender.sin_addr.s_net, sender.sin_addr.s_host, sender.sin_addr.s_lh, sender.sin_addr.s_impno);
			continue;
		}

		if ((m_BuffRecv[RECVOFFSET_SYNC] != m_nSyncNo) ||
#ifdef INSERT_AXIS_IN_PACKET
		(m_BuffRecv[RECVOFFSET_AXIS] != iAxisNo) ||
#endif
			(m_BuffRecv[RECVOFFSET_CMD] != FrameType))
		{
			// Not mine. Reset
			continue;
		}

		if (m_BuffRecv[RECVOFFSET_STATUS] != FMM_OK)
		{
			// Communication Status is NOT OK.
			return m_BuffRecv[RECVOFFSET_STATUS];
		}

		if ((nRead != (dwLen + RECVOFFSET_DATA)) &&
			(((int)dwLen != MAX_BUFFER_SIZE) || (nRead > (int)dwLen)))
		{
			// Not enough
			if (bRecordLog)
			{
				//				TraceMsg(_T("CSocket::RecvPacket(Axis %d, FrameType 0x%02X) : FMC_RECVPACKET_ERROR : ((%d != (%d + %d)) && (%d != 256))\n"), iAxisNo, FrameType, nRead, dwLen, RECVOFFSET_DATA, dwLen);
#ifdef _PRINT_LOG_MODE
				// ���� ������ ���
				TraceRecvBuffer(nRead);
#endif //_PRINT_LOG_MODE
			}
			return FMC_RECVPACKET_ERROR;
		}

		// everything is FINE.
		if ((dwLen > 0) && (nRead > RECVOFFSET_DATA))
		{
			memcpy(lpData, &(m_BuffRecv[RECVOFFSET_DATA]), min(nRead, (int)dwLen));
		}

		return FMM_OK;
	} while (true);
}

const INT CFSocket::RecvTCPPacket( IN const BYTE iAxisNo, IN const BYTE FrameType, OUT LPBYTE lpData, IN const DWORD dwLen, IN const DWORD dwWaitTime, IN const BOOL bRecordLog)
{
	INT nRet = FMM_OK;

	int nTotalRecv = 0;
	const DWORD dwStartTime = timeGetTime();

	// Set socket opt
//	setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&dwWaitTime, sizeof(DWORD));
//	setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&dwWaitTime, sizeof(DWORD));

	PCHAR pBuffer = NULL;

	while (1)
	{
		//	Check received socket buffer
		ULONG cbSize = -1;
		if (SOCKET_ERROR == ioctlsocket(m_socket, FIONREAD, &cbSize))
		{
			nRet = FMC_SOCKET_ERROR;
			break;
		}

		//	Check Timeout
		if (0 == cbSize)
		{
			
			if (dwWaitTime <= timeGetTime() - dwStartTime)
			{
				nRet = FMC_TIMEOUT_ERROR;
				break;
			}

			Sleep(1);
			continue;
		}

		//	Read data from socket
		if (pBuffer)
		{
			delete pBuffer;
		}

		pBuffer = new CHAR[cbSize];

		const INT nRead = recv(m_socket, pBuffer, cbSize, 0);
		if (nRead == SOCKET_ERROR)
		{
			nRet = FMC_SOCKET_ERROR;
			break;
		}

		//	Interpret data
		nRet = InterpretReceivedProtocol(iAxisNo, FrameType, lpData, dwLen, (LPBYTE)pBuffer, cbSize );
		if( nRet != FMC_NOT_COMPLPETE )
		{
			break;
		}
	}

	if (FMC_SOCKET_ERROR == nRet)	//		Socket error
	{
		m_nLastErr = WSAGetLastError();
		WSASetLastError(0);
		CloseSocket();
	}

	if (pBuffer)
	{
		delete pBuffer;
	}

	return nRet;
}

const INT CFSocket::DoTrySendCommand(IN const BYTE iAxisNo, IN const BYTE byCmd, IN LPCVOID lpIN, IN const DWORD dwINlen, OUT LPVOID lpOUT, IN const DWORD dwOUTlen, IN const DWORD dwWaitTime, IN const BOOL bReconnect)
{
	int nRtn = FMM_OK;

	WaitForSingleObject(m_hCritical, INFINITE);

	if (m_socket != INVALID_SOCKET)
	{
		m_nSyncNo++;

		if (m_bTCP)
		{
			nRtn = SendTCPPacket(iAxisNo, byCmd, (BYTE*)lpIN, dwINlen);
			if (FMM_OK != nRtn && bReconnect)
			{
				if (_Reconnect())
				{
					nRtn = SendTCPPacket(iAxisNo, byCmd, (BYTE*)lpIN, dwINlen);
				}
			}

			if( FMM_OK == nRtn)
			{
				nRtn = RecvTCPPacket(iAxisNo, byCmd, (BYTE*)lpOUT, dwOUTlen, dwWaitTime);
			}
		}
		else
		{
			// Send
			if (SendUDPPacket(iAxisNo, byCmd, (BYTE*)lpIN, dwINlen, FALSE))
			{
				// Recv
				nRtn = RecvUDPPacket(iAxisNo, byCmd, (BYTE*)lpOUT, dwOUTlen, dwWaitTime, FALSE);
			}
			else
			{
				// Send Failed!
				nRtn = FMC_DISCONNECTED;
			}
		}
	}
	else
	{
		nRtn = FMM_NOT_OPEN;
	}

	ReleaseMutex(m_hCritical);

	return nRtn;
}

const INT CFSocket::DoSendCommand(IN const BYTE iAxisNo, IN const BYTE byCmd, IN LPCVOID lpIN, IN const DWORD dwINlen, OUT LPVOID lpOUT, IN const DWORD dwOUTlen, IN const DWORD dwWaitTime, IN const BOOL bStopRecordLog, IN const BOOL bReconnect)
{
								
	int nRtn = FMM_OK;

	WaitForSingleObject(m_hCritical, INFINITE);

	if (m_socket != INVALID_SOCKET)
	{
		m_nSyncNo++;

		BOOL bExit = FALSE;
		INT nTimeoutCounter = 3;
		INT nCheckSumCounter = 3;
		while (FALSE == bExit)
		{
			if (m_bTCP)
			{
				nRtn = SendTCPPacket(iAxisNo, byCmd, (BYTE*)lpIN, dwINlen);
				if (FMM_OK != nRtn && bReconnect)
				{
					if (_Reconnect())
					{
						nRtn = SendTCPPacket(iAxisNo, byCmd, (BYTE*)lpIN, dwINlen);
					}
				}

				switch (nRtn)
				{
				case FMM_OK:
				{
					//	�۽� �Ϸ�
					nRtn = RecvTCPPacket(iAxisNo, byCmd, (BYTE*)lpOUT, dwOUTlen, dwWaitTime, !bStopRecordLog);
					switch (nRtn)
					{
					case FMM_OK:
					{
						//	���� �Ϸ�
						bExit = TRUE;
					}	break;

					case FMC_CRCFAILED_ERROR:
					{
						nCheckSumCounter--;
						if (0 == nCheckSumCounter)
						{
							nRtn = FMC_CRCFAILED_ERROR;
							bExit = TRUE;
						}
					}	break;

					case FMC_TIMEOUT_ERROR:
					{
						nTimeoutCounter--;
						if (0 == nTimeoutCounter )
						{
							nRtn = FMC_NO_RESPONSE;
							bExit = TRUE;
						}
					}	break;

					default:
					{
						bExit = TRUE;
					}	break;
					}	// End of 'switch (nRtn)'
				}	break;	// End of 'case FMM_OK:'

				default:	// ������ ����
				{
					bExit = TRUE;
				}	break;
				}	// End of 'switch (nRtn)'
			}
			else
			{
				assert(0);
			}
		}
	}
	else
	{
		nRtn = FMM_NOT_OPEN;
	}

	ReleaseMutex(m_hCritical);

	return nRtn;
}

const INT CFSocket::DoSendCommandNoResp(IN const BYTE iAxisNo, IN const BYTE byCmd, IN LPCVOID lpIN, IN const DWORD dwINlen, IN const BOOL bReconnect)
{
	int nRtn = FMM_OK;

	WaitForSingleObject(m_hCritical, INFINITE);

	if (m_socket != INVALID_SOCKET)
	{
		m_nSyncNo++;

		// Send
		if (m_bTCP)
		{
			if (!SendTCPPacket(iAxisNo, byCmd, (BYTE*)lpIN, dwINlen, FALSE))
			{
				if (local_bAutoReconnect)
				{
					// Reconnect to other port
					if (_Reconnect())
					{
						if (!SendTCPPacket(iAxisNo, byCmd, (BYTE*)lpIN, dwINlen, FALSE))
						{
							nRtn = FMC_DISCONNECTED;
						}
					}
				}
			}
		}
		else
		{
			if (!SendUDPPacket(iAxisNo, byCmd, (BYTE*)lpIN, dwINlen, FALSE))
			{
				nRtn = FMC_DISCONNECTED;
			}
		}
	}
	else
	{
		nRtn = FMM_NOT_OPEN;
	}

	ReleaseMutex(m_hCritical);

	return nRtn;
}

void CFSocket::SetOptionAutoReconnect(BOOL bSET)
{
	local_bAutoReconnect = bSET;
}

#ifdef _PRINT_LOG_MODE
void CFSocket::LogEnable(BOOL bEnable)
{
	local_bWriteLogs = bEnable;
}

BOOL CFSocket::LogSetPath(LPCTSTR lpPath)
{
	// verify lpPath 
	if ((lpPath == NULL) || (_tcslen(lpPath) == 0))
	{
		// NULL path.
		// path will be current path.
		memset(local_chLogFilePath, 0x00, sizeof(local_chLogFilePath));
	}
	else if (_tcslen(lpPath) < (_MAX_PATH * 2 - 4))
	{
		if (PathIsDirectory(lpPath))
		{
			memset(local_chLogFilePath, 0x00, sizeof(local_chLogFilePath));
			_tcscpy_s(local_chLogFilePath, sizeof(local_chLogFilePath) / sizeof(local_chLogFilePath[0]), lpPath);

			if ((&(local_chLogFilePath[_tcslen(lpPath) - 1]) != _tcsrchr(local_chLogFilePath, _T('\\'))) &&
				(&(local_chLogFilePath[_tcslen(lpPath) - 1]) != _tcsrchr(local_chLogFilePath, _T('/'))))
			{
				_tcscat_s(local_chLogFilePath, _MAX_PATH * 2, _T("\\"));
			}
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

bool CFSocket::OpenLogFile(struct tm* today)
{
	TCHAR chFilePath[_MAX_PATH * 3];
	TCHAR chFileName[_MAX_PATH];
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR * 3];

	if (m_pfileLogMsg != NULL)
		CloseLogFile();

	m_tmLogYear = today->tm_year;
	m_tmLogDay = today->tm_yday;

	if (_tcslen(local_chLogFilePath) == 0)
	{
		// Get Path.
		GetModuleFileName(NULL, chFilePath, _MAX_PATH * 3);
		_tsplitpath_s(chFilePath, drive, _MAX_DRIVE, dir, _MAX_DIR, NULL, 0, NULL, 0);		// chFilePath -> drive, dir
	}
	else
	{
		_tsplitpath_s(local_chLogFilePath, drive, _MAX_DRIVE, dir, _MAX_DIR, NULL, 0, NULL, 0); // local_chLogFilePath -> drive, dir
	}

	// Make file name
	{
		TCHAR chDate[50];
		_tcsftime(chDate, 50, _T("%y_%m_%d"), today);
		_stprintf_s(chFileName, _MAX_PATH, _T("PE_Logfile_%d.%d.%d.%d_%s.txt"), GetIPAddress().s_net, GetIPAddress().s_host, GetIPAddress().s_lh, GetIPAddress().s_impno, chDate);
	}

	// Make full path.
	_tmakepath_s(chFilePath, _MAX_PATH * 3, drive, dir, chFileName, NULL);	// chFilePath <- drive, dir, chFileName

	m_pfileLogMsg = _tfsopen(chFilePath, _T("ab"), _SH_DENYWR);
	if (m_pfileLogMsg == NULL)
	{
		m_pfileLogMsg = NULL;
		return false;
	}
	else
	{
		fseek(m_pfileLogMsg, 0, SEEK_END);

		// for UNICODE
		int fileLen = ftell(m_pfileLogMsg);
		if (fileLen <= 0)
		{
			_fputtc(0xFEFF, m_pfileLogMsg);
		}

		return true;
	}
}

void CFSocket::CloseLogFile()
{
	if (m_pfileLogMsg != NULL)
	{
		fflush(m_pfileLogMsg);
		//		Sleep(1000);
		fclose(m_pfileLogMsg);

		m_pfileLogMsg = NULL;
	}
}

void CFSocket::Trace0Msg(LPCTSTR pszMessage)
{
	TCHAR chBuff[24];
	time_t lTime;
	struct tm today = { 0 };

	if (local_bWriteLogs)
	{
		WaitForSingleObject(m_hLogSync, INFINITE);

		time(&lTime);
		localtime_s(&today, &lTime);
		if ((today.tm_year != m_tmLogYear) || (today.tm_yday != m_tmLogDay) || (m_pfileLogMsg == NULL))
		{
			OpenLogFile(&today);
		}

		if (m_pfileLogMsg != NULL)
		{
			// �ð� ���.
			_tcsftime(chBuff, 24, _T("[%H:%M:%S]"), &today);
			_ftprintf_s(m_pfileLogMsg, chBuff);
			// �޼��� ���.
			fwrite(pszMessage, sizeof(TCHAR), _tcslen(pszMessage), m_pfileLogMsg);
		}

		ReleaseMutex(m_hLogSync);
	}
}

void CFSocket::TraceMsg(LPCTSTR pszFormat, ...)
{
	va_list va;
	time_t lTime;
	struct tm today = { 0 };
	TCHAR chBuff[24];

	if (local_bWriteLogs)
	{
		WaitForSingleObject(m_hLogSync, INFINITE);

		time(&lTime);
		localtime_s(&today, &lTime);
		if ((today.tm_year != m_tmLogYear) || (today.tm_yday != m_tmLogDay) || (m_pfileLogMsg == NULL))
		{
			OpenLogFile(&today);
		}

		if (m_pfileLogMsg != NULL)
		{
			// �ð� ���.
			_tcsftime(chBuff, 24, _T("[%H:%M:%S]"), &today);
			_ftprintf_s(m_pfileLogMsg, chBuff);

			// �޼��� ���.
			va_start(va, pszFormat);
			_vftprintf(m_pfileLogMsg, pszFormat, va);
			va_end(va);
		}

		ReleaseMutex(m_hLogSync);
	}
}

void CFSocket::TraceSendBuffer(UINT nLen)
{
	DWORD i;

	if ((m_pfileLogMsg != NULL) && local_bWriteLogs)
	{
		WaitForSingleObject(m_hLogSync, INFINITE);

		_ftprintf_s(m_pfileLogMsg, _T("Send_Buffer :\n"));

		// SendBuffer ���.
		for (i = 0; i < nLen; i++)
		{
			if (((i + 1) % 20) == 1)
				_ftprintf_s(m_pfileLogMsg, _T("\t"));

			_ftprintf_s(m_pfileLogMsg, _T("0x%02X "), m_BuffSend[i]);

			if (((i + 1) % 20) == 0)
				_ftprintf_s(m_pfileLogMsg, _T("\n"));
		}

		_ftprintf_s(m_pfileLogMsg, _T("\n"));

		ReleaseMutex(m_hLogSync);
	}
}

void CFSocket::TraceRecvBuffer(UINT nLen)
{
	DWORD i;

	if (nLen <= 0)
		return;

	if ((m_pfileLogMsg != NULL) && local_bWriteLogs)
	{
		WaitForSingleObject(m_hLogSync, INFINITE);

		_ftprintf_s(m_pfileLogMsg, _T("Recv_Buffer :\n"));

		// SendBuffer ���.
		for (i = 0; i < nLen; i++)
		{
			if (((i + 1) % 20) == 1)
				_ftprintf_s(m_pfileLogMsg, _T("\t"));

			_ftprintf_s(m_pfileLogMsg, _T("0x%02X "), m_BuffRecv[i]);

			if (((i + 1) % 20) == 0)
				_ftprintf_s(m_pfileLogMsg, _T("\n"));
		}

		_ftprintf_s(m_pfileLogMsg, _T("\n"));

		ReleaseMutex(m_hLogSync);
	}
}
#endif //_PRINT_LOG_MODE

static const WORD TABLE_CRCVALUE[] =
{
	0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
	0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
	0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
	0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
	0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
	0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
	0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
	0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
	0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
	0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
	0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
	0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
	0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
	0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
	0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
	0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
	0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
	0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
	0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
	0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
	0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
	0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
	0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
	0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
	0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
	0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
	0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
	0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
	0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
	0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
	0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
	0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040
};

const WORD CFSocket::CalcCRC(IN LPCBYTE lpBuff, IN const DWORD dwLen)
{
	LPCBYTE pBuffer = lpBuff;
	DWORD dwIndex = dwLen;

	WORD wCRCWord = 0xFFFF;

	while (dwIndex--)
	{
		//nTemp = wCRCWord ^ *(lpBuff++);
		//wCRCWord >>= 8;
		//wCRCWord ^= TABLE_CRCVALUE[nTemp];
		wCRCWord = (wCRCWord >> 8) ^ TABLE_CRCVALUE[((wCRCWord ^ *pBuffer++) & 0xFF)];
	}

	return wCRCWord;
}

const DWORD CFSocket::BuildPlusR8FramePacket(OUT LPBYTE pBuffer, IN const INT cbSize, IN const BYTE iSlaveNo, IN const BYTE iSyncNo, IN const BYTE FrameType, IN LPCBYTE lpData, IN const DWORD dwLen)
{
	/**
	*	1�� Build Basic Frame
	*/
	BYTE btBuffer[1024] = { 0, };
	LPBYTE ptr = btBuffer;

	*ptr++ = iSlaveNo;
	*ptr++ = iSyncNo;
	*ptr++ = FrameType;

	memcpy(ptr, lpData, dwLen);
	ptr += dwLen;

	const WORD wCRC = CalcCRC(btBuffer, ptr - btBuffer);
	*(ptr++) = LOBYTE(wCRC);
	*(ptr++) = HIBYTE(wCRC);

	/**
	*	2�� Add Header, Tail and Byte Stuffing of Frame
	*/
	const DWORD cbBuffer = ptr - btBuffer;
	ptr = btBuffer;

	LPBYTE ptrSend = pBuffer;

	*ptrSend++ = ASCII_NODE;
	*ptrSend++ = ASCII_NODESTART;

	for (DWORD dwIndex = 0; dwIndex < cbBuffer; dwIndex++)
	{
		*ptrSend++ = *ptr;
		if (ASCII_NODE == *ptr)
		{
			*ptrSend++ = *ptr;
		}

		ptr++;
	}

	*ptrSend++ = ASCII_NODE;
	*ptrSend++ = ASCII_NODEEND;

	const DWORD dwResult = ptrSend - pBuffer;
	return dwResult;
}

const INT CFSocket::GetLastSockError()
{
	return m_nLastErr;
}

const INT CFSocket::GetDefinedLastSockError()
{
	INT nRet = FMC_UNKNOWN_ERROR;
	switch (m_nLastErr)
	{
	case 0:
	{
		nRet = FMM_OK;
	}	break;

	case WSAECONNREFUSED:
	{
		nRet = FMC_REFUSED;
	}	break;
	}

	return m_nLastErr;

	/*WSA_INVALID_HANDLE 6 ������ �̺�Ʈ ��ü �ڵ��� �߸� �Ǿ���.
	WSA_NOT_ENOUGH_MEMORY 8 �޸𸮰� ������� �ʴ�.
	WSA_INVALID_PARAMETER 87 �ϳ� �̻��� �߸��� ���ڰ� ���ƴ�.
	WSA_OPERATION_ABORTED 995 overlapped ������ �ߴ� �Ǿ���.

	WSA_IO_INCOMPLETE 996 Overlapped ����� �̺�Ʈ�� ��ȣ ���°� �ƴϴ�.
	WSA_IO_PENDING 997 Overlapped ������ ���߿� �Ϸ�� ���̴�.��ø ������ ���� �غ� �Ǿ�����, ��� �Ϸ���� �ʾ��� ���߻�
	WSAEINTR 10004 WSACancelBlockingCall()�� ���� ���ȭ ȣ���� ��� : Interrupted system call
	WSAEBADF 10009 �߸��� ���� ����ڰ� ���Ǿ���
	WSAEACCES 10013 ��û�� �ּҰ� ��ε�ĳ��Ʈ �ּ��ε� setsockopt()�� SO_BROADCAST �� �������� �ʾҴ�.
	WSAEFAULT 10014 �߸��� �ּҸ� �������
	WSAEINVAL 10022 ���ε� ����.�̹� bind�� ���Ͽ� ���ε��ϰų� �ּ�ü�谡 �ϰ������� ���� ��
	WSAEMFILE 10024 �ʹ� ���� ������ ��������
	WSAEWOULDBLOCK 10035 non overlapped ���� : �� ���� ���Ͽ� ���� ���� �����Ͱ� ����, overlapped(:12) ���� : �ʹ� ���� ��ø ����� �䱸�� ����
	WSAEINPROGRESS 10036 ���ȭ �Լ��� ����Ǵ� ���� �������� ���� API�Լ��� ȣ��.
	WSAEALREADY 10037 ������� ���Ͽ��� ���� �������� �۾���. (connect�� �Ϸ���� ���� ���¿��� connect �Լ��� ȣ���..)
	WSAENOTSOCK 10038 �߸��� ���ϱ���ڸ� �������
	WSAEDESTADDRREQ 10039 ������ �ּҸ� ������� �ʾ���
	WSAEMSGSIZE 10040 �ۼ��ſ� ���� �����Ͱ� ������ ũ�⸦ �ʰ��ؼ� ũ�⿡ �°� �߷���
	WSAEPROTOTYPE 10041 ���Ͽ� ���� �ʴ� ���������� �������
	WSAENOPROTOOPT 10042 �߸��� �ɼ��� �������.�������� �ʴ� �ɼ����� getsockopt�Լ��� ȣ���ϴ� ��.
	WSAEPROTONOSUPPORT 10043 ���ӿ��� �������� �ʴ� �ּ�ü�踦 �������
	WSAESOCKTNOSUPPORT 10044 ����Ÿ���� �������� �ʴ� �ּ�ü�踦 �������
	WSAEOPNOTSUPP 10045 ������ �������� �ʴ� ����� �������.listen()�Լ��� �����ͱ׷� ���(SO_DGRAM)���� ȣ��
	WSAEPFNOSUPPORT 10046 �������� �ʴ� ���������� �������
	WSAEAFNOSUPPORT 10047 ���ӿ��� �������� �ʴ� �ּ�ü�踦 �������
	WSAEADDRINUSE 10048 ������ �ּҰ� �̹� ��� ����
	WSAEADDRNOTAVAIL 10049 ����� �� ���� �ּ���
	WSAENETDOWN 10050 ��Ʈ��ũ ���� �ý��ۿ� ������ ����.��Ʈ��ũ ���� �����.
	WSAENETUNREACH 10051 ��ũ��ũ�� ������ �� ����
	WSAENETRESET 10052 ��Ʈ��ũ �缳������ ������ ��������.
	WSAECONNABORTED 10053 Ÿ�Ӿƿ� Ȥ�� ������ ���������� ���� ����Ʈ�������� ������ ������ ������.
	WSAECONNRESET 10054 ������ ���� ȣ��Ʈ�� ���� �缳���Ǿ���.
	WSAENOBUFS 10055 �����ִ� ���۰����� ��� ������ ����� �� ����
	WSAEISCONN 10056 �̹� ������ �Ϸ�� ������.connect�� ������ �Ϸ�� ���Ͽ� �ٽ� connect�� �õ��� ���
	WSAENOTCONN 10057 ������� ���� ������.������� ���� ���Ͽ� �а� ���� ���
	WSAESHUTDOWN 10058 ������ ����Ǿ���.����� ���Ͽ� �����͸� �а� ���� ���
	WSAETOOMANYREFS 10059 Too many references
	WSAETIMEDOUT 10060 ���ӽõ� ���� �ð� �ʰ�
	WSAECONNREFUSED 10061 ������ ����õ��� ������.���� ��⿭�� ������ ���¿��� Ŭ���̾�Ʈ�� connect ȣ���� ���� ���
	WSAELOOP 10062 �ʹ� ���� �ɺ�����ũ�� ���Ǿ���
	WSAENAMETOOLONG 10063 �����̸��� �ʹ� ��
	WSAEHOSTDOWN 10064 ȣ��Ʈ �ٿ�
	WSAEHOSTUNREACH 10065 ȣ��Ʈ���� ��θ� ������ �� ����
	WSAENOTEMPTY 10066 ��Ʈ��ũ ����ý����� ���� ����� �غ� �Ǿ� ���� ����.WSAStartup�� ���� ��ȯ
	WSAEPROCLIM 10067 �ʹ� ���� ���μ����� �����Ǿ���
	WSAEUSERS 10068 ����ڰ� �ʹ� ����
	WSAEDQUOT 10069 ��ũ ��� �Ҵ緮 �ʰ�
	WSAESTALE 10070 Stale NFS file handle
	WSAEREMOTE 10071 Too many levels of remote in path
	WSAEDISCON 10101 ���ᰡ ��������
	WSASYSNOTREADY 10091 ��Ʈ��ũ ����ý����� ���� ����� �غ� �Ǿ� ���� �ʾ���
	WSAVERNOTSUPPORTED 10092 ������ �������� �ʴ� �������� ������ �������.WSAStartUp()���� ��ȯ
	WSANOTINITIALISED 10093 WSAStartup() �Լ��� ���������� ������� ���� ��Ȳ���� ���� �Լ��� ȣ������ ��
	WSAHOST_NOT_FOUND 11001 ��û�� ȣ��Ʈ�� ã�� �� ����
	WSATRY_AGAIN 11002 ��û�� ȣ��Ʈ�� ã�� �� ����
	WSANO_RECOVERY 11003 ������ �� ���� ������ �߻��Ͽ���
	WSANO_DATA 11004 ��û�� �̸��� ��ȿ������ ������ ã�� ������.gethostbyaddr()��� �߻�*/
}

const INT CFSocket::InterpretReceivedProtocol(IN const BYTE iAxisNo, IN const BYTE FrameType, OUT LPBYTE lpData, IN const DWORD dwLen, IN LPBYTE lpBuffer, IN const DWORD cbBuffer)
{
	if (MAX_BUFFER_SIZE - m_cbBuffRecv < cbBuffer)
	{
		//	Insufficient Buffer
		return FMC_INSUFFICIENT_BUFFER;
	}

	memcpy(&m_BuffRecv[m_cbBuffRecv], lpBuffer, cbBuffer);
	m_cbBuffRecv += cbBuffer;

	BYTE	btBuffer[MAX_BUFFER_SIZE] = { 0 };
	LPBYTE	pbtBuffer = nullptr;
	LPBYTE	pbtGlobalBuffer = m_BuffRecv;

	BOOL bNodeMode = FALSE;
	for (DWORD dwIndex = 0; dwIndex < m_cbBuffRecv; dwIndex++)
	{
		if (FALSE == bNodeMode)
		{
			if (ASCII_NODE != *pbtGlobalBuffer)
			{
				//	0xaa�� ������ �ʾ��� �� ( �Ϲ� ������ )
				if (pbtBuffer)
				{
					*pbtBuffer++ = *pbtGlobalBuffer;
				}
			}
			else
			{
				//	Byte Stuffing
				//	0xaa�� ������ �� ( ���� 1��  Byte�� �����ؾ��Ѵ�. )
				//	0xaa 0xaa	= Data 0xaa
				//	0xaa 0xcc	= Packet ����
				//	0xaa 0xee	= Packet ����
				bNodeMode = TRUE;
			}
		}
		else
		{
			bNodeMode = FALSE;

			switch (*pbtGlobalBuffer)
			{
			case ASCII_NODE:
			{
				if (pbtBuffer)
				{
					*pbtBuffer++ = *pbtGlobalBuffer;
				}
			}	break;

			case ASCII_NODESTART:
			{
				pbtBuffer = btBuffer;
			}	break;

			case ASCII_NODEEND:
			{
				if (pbtBuffer)
				{
					//	Frame ����
					const DWORD dwRecvLen = (pbtBuffer - btBuffer - 2);
					const WORD wRevCRC16 = *(WORD*)(pbtBuffer - 2);
					const WORD wCalcCRC16 = CalcCRC(btBuffer, dwRecvLen);

					if (wRevCRC16 != wCalcCRC16)
					{
						return FMC_CRCFAILED_ERROR;
					}
					else if (btBuffer[0] != iAxisNo)
					{
						return FMM_INVALID_SLAVE_NUM;
					}
					else if (btBuffer[1] != m_nSyncNo)
					{
						return FMP_PACKETERROR;
					}
					else if (btBuffer[2] != FrameType)
					{
						return FMP_FRAMETYPEERROR;
					}
					else if (btBuffer[3] != FMM_OK)
					{
						//	Error Code from Device
						return btBuffer[3];
					}
					else
					{
						if (0)
						{
							if (0xcd == btBuffer[2] && 0x00 != btBuffer[16])
							{
								int i = 0;
							}

							FILE* pFile = NULL;
							if (0 == fopen_s(&pFile, "d:\\PacketData.txt", "a+"))
							{
								const DWORD dwLen = pbtBuffer - btBuffer;

								char strText[1024] = { 0, };
								for (DWORD dwIndex = 0; dwIndex < dwLen; dwIndex++)
								{
									char strTemp[255] = { 0, };
									sprintf_s(strTemp, 255, "%02X ", btBuffer[dwIndex]);
									strcat_s(strText, 1024, strTemp);
								}
								strcat_s(strText, 1024, "\n");
								fprintf(pFile, strText);
								fclose(pFile);
							}
						}

						if (dwLen && lpData)
						{
							const DWORD dwOffset = 4;
							const DWORD cbData = dwRecvLen - dwOffset;
							if (dwLen < cbData)
							{
								return FMC_INSUFFICIENT_BUFFER2;
							}
							else
							{
								memcpy_s(lpData, dwLen, &btBuffer[dwOffset], cbData);
								return FMM_OK;
							}
						}
						else
						{
							return FMM_OK;
						}
					}
				}
			}	break;

			default:
			{
				return FMC_DATA_ERROR;
			}	break;
			}	// End of 'switch (*pbtGlobalBuffer)'
		}

		pbtGlobalBuffer++;
	}

	return FMC_NOT_COMPLPETE;
}
