#define  _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
#include<TlHelp32.h>

void enumDLL();
void enumProcess();
void adjustProcessPrivilege();
/*
	˼·�͹��̸�ö�ٽ��̲��.
		1. �ȴ�������CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid)
		2. �ٱ��������е�ÿһ��ModuleEntry32�ṹ��.
			Module32First
	
 1. ���̵�Ȩ�޲������ܻᵼ��ʹ��CreateToolhelp32Snapshot()��������,openProcess()����
	��smss.exe,winlogon.exe��ϵͳ����ʱ����Զ���߳̽���ע��ʱ����ʧ��.
	���������İ취���ǽ��ý��̵�Ȩ��������SeDebugPrivilege.
	��������(�ο�https://baike.baidu.com/item/OpenProcessToken):
		1. ʹ��OpenProcessToken()�����򿪵�ǰ���̵ķ�������
			BOOL OpenProcessToken(
				__in HANDLE ProcessHandle, //Ҫ�޸ķ���Ȩ�޵Ľ��̾��
				__in DWORD DesiredAccess, //ָ����Ҫ���еĲ�������
				__out PHANDLE TokenHandle //���صķ�������ָ��
			)��
		2. ʹ��LookupPrivilegeValue()����ȡ������Ȩ�޵�LUID(ָlocally unique identifier),��ͬϵͳ��ֵ��ͬ
			BOOL LookupPrivilegevalue(
				LPCTSTR lpSystemName, // system name,����ϵͳ����NULL����
				LPCTSTR lpName, // privilege name
				PLUID lpLuid // locally unique identifier,���Ƿ���LUID��ָ��
			);
		3. ʹ��AdjustTokenPrivilege()���������������Ƶ�Ȩ��
  2. ��������˽��̵����ȼ������޷�ö��ϵͳ���̵�DLL(��CreateToolhelp32Snapshot()������ϵͳ����ʧ��),
	����StackOverflow�ϵĴ�:���ָ���Ľ�����64λ�����ҵ��ó�����32λ���̣���˺�����ʧ�ܣ����һ���������ΪERROR_PARTIAL_COPY��299����
	����취�Ǳ����64λ�ĳ���
*/
void main() {
	adjustProcessPrivilege();
	enumProcess();
	enumDLL();
	system("pause");
}

/*
	�����������ȼ�
*/
void adjustProcessPrivilege() {
	HANDLE hToken = NULL;
	BOOL result = OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken);
	if (result == TRUE)
	{
		TOKEN_PRIVILEGES tp;
		// PrivilegeCountָ������Ԫ�صĸ���
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

void enumDLL() {
	printf("--------�������PID------------\n");
	int pid = 0;
	scanf("%d", &pid);
	printf("DLL����\t\t|  DLL·��\n");
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);

	if (snapshot == INVALID_HANDLE_VALUE)
	{
		printf("����DLL����ʧ��!\n");
		return;
	}

	MODULEENTRY32 entry = { 0 };
	entry.dwSize = sizeof(MODULEENTRY32);
	BOOL result = Module32First(snapshot, &entry);
	while (result)
	{
		printf("%15ls  |  %ls\n", entry.szModule, entry.szExePath);
		result = Module32Next(snapshot, &entry);
	}

	CloseHandle(snapshot);
}

void enumProcess() {
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE)
	{
		printf("�������̿���ʧ��!\n");
		return;
	}
	printf("PID    |  ��������\n");
	PROCESSENTRY32 entry = { 0 };
	entry.dwSize = sizeof(PROCESSENTRY32);
	BOOL result = Process32First(snapshot, &entry);
	while (result)
	{
		printf("%-6d | %ls\n", entry.th32ProcessID, entry.szExeFile);
		result = Process32Next(snapshot, &entry);
	}
	CloseHandle(snapshot);
}