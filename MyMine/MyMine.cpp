// MyMine.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "MyMine.h"
#include<windows.h>

TCHAR szAppName[] = TEXT("MYMINE");
HINSTANCE hInst;							
HWND hWnd;
INT iRow = 9;
INT iCol = 9;
INT iWidth = 2 * CLIENTGAP + iCol*MINELONG;
INT iHeigth = 5 * CLIENTGAP + iRow*MINELONG;
HBITMAP hBitmap;
HBITMAP hBitmapFace;
HBITMAP hBitmapNum;
static INT iMines[2][18][32];
INT iGameState = GAMEUNSTART;
INT iFaceState = SMILEFACE1;
INT iTimeNum = 0;
INT iMineNum = 10;
INT iFlag = 10;


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK Help(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK About(HWND, UINT, WPARAM, LPARAM);
BOOL ModifyDifficulty(INT, INT, INT);
BOOL StartGame(VOID);
BOOL BuryMine(VOID);
BOOL BuryNumberAndBlank(VOID);
BOOL WinOrNot(VOID);
BOOL Recursion(INT, INT);
BOOL Convert(INT);


int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
                   LPSTR szCmdLine,int iCmdShow)
{
	MSG msg;
	WNDCLASS wndclass;
	hInst = hInstance;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYMINE));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wndclass.lpszMenuName = MAKEINTRESOURCE(IDC_MYMINE);
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("This Program requires Windows NT!"),
			szAppName,MB_ICONERROR);
		return 0;
	}

	hWnd = CreateWindow(szAppName,TEXT("扫雷"),
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		700, 250, iWidth, iHeigth,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int) msg.wParam;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc,hdcMem;
	HINSTANCE hInstance;
	BITMAP bitmap;
	BITMAP bitmapface;
	BITMAP bitmapnum;
	INT iX, iY, iMX, iMY, ix, iy;
	INT cxClient, cyClient;
	INT iFlag1, iFlag10,iTime1, iTime10, iTime100;

	switch (message)
	{
	case WM_CREATE:
		hInstance = ((LPCREATESTRUCT)lParam)->hInstance;

		hBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_MINE));
		GetObject(hBitmap, sizeof(BITMAP), &bitmap);

		hBitmapFace = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_FACE));
		GetObject(hBitmapFace, sizeof(BITMAP), &bitmapface);

		hBitmapNum = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_NUM));
		GetObject(hBitmapNum, sizeof(BITMAP), &bitmapnum);		

		StartGame();
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		hdcMem = CreateCompatibleDC(hdc);

		SetTimer(hWnd, IDT_TIMER, 1000, NULL);

		SelectObject(hdcMem, hBitmap);
		for (iY = 2 * CLIENTGAP+15, iMY = 1; iMY <= iRow; iY += MINELONG, iMY++)
		{
			for (iX =CLIENTGAP-8, iMX = 1; iMX <= iCol; iX += MINELONG, iMX++)
			{
				BitBlt(hdc, iX, iY, MINELONG, MINELONG,
					hdcMem, 0, MINELONG*iMines[0][iMY][iMX], SRCCOPY);
			}
		}

		SelectObject(hdcMem, hBitmapFace);
		BitBlt(hdc, iWidth / 2-20, 30, FACELONG, FACELONG,
			hdcMem, 0, iFaceState*FACELONG, SRCCOPY);

		SelectObject(hdcMem, hBitmapNum);
		if (iFlag >= 0)
		{
			BitBlt(hdc, 25, 30, NUMWIDTH, NUMHEIGTH,
				hdcMem, 0, NUMHEIGTH*ZERO, SRCCOPY);
			iFlag10 = iFlag / 10;
			iFlag1 = iFlag % 10;
		}
		else
		{
			BitBlt(hdc, 25, 30, NUMWIDTH, NUMHEIGTH,
				hdcMem, 0, NUMHEIGTH * 0, SRCCOPY);
			iFlag10 = -iFlag / 10;
			iFlag1 = -iFlag % 10;
		}
		
		BitBlt(hdc, 40, 30, NUMWIDTH, NUMHEIGTH,
			hdcMem, 0, NUMHEIGTH*(11 - iFlag10), SRCCOPY);

		BitBlt(hdc, 55, 30, NUMWIDTH, NUMHEIGTH,
			hdcMem, 0, NUMHEIGTH*(11 - iFlag1), SRCCOPY);

		iTime100 = iTimeNum / 100;
		iTime10 = (iTimeNum - iTime100 * 100) / 10;
		iTime1 = (iTimeNum - iTime100 * 100) % 10;

		BitBlt(hdc, iWidth - 53, 30, NUMWIDTH, NUMHEIGTH,
			hdcMem, 0, NUMHEIGTH*(11 - iTime1), SRCCOPY);
		BitBlt(hdc, iWidth - 68, 30, NUMWIDTH, NUMHEIGTH,
			hdcMem, 0, NUMHEIGTH*(11 - iTime10), SRCCOPY);
		BitBlt(hdc, iWidth - 83, 30, NUMWIDTH, NUMHEIGTH,
			hdcMem, 0, NUMHEIGTH*(11 - iTime100), SRCCOPY);

		DeleteObject(hdcMem);
		EndPaint(hWnd, &ps);
		return 0;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		
		switch (wmId)
		{
		case IDM_START:
			if (iGameState == GAMESTART)
			{
				INT Choice;
				Choice=MessageBox(hWnd, TEXT("Quit the Game?"),
					szAppName, MB_OKCANCEL|MB_ICONQUESTION);
				if (Choice == IDOK)
				{
					StartGame();
					InvalidateRect(hWnd, NULL, FALSE);
				}
			}
			else
			{
				StartGame();
				InvalidateRect(hWnd, NULL, FALSE);
			}
			return 0;

		case IDM_PRIMARY:
			ModifyDifficulty(9, 9, 10);
			StartGame();
			InvalidateRect(hWnd, NULL, FALSE);

			return 0;

		case IDM_MIDDLE:
			ModifyDifficulty(16, 16, 40);
			StartGame();
			InvalidateRect(hWnd, NULL, FALSE);

			return 0;

		case IDM_ADVANCED:
			ModifyDifficulty(30, 16, 99);
			StartGame();
			InvalidateRect(hWnd, NULL, FALSE);
			
			return 0;

		case IDM_EXIT:
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			return 0;

		case IDM_HELP:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_HELP), hWnd, Help);
			return 0;

		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUT), hWnd, About);
			return 0;
		}
		return 0;

	case WM_TIMER:
		if (iTimeNum <= 999)
		{
			iTimeNum++;
			InvalidateRect(hWnd, NULL, FALSE);
		}
		else
		{
			KillTimer(hWnd, IDT_TIMER);
			iGameState = GAMEOVER;
			iFaceState = FAILFACE;
			for (iMY = 1; iMY <= iRow; iMY++)
			{
				for (iMX = 1; iMX <= iCol; iMX++)
				{
					if ((iMines[0][iMY][iMX] == FLAG || iMines[0][iMY][iMX] == QUES)
						&& iMines[1][iMY][iMX] == MINE)
					{
						iMines[0][iMY][iMX] = MINE;
						InvalidateRect(hWnd, NULL, FALSE);
					}

					if ((iMines[0][iMY][iMX] == FLAG || iMines[0][iMY][iMX] == QUES) &&
						iMines[1][iMY][iMX] != MINE)
					{
						iMines[0][iMY][iMX] = WRONGMINE;
						InvalidateRect(hWnd, NULL, FALSE);
					}
				}
			}
			MessageBox(hWnd, TEXT("游戏耗时超过999秒，游戏结束！"), TEXT("扫雷"), MB_OK);
		}
		
		return 0;

	case WM_LBUTTONUP:
		hdc = GetDC(hWnd);
		hdcMem = CreateCompatibleDC(hdc);

		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);

		if (cxClient>=CLIENTGAP-8&&cxClient<=CLIENTGAP-8+iCol*MINELONG
			&&cyClient >= 2 * CLIENTGAP + 15 && cyClient <= 
			2 * CLIENTGAP + 15+iRow*MINELONG&&iGameState==GAMESTART)
		{
			cxClient = cxClient - (cxClient - (CLIENTGAP-8)) % MINELONG;
			cyClient = cyClient - (cyClient - (2*CLIENTGAP+15)) % MINELONG;
			ix = (cxClient - (CLIENTGAP - 8)) / MINELONG + 1;
			iy = (cyClient - (2 * CLIENTGAP + 15)) / MINELONG + 1;

			SelectObject(hdcMem, hBitmap);

			if (MINE==iMines[1][iy][ix])
			{
				iGameState = GAMEOVER;
				iFaceState = BADFACE;
				KillTimer(hWnd, IDT_TIMER);

				for (iY = 2 * CLIENTGAP + 15, iMY = 1; iMY <= iRow; iY += MINELONG, iMY++)
				{
					for (iX = CLIENTGAP-8, iMX = 1; iMX <= iCol; iX += MINELONG, iMX++)
					{
						if (MINE==iMines[1][iMY][iMX])
						{
							BitBlt(hdc, iX, iY, MINELONG, MINELONG,
								hdcMem, 0, MINELONG*iMines[1][iMY][iMX], SRCCOPY);
							iMines[0][iMY][iMX] = MINE;
						}

						if (MINE!=iMines[1][iMY][iMX] &&(FLAG==iMines[0][iMY][iMX]||
							QUES==iMines[0][iMY][iMX]))
						{
							iMines[0][iMY][iMX] = WRONGMINE;
							BitBlt(hdc, iX, iY, MINELONG, MINELONG,
								hdcMem, 0, MINELONG*iMines[0][iMY][iMX], SRCCOPY);
						}
					}
				}

				iMines[0][iy][ix] = REDMINE;
				BitBlt(hdc, cxClient, cyClient, MINELONG, MINELONG,
					hdcMem, 0, MINELONG*iMines[0][iy][ix], SRCCOPY);

				SelectObject(hdcMem, hBitmapFace);
				BitBlt(hdc, iWidth / 2 - 20, 30, FACELONG, FACELONG,
					hdcMem, 0, iFaceState*FACELONG, SRCCOPY);
				
				MessageBox(hWnd, TEXT("踩到雷啦，下次走运!!"), TEXT("扫雷"), 
					MB_OK | MB_ICONINFORMATION);
			}
			else
			{
				if (iMines[1][iy][ix] == BLANK)
				{
					iMines[0][iy][ix] = BLANK;
					Recursion(iy, ix);
					InvalidateRect(hWnd, NULL, FALSE);
				}
				else
				{
					iMines[0][iy][ix] =Convert( iMines[1][iy][ix]);
					BitBlt(hdc, cxClient, cyClient, MINELONG, MINELONG,
						hdcMem, 0, MINELONG* iMines[0][iy][ix], SRCCOPY);
				}

				if (WinOrNot())
				{
					iGameState = GAMEOVER;
					iFaceState = COOLFACE;
					KillTimer(hWnd, IDT_TIMER);

					SelectObject(hdcMem, hBitmapFace);
					BitBlt(hdc, iWidth / 2 - 20, 30, FACELONG, FACELONG,
						hdcMem, 0, iFaceState*FACELONG, SRCCOPY);

					MessageBox(hWnd, TEXT("恭喜你赢了!!!\n6 6 6!!!"), TEXT("扫雷"),
						MB_OK|MB_ICONINFORMATION);
				}
			}
		}

		DeleteObject(hdcMem);
		ReleaseDC(hWnd, hdc);
		return 0;

	case WM_RBUTTONUP:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);

		if (cxClient >= CLIENTGAP-8&&cxClient <= CLIENTGAP-8 + iCol*MINELONG
			&&cyClient >= 2 * CLIENTGAP + 15 && cyClient <=
			2 * CLIENTGAP + 15 + iRow*MINELONG&&iGameState==GAMESTART)
		{
			cxClient = cxClient - (cxClient - (CLIENTGAP-8)) % MINELONG;
			cyClient = cyClient - (cyClient -(2* CLIENTGAP+15)) % MINELONG;
			ix = (cxClient - (CLIENTGAP - 8)) / MINELONG + 1;
			iy = (cyClient - (2 * CLIENTGAP + 15)) / MINELONG + 1;

			if (iMines[0][iy][ix] == ORIG)
			{
				iMines[0][iy][ix] = FLAG;
				iFlag--;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			else if (iMines[0][iy][ix] == FLAG)
			{
				iMines[0][iy][ix] = QUES;
				iFlag++;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			else if (iMines[0][iy][ix] == QUES)
			{
				iMines[0][iy][ix] = ORIG;
				InvalidateRect(hWnd, NULL, FALSE);
			}
		}
		return 0;

	case WM_DESTROY:
		KillTimer(hWnd, IDT_TIMER);
		DeleteObject(hBitmap);
		DeleteObject(hBitmapFace);
		DeleteObject(hBitmapNum);
		PostQuitMessage(0);
		break;
	
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}


BOOL CALLBACK Help(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(hWnd, 0);
			return TRUE;
		}
		break;
	}
	return 0;
}

BOOL CALLBACK About(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(hWnd, 0);
			return TRUE;
		}
		break;
	}
	return 0;
}


BOOL ModifyDifficulty(INT x, INT y, INT iNum)
{
	iRow = y;
	iCol = x;
	iMineNum = iNum;
	iFlag = iMineNum;
	iTimeNum = 0;
	iWidth = 2 * CLIENTGAP + iCol*MINELONG;
	iHeigth = 5 * CLIENTGAP + iRow*MINELONG;
	
	MoveWindow(hWnd, 700, 250, iWidth, iHeigth, TRUE);

	return TRUE;
}

BOOL StartGame(VOID)
{
	SecureZeroMemory(iMines, sizeof(iMines));
	BuryMine();
	BuryNumberAndBlank();
	iFaceState = SMILEFACE1;
	iGameState = GAMESTART;
	iFlag = iMineNum;
	iTimeNum = 0;
	return TRUE;
}


BOOL BuryMine(VOID)
{
	INT iX, iY, iNum = 0;

	srand((UINT)GetCurrentTime());

	while (iNum < iMineNum)
	{
		iX = rand() % (iCol + 1);
		iY = rand() % (iRow + 1);
		if (iX != 0 && iY != 0 && iMines[1][iY][iX] != MINE)
		{
			iMines[1][iY][iX] = MINE;
			iNum++;
		}
	}
	return TRUE;
}


BOOL BuryNumberAndBlank(VOID)
{
	INT	iX, iY, iNum;

	for (iY = 1; iY <= iCol; iY++)
	{
		for (iX = 1; iX <= iRow; iX++)
		{
			iNum = 0;
			if (ORIG == iMines[1][iX][iY])	
			{
				if (iMines[1][iX - 1][iY] == MINE)iNum++;
				if (iMines[1][iX + 1][iY] == MINE)iNum++;
				if (iMines[1][iX][iY - 1] == MINE)iNum++;
				if (iMines[1][iX][iY + 1] == MINE)iNum++;
				if (iMines[1][iX - 1][iY - 1] == MINE)iNum++;
				if (iMines[1][iX + 1][iY + 1] == MINE)iNum++;
				if (iMines[1][iX - 1][iY + 1] == MINE)iNum++;
				if (iMines[1][iX + 1][iY - 1] == MINE)iNum++;
				iMines[1][iX][iY] = iNum;
			}
		}
	}

	for (iY = 1; iY <= iCol; iY++)
	{
		for (iX = 1; iX <= iRow; iX++)
		{
			if (ORIG == iMines[1][iX][iY])
			{
				iMines[1][iX][iY] = BLANK;			
			}
		}
	}
	return TRUE;
}


BOOL Recursion(INT iy, INT ix)
{
	INT iNum = 0;
	do
	{

		if (iMines[1][iy - 1][ix] == BLANK&&iMines[0][iy - 1][ix]!=BLANK)
		{
			iMines[0][iy - 1][ix] = BLANK;
			Recursion(iy-1,ix );
			iNum++;
		}
		else if (iMines[1][iy-1][ix] != MINE)
		{
			iMines[0][iy - 1][ix] = Convert(iMines[1][iy - 1][ix]);
		}


		if (iMines[1][iy + 1][ix] == BLANK&&iMines[0][iy + 1][ix] != BLANK)
		{
			iMines[0][iy + 1][ix] = BLANK;
			Recursion(iy+1,ix);
			iNum++;
		}
		else if (iMines[1][iy + 1][ix] != MINE)
		{
			iMines[0][iy + 1][ix] = Convert(iMines[1][iy + 1][ix]);
			iNum++;
		}
		

		if (iMines[1][iy][ix-1] == BLANK&&iMines[0][iy][ix-1] != BLANK)
		{
			iMines[0][iy][ix-1] = BLANK;
			Recursion(iy,ix-1);
			iNum++;
		}
		else if (iMines[1][iy][ix-1] != MINE)
		{
			iMines[0][iy][ix-1] = Convert(iMines[1][iy][ix-1]);
			iNum++;
		}


		if (iMines[1][iy][ix + 1] == BLANK&&iMines[0][iy][ix + 1] != BLANK)
		{
			iMines[0][iy][ix + 1] = BLANK;
			Recursion(iy,ix+1);
			iNum++;
		}
		else if (iMines[1][iy][ix + 1] != MINE)
		{
			iMines[0][iy][ix + 1] = Convert(iMines[1][iy][ix + 1]);
			iNum++;
		}



		if (iMines[1][iy - 1][ix-1] == BLANK&&iMines[0][iy - 1][ix-1] != BLANK)
		{
			iMines[0][iy - 1][ix-1] = BLANK;
			Recursion(iy-1, ix-1);
			iNum++;
		}
		else if (iMines[1][iy - 1][ix-1] != MINE)
		{
			iMines[0][iy - 1][ix-1] = Convert(iMines[1][iy - 1][ix-1]);
			iNum++;
		}


		if (iMines[1][iy + 1][ix + 1] == BLANK&&iMines[0][iy + 1][ix + 1] != BLANK)
		{
			iMines[0][iy + 1][ix + 1] = BLANK;
			Recursion(iy+1,ix+1);
			iNum++;
		}
		else if (iMines[1][iy + 1][ix + 1] != MINE)
		{
			iMines[0][iy + 1][ix + 1] = Convert(iMines[1][iy + 1][ix + 1]);
			iNum++;
		}


		if (iMines[1][iy - 1][ix + 1] == BLANK&&iMines[0][iy - 1][ix + 1] != BLANK)
		{
			iMines[0][iy - 1][ix + 1] = BLANK;
			Recursion(iy-1,ix+1);
			iNum++;
		}
		else if (iMines[1][iy - 1][ix + 1] != MINE)
		{
			iMines[0][iy - 1][ix + 1] = Convert(iMines[1][iy - 1][ix + 1]);
			iNum++;
		}


		if (iMines[1][iy + 1][ix - 1] == BLANK&&iMines[0][iy + 1][ix - 1] != BLANK)
		{
			iMines[0][iy + 1][ix - 1] = BLANK;
			Recursion(iy+1,ix-1);
			iNum++;
		}
		else if (iMines[1][iy + 1][ix - 1] != MINE)
		{
			iMines[0][iy + 1][ix - 1] = Convert(iMines[1][iy + 1][ix - 1]);
			iNum++;
		}		
	}while (iNum == 0);

	return TRUE;
}


BOOL Convert(INT Old)
{
	switch (Old)
	{
	case 15:
		return 15;break;
	case 1:
		return 14; break;
	case 2:
		return 13; break;
	case 3:
		return 12; break;
	case 4:
		return 11; break;
	case 5:
		return 10; break;
	case 6:
		return 9; break;
	case 7:
		return 8; break;
	case 8:
		return 7; break;
	default:break;
	}
}

BOOL WinOrNot(VOID)
{
	INT iMX, iMY;
	INT Flag = 0, Ques = 0, Orig = 0;

	for (iMY = 1; iMY <= iRow; iMY++)
	{
		for (iMX = 1; iMX <= iCol; iMX++)
		{
			if (iMines[0][iMY][iMX] == FLAG)
			{
				Flag++;
			}
			if (iMines[0][iMY][iMX] == QUES)
			{
				Ques++;
			}
			if (iMines[0][iMY][iMX] == ORIG)
			{
				Orig++;
			}
		}
	}

	if (iMineNum == Flag + Ques + Orig)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}