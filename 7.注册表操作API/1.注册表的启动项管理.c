#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
#define REG_RUN "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\"

void showRunList(char* masterKeyString, HKEY masterKey, LPCTSTR lpSubKey, int *order);
void enumRunList();
void addRunApplication();
void deleteRunApplication();

/*
1|  RTHDVCPL|"C:\Program Files\Realtek\Audio\HDA\RAVCpl64.exe" -s
2|          |
3|   Sysdiag|"D:\Huorong\Sysdiag\bin\HipsTray.exe"

1|vmware-tray.exe|"D:\�����\���������\vmware-tray.exe"
2|          |
3|SunloginClient|"D:\�������\SunloginClient\SunloginClient.exe" --cmd=autorun
*/
void main() {
	int option = 0;
	while (1)
	{
		printf("1.�鿴������\t2.���������\t3.ɾ��������\t4.�˳�\n");
		scanf("%d",&option);
		switch (option) {
		case 1:
			enumRunList();
			break;
		case 2:
			addRunApplication();
		case 3:
			deleteRunApplication();
			break;
		case 4:
			return;
		}
	}
	system("pause");
}

/*
	ɾ��ָ��·���µ�������:HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Run
*/
void deleteRunApplication() {
	char keyName[100] = { 0 };
	printf("�������:\n");
	scanf("%s", keyName);
	if (strlen(keyName) == 0) {
		printf("������Ч!");
		return;
	}
	// ��ע���
	HKEY hKey = NULL;
	LONG lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_RUN, 0, KEY_WOW64_64KEY | KEY_ALL_ACCESS, &hKey);
	if (lResult != ERROR_SUCCESS)
	{
		// ע����ʧ��
		printf("%s\n", "ע����ʧ��");
		return;
	}

	RegDeleteValue(hKey, keyName);

	RegCloseKey(hKey);
}

/*
	��������·���µ�����������
*/
void enumRunList() {
	int order = 1;
	showRunList("HKEY_LOCAL_MACHINE", HKEY_LOCAL_MACHINE, REG_RUN, &order);
	showRunList("HKEY_CURRENT_USER", HKEY_CURRENT_USER,
		"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\", &order);
	showRunList("HKEY_LOCAL_MACHINE", HKEY_LOCAL_MACHINE, 
		"SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Run", &order);
	order = 1;
}

void showRunList(char* masterKeyString, HKEY masterKey, LPCTSTR lpSubKey, int *order) {
	printf("%s\\%s:\n", masterKeyString, lpSubKey);
	DWORD dwType = 0;
	DWORD dwBufferSize = MAXBYTE;
	DWORD dwKeySize = MAXBYTE;
	char szValueName[MAXBYTE] = { 0 };
	char szValueKey[MAXBYTE] = { 0 };

	HKEY hKey = NULL;
	// ��ע���
	/* ע��:����ʹ��KEY_WOW64_64KEYȨ��ֵ������64λע���
		    ʹ��KEY_WOW32_64KEYȨ��ֵ������32λע���
			����ע���ᱻ�ض���.
			ע���ᱻ�ض���,�ο�:
			https://blog.csdn.net/is2120/article/details/7246334
	*/
	LONG lReturn = RegOpenKeyEx(
		masterKey, lpSubKey, 0, KEY_WOW64_64KEY | KEY_ALL_ACCESS, &hKey);
	if (lReturn != ERROR_SUCCESS)
	{
		return;
	}

	int i = 0;
	while (TRUE) {
		// ö�ټ���
		lReturn = RegEnumValue(hKey,i,
			szValueName,&dwBufferSize,NULL,
			&dwType,(unsigned char*)szValueKey,&dwKeySize);
		// û�����˳�ѭ��
		if (lReturn == ERROR_NO_MORE_ITEMS) {
			break;
		}
		// ��ӡ����ǰ��
		printf("%d|", (*order)++);
		printf("%8s|", szValueName);
		printf("%10s\n", szValueKey);

		memset(szValueName, 0, MAXBYTE);
		memset(szValueKey, 0, MAXBYTE);

		i++;
	}
	printf("\n");
	RegCloseKey(hKey);
}

/*
	��������ָ��·����:HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Run
*/
void addRunApplication() {
	char keyName[100] = { 0 };
	char keyValue[100] = { 0 };
	printf("�������:\n");
	scanf("%s", keyName);
	printf("�����ֵ:\n");
	scanf("%s", keyValue);
	if (strlen(keyName) == 0 || strlen(keyValue) == 0) {
		printf("������Ч!");
		return;
	}
	// ��ע���
	HKEY hKey = NULL;
	LONG lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_RUN, 0, KEY_WOW64_64KEY | KEY_ALL_ACCESS, &hKey);
	if (lResult != ERROR_SUCCESS)
	{
		// ע����ʧ��
		printf("%s\n", "ע����ʧ��");
		return;
	}

	// ���ע�����  sizeof(char):C�����ַ�����\0����,���м�1
	RegSetValueEx(hKey, keyName, 0, REG_SZ, (const unsigned char*)keyValue, strlen(keyValue) + sizeof(char));

	// �ر�ע���
	RegCloseKey(hKey);
}
