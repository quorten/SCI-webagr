/* Spy Corp. International Website Access Agreement and "sciup"
   "spyware downloader."  It doesn't actually download spyware, but it
   pretends to do so.  */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <mmsystem.h>

#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

#define ID_DECLINE 110
#define ID_ACCEPT 111

BOOL respectUser = FALSE;
BOOL accessDenied = FALSE;
BOOL fastAccessFail = FALSE;

char numRestarts = '0';
char stage = '0';
DWORD timeLeft = 12300;

TCHAR g_exeName[260];
string strTable[13];

BOOL WINAPI CtrlHandler(DWORD dwCtrlType)
{
	TCHAR params[9];
	if (dwCtrlType == CTRL_C_EVENT ||
		dwCtrlType == CTRL_CLOSE_EVENT ||
		dwCtrlType == CTRL_LOGOFF_EVENT ||
		dwCtrlType == CTRL_SHUTDOWN_EVENT)
	{
		numRestarts++;
		sprintf(params, "5%c%c%u", numRestarts, stage, timeLeft);
		ShellExecute(NULL, "open",
			g_exeName,
			params, NULL, SW_SHOWDEFAULT);
		ExitProcess(1);
	}
	return TRUE;
}

void CountdownStage(DWORD duration, HANDLE hConOut)
{
	COORD dwPos;
	if (numRestarts == '0')
	{
		dwPos.X = 16;
		dwPos.Y = 3;
	}
	else
	{
		dwPos.X = 16;
		dwPos.Y = 2;
	}

	DWORD lastTime = timeGetTime();
	while (true)
	{
		DWORD curTime = timeGetTime();
		if (curTime - lastTime >= duration)
		{
			timeLeft -= duration;
			break;
		}
		else
		{
			TCHAR numBuf[6] = "\0\0\0\0\0";
			DWORD charsWritten;
			sprintf(numBuf, "%u", timeLeft - (curTime - lastTime));
			WriteConsoleOutputCharacter(hConOut, numBuf, 5, dwPos, &charsWritten);
		}
	}
}

int SCIUpMain()
{
	HANDLE hConIn;
	HANDLE hConOut;
	COORD dwSize = { 80, 25 };
	DWORD bytesWritten;

	AllocConsole();
	hConIn = GetStdHandle(STD_INPUT_HANDLE);
	hConOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCtrlHandler(CtrlHandler, TRUE);
	SetConsoleTitle("sciup");
	SetConsoleScreenBufferSize(hConOut, dwSize);

	if (stage != '0') // tried to exit
	{
		WriteFile(hConOut, strTable[4].c_str(), strTable[4].length(), &bytesWritten, NULL);
		WriteFile(hConOut, strTable[5].c_str(), strTable[5].length(), &bytesWritten, NULL);
	}
	if (fastAccessFail)
	  stage = '9';
	switch (stage)
	{
	case '0':
		WriteFile(hConOut, strTable[3].c_str(), strTable[3].length(), &bytesWritten, NULL);
		WriteFile(hConOut, strTable[5].c_str(), strTable[5].length(), &bytesWritten, NULL);
		stage++;
	case '1':
		WriteFile(hConOut, strTable[6].c_str(), strTable[6].length(), &bytesWritten, NULL);
		CountdownStage(3000, hConOut);
		stage++;
	case '2':
		WriteFile(hConOut, strTable[7].c_str(), strTable[7].length(), &bytesWritten, NULL);
		CountdownStage(250, hConOut);
		stage++;
	case '3':
		WriteFile(hConOut, strTable[8].c_str(), strTable[8].length(), &bytesWritten, NULL);
		CountdownStage(2500, hConOut);
		stage++;
	case '4':
		WriteFile(hConOut, strTable[9].c_str(), strTable[9].length(), &bytesWritten, NULL);
		CountdownStage(500, hConOut);
		stage++;
	case '5':
		WriteFile(hConOut, strTable[10].c_str(), strTable[10].length(), &bytesWritten, NULL);
		CountdownStage(6000, hConOut);
		stage++;
	case '6':
	default:
		WriteFile(hConOut, strTable[11].c_str(), strTable[11].length(), &bytesWritten, NULL);
		CountdownStage(50, hConOut);
		if (accessDenied)
		{
			MessageBox(NULL, "Sorry, you cannot access\n\
Spy Corp. International's\n\
website. Sharing Violation.", "Extended Error", MB_OK | MB_ICONERROR);
			MessageBox(NULL, "You will not be able to access\n\
this website without proper\n\
permission from the current\n\
business partner you are using.", "Access Error", MB_OK | MB_ICONEXCLAMATION);
		}
		FreeConsole();
		if (accessDenied ||
			!ShellExecute(NULL, "open", strTable[12].c_str(), NULL, NULL, SW_SHOWDEFAULT))
			return 1;
	}

	return 0;
}

LRESULT CALLBACK TextWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP hBitmap = NULL;
	static HDC hDCMem = NULL;
	static HFONT hFont = NULL;
	HDC hDC;

	switch (uMsg)
	{
	case WM_CREATE:
		HBITMAP hBitmap;
		hBitmap = LoadBitmap((HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), (LPCTSTR)101);
		if (!hBitmap)
			return -1;
		hDC = GetDC(hwnd);
		hDCMem = CreateCompatibleDC(hDC);
		if (!hDCMem)
			return -1;
		SelectObject(hDCMem, hBitmap);
		ReleaseDC(hwnd, hDC);
		break;
	case WM_DESTROY:
		DeleteDC(hDCMem); hDCMem = NULL;
		DeleteObject(hBitmap); hBitmap = NULL;
		break;
	case WM_SETFONT:
		hFont = (HFONT)wParam;
		break;
	case WM_PAINT:
		RECT rt = { 5, 5, 720, 400 };
		PAINTSTRUCT ps;
		hDC = BeginPaint(hwnd, &ps);
		SelectObject(hDC, hFont);
		SetBkMode(hDC, TRANSPARENT);
		BitBlt(hDC, 325, 0, 725, 400, hDCMem, 0, 0, SRCCOPY);
		DrawText(hDC, strTable[1].c_str(), strTable[1].length(), &rt, DT_LEFT);
		EndPaint(hwnd, &ps);
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndDecline = NULL, hwndAccept = NULL, hwndText = NULL;
	static HFONT hFont = NULL;
	static BOOL acceptAgr = TRUE;

	switch (uMsg)
	{
	case WM_KEYUP:
		break;

	case WM_KEYDOWN:
		if ((int)wParam == VK_RETURN)
			PostMessage(hwnd, WM_COMMAND, ID_DECLINE, 0);
		break;

	case WM_COMMAND:
		switch (wParam)
		{
		case ID_DECLINE:
			if (respectUser)
				acceptAgr = FALSE;
			DestroyWindow(hwnd);
			break;
		case ID_ACCEPT:
			acceptAgr = TRUE;
			DestroyWindow(hwnd);
			break;
		}

	case WM_CREATE:
		if (respectUser)
			acceptAgr = FALSE;
		else
			acceptAgr = TRUE;
		hFont = CreateFont(18, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE, "MS SANS SERIF");
		if (!hFont)
			return -1;
		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = 0;
		wcex.lpfnWndProc = TextWindowProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = "spytxt";
		wcex.hIconSm = NULL;

		if (!RegisterClassEx(&wcex))
			return -1;

		hwndDecline = CreateWindowEx(NULL, "BUTTON", NULL, WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
			110, 450, 80, 28, hwnd, (HMENU)ID_DECLINE,
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
		if (!hwndDecline)
			return -1;
		hwndAccept = CreateWindowEx(NULL, "BUTTON", NULL, WS_CHILD | WS_VISIBLE,
			15, 450, 80, 28, hwnd, (HMENU)ID_ACCEPT,
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
		if (!hwndAccept)
			return -1;
		hwndText = CreateWindowEx(WS_EX_CLIENTEDGE, "spytxt", NULL,
			WS_CHILD | WS_OVERLAPPED | WS_VISIBLE, 20, 20, 725, 400, hwnd, NULL,
			(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
		if (!hwndText)
			return -1;
		SendMessage(hwndDecline, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(hwndAccept, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(hwndText, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessage(hwndDecline, WM_SETTEXT, 0, (LPARAM)"I Decline");
		SendMessage(hwndAccept, WM_SETTEXT, 0, (LPARAM)"I Accept");
		break;

	case WM_DESTROY:
	case WM_CLOSE:
		DestroyWindow(hwndDecline); hwndDecline = NULL;
		DestroyWindow(hwndAccept); hwndAccept = NULL;
		DestroyWindow(hwndText); hwndText = NULL;
		DeleteObject(hFont); hFont = NULL;
		if (!acceptAgr)
			PostQuitMessage(1);
		else
			PostQuitMessage(0);
		break;

	default:
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WebAgrMain(HINSTANCE hInstance)
{
	int xpos, ypos;
	int winWidth = 770, winHeight = 520;
	HWND hwnd;
	WNDCLASS wndclass;
	MSG msg;

	wndclass.style = 0;
	wndclass.lpfnWndProc = MainWindowProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = NULL;
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = "spy";

	if (!RegisterClass(&wndclass))
		return 1;

	{ /* Dynamically compute the window position so as to center it on
		 the screen, if possible.  */
		int winWidth_2 = winWidth / 2;
		int winHeight_2 = winHeight / 2;
		RECT workArea;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
		xpos = workArea.left + (workArea.right - workArea.left) / 2;
		ypos = workArea.top + (workArea.bottom - workArea.top) / 2;
		if (winWidth_2 > xpos - workArea.left) xpos = workArea.left;
		else xpos -= winWidth_2;
		if (winHeight_2 > ypos - workArea.top) ypos = workArea.top;
		else ypos -= winHeight_2;
	}

	hwnd = CreateWindowEx(WS_EX_WINDOWEDGE, "spy", "Spy Corp. International",
		WS_CAPTION | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPED,
		xpos, ypos, winWidth, winHeight, NULL, NULL, hInstance, NULL);
	if (!hwnd)
		return 1;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg);
		DispatchMessage (&msg);
	}

	if ((int)msg.wParam != 0)
		return (int)msg.wParam;

	// If the agreement was successful, run sciup.
	return SCIUpMain();
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	int i;
	TCHAR buffer[1024];

	// Load the string table.
	for (i = 1; i < 13; i++)
	{
		LoadString(hInstance, i, buffer, 1024);
		strTable[i] = buffer;
	}

	/* Get the name of the executable so that we can restart sciup if
	   the user tries to close it.  */
	GetModuleFileName(hInstance, g_exeName, 260);

	// Check if sciup should be restarted.
	if (lpCmdLine[0] == '5')
	{
		lpCmdLine++;
		numRestarts = *lpCmdLine++;
		stage = *lpCmdLine++;
		timeLeft = atoi(lpCmdLine);
		return SCIUpMain();
	}

	return WebAgrMain(hInstance);
}
