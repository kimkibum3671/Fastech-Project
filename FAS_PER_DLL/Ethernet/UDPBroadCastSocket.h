#pragma once
#include "FSocket.h"

class CUDPBroadCastSocket
{
public:
	CUDPBroadCastSocket(void);
	virtual ~CUDPBroadCastSocket(void);
	
	// Socket Connection
private:
	SOCKET		m_socket;
	
protected:
	BOOL		Connect();
	void		Close();

	// Command
private:
	BYTE		m_nSyncNo;

protected:
	BOOL		SendPacketBroadCast(BYTE FrameType, LPBYTE lpData, DWORD dwLen, ULONG sendaddr = INADDR_BROADCAST);
	BOOL		RecvPacketFromAny(BYTE FrameType, DWORD dwLen, DWORD dwWaitTime);

public:
	// Gather Network Interface Address
	int			ReadNICAddress();

	// Scan Device Address
	int			Find();

	// Get Found device's address
	ULONG		GetFoundAddress(unsigned int index);
};

