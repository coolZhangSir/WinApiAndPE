
// 1.Windows��Ϣ����.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMy1Windows��Ϣ����App: 
// �йش����ʵ�֣������ 1.Windows��Ϣ����.cpp
//

class CMy1Windows��Ϣ����App : public CWinApp
{
public:
	CMy1Windows��Ϣ����App();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMy1Windows��Ϣ����App theApp;