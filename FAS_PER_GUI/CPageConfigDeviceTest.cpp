// CPageConfigDeviceTest.cpp: 구현 파일
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageConfigDeviceTest.h"


// CPageConfigDeviceTest

IMPLEMENT_DYNAMIC(CPageConfigDeviceTest, CMFCPropertyPage)

CPageConfigDeviceTest::CPageConfigDeviceTest()
	:CMFCPropertyPage(IDP_CONFIG_DEVICE_TEST)
{

}

CPageConfigDeviceTest::~CPageConfigDeviceTest()
{
}


BEGIN_MESSAGE_MAP(CPageConfigDeviceTest, CMFCPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON1, &CPageConfigDeviceTest::OnBnClickedButton1)
END_MESSAGE_MAP()



// CPageConfigDeviceTest 메시지 처리기




void CPageConfigDeviceTest::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
