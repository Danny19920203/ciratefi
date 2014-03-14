
// ciratefiDlg.cpp : ��@��
//

#include "stdafx.h"
#include "ciratefiApp.h"
#include "ciratefiDlg.h"

using namespace cv;
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// �� App About �ϥ� CAboutDlg ��ܤ��

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ��ܤ�����
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �䴩

// �{���X��@
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CciratefiDlg ��ܤ��




CiratefiDlg::CiratefiDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CiratefiDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CiratefiDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CiratefiDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTONLOADSOURCE, &CiratefiDlg::OnBnClickedButtonLoadSource)
	ON_BN_CLICKED(IDC_BUTTONLOADTEMPLATE, &CiratefiDlg::OnBnClickedButtonloadtemplate)
	ON_BN_CLICKED(IDC_BUTTONMATCH, &CiratefiDlg::OnBnClickedButtonMatch)
END_MESSAGE_MAP()


// CciratefiDlg �T���B�z�`��

BOOL CiratefiDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �N [����...] �\���[�J�t�Υ\���C

	// IDM_ABOUTBOX �����b�t�ΩR�O�d�򤧤��C
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �]�w����ܤ�����ϥܡC�����ε{�����D�������O��ܤ���ɡA
	// �ج[�|�۰ʱq�Ʀ��@�~
	SetIcon(m_hIcon, TRUE);			// �]�w�j�ϥ�
	SetIcon(m_hIcon, FALSE);		// �]�w�p�ϥ�

	// TODO: �b���[�J�B�~����l�]�w

	return TRUE;  // �Ǧ^ TRUE�A���D�z�ﱱ��]�w�J�I
}

void CiratefiDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �p�G�N�̤p�ƫ��s�[�J�z����ܤ���A�z�ݭn�U�C���{���X�A
// �H�Kø�s�ϥܡC���ϥΤ��/�˵��Ҧ��� MFC ���ε{���A
// �ج[�|�۰ʧ������@�~�C

void CiratefiDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ø�s���˸m���e

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N�ϥܸm����Τ�ݯx��
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �yø�ϥ�
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ��ϥΪ̩즲�̤p�Ƶ����ɡA
// �t�ΩI�s�o�ӥ\����o�����ܡC
HCURSOR CiratefiDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CiratefiDlg::OnBnClickedButtonLoadSource()
{
	// TODO: �b���[�J����i���B�z�`���{���X
	CFileDialog loadFile(TRUE, "bmp", "*.bmp", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "bmp|*.bmp|*.*|*.*||", this);

	if(loadFile.DoModal() == IDOK)
	{
		_sourceImage=imread((LPCTSTR)loadFile.GetPathName(), CV_LOAD_IMAGE_GRAYSCALE);
		stringstream resolution;
		resolution<<_sourceImage.cols<<"X"<<_sourceImage.rows;
		((CStatic*)GetDlgItem(IDC_SOURCERESOLUTION))->SetWindowText(resolution.str().c_str());
		CRect rect;
		((CStatic*)GetDlgItem(IDC_PICTURESOURCEIMAGE))->GetWindowRect(rect);
		ScreenToClient(rect);
		RedrawWindow(rect);
		CiratefiApp::ShowMatOnPicture(_sourceImage, this, IDC_PICTURESOURCEIMAGE);
	}
}

void CiratefiDlg::OnBnClickedButtonloadtemplate()
{
	// TODO: �b���[�J����i���B�z�`���{���X
	CFileDialog loadFile(TRUE, "bmp", "*.bmp", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "bmp|*.bmp|*.*|*.*||", this);

	if(loadFile.DoModal() == IDOK)
	{
		_templateImage=imread((LPCTSTR)loadFile.GetPathName(), CV_LOAD_IMAGE_GRAYSCALE);
		stringstream resolution;
		resolution<<_templateImage.cols<<"X"<<_templateImage.rows;
		((CStatic*)GetDlgItem(IDC_TEMPLATERESOLUTION))->SetWindowText(resolution.str().c_str());
		CRect rect;
		((CStatic*)GetDlgItem(IDC_PICTURETEMPLATE))->GetWindowRect(rect);
		ScreenToClient(rect);
		RedrawWindow(rect);
		CiratefiApp::ShowMatOnPicture(_templateImage, this, IDC_PICTURETEMPLATE);
	}
}

void CiratefiDlg::OnBnClickedButtonMatch()
{
	// TODO: �b���[�J����i���B�z�`���{���X
	_templateImage=_cirateData.quadradaimpar(_templateImage);
	stringstream processTime;
	time_t start;
	_cirateData.CountParameter(_templateImage);
	_cirateData.Cissq(_templateImage);
	start=clock();
	_cirateData.Cisssa(_sourceImage);
	processTime<<"Cissa:\t"<<(clock()-start)*1000/CLK_TCK<<"\tms"<<endl;

	start=clock();
	_cirateData.Cifi(_sourceImage);
	processTime<<"Cifi:\t"<<(clock()-start)*1000/CLK_TCK<<"\tms"<<endl;


	_cirateData.Rassq(_templateImage);
	start=clock();
	_cirateData.Rafi(_sourceImage);
	processTime<<"Rafi:\t"<<(clock()-start)*1000/CLK_TCK<<"\tms"<<endl;

	start=clock();
	_cirateData.Tefi(_sourceImage, _templateImage);
	processTime<<"Tefi:\t"<<(clock()-start)*1000/CLK_TCK<<"\tms"<<endl;

	((CStatic*)GetDlgItem(IDC_PROCESSTIME))->SetWindowText(processTime.str().c_str());
	Mat tefiResult=_cirateData.DrawTefiResult(_sourceImage);
	CRect rect;
	((CStatic*)GetDlgItem(IDC_PICTURESOURCEIMAGE))->GetWindowRect(rect);
	ScreenToClient(rect);
	RedrawWindow(rect);
	CiratefiApp::ShowMatOnPicture(tefiResult, this, IDC_PICTURESOURCEIMAGE);
}
