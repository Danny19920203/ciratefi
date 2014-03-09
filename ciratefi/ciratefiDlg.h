
// ciratefiDlg.h : ���Y��
//

#pragma once
#include "ciratefiAlgorithm.h"


// CciratefiDlg ��ܤ��
class CciratefiDlg : public CDialog
{
// �غc
public:
	CciratefiDlg(CWnd* pParent = NULL);	// �зǫغc�禡

// ��ܤ�����
	enum { IDD = IDD_CIRATEFI_DIALOG };
	Ciratefi::CiratefiData _cirateData;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �䴩


// �{���X��@
protected:
	HICON m_hIcon;
	cv::Mat _sourceImage;
	cv::Mat _templateImage;

	// ���ͪ��T�������禡
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonLoadSource();
	afx_msg void OnBnClickedButtonloadtemplate();
	afx_msg void OnBnClickedButtonMatch();
};
