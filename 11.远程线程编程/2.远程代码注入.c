#define _CRT_SECURE_NO_WARNINGS

#include<Windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<TlHelp32.h>
/*
	Ŀ��: ��������DLL�ļ�ֱ����Ŀ�����д��Ҫִ�еĴ���,������ض�����.
		Ҫ��Ŀ����������һ���Ĺ���,����Ҫʹ����ص�API����,��ͬ��API����ʵ���ڲ�ͬ��DLL��.
	Kernel32.dll�ļ���ÿ�����̵ĵ�ַ����ͬ��,	������������dll�ļ���ÿ�������еĵ�ַ����һ����.
	����,��Ŀ������е���API����ʱ,����ʹ��LoadLibrary()��GetProcAddress()������̬�������õ���API����.
	����Ҫʹ�õ�API������API�������ڵ�DLL�ļ�����յ�һ���ṹ����,ֱ��д��Ŀ����̵Ŀռ���.
	ֱ�Ӱ�Ҫ��Զ���߳���ִ�еĴ���Ҳд��Ŀ����̵��ڴ�ռ���,	������CreateRemoteThread()�������ɽ�������.
	��:
		1. ��ע��ĳ������(�����¼�����:	APPCRASH)
			���ܵ����1:
				Debug�汾�����ܶ������Ϣ.��ĳЩ������Ϣ���������ڴ�����,����������DLLģ����,
				������ִ�е�������صĴ���ʱ����ʲ����ڵ�DLLģ���еĴ���,�͵����˱���.
			���:
				��������Release��ʽ���б�������.
			���ܵ����2:
				�ڱ����������Ż��������,���ܻᵼ�³���.
			���:
				��Releaseģʽ����Ŀ���Խ����޸�:C/C++->�Ż�
					���Ż�����Ϊ:�ѽ���
					��ѡ��С���ٶ�����Ϊ:����
		2. ��ע��ĳ������(�����¼�����:  BEX)
			����:
				ע������������������Ϊ����ִ�д�����ڴ�ռ�û��дִ������PAGE_READWRITE.
			���: 
				����ִ�д�����ڴ�ռ�Ҫ��ִ������PAGE_EXECUTE_READWRITE
*/
#define STRLEN 50

void adjustPrivilege();
int getPidByEnumProcessName(char *szProcessName);
void injectCode(DWORD dwPid);
DWORD WINAPI remoteThreadProc(LPVOID lpParam);

typedef struct _DATA {
	/*
		���ĸ������ڶ�����kernel32.dll�ĵ�������,������ע��ǰ�ͻ�ȡ�����ַ.
		���������Զ���߳�ͨ���⼸������������������.
	*/
	SIZE_T dwLoadLibrary;
	SIZE_T dwGetProcAddress;
	// ��ȡһ��Ӧ�ó����̬���ӿ��ģ������
	SIZE_T dwGetModuleHandle;
	// ��ȡ��ǰ�����Ѽ���ģ����ļ�������·������ģ������ɵ�ǰ���̼��ء�
	SIZE_T dwGetModuleFileName;

	/*
		user32DLL����"User32.DLL",��ΪMessageBoxA()������User32.dll�ĵ�������.
	*/
	char user32Dll[STRLEN];
	char messageBox[STRLEN];
	/*
		MessageBoxA()�������ַ���
	*/
	char message[STRLEN];
}DATA, *PDATA;

void main() {
	char szProcessName[MAX_PATH] = { 0 };
	DWORD dwPid = 0;

	adjustPrivilege();

	printf("�����������:\n");
	scanf("%s", szProcessName);

	dwPid = getPidByEnumProcessName(szProcessName);

	injectCode(dwPid);

	system("pause");
}

/*
	�ڸö�ע������ж���һ���ṹ�����,�����г�ʼ��.
*/
void injectCode(DWORD dwPid) {
	// �򿪽��̲���ȡ���̾��
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	if (NULL == hProcess)
	{
		DWORD dwErr = GetLastError();
		printf("�򿪽���ʧ��:%ld\n", dwErr);
		return;
	}
	
	// ��ȡKernel32.dll����صĵ�������,����ʼ���ṹ��
	DATA data = { 0 };
	char *moduleName = "kernel32.dll";
	data.dwLoadLibrary = (SIZE_T)GetProcAddress(
		GetModuleHandle(moduleName), "LoadLibraryA");
	data.dwGetProcAddress = (SIZE_T)GetProcAddress(
		GetModuleHandle(moduleName), "GetProcAddress");
	data.dwGetModuleHandle = (SIZE_T)GetProcAddress(
		GetModuleHandle(moduleName), "GetModuleHandleA"); 
	// ����Release���������ɶ�������
	data.dwGetModuleFileName = (SIZE_T)GetProcAddress(
		GetModuleHandle(moduleName), "GetModuleFileNameA");
	
	// ��Ҫʹ�õ�����DLL�͵�������
	strcpy(data.user32Dll, "user32.dll");
	strcpy(data.messageBox, "MessageBoxA");
	// messageBox����ʾ���ַ���
	strcpy(data.message, "ע��Ĵ�����ʾһ���Ի���!");

	// ��Ŀ�����������ռ䱣��API�����Ϣ�Ľṹ��
	LPVOID lpData = VirtualAllocEx(hProcess, NULL, sizeof(DATA), MEM_COMMIT, PAGE_READWRITE);
	if (NULL == lpData)
	{
		DWORD dwErr = GetLastError();
		printf("����ռ�ʧ��:%ld\n", dwErr);
	}
	SIZE_T stWriteNum = 0;
	WriteProcessMemory(hProcess, lpData, &data, sizeof(data), &stWriteNum);

	/* 
		��Ŀ����̿ռ���������ڱ������ĳ���,��СΪ1KB.
		����ִ�д�����ڴ�ռ�Ҫ��ִ������PAGE_EXECUTE_READWRITE
		ע������������������Ϊ����ִ�д�����ڴ�ռ�û��дִ������PAGE_READWRITE.
	*/
	SIZE_T stFunSize = 0x400;
	LPVOID lpCode = VirtualAllocEx(hProcess, NULL, stFunSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (NULL == lpData)
	{
		DWORD dwErr = GetLastError();
		printf("����ռ�ʧ��:%ld\n", dwErr);
	}
	// ����Ҫִ�еĴ���д�뵽Ŀ����̿ռ�
	WriteProcessMemory(hProcess, lpCode, &remoteThreadProc, stFunSize, &stWriteNum);

	// ָ����Զ���߳���Ҫִ�еĺ�����ַΪд��Ĵ����ַ
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)lpCode, lpData, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);
	CloseHandle(hProcess);
}

DWORD WINAPI remoteThreadProc(LPVOID lpParam) {
	PDATA pData = (PDATA)lpParam;

	// ʹ�ú���ָ��,����API����ԭ��
	HMODULE(__stdcall *MyLoadLibrary)(LPCTSTR);
	FARPROC(__stdcall *MyGetProcAddress)(HMODULE,LPCTSTR);
	HMODULE(__stdcall *MyGetModuleHandle)(LPCTSTR);
	DWORD(__stdcall *MyGetModuleFileName)(HMODULE,LPCTSTR,DWORD);
	int(__stdcall *MyMessageBox)(HWND, LPCTSTR, LPCTSTR, UINT);

	// �Ը�������ַ���и�ֵ
	MyLoadLibrary = (HMODULE(__stdcall *)(LPCTSTR))pData->dwLoadLibrary;
	MyGetProcAddress = (FARPROC(__stdcall *)(HMODULE, LPCTSTR))pData->dwGetProcAddress;
	MyGetModuleHandle = (HMODULE(__stdcall *)(LPCTSTR))pData->dwGetModuleHandle;
	MyGetModuleFileName = (DWORD(__stdcall *)(HMODULE, LPCTSTR, DWORD))pData->dwGetModuleFileName;

	// ����User32.dll
	HMODULE hModule = MyLoadLibrary(pData->user32Dll);
	// ���MessageBoxA�ĺ�����ַ
	MyMessageBox = (int(__stdcall *)(HWND, LPCTSTR, LPCTSTR, UINT))
		MyGetProcAddress(hModule, pData->messageBox);

	char szModuleFile[MAX_PATH] = { 0 };
	/*
		��ȡ��ǰ���̶�Ӧ�Ŀ�ִ���ļ�����·����
		hModule: Ӧ�ó����DLLʵ�����,NULL��Ϊ��ȡ��ǰ�����ִ���ļ�·����
	*/
	MyGetModuleFileName(NULL, szModuleFile, MAX_PATH);

	MyMessageBox(NULL, pData->message, szModuleFile, MB_OK);

	return 0;
}

/*
	����ö�ٽ��̵ķ�ʽ���ݽ�������ȡ��Ӧ��Pid
	����ʧ�ܷ���-1
*/
int getPidByEnumProcessName(char *szProcessName) {
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (snap == INVALID_HANDLE_VALUE)
	{
		printf("CreateToolhelp32Snapshot ʧ��\n");
		return -1;
	}
	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(pe32);
	BOOL result = Process32First(snap, &pe32);
	while (result)
	{
		// �жϽ������Ƿ����,����򷵻����Ӧ��pid
		if (strcmp(pe32.szExeFile, szProcessName) == 0)
		{
			return pe32.th32ProcessID;
		}

		result = Process32Next(snap, &pe32);
	}
	CloseHandle(snap);
	return -1;
}
/*
	�����������ȼ�
*/
void adjustPrivilege() {
	HANDLE hKey = NULL;
	BOOL result = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hKey);
	if (result == FALSE)
	{
		DWORD dwErr = GetLastError();
		printf("�򿪽��̷�������ʧ��:%ld\n", dwErr);
		return;
	}
	TOKEN_PRIVILEGES tp;
	// PrivilegeCountָ������Ԫ�صĸ���
	tp.PrivilegeCount = 1;
	LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hKey, FALSE, &tp, sizeof(tp), NULL, NULL);
}