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
