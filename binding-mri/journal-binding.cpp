#include "binding-util.h"
#include "binding-types.h"
#include "pipe.h"
#include "debugwriter.h"
#include "i18n.h"

#include <SDL.h>

//OS-Specific code
#if defined _WIN32
	#define OS_W32
#elif defined __APPLE__ || __linux__
	#define LINUX
	#ifdef __APPLE__
		#define OS_OSX
	#else
		#define OS_LINUX
	#endif

	#include <fcntl.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#ifdef OS_LINUX
		#include <sys/inotify.h>
	#endif
	#include <unistd.h>
	#include <stdio.h>
#else
    #error "Operating system not detected."
#endif

#define BUFFER_SIZE 256

static SDL_Thread *thread = NULL;
static SDL_mutex *mutex = NULL;
static volatile char lang_buffer[BUFFER_SIZE];
static volatile char message_buffer[BUFFER_SIZE];
static volatile bool active = false;
static volatile int message_len = 0;

#ifdef LINUX
	#define PIPE_PATH "/tmp/oneshot-pipe"
	static volatile int out_pipe = -1;
	void cleanup_pipe()
	{
		unlink(PIPE_PATH);
		remove(PIPE_PATH);
	}
#endif

int server_thread(void *data)
{
	(void)data;
#if defined OS_W32
	HANDLE pipe = CreateNamedPipeW(L"\\\\.\\pipe\\oneshot-journal-to-game",
	                               PIPE_ACCESS_OUTBOUND,
	                               PIPE_TYPE_BYTE | PIPE_WAIT,
	                               PIPE_UNLIMITED_INSTANCES,
	                               BUFFER_SIZE,
	                               BUFFER_SIZE,
	                               0,
	                               NULL);
	for (;;) {
		ConnectNamedPipe(pipe, NULL);
		SDL_LockMutex(mutex);
		DWORD written;
		WriteFile(pipe, (const void*)message_buffer, BUFFER_SIZE, &written, NULL);
		active = true;
		SDL_UnlockMutex(mutex);
		FlushFileBuffers(pipe);
		DisconnectNamedPipe(pipe);
	}
	CloseHandle(pipe);
#else
	if (FILE *file = fopen(PIPE_PATH, "r")) {
		fclose(file);
		out_pipe = open(PIPE_PATH, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
		SDL_LockMutex(mutex);
		active = true;
		if (message_len > 0)
			write(out_pipe, (char*)message_buffer, message_len);
		SDL_UnlockMutex(mutex);
	}
	return 0;
#endif
}

RB_METHOD(journalSet)
{
	RB_UNUSED_PARAM;
	const char *name;
	rb_get_args(argc, argv, "z", &name RB_ARG_END);
	// Record message
	SDL_LockMutex(mutex);
	message_len = strlen(name);
	strcpy((char*)message_buffer, name);
	if (message_len > 0) {
		// in the case where journal is being sent empty string
		// do not append the language suffix, because empty string
		// is the signifier to terminate the journal
		strcpy((char*)message_buffer + message_len, (char*)lang_buffer);
		message_len += strlen((char*)lang_buffer);
	}
	SDL_UnlockMutex(mutex);

#if defined _WIN32
	HANDLE pipe = CreateFileW(L"\\\\.\\pipe\\oneshot-game-to-journal",
	                          GENERIC_WRITE,
	                          0,
	                          NULL,
	                          OPEN_EXISTING,
	                          0,
	                          NULL);
	if (pipe != INVALID_HANDLE_VALUE) {
		active = true;
		DWORD written;
		WriteFile(pipe, (const void*)message_buffer, BUFFER_SIZE, &written, NULL);
		FlushFileBuffers(pipe);
		CloseHandle(pipe);
	}
	if (thread == NULL) {
		thread = SDL_CreateThread(server_thread, "journal", NULL);
	}
#else
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

RB_METHOD(journalSetLang)
{
	RB_UNUSED_PARAM;
	const char *lang;
	rb_get_args(argc, argv, "z", &lang RB_ARG_END);
	strcpy((char*)lang_buffer+1, lang);
	loadLocale(lang);
	return Qnil;
}

RB_METHOD(journalActive)
{
	RB_UNUSED_PARAM;
	return active ? Qtrue : Qfalse;
}

void journalBindingInit()
{
	mutex = SDL_CreateMutex();
	memset((char*)lang_buffer, 0, BUFFER_SIZE);
	lang_buffer[0] = '_';
#if defined __linux
	mkfifo(PIPE_PATH, 0666);
	atexit(cleanup_pipe);
#endif

	VALUE module = rb_define_module("Journal");
	_rb_define_module_function(module, "set", journalSet);
	_rb_define_module_function(module, "active?", journalActive);
	_rb_define_module_function(module, "setLang", journalSetLang);
}
