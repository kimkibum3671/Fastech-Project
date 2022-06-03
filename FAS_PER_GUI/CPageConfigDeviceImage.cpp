// CPageConfigDeviceImage.cpp : implementation file
//

#include "pch.h"
#include "FAS_PER_GUI.h"
#include "CPageConfigDeviceImage.h"
#include "afxdialogex.h"

#include "CSheetConfigDevice.h"

#include "CDataBaseManager.h"

#include "CDataBaseManager.h"

// CPageConfigDeviceImage dialog

IMPLEMENT_DYNAMIC(CPageConfigDeviceImage, CPageConfig)

CPageConfigDeviceImage::CPageConfigDeviceImage(CWnd* pParent /*=nullptr*/)
	: CPageConfig(IDP_CONFIG_DEVICE_IMAGE)
{
	
}

CPageConfigDeviceImage::~CPageConfigDeviceImage()
{
	if (m_bmpImage.GetSafeHandle())
	{
		m_bmpImage.DeleteObject();
	}
}

void CPageConfigDeviceImage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageConfigDeviceImage, CMFCPropertyPage)
	ON_BN_CLICKED(IDC_BTN_DEFAULT, &CPageConfigDeviceImage::OnBnClickedBtnDefault)
	ON_BN_CLICKED(IDC_BTN_LOAD, &CPageConfigDeviceImage::OnBnClickedBtnLoad)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CPageConfigDeviceImage::OnBnClickedBtnSave)
END_MESSAGE_MAP()


// CPageConfigDeviceImage message handlers


BOOL CPageConfigDeviceImage::OnInitDialog()
{
	CMFCPropertyPageEx::OnInitDialog();

	if (m_wndViewer.SubclassDlgItem(IDC_ST_IMAGE, this))
	{
		CSheetConfigDevice* pParent = dynamic_cast<CSheetConfigDevice*>(GetParent());

		ASSERT(pParent);

		m_bmpImage.Attach(::CopyImage(static_cast<HBITMAP>(pParent->GetEditSystemImage(this)), IMAGE_BITMAP, 0, 0, 0));

		m_wndViewer.LoadMainImage(&m_bmpImage);
	}

	return TRUE;
}


void CPageConfigDeviceImage::OnBnClickedBtnDefault()
{
	CDataBaseConfig::GetInstance()->LoadNoImage(&m_bmpImage);

	m_wndViewer.LoadMainImage(&m_bmpImage);
}


void CPageConfigDeviceImage::OnBnClickedBtnLoad()
{
	CString strFilter( _T("Image Files|*.BMP;*.GIF;*.JPG;*.PNG;*.TIF||") );
	CFileDialog dlg(TRUE, NULL, NULL, OFN_EXPLORER, strFilter);

	if (IDOK == dlg.DoModal())
	{		
		CImage image;
		if (S_OK == image.Load(dlg.GetPathName()))
		{
			if (m_bmpImage.GetSafeHandle())
			{
				m_bmpImage.Detach();
			}

			m_bmpImage.Attach(image.Detach());
			
			m_wndViewer.LoadMainImage(&m_bmpImage);
		}
	}
}


void CPageConfigDeviceImage::OnBnClickedBtnSave()
{
	
}


void CPageConfigDeviceImage::OnOK()
{
	CSheetConfigDevice* pParent = dynamic_cast<CSheetConfigDevice*>(GetParent());

	ASSERT(pParent);

	pParent->SetEditSystemImage(m_bmpImage, this);

	/*switch (m_nCallMethod)
	{
	case callParent:
	{
		CDeviceMotionGate* pDevice = dynamic_cast<CSheetConfigDevice*>(GetParent())->GetMotiongateCtrl();

		pDevice->SetEditSystemImage(m_bmpImage);
	}	break;

	case callConfigDB:
	{
		CDataBaseConfig* pConfigDB = CDataBaseConfig::GetInstance();
		pConfigDB->SaveNoImage(m_bmpImage);
	}	break;
	}*/
	
	CPageConfig::OnOK();
}
