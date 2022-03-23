
// dcmDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "dcm.h"
#include "dcmDlg.h"
#include "afxdialogex.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDcmDlg 对话框
#define enum_to_string(x) #x

CDcmDlg::CDcmDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DCM_DIALOG, pParent)
	, m_pDcmFile(nullptr)
	, m_pShowDlg(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CDcmDlg::~CDcmDlg()
{
	if (m_pDcmFile != nullptr)
	{
		delete m_pDcmFile;
		m_pDcmFile = nullptr;
	}
	if (m_pShowDlg != nullptr)
	{
		delete m_pShowDlg;
		m_pShowDlg = nullptr;
	}
}

void CDcmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FILEPATH, m_Edit_InFilePath);
	DDX_Control(pDX, IDC_EDIT_FILEPATH_OUT, m_Edit_OutFilePath);
	DDX_Control(pDX, IDC_RADIO_DCM_2_BMP, m_button_DCM2BMP);
	DDX_Control(pDX, IDC_RADIO_DCM_2_JPEG, m_button_DCM2JPEGP);
}

BEGIN_MESSAGE_MAP(CDcmDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CDcmDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_DE, &CDcmDlg::OnBnClickedButtonDe)
END_MESSAGE_MAP()


// CDcmDlg 消息处理程序

BOOL CDcmDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_button_DCM2BMP.SetCheck(true);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDcmDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDcmDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CDcmDlg::OnBnClickedButtonOpen()
{
	CFileDialog _filedlg(TRUE, "DCM File(*.cmd)|*.dcm", nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "DCM File(*.cmd)|*.dcm| All Files (*.*)|*.*||", this);
	if (_filedlg.DoModal() == IDOK)
	{
		CString _strFilepath = _filedlg.GetPathName();
		m_Edit_InFilePath.SetWindowText(_strFilepath);

		if (m_pDcmFile != nullptr)
		{
			delete m_pDcmFile;
			m_pDcmFile = nullptr;							 
		}																	   

		if (m_pShowDlg == nullptr)
		{
  			m_pShowDlg = new CShowDcmDlg();
			m_pShowDlg->Create(IDD_DIALOG_SHOW_DCM, this);
		}

		m_pDcmFile = new CDcmFile(_strFilepath.GetBuffer(), m_pShowDlg);
	}
}																						 


void CDcmDlg::OnBnClickedButtonDe()
{
	CString _strfile = "BMP File(*.bmp) | *.bmp";
	if (m_button_DCM2BMP.GetCheck() == 1)
	{
		_strfile = "BMP File(*.bmp) | *.bmp";
	}	
	else if (m_button_DCM2JPEGP.GetCheck() == 1)
	{
		_strfile = "JPEG File(*.jpeg) | *.jpeg";
	}

	CFileDialog _filedlg(TRUE, _strfile, nullptr, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _strfile+"| All Files (*.*)|*.*||", this);
	if (_filedlg.DoModal() == IDOK)
	{
		CString _strFilepath = _filedlg.GetPathName();

		if (m_button_DCM2BMP.GetCheck() == 1)
		{
			m_pDcmFile->Save_bmp(_strFilepath.GetBuffer());
		}
		else if (m_button_DCM2JPEGP.GetCheck() == 1)
		{
			m_pDcmFile->Save_Jpeg(_strFilepath.GetBuffer());
		}
		m_Edit_OutFilePath.SetWindowText(_strFilepath);
	}
 }
