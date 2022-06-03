#pragma once
#include "EthernetInterface.h"

typedef	CEthernetInterface	TargetInterface;
//typedef	CConfigPEInterface	TargetInterface;

#include <map>
typedef std::map<int, TargetInterface*> SOCKET_MAP;
typedef SOCKET_MAP::iterator SOCKET_ITER;

class CEthernetManager  
{
protected:
	CEthernetManager();

public:
	virtual ~CEthernetManager();

	static	CEthernetManager* GetInstance();

// Attributes
protected:
	SOCKET_MAP	m_lstSocket;

// Operations
protected:
	void	Ethernet_Start();
	void	Ethernet_Stop();

public:
	BOOL	Ping(BYTE sb1, BYTE sb2, BYTE sb3, BYTE sb4);

	const INT MakeNewSocket(const BOOL bTCP, const INT iID, const BYTE sb1, const BYTE sb2, const BYTE sb3, const BYTE sb4, const WORD wPortNo );

	const INT RemoveAllSocket();
	const INT RemoveSocket(int iBdID);

	int		GetSocketCount()	{return (int)m_lstSocket.size();}

	TargetInterface*	GetSocketbyIP(const ULONG dwAddress);
	TargetInterface*	GetSocketbyIP(BYTE sb1, BYTE sb2, BYTE sb3, BYTE sb4);
	TargetInterface*	GetSocketbyBdID(int iBdID);
};
