#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
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
PIMAGE_SECTION_HEADER pSectionHeaders = NULL;
// ������
PIMAGE_EXPORT_DIRECTORY pExportDirectory = NULL;

BOOL isPeFileAndGetPointer();
DWORD RvaToFoa(DWORD rva);
void release();

/*
	ͨ����������Ի�ȡ��غ����ĵ�ַ.��������ͨ������ֵ��λ,Ҳ����ͨ����������λ.ͨ����̲��Һ�����ַ��������ͬ����,�ֱ���:
	1. ���ݱ�Ų��Һ�����ַ
	2. ���ݺ��������Һ�����ַ
*/
/*
	1. ���ݱ�Ų��Һ�����ַ,��������:
		1.1 ��λ��PEͷ
		1.2 ��PE�ļ�ͷ���ҵ�����Ŀ¼��,����ĵ�һ��˫��ֵ�ǵ��������ʼRVA
		1.3 �ӵ������nBase�ֶεõ���ʼ���
		1.4 ������ż�ȥ��ʼ��ŵõ����Ǻ�����AddressOfFunctions�е�������
		1.5 ͨ����ѯAddressOfFunctionsָ��λ�õ�ֵ,�ҵ������ַ
		1.6 �������ַ���ϸö�̬���ӿ��ڱ����뵽��ַ�ռ������ַ,,��Ϊ��������ʵ��ڵ�ַ.
	������ʹ�ñ�Ų��Һ�����ַ,��Ϊ�кܶ�Ķ�̬���ӿ��б�ʶ�ı�����Ӧ�ĺ�������һ��,ͨ�����ֺ�����ַ�����Ǵ����.
*/
/*
	2. ���ݺ��������Һ�����ַ
		2.1 ��λ��PEͷ
		2.2 ��PE�ļ�ͷ���ҵ�����Ŀ¼��,����ĵ�һ��˫��ֵ�ǵ��������ʼRVA
		2.3 �ӵ������л�ȡNumberOfNames�ֶε�ֵ,�Ա㹹��һ��ѭ��,���ݴ�ֵȷ��ѭ���Ĵ���
		2.4 ��AddressOfNames�ֶ�ָ��ĺ����������ݵĵ�һ�ʼ,������ĺ������ֽ���ƥ��;
			���ƥ��ɹ�,���¼��AddressOfNames��ʼ��������.
		2.5 ͨ����������ȥ����AddressOfFunctionsָ��������ַ����λ�õ�ֵ,�ҵ������ַ.
		2.6 ͨ����ѯAddressOfFunctionsָ��������ַ����λ�õ�ֵ,�ҵ������ַ.
		2.7 ��������ַ���ϸö�̬���ӿ��ڱ����뵽��ַ�ռ�����ַ,��Ϊ��������ʵ��ڵ�ַ
*/
void main() {
	/*char filePath[MAX_PATH] = { 0 };
	printf("�����ļ�·��:\n");
	scanf("%s", filePath);*/
	char filePath[MAX_PATH] = "C:\\Users\\ZhangHao\\Desktop\\test.dll";

	hFile = CreateFile(filePath,
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	hMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, 0);
	lpBase = MapViewOfFile(hMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	BOOL bResult = isPeFileAndGetPointer();
	if (bResult == TRUE)
	{
		if (pExportDirectory != NULL)
		{
			printf("Name:%s\n", (DWORD)lpBase + RvaToFoa(pExportDirectory->Name));
			printf("Base:%d\n",pExportDirectory->Base);
			printf("NumberOfFunctions:%d\n", pExportDirectory->NumberOfFunctions);
			printf("NumberOfNames:%d\n", pExportDirectory->NumberOfNames);
			DWORD *functionsAddress = (DWORD)lpBase + RvaToFoa(pExportDirectory->AddressOfFunctions);
			printf("���е������������ַ:\n");
			for (int i = 0; i < pExportDirectory->NumberOfFunctions; i++)
			{
				printf("\t0x%X\n",functionsAddress[i]);
			}
			DWORD *functionNamesAddress = (DWORD)lpBase + RvaToFoa(pExportDirectory->AddressOfNames);
			WORD *nameOrdinalsAddress = (DWORD)lpBase + RvaToFoa(pExportDirectory->AddressOfNameOrdinals);
			for (int i = 0; i < pExportDirectory->NumberOfNames; i++)
			{
				printf("\t������:%s\n", (DWORD)lpBase + RvaToFoa(functionNamesAddress[i]));
				printf("\t����:%d\n", nameOrdinalsAddress[i]);
				// ���ݶ�Ӧ������������ֵ��ȡAddressOfFunctions��Ӧ�ĺ�����ַ
				DWORD virtualAddress = functionsAddress[nameOrdinalsAddress[i]];
				printf("\t��ʵ��ַ:0x%X\n", virtualAddress + (DWORD)lpBase);
				printf("\t--------------------\n");
			}
		}
		else
		{
			printf("û�е�����.\n");
		}
	}

	system("pause");
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
	pSectionHeaders = (PIMAGE_SECTION_HEADER)((DWORD)&(pNtHeader->OptionalHeader)
		+ pNtHeader->FileHeader.SizeOfOptionalHeader);

	// ��ȡ����Ŀ¼���еĵ�����ĵ�ַ: ������ļ���ƫ��,�ټ��ϸ��ļ�ӳ�䵽�ڴ��еĻ�ַ��OK
	DWORD pExportAddress = (DWORD)lpBase + RvaToFoa(pNtHeader->OptionalHeader.DataDirectory[0].VirtualAddress);
	if (pExportAddress == -1)
	{
		pExportDirectory = NULL;
	}
	else {
		pExportDirectory = (PIMAGE_EXPORT_DIRECTORY)(pExportAddress);
	}

	bResult = TRUE;
	return bResult;
}
/*
	��RVAת����FOA
*/
DWORD RvaToFoa(DWORD rva) {
	int numberOfSections = pNtHeader->FileHeader.NumberOfSections;
	for (int i = 0; i < numberOfSections; i++)
	{
		if (rva >= pSectionHeaders[i].VirtualAddress && 
			rva <= pSectionHeaders[i].VirtualAddress + pSectionHeaders[i].Misc.VirtualSize)
		{
			pSectionHeaders[i].Characteristics = 0xE00000A0;
			return pSectionHeaders[i].PointerToRawData + rva - pSectionHeaders[i].VirtualAddress;
		}
	}
	return -1;
}

void release() {
	UnmapViewOfFile(lpBase);
	CloseHandle(hMap);
	CloseHandle(hFile);
}