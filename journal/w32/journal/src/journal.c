#define WINVER 0x0600
#define _WIN32_WINNT 0x0600
// needed to suppress warnings about wcscpy and such
#define _CRT_SECURE_NO_WARNINGS 1

#include <windows.h>
#include <stdio.h>
#include <objbase.h>
#include <shlobj.h>

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600
#define IN_BUFFER_SIZE 256
#define IMAGE_PATH_MAX_SIZE 512

#define WINDOW_STYLE (WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX))

static const WCHAR journal_classname[] = L"journal";

static HBITMAP image_handle = NULL;
static HANDLE image_mutex = NULL;

static DWORD err = 0;

static HWND window = NULL;

static char DataPath[512];

static void loadImage(const char *name) {
    char imagePath[IMAGE_PATH_MAX_SIZE];

    if (image_handle) {
        DeleteObject(image_handle);
    }

    // handle default as a special case, since we'll keep that image internally
    if (strcmp(name, "default") == 0) {
        image_handle = LoadBitmapA(GetModuleHandleW(NULL), "default");
    }
    else {
        // names come in the format XX_YY
        // where XX is the image (ex: c1)
        // and YY is the lang code (ex: ja, or pt_br)
        // so let's separate this into 2 char pointers
        char imageName[IN_BUFFER_SIZE];
        char* langCode = 0;
        char* emptyString = "\0";
        strcpy(imageName, name);
        langCode = strchr(imageName, '_');
        if (langCode > 0) {
            // replace the code with 0 to null terminate the image name where this is
            // then use the rest of the string as the language code
            // so c1_ja becomes "c1\0ja", thus imageName is "c1" and then langCode is "ja"
            (*langCode) = 0;
            langCode++;
        }
        else {
            langCode = emptyString;
        }

        // build imagePath
        strcpy(imagePath, DataPath);
        strcat(imagePath, "Graphics\\Journal\\");
        strcat(imagePath, langCode);
        strcat(imagePath, "\\");
        strcat(imagePath, imageName);
        strcat(imagePath, ".bmp");

        image_handle = LoadImageA(NULL, imagePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        // In the event we can't find an image's translated value
        // try to find a matching file in the root folder
        if (!image_handle) {
            // build imagePath
            strcpy(imagePath, DataPath);
            strcat(imagePath, "Graphics\\Journal\\");
            strcat(imagePath, imageName);
            strcat(imagePath, ".bmp");
            image_handle = LoadImageA(NULL, imagePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            if (!image_handle) {
                err = GetLastError();
            }
        }
    }
}

// Window procedure
LRESULT CALLBACK journal_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc;
        HDC hdcMem;

        WaitForSingleObject(image_mutex, INFINITE);
        hdc = BeginPaint(hwnd, &ps);
        hdcMem = CreateCompatibleDC(hdc);
        HANDLE oldBitmap = SelectObject(hdcMem, image_handle);
        BitBlt(hdc, 0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT, hdcMem, 0, 0, SRCCOPY);
        SelectObject(hdcMem, oldBitmap);
        DeleteDC(hdcMem);
        EndPaint(hwnd, &ps);
        ReleaseMutex(image_mutex);
    } break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// IPC thread
DWORD WINAPI ipc_thread(LPVOID lpParam)
{
    (void)lpParam;

    char message[IN_BUFFER_SIZE];
    DWORD len;

    HANDLE pipe = CreateNamedPipeW(L"\\\\.\\pipe\\oneshot-game-to-journal",
                                   PIPE_ACCESS_INBOUND,
                                   PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
                                   PIPE_UNLIMITED_INSTANCES,
                                   IN_BUFFER_SIZE,
                                   IN_BUFFER_SIZE,
                                   0,
                                   NULL);

    for (;;) {
        if (!ConnectNamedPipe(pipe, NULL))
            continue;
        for (;;) {
            if (ReadFile(pipe, (void*)message, IN_BUFFER_SIZE, &len, NULL)
                    && len == IN_BUFFER_SIZE)
            {
                WaitForSingleObject(image_mutex, INFINITE);
                if (*message == 0) {
                    exit(0);
                }
                loadImage(message);
                InvalidateRect(window, NULL, FALSE);
                ReleaseMutex(image_mutex);
            } else {
                break;
            }
        }
        DisconnectNamedPipe(pipe);
    }

    CloseHandle(pipe);

    return 0;
}

// check if the file "My Documents/My Games/Oneshot/save_progress.oneshot" exist
// if not, load the "default" image background
// if it does exist, try to read the last 8.. or so.. bytes from the file.
// this (should) contain a string like [XX_XX] or [XX] indicating the user's
// language code. This will be apended with an underscore to "save"
// to give the localized image file IFF the lang str exists
void init_check_save(WCHAR* save_path) {
  FILE* savefile = _wfopen(save_path, L"rb");
  char imgfile[16] = {0};
  char langbuf[17] = {0};
  char* openbrace = 0;
  char* closebrace = 0;
  if (savefile) {
    strcpy(imgfile, "save");
    fseek(savefile, -16, SEEK_END);
    fread(langbuf, 1, 16, savefile);
    fclose(savefile);

    openbrace = strchr(langbuf, '[');
    closebrace = strchr(langbuf, ']');
    if (openbrace && closebrace && openbrace < closebrace) {
      //we very probably have a language code here.
      //so, lets do some hacky string manipulation to
      //name our image file
      imgfile[4] = '_';
      strncpy(imgfile+5, openbrace+1, closebrace-(openbrace+1));
    }

    loadImage(imgfile);
  } else {
    loadImage("default");
  }
}

// images should be loaded from gamepath folder
// so we need a gamepath, which should be saved in the registry
boolean readGamePath() {
    // clear data path
    memset(&DataPath[0], 0, sizeof(DataPath));
    DWORD dataPathSize = sizeof(DataPath);
    
    HKEY key;
    long keyOpenError = RegOpenKey(HKEY_CURRENT_USER, TEXT("Software\\OneShot\\"), &key);

    if (keyOpenError == ERROR_SUCCESS) {
        long dirReadError = RegQueryValueEx(key, TEXT("GameDirectory"), NULL, NULL, (LPBYTE)DataPath, &dataPathSize);

        RegCloseKey(key);

        return dirReadError == ERROR_SUCCESS;
    }
    return FALSE;
}

int do_journal()
{
  // Create
  HINSTANCE module = GetModuleHandleW(NULL);

  // load data path from registry
  readGamePath();

	// Default image
	WCHAR save_path[MAX_PATH];
  PWSTR folder_path = NULL;
	SHGetKnownFolderPath((REFKNOWNFOLDERID)&FOLDERID_Documents, 0, NULL, &folder_path);
  wcscpy(save_path, folder_path);
	wcscat(save_path, L"\\My Games\\Oneshot\\save_progress.oneshot");
  CoTaskMemFree(folder_path);
  init_check_save(save_path);

    // Initial image
    char message[IN_BUFFER_SIZE];
    DWORD len;
    HANDLE pipe = CreateFileW(L"\\\\.\\pipe\\oneshot-journal-to-game",
	                          GENERIC_READ,
	                          0,
	                          NULL,
	                          OPEN_EXISTING,
	                          0,
	                          NULL);
    if (pipe != INVALID_HANDLE_VALUE) {
        if (ReadFile(pipe, (void*)message, IN_BUFFER_SIZE, &len, NULL)
                && len == IN_BUFFER_SIZE)
        {
            if (*message)
                loadImage(message);
        }
        CloseHandle(pipe);
    }

    WNDCLASSEXW wc;
    wc.cbSize           = sizeof(wc);
    wc.style            = 0;
    wc.lpfnWndProc      = journal_proc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = module;
    wc.hIcon            = LoadIcon(NULL, L"MAINICON");
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = NULL;
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = journal_classname;
    wc.hIconSm          = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
        return 1;

    RECT winrect;
    winrect.left = 0;
    winrect.top = 0;
    winrect.right = DEFAULT_WIDTH;
    winrect.bottom = DEFAULT_HEIGHT;
    AdjustWindowRectEx(&winrect, WINDOW_STYLE, FALSE, 0);

    if (!(window = CreateWindowExW(WS_EX_COMPOSITED | WS_EX_LAYERED,
                                   journal_classname,
                                   L"",
                                   WINDOW_STYLE,
                                   CW_USEDEFAULT, CW_USEDEFAULT,
                                   winrect.right - winrect.left, winrect.bottom - winrect.top,
                                   NULL, NULL, module, NULL))) {
        return 1;
    }

    SetLayeredWindowAttributes(window, RGB(0, 255, 0), 0, LWA_COLORKEY);

    ShowWindow(window, SW_SHOWNOACTIVATE);
    UpdateWindow(window);

    image_mutex = CreateMutexW(NULL, FALSE, NULL);
    HANDLE thread = CreateThread(NULL, 0, ipc_thread, NULL, 0, NULL);

    // Dispatch messages
    for (;;) {
        MSG msg;
        if (GetMessage(&msg, window, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            break;
        }
    }

    // TODO make this less messy?
    TerminateThread(thread, 0);

    DestroyWindow(window);

    return 0;
}
