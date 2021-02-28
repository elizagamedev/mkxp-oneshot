#include <windows.h>
#include <wchar.h>

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
        _wchdir(oneshotDir);
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
