#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>

void terminateProcess();

void main() {

	terminateProcess();

	system("pause");
}

/*
	ͨ��Ѱ�Ҵ��ھ�����رս���
*/
void terminateProcess() {
	HWND hNotepadWnd = FindWindowA(NULL, "�ޱ��� - ���±�");
	if (hNotepadWnd == NULL)
	{
		printf("����Ѱ��ʧ��\n");
		return;
	}

	// ���ݴ��ھ����ȡ����ID
	DWORD dwNotepadPid = 0;
	GetWindowThreadProcessId(hNotepadWnd, &dwNotepadPid);
	if (dwNotepadPid == 0)
	{
		printf("��ȡ����IDʧ��\n");
		return;
	}
	else
	{
		printf("PID:%ld\n", dwNotepadPid);
	}
	
	// ���ݽ���ID�õ�����
	HANDLE hNotepadHandle = OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwNotepadPid);
	if (hNotepadHandle == NULL)
	{
		printf("��ȡ���̾��ʧ��\n");
		return;
	}

	// ���ݾ����������
	BOOL bResult = TerminateProcess(hNotepadHandle, 0);
	if (bResult == TRUE)
	{
		printf("�������̳ɹ�\n");
	}

	CloseHandle(hNotepadHandle);
}