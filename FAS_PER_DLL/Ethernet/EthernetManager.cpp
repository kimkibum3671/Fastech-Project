#pragma once

#include "EthernetManager.h"

#include <shlwapi.h>
#include <Shlobj.h>

#define DEF_PACKET_SIZE 32
#define MAX_PACKET_SIZE 1024

#define ICMP_MIN 8
#define ICMP_ECHO 8
#define ICMP_ECHOREPLY 0

/////////////////////////////////////////////////////////////////////////////
// IpHeader

typedef struct
{
	unsigned int h_len:4;          // length of the header
	unsigned int version:4;        // Version of IP
	unsigned char tos;             // Type of service
	unsigned short total_len;      // total length of the packet
	unsigned short ident;          // unique identifier
	unsigned short frag_and_flags; // flags
	unsigned char ttl; 
	unsigned char proto;           // protocol (TCP, UDP etc)
	unsigned short checksum;       // IP checksum
	unsigned int sourceIP;
	unsigned int destIP;
} IPHEADER;

/////////////////////////////////////////////////////////////////////////////
// IcmpHeader

typedef struct
{
	BYTE i_type;
	BYTE i_code;				   // type sub code
	USHORT i_cksum;
	USHORT i_id;
	USHORT i_seq;
} ICMPHEADER;

typedef struct
{
	ICMPHEADER ipHeader;
	char dataPart[MAX_PACKET_SIZE];
} PINGPACKET;


CEthernetManager::CEthernetManager()
{
	m_lstSocket.clear();

	Ethernet_Start();

#ifdef _PRINT_LOG_MODE
	TCHAR chAppDataPath[_MAX_PATH] = _T("");
	TCHAR chPATH[_MAX_PATH * 2];
	if (SUCCEEDED(SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, 0, chAppDataPath)))
	{
		_stprintf_s(chPATH, _MAX_PATH * 2, _T("%s\\FASTECH\\EziMOTION\\"), chAppDataPath);
		CFSocket::LogSetPath(chPATH);
	}
	CFSocket::LogEnable(TRUE);
#endif
}

CEthernetManager::~CEthernetManager()
{
	Ethernet_Stop();
}

CEthernetManager* CEthernetManager::GetInstance()
{
	static CEthernetManager _OneInstance;
	return &_OneInstance;
}

void CEthernetManager::Ethernet_Start()
{
	WORD	wVersion = MAKEWORD(2, 2);
	WSADATA wsaData;

	RemoveAllSocket();

	WSAStartup(wVersion, &wsaData);
}

void CEthernetManager::Ethernet_Stop()
{
	RemoveAllSocket();
	WSACleanup();
}

BOOL CEthernetManager::Ping(BYTE sb1, BYTE sb2, BYTE sb3, BYTE sb4)
{
	SOCKET hsocket;
	sockaddr_in ip;
	PINGPACKET pingpacket;

	IPHEADER *pipheader;
	ICMPHEADER *picmp;

	WORD wCurrID;
	int nPacketLen;
	UINT nTimeout = 50;

	WORD wChecksum, *ptr;
	int i, nSocketStatus, nRtn;

	hsocket = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0, WSA_FLAG_OVERLAPPED);

	nRtn = setsockopt(hsocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&nTimeout, sizeof(nTimeout));
	nRtn = setsockopt(hsocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&nTimeout, sizeof(nTimeout));

	ip.sin_addr.s_net = sb1;
	ip.sin_addr.s_host = sb2;
	ip.sin_addr.s_lh = sb3;
	ip.sin_addr.s_impno = sb4;
	ip.sin_family = AF_INET;

	wCurrID = (WORD)GetCurrentProcessId();
	nPacketLen = sizeof(ICMPHEADER) + DEF_PACKET_SIZE;

	memset(&pingpacket.dataPart, 'E', DEF_PACKET_SIZE);

	pingpacket.ipHeader.i_type = ICMP_ECHO;
	pingpacket.ipHeader.i_code = 0;
	pingpacket.ipHeader.i_id = wCurrID;
	pingpacket.ipHeader.i_cksum = 0;
	pingpacket.ipHeader.i_seq = 0;

	wChecksum = 0;
	ptr = (WORD*)&pingpacket;
	for (i=0; i<nPacketLen/2; i++)
		wChecksum += (WORD)*(ptr+i);
	pingpacket.ipHeader.i_cksum = ~(wChecksum);

	nRtn = FALSE;

	do
	{
		nSocketStatus = sendto(hsocket, (const char*)&pingpacket, nPacketLen, 0, (sockaddr*)&ip, sizeof(sockaddr_in));
		if (nSocketStatus == SOCKET_ERROR) break;

		nSocketStatus = recvfrom(hsocket, (char*)&pingpacket, MAX_PACKET_SIZE, 0, NULL, NULL);
		if (nSocketStatus == SOCKET_ERROR) break;

		pipheader = (IPHEADER*)&pingpacket;
		if (nSocketStatus < (long)(pipheader->h_len * 4 + ICMP_MIN)) break;

		picmp = (ICMPHEADER*)((char*)&pingpacket + pipheader->h_len * 4);
		if (picmp->i_type != ICMP_ECHOREPLY) break;
		if (picmp->i_id != (WORD)GetCurrentProcessId()) break;

		nRtn = TRUE;
	}
	while (0);

	closesocket(hsocket);

	return nRtn;
}

const INT CEthernetManager::MakeNewSocket(const BOOL bTCP, const INT iID, const BYTE sb1, const BYTE sb2, const BYTE sb3, const BYTE sb4, const WORD wPortNo)
{
	TargetInterface* pSocket;

	pSocket = GetSocketbyIP(sb1, sb2, sb3, sb4);
	if (pSocket)
	{
		return FMM_OK;
	}

	IN_ADDR addr;
	addr.s_net = sb1;
	addr.s_host = sb2;
	addr.s_lh = sb3;
	addr.s_impno = sb4;

	pSocket = new TargetInterface(bTCP, addr, wPortNo, iID);
	
	INT nRet = pSocket->ConnectSocket();
	if ( FMM_OK == nRet )
	{
		nRet = pSocket->DoAck(0xfa, SOCKET_WAITTIME_NORMAL);
	}

	if (FMM_OK == nRet)
	{
		m_lstSocket.insert(std::pair<int, TargetInterface*>(iID, pSocket));
	}
	else
	{
		delete pSocket;
	}

	return nRet;
}

const INT CEthernetManager::RemoveAllSocket()
{
	SOCKET_ITER iter;
	for (iter = m_lstSocket.begin(); iter != m_lstSocket.end(); iter++)
	{
		TargetInterface* pSocket = iter->second;

		pSocket->CloseSocket();

		delete pSocket;
	}

	m_lstSocket.clear();

	return FMM_OK;
}

const INT CEthernetManager::RemoveSocket(int iBdID)
{
	SOCKET_ITER iter;
	iter = m_lstSocket.find(iBdID);
	if (iter != m_lstSocket.end())
	{
		TargetInterface* pSocket = iter->second;
		delete pSocket;

		m_lstSocket.erase(iter);

		return FMM_OK;
	}

	return FMM_NOT_OPEN;
}

TargetInterface* CEthernetManager::GetSocketbyIP(BYTE sb1, BYTE sb2, BYTE sb3, BYTE sb4)
{
	SOCKET_ITER iter;

	for (iter = m_lstSocket.begin(); iter != m_lstSocket.end(); iter++)
	{
		TargetInterface* pSocket = iter->second;

		if ((pSocket->GetIPAddress(0) == sb1)
			&& (pSocket->GetIPAddress(1) == sb2)
			&& (pSocket->GetIPAddress(2) == sb3)
			&& (pSocket->GetIPAddress(3) == sb4))
		{
			return pSocket;
		}
	}

	return NULL;
}

TargetInterface* CEthernetManager::GetSocketbyBdID(int iBdID)
{
	SOCKET_ITER iter;

	iter = m_lstSocket.find(iBdID);
	if (iter != m_lstSocket.end())
	{
		return iter->second;
	}

	return NULL;
}
