#pragma once
#include "CStaticSignalDisplayForm.h"

class CStaticSignalDisplayGlobalForm
{
protected:
    CStaticSignalDisplayGlobalForm();

public:
    ~CStaticSignalDisplayGlobalForm();

public:
    static CStaticSignalDisplayGlobalForm* GetInstance();

protected:
    CMap<ULONG, ULONG, CStaticSignalDisplayForm*, CStaticSignalDisplayForm*> m_mapForm;

protected:
    void RemoveAll();

public:
    CStaticSignalDisplayForm* GetForm(const ULONG index);
};
