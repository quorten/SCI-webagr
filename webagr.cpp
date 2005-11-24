#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>

#define ID_DECLINE 110
#define ID_ACCEPT 111

HWND hwndDecline;
HWND hwndAccept;
HWND hwndText;
HDC hDCMem;
HFONT hFont;

LRESULT CALLBACK TextWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		HBITMAP hBitmap;
		HDC hDC;
		hBitmap = LoadBitmap((HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), "#101");
		hDC = GetDC(hwnd);
		hDCMem = CreateCompatibleDC(hDC);
		SelectObject(hDCMem, hBitmap);
		ReleaseDC(hwnd, hDC);
		break;
	case WM_DESTROY:
		break;
	case WM_PAINT:
		HDC hdc;
		RECT rt = { 5, 5, 720, 400 };
		PAINTSTRUCT ps;
		hdc = BeginPaint(hwnd, &ps);
		SelectObject(hdc, hFont);
		SetBkMode(hdc, TRANSPARENT);
		BitBlt(hdc, 325, 0, 725, 400, hDCMem, 0, 0, SRCCOPY);
		DrawText(hdc,
"YOU MUST AGREE TO THE FOLOWING TERMS TO ACCESS THIS WEBSITE:\n\
	    1. Spy Corp. owns ANY AND ALL products you make on this computer.\n\
	    2. Spy Corp. is NOT RESPONSIBLE for any damage caused to your computer by use of this website.\n\
	    3. Spy Corp. DOES NOT need your permission to take files from your computer.\n\
	    4. Your life is no longer private once you agree to these rules.\n\
	    5. You have given Spy Corp. permission to know what you are doing at any given time.\n\
	    6. If you copy any information from this website, Spy Corp. will assassinate you.\n\
	    7. If you attempt to hack into any of our computers, YOU WILL BE ASSASSINATED.\n\
	    8. Spy Corp. can bring you in for interrogation at ANY TIME.\n\
	    9. Spy Corp. is allowed to use your house as a base of operations WITHOUT YOUR KNOWLEDGE.\n\
	    10. These rules DO NOT violate your freedoms, rights, and/or responsibilities.",
			889, &rt, DT_LEFT);
		EndPaint(hwnd, &ps);
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYUP:
		break;

	case WM_KEYDOWN:
		int fwKeys;
		LPARAM keyData;
		fwKeys = (int)wParam;    // virtual-key code 
		keyData = lParam;          // key data 

		switch(fwKeys)
		{
		case VK_RETURN:
			PostMessage(hwnd, WM_COMMAND, ID_DECLINE, 0);
			break;
		default:
			break;
		}

		break;

	case WM_COMMAND:
		switch (wParam)
		{
		case ID_DECLINE:
		case ID_ACCEPT:
			ShellExecute(NULL, "open", "C:\\Documents and Settings\\Christopher\\My Documents\\SpyCorp\\SCI_Webpage.doc", NULL, NULL, SW_SHOWDEFAULT);
			PostQuitMessage(0);
			break;
		}

	case WM_CREATE:
		hFont = CreateFont(18, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE, "MS SANS SERIF");
		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = TextWindowProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_MENU+1);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = "spytxt";
		wcex.hIconSm = NULL;

		RegisterClassEx(&wcex);

		hwndDecline = CreateWindowEx(NULL, "BUTTON", NULL, WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
			110, 450, 80, 28, hwnd, (HMENU)ID_DECLINE,
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
		hwndAccept = CreateWindowEx(NULL, "BUTTON", NULL, WS_CHILD | WS_VISIBLE,
			15, 450, 80, 28, hwnd, (HMENU)ID_ACCEPT,
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
		hwndText = CreateWindowEx(WS_EX_CLIENTEDGE, "spytxt", NULL,
			WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 20, 20, 725, 400, hwnd, NULL,
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
		SendMessage(hwndDecline, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(hwndAccept, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(hwndDecline, WM_SETTEXT, 0, (LPARAM)"I Decline");
		SendMessage(hwndAccept, WM_SETTEXT, 0, (LPARAM)"I Accept");
		break;

	case WM_DESTROY:
	case WM_QUIT:
	case WM_CLOSE:
		DestroyWindow(hwndDecline);
		DestroyWindow(hwndAccept);
		DestroyWindow(hwndText);
		PostQuitMessage(0);
		break;

	default:
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	HWND hwnd;
	WNDCLASS wndclass;
	MSG msg;

	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_PARENTDC;
	wndclass.lpfnWndProc = MainWindowProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = NULL;
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = "spy";

	RegisterClass(&wndclass);

	hwnd = CreateWindowEx(WS_EX_WINDOWEDGE, "spy", "Spy Corp. International",
		WS_CAPTION | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPED,
		145, 120, 770, 520, NULL, NULL, hInstance, NULL);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg);
		DispatchMessage (&msg);
	}

	return (int)msg.wParam;
}