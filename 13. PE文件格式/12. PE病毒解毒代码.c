#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>
/*
	˼·:
	һ. ��ֲ��PE�ļ��ķ�ʽ,���Ƿ��Ⱦ�ļ�δ֪������½����޸�
		1. �жϵ�ǰPE�ļ����Ƿ���ڱ������,��������ڱ������,ת2.������ڱ������,ת4.
		2. ��ǰPE�ļ��в���һ���������(cqnu)
		3. ��ԭʼ��ڵ㱣�浽Image_Dos_Header�е�e_cblp�ֶ���
		4. �жϵ�ǰ�ĳ�����ڵ���e_cblp�ֶ��Ƿ���ͬ,�����ͬ,�˳�,�������ͬ,ת5
		5. ����PE�ļ��еĹؼ�����--������ڵ�
		6. ���ݲ����޸ĵ���ڵ���ɾ���������ڽ�
		7. �����ڵ�����,SizeOfCode,SizeOfImage�ֶ�
	��. ��ֲ��PE�ļ�,������֪ĳ���ļ��ѱ���Ⱦ�����޸�
		1. ����PE�ļ��еĹؼ�����--������ڵ�
		2. ���ݲ����޸ĵ���ڵ���ɾ���������ڽ�
		3. �����ڵ�����,SizeOfCode,SizeOfImage�ֶ�

*/
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

BOOL isPeFileAndGetPointer();
void release();

void main() {
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
	bResult = TRUE;
	return bResult;
}

void release() {
	UnmapViewOfFile(lpBase);
	CloseHandle(hMap);
	CloseHandle(hFile);
}