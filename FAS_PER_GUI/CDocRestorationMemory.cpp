#include "pch.h"
#include "CDocRestorationMemory.h"

CDocRestorationMemory::CDocRestorationMemory()
{

}

CDocRestorationMemory::~CDocRestorationMemory()
{

}

CDocRestorationMemory* CDocRestorationMemory::GetInstance()
{
	static CDocRestorationMemory _instance;

	return &_instance;
}

void	CDocRestorationMemory::SetSelectedProjectName(const DWORD dwAddress, LPCTSTR lpszName)
{
	m_mapSelProjectName.SetAt(dwAddress, lpszName);
}

const BOOL CDocRestorationMemory::GetSelectedProjectName(const DWORD dwAddress, CString& strName )
{
	return m_mapSelProjectName.Lookup(dwAddress, strName);
}
