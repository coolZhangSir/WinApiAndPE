#include<Windows.h>
void main3() {
	//__asm
	//{
	//	/*
	//		������õ�ԭ���ǽ���Ҫ������ַ���������ѹ��ջ��,Ȼ�����������Ӧ�ַ�����ջ�еĵ�ַ��Ϊ����.
	//	*/
	//	push word ptr 0
	//	push '4321'
	//	push word ptr 0
	//	push 'dcba'
	//	mov eax, esp
	//	// ��ʼ�������,�Ի�������
	//	push 0
	//	// Title
	//	push eax
	//	// ��Ϊ������ַ�����ջ��ռ��6���ֽ�(�ַ���4 + ������'0'2)
	//	add eax, 6
	//	push eax
	//	// ��һ������
	//	push 0
	//	call MessageBoxA
	//	add esp, 8
	//}


	// 5045cec4 bcfeb8d0 c8be0000
	//char test1[] = "PE�ļ���Ⱦ";
	//// b8c3cec4 bcfed2d1 b1bbb8d0 c8be0000
	//char test2[] = "���ļ��ѱ���Ⱦ";
	//__asm
	//{
	//	push    0
	//	lea     EAX, test1
	//	push    EAX
	//	lea     EAX, test2        // �ֲ�������LEA 
	//	push    EAX
	//	push    0
	//	call    DWORD PTR[MessageBoxA]
	//}
}
/*
	1. ��ȡGetProcAddress��ַ
	2. ��ȡLoadLibrary��ַ
	3. ����User32.dllģ��
	4. ��ȡMessageBoxA��ַ
	5. ִ��MessageBoxA����
*/
void main4() {
	/*
	HMODULE a = LoadLibraryA("user32");
	FARPROC b = GetProcAddress(a,"MessageBoxA");
	MessageBoxA(NULL, "tes1", "tes2", 0);
	*/

	/*
		�ܽ���Գ�����_asm����:
			1. ջ������16λ����
			2. ���ÿ⺯�����ε�ԭ���ǽ���Ҫ������ַ���������ѹ��ջ��,Ȼ�����������Ӧ�ַ�����ջ�еĵ�ַ��Ϊ����.
				�����Ϊʲôÿ�ζ�������ջ��ѹ���ַ���,��ѹ��ESP��ԭ��
			3. Windows�������ε�˳����,���ҵ���ѹջ
	*/
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
				PUSH  DWORD PTR 0XD0B8FEBC;
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
}