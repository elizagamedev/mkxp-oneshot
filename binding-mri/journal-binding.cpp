#include "binding-util.h"
#include "binding-types.h"
#include "pipe.h"
#include "debugwriter.h"

#include <SDL.h>

#define OUT_BUFFER_SIZE 256

static SDL_Thread *thread = NULL;
static SDL_mutex *mutex = NULL;
static volatile char message_buffer[OUT_BUFFER_SIZE];
static volatile int message_len = 0;

int server_thread(void *data)
{
	(void)data;
	HANDLE pipe = CreateNamedPipeW(L"\\\\.\\pipe\\oneshot-journal-to-game",
	                               PIPE_ACCESS_OUTBOUND,
	                               PIPE_TYPE_BYTE | PIPE_WAIT,
	                               PIPE_UNLIMITED_INSTANCES,
	                               OUT_BUFFER_SIZE,
	                               OUT_BUFFER_SIZE,
	                               0,
	                               NULL);
	for (;;) {
		ConnectNamedPipe(pipe, NULL);
		SDL_LockMutex(mutex);
		WriteFile(pipe, (const void*)message_buffer, OUT_BUFFER_SIZE, NULL, NULL);
		SDL_UnlockMutex(mutex);
		FlushFileBuffers(pipe);
		DisconnectNamedPipe(pipe);
	}

	CloseHandle(pipe);

	return 0;
}

RB_METHOD(journalSet)
{
	RB_UNUSED_PARAM;
	const char *name;
	int len;
	rb_get_args(argc, argv, "s", &name, &len RB_ARG_END);
#ifdef _WIN32
	if (thread == NULL) {
		thread = SDL_CreateThread(server_thread, "journal", NULL);
	}
	HANDLE pipe = CreateFileW(L"\\\\.\\pipe\\oneshot-game-to-journal",
	                          GENERIC_WRITE,
	                          0,
	                          NULL,
	                          OPEN_EXISTING,
	                          0,
	                          NULL);
	SDL_LockMutex(mutex);
	strcpy((char*)message_buffer, name);
	message_len = len;
	SDL_UnlockMutex(mutex);
	if (pipe != INVALID_HANDLE_VALUE) {
		WriteFile(pipe, (const void*)message_buffer, OUT_BUFFER_SIZE, NULL, NULL);
		FlushFileBuffers(pipe);
		CloseHandle(pipe);
	}
#endif
	return Qnil;
}

void journalBindingInit()
{
	mutex = SDL_CreateMutex();

	VALUE module = rb_define_module("Journal");
	_rb_define_module_function(module, "set", journalSet);
}
