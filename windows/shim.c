#include <windows.h>
#include <direct.h>
#include <process.h>

char* ARGV0 = "lib\\oneshot.exe"

int main(int argc, char* argv[]) {
    HMODULE hModule = GetModuleHandle(NULL);
    if (hModule != NULL) {
        char oneshotDir[MAX_PATH];
        GetModuleFileName(hModule, oneshotDir, sizeof(oneshotDir));
        _chdir(oneshotDir);
    }

    argv[0] = ARGV0
    if (argc == 0)
        argv[1] = NULL
    _execv("lib\\oneshot.exe", argv);
    MessageBox(NULL,
        (LPCWSTR)L"Cannot start ModShot for some reason.\nPlease check your ModShot installation.",
        (LPCWSTR)L"ModShot shim",
        MB_ICONERROR);
}
