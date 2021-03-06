
// ClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "5.通过自定义消息进行进程间通信.h"
#include "ClientDlg.h"
#include "ServerDialg.h"
#include "afxdialogex.h"
#define WM_UMSG WM_USER + 1

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ClientDlg 对话框



ClientDlg::ClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MY5_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void ClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(ClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SEND, &ClientDlg::OnBnClickedButtonSend)
END_MESSAGE_MAP()


// ClientDlg 消息处理程序

BOOL ClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void ClientDlg::OnPaint()
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
HCURSOR ClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/*
	发送消息
*/
void ClientDlg::OnBnClickedButtonSend()
{
	// 创建服务端对话框
	ServerDialg *dialog = new ServerDialg();//指向一个非模对话框示例
	dialog->Create(IDD_DIALOG1, NULL);//创建
	dialog->ShowWindow(SW_SHOW);//显示

	HWND hWnd = ::FindWindow(NULL, L"自定义消息服务端");
	::ShowWindow(hWnd,SW_SHOW);

	int num1, num2;
	num1 = GetDlgItemInt(IDC_EDIT_A, FALSE, FALSE);
	num2 = GetDlgItemInt(IDC_EDIT_B, FALSE, FALSE);

	::SendMessage(hWnd, WM_UMSG, num1, num2);
}
