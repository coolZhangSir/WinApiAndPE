// ServerDialg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "5.ͨ���Զ�����Ϣ���н��̼�ͨ��.h"
#include "ServerDialg.h"
#include "afxdialogex.h"
#define WM_UMSG (WM_USER + 1)

// ServerDialg �Ի���

IMPLEMENT_DYNAMIC(ServerDialg, CDialogEx)

ServerDialg::ServerDialg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{
	
}

ServerDialg::~ServerDialg()
{
}

void ServerDialg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ServerDialg, CDialogEx)
	// ������Ϣӳ��
	ON_WM_SYSCOMMAND(ServerDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_UMSG, RevcMsg)
END_MESSAGE_MAP()


/*
	ServerDialg ��Ϣ�������
	��VC6�п��Է���void,��VS�б��뷵��LRESULT����,
	����ON_MESSAGEӳ���лᱨ����ת����Ч����
*/
LRESULT ServerDialg::RevcMsg(WPARAM wParam, LPARAM lParam) {
	int num1, num2, sum;
	num1 = (int)wParam;
	num2 = (int)lParam;
	sum = num1 + num2;
	CString str;
	str.Format(L"%d",sum);
	SetDlgItemText(IDC_EDIT_RESULT, str);
	return NULL;
}
