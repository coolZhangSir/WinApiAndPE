#include<Windows.h>
#include<stdio.h>
#include<stdlib.h>
void main1() {

	/*
		01003836: ����ʱ��Ϊ1


		0x01002005: ������֮��Ĳ���


		0x0100383B��ַ�����ö�ʱ��
	*/
	// ͨ��Ѱ��ɨ�׶�Ӧ�Ĵ��ھ���ͻ�ȡ�����ID
	HWND hWnd = FindWindowA(NULL, "ɨ��");
	DWORD hPid = 0;
	GetWindowThreadProcessId(hWnd, &hPid);

	// ͨ������ID��ȡ����̾��
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, hPid);
	/*
		0x01005330:ɨ����Ϸ�������ݵĵ�ַ
			�Ӹõ�ַ��ʼ���α�����3��DWORD���͵�����:
			�׵�����: 0x63  ����: 0x1E  ����: 0x10
		0x01005340:ɨ����Ϸ�������ݵĵ�ַ
			�����򱣴���������ķֲ�ͼ
			������Χ��һȦ0x10��ʾǽ
			�������Կɵó���������ֵ�ֱ��ʾ����˼:
				0x10:ǽ
				0x0F:�հ׿�
				0x8F:��
				0x8E:��
	*/
	DWORD dwDataAddress = 0x01005330;
	// �׵�����
	DWORD dwNumberOfMine = 0;
	// ʵ�ʶ�ȡ���ֽ���
	DWORD dwBytesOfRead = 0;
	// ���
	DWORD dwHeight = 0, dwWidth = 0;

	// ��ȡ�׵�����
	ReadProcessMemory(hProcess, (LPCVOID)dwDataAddress, &dwNumberOfMine, sizeof(DWORD), &dwBytesOfRead);
	// ��ȡ��
	ReadProcessMemory(hProcess, (LPCVOID)(dwDataAddress + 4), &dwWidth, sizeof(DWORD), &dwBytesOfRead);
	// ��ȡ��
	ReadProcessMemory(hProcess, (LPCVOID)(dwDataAddress + 8), &dwHeight, sizeof(DWORD), &dwBytesOfRead);
	// ֻ�����Ϸ�еĸ߼��汾,��Ҫ���ж�
	if (dwHeight != 16 || dwWidth != 30)
	{
		printf("��Ϸ�Ǹ߼��Ѷ�,ʧ��!\n");
		CloseHandle(hProcess);
		return;
	}
	// ������ַ
	DWORD dwMineAddress = 0x01005340;
	/* 
		��Ϸ�������� = dwHeight  * dwWidth;
		����ǽ = dwHeight  * 2;
		����ǽ = dwWidth * 2;
		4������
	*/
	DWORD dwByteOfMineArea = dwHeight  * dwWidth + dwHeight * 2 + dwHeight * 2 + 4;
	// ���������ͬ���ֽڴ�С���ڴ���װ�ظ�����
	PBYTE pByte = (PBYTE)malloc(dwByteOfMineArea);

	// ��ȡ��������������
	ReadProcessMemory(hProcess, (LPCVOID)dwMineAddress, pByte, dwByteOfMineArea, &dwBytesOfRead);
	// ��ֵ������
	BYTE bClear = 0x8E;
	int n = dwNumberOfMine;
	for (int i = 0; i < dwByteOfMineArea; i++)
	{
		if (pByte[i] == 0x8F) {
			DWORD addressToWrite = dwMineAddress + i;
			WriteProcessMemory(hProcess
				, (LPVOID)addressToWrite, (LPVOID)&bClear, sizeof(BYTE)
				, &dwBytesOfRead);
			n--;
		}
	}

	// ˢ��ɨ�׵Ŀͻ���
	RECT rectangle;
	// �����������ܵ���˵���ǰѿͻ����Ĵ�Сд���ڶ���������ָ��Rect�ṹ���С�
	GetClientRect(hWnd, &rectangle);
	// �ú�����ָ���Ĵ�������������һ�����Σ�Ȼ�󴰿ڿͻ��������һ���ֽ������»��ơ�
	InvalidateRect(hWnd, &rectangle, TRUE);

	// �ͷŴ���������ݵ��ڴ�ռ�
	free(pByte);

	CloseHandle(hProcess);

	printf("�׵�����99,��ʣ%d����û�㶨.\n", n);

	system("pause");
}