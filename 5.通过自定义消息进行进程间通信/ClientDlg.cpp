
// ClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "5.ͨ���Զ�����Ϣ���н��̼�ͨ��.h"
#include "ClientDlg.h"
#include "ServerDialg.h"
#include "afxdialogex.h"
#define WM_UMSG WM_USER + 1

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ClientDlg �Ի���



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


// ClientDlg ��Ϣ�������

BOOL ClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void ClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR ClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/*
	������Ϣ
*/
void ClientDlg::OnBnClickedButtonSend()
{
	// ��������˶Ի���
	ServerDialg *dialog = new ServerDialg();//ָ��һ����ģ�Ի���ʾ��
	dialog->Create(IDD_DIALOG1, NULL);//����
	dialog->ShowWindow(SW_SHOW);//��ʾ

	HWND hWnd = ::FindWindow(NULL, L"�Զ�����Ϣ�����");
	::ShowWindow(hWnd,SW_SHOW);

	int num1, num2;
	num1 = GetDlgItemInt(IDC_EDIT_A, FALSE, FALSE);
	num2 = GetDlgItemInt(IDC_EDIT_B, FALSE, FALSE);

	::SendMessage(hWnd, WM_UMSG, num1, num2);
}
