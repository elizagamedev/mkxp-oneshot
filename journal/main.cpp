#include <windows.h>

extern int do_niko(int x, int y);
extern int do_journal();

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    (void)hInstance;
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;

    int argc;
    LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    // argc == 3, assume niko
    if (argc == 3) {
        int x = _wtoi(argv[1]);
        int y = _wtoi(argv[2]);
        return do_niko(x, y);
    }
    // assume journal
    return do_journal();
}
