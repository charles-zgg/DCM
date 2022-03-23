#pragma once


// CShowDcmDlg 对话框

class CShowDcmDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CShowDcmDlg)

public:
	CShowDcmDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CShowDcmDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SHOW_DCM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
