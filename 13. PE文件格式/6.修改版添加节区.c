//���ܲ�����
//��ȡLoadLibraryA�ĺ�����ַ������
//
//
//release��
//������δ���õĺ���������    �� (/OPT:NOREF)
//�Ż� �ѽ��� (/Od)
//���ð�ȫ��� (/GS-)
//
//
//
//��Ϊnotepad��.textʣ��ֻ��0x100���ҵĿռ�
//��ֱ�Ӹ���֮ǰ�ĺ������ɵ�ָ��̫�࣬�����0x300����
//���Բ�ֱ�Ӹ���֮ǰ�ĺ��������ǰ���Щ��価���ϲ��������������ָ���СΪ0xe1
//����������������е㲻����⣬
//���Բ����������ֱ��������������ɵ�ָ���
//
//
//�����win7x64,��SysWOW64�µ�notepad
//win7��notepad���ض�λ����Ҫ���ض�λȥ��(IMAGE_NT_HEADERS32.IMAGE_FILE_HEADER.Characteristics | 1)�����У�
//��Ȼ������ص�ʱ����д��������޸��ˣ�������ʧ��
//����ڵ�(IMAGE_NT_HEADERS32.AddressOfEntryPoint.AddressOfEntryPoint)��Ϊ0xb700
//���ļ�ƫ��0xab00(0xb700���ļ�ƫ��) ����ʼд��
//д�����������13-07_getLoadLib.exe
//�ļ�ƫ��0x400��0x4e1
//
//
//��13-08_winResult.dll���ָ�Ϊpa.dll
//�ʹ򲹶����notepad����ͬһĿ¼��Ȼ������notepad
//
//


#include <Windows.h>


int  main1()
{


	DWORD dwPEB;
	DWORD dwDllBase;//��ǰ��ַ
	PIMAGE_EXPORT_DIRECTORY pImageExportDirectory;//ָ�򵼳����ָ��

	TCHAR ** lpAddressOfNames;
	PWORD lpAddressOfNameOrdinals;
	DWORD szStr[] = { 0x64616f4c, 0x7262694c, 0x41797261, //"LoadLibraryA"
		0x00617000 };//"pa"



	__asm
	{
		mov eax, FS:[0x30]//fs:[30h]ָ��PEB
		mov dwPEB, eax
	}


	//kernel32.dllģ���ַ �ȼ��� HMODULE h = LoadLibraryA("kernel32.dll");
	dwDllBase = *(PDWORD)(*(PDWORD)(*(PDWORD)(*(PDWORD)(*(PDWORD)(dwPEB + 0xc) + 0x1c))) + 8);

	//kernel32.dll��ָ�򵼳����ָ��
	/*
		(dwDllBase + 0x3c): DOSͷ�е�e_lfanew
		*(PDWORD)(dwDllBase + *(PDWORD)(dwDllBase + 0x3c): NTHeader
		*(PDWORD)(dwDllBase + *(PDWORD)(dwDllBase + 0x3c) + 0x78: DataDirectory
	*/
	pImageExportDirectory = (PIMAGE_EXPORT_DIRECTORY)(dwDllBase + *(PDWORD)(dwDllBase + *(PDWORD)(dwDllBase + 0x3c) + 0x78));

	//�����ֵ��������б�
	lpAddressOfNames = (TCHAR **)(dwDllBase + pImageExportDirectory->AddressOfNames);

	/*
		����д����ѭ����Ŀ��:���൱���ֶ�ʵ����һ��strcmp����
		ԭ���������:
			��Ϊ������ע�뵽����PE�ļ���,���Բ�������������ַ���������
		i: ���ж���������������ĸ�Ƿ���ͬ
		j: ������ĸ��ͬ�����,������ȶ�ʣ���ַ�.Դ�ַ�����������LoadLibraryA��pa�м����һ��0,�����ַ���������־λ.
		���Դ�ַ���ָ���ƶ�����0�±괦,˵�������ַ�����ȫ��ͬ.
	*/
	int i = 0;
	do
	{
		int j = 0;
		do
		{
			if (((PTCHAR)szStr)[j] != (dwDllBase + lpAddressOfNames[i])[j])
			{
				break;
			}

			if (((PTCHAR)szStr)[j] == 0)
			{
				lpAddressOfNameOrdinals = (PWORD)(dwDllBase +
					pImageExportDirectory->AddressOfNameOrdinals);//�����ֵ������������б�

				((PROC)(dwDllBase + ((PDWORD)(dwDllBase + pImageExportDirectory->AddressOfFunctions))
					[lpAddressOfNameOrdinals[i]]))((PTCHAR)szStr + 0xd);//����LoadLibraryA����pa,0xd������p��ƫ��

				__asm
				{
					leave	//ƽ���ջ
					mov eax, 0x010031C9//ԭ��ڵ�
					jmp eax
				}
			}
		} while (++j);
	} while (++i);

	return 0;
}

