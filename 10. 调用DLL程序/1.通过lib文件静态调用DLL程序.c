#include<Windows.h>
/*
	��̬����:ͨ����������DLL�ĵ�������д����ִ���ļ�.
	Ĭ�������,��Ҫ��.lib�ļ���������ĿĿ¼��,�����ڱ���ʱ������޷���.lib�ļ��Ĵ���.
	.dll������Ҫ���ڿ�ִ���ļ�Ŀ¼��,����������ʱ����ֶ�ʧdll�ļ��Ĵ���.
*/
/*
	������������Ҫ��9.DLL���.lib�ļ����ҵ�DLL�е�����������Ϣ
*/
#pragma comment (lib, "9.DLL���.lib")
void MsgBox(char *szMsg);

void main() {
	MsgBox("try to invoke MsgBox method in dll");
	system("pause");
}