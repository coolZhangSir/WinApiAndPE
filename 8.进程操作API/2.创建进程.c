#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#define FILE_PATH "C:\\Windows\\notepad.exe"

void main() {
	// �ýṹ�����ڱ����½��̺����̵߳ľ���ͽ���ID,���̴�����,��������������Ҫ�ر�
	PROCESS_INFORMATION pi = { 0 };
	// �ýṹ���������������״̬,ʹ��ǰ��Ҫ����cb��Ա���и�ֵ,�ó�Ա�������ڱ���ýṹ��Ĵ�С
	STARTUPINFO si = { 0 };
	si.cb = sizeof(STARTUPINFO);

	BOOL bResult = CreateProcessA(FILE_PATH, 
		NULL, NULL, NULL, FALSE,
		NULL, NULL, NULL, &si, &pi);
	if (bResult == FALSE)
	{
		printf("create process error!\n");
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	system("pause");
}