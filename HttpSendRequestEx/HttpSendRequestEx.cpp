// HttpSendRequestEx.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "HttpSendRequestEx.h"

#define MAX_LOADSTRING 100
#define BUFSIZE         (1024)

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。
	HINTERNET hsession = 0;
	HINTERNET hconnect = 0;
	HINTERNET hrequest = 0;
	char szBuf[1025];
	DWORD dwSize = 0;
	INTERNET_BUFFERS inetbuf;

	hsession = InternetOpen(_T("HttpGet - simple http client"),
		INTERNET_OPEN_TYPE_PRECONFIG,
		NULL, NULL, 0);

	if (hsession == NULL)
	{
		printf("hsession==NULL");
		return -1;
	}

	hconnect = InternetConnect(
		hsession,
		_T("www.tcct.zaq.ne.jp"),
		80,
		_T("iga"),
		_T("password"),
		INTERNET_SERVICE_HTTP,
		0,
		0);
	if (hconnect == NULL)
	{
		printf("hconnect==NULL");
		return -1;
	}

	hrequest = HttpOpenRequest(
		hconnect,
		_T("POST"),
		_T("/iga/tosiki/igapyon/diary/index.html"),
		NULL,
		NULL,
		NULL,
		INTERNET_FLAG_NO_CACHE_WRITE,
		0);

	if (hrequest == NULL)
	{
		printf("hrequest==NULL");
		return -1;
	}

	memset(&inetbuf, 0, sizeof(INTERNET_BUFFERS));
	inetbuf.dwStructSize = sizeof(INTERNET_BUFFERS);
	inetbuf.dwBufferTotal = 5;

	if (HttpSendRequestEx(hrequest, &inetbuf, NULL, 0, 0) == FALSE)
	{
		printf("Error on HttpSendRequestEx %d\n", GetLastError());
		return -1;
	}

	// ここで実際のPOST動作を実行
	strcpy(szBuf, "ABCDE");
	if (InternetWriteFile(hrequest, szBuf, 5, &dwSize) == FALSE)
	{
		printf("Error on InternetWriteFile %d\n", GetLastError());
		return -1;
	}

	if (HttpEndRequest(hrequest, NULL, 0, 0) == FALSE)
	{
		printf("Error on HttpEndRequest %d\n", GetLastError());
		return -1;
	}

	szBuf[0] = 0;

	{
		dwSize = BUFSIZE;
		if (HttpQueryInfo(
			hrequest,
			HTTP_QUERY_CONTENT_LENGTH,

			szBuf, &dwSize, 0))
		{
			szBuf[dwSize] = 0;
			printf("長さ[%s]\n", szBuf);
		}

		dwSize = BUFSIZE;
		if (HttpQueryInfo(
			hrequest,
			HTTP_QUERY_STATUS_CODE,
			szBuf, &dwSize, 0))
		{
			szBuf[dwSize] = 0;
			printf("ステータス[%s]\n", szBuf);
		}

		dwSize = BUFSIZE;
		if (HttpQueryInfo(
			hrequest,
			HTTP_QUERY_STATUS_TEXT,
			szBuf, &dwSize, 0))
		{
			szBuf[dwSize] = 0;
			printf("ステータス文字列[%s]\n", szBuf);
		}
	}


	for (;;)
	{
		dwSize = BUFSIZE;
		if (InternetReadFile(
			hrequest,
			szBuf,
			dwSize,
			&dwSize) == FALSE)
		{
			break;
		}

		if (dwSize <= 0)
		{
			break;
		}

		szBuf[dwSize] = 0;
		printf("%s\n", szBuf);
	}

	InternetCloseHandle(hrequest);
	InternetCloseHandle(hconnect);
	InternetCloseHandle(hsession);

    // TODO: 在此放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_HTTPSENDREQUESTEX, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HTTPSENDREQUESTEX));

    MSG msg;

    // 主消息循环: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HTTPSENDREQUESTEX));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_HTTPSENDREQUESTEX);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择: 
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
