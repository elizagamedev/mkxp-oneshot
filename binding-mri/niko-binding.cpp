#include "binding-util.h"
#include "binding-types.h"
#include "sharedstate.h"
#include "eventthread.h"
#include "debugwriter.h"

#if defined _WIN32
#include <shlwapi.h>
#elif defined __APPLE__ || __linux__
	#define LINUX
	#ifdef __APPLE__
		#define OS_OSX
	#else
		#define OS_LINUX
	#endif
#endif

#include <SDL.h>
namespace syswm {
#include <SDL_syswm.h>
}

#define NIKO_X (320 - 16)
#define NIKO_Y ((13 * 16) * 2)

RB_METHOD(nikoStart)
{
	RB_UNUSED_PARAM;

	// Prime native window info
	syswm::SDL_SysWMinfo syswindow;
	SDL_VERSION(&syswindow.version);
	SDL_GetWindowWMInfo(shState->rtData().window, &syswindow);

#if defined _WIN32
	// Calculate where to stick the window
	POINT pos;
	pos.x = NIKO_X;
	pos.y = NIKO_Y;
	ClientToScreen(syswindow.info.win.window, &pos);
	// Start process
	WCHAR path[MAX_PATH];
	WCHAR args[MAX_PATH];
	GetModuleFileNameW(NULL, path, MAX_PATH);
	PathRemoveFileSpecW(path);
	wcscat(path, L"\\_______.exe");
	wsprintfW(args, L"_______.exe %d %d", pos.x, pos.y);
	STARTUPINFOW si;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi;
	CreateProcessW(path, args, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
#elif defined LINUX
	// Calculate where to stick the window
	int x, y; // Top-left area of client (hopefully)
	SDL_GetWindowPosition(shState->rtData().window, &x, &y);
	x += NIKO_X;
	y += NIKO_Y;
	char path[PATH_MAX];
	ssize_t len = readlink("/proc/self/exe", path, PATH_MAX);
	if (len > 0) {
		// Replace filename
		for (size_t i = len; i > 0; --i) {
			if (path[i-1] == '/') {
				strcpy(path + i, "_______");
				break;
			}
		}
		// X and Y string arguments
		char x_str[16];
		char y_str[16];
		sprintf(x_str, "%d", x);
		sprintf(y_str, "%d", y);
		// Assume that was a success and run the binary
		pid_t pid = fork();
		if (pid == 0) {
			execl(path, "_______", x_str, y_str, NULL);
			exit(1);
		}
	}
#else
#error "not yet implemented"
#endif
	return Qnil;
}

void nikoBindingInit()
{
	VALUE module = rb_define_module("Niko");

	//Functions
	_rb_define_module_function(module, "do_your_thing", nikoStart);
}
