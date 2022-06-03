#include "pch.h"
#include "CMultiDocTemplateEx.h"

IMPLEMENT_DYNAMIC(CMultiDocTemplateEx, CMultiDocTemplate)

CMultiDocTemplateEx::CMultiDocTemplateEx(UINT nIDResource, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass)
	: CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
{

}

CMultiDocTemplateEx::~CMultiDocTemplateEx()
{

}

const BOOL CMultiDocTemplateEx::IsMatchedTemplate(CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass)
{
	return m_pDocClass == pDocClass && m_pFrameClass == pFrameClass, m_pViewClass == pViewClass;
}