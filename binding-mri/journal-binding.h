#pragma once

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
	void cleanup_pipe();
#endif

int server_thread(void *data);
