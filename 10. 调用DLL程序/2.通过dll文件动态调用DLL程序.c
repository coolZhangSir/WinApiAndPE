#include<Windows.h>
/*
	��̬����: ��̬���ò���������ʱ��ɵ�.��̬���ò����ڿ�ִ���ļ���д��DLL�������Ϣ.
	HMODULE LoadLibrary(LPCTSTR lpFileName)
		lpFileName: ͨ��������DLL�ļ�������DLL�ļ�
		�ú���ִ�гɹ�������·���һ��ģ����
	FARPROC GetProcAddress(HMODULE module, LPCTSTR lpProcName):
		module: ģ����, ͨ��ͨ��LoadLibrary()����GetModuleHandle�������
		lpProcName: ָ��Ҫ��ȡ������ַ�ĺ�������
		�ú���ִ�гɹ�������·���lpProcNameָ��ĺ������ĺ�����ַ
*/

typedef void(*PFUNMSG)(char *);

void main() {
	HMODULE module = LoadLibrary("9.DLL���.dll");
	if (module == NULL)
	{
		MessageBox(NULL, "9.DLL���.dll�ļ�������", "DLL�ļ�����ʧ��", MB_OK);
		return;
	}
	PFUNMSG pFunMsg = (PFUNMSG)GetProcAddress(module, "MsgBox");
	pFunMsg("try to invoke MsgBox method in dll");
	system("pause");
}