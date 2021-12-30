// PicTest.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "framework.h"
#include "PicTest.h"
#include "exif.h"
#include <objidl.h>
#include <gdiplus.h>

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
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

    ULONG_PTR token;
    GdiplusStartupInput input = { 0 };
    input.GdiplusVersion = 1;
    GdiplusStartup(&token, &input, NULL);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PICTEST, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PICTEST));

    MSG msg;

    // Main message loop:
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
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PICTEST));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PICTEST);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

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
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

static RotateFlipType OrientationToFlipType(int nOrientation)
{
    switch (nOrientation)
    {
    case 1:
        return RotateFlipType::RotateNoneFlipNone;
        break;
    case 2:
        return RotateFlipType::RotateNoneFlipX;
        break;
    case 3:
        return RotateFlipType::Rotate180FlipNone;
        break;
    case 4:
        return RotateFlipType::Rotate180FlipX;
        break;
    case 5:
        return RotateFlipType::Rotate90FlipX;
        break;
    case 6:
        return RotateFlipType::Rotate90FlipNone;
        break;
    case 7:
        return RotateFlipType::Rotate270FlipX;
        break;
    case 8:
        return RotateFlipType::Rotate270FlipNone;
        break;
    default:
        return RotateFlipType::RotateNoneFlipNone;
    }
}

#define TEST_JPG_PATH "E:\\Code\\MultiMonitorScreenSaver\\IMG_0277.JPG"
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int nRetCode = 0;
    static Image* pimg = nullptr;
    switch (message)
    {
    case WM_CREATE:
        {
            pimg = new Image(_TEXT(TEST_JPG_PATH));
            easyexif::EXIFInfo exif;
            {
                FILE* fp = nullptr;
                fopen_s(&fp, TEST_JPG_PATH, "rb");
                assert(fp);

                fseek(fp, 0, SEEK_END);
                int len = ftell(fp);

                fseek(fp, 0, SEEK_SET);
                BYTE* pbyBuffer = new BYTE[len];
                fread(pbyBuffer, len, 1, fp);

                nRetCode = exif.parseFrom(pbyBuffer, len);
                assert(nRetCode == PARSE_EXIF_SUCCESS);

                fclose(fp);

                pimg->RotateFlip(OrientationToFlipType(exif.Orientation));
            }
        }
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
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
            // TODO: Add any drawing code that uses hdc here...
            if (pimg)
            {
                Graphics g(hdc);

                RECT rc;
                GetClientRect(hWnd, &rc);
                RECT rc2 = rc;

                //pimg->RotateFlip(RotateFlipType::Rotate90FlipY);

                int nWidth = pimg->GetWidth();
                int nHeight = pimg->GetHeight();
                if (nWidth < nHeight)
                {
                    rc2.right = nWidth * rc.bottom / nHeight;
                }
                else
                {
                    rc2.bottom = nHeight * rc.right / nWidth;
                }
                g.DrawImage(pimg, (rc.right - rc2.right) / 2, (rc.bottom - rc2.bottom) / 2, (int)rc2.right, (int)rc2.bottom);
            }
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

// Message handler for about box.
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
