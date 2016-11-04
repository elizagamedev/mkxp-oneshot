#include "binding-util.h"
#include "binding-types.h"
#include "sharedstate.h"
#include "eventthread.h"
#include "debugwriter.h"

#ifdef _WIN32
#include <shlwapi.h>
#endif

#include <SDL.h>
#include <SDL_syswm.h>

RB_METHOD(nikoStart)
{
	RB_UNUSED_PARAM;
	// Calculate where to stick the window
	POINT pos;
	pos.x = 320 - 16;
	pos.y = (13 * 16) * 2;
	SDL_SysWMinfo syswindow;
	SDL_VERSION(&syswindow.version);
	SDL_GetWindowWMInfo(shState->rtData().window, &syswindow);
	ClientToScreen(syswindow.info.win.window, &pos);
	// Start process
#ifdef _WIN32
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
#else
#ifdef linux
#else
#endif
#endif
	return Qnil;
}

void nikoBindingInit()
{
	VALUE module = rb_define_module("Niko");

	//Functions
	_rb_define_module_function(module, "do_your_thing", nikoStart);
}
