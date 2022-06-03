#include "UDPBroadCastSocket.h"

#include <ws2tcpip.h>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")

#include <deque>

std::deque<ULONG> global_lstNIC;
std::deque<ULONG> global_lstDeviceAddress;

#define SYNC_NO_FOR_BROADCAST	0xE0
#define UDP_PORT_NO		3001

CUDPBroadCastSocket::CUDPBroadCastSocket(void)
{
	m_socket = INVALID_SOCKET;

	m_nSyncNo = SYNC_NO_FOR_BROADCAST;
}


CUDPBroadCastSocket::~CUDPBroadCastSocket(void)
{
	Close();

	global_lstNIC.clear();
	global_lstDeviceAddress.clear();
}

void CUDPBroadCastSocket::Close()
{
	if (m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket);

		m_socket = INVALID_SOCKET;
	}
}

int CUDPBroadCastSocket::ReadNICAddress()
{
	PMIB_IPADDRTABLE pIPAddrTable;
	DWORD dwSize = 0;
	DWORD dwRetVal = 0;
	IN_ADDR ipaddr;
	IN_ADDR subnet;

	global_lstNIC.clear();

	pIPAddrTable = (MIB_IPADDRTABLE *) malloc(sizeof (MIB_IPADDRTABLE));

	if (pIPAddrTable)
	{
		// Make an initial call to GetIpAddrTable to get the
		// necessary size into the dwSize variable
		if (GetIpAddrTable(pIPAddrTable, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER)
		{
			free(pIPAddrTable);
			pIPAddrTable = (MIB_IPADDRTABLE *) malloc(dwSize);
		}

		if (pIPAddrTable != NULL)
		{
			// Make a second call to GetIpAddrTable to get the
			// actual data we want
			if ( (dwRetVal = GetIpAddrTable( pIPAddrTable, &dwSize, 0 )) == NO_ERROR )
			{
				for (int i=0; i < (int) pIPAddrTable->dwNumEntries; i++)
				{
					ipaddr.S_un.S_addr = (u_long) pIPAddrTable->table[i].dwAddr;
					subnet.S_un.S_addr = (u_long) pIPAddrTable->table[i].dwMask;

					if (((pIPAddrTable->table[i].wType & (MIB_IPADDR_DISCONNECTED | MIB_IPADDR_DELETED)) == 0) &&
						(ipaddr.S_un.S_addr != 0x0100007f))
					{
						ipaddr.S_un.S_addr = ipaddr.S_un.S_addr | ~(subnet.S_un.S_addr);

						global_lstNIC.push_back(ipaddr.S_un.S_addr);
					}
				}
			}

			free(pIPAddrTable);
			pIPAddrTable = NULL;
		}
	}

	if (global_lstNIC.size() <= 0)
		global_lstNIC.push_back(INADDR_BROADCAST);

	return (int)(global_lstNIC.size());
}

BOOL CUDPBroadCastSocket::Connect()
{
	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (m_socket == INVALID_SOCKET)
		return FALSE;
	
	int yes = 1;
	setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, (const char*)&yes, sizeof(int));
	int nTimeout = SOCKET_WAITTIME_NORMAL;
	setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&nTimeout, sizeof(nTimeout));

	return TRUE;
}

BOOL CUDPBroadCastSocket::SendPacketBroadCast(BYTE FrameType, LPBYTE lpData, DWORD dwLen, ULONG sendaddr)
{
	SOCKADDR_IN	sender;

	sender.sin_family = AF_INET;
	sender.sin_addr.s_addr = sendaddr;
	sender.sin_port = htons(UDP_PORT_NO);

	int byLen;
	BYTE buffSend[MAX_BUFFER_SIZE] = {0};
	
	//TRACE(_T("SendPacketBroadCast Start"));

	WSASetLastError(0);

	byLen = (dwLen + SENDOFFSET_DATA);	// (Header, Length, Sync, CMD) + dwLen

	if (byLen > MAX_BUFFER_SIZE)
	{
		return FALSE;
	}

	// Make Packet -----------------------------------------------------
	buffSend[SENDOFFSET_HEADER] = PACKET_HEADER;
	buffSend[SENDOFFSET_LENGTH] = (unsigned char)(dwLen + (SENDOFFSET_CMD - SENDOFFSET_LENGTH));	// Sync, CMD
	buffSend[SENDOFFSET_SYNC] = m_nSyncNo;
#ifdef INSERT_AXIS_IN_PACKET
	buffSend[SENDOFFSET_AXIS] = 0;
#endif
	buffSend[SENDOFFSET_CMD] = FrameType;
	if (dwLen > 0)
		memcpy(&(buffSend[SENDOFFSET_DATA]), lpData, dwLen);

	// Send Packet ----------------------------------------------------
	if (sendto(m_socket, (const char*)buffSend, byLen, 0, (SOCKADDR *)&sender, sizeof(sender)) == SOCKET_ERROR)
	{
		//TRACE(_T("SendPacketBroadCast Error"));
		return FALSE;
	}
	//TRACE(_T("SendPacketBroadCast OK"));
	return TRUE;
}

BOOL CUDPBroadCastSocket::RecvPacketFromAny(BYTE FrameType, DWORD dwLen, DWORD dwWaitTime)
{
	int nRead;
	SOCKADDR_IN responsor = {0};
	int nAddrSize = sizeof(responsor);
	BYTE buffRecv[MAX_BUFFER_SIZE] = {0};
	BOOL bAlreadFound;

	do
	{
		// Packet
		nRead = recvfrom(m_socket, (char*)buffRecv, MAX_BUFFER_SIZE, 0, (SOCKADDR *)&responsor, &nAddrSize);

		if (nRead <= 0)
		{
			// Socket error
			//TRACE(_T("Error %d"), WSAGetLastError());
			//break;
			return FALSE;
		}

		// Is it wrong header?
		if (buffRecv[RECVOFFSET_HEADER] != PACKET_HEADER)
			break;
		
		// Is it not enough size? (minimal packet size)
		if ((buffRecv[RECVOFFSET_LENGTH] + RECVOFFSET_SYNC) > nRead)
			break;

		// Is it not to me?
		if ((buffRecv[RECVOFFSET_SYNC] != m_nSyncNo) ||
			(buffRecv[RECVOFFSET_CMD] != FrameType))
			break;
		
		//if (buffRecv[RECVOFFSET_STATUS] != FMM_OK)
		//{
		//	// Communication Status is NOT OK.
		//	break;
		//}

		// Is it not enough size? (frame data size)
		if ((nRead != (dwLen + RECVOFFSET_DATA)) &&
			(((int)dwLen != MAX_BUFFER_SIZE) || (nRead > (int)dwLen)))
		{
			break;
		}

		// everything is FINE.
		//if ((dwLen > 0) && (nRead > RECVOFFSET_DATA))
		//{
		//	memcpy(lpData, &(buffRecv[RECVOFFSET_DATA]), min(nRead, (int)dwLen));
		//}

		// insert
		bAlreadFound = FALSE;
		for (unsigned int index = 0; index < global_lstDeviceAddress.size(); index++)
		{
			if (global_lstDeviceAddress.at(index) == responsor.sin_addr.s_addr)
			{
				bAlreadFound = TRUE;
				break;
			}
		}

		if (bAlreadFound == FALSE)
			global_lstDeviceAddress.push_back(responsor.sin_addr.s_addr);
	}
	while (0);

	return TRUE;
}

int CUDPBroadCastSocket::Find()
{
	std::deque<ULONG>::iterator iter;
	ULONG addrNIC;

	//global_lstDeviceAddress.clear();

	// Open
	if (Connect())
	{
		for (iter = global_lstNIC.begin(); iter != global_lstNIC.end(); iter++)
		{
			addrNIC = *iter;

			m_nSyncNo++;
			if (m_nSyncNo < SYNC_NO_FOR_BROADCAST)
				m_nSyncNo = SYNC_NO_FOR_BROADCAST;

			// Send
			if (!SendPacketBroadCast(0x01, NULL, 0, addrNIC))		// FRAME_GETSLAVEINFO
				continue;

			// Receive
			while (RecvPacketFromAny(0x01, MAX_BUFFER_SIZE, SOCKET_WAITTIME_NORMAL / 2));
		}

		// Close
		Close();
	}

	return (int)(global_lstDeviceAddress.size());
}

ULONG CUDPBroadCastSocket::GetFoundAddress(unsigned int index)
{
	if ((index >= 0) && ((unsigned int)index < global_lstDeviceAddress.size()))
		return global_lstDeviceAddress.at(index);

	return 0;
}