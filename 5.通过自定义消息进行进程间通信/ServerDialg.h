#pragma once


// ServerDialg �Ի���

class ServerDialg : public CDialogEx
{
	DECLARE_DYNAMIC(ServerDialg)

public:
	ServerDialg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ServerDialg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT RevcMsg(WPARAM wParam, LPARAM lParam);
};
