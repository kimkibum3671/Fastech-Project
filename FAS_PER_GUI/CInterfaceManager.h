#pragma once

#include "CInterface.h"

class CInterfaceManager
{
protected:
	CInterfaceManager();

public:
	~CInterfaceManager();

protected:
	CInterface* m_pInterface;

public:
	static CInterface* GetInterface();
};
