#include "etc.h"
#include "binding-util.h"
#include "binding-types.h"
#include "debugwriter.h"
#include "config.h"
#include "sharedstate.h"
#include "pipe.h"

static Pipe ipc;

static void start()
{
	ipc.open("oneshot-pipe", Pipe::Write);

	// Create process
#if defined _WIN32
	WCHAR path[MAX_PATH];
	WCHAR gameFolder[MAX_PATH];
	MultiByteToWideChar(CP_UTF8, 0, shState->config().gameFolder.c_str(), -1, gameFolder, MAX_PATH);
	GetModuleFileNameW(NULL, path, MAX_PATH);
	STARTUPINFOW si;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi;
	std::wstring argString = std::wstring(L"oneshot.exe \"--gameFolder=") + gameFolder + L"\" --screenMode=true";
	WCHAR *args = new WCHAR[argString.size() + 1];
	memcpy(args, argString.c_str(), (argString.size() + 1) * sizeof(WCHAR));
	CreateProcessW(path, args, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	delete [] args;
#else
#if defined __linux
	char path[PATH_MAX];
	ssize_t len = readlink("/proc/self/exe", path, PATH_MAX);
	if (len == -1)
		rb_raise(rb_eRuntimeError, "Cannot determine path of running executable");
	std::string exename = std::string(path, len);
#else
#endif

	pid_t pid = fork();
	if (pid == 0) {
		execl(exename.c_str(), "oneshot",
		      (std::string("--gameFolder=") + shState->config().gameFolder).c_str(),
		      "--screenMode=true", NULL);
		exit(1);
	}
#endif

	ipc.connect();
}

RB_METHOD(screenStart)
{
	RB_UNUSED_PARAM;
	start();
	return Qnil;
}

RB_METHOD(screenFinish)
{
	RB_UNUSED_PARAM;
	ipc.write("END", sizeof("END"));
	ipc.close();
	return Qnil;
}

RB_METHOD(screenSet)
{
	RB_UNUSED_PARAM;
	const char *imageName;
	rb_get_args(argc, argv, "z", &imageName RB_ARG_END);
	if (!ipc.isOpen())
		start();
	ipc.write(imageName, strlen(imageName) + 1);
	return Qnil;
}

void screenBindingInit()
{
    VALUE module = rb_define_module("Screen");
    _rb_define_module_function(module, "start", screenStart);
    _rb_define_module_function(module, "finish", screenFinish);
	_rb_define_module_function(module, "set", screenSet);
}
