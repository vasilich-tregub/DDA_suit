// flattenham_win.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "flattenham_win.h"
#include "showroom.h"
#include <format>
#include <windowsx.h>

#define MAX_LOADSTRING 100

// Global Variables:
bool inprocess = false;
double x0 = 0;
double yy0 = 0; // cannot use y0 indentifier as double y0(double x) is a 2nd kind order 0 Bessel function declared in corecrt_math.h
double x1 = 0;
double yy1 = 0; // cannot use y1 indentifier as double y1(double x) is a 2nd kind order 1 Bessel function declared in corecrt_math.h
double seglen, seglen2;
double yintersect = HEIGHT;
double xintersect = WIDTH;
int ix0, iy0, ix1, iy1;
inode nextnode{ WIDTH, HEIGHT, LEFT }; // WIDYJ, HEIGHT values are assigned in flattenham_win.h
vec linevec{};
bool intersectfound = false;
std::vector<POINT> vertices;

int xPosDown = 0, yPosDown = 0, xPosUp = 0, yPosUp = 0;
int lineIx = 0;

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

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_FLATTENHAMWIN, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FLATTENHAMWIN));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FLATTENHAMWIN));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_FLATTENHAMWIN);
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
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
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
            case ID_ACCELF9:
                inprocess = false;
                for (int ix = 0; ix < WIDTH * HEIGHT; ++ix) fld[ix] = pxlnan;
                vertices.clear();
                InvalidateRect(hWnd, NULL, TRUE);
                break;
            case ID_ACCELF10:
                if (!inprocess)
                {
                    
                    //startBoundaryLine((int)(WIDTH / 2.), (int)(HEIGHT /9.), (int)(8 * WIDTH /9.), (int)(HEIGHT /2.));
                    //startBoundaryLine((int)(8 * WIDTH / 9.), (int)(HEIGHT / 2.), (int)(WIDTH / 2.), (int)(8 * HEIGHT / 9.));
                    //startBoundaryLine((int)(WIDTH / 2.), (int)(8 * HEIGHT / 9.), (int)(WIDTH / 9.), (int)(HEIGHT / 2.));
                    //startBoundaryLine((int)(WIDTH / 9.), (int)(HEIGHT / 2.), (int)(WIDTH / 2.), (int)(HEIGHT / 9.));

                    /*startBoundaryLine(WIDTH / 2., HEIGHT / 9., 8 * WIDTH / 9., HEIGHT / 2.);
                    //startBoundaryLine(8 * WIDTH / 9., HEIGHT / 2., WIDTH / 2., 8 * HEIGHT / 9.);
                    //startBoundaryLine(WIDTH / 2., 8 * HEIGHT / 9., WIDTH / 9., HEIGHT / 2.);
                    //startBoundaryLine(WIDTH / 9., HEIGHT / 2., WIDTH / 2., HEIGHT / 9.);

                    //startBoundaryLine(15.25, 5.25, 5.25, 15.25);
                    //startBoundaryLine(5.25, 15.25, 15.75, 25.75);

                    startBoundaryLine(0, 0, 30, 30);
                    //startBoundaryLine(1, 7, 7, 7);
                    //startBoundaryLine(7, 7, 1, 7);
                    //startBoundaryLine(7, 1, 7, 7);
                    //startBoundaryLine(7, 7, 7, 1);
                    //startBoundaryLine(1, 1, 7, 7 );
                    //startBoundaryLine(1, 7, 7, 1 );
                    //startBoundaryLine(1, 1, 7, 4 );
                    //startBoundaryLine(1, 1, 4, 7 );
                    //startBoundaryLine(1, 4, 7, 1 );
                    //startBoundaryLine(1, 7, 4, 1 );
                    //startBoundaryLine(7, 7, 1, 15);
                    //startBoundaryLine(7, 15, 1, 7);
                    
                    OutputDebugStringA(std::format("startBoundaryLine({}, {}, {}, {});\n",
                        x0, yy0, x1, yy1).c_str());
                    OutputDebugStringA(std::format("Next node {} {} {} intersect {} {}\n",
                        nextnode.x, nextnode.y, (int)nextnode.entry, xintersect, yintersect).c_str());*/
                    if (vertices.size() >= 2 && lineIx < vertices.size() - 1)
                    {
                        x0 = vertices[lineIx].x * WIDTH / 512.0; yy0 = HEIGHT - 0 - vertices[lineIx].y * HEIGHT / 512.0;
                        x1 = vertices[lineIx + 1].x * WIDTH / 512.0; yy1 = HEIGHT - 0 - vertices[lineIx + 1].y * HEIGHT / 512.0;
                        startBoundaryLine(x0, yy0, x1, yy1, lineIx<1);
                        ++lineIx;
                    }
                    else
                        lineIx = 0;
                }
                else
                {
                    //if (inprocess)
                    //{
                        inprocess = (0==continueBoundaryLine());
                    //}
                    OutputDebugStringA(std::format("  next node {} {} {} intersect {} {}\n",
                        nextnode.x, nextnode.y, (int)nextnode.entry, xintersect, yintersect).c_str());
                }
                InvalidateRect(hWnd, NULL, TRUE);
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
            BITMAPINFO info{};
            info.bmiHeader.biSize = sizeof(info.bmiHeader);
            info.bmiHeader.biPlanes = 1;
            info.bmiHeader.biBitCount = 32;
            info.bmiHeader.biCompression = BI_RGB;
            info.bmiHeader.biWidth = 512;
            info.bmiHeader.biHeight = 512;

            unsigned char* pixels = showroom(WIDTH, HEIGHT, fld);

            SetDIBitsToDevice(hdc, 0, 0, 512, 512, 0, 0, 0, 512,
                (void*)pixels, &info, DIB_RGB_COLORS);
            delete[] pixels;
            if (vertices.size() > 1)
            {
                HPEN hPen = CreatePen(PS_DOT, 1, RGB(0, 255, 0));
                SelectObject(hdc, hPen);
                MoveToEx(hdc, vertices[0].x, vertices[0].y, (LPPOINT)NULL);
                //LineTo(hdc, 512 / WIDTH * (WIDTH -1 ), 512 / HEIGHT * (HEIGHT - 1));
                PolylineTo(hdc, vertices.data(), vertices.size());
            }
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_LBUTTONDOWN:
        xPosDown = GET_X_LPARAM(lParam);
        yPosDown = GET_Y_LPARAM(lParam);
        break;
    case WM_LBUTTONUP:
        xPosUp = GET_X_LPARAM(lParam);
        yPosUp = GET_Y_LPARAM(lParam);
        if (wParam & MK_CONTROL)
        {
            vertices.clear();
            x0 = xPosDown * WIDTH / 512.0; yy0 = HEIGHT - 0 - yPosDown * HEIGHT / 512.0;
            POINT p0(xPosDown, yPosDown);
            vertices.push_back(p0);
            x1 = xPosUp * WIDTH / 512.0; yy1 = HEIGHT - 0 - yPosUp * HEIGHT / 512.0;
            POINT p1(xPosUp, yPosUp);
            vertices.push_back(p1);
            startBoundaryLine(x0, yy0, x1, yy1);
            OutputDebugStringA(std::format("startBoundaryLine({}, {}, {}, {});\n",
                x0, yy0, x1, yy1).c_str());
            OutputDebugStringA(std::format("Next node {} {} {} intersect {} {}\n",
                nextnode.x, nextnode.y, (int)nextnode.entry, xintersect, yintersect).c_str());
        }
        else if(wParam && MK_SHIFT)
        {
            vertices.clear();
            x0 = xPosDown * WIDTH / 512; yy0 = HEIGHT - 1 - yPosDown * HEIGHT / 512;
            POINT p0((xPosDown * WIDTH / 512) * 512 / WIDTH, (yPosDown * HEIGHT / 512 + 1) * 512 / HEIGHT);
            vertices.push_back(p0);
            x1 = xPosUp * WIDTH / 512; yy1 = HEIGHT - 1 - yPosUp * HEIGHT / 512;
            POINT p1((xPosUp * WIDTH / 512) * 512 / WIDTH, (yPosUp * HEIGHT / 512 + 1) * 512 / HEIGHT);
            vertices.push_back(p1);
            startBoundaryLine(x0, yy0, x1, yy1);
            OutputDebugStringA(std::format("startBoundaryLine({}, {}, {}, {});\n",
                x0, yy0, x1, yy1).c_str());
            OutputDebugStringA(std::format("Next node {} {} {} intersect {} {}\n",
                nextnode.x, nextnode.y, (int)nextnode.entry, xintersect, yintersect).c_str());
        }
        else
        {
            POINT p(xPosUp, yPosUp);
            vertices.push_back(p);
        }
        InvalidateRect(hWnd, NULL, TRUE);
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
