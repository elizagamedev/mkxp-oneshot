#include <windows.h>

static const WCHAR niko_classname[] = L"niko";
static HWND niko = NULL;
static HBITMAP niko_bmps[3] = {NULL};
static int niko_offset = 0;

// Window procedure
LRESULT CALLBACK niko_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_CLOSE:
        return 0;

    case WM_PAINT: {
        static int frame = 0;
        if (niko_offset % 32 >= 16) {
            frame = 1;
        } else {
            if ((niko_offset / 32) % 2)
                frame = 0;
            else
                frame = 2;
        }

        PAINTSTRUCT ps;
        HDC hdc;
        BITMAP bitmap;
        HDC hdcMem;
        HGDIOBJ oldBitmap;

        hdc = BeginPaint(hwnd, &ps);

        hdcMem = CreateCompatibleDC(hdc);
        oldBitmap = SelectObject(hdcMem, niko_bmps[frame]);

        GetObject(niko_bmps[frame], sizeof(bitmap), &bitmap);
        BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

        SelectObject(hdcMem, oldBitmap);
        DeleteDC(hdcMem);

        EndPaint(hwnd, &ps);
    }
    break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int do_niko(int x, int y)
{
    // Create
    HINSTANCE module = GetModuleHandleW(NULL);
    niko_bmps[0] = LoadBitmapW(module, L"NIKO1");
    niko_bmps[1] = LoadBitmapW(module, L"NIKO2");
    niko_bmps[2] = LoadBitmapW(module, L"NIKO3");

    WNDCLASSEXW wc;
    wc.cbSize           = sizeof(wc);
    wc.style            = 0;
    wc.lpfnWndProc      = niko_proc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = module;
    wc.hIcon            = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = NULL;
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = niko_classname;
    wc.hIconSm          = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
        return 1;

    if (!(niko = CreateWindowExW(WS_EX_COMPOSITED | WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
                                 niko_classname,
                                 L"Niko",
                                 WS_POPUP,
                                 x, y,
                                 24 * 2, 32 * 2,
                                 NULL, NULL, module, NULL))) {
        return 1;
    }

    SetLayeredWindowAttributes(niko, RGB(0, 255, 0), 0, LWA_COLORKEY);

    ShowWindow(niko, SW_SHOWNOACTIVATE);
    UpdateWindow(niko);

    // Thread
    int screenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    // Get starting position
    RECT area;
    GetWindowRect(niko, &area);

    // Just keep walking down
    for (;;) {
        // Dispatch messages
        unsigned long tick = GetTickCount();
        for (;;) {
            MSG msg;
            if (PeekMessage(&msg, niko, 0, 0, PM_REMOVE) > 0) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            } else {
                if (GetTickCount() - tick >= 1000 / 60)
                    break;
            }
        }

        niko_offset += 2;
        if (area.top + niko_offset >= screenHeight)
            break;
        SetWindowPos(niko, NULL, area.left, area.top + niko_offset, 0, 0, SWP_NOSIZE);
        InvalidateRect(niko, NULL, FALSE);
    }

    DestroyWindow(niko);

    for (int i = 0; i < 3; i++) {
        if (niko_bmps[i])
            DeleteObject(niko_bmps[i]);
    }

    return 0;
}
