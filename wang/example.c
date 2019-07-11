#include <Windows.h>
#include <stdio.h>
#include <math.h>
#include "prim1.h"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE L"PRIM生成树"
#define MAXV 100
#define INF 32767

typedef WNDPROC WMPROC;
typedef struct{
	UINT msg;
	WMPROC wmProc;
}MsgWithProc;

POINT pt[7] = {{0,0},{90,130},{250,200},{140,150},{160,80},{400,360},{300,370}}; //结点位置
HWND EDIT1 ;

HDC g_hdc = NULL;
HINSTANCE g_hInstance = NULL;

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT __stdcall WMcreate(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall WMpaint(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall WMcommand(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall WMkeydown(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall WMdestory(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd){
	g_hInstance = hInstance;
	WNDCLASSEX wndClass = {0};
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = NULL;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = L"Page36";


	if(!RegisterClassEx(&wndClass)){
		return -1;
	}
    //为了让窗口居中
    int cxScreen = GetSystemMetrics(SM_CXSCREEN);
    int cyScreen = GetSystemMetrics(SM_CYSCREEN);
    RECT rect = {};
    rect.left = cxScreen / 4;
    rect.right = cxScreen * 3 / 4;
    rect.top = cyScreen / 4;
    rect.bottom = cyScreen * 3 / 4;
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, TRUE);

	HWND hwnd = CreateWindowEx(
		NULL,
		L"Page36",
		WINDOW_TITLE,
		WS_POPUP | WS_SYSMENU ,
		rect.left, rect.top,rect.right-rect.left, rect.bottom-rect.top,
		NULL, NULL, hInstance, NULL
	);

	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);

	MSG msg = {0};
	while(msg.message != WM_QUIT){
		if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	UnregisterClass(L"Page36", hInstance);
	return 0;
}

//绘制指定属性的直线
static void DrawLine(HDC hDC, int x0, int y0, int x1, int y1, int style, int width, COLORREF color)
{
	HPEN hPen = CreatePen(style, width, color);
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

	MoveToEx(hDC, x0, y0, NULL);
	LineTo(hDC, x1, y1);

	SelectObject(hDC, hOldPen);
	DeleteObject(hPen);
}

//绘制实心圆
static void DrawCircle(HDC hDC, int x, int y, int len, COLORREF color)
{
	HBRUSH hBrush = CreateSolidBrush(color);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	HPEN hPen = CreatePen(PS_SOLID, 1, color);
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

	Ellipse(hDC, x-len/2, y-len/2, x+len/2, y+len/2);

	SelectObject(hDC, hOldBrush);
	DeleteObject(hPen);

	SelectObject(hDC, hOldPen);
	DeleteObject(hOldBrush);
}

//绘制填充矩形
static void DrawRect(HDC hDC, int left, int top, int width, int height, int style, COLORREF color)
{
	HBRUSH hBrush = CreateHatchBrush(style, color);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	Rectangle(hDC, left, top, left+width, top+height);

	SelectObject(hDC, hOldBrush);
	DeleteObject(hOldBrush);
}

//绘制位图填充矩形
static void DrawBmpRect(HDC hDC, int left, int top, int width, int height, LPCTSTR file)
{
	HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, file, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	HBRUSH hBrush = CreatePatternBrush(hBitmap);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	Rectangle(hDC, left, top, left+width, top+height);

	SelectObject(hDC, hOldBrush);
	DeleteObject(hOldBrush);
	DeleteObject(hBitmap);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){

	MsgWithProc msgWithProc[] = {
		{WM_CREATE,WMcreate},
		{WM_PAINT,WMpaint},
		{WM_COMMAND,WMcommand},
		{WM_KEYDOWN,WMkeydown},
		{WM_DESTROY,WMdestory}
	};

	for(int i=0 ;i < 4; i++){
		if(msgWithProc[i].msg == message){
			msgWithProc[i].wmProc(hwnd, message, wParam, lParam);
			return 0;
		}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT __stdcall WMcreate(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	CreateWindow(
		TEXT("button"),
		TEXT("Go"),
		WS_CHILD | WS_VISIBLE,
		520, 290, 80, 30,
		hwnd,
		(HMENU)10001,
		g_hInstance, NULL
	);
	EDIT1 = CreateWindow(
		TEXT("edit"),
		NULL,
		WS_CHILD | WS_VISIBLE |WS_BORDER,
		520, 220, 80, 20,
		hwnd,
		(HMENU)10002,
		g_hInstance, NULL
	);
}

LRESULT __stdcall WMpaint(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
    PAINTSTRUCT ps;
    HDC hDC = BeginPaint(hwnd, &ps);
    for (int i=10; i<50; i+=4)
    {
        SetPixel(hDC, i, 10, RGB(0, 0, 0)); //绘制像素点
    }
    //绘制不同模式的直线
    DrawLine(hDC, 105, 50, 425, 50, PS_JOIN_BEVEL, 2, RGB(0,0,0));
    DrawLine(hDC, 70, 400, 425, 400, PS_JOIN_BEVEL, 2, RGB(0,0,0));
    DrawLine(hDC, 425, 50, 425, 400, PS_JOIN_BEVEL, 2, RGB(0,0,0));
    DrawLine(hDC, 70, 50, 70, 400, PS_JOIN_BEVEL, 2, RGB(0,0,0));

    DrawLine(hDC, 500, 50, 650,  50, PS_JOIN_BEVEL, 2, RGB(0,0,0));
    DrawLine(hDC, 500, 400, 650,400, PS_JOIN_BEVEL, 2, RGB(0,0,0));
    DrawLine(hDC, 500, 50, 500, 400, PS_JOIN_BEVEL, 2, RGB(0,0,0));
    DrawLine(hDC, 650, 50, 650, 400, PS_JOIN_BEVEL, 2, RGB(0,0,0));


    //绘制圆
    for(int i=1;i<=6;i++){
        DrawCircle(hDC, pt[i].x, pt[i].y, 25, RGB(255, 255, 255));
        char string[16] = {0};
        itoa(i,string,16);

        TextOut(hDC, pt[i].x-6, pt[i].y-6, TEXT(string), 1);
    }
    //绘制文本*/
    SetBkColor(hDC,RGB(192,192,192));
    TextOut(hDC, 30, 40, TEXT("Show Graph"), 10);
    TextOut(hDC, 480, 40, TEXT("Control"), 7);
    TextOut(hDC, 520, 190, TEXT("Start"),5 );

    EndPaint(hwnd, &ps);
    return 0;

}

LRESULT __stdcall WMcommand(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	WORD wHigh = HIWORD(wParam);
	WORD wLow = LOWORD(wParam);
    PAINTSTRUCT ptStr; // 定义绘图信息结构体
    HDC hdc;
    char buff[32]; //起点文本框字符串
    int ret_num1 = 0 ;
    int start=0;  //起点文本框数值
	switch(wLow){
		case 10001:
            ret_num1 = GetWindowText(EDIT1, buff,32);
            start=atoi(buff);
            initm();
            prim(start);
		    hdc = GetDC(hwnd);
		    for(int i=0;i<100;i++){
                for(int j=0;j<1000;j++){
                    if(root[i][j][0]!=0&&root[i][j][1]!=0){
                        //printf("%d  %d\n",root[i][j][0],root[i][j][1]);
                        MoveToEx(hdc, pt[root[i][j][0]].x, pt[root[i][j][0]].y, NULL);
                        //画直线
                        LineTo(hdc, pt[root[i][j][1]].x, pt[root[i][j][1]].y);
                        //LineTo画完后 坐标会跟着改变
                    }
                }
		    }
            EndPaint(hwnd,  & ptStr); // 结束绘图
            return 0;
	}
	return 0;
}

LRESULT __stdcall WMkeydown(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	switch(wParam){
		case VK_F11:
		{
			HWND    hDesk;
			RECT    rc;
			hDesk = GetDesktopWindow();
			GetWindowRect(hDesk, &rc);
			SetWindowLong(hwnd, GWL_STYLE, WS_BORDER);
			SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, rc.right, rc.bottom, SWP_SHOWWINDOW);
			break;
		}
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
	}
	return 0;
}

LRESULT __stdcall WMdestory(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	PostQuitMessage(0);
	return 0;
}


