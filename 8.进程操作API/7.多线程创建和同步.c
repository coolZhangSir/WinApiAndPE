#define  _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>

/*
	�̴߳�������,��������:
		HANDLE thread = CreateThread(
			NULL, // ��ȫѡ��,ͨ��ΪNULL
			NULL, // ���̵߳�ջ��С,ͨ��ΪNULL,�����̴߳�С��ͬ
			threadProc, // �߳�ִ�к����ĵ�ַ
			(void*)&a, // �̺߳�������
			0, // ������ѡ��,����CREATE_SUSPEND����֮���ȹ���,�ȴ�����
			&threadId //�����´������߳�ID
		);
	ͬ��������������:
		�ȴ�һ���̵߳ĺ���:
		WaitForSingleObject(
			HANDLE hHandle,	// ��Ҫ�ȴ��Ķ�����
			DWORD dwMillliseconds // ָ���ȴ���ʱ�ĺ�����,�����Ϊ0,�����������,���ΪINFINITE��ʾһֵ�ȴ��̺߳����ķ���
		);
		�ȴ�һ���̵߳ĺ���:
		WaitForMultipleObjects(
			DWORD nCount,
			CONST HANDLE *lpHandles, // ��Ҫ�ȴ��Ķ���������
			BOOL fWaitAll, // ��ʾ�Ƿ�ȴ�ȫ���̵߳�״̬���,�������ΪTRUE,��ȴ�ȫ��.
			DWORD dwMillliseconds  // �ò���ָ���ȴ���ʱ�ĺ�����,�����Ϊ0,�����������,���ΪINFINITE��ʾһֵ�ȴ��̺߳����ķ���
		);
*/

DWORD WINAPI threadProc(LPVOID lpParameter) {
	int *a = (int *)lpParameter;
	printf("thread parameter is %d\n", *a);
}
void main() {
	int a = 2;
	DWORD threadId;
	HANDLE thread = CreateThread(
		NULL, // ��ȫѡ��,ͨ��ΪNULL
		NULL, // ���̵߳�ջ��С,ͨ��ΪNULL,�����̴߳�С��ͬ
		threadProc, // �߳�ִ�к����ĵ�ַ
		(void*)&a, // �̺߳�������
		0, //������ѡ��,����CREATE_SUSPEND����֮���ȹ���,�ȴ�����
		&threadId //�����´������߳�ID
	);
	printf("main thread created a thread which id is %d\n", threadId);

	WaitForSingleObject(thread, INFINITE);

	CloseHandle(thread);
	system("pause");
}