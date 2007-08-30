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

char numRestarts = '0';
char stage = '0';
DWORD timeLeft = 12300;
string strTable[12];
HWND hwndDecline;
HWND hwndAccept;
HWND hwndText;
HDC hDCMem;
HFONT hFont;

BOOL WINAPI CtrlHandler(DWORD dwCtrlType)
{
	char params[8];
	if (dwCtrlType == CTRL_CLOSE_EVENT ||
		dwCtrlType == CTRL_LOGOFF_EVENT ||
		dwCtrlType == CTRL_SHUTDOWN_EVENT)
	{
		numRestarts++;
		sprintf(params, "\x05%c%c%u", numRestarts, stage, timeLeft);
		ShellExecute(NULL, "open",
			"C:\\Documents and Settings\\Christopher\\My Documents\\SpyCorp\\webagr.exe",
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
			char numBuf[5];
			DWORD charsWritten;
			sprintf(numBuf, "%u", timeLeft - (curTime - lastTime));
			WriteConsoleOutputCharacter(hConOut, numBuf, 5, dwPos, &charsWritten);
		}
	}
}

void SCIUpMain()
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

	if (stage != '0') //tried to exit
	{
		WriteFile(hConOut, strTable[4].c_str(), strTable[4].length(), &bytesWritten, NULL);
		WriteFile(hConOut, strTable[5].c_str(), strTable[5].length(), &bytesWritten, NULL);
	}
	switch (stage)
	{
	//case '0':
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
		MessageBox(NULL, "Sorry, you cannot access\n\
Spy Corp. International's\n\
website. Sharing Violation.", "Extended Error", MB_OK | MB_ICONERROR);
		MessageBox(NULL, "You will not be able to access\n\
this website without proper\n\
permission from the current\n\
business partner you are using.", "Access Error", MB_OK | MB_ICONEXCLAMATION);
		FreeConsole();
		//ShellExecute(NULL, "open", strTable[12].c_str(), NULL, NULL, SW_SHOWDEFAULT);
	}
}

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
		DrawText(hdc, strTable[1].c_str(), strTable[1].length(), &rt, DT_LEFT);
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
		if ((int)wParam == VK_RETURN)
			PostMessage(hwnd, WM_COMMAND, ID_DECLINE, 0);
		break;

	case WM_COMMAND:
		switch (wParam)
		{
		case ID_DECLINE:
		case ID_ACCEPT:
			ShowWindow(hwnd, SW_HIDE);
			SCIUpMain();
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

int ShowWebAgr(HINSTANCE hInstance)
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

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	int i;
	TCHAR buffer[1024];
	int msgwParam;

	//Load the string table.
	for (i = 1; i < 13; i++)
	{
		LoadString(hInstance, i, buffer, 1024);
		strTable[i] = buffer;
	}

	//Check if sciup should be restarted.
	if (lpCmdLine[0] == '\x05')
	{
		lpCmdLine++;
		numRestarts = *lpCmdLine++;
		stage = *lpCmdLine++;
		timeLeft = atoi(lpCmdLine);
		SCIUpMain();
		return 0;
	}

	msgwParam = ShowWebAgr(hInstance);

	return 0;
}