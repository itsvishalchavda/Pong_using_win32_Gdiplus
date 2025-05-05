#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;

HINSTANCE g_hInst = NULL;
Image* g_ImgBall = nullptr; // ball-a.png
Image* g_ImgBkgrn = nullptr; // Neon Tunnel.png
Image* g_ImgLine = nullptr; // line.png
Image* g_ImgPaddle = nullptr; //paddle.png
Rect g_RecBall;
Rect g_RecBkgrn;
Rect g_RecLine;
Rect g_RecPaddle;
BOOL g_bGameStarted = FALSE;

float delta = 3;
float dX = delta;
float dY = -delta;
int Score = 0;

HWND g_hBtn = NULL;

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("PongMadeByVK");

VOID OnPaint(HDC hdc)
{
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP hMemBmp = CreateCompatibleBitmap(hdc,900,900);
    HBITMAP hOldBmp =  (HBITMAP)SelectObject(memDC,hMemBmp);

    Graphics graphics(memDC);
    Gdiplus::SolidBrush wBrush(Gdiplus::Color(255, 255, 255));
    graphics.FillRectangle(&wBrush,0,0,900,900);

    graphics.DrawImage(g_ImgBkgrn, g_RecBkgrn);
    graphics.DrawImage(g_ImgPaddle, g_RecPaddle);
    graphics.DrawImage(g_ImgLine, g_RecLine);
    graphics.DrawImage(g_ImgBall, g_RecBall);

    RECT RecText;
    RecText.left = g_RecBkgrn.X + 150;
    RecText.top = g_RecBkgrn.Y + g_RecBkgrn.Height + 10;
    RecText.right = RecText.left + 500;
    RecText.bottom = RecText.top + 100;

    if(!g_bGameStarted)
    {

        char text[100] = {0};
        sprintf(text, "Click To Start!!! - Last Score is :- %d", Score);
        DrawText(memDC, text, sizeof(text), &RecText, DT_LEFT);
    }
    else
    {
        char text[100] = {0};
        sprintf(text, "Score :- %d", Score);
        DrawText(memDC, text, sizeof(text), &RecText, DT_LEFT);
    }

    BitBlt(hdc, 0, 0, 900, 900, memDC, 0, 0, SRCCOPY);
    // Always select the old bitmap back into the device context
    SelectObject(memDC, hOldBmp);
    DeleteObject(hMemBmp);
    DeleteDC(memDC);

}

void SetPeddleAsPerMouse(int xPos, int yPos)
{
    if((xPos >= g_RecBkgrn.X) && (xPos <= (g_RecBkgrn.X + g_RecBkgrn.Width - g_RecPaddle.Width)))
    {
        g_RecPaddle.X = xPos;
    }
    else
    {
        if(xPos <= g_RecBkgrn.X)
        {
            g_RecPaddle.X = g_RecBkgrn.X;
        }
        else  g_RecPaddle.X = g_RecBkgrn.X + g_RecBkgrn.Width - (g_RecPaddle.Width);
    }
}

void BounceTheBall()
{
    if(((g_RecBall.X + g_RecBall.Width) >= (g_RecBkgrn.X + g_RecBkgrn.Width)))
        dX = -delta;
    if(g_RecBall.X <= g_RecBkgrn.X)
        dX = delta;
    if(g_RecBall.Y <= g_RecBkgrn.Y)
        dY = delta;
    if((g_RecBall.Y + g_RecBall.Height) >= (g_RecBkgrn.Y + g_RecBkgrn.Height - g_RecLine.Height))
        g_bGameStarted = !g_bGameStarted; // game over
    if(
            (g_RecBall.Y + g_RecBall.Height >= g_RecPaddle.Y)
       && (g_RecBall.Y + g_RecBall.Height <= g_RecPaddle.Y + g_RecPaddle.Height)
       && (g_RecBall.X + g_RecBall.Width >= g_RecPaddle.X)
       && (g_RecBall.X  <= g_RecPaddle.X + g_RecPaddle.Width)
       )
       {
            dY = -delta;
            Score++;
       }
}

void GameLoop(HWND hwnd)
{
    if(g_bGameStarted)
    {
        g_RecBall.X += dX;
        g_RecBall.Y += dY;

        POINT pt = {0};
        GetCursorPos(&pt);
        ScreenToClient(hwnd, &pt);
        SetPeddleAsPerMouse( pt.x, pt.y);

        BounceTheBall();
        HDC hdc = GetDC(hwnd);
        OnPaint(hdc);
        ReleaseDC(hwnd, hdc);

        delta += 0.001;

    }
}

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;

    // Initialize GDI+.
   GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    /* The Window structure */
    g_hInst = wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);//COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("Pong The Game"),       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           900,                 /* The programs width */
           700,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
//    while (GetMessage (&messages, NULL, 0, 0))
//    {
//        /* Translate virtual-key messages into character messages */
//        TranslateMessage(&messages);
//        /* Send message to WindowProcedure */
//        DispatchMessage(&messages);
//    }

    while (TRUE)
    {
        if(PeekMessageA(&messages, NULL, 0, 0, PM_NOREMOVE))
        {
            if(!GetMessage (&messages, NULL, 0, 0)) break;
            /* Translate virtual-key messages into character messages */
            TranslateMessage(&messages);
            /* Send message to WindowProcedure */
            DispatchMessage(&messages);
        }
        GameLoop(hwnd);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    GdiplusShutdown(gdiplusToken);
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_LBUTTONDOWN:
        {
            if(!g_bGameStarted)
            {
                Score = 0;
                delta = 3;
                dX = delta;
                dY = -delta;
                g_bGameStarted = TRUE;
            }
        }
        break;
        case WM_COMMAND:
        {
            if(HWND(lParam) == g_hBtn)
            {
                g_bGameStarted = TRUE;
            }
        }
        break;
        case WM_MOUSEMOVE:
        {
//            int xPos = GET_X_LPARAM(lParam);
//            int yPos = GET_Y_LPARAM(lParam);
//            if(g_bGameStarted)
//            {
//                OnMouseMove(hwnd, xPos, yPos);
//                GameLoop(hwnd);
//            }
        }
        break;
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            OnPaint(hdc);
            // All painting occurs here, between BeginPaint and EndPaint.
            //FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
            EndPaint(hwnd, &ps);
        }
        break;
        case WM_CREATE:
        {
            char szFileName[1000];
            WCHAR wcFilePath[1000];

            GetModuleFileName(NULL, szFileName, 1000);
            char* temp = strrchr(szFileName, '\\');
            if(temp)
            {
                temp++;
            }
            else
            {
                strcpy(szFileName, ".\\");
            }

            strcpy(temp, "ball-a.png");
            MultiByteToWideChar(CP_ACP, 0, szFileName, 999, wcFilePath, 999);
            g_ImgBall = Image::FromFile(wcFilePath);
//            MessageBoxW(hwnd,wcFilePath, L"",MB_OK);
//            if(!g_ImgBall)
//            {
//                MessageBoxA(hwnd, "No image", "", MB_OK);
//                return (-1);
//            }

            strcpy(temp, "Neon_Tunnel.png");
            MultiByteToWideChar(CP_ACP, 0, szFileName, 999, wcFilePath, 999);
            g_ImgBkgrn = Image::FromFile(wcFilePath);
//            MessageBoxW(hwnd,wcFilePath, L"",MB_OK);
//            if(!g_ImgBkgrn)
//            {
//                MessageBoxA(hwnd, "No image", "", MB_OK);
//                return (-1);
//            }

            strcpy(temp, "line.png");
            MultiByteToWideChar(CP_ACP, 0, szFileName, 999, wcFilePath, 999);
            g_ImgLine = Image::FromFile(wcFilePath);
//            MessageBoxW(hwnd,wcFilePath, L"",MB_OK);
//            if(!g_ImgLine)
//            {
//                MessageBoxA(hwnd, "No image", "", MB_OK);
//                return (-1);
//            }

            strcpy(temp, "paddle.png");
            MultiByteToWideChar(CP_ACP, 0, szFileName, 999, wcFilePath, 999);
            g_ImgPaddle = Image::FromFile(wcFilePath);
//            MessageBoxW(hwnd,wcFilePath, L"",MB_OK);
//            if(!g_ImgPaddle)
//            {
//                MessageBoxA(hwnd, "No image", "", MB_OK);
//                return (-1);
//            }

            if(!g_ImgBall || !g_ImgBkgrn || !g_ImgLine || !g_ImgPaddle)
            {
                MessageBoxA(hwnd, "No image", "", MB_OK);
                return (-1);
            }

            // SETTING POSITIONS

            float flScale = 1.5;

            // BACKGROUND
            g_RecBkgrn.X = 100;
            g_RecBkgrn.Y = 25;
            g_RecBkgrn.Width = g_ImgBkgrn->GetWidth() / flScale;
            g_RecBkgrn.Height = g_ImgBkgrn->GetHeight() / flScale;

            // RED LINE
            g_RecLine.Width = g_ImgLine->GetWidth() / flScale;
            g_RecLine.Height = g_ImgLine->GetHeight() / flScale;
            g_RecLine.X = g_RecBkgrn.X;
            g_RecLine.Y = g_RecBkgrn.Y + g_RecBkgrn.Height - g_RecLine.Height;

            // PADDLE
            g_RecPaddle.Width = g_ImgPaddle->GetWidth() / flScale;
            g_RecPaddle.Height = g_ImgPaddle->GetHeight() / flScale;
            g_RecPaddle.X = g_RecBkgrn.X + (g_RecBkgrn.Width / 2) - (g_ImgPaddle->GetWidth() / 2);
            g_RecPaddle.Y = g_RecBkgrn.Y + g_RecBkgrn.Height - (3 * g_RecPaddle.Height);

            // BALL
            g_RecBall.Width = g_ImgBall->GetWidth() / flScale;
            g_RecBall.Height = g_ImgBall->GetHeight() / flScale;
            g_RecBall.X = g_RecPaddle.X + (g_RecPaddle.Width/2) - (g_RecBall.Width / 2);
            g_RecBall.Y = g_RecPaddle.Y - g_RecBall.Height;

            // CREATE START BUTTON

            g_hBtn = CreateWindowExA(NULL,
            "BUTTON",
            "Start",
            WS_CHILD | BS_DEFPUSHBUTTON, //WS_VISIBLE
            15,
            25,
            70,
            25,
            hwnd,
            (HMENU)NULL,
            g_hInst,
            NULL);

        }
        break;
        case WM_DESTROY:
        {
            delete g_ImgBall;
            delete g_ImgBkgrn;
            delete g_ImgLine;
            delete g_ImgPaddle;
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
        }
        break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
