
// ciratefi.cpp : �w�q���ε{�������O�欰�C
//

#include "stdafx.h"
#include "ciratefiApp.h"
#include "ciratefiDlg.h"

using namespace cv;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CciratefiApp

BEGIN_MESSAGE_MAP(CiratefiApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CciratefiApp �غc

CiratefiApp::CiratefiApp()
{
	// TODO: �b���[�J�غc�{���X�A
	// �N�Ҧ����n����l�]�w�[�J InitInstance ��
}


// �Ȧ����@�� CciratefiApp ����

CiratefiApp theApp;


// CciratefiApp ��l�]�w

BOOL CiratefiApp::InitInstance()
{
	// ���p���ε{����T�M����w�ϥ� ComCtl32.dll 6 (�t) �H�᪩���A
	// �ӱҰʵ�ı�Ƽ˦��A�b Windows XP �W�A�h�ݭn InitCommonControls()�C
	// �_�h����������إ߳��N���ѡC
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// �]�w�n�]�t�Ҧ��z�Q�n�Ω����ε{������
	// �q�α�����O�C
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	// �зǪ�l�]�w
	// �p�G�z���ϥγo�ǥ\��åB�Q���
	// �̫᧹�����i�����ɤj�p�A�z�i�H
	// �q�U�C�{���X�������ݭn����l�Ʊ`���A
	// �ܧ��x�s�]�w�Ȫ��n�����X
	// TODO: �z���ӾA�׭ק惡�r��
	// (�Ҧp�A���q�W�٩β�´�W��)
	SetRegistryKey(_T("���� AppWizard �Ҳ��ͪ����ε{��"));

	CiratefiDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �b����m��ϥ� [�T�w] �Ӱ���ϥι�ܤ����
		// �B�z���{���X
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �b����m��ϥ� [����] �Ӱ���ϥι�ܤ����
		// �B�z���{���X
	}

	// �]���w�g������ܤ���A�Ǧ^ FALSE�A�ҥH�ڭ̷|�������ε{���A
	// �ӫD���ܶ}�l���ε{�����T���C
	return FALSE;
}

void CiratefiApp::ShowMatOnPicture(Mat& image, CDialog* dlg, int pID)
{

	CRect PictureRect;
	CStatic* PictureControl=(CStatic*)dlg->GetDlgItem(pID);
	PictureControl->GetClientRect(&PictureRect);
	CDC *pDc = PictureControl->GetWindowDC();
	SetStretchBltMode(pDc->m_hDC,STRETCH_HALFTONE);

	Mat resizeImage = image.clone();
	if(resizeImage.rows>PictureRect.Height() || resizeImage.cols>PictureRect.Width())
	{
		double resizeRatio = min((double)PictureRect.Width()/(double)image.cols, (double)PictureRect.Height()/(double)image.rows);
		resize(image, resizeImage, Size(), resizeRatio, resizeRatio);
	}

	CImage outputImage;
	int width = resizeImage.cols;
	int height = resizeImage.rows;
	int channels = resizeImage.channels();
	outputImage.Destroy(); //clear
	outputImage.Create(width, height, 8*channels);

	if(channels==1)
	{
		RGBQUAD* ColorTable;
		int MaxColors=outputImage.GetMaxColorTableEntries();
		ColorTable = new RGBQUAD[MaxColors];
		outputImage.GetColorTable(0, MaxColors, ColorTable);
		for (int i = 0; i < MaxColors; i++)
		{
			ColorTable[i].rgbBlue = (BYTE)i;
			ColorTable[i].rgbGreen = (BYTE)i;
			ColorTable[i].rgbRed = (BYTE)i;
		}
		outputImage.SetColorTable(0, MaxColors, ColorTable);
		delete []ColorTable;
	}


	uchar* ps;
	uchar* pimg = (uchar*)outputImage.GetBits(); //A pointer to the bitmap buffer
	//The pitch is the distance, in bytes. represent the beginning of
	// one bitmap line and the beginning of the next bitmap line
	int step = outputImage.GetPitch();
	for (int i = 0; i < height; ++i)
	{
		ps = (resizeImage.ptr<uchar>(i));
		for ( int j = 0; j < width; j++ )
		{
			if ( channels == 1 ) //gray
			{
				*(pimg + i*step + j) = ps[j];
			}
			else if ( channels == 3 ) //color
			{
				for (int k = 0 ; k < 3; k++ )
				{
					*(pimg + i*step + j*3 + k ) = ps[j*3 + k];
				}
			}
		}
	}
	outputImage.Draw(pDc->m_hDC, CRect(CPoint(PictureRect.TopLeft().x+(PictureRect.Width()-width)/2,PictureRect.TopLeft().y+(PictureRect.Height()-height)/2), CSize(width,height)));
	dlg->ReleaseDC(pDc);
}
