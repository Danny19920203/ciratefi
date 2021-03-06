
// ciratefiDlg.h : 標頭檔
//

#pragma once
#include "ciratefiAlgorithm.h"


// CciratefiDlg 對話方塊
class CiratefiDlg : public CDialog
{
// 建構
public:
	CiratefiDlg(CWnd* pParent = NULL);	// 標準建構函式

// 對話方塊資料
	enum { IDD = IDD_CIRATEFI_DIALOG };
	Ciratefi::CiratefiData _cirateData;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支援


// 程式碼實作
protected:
	HICON m_hIcon;
	cv::Mat _sourceImage;
	cv::Mat _templateImage;

	// 產生的訊息對應函式
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
