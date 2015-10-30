#include <Windows.h>
#include "resource.h"

#define boardsize 9
#define imagesize 60

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void CALLBACK PlaceQueen(HWND, UINT, UINT, DWORD);
void CALLBACK AllQueens(HWND, UINT, UINT, DWORD);

HINSTANCE g_hInst;

HBITMAP QBit, OldBit;

LPCWSTR lpszClass = TEXT("NQueen");

static int Row = 0; //stores current row
static int Queen[boardsize] = {}; //stores 8 Queens.
int cnt = 0;
TCHAR mystringbuf[256];
RECT R{ 0, 0, (boardsize+1) * imagesize, (boardsize+1.2) * imagesize };
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_VREDRAW | CS_HREDRAW;
	RegisterClass(&WndClass);
	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,(boardsize+1)*imagesize,(boardsize+1.2)*imagesize, NULL, (HMENU)NULL, hInstance, NULL);
	
	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&Message, 0,0,0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	return Message.wParam;

}


LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	static RECT TextRect{ (imagesize*boardsize) / 2 - 30, (imagesize*boardsize) - 20, (imagesize*boardsize) / 2 + 30, (imagesize*boardsize) + 20 };

	switch (iMessage)
	{
	case WM_CREATE:
		QBit = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP3));
		SetTimer(hWnd, 1, USER_TIMER_MINIMUM, PlaceQueen);
		//GetClientRect(hWnd, &R);
		return 0;
	case WM_PAINT:
	{	
		hdc = BeginPaint(hWnd, &ps);
		HDC MemDC = CreateCompatibleDC(hdc);
		OldBit = (HBITMAP)SelectObject(MemDC, QBit);
		for (int i = 0; i < boardsize; i++)
		{
			for (int j = 0; j < boardsize; j++)
			{
				SelectObject(hdc, GetStockObject((i + j) & 1 ? WHITE_BRUSH : BLACK_BRUSH));
				Rectangle(hdc, i * imagesize, j * imagesize, (i + 1) * imagesize, (j + 1) * imagesize);
			}
		}
		SetTextAlign(hdc, TA_CENTER);
		TextOut(hdc, boardsize * 30, boardsize * imagesize, L"N-QUEEN SIMULATOR", lstrlen(L"N-QUEEN SIMULATOR"));
		for (int l = 0; l <Row; l++)
		{
			BitBlt(hdc, Queen[l] * imagesize, l * imagesize, imagesize, imagesize, MemDC, 0, 0, SRCINVERT);
		}
		wsprintf(mystringbuf, L"%03d", cnt);
		TextOut(hdc, boardsize * imagesize, boardsize * imagesize, mystringbuf, lstrlen(mystringbuf));
		//DrawText(hdc, mystringbuf, lstrlen(mystringbuf), &TextRect, DT_CENTER);
		//SetWindowText(hWnd, mystringbuf);

		SelectObject(MemDC, OldBit);
		DeleteDC(MemDC);

		EndPaint(hWnd, &ps);
		return 0;
	}
	case WM_DESTROY:
		DeleteObject(QBit);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, iMessage, wParam, lParam);
}


void CALLBACK PlaceQueen(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	HDC hdc;

	for (int j = Queen[Row]; j < boardsize; j++) //starts from current value
	{
		//hdc = GetDC(hWnd);
		//wsprintf(mystringbuf, L"(%05d)", ++callnum);
		//TextOut(hdc, 540 + cntx, 30 * cnty, mystringbuf, lstrlen(mystringbuf));
		//cntx += cnty / 10;
		//cnty = (cnty + 1) % 10;
		//ReleaseDC(hWnd, hdc);

		int k = 0;
		for (k = 0; k < Row; k++) //check all the previously-put queens
		{
			if (Queen[k] == j || (abs(Queen[k] - j) == abs(k - Row)))
				break; // diagonally/vertically lined.
		}

		if (k == Row) //passed all the tests
		{
			Queen[Row++] = j; //place the new queen!
			//hdc = GetDC(hWnd); //Draw Queens
			InvalidateRect(hWnd,&R, TRUE); //ClrScr
			//wsprintf(mystringbuf, L"(i=%02d,j=%02d)", i, j);
			//TextOut(hdc,540, 30*(++cnty), mystringbuf, lstrlen(mystringbuf));

			//ReleaseDC(hWnd, hdc);

			if (Row == boardsize) // all queens are placed, wait for 2.2 seconds.
			{
				MessageBeep(MB_OK);
				//Row -= 2; // i = 6 (Meaning, Go BACK 1 ROW ... from 7 -> 6) working on Queen[6] Now. with advanced position
				//Since there is no point to working on row 7 anymore.
				KillTimer(hWnd, idEvent);
				SetTimer(hWnd, 2, 1000, AllQueens);
				
			}
			else
			{
				KillTimer(hWnd, idEvent);
				SetTimer(hWnd, 2, 20, AllQueens);
			}
			return;
		}
	} //THIS POINT, None of Queen[i][j]s Satisfied No-Touchy Dictum.
	if (Row)
	{
		Queen[Row] = 0; //reset current row
		++Queen[--Row]; // Go back one row, with advanced position
		//here, couldn't place any queen = need to advance the previous queen but still working on current row
	}
	else
		KillTimer(hWnd, 1); //here, couldn't place any queens at all = QUIT.
}

void CALLBACK AllQueens(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	if (Row == boardsize) ++Queen[--Row], ++cnt; //advance the position of Queen[6].
	KillTimer(hWnd, idEvent);
	SetTimer(hWnd, 1, USER_TIMER_MINIMUM, PlaceQueen);
	return;
}