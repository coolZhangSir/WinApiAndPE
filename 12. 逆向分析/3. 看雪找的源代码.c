#include<Windows.h>
#include<stdio.h>
#include<stdlib.h>
// �ο�����:
// https://bbs.pediy.com/thread-144908.htm
void Demining(int Index);

void main() {
	// 0�ǰ�������ͨ���޸��ڴ�ķ�ʽ��ǳ���
	//Demining(0);
	// 1��ģ�������ȥ�������ױ�ʶ����
	Demining(1);
}
void Demining(int Index)
{
	DWORD addr = 0x1005340;
	DWORD x_addr = 0x10056A8;
	DWORD y_addr = 0x10056AC;
	DWORD lei_addr = 0x1005194;
	char X, Y, num;
	unsigned char  old_byte, new_byte;
	DWORD index_x, index_y;

	HWND hwnd = FindWindow(NULL, "ɨ��");
	DWORD hProcessId;

	GetWindowThreadProcessId(hwnd, &hProcessId);
	HANDLE Process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, hProcessId);
	if (Process == NULL)
	{
		MessageBox(NULL, "ɨ��û������!", "����", MB_OK);
		return;
	}

	ReadProcessMemory(Process, (LPCVOID)x_addr, &X, 1, NULL);    //��ȡ���򷽸񳤶�
	ReadProcessMemory(Process, (LPCVOID)y_addr, &Y, 1, NULL);    //��ȡ���򷽸񳤶�
	ReadProcessMemory(Process, (LPCVOID)lei_addr, &num, 1, NULL);

	for (index_x = 1; index_x <= X; index_x++)
	{
		for (index_y = 1; index_y <= Y; index_y++)
		{
			if (Index == 0)
			{
				ReadProcessMemory(Process, (LPCVOID)(addr + (index_x << 5) + index_y), &old_byte, 1, NULL);
				if (old_byte == 0x0e || old_byte == 0x0d)
				{
					new_byte = 0x0f;
					if (old_byte == 0x0e)
					{
						num++;
						WriteProcessMemory(Process, (LPVOID)lei_addr, &num, 1, NULL);
					}
				}
				else if (old_byte == 0x8f || old_byte == 0x8d)
				{
					new_byte = 0x8e;
					num--;
					WriteProcessMemory(Process, (LPVOID)lei_addr, &num, 1, NULL);
				}
				else
				{
					new_byte = old_byte;
				}
				WriteProcessMemory(Process, (LPVOID)(addr + (index_x << 5) + index_y), &new_byte, 1, NULL);
			}
			if (Index == 1)
			{
				/* 
					addr + (index_x << 5):Խ������32���ֽڵ�ǽ
					(addr + (index_x << 5) + index_y):����ÿһ��
				*/
				ReadProcessMemory(Process, (LPCVOID)(addr + (index_x << 5) + index_y), &old_byte, 1, NULL);
				// �жϷ�8E(��)�ͷ�8F(��),Ȼ��ģ�����㿪����
				if (!(old_byte & 0x80))
				{
					/* 
					ģ������¼����������,ͨ�������ó�
					;  lParam,��16λX���꣬��16λY����
					0100209C  |.  C1E8 10       SHR EAX,10                               ;  ����16λ��ȡX����
					0100209F  |.  83E8 27       SUB EAX,27                               ;  X�����ȥ0x27
					010020A2  |.  C1F8 04       SAR EAX,4                                ;  ��������4λ
					010020A5  |.  50            PUSH EAX                                 ; /Arg2
					010020A6  |.  0FB745 14     MOVZX EAX,WORD PTR SS:[EBP+14]           ; |
					010020AA  |.  83C0 04       ADD EAX,4                                ; |Y�����0x04
					010020AD  |.  C1F8 04       SAR EAX,4                                ; |��������4λ
					[ebp+14]������lParam��ֵ����WM_LBUTTONDOWN�У�lParam�����˰������ʱ������λ�á�
					������һЩ�����ǽ��û����������ת�����������ӵ�����:
					������������X = (�û����ͼ������X - 0x27) >> 4
					������������Y = (�û����ͼ������Y + 0x04) >> 4
					���������ǿ��Եó����½���:
					�������ӵĶ�����X���������������X����ľ���Ϊ0x27;
					�������ӵĶ�����Y���������������X����ľ���Ϊ0x04;
					�������ӵ�ͼ�ν���Ϊ0x04 * 0x04��
					*/
					LPARAM lParam = (((index_x << 4) + 0x27) << 0x10) + (index_y << 4) - 4;
					SendMessage(hwnd, (UINT)WM_LBUTTONDOWN, 0, lParam);
					Sleep(1000);
					SendMessage(hwnd, (UINT)WM_LBUTTONUP, 0, lParam);
				}
			}
		}
	}
	InvalidateRect(hwnd, NULL, TRUE);
	CloseHandle(Process);
}