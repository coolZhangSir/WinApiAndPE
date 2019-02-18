#define  _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
#include<TlHelp32.h>
int getProcessId();
void enumProcess();
void suspendProcess();
void resumeProcess();
void adjustProcessPrivilege();
/*
 1. ����:
	�е�ʱ��,���ò���ĳЩ���̴�����ͣ״̬,����,�������������еĽ���,�����ڲ��ϻ�����,
	��һ���������̷�����һ���������̱�������,��ô�����ٴΰѱ������Ľ�����������.
	���������������еĻ�����Ƶ�ʽϸ�,��˺��Ѱ������������̶�������.
	���,ֻ���������������̶���ͣ��,�ٽ������������Ľ���.
 2. ��ͣ����ԭ��(û���ֳɵ�SuspendProcess()API����������):
	����1: ��ԭ����ǰ��ս��̵�ID�������̵������̣߳�ͨ����Щ�̵߳�ID��þ�����������ǡ�
			���������̶߳��������ˣ���˽���Ҳ�������ˡ�
	����2: ���̵���ͣʵ�������̵߳���ͣ,��Ϊ������һ����Դ��λ,������ռ��CPUʱ������߳�,
			�����Ҫ��������ͣ,����Ҫ�������е������߳�ȫ����ͣ.
 3. API����
	3.1 ��ͣ��ָ��߳����躯��:
		���߳���ͣʹ�õ�API������SuspendThread(),��������:
			DWORD SuspendThread(HANDLE hThread);
		�ú���ֻ��һ������,����Ҫ��ͣ���߳̾��,����̵߳ľ������ʹ��OpenThread()����,��������:
			HANDLE OpenThread(
				DWORD dwDesiredAccess, //����Ȩ��
				BOOL bInheritHandle, // �Ƿ���Լ̳�
				DWORD dwThreadId //�߳�ID
			);
	3.2	ö���߳�ϸ��:
		Ҫ��ͣ�����е�ȫ���߳�,���벻��ö���߳�,ö���̵߳ĺ�����Thread32First()��Thread32Next().
		��ö���߳�ʱ,���ܴ���ָ�������е��߳̿���,�߳̿���ֻ����ϵͳ�������̵߳Ŀ���.��������ͣ�߳�ʱ,�����ö�ٵ�
		���߳̽����ж�,�����Ƿ�����ָ�������е��߳�.����ж�һ���߳������ĸ�������?��Ҫ����THREADENTRY32�ṹ��,
		�ڸýṹ����,th32ThreadID��ʾ��ǰö�ٵ����̵߳��߳�ID,��th32OwnerProcessID���ʾ�߳������Ľ���ID.
		������ö���߳�ʱ,ֻҪ�ж��Ƿ�����ָ���Ľ���,���ɽ�����ͣ����.
	3.3 �ָ��̵߳�API����:
		DWORD ResumeThread(HANDLE hThread);
*/
void main() {
	adjustProcessPrivilege();
	int option = 0;
	while (1)
	{
		printf("ѡ����: 1.ö�ٽ���  2.��ͣ����  3.�ָ�����  4.�˳�\n");
		scanf("%d",&option);
		
		if (option == 4)
		{
			break;
		}

		switch (option)
		{
		case 1:
			enumProcess();
			break;

		case 2:
			suspendProcess();
			break;
		case 3:
			resumeProcess();
			break;
		}
	}

	system("pause");
}

int getProcessId() {
	int id = 0;
	printf("�������ID:\n");
	scanf("%d", &id);
	return id;
}

void suspendProcess() {
	int pid = getProcessId();
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, NULL);
	if (snap == INVALID_HANDLE_VALUE)
	{
		printf("����ϵͳ�߳̿���ʧ��!\n");
		return;
	}
	THREADENTRY32 entry = { 0 };
	entry.dwSize = sizeof(entry);
	
	BOOL result = Thread32First(snap, &entry);
	while (result) {
		// �жϸ��߳��Ƿ�����ָ������
		if (entry.th32OwnerProcessID == pid)
		{
			HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, entry.th32ThreadID);
			SuspendThread(hThread);
			CloseHandle(hThread);
		}
		result = Thread32Next(snap, &entry);
	}
	CloseHandle(snap);
}

void resumeProcess() {
	int pid = getProcessId();
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, NULL);
	if (snap == INVALID_HANDLE_VALUE)
	{
		printf("����ϵͳ�߳̿���ʧ��!\n");
		return;
	}
	THREADENTRY32 entry = { 0 };
	entry.dwSize = sizeof(entry);

	BOOL result = Thread32First(snap, &entry);
	while (result) {
		// �жϸ��߳��Ƿ�����ָ������
		if (entry.th32OwnerProcessID == pid)
		{
			HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, entry.th32ThreadID);
			ResumeThread(hThread);
			CloseHandle(hThread);
		}
		result = Thread32Next(snap, &entry);
	}
	CloseHandle(snap);
}

void enumProcess() {
	HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32 entry = { 0 };
	entry.dwSize = sizeof(entry);
	BOOL result = Process32First(handle, &entry);
	while (result)
	{
		while (result)
		{
			printf("%-6d | %ls\n", entry.th32ProcessID, entry.szExeFile);
			result = Process32Next(handle, &entry);
		}
	}
	CloseHandle(handle);
}

void adjustProcessPrivilege() {
	HANDLE hToken = NULL;
	BOOL result = OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken);
	if (result)
	{
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
		CloseHandle(hToken);
	}
	else
	{
		printf("�޸Ľ������ȼ�ʧ��!\n");
	}
}