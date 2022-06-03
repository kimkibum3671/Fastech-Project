#pragma once
#include <afxwin.h>

class CMultiDocTemplateEx : public CMultiDocTemplate
{
public:
    DECLARE_DYNAMIC(CMultiDocTemplateEx)

    // Constructors
public:
    CMultiDocTemplateEx(UINT nIDResource, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);
    virtual ~CMultiDocTemplateEx();

public:
    const BOOL IsMatchedTemplate(CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);
};

