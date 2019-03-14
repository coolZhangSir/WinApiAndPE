#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
// �ļ����
HANDLE hFile = NULL;
// ӳ����
HANDLE hMap = NULL;
// ӳ����ͼָ��
LPVOID lpBase = NULL;
// DOSͷ
PIMAGE_DOS_HEADER pDosHeader = NULL;
// PEͷ
PIMAGE_NT_HEADERS pNtHeader = NULL;
// �ڱ�ͷ
PIMAGE_SECTION_HEADER pSectionHeader = NULL;

BOOL fileOpen(char *filePath);
BOOL isPeFileAndGetPointer();
void parseBasePe();
void enumSections();
void release();
/*
	��ַ������غ���
*/
int getSectionNumber(int option, DWORD address);
void calcAddress(int sectionIndex, int option, DWORD address);
/*
	�����ڴ����ƫ�Ƶ�ַRVA������ļ�ƫ��FOA
*/
void main() {

	char filePath[MAX_PATH] = { 0 };
	printf("�����ļ�·��:\n");
	scanf("%s", filePath);

	BOOL bResult = fileOpen(filePath);
	if (bResult == TRUE)
	{
		bResult = isPeFileAndGetPointer();
	}
	if (bResult == TRUE)
	{
		parseBasePe();
		enumSections();
	}

	printf("��ַת��:\n");
	int option = 0;
	int knownAddress = 0;

	while (TRUE)
	{
		printf("ѡ����֪�ĵ�ַ����:1. �����ַ 2. ��������ַ  3. �ļ�ƫ��  4.�˳�\n");
		scanf("%d", &option);
		if (option == 4)
		{
			break;
		}
		printf("������֪�ĵ�ֵַ:\n");
		scanf("%X", &knownAddress);
		int sectionIndex = getSectionNumber(option, knownAddress);
		calcAddress(sectionIndex, option, knownAddress);
	}

	if (bResult == TRUE)
	{
		release();
	}

	system("pause");
}
/*
	���ļ�������ӳ����ͼ
	�ú�������Ҫ�����Ǵ��ļ��������ڴ��ļ�ӳ��.ͨ�����ļ�����������дʱֱ��ʹ��ReadFile()��WriteFile()��������.
	�������������ļ�ʱ,ÿ����ReadFile()����WriteFile()���Ҫʹ��SetFilePointer()�������ļ�ָ���λ��,�����Ĳ�����Ϊ����.
	�ڴ��ļ�ӳ��������ǰ������ļ�ӳ������̵�����ռ���,���������ļ���������ڴ�������ڴ�����һ������.
	�����ڴ��ļ�ӳ����ʹ�õ��ĺ���������:�ֱ���CreateFileMapping()��MapViewOfFile()
	https://blog.csdn.net/ktpd_pro/article/details/56481519
*/
BOOL fileOpen(char *filePath) {
	BOOL bResult = FALSE;
	hFile = CreateFile(filePath, GENERIC_WRITE | GENERIC_READ, 
		FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile  == INVALID_HANDLE_VALUE)
	{
		DWORD error = GetLastError();
		printf("���ļ�ʧ��!%d\n", error);
		return bResult;
	}
	hMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE | SEC_IMAGE, 0, 0, 0);
	if (hMap == NULL)
	{
		printf("�����ļ�ӳ��ʧ��!\n");
		CloseHandle(hFile);
		return bResult;
	}
	lpBase = MapViewOfFile(hMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	if (lpBase == NULL)
	{
		printf("ӳ���ļ�ʧ��!\n");
		CloseHandle(hFile);
		CloseHandle(hMap);
		return bResult;
	}
	bResult = TRUE;
	return bResult;
}
/*
	�ж��Ƿ�Ϊ��Ч��PE�ļ�
	�����Чֱ�ӷ���
	�����Ч��ѽ���PE��ʽ��صĽṹ��ָ��Ҳ�õ�
*/
BOOL isPeFileAndGetPointer() {
	BOOL bResult = FALSE;
	// �ж��Ƿ�ΪMZͷ
	pDosHeader = (PIMAGE_DOS_HEADER)lpBase;
	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
		printf("������DOSͷ\n");
		release();
		return bResult;
	}
	// ����IMAGE_DOS_HEADER�е�e_lfanew��ֵ�õ�PEͷ��λ��
	pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)lpBase + pDosHeader->e_lfanew);
	// �ж��Ƿ�ΪPE\0\0
	if (pNtHeader->Signature != IMAGE_NT_SIGNATURE)
	{
		printf("������PEͷ\n");
		release();
		return bResult;
	}
	// ��ȡ�ڱ��λ��,��NT_HEADER�е�&(pNtHeader->OptionalHeader)������SizeOfOptionalHeader���ǽڱ��λ����
	// &(pNtHeader->OptionalHeader):ȡ��OptionalHeader�ڸ��ļ��е�ƫ�Ƶ�ַ
	printf("%08X,%08X\n", &(pNtHeader->OptionalHeader), (pNtHeader->OptionalHeader));
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)&(pNtHeader->OptionalHeader)
		+ pNtHeader->FileHeader.SizeOfOptionalHeader);
	bResult = TRUE;
	return bResult;
}
/*
	����PE�ļ�
*/
void parseBasePe() {
	// ��ڵ�ַ
	printf("��ڵ�ַ:%08X\n", pNtHeader->OptionalHeader.AddressOfEntryPoint);
	// ӳ�����ַ
	printf("ӳ�����ַ:%08X\n", pNtHeader->OptionalHeader.ImageBase);
	// �ڱ�����
	printf("�ڱ�����:%08X\n", pNtHeader->FileHeader.NumberOfSections);
	// �ڴ����
	printf("�ڴ����:%08X\n", pNtHeader->OptionalHeader.SectionAlignment);
	// �ļ�����
	printf("�ļ�����:%08X\n", pNtHeader->OptionalHeader.FileAlignment);
}

/*
	ö�ٽڱ�
*/
void enumSections() {
	int sectionNumber = pNtHeader->FileHeader.NumberOfSections;
	printf("�ڱ����� | V.ƫ�� | V.��С | R.ƫ�� | R.��С | ��־\n");
	for (int i = 0; i < sectionNumber; i++)
	{
		printf("%6s | %08X | %08X | %08X | %08X | %08X\n",
			pSectionHeader[i].Name, 
			pSectionHeader[i].VirtualAddress, pSectionHeader[i].Misc.VirtualSize, 
			pSectionHeader[i].PointerToRawData, pSectionHeader[i].SizeOfRawData,
			pSectionHeader[i].Characteristics);
	}
}
/*
	ͨ�������ĵ�ַ���ͺ͵�ַ,��ȡ�������ڱ���
*/
int getSectionNumber(int option, DWORD address) {
	int sectionNumber = pNtHeader->FileHeader.NumberOfSections;
	switch (option)
	{
	case 1:
	// �����ַ
		{
			int imageBase = pNtHeader->OptionalHeader.ImageBase;
			for (int i = 0; i < sectionNumber; i++)
			{
				if (address >= imageBase + pSectionHeader[i].VirtualAddress 
					&& address <= imageBase + pSectionHeader[i].VirtualAddress 
					+ pSectionHeader[i].Misc.VirtualSize)
				{
					return i;
				}
			}
			break;
		}
	case 2:
		// ��������ַ
		{
			for (int i = 0; i < sectionNumber; i++)
			{
				if (address >= pSectionHeader[i].VirtualAddress
					&& address <= pSectionHeader[i].VirtualAddress
					+ pSectionHeader[i].Misc.VirtualSize)
				{
					return i;
				}
			}
			break;
		}
	case 3:
		// �ļ�ƫ�Ƶ�ַ
		{
			for (int i = 0; i < sectionNumber; i++)
			{
				if (address >= pSectionHeader[i].PointerToRawData
					&& address <= pSectionHeader[i].PointerToRawData + pSectionHeader[i].SizeOfRawData)
				{
					return i;
				}
			}
			break;
		}
	}
	return -1;
}
/*
	���ݴ���������ڽڱ��, ��ַ����, �����ַ��������������
	�����ڴ����ƫ�Ƶ�ַRVA������ļ�ƫ��FOA
*/
void calcAddress(int sectionIndex, int option, DWORD address) {
	int dwRva = 0, dwVa = 0, dwFoa = 0;
	switch (option)
	{
	case 1:
		dwVa = address;
		dwRva = dwVa - pNtHeader->OptionalHeader.ImageBase;
		dwFoa = dwRva - pSectionHeader[sectionIndex].VirtualAddress + pSectionHeader[sectionIndex].PointerToRawData;
		break;
	case 2:
		dwRva = address;
		dwVa = dwRva + pNtHeader->OptionalHeader.ImageBase;
		dwFoa = dwRva - pSectionHeader[sectionIndex].VirtualAddress + pSectionHeader[sectionIndex].PointerToRawData;
		break;
	case 3:
		dwFoa = address;
		dwRva = dwFoa - pSectionHeader[sectionIndex].PointerToRawData + pSectionHeader[sectionIndex].VirtualAddress;
		dwVa = dwRva + pNtHeader->OptionalHeader.ImageBase;
		break;
	}
	printf("RVA:%08X\n", dwRva);
	printf("VA:%08X\n", dwVa);
	printf("FOA:%08X\n", dwFoa);
}
void release() {
	CloseHandle(hFile);
	CloseHandle(hMap);
	UnmapViewOfFile(lpBase);
}