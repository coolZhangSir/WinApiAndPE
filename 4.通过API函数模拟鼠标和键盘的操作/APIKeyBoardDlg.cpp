
// APIKeyBoardDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "4.ͨ��API����ģ�����ͼ��̵Ĳ���.h"
#include "APIKeyBoardDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// APIKeyBoardDlg �Ի���



APIKeyBoardDlg::APIKeyBoardDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MY4API_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void APIKeyBoardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(APIKeyBoardDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(Button_keyboard, &APIKeyBoardDlg::OnBnClickedkeyboard)
	ON_BN_CLICKED(button_mouse, &APIKeyBoardDlg::OnBnClickedmouse)
END_MESSAGE_MAP()


// APIKeyBoardDlg ��Ϣ�������

BOOL APIKeyBoardDlg::OnInitDialog()
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

void APIKeyBoardDlg::OnPaint()
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
HCURSOR APIKeyBoardDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void APIKeyBoardDlg::OnBnClickedkeyboard()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// �ҵ����ڲ���ȡ����
	findAndFocus();
	Sleep(1000);

	// ģ��F5����
	keybd_event(VK_F5, 0, 0, 0);
	keybd_event(VK_F5, 0, 2, 0);
	Sleep(1000);
	keybd_event(VK_F5, 0, 0, 0);
	keybd_event(VK_F5, 0, 2, 0);
	Sleep(1000);
	keybd_event(VK_F5, 0, 0, 0);
	keybd_event(VK_F5, 0, 2, 0);
}

void APIKeyBoardDlg::findAndFocus() {
	CString caption = NULL;
	GetDlgItemText(Edit_Caption, caption);

	//�ж������Ƿ�Ϊ��
	if ("" == caption) {
		return;
	}

	HWND hWnd = ::FindWindow(NULL, caption.GetBuffer(0));
	// ��ָ���������õ�ǰ̨����ȡ����
	::SetForegroundWindow(hWnd);
}

void APIKeyBoardDlg::OnBnClickedmouse()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	findAndFocus();

	CString caption = NULL;
	GetDlgItemText(Edit_Caption, caption);
	HWND hWnd = ::FindWindow(NULL, caption.GetBuffer(0));

	// �õ���������Ļ������{x,y}
	POINT point = { 0 };
	// ���������������ת��Ϊ��Ļ������,���õ�ָ����������Ļ�е�����λ��
	::ClientToScreen(hWnd, &point);

	/*
		�������λ��,�ú���ͨ����GetCursorPosһ��ʹ��,
		������֮ǰ�Ȼ�ȡ���浱ǰ���λ��,�������֮����Ҫ��������û�ԭ����λ��
		mouse_event����һ����û�иú�������
	*/
	SetCursorPos(point.x + 36, point.y + 100);

	// ģ�ⵥ������Ҽ�,������ᵯ���Ҽ��˵�
	mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
	Sleep(1000);
	mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
	Sleep(1000);

	// �ڵ����˵�����R������ˢ��,R����������Ϊ0x52
	keybd_event(0x52, 0, 0, 0);
}
