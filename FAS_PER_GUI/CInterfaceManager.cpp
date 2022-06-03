#include "pch.h"
#include "CInterfaceManager.h"

#include "CDataBaseManager.h"

#include "CInterfaceSocket.h"
#include "CInterfaceSerial.h"

CInterfaceManager::CInterfaceManager()
{
	switch (CDataBaseConfig::GetInstance()->GetInterface())
	{
	case CDataBaseConfig::interface_TCP:
	{
		m_pInterface = CInterfaceSocket::GetInstance();
	}	break;

	case CDataBaseConfig::interface_Serial:
	{
		m_pInterface = CInterfaceSerial::GetInstance();
	}	break;

	default:
	{
		m_pInterface = NULL;		
	}	break;
	}

//	ASSERT(m_pInterface);
}

CInterfaceManager::~CInterfaceManager()
{

}

CInterface* CInterfaceManager::GetInterface()
{
	static CInterfaceManager _instance;

	return _instance.m_pInterface;
}
