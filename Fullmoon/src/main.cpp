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
	//������ˮƽ����Ŀ�ȱ仯ʱ�ػ���������.CS_VREDRAW �����ڴ�ֱ����Ŀ�ȱ仯ʱ�ػ���������.
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
	//WS_CAPTION ����һ���б����Ĵ��ڣ�����WS_BORDER��񣩡� SW_SHOWMAXIMIZED ����ڲ�������ʾΪ��󻯴��ڡ�
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	//std::thread t(mainLoop, hWnd);

	// ��Ϣ�ṹ�����      message����Ϣ
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (msg.message != WM_QUIT){//��Ϣѭ������Ϸ����ѭ����
		//����Ϣ�����л�ȡ��Ϣ�������ȡ��   
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)){
			TranslateMessage(&msg);     //������Ϣ  
			DispatchMessage(&msg);      //�ɷ���Ϣ���ɷ�����Ϣ������������  
		}
		else{//û�л�ȡ��
			static DWORD dwTime = timeGetTime();
			DWORD dwCurrentTime = timeGetTime();
			DWORD dwElapsedTime = dwCurrentTime - dwTime;
			//��δ��������Ϊ��֡��60֡��Ҳ�����ñ���Ԥ����ָ����ƣ���������Ҿ����Ƿ������֡����
			float fElapsedTime = dwElapsedTime*0.001f;
			if (!game->gameLoop()){
				//���ٴ���
				SendNotifyMessage(hWnd, WM_DESTROY, NULL, NULL);
			}
			//   ��Ϸ�����߼�ѭ��  
			//   ��Ϸ������Ⱦѭ��  
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
	/***************************************����5***************************************/
	/**
	HWND hWnd = CreateWindow("game","���ƴ���",WS_OVERLAPPEDWINDOW,20,12,450,300,nullptr, nullptr, NU LL,nullptr);
	tagPAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	hdc����GDI�������gdi32.dll������hwnd�����û����(��user32.dll����)
	hwnd���������һ�����ڵ���״��λ�á���С���Ƿ���ʾ�����ĸ����ڡ��ֵܴ��ڡ��ȵȵ�һ�����ݽṹ��
	hdc�����һ��ʵʵ���ڵ����ھ�������������ʱ����Ҫ����������и�������ʵ�ֵĵط���

	case WM_KEYDOWN :	//���̰���
	case WM_KEYUP :		//�����ɿ�
	case WM_SYSKEYDOWN:	//ϵͳ������
	case WM_SYSKEYUP:	//ϵͳ���ɿ�
	case WM_CHAR:		//�ַ���Ϣ
	/**/
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message){
	case WM_PAINT:{//Ҫ��һ�������ػ��Լ�
					  hdc = BeginPaint(hWnd, &ps);
					  if (!isInitialization){
						  tagRECT winRECT;//LPRECT
						  //GetClientRect ����ָ�����ڿͻ������εĴ�С 
						  //GetWindowRect(hWnd, &winRECT);
						  GetClientRect(hWnd, &winRECT);
						  GP Rect winRect;
						  winRect.X = winRECT.left;
						  winRect.Y = winRECT.top;
						  winRect.Width = winRECT.right - winRECT.left;
						  winRect.Height = winRECT.bottom - winRECT.top;
						  //make_shared<Game>()// unique_ptr<>;
						  //����hWnd������ָ���Ĵ��ڵ��豸������: GetWindowDC(); 
						  //���ڻ��hWnd������ָ�����ڵĿͻ������һ���豸������: GetDC(hWnd) //�ػ�ʱ����˸
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
	case WM_MOUSEMOVE://				����ƶ�
	case WM_MOUSEWHEEL://				�����ֲ���
	case WM_LBUTTONDOWN://				�������
	case WM_LBUTTONUP://				�������
	case WM_LBUTTONDBLCLK://			���˫��
	case WM_MBUTTONDOWN://				�м�����
	case WM_MBUTTONUP://				�м�����
	case WM_MBUTTONDBLCLK://			�м�˫��
	case WM_RBUTTONDOWN://				�Ҽ�����
	case WM_RBUTTONUP://				�Ҽ�����
	case WM_RBUTTONDBLCLK://			�Ҽ�˫��
	{//����¼�
		MOUSEMSG c;
		tagPOINT m_mouse;
		GetCursorPos(&m_mouse);
		//ClientToScreen(hWnd, &m_mouse);  //�ͻ���λ��ת��Ϊ��Ļλ��
		ScreenToClient(hWnd, &m_mouse);  //��Ļת��Ϊ�ͻ���
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
	case WM_CREATE:{//Ӧ�ó��򴴽�һ������ 
					  break;
	}case WM_DESTROY:{//һ�����ڱ�����
						PostQuitMessage(0);
						break;
	}default:{
				return DefWindowProc(hWnd, message, wParam, lParam);
				break;
	}
	}
	return 0;
}
