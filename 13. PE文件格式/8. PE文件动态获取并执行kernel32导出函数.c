#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>

/*
	/// ע�͵�����Ϊ�Ż���,Ϊ�˽��ļ���������,����ʹ�ñ���,ֱ��ʹ��ƫ��������һЩ��ַ
	(lpBase + 0x3c): DOSͷ�е�e_lfanew
	*(PDWORD)((DWORD)lpBase + *(PDWORD)((DWORD)lpBase + 0x3c): NTHeader
	*(PDWORD)((DWORD)lpBase + *(PDWORD)((DWORD)lpBase + 0x3c) + 0x78: DataDirectory
	((DWORD)lpBase + *(PWORD)((DWORD)lpBase + *(PDWORD)((DWORD)lpBase + 0x3c) + 0x18)): ((DWORD)&(pNtHeader->OptionalHeader)
*/

/*
	__stdcall��Standard Call����д����C++�ı�׼���÷�ʽ,��Ȼ����΢����ı�׼��
	__stdcallͨ������Win32 API��(�ɲ鿴WINAPI�Ķ���)��
	�ɱ������߰Ѳ�������ջ���мǣ������Լ����˳�ʱ��ն�ջ������ֵ��EAX�С�
*/
int myStrcmp(char *source, char *dest);

void main2() {
	// �Զ��庯��ָ��
	HMODULE(__stdcall *myLoadLibraryA)(char* moduleName) = NULL;
	int(_stdcall *myGetProcAddress)(HMODULE hModule, LPCSTR lpProcName) = NULL;
	int(_stdcall *myMessageBox)(HWND hWnd, char* lpText, char* lpCaption, UINT uType) = NULL;

	// ӳ����ͼָ��
	LPVOID lpBase = NULL;
	// ������
	PIMAGE_EXPORT_DIRECTORY pExportDirectory = NULL;
	// LoadLibraryA��ASCII��,ת����������,��Ҫע���������,������daoL rbiL Ayra
	// DWORD strLoadLibrary[] = { 0x64616f4c, 0x7262694c, 0x41797261 , 0};
	char *strLoadLibrary = "LoadLibraryA";
	char *strGetProcAddress = "GetProcAddress";
	char *user32 = "user32.dll";
	char *messageBox = "MessageBoxA";
	HMODULE userModule = NULL;

	// ͨ��PEB��ȡKernel32.dll�Ļ���ַ
	// kernel32.dllģ���ַ �ȼ��� HMODULE h = LoadLibraryA("kernel32.dll");
	DWORD dwPEB = NULL;
	__asm
	{
		mov eax, FS:[0x30]//fs:[30h]ָ��PEB
		mov dwPEB, eax
	}
	lpBase = *(PDWORD)(*(PDWORD)(*(PDWORD)(*(PDWORD)(*(PDWORD)(dwPEB + 0xc) + 0x1c))) + 8);

	// �ж��Ƿ�ΪMZͷ
	if (*(PWORD)lpBase != IMAGE_DOS_SIGNATURE)
	{
		return ;
	}
	// ����IMAGE_DOS_HEADER�е�e_lfanew��ֵ�õ�PEͷ��λ��
	// �ж��Ƿ�ΪPE\0\0
	if (*(PDWORD)((DWORD)lpBase + *(PDWORD)((DWORD)lpBase + 0x3c)) != IMAGE_NT_SIGNATURE)
	{
		return ;
	}
	// ��ȡ����Ŀ¼���еĵ�����ĵ�ַ: ������ļ���ƫ��,�ټ��ϸ��ļ�ӳ�䵽�ڴ��еĻ�ַ��OK
	DWORD exportRva = (*(PDWORD)((DWORD)lpBase + *(PDWORD)((DWORD)lpBase + 0x3c) + 0x78));

	if (exportRva == 0)
	{
		pExportDirectory = NULL;
	}
	else {
		pExportDirectory = (PIMAGE_EXPORT_DIRECTORY)((DWORD)lpBase + exportRva);
	}

	if (pExportDirectory != NULL)
	{
		/// DWORD *functionsAddress = (DWORD)lpBase + RvaToFoa(pExportDirectory->AddressOfFunctions);
		DWORD *functionsAddress = (DWORD)lpBase + pExportDirectory->AddressOfFunctions;
		/// DWORD *functionNamesAddress = (DWORD)lpBase + RvaToFoa(pExportDirectory->AddressOfNames);
		DWORD *functionNamesAddress = (DWORD)lpBase + pExportDirectory->AddressOfNames;
		// ����ǿת������DWORD��Ϊ���ֶ�ʵ�ʳ�����˫��,��ʵ������Ҫ��WORD����,��Ϊ����Ҫ������Ϊ�±�,���ʵ������Ҳ��DWORD,���ֻ��Խ��
		/// WORD *nameOrdinalsAddress = (DWORD)lpBase + RvaToFoa(pExportDirectory->AddressOfNameOrdinals);
		WORD *nameOrdinalsAddress = (DWORD)lpBase + pExportDirectory->AddressOfNameOrdinals;
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
			myMessageBox(NULL, "ͨ��kernel32.dll��������ز�ִ�п⺯��.", "PE��Ⱦ", 0);
		}
	}

	/*
	�ô���Ӧ�÷��ڲ�����������,����ȥ��̬��ȡ���������ԭ��ڵ�,��������
		__asm
		{
			mov eax, 0x010031C9//ԭ��ڵ�
			jmp eax
		}
	*/
}

/*
	�ֶ�ʵ��strcmp������ÿ⺯��
*/
int myStrcmp(char *source, char *dest) {
	while((*source) && ( *source == *dest ))
    {
        source++;
        dest++;
    }
    if( *(unsigned char*)source != *(unsigned char*)dest )
        return 1;
    else
        return 0;
}
