
// 5.ͨ���Զ�����Ϣ���н��̼�ͨ��.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// ClientApp: 
// �йش����ʵ�֣������ 5.ͨ���Զ�����Ϣ���н��̼�ͨ��.cpp
//

class ClientApp : public CWinApp
{
public:
	ClientApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern ClientApp theApp;