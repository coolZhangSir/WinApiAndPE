#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
void main() {
	/*
	FindFirstFile��һ����������
		1. c:\\Windows\\System32\\*.dll
			��c:\Windows\System32Ŀ¼�в�������dll�ļ�
		2. *.* 
			�ڵ�ǰĿ¼���������ļ�
	*/
	char *szFileName = NULL;
	LPCTSTR lpFileName = "*.exe";
	WIN32_FIND_DATA pNextInfo;
	HANDLE hSearch = FindFirstFile(lpFileName, &pNextInfo);
	if (hSearch == INVALID_HANDLE_VALUE)
	{
		printf("��Ŀ���ļ�!\n");
	}
	else {
		do {
			szFileName = pNextInfo.cFileName;
			printf("�õ��ļ���%s\n", szFileName);
		} while (FindNextFile(hSearch, &pNextInfo));
		FindClose(hSearch);
	}
	system("pause");
}