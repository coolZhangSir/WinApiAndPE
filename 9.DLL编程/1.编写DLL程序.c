#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>

/*
1. DLL�������ں���
	BOOL WINAPI DllMain(
		HINSTANCE hInstanceDLL, 
		DWORD reasonForCall, 
		LPVOID lpReserved
	) {
	����˵��:
		hInstanceDLL: ��ǰDLLģ��ľ��,������̬���ӿ��ʵ�����
		reasonForCall: DLLMain()���������õ�ԭ��.
			�ò�����ȡֵ��4��,������4�ֵ���DllMain���������,��4��ֵ�ֱ���
			DLL_PROCESS_ATTACH: ��DLL��ĳ���̼���ʱ,DllMain����������
			DLL_PROCESS_DETACH: ��DLL��ĳ����ж��ʱ,DllMain����������
			DLL_THREAD_ATTACH: �����������̱߳�����ʱ,DllMain����������
			DLL_THREAD_DETACH: �����������߳̽���ʱ,DllMain����������
		lpReserved:��������,����������Աʹ�õĲ���
2.	����DllMain()������ֹһ�εı�����,���ݵ��õ������ͬ,��Ҫִ�в�ͬ�Ĵ���,
	���統���̼��ظ�DLL�ļ�ʱ,������DLL��Ҫ����һЩ��Դ;
	����ж�ظ�DLLʱ,����Ҫ�ͷ��������Դ.�����ڱ�дDll����ʱ,��Ҫ���������switch�ṹ�ж������ԭ��.
3. ��ӵ�������
	(extern "C") __declspec(dllexport) VOID MsgBox(char *szMsg);
	extern "C": ��C++������ʹ��,��ʾ��C��ʽ����,��extern "C"���εı����ͺ����ǰ���C���Է�ʽ���б�������ӵ�
	__declspec(dllexport): ����һ����������,���ú����ӱ�DLL�п����ṩ������ģ��ʹ��.
4. ����DLL�к��������ַ���:
	4.1 __declspec(dllexport): �ڴ�����ٵ�ʱ��ȽϷ���
	4.2 .DEF�ļ�,�ڴ���϶������¿��Խ�Ϊ����Ĺ���DLL�еĵ�������

5. �ó�����������������ļ�,�ֱ���.lib��.dll.
	.lib: ���ļ�,���а���һЩ���������������Ϣ.
	.dll: dll�ļ�,���а�����д�Ĵ���
*/

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