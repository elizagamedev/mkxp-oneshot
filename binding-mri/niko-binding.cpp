#include "journal-binding.h"
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

	#include <unistd.h>
#endif

#include <SDL.h>
namespace syswm {
#include <SDL_syswm.h>
}

#define NIKO_X (320 - 16)
#define NIKO_Y ((13 * 16) * 2)

RB_METHOD(nikoPrepare)
{
	// Prime native window info
	syswm::SDL_SysWMinfo syswindow;
	SDL_VERSION(&syswindow.version);
	SDL_GetWindowWMInfo(shState->rtData().window, &syswindow);

#ifdef LINUX
	char path[PATH_MAX];
	std::string journal;

	// Get current path
	getcwd(path, sizeof(path));

	#ifdef OS_OSX
		journal = std::string(path) + std::string("/_______.app/Contents/MacOS/_______");
	#else
		journal = std::string(path) + std::string("_______");
	#endif

	// Run the binary
	pid_t pid = fork();
	if (pid == 0) {
		execl(journal.c_str(), journal.c_str(), "niko", (char*)0);
		exit(1);
	}
#endif
	return Qnil;
}

RB_METHOD(nikoStart)
{
	RB_UNUSED_PARAM;

	// Prime native window info
	syswm::SDL_SysWMinfo syswindow;
	SDL_VERSION(&syswindow.version);
	SDL_GetWindowWMInfo(shState->rtData().window, &syswindow);

#ifdef _WIN32
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
#else
	// Calculate where to stick the window
	int x, y; // Top-left area of client (hopefully)
	SDL_GetWindowPosition(shState->rtData().window, &x, &y);
	x += NIKO_X;
	y += NIKO_Y;
	char message[32];
	sprintf(message, "%d,%d", x, y);

	SDL_LockMutex(mutex);
	message_len = strlen(message);
	strcpy((char*)message_buffer, message);
	SDL_UnlockMutex(mutex);

	// Clean up connection thread
	if (thread != NULL && out_pipe != -1) {
		SDL_WaitThread(thread, NULL);
		thread = NULL;
	}
	// Attempt to send it over the tubes
	if (out_pipe != -1) {
		// We have a connection, so send it over
		if (write(out_pipe, (char*)message_buffer, message_len) <= 0) {
			// In the case of an error, close
			close(out_pipe);
			out_pipe = -1;
		}
	}
	if (out_pipe == -1) {
		// We don't have a pipe open, so spawn the connection thread
		thread = SDL_CreateThread(server_thread, "journal", NULL);
	}
#endif
	return Qnil;
}

void nikoBindingInit()
{
	VALUE module = rb_define_module("Niko");

	//Functions
	_rb_define_module_function(module, "get_ready", nikoPrepare);
	_rb_define_module_function(module, "do_your_thing", nikoStart);
}
