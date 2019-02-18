#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
#include <WinUser.h>
/*
	WinMain�����е�����:
		1. ע��һ��������,�����ô��ڲ���ʾ�����Ĵ���
		2. ��ͣ�Ļ�ȡ�����Լ�����Ϣ���ַ����Լ��Ĵ��ڴ������
		3. ֱ���յ�WM_QUIT��Ϣ���˳���Ϣѭ����������
*/

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL initInstance(HINSTANCE hInstance, int cmdShow);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(
	// ��ǰʵ�����
	HINSTANCE hInstance,
	// ͬһ����Դ�ļ���������һ��ʵ�����,�ò�����Win16ƽ̨�µ�������,Win32���Ѿ�����ʹ��
	HINSTANCE previousInstance,
	// ����������ʱ������Ĳ���
	LPSTR lpCmdLine,
	// ���̵���ʾ��ʽ(���/��С��/���صȷ�ʽ)
	int nCmdShow
) {
	MSG msg;
	BOOL result;

	// ע�ᴰ����
	ATOM atom = MyRegisterClass(hInstance);
	if (0 == atom)
	{
		MessageBox(NULL, L"ע����ʧ��", L"��ʾ", 0);
		return 0;
	}


	// �������ڲ���ʾ
	BOOL flag = initInstance(hInstance, SW_NORMAL);
	if (!flag) {
		return FALSE;
	}

	// ��Ϣѭ��,��ȡ�����Լ�����Ϣ�����зַ�
	while ((result = GetMessage(&msg, NULL, 0, 0)) != 0) {
		if (result == -1) {
			// ȡ����������,�����˳�
			break;
		}
		else
		{
			// ���������Ϣ,����������Ϣת�����ַ���Ϣ
			TranslateMessage(&msg);
			// ����Ϣ�ַ������ڴ��������ȥ
			DispatchMessage(&msg);
		}
	}
}

/*
	ע�ᴰ����ĺ���,��������:
	1. ���WNDCLASSEX�ṹ��
	2. ����RegisterClassEx����ע��
*/
ATOM MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEX windowClass;

	// ���ṹ��Ϊ0
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

	// cbSize�ǽṹ���С
	windowClass.cbSize = sizeof(WNDCLASSEX);
	// ipfnProc�Ǵ��ڴ�����̵�ַ
	windowClass.lpfnWndProc = WindowProc;
	// hInstance��ʵ�����
	windowClass.hInstance = hInstance;
	// lpszClassName�Ǵ���������
	windowClass.lpszClassName = L"firstWindow";
	// style�Ǵ��ڷ��
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	// hbrBackground�Ǵ����౳��ɫ
	windowClass.hbrBackground = (HBRUSH)COLOR_WINDOWFRAME + 1;
	// hCursor�������
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	// hIcon��ͼ����
	windowClass.hIcon = LoadIcon(NULL, IDI_QUESTION);
	// ����
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;

	return RegisterClassEx(&windowClass);
}

/*
	���������ڲ���ʾ
*/
BOOL initInstance(HINSTANCE hInstance, int cmdShow) {
	HWND hWnd = NULL;
	// ��������
	hWnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		L"firstWindow",
		L"MyFirstWindow",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL
	);
	if (NULL == hWnd)
	{
		MessageBox(NULL, L"��������ʧ��", L"��ʾ", 0);
		return FALSE;
	}
	else {
		// ��ʾ����
		ShowWindow(hWnd, cmdShow);
		// ���´���
		UpdateWindow(hWnd);
	}
	return TRUE;
}

/*
	������Ϣ�Ĵ��ڹ���,����������,��������ǰ��ע�ᴰ��ʱ���HWDCLASSEX�ṹ���е�ipfnWndProc�ĳ�Ա������ֵһ��
*/
LRESULT CALLBACK WindowProc (
	HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam
	) {
	PAINTSTRUCT paint;
	HDC hDc;
	RECT rect;

	WCHAR *pszDrawText = L"Hello Window Program";

	switch (uMsg)
	{
	case WM_PAINT:
		hDc = BeginPaint(hWnd, &paint);
		GetClientRect(hWnd, &rect);
		/*
			���ڿ��ַ��󳤶���Ҫʹ��wcs��ͷ�ĺ���
		*/
		DrawText(hDc, pszDrawText, wcslen(pszDrawText), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		EndPaint(hWnd, &paint);
		break;
	case WM_CLOSE:
		if (IDYES == MessageBox(hWnd, L"�Ƿ��˳�����", L"��ʾ", MB_YESNO))
		{
			// ���ٴ���
			DestroyWindow(hWnd);
			// �˳���Ϣѭ��
			PostQuitMessage(0);
		}
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}