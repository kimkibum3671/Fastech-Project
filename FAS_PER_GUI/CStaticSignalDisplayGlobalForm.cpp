#include "pch.h"
#include "CStaticSignalDisplayGlobalForm.h"

CStaticSignalDisplayGlobalForm::CStaticSignalDisplayGlobalForm()
{

}

CStaticSignalDisplayGlobalForm::~CStaticSignalDisplayGlobalForm()
{
	this->RemoveAll();
}

CStaticSignalDisplayGlobalForm* CStaticSignalDisplayGlobalForm::GetInstance()
{
	static CStaticSignalDisplayGlobalForm _instance;

	return &_instance;
}

void CStaticSignalDisplayGlobalForm::RemoveAll()
{
	POSITION pos = m_mapForm.GetStartPosition();
	while (pos)
	{
		ULONG key = 0;
		CStaticSignalDisplayForm* pData = NULL;
		m_mapForm.GetNextAssoc(pos, key, pData);
		delete pData;
	}

	m_mapForm.RemoveAll();
}

CStaticSignalDisplayForm* CStaticSignalDisplayGlobalForm::GetForm(const ULONG index)
{
	CStaticSignalDisplayForm* pData = NULL;
	if (FALSE == m_mapForm.Lookup(index, pData))
	{
		pData = new CStaticSignalDisplayForm;
		m_mapForm.SetAt(index, pData);
	}

	return pData;
}
