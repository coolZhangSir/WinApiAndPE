#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>
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
int myStrcmp(char *source, char *dest);
BOOL isPeFileAndGetPointer();
DWORD RvaToFoa(DWORD rva);
/*
__stdcall��Standard Call����д����C++�ı�׼���÷�ʽ,��Ȼ����΢����ı�׼��
__stdcallͨ������Win32 API��(�ɲ鿴WINAPI�Ķ���)��
�ɱ������߰Ѳ�������ջ���мǣ������Լ����˳�ʱ��ն�ջ������ֵ��EAX�С�
*/
HMODULE(__stdcall *myLoadLibraryA)(char* moduleName);
int(_stdcall *myGetProcAddress)(HMODULE hModule, LPCSTR lpProcName);
int(_stdcall *myMessageBox)(HWND hWnd, char* lpText, char* lpCaption, UINT uType);

// LoadLibraryA��ASCII��,ת����������,��Ҫע���������,������daoL rbiL Ayra
//DWORD strLoadLibrary[] = { 0x64616f4c, 0x7262694c, 0x41797261 , 0};
char *strLoadLibrary = "LoadLibraryA";
char *strGetProcAddress = "GetProcAddress";
char *user32 = "user32.dll";
char *messageBox = "MessageBoxA";
HMODULE userModule;


void main2() {
	// ͨ��PEB��ȡKernel32.dll�Ļ���ַ
	//kernel32.dllģ���ַ �ȼ��� HMODULE h = LoadLibraryA("kernel32.dll");
	DWORD dwPEB;
	__asm
	{
		mov eax, FS:[0x30]//fs:[30h]ָ��PEB
		mov dwPEB, eax
	}
	lpBase = *(PDWORD)(*(PDWORD)(*(PDWORD)(*(PDWORD)(*(PDWORD)(dwPEB + 0xc) + 0x1c))) + 8);

	BOOL bResult = isPeFileAndGetPointer();
	if (bResult == TRUE)
	{
		if (pExportDirectory != NULL)
		{
			printf("Name:%s\n", (DWORD)lpBase + RvaToFoa(pExportDirectory->Name));
			DWORD *functionsAddress = (DWORD)lpBase + RvaToFoa(pExportDirectory->AddressOfFunctions);
			DWORD *functionNamesAddress = (DWORD)lpBase + RvaToFoa(pExportDirectory->AddressOfNames);
			// ����ǿת������DWORD��Ϊ���ֶ�ʵ�ʳ�����˫��,��ʵ������Ҫ��WORD����,��Ϊ����Ҫ������Ϊ�±�,���ʵ������Ҳ��DWORD,���ֻ��Խ��
			WORD *nameOrdinalsAddress = (DWORD)lpBase + RvaToFoa(pExportDirectory->AddressOfNameOrdinals);
			for (int i = 0; i < pExportDirectory->NumberOfNames; i++)
			{
				if (myStrcmp(strGetProcAddress, (char*)((DWORD)lpBase + functionNamesAddress[i])) == 0)
				{
					// ���ݶ�Ӧ������������ֵ��ȡAddressOfFunctions��Ӧ�ĺ�����ַ
					DWORD virtualAddress = functionsAddress[nameOrdinalsAddress[i]];
					// ���ҵ���LoadLibraryA������ַ��ֵ������ָ�벢���е���
					myGetProcAddress = virtualAddress + (DWORD)lpBase;
				}
				if (myStrcmp(strLoadLibrary, (char*)((DWORD)lpBase + functionNamesAddress[i])) == 0)
				{
					printf("�ҵ�����:%s\n", ((DWORD)lpBase + functionNamesAddress[i]));
					// ���ݶ�Ӧ������������ֵ��ȡAddressOfFunctions��Ӧ�ĺ�����ַ
					DWORD virtualAddress = functionsAddress[nameOrdinalsAddress[i]];
					// ���ҵ���LoadLibraryA������ַ��ֵ������ָ�벢���е���
					myLoadLibraryA = virtualAddress + (DWORD)lpBase;
					userModule = myLoadLibraryA(user32);
				}
			}
			// �ڳɹ���ȡGetProcAddress��User32.dll��ַ��,ִ�ж�Ӧ�Ĵ���
			if (userModule != NULL && myGetProcAddress != NULL)
			{
				myMessageBox = myGetProcAddress(userModule, messageBox);
				myMessageBox(NULL, "ͨ��kernel32.dll�������ֶ����ض�Ӧ�⺯��.", "PE��Ⱦ", 0);
			}
		}
	}

	__asm
	{
		//mov eax, 0x010031C9//ԭ��ڵ�
		//jmp eax
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
		return bResult;
	}
	// ����IMAGE_DOS_HEADER�е�e_lfanew��ֵ�õ�PEͷ��λ��
	pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)lpBase + pDosHeader->e_lfanew);
	// �ж��Ƿ�ΪPE\0\0
	if (pNtHeader->Signature != IMAGE_NT_SIGNATURE)
	{
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
			return pSectionHeaders[i].PointerToRawData + rva - pSectionHeaders[i].VirtualAddress;
		}
	}
	return -1;
}

/*
�ֶ�ʵ��strcmp������ÿ⺯��
*/
int myStrcmp(char *source, char *dest) {
	int i = 0;
	while (source[i] != '\0' && dest[i] != '\0')
	{
		// �ַ���ͬ����-1
		if (source[i] != dest[i])
		{
			return -1;
		}
		i++;
	}
	// ���Ȳ�ͬ����-1
	if (source[i] != '\0' || dest[i] != '\0')
	{
		return -1;
	}
	return 0;
}
