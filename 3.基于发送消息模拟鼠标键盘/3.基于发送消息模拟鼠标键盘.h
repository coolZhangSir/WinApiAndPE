
// 3.���ڷ�����Ϣģ��������.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CKeyBoardApp: 
// �йش����ʵ�֣������ 3.���ڷ�����Ϣģ��������.cpp
//

class CKeyBoardApp : public CWinApp
{
public:
	CKeyBoardApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CKeyBoardApp theApp;