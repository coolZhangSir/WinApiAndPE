
// 4.ͨ��API����ģ�����ͼ��̵Ĳ���.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// APIKeyBoardAPP: 
// �йش����ʵ�֣������ 4.ͨ��API����ģ�����ͼ��̵Ĳ���.cpp
//

class APIKeyBoardAPP : public CWinApp
{
public:
	APIKeyBoardAPP();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern APIKeyBoardAPP theApp;