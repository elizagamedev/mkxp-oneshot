#include <windows.h>
#include <wchar.h>
#include <errno.h>
#include <stdio.h>

const wchar_t *ARGV0 = "lib\\oneshot.exe";

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow) {
    (void)hInstance;
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;

    int argc;
    LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    HMODULE hModule = GetModuleHandle(NULL);
    if (hModule != NULL) {
        wchar_t *oneshotDir[MAX_PATH];
        GetModuleFileNameW(hModule, oneshotDir, sizeof(oneshotDir));
        wchar_t *pathend = wcsrchr(oneshotDir, L'\\');
        if(pathend != NULL) {
            *pathend = L'\0';
        }
        if(_wchdir(oneshotDir)) {
            MessageBoxW(NULL,
                L"Changing working directory failed. This should never happen.\nFind rkevin and beat him with a stick.",
                L"ModShot Shim",
                MB_ICONERROR);
            printf("chdir errno: %d", errno);
        }
    }

    if (argc != 0) {
        argv[0] = ARGV0;
    }

    _wexecv(L"lib\\oneshot.exe", argv);
    MessageBoxW(NULL,
        L"Cannot start ModShot for some reason.\nPlease check your ModShot installation.",
        L"ModShot Shim",
        MB_ICONERROR);
    return 1;
}
