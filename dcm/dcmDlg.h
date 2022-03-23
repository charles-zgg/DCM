
// dcmDlg.h: 头文件
//

#pragma once


#include "../DcmFile/DcmFile.h"
#include "CShowDcmDlg.h"


// CDcmDlg 对话框
class CDcmDlg : public CDialogEx
{
// 构造
public:
	CDcmDlg(CWnd* pParent = nullptr);	// 标准构造函数
	~CDcmDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DCM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	CEdit m_Edit_InFilePath;
	CEdit m_Edit_OutFilePath;
	CButton m_button_DCM2BMP;
	CButton m_button_DCM2JPEGP;
	//CDialogEx* m_pShowDlg;
	CShowDcmDlg* m_pShowDlg;

	CDcmFile * m_pDcmFile;


	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonDe();
};
