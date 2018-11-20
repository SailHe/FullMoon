#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

#include "../resource.h"
#include "Game.h"
static TCHAR szWindowClass[] = _T("DesktopApp");
static TCHAR szTitle[] = _T("Fullmoon");
HINSTANCE hInst;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


bool isInitialization = false;
std::shared_ptr<Game> game;
int CALLBACK WinMain(
	_In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow){
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	//当窗口水平方向的宽度变化时重绘整个窗口.CS_VREDRAW 当窗口垂直方向的宽度变化时重绘整个窗口.
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));//IDI_APPLICATION
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex)){
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Fullmoon"),
			NULL);

		return 1;
	}
	UINT width = GetSystemMetrics(SM_CXSCREEN);
	UINT height = GetSystemMetrics(SM_CXSCREEN);
	hInst = hInstance;
	HWND hWnd = CreateWindow(szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0,
		width, height,
		//CW_USEDEFAULT, CW_USEDEFAULT,
		//0, 0,
		//1920, 1080,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (!hWnd){
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Fullmoon"),
			NULL);

		return 1;
	}
	//WS_CAPTION 创建一个有标题框的窗口（包括WS_BORDER风格）。 SW_SHOWMAXIMIZED 激活窗口并将其显示为最大化窗口。
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	//std::thread t(mainLoop, hWnd);

	// 消息结构体对象      message：消息
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (msg.message != WM_QUIT){//消息循环（游戏的主循环）
		//从消息队列中获取消息，如果获取到   
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)){
			TranslateMessage(&msg);     //翻译消息  
			DispatchMessage(&msg);      //派发消息（派发给消息处理函数来处理）  
		}
		else{//没有获取到
			static DWORD dwTime = timeGetTime();
			DWORD dwCurrentTime = timeGetTime();
			DWORD dwElapsedTime = dwCurrentTime - dwTime;
			//这段代码的作用为锁帧，60帧，也可以用编译预处理指令控制，并且有玩家决定是否进行锁帧操作
			float fElapsedTime = dwElapsedTime*0.001f;
			if (!game->gameLoop()){
				//销毁窗口
				SendNotifyMessage(hWnd, WM_DESTROY, NULL, NULL);
			}
			//   游戏的主逻辑循环  
			//   游戏的主渲染循环  
			//------------------------------------------  
			if (dwElapsedTime < 1000 / 60)
				Sleep(1000 / 60 - dwElapsedTime);
			dwTime = dwCurrentTime;
		}
	}
	//t.join();

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	/***************************************步骤5***************************************/
	/**
	HWND hWnd = CreateWindow("game","绘制窗口",WS_OVERLAPPEDWINDOW,20,12,450,300,nullptr, nullptr, NU LL,nullptr);
	tagPAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	hdc属于GDI句柄（由gdi32.dll管理），hwnd属于用户句柄(由user32.dll管理)
	hwnd句柄是描述一个窗口的形状、位置、大小、是否显示、它的父窗口、兄弟窗口、等等的一组数据结构；
	hdc句柄是一个实实在在的用于具体表现这个窗口时，需要对这个窗口有个场合来实现的地方。

	case WM_KEYDOWN :	//键盘按下
	case WM_KEYUP :		//键盘松开
	case WM_SYSKEYDOWN:	//系统键按下
	case WM_SYSKEYUP:	//系统键松开
	case WM_CHAR:		//字符消息
	/**/
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message){
	case WM_PAINT:{//要求一个窗口重画自己
					  hdc = BeginPaint(hWnd, &ps);
					  if (!isInitialization){
						  tagRECT winRECT;//LPRECT
						  //GetClientRect 返回指定窗口客户区矩形的大小 
						  //GetWindowRect(hWnd, &winRECT);
						  GetClientRect(hWnd, &winRECT);
						  GP Rect winRect;
						  winRect.X = winRECT.left;
						  winRect.Y = winRECT.top;
						  winRect.Width = winRECT.right - winRECT.left;
						  winRect.Height = winRECT.bottom - winRECT.top;
						  //make_shared<Game>()// unique_ptr<>;
						  //返回hWnd参数所指定的窗口的设备环境。: GetWindowDC(); 
						  //用于获得hWnd参数所指定窗口的客户区域的一个设备环境。: GetDC(hWnd) //重绘时会闪烁
						  //game = shared_ptr<Game>(new Game(GetDC(hWnd), GP Size(winRect.Width, winRect.Height)));
						  game = std::shared_ptr<Game>(new Game(GetDC(hWnd), GP Size(16 * 90, 9 * 90)));
						  isInitialization = true;
					  }
					  else{
						  game->gameLoop();
						  //game->reInitialization(hdc);
					  }
					  EndPaint(hWnd, &ps);
					  break;
	}
	case WM_MOUSEMOVE://				鼠标移动
	case WM_MOUSEWHEEL://				鼠标滚轮拨动
	case WM_LBUTTONDOWN://				左键按下
	case WM_LBUTTONUP://				左键弹起
	case WM_LBUTTONDBLCLK://			左键双击
	case WM_MBUTTONDOWN://				中键按下
	case WM_MBUTTONUP://				中键弹起
	case WM_MBUTTONDBLCLK://			中键双击
	case WM_RBUTTONDOWN://				右键按下
	case WM_RBUTTONUP://				右键弹起
	case WM_RBUTTONDBLCLK://			右键双击
	{//鼠标事件
		MOUSEMSG c;
		tagPOINT m_mouse;
		GetCursorPos(&m_mouse);
		//ClientToScreen(hWnd, &m_mouse);  //客户端位置转化为屏幕位置
		ScreenToClient(hWnd, &m_mouse);  //屏幕转化为客户端
		c.x = (short)m_mouse.x;
		c.y = (short)m_mouse.y;
		c.uMsg = message;
		game->mouseMasege(c);
					  break;
	}case WM_KEYDOWN:{
		switch (wParam){
		case VK_ESCAPE:{
						  HWND   hDesk;
						  RECT   rc;
						  hDesk = GetDesktopWindow();
						  GetWindowRect(hDesk, &rc);
						  SetWindowLong(hWnd, GWL_STYLE, WS_BORDER);
						  SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, rc.right, rc.bottom, SWP_SHOWWINDOW);
		}
			break;
		}
	}
	case WM_CREATE:{//应用程序创建一个窗口 
					  break;
	}case WM_DESTROY:{//一个窗口被销毁
						PostQuitMessage(0);
						break;
	}default:{
				return DefWindowProc(hWnd, message, wParam, lParam);
				break;
	}
	}
	return 0;
}
