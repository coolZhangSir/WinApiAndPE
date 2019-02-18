#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
#include<TlHelp32.h>

/*
 1. ��Windows����,ö�ٽ���,�̻߳���DLL�ļ�ʱ,����������ͬ��,����ͨ������ָ������ؿ���,��ͨ��ѭ�������������յ�����.
	���Ʋ���������ֻ���ڴ�������ʱ������ͬ,��������ȡ��������ʱ��API������ͬ����.
 2. ��غ�����������Tlhelp32.hͷ�ļ���.
 3.	��������,�̻߳���DLL�ļ����յ�API����:
		HANDLE WINAPI CreateToolhelp32Snapshot(DWORD dwFlags, DWORD th32ProcessID);
		����˵��:
			dwFlags:ָ��Ҫ����ϵͳ���յ�����,����Ҫö�ٵ�����,�ò�������ָ������ֵ:
				TH32CS_SNAPMODULE:  ��ö�ٽ����е�DLLʱָ��
				TH32CS_SNAPPROCESS: ��ö��ϵͳ�еĽ���ʱָ��
				TH32CS_SNAPTHREAD:  ��ö��ϵͳ�е��߳�ʱָ��
			th32ProcessID:�ò�������dwFlags�����Ĳ�ͬ����ͬ.���ö�ٵ���ϵͳ�еĽ��̻�ϵͳ�е��߳�,�ò���ΪNULL;
				���ö�ٵ��ǽ����м��ص�DLLʱ,�ò����ǽ���ID
		����ֵ: �ú�������һ�����յľ��,���ṩ��ö�ٺ���ʹ��.
	����ö�ٺ���:
		Process32First(HANDLE hSnapshot, LPPROCESSENTRY32 lppe):
			����˵��:
				hSnapshot:�ò���ΪCreateTollhelp32Snapshot()�������صľ��.
				lppe:�ò���Ϊָ��PROCESSENTRY32�ṹ���ָ��,�ýṹ��ʹ��֮ǰ��Ҫ�Ա���dwSize���̸�ֵ.
					�ñ���������PROCESSENTRY32�ṹ��Ĵ�С.
		Process32Next(HANDLE hSnapshot, LPPROCESSENTRY32 lppe):
			��Process32Firstʹ����ͬ.
*/
void enumProcess();

void main() {

	enumProcess();

	system("pause");
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
	char buffer[MAXBYTE] = { 0 };
	while (result)
	{
		printf("%-6d | %ls\n",entry.th32ProcessID,entry.szExeFile);
		result = Process32Next(snapshot, &entry);
	}
	CloseHandle(snapshot);
}