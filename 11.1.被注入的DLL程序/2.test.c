#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>

//BOOL WINAPI DllMain(
//	_In_ HINSTANCE hinstDLL, // ָ������ľ��
//	_In_ DWORD fdwReason, // ����ԭ��
//	_In_ LPVOID lpvReserved // ��ʽ���غ���ʽ����
//){
//	switch (fdwReason)
//	{
//	case DLL_PROCESS_ATTACH:
//		MessageBox(NULL, L"ע��ɹ�",L"TIPS",MB_OK);
//		MessageBox(NULL, L"DLL�����̼���", L"TIPS", MB_OK);
//		break;
//	case DLL_PROCESS_DETACH:
//		MessageBox(NULL, L"DLL������ж��", L"TIPS", MB_OK);
//		break;
//	case DLL_THREAD_ATTACH:
//		MessageBox(NULL, L"DLL�����صĽ��������̴߳���", L"TIPS", MB_OK);
//		break;
//	case DLL_THREAD_DETACH:
//		MessageBox(NULL, L"DLL�����صĽ������߳̽���", L"TIPS", MB_OK);
//		break;
//	}
//	return TRUE;
//}

__declspec(dllexport) VOID MsgBox(char *szMsg);
void virusCode();

BOOL WINAPI DllMain(
	HINSTANCE hInstanceDLL, // ��ǰDLLģ��ľ��,������̬���ӿ��ʵ�����
	DWORD reasonForCall,
	LPVOID lpReserved
) {
	switch (reasonForCall)
	{
	case DLL_PROCESS_ATTACH:
		MsgBox("DLL�����̼���");
		/*
		��DLL������ʱ,���ǿ������������ָ�������Դﵽ������Ҫ��Ч��
		*/
		virusCode();
		break;
	case DLL_PROCESS_DETACH:
		MsgBox("DLL������ж��");
		break;
	case DLL_THREAD_ATTACH:
		MsgBox("DLL�����صĽ��������̴߳���");
		break;
	case DLL_THREAD_DETACH:
		MsgBox("DLL�����صĽ������߳̽���");
		break;
	}
	return TRUE;
}

void MsgBox(char *szMsg) {
	char szModuleName[MAXBYTE] = { 0 };
	GetModuleFileName(NULL, szModuleName, MAXBYTE);
	MessageBox(NULL, szMsg, szModuleName, MB_OK);
}

void virusCode() {
	char szModuleName[MAXBYTE] = { 0 };
	GetModuleFileName(NULL, szModuleName, MAXBYTE);
	MessageBox(NULL, "����������һ�ζ������", szModuleName, MB_OK);
}
