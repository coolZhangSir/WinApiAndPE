
// 1.Windows��Ϣ����Dlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "1.Windows��Ϣ����.h"
#include "1.Windows��Ϣ����Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy1Windows��Ϣ����Dlg �Ի���



CMy1Windows��Ϣ����Dlg::CMy1Windows��Ϣ����Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MY1WINDOWS_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMy1Windows��Ϣ����Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMy1Windows��Ϣ����Dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMy1Windows��Ϣ����Dlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMy1Windows��Ϣ����Dlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CMy1Windows��Ϣ����Dlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CMy1Windows��Ϣ����Dlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CMy1Windows��Ϣ����Dlg ��Ϣ�������

BOOL CMy1Windows��Ϣ����Dlg::OnInitDialog()
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

void CMy1Windows��Ϣ����Dlg::OnPaint()
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
HCURSOR CMy1Windows��Ϣ����Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/*
	�򿪼��±�
*/
void CMy1Windows��Ϣ����Dlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	WinExec("notepad.exe",SW_SHOW);
}

/*
	�رռ��±�
*/
void CMy1Windows��Ϣ����Dlg::OnBnClickedButton2()
{
	// ��_T()�궨�����ڴ���LPCTSTRת��Ϊ֧��Unicode���ַ���
	HWND hWnd = ::FindWindow(_T("Notepad"), NULL);
	if (NULL == hWnd) {
		MessageBox(_T("û���ҵ����±�"),_T("��ʾ"),0);
	}
	else
	{
		/*
			ʹ��::��ԭ��:
				::�������ʾʹ�������ȫ��API����,���벻��ð�ŵ���������,�������ṩ��һ������,����ָ�����ĸ������ʹ�øú���
		*/
		::SendMessage(hWnd, WM_CLOSE, NULL, NULL);
	}
}

/*
	�޸ļ��±�����
*/
void CMy1Windows��Ϣ����Dlg::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// HWND hWnd = ::FindWindow(_T("Notepad"), NULL);
	HWND hWnd = ::FindWindow(NULL, _T("�ޱ��� - ���±�"));
	if (NULL == hWnd) {
		MessageBox(_T("û���ҵ����±�"), _T("��ʾ"), 0);
	}
	else
	{
		wchar_t *pCaption = _T("��Ϣ���Ʋ���");
		::SendMessage(hWnd, WM_SETTEXT, (WPARAM)0, (LPARAM)pCaption);
	}
}

/*
	��ȡ����
*/
void CMy1Windows��Ϣ����Dlg::OnBnClickedButton4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	HWND hWnd = ::FindWindow(_T("Notepad"), NULL);
	if (NULL == hWnd) {
		MessageBox(_T("û���ҵ����±�"), _T("��ʾ"), 0);
	}
	else
	{
		char caption[1024] = { 0 };
		::SendMessage(hWnd, WM_GETTEXT, 1024, (LPARAM)caption);
		MessageBox((LPCTSTR)caption,_T("��ʾ"),0);
	}
}
