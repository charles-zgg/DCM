// CShowDcmDlg.cpp: 实现文件
//

#include "pch.h"
#include "dcm.h"
#include "CShowDcmDlg.h"
#include "afxdialogex.h"


// CShowDcmDlg 对话框

IMPLEMENT_DYNAMIC(CShowDcmDlg, CDialogEx)

CShowDcmDlg::CShowDcmDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SHOW_DCM, pParent)
{

}

CShowDcmDlg::~CShowDcmDlg()
{
}

void CShowDcmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CShowDcmDlg, CDialogEx)
END_MESSAGE_MAP()


// CShowDcmDlg 消息处理程序
