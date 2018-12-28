#include "oneshot.h"

/******************
 * HERE BE DRAGONS
 ******************/

#include "eventthread.h"
#include "debugwriter.h"
#include "bitmap.h"
#include "font.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

// OS-Specific code
#if defined _WIN32
	#define OS_W32
	#define WIN32_LEAN_AND_MEAN
	#define SECURITY_WIN32
	#include <windows.h>
	#include <mmsystem.h>
	#include <security.h>
	#include <shlobj.h>
	#include <SDL2/SDL_syswm.h>
#elif defined __APPLE__ || __linux__
	#include <stdlib.h>
	#include <unistd.h>
	#include <pwd.h>
	#include <dlfcn.h>

	#ifdef __APPLE__
		#define OS_OSX
		#include <dispatch/dispatch.h>
	#else
		#define OS_LINUX
		#include <gtk/gtk.h>
		#include <gdk/gdk.h>
	#endif
#else
	#error "Operating system not detected."
#endif

#define DEF_SCREEN_W 640
#define DEF_SCREEN_H 480

struct OneshotPrivate
{
	// Main SDL window
	SDL_Window *window;

	// String data
	std::string os;
	std::string lang;
	std::string userName;
	std::string savePath;
	std::string docsPath;
	std::string gamePath;
	std::string journal;

	// Dialog text
	std::string txtYes;
	std::string txtNo;

	bool exiting;
	bool allowExit;

	// Alpha texture data for portions of window obscured by screen edges
	int winX, winY;
	SDL_mutex *winMutex;
	bool winPosChanged;
	std::vector<uint8_t> obscuredMap;
	bool obscuredCleared;

	OneshotPrivate()
		: window(0),
	      winMutex(SDL_CreateMutex())
	{
	}

	~OneshotPrivate()
	{
		SDL_DestroyMutex(winMutex);
	}
};

//OS-SPECIFIC FUNCTIONS
#if defined OS_LINUX
struct linux_DialogData
{
	// Input
	int type;
	const char *body;
	const char *title;

	// Output
	bool result;
};

static int linux_dialog(void *rawData)
{
	linux_DialogData *data = reinterpret_cast<linux_DialogData*>(rawData);

	// Determine correct flags
	GtkMessageType gtktype;
	GtkButtonsType gtkbuttons = GTK_BUTTONS_OK;
	switch (data->type)
	{
		case Oneshot::MSG_INFO:
			gtktype = GTK_MESSAGE_INFO;
			break;
		case Oneshot::MSG_YESNO:
			gtktype = GTK_MESSAGE_QUESTION;
			gtkbuttons = GTK_BUTTONS_YES_NO;
			break;
		case Oneshot::MSG_WARN:
			gtktype = GTK_MESSAGE_WARNING;
			break;
		case Oneshot::MSG_ERR:
			gtktype = GTK_MESSAGE_ERROR;
			break;
		default:
			gtk_main_quit();
			return 0;
	}

	// Display dialog and get result
	GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, gtktype, gtkbuttons, "%s", data->body);
	gtk_window_set_title(GTK_WINDOW(dialog), data->title);
	int result = gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);

	// Interpret result and return
	data->result = (result == GTK_RESPONSE_OK || result == GTK_RESPONSE_YES);
	gtk_main_quit();
	return 0;
}
#elif defined OS_W32
/* Convert WCHAR pointer to std::string */
static std::string w32_fromWide(const WCHAR *ustr)
{
	std::string result;
	int size = WideCharToMultiByte(CP_UTF8, 0, ustr, -1, 0, 0, 0, 0);
	if (size > 0)
	{
		CHAR *str = new CHAR[size];
		if (WideCharToMultiByte(CP_UTF8, 0, ustr, -1, str, size, 0, 0) == size)
			result = str;
		delete [] str;
	}
	return result;
}
/* Convert WCHAR pointer from const char* */
static WCHAR *w32_toWide(const char *str)
{
	if (str)
	{
		int size = MultiByteToWideChar(CP_UTF8, 0, str, -1, 0, 0);
		if (size > 0)
		{
			WCHAR *ustr = new WCHAR[size];
			if (MultiByteToWideChar(CP_UTF8, 0, str, -1, ustr, size) == size)
				return ustr;
			delete [] ustr;
		}
	}

	//Return empty string
	WCHAR *ustr = new WCHAR[1];
	*ustr = 0;
	return ustr;
}
#endif

Oneshot::Oneshot(RGSSThreadData &threadData) :
    threadData(threadData)
{
	p = new OneshotPrivate();
	p->window = threadData.window;
	p->savePath = threadData.config.commonDataPath.substr(0, threadData.config.commonDataPath.size() - 1);
	p->obscuredMap.resize(640 * 480, 255);
	obscuredDirty = true;
	p->winX = 0;
	p->winY = 0;
	p->winPosChanged = false;
	p->allowExit = true;
	p->exiting = false;
	#ifdef OS_W32
		p->os = "windows";
	#elif defined OS_OSX
		p->os = "macos";
	#else
		p->os = "linux";
	#endif

	/********************
	 * USERNAME/DOCS PATH
	 ********************/
#if defined OS_W32
	//Get language code
	WCHAR wlang[9];
	GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_SISO639LANGNAME, wlang, sizeof(wlang) / sizeof(WCHAR));
	p->lang = w32_fromWide(wlang) + "_";
	GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_SISO3166CTRYNAME, wlang, sizeof(wlang) / sizeof(WCHAR));
	p->lang += w32_fromWide(wlang);

	//Get user's name
	ULONG size = 0;
	GetUserNameEx(NameDisplay, 0, &size);
	if (GetLastError() == ERROR_MORE_DATA)
	{
		//Get their full (display) name
		WCHAR *name = new WCHAR[size];
		GetUserNameEx(NameDisplay, name, &size);
		p->userName = w32_fromWide(name);
		delete [] name;
	}
	else
	{
		//Get their login name
		DWORD size2 = 0;
		GetUserName(0, &size2);
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			WCHAR *name = new WCHAR[size2];
			GetUserName(name, &size2);
			p->userName = w32_fromWide(name);
			delete [] name;
		}
	}

	// Get documents path
	WCHAR path[MAX_PATH];
	SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, path);
	p->docsPath = w32_fromWide(path);
	p->gamePath = p->docsPath+"\\My Games";
	p->journal = "_______.exe";
#else
	// Get language code
	const char *lc_all = getenv("LC_ALL");
	const char *lang = getenv("LANG");
	const char *code = (lc_all ? lc_all : lang);
	if (code)
	{
		// find first dot, copy language code
		int end = 0;
		for (; code[end] && code[end] != '.'; ++end) {}
		p->lang = std::string(code, end);
	}
	else
		p->lang = "en";

	// Get user's name
	#ifdef OS_OSX
		struct passwd *pwd = getpwuid(geteuid());
	#elif defined OS_LINUX
		struct passwd *pwd = getpwuid(getuid());
	#endif
	if (pwd)
	{
		if (pwd->pw_gecos && pwd->pw_gecos[0] && pwd->pw_gecos[0] != ',')
		{
			//Get the user's full name
			int comma = 0;
			for (; pwd->pw_gecos[comma] && pwd->pw_gecos[comma] != ','; ++comma) {}
			p->userName = std::string(pwd->pw_gecos, comma);
		}
		else
			p->userName = pwd->pw_name;
	}

	// Get documents path
	std::string path = std::string(getenv("HOME")) + "/Documents";
	p->docsPath = path.c_str();
	p->gamePath = path.c_str();
	#ifdef OS_OSX
		p->journal = "_______.app";
	#elif defined OS_LINUX
		p->journal = "_______";
	#endif
#endif

#ifdef OS_LINUX
	char const* xdg_current_desktop = getenv("XDG_CURRENT_DESKTOP");
	if (xdg_current_desktop == NULL) {
		desktopEnv = "nope";
	} else {
		std::string desktop(xdg_current_desktop);
		std::transform(desktop.begin(), desktop.end(), desktop.begin(), ::tolower);
		if (desktop.find("cinnamon") != std::string::npos) {
			desktopEnv = "cinnamon";
			gtk_init(0, 0);
		} else if (
			desktop.find("gnome") != std::string::npos ||
			desktop.find("unity") != std::string::npos
		) {
			desktopEnv = "gnome";
			gtk_init(0, 0);
		} else if (desktop.find("mate") != std::string::npos) {
			desktopEnv = "mate";
			gtk_init(0, 0);
		} else if (desktop.find("xfce") != std::string::npos) {
			desktopEnv = "xfce";
			gtk_init(0, 0);
		} else if (desktop.find("kde") != std::string::npos) {
			desktopEnv = "kde";
		} else if (desktop.find("lxde") != std::string::npos) {
			desktopEnv = "lxde";
		} else if (desktop.find("deepin") != std::string::npos) {
			desktopEnv = "deepin";
		}
	}
#endif

	/********
	 * MISC
	 ********/
#if defined OS_W32
	//Get windows version
	OSVERSIONINFOW version;
	ZeroMemory(&version, sizeof(version));
	version.dwOSVersionInfoSize = sizeof(version);
	GetVersionEx(&version);
#endif
}

Oneshot::~Oneshot()
{
	delete p;
}

void Oneshot::update()
{
	if (p->winPosChanged)
	{
		p->winPosChanged = false;

		//Map of unobscured pixels in this frame
		static std::vector<bool> obscuredFrame;
		obscuredFrame.resize(p->obscuredMap.size());
		std::fill(obscuredFrame.begin(), obscuredFrame.end(), true);

		SDL_Rect screenRect;
		SDL_LockMutex(p->winMutex);
		screenRect.x = p->winX;
		screenRect.y = p->winY;
		SDL_UnlockMutex(p->winMutex);
		screenRect.w = 640;
		screenRect.h = 480;

		//Update obscured map and texture for window portion offscreen
		for (int i = 0, max = SDL_GetNumVideoDisplays(); i < max; ++i)
		{
			SDL_Rect bounds;
			SDL_GetDisplayBounds(i, &bounds);

			//Get intersection of window and the screen
			SDL_Rect intersect;
			if (!SDL_IntersectRect(&screenRect, &bounds, &intersect))
				continue;
			intersect.x -= screenRect.x;
			intersect.y -= screenRect.y;

			//If it's entirely within the bounds of the screen, we don't need to check out
			//any other monitors
			if (intersect.x == 0 && intersect.y == 0 && intersect.w == 640 && intersect.h == 480)
				return;

			for (int y = intersect.y; y < intersect.y + intersect.h; ++y)
			{
				int start = y * 640 + intersect.x;
				std::fill(obscuredFrame.begin() + start, obscuredFrame.begin() + (start + intersect.w), false);
			}
		}

		//Update the obscured map, and return prematurely if we don't have any changes
		//to make to the texture
		bool needsUpdate = false;
		bool cleared = true;
		for (size_t i = 0; i < obscuredFrame.size(); ++i)
		{
			if (obscuredFrame[i])
			{
				p->obscuredMap[i] = 0;
				needsUpdate = true;
			}
			if (p->obscuredMap[i] == 255)
				cleared = false;
		}
		p->obscuredCleared = cleared;
		if (!needsUpdate)
			return;

		//Flag as dirty
		obscuredDirty = true;
	}
}

const std::string &Oneshot::os() const
{
	return p->os;
}

const std::string &Oneshot::lang() const
{
	return p->lang;
}

const std::string &Oneshot::userName() const
{
	return p->userName;
}

const std::string &Oneshot::savePath() const
{
	return p->savePath;
}

const std::string &Oneshot::docsPath() const
{
	return p->docsPath;
}

const std::string &Oneshot::gamePath() const
{
	return p->gamePath;
}

const std::string &Oneshot::journal() const
{
	return p->journal;
}

const std::vector<uint8_t> &Oneshot::obscuredMap() const
{
	return p->obscuredMap;
}

bool Oneshot::obscuredCleared() const
{
	return p->obscuredCleared;
}

bool Oneshot::exiting() const
{
	return p->exiting;
}

bool Oneshot::allowExit() const
{
	return p->allowExit;
}

void Oneshot::setYesNo(const char *yes, const char *no)
{
	p->txtYes = yes;
	p->txtNo = no;
}

void Oneshot::setExiting(bool exiting)
{
	if (p->exiting != exiting) {
		p->exiting = exiting;
		if (exiting) {
			threadData.exiting.set();
		} else {
			threadData.exiting.clear();
		}
	}
}

void Oneshot::setAllowExit(bool allowExit)
{
	if (p->allowExit != allowExit) {
		p->allowExit = allowExit;
		if (allowExit) {
			threadData.allowExit.set();
		} else {
			threadData.allowExit.clear();
		}
	}
}

bool Oneshot::msgbox(int type, const char *body, const char *title)
{
	if (!title)
		title = "";
	#if defined OS_LINUX
	if (
		desktopEnv == "gnome" ||
		desktopEnv == "mate" ||
		desktopEnv == "cinnamon" ||
		desktopEnv == "xfce"
	) {
		linux_DialogData data = {type, body, title, 0};
		gdk_threads_add_idle(linux_dialog, &data);
		gtk_main();
		return data.result;
	}
	#endif

	// SDL message box
	// Button data
	static const SDL_MessageBoxButtonData buttonOk = {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "OK"};
	static const SDL_MessageBoxButtonData buttonsOk[] = {buttonOk};
	SDL_MessageBoxButtonData buttonYes = {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, p->txtYes.c_str()};
	SDL_MessageBoxButtonData buttonNo = {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, p->txtNo.c_str()};
	SDL_MessageBoxButtonData buttonsYesNo[] = {buttonNo, buttonYes};

	// Messagebox data
	SDL_MessageBoxData data;
	data.window = NULL; //p->window;
	data.colorScheme = 0;
	data.title = title;
	data.message = body;
#ifdef OS_W32
	DWORD sound;
#endif

	//Set type
	switch (type)
	{
	case MSG_INFO:
	case MSG_YESNO:
	default:
		data.flags = SDL_MESSAGEBOX_INFORMATION;
#ifdef OS_W32
		sound = SND_ALIAS_SYSTEMQUESTION;
#endif
		break;
	case MSG_WARN:
		data.flags = SDL_MESSAGEBOX_WARNING;
#ifdef OS_W32
		sound = SND_ALIAS_SYSTEMEXCLAMATION;
#endif
		break;
	case MSG_ERR:
		data.flags = SDL_MESSAGEBOX_WARNING;
#ifdef OS_W32
		sound = SND_ALIAS_SYSTEMASTERISK;
#endif
		break;
	}

	// Set buttons
	switch (type)
	{
	case MSG_INFO:
	case MSG_WARN:
	case MSG_ERR:
	default:
		data.numbuttons = 1;
		data.buttons = buttonsOk;
		break;
	case MSG_YESNO:
		data.numbuttons = 2;
		data.buttons = buttonsYesNo;
		break;
	}

	// Show messagebox
#ifdef OS_W32
	PlaySoundW((LPCWSTR)sound, NULL, SND_ALIAS_ID | SND_ASYNC);
#endif
	int button;

	#ifdef OS_OSX
		int *btn = &button;

		// Message boxes and UI changes must be performed from the main thread on macOS Mojave and above.
		// This block ensures the message box will show from the main thread.
		dispatch_sync(dispatch_get_main_queue(),
			^{ SDL_ShowMessageBox(&data, btn); }
		);
	#else
		SDL_ShowMessageBox(&data, &button);
	#endif

	return button ? true : false;
}

std::string Oneshot::textinput(const char* prompt, int char_limit, const char* fontName) {
	std::vector<std::string> *fontNames = new std::vector<std::string>();
	fontNames->push_back(fontName);
	fontNames->push_back("VL Gothic");
	Font *font = new Font(fontNames, 18);

	Bitmap *promptBmp = new Bitmap(DEF_SCREEN_W, DEF_SCREEN_H);
	promptBmp->setInitFont(font);
	promptBmp->drawText(0, 0, DEF_SCREEN_W, DEF_SCREEN_H, prompt, 1);

	Bitmap *inputBmp = new Bitmap(DEF_SCREEN_W, DEF_SCREEN_H);
	inputBmp->setInitFont(font);
	inputBmp->drawText(0, 0, DEF_SCREEN_W, DEF_SCREEN_H, "", 1);

	std::string inputTextPrev = std::string("");
	threadData.acceptingTextInput.set();
	threadData.inputTextLimit = char_limit;
	threadData.inputText.clear();
	SDL_StartTextInput();

	// Main loop
	while (threadData.acceptingTextInput) {
		if (inputTextPrev != threadData.inputText) {
			inputBmp->clear();
			inputBmp->drawText(DEF_SCREEN_W / 2, DEF_SCREEN_H / 2, DEF_SCREEN_W, DEF_SCREEN_H, threadData.inputText.c_str(), 1);
			inputTextPrev = threadData.inputText;
		}
	}

	// Disable text input
	SDL_StopTextInput();

	return threadData.inputText;
}

void Oneshot::setWindowPos(int x, int y)
{
	SDL_LockMutex(p->winMutex);
	p->winX = x;
	p->winY = y;
	p->winPosChanged = true;
	SDL_UnlockMutex(p->winMutex);
}

void Oneshot::resetObscured()
{
	std::fill(p->obscuredMap.begin(), p->obscuredMap.end(), 255);
	obscuredDirty = true;
	p->obscuredCleared = false;
}
