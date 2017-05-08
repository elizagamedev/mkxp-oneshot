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

//OS-Specific code
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
	#else
		#define OS_LINUX

		class GtkWidget;

		typedef enum
		{
			GTK_MESSAGE_INFO,
			GTK_MESSAGE_WARNING,
			GTK_MESSAGE_QUESTION,
			GTK_MESSAGE_ERROR
		} GtkMessageType;

		typedef enum
		{
			GTK_BUTTONS_NONE,
			GTK_BUTTONS_OK,
			GTK_BUTTONS_CLOSE,
			GTK_BUTTONS_CANCEL,
			GTK_BUTTONS_YES_NO,
			GTK_BUTTONS_OK_CANCEL
		} GtkButtonsType;

		typedef enum
		{
			GTK_RESPONSE_NONE = -1,
			GTK_RESPONSE_REJECT = -2,
			GTK_RESPONSE_ACCEPT = -3,
			GTK_RESPONSE_DELETE_EVENT = -4,
			GTK_RESPONSE_OK = -5,
			GTK_RESPONSE_CANCEL = -6,
			GTK_RESPONSE_CLOSE = -7,
			GTK_RESPONSE_YES = -8,
			GTK_RESPONSE_NO = -9,
			GTK_RESPONSE_APPLY = -10,
			GTK_RESPONSE_HELP = -11
		} GtkResponseType;
	#endif

	/**
	 * xdg_user_dir_lookup_with_fallback:
	 * @type: a string specifying the type of directory
	 * @fallback: value to use if the directory isn't specified by the user
	 * @returns: a newly allocated absolute pathname
	 *
	 * Looks up a XDG user directory of the specified type.
	 * Example of types are "DESKTOP" and "DOWNLOAD".
	 *
	 * In case the user hasn't specified any directory for the specified
	 * type the value returned is @fallback.
	 *
	 * The return value is newly allocated and must be freed with
	 * free(). The return value is never NULL if @fallback != NULL, unless
	 * out of memory.
	 **/
	static char *
	xdg_user_dir_lookup_with_fallback (const char *type, const char *fallback)
	{
	  FILE *file;
	  char *home_dir, *config_home, *config_file;
	  char buffer[512];
	  char *user_dir;
	  char *p, *d;
	  int len;
	  int relative;

	  home_dir = getenv ("HOME");

	  if (home_dir == NULL)
	    goto error;

	  config_home = getenv ("XDG_CONFIG_HOME");
	  if (config_home == NULL || config_home[0] == 0)
	    {
	      config_file = (char*) malloc (strlen (home_dir) + strlen ("/.config/user-dirs.dirs") + 1);
	      if (config_file == NULL)
	        goto error;

	      strcpy (config_file, home_dir);
	      strcat (config_file, "/.config/user-dirs.dirs");
	    }
	  else
	    {
	      config_file = (char*) malloc (strlen (config_home) + strlen ("/user-dirs.dirs") + 1);
	      if (config_file == NULL)
	        goto error;

	      strcpy (config_file, config_home);
	      strcat (config_file, "/user-dirs.dirs");
	    }

	  file = fopen (config_file, "r");
	  free (config_file);
	  if (file == NULL)
	    goto error;

	  user_dir = NULL;
	  while (fgets (buffer, sizeof (buffer), file))
	    {
	      /* Remove newline at end */
	      len = strlen (buffer);
	      if (len > 0 && buffer[len-1] == '\n')
		buffer[len-1] = 0;

	      p = buffer;
	      while (*p == ' ' || *p == '\t')
		p++;

	      if (strncmp (p, "XDG_", 4) != 0)
		continue;
	      p += 4;
	      if (strncmp (p, type, strlen (type)) != 0)
		continue;
	      p += strlen (type);
	      if (strncmp (p, "_DIR", 4) != 0)
		continue;
	      p += 4;

	      while (*p == ' ' || *p == '\t')
		p++;

	      if (*p != '=')
		continue;
	      p++;

	      while (*p == ' ' || *p == '\t')
		p++;

	      if (*p != '"')
		continue;
	      p++;

	      relative = 0;
	      if (strncmp (p, "$HOME/", 6) == 0)
		{
		  p += 6;
		  relative = 1;
		}
	      else if (*p != '/')
		continue;

	      if (relative)
		{
		  user_dir = (char*) malloc (strlen (home_dir) + 1 + strlen (p) + 1);
	          if (user_dir == NULL)
	            goto error2;

		  strcpy (user_dir, home_dir);
		  strcat (user_dir, "/");
		}
	      else
		{
		  user_dir = (char*) malloc (strlen (p) + 1);
	          if (user_dir == NULL)
	            goto error2;

		  *user_dir = 0;
		}

	      d = user_dir + strlen (user_dir);
	      while (*p && *p != '"')
		{
		  if ((*p == '\\') && (*(p+1) != 0))
		    p++;
		  *d++ = *p++;
		}
	      *d = 0;
	    }
	error2:
	  fclose (file);

	  if (user_dir)
	    return user_dir;

	 error:
	  if (fallback)
	    return strdup (fallback);
	  return NULL;
	}
#else
    #error "Operating system not detected."
#endif

#define DEF_SCREEN_W 640
#define DEF_SCREEN_H 480

struct OneshotPrivate
{
	//Main SDL window
	SDL_Window *window;

	//String data
	std::string lang;
	std::string userName;
	std::string savePath;
	std::string docsPath;
	std::string gamePath;
	std::string journal;

	//Dialog text
	std::string txtYes;
	std::string txtNo;

	bool exiting;
	bool allowExit;

	//Alpha texture data for portions of window obscured by screen edges
	int winX, winY;
	SDL_mutex *winMutex;
	bool winPosChanged;
	std::vector<uint8_t> obscuredMap;
	bool obscuredCleared;

#if defined OS_LINUX
	//GTK+
	void *libgtk;
	void (*gtk_init)(int *argc, char ***argv);
	GtkWidget *(*gtk_message_dialog_new)(void *parent, int flags, GtkMessageType type, GtkButtonsType buttons, const char *message_format, ...);
	void (*gtk_window_set_title)(GtkWidget *window, const char *title);
	GtkResponseType (*gtk_dialog_run)(GtkWidget *dialog);
	void (*gtk_widget_destroy)(GtkWidget *widget);
	void (*gtk_main_quit)();
	void (*gtk_main)();
	unsigned int (*gdk_threads_add_idle)(int (*function)(void *data), void *data);
#endif

	OneshotPrivate()
		: window(0),
	      winMutex(SDL_CreateMutex())
#if defined OS_LINUX
		  ,libgtk(0)
#endif
	{
	}

	~OneshotPrivate()
	{
		SDL_DestroyMutex(winMutex);
#ifdef OS_LINUX
		if (libgtk)
			dlclose(libgtk);
#endif
	}
};

//OS-SPECIFIC FUNCTIONS
#if defined OS_LINUX
struct linux_DialogData
{
	//Input
	OneshotPrivate *p;
	int type;
	const char *body;
	const char *title;

	//Output
	bool result;
};

static int linux_dialog(void *rawData)
{
	linux_DialogData *data = reinterpret_cast<linux_DialogData*>(rawData);
	OneshotPrivate *p = data->p;

	//Determine correct flags
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
		p->gtk_main_quit();
		return 0;
	}

	//Display dialog and get result
	GtkWidget *dialog = p->gtk_message_dialog_new(0, 0, gtktype, gtkbuttons, data->body);
	p->gtk_window_set_title(dialog, data->title);
	int result = p->gtk_dialog_run(dialog);
	p->gtk_widget_destroy(dialog);

	//Interpret result and return
	data->result = (result == GTK_RESPONSE_OK || result == GTK_RESPONSE_YES);
	p->gtk_main_quit();
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

LTexture::LTexture() {
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture() {
	//Deallocate
	free();
}

bool LTexture::loadFromFile(std::string path, SDL_Renderer *gRenderer) {
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture *newTexture = NULL;

	//Load image at specified path
	SDL_Surface *loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL) {
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	} else {
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL) {
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		} else {
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

#ifdef _SDL_TTF_H
bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor, SDL_Renderer *gRenderer, TTF_Font *gFont) {
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface *textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
	if (textSurface != NULL) {
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL) {
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		} else {
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	} else {
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}


	//Return success
	return mTexture != NULL;
}
#endif

void LTexture::free() {
	//Free texture if it exists
	if (mTexture != NULL) {
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue) {
	//Modulate texture rgb
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending) {
	//Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha) {
	//Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(SDL_Renderer *gRenderer, int x, int y, SDL_Rect *clip, double angle, SDL_Point *center,
                      SDL_RendererFlip flip) {
	//Set rendering space and render to screen
	SDL_Rect renderQuad = {x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if (clip != NULL) {
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth() {
	return mWidth;
}

int LTexture::getHeight() {
	return mHeight;
}

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

	//Get documents path
	WCHAR path[MAX_PATH];
	SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, path);
	p->docsPath = w32_fromWide(path);
	p->gamePath = p->docsPath+"\\My Games";
	p->journal = "_______.exe";
#else
	//Get language code
	const char *lc_all = getenv("LC_ALL");
	const char *lang = getenv("LANG");
	const char *code = (lc_all ? lc_all : lang);
	if (code)
	{
		//find first dot, copy language code
		int end = 0;
		for (; code[end] && code[end] != '.'; ++end) {}
		p->lang = std::string(code, end);
	}
	else
		p->lang = "en";

	//Get user's name
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

	//Get documents path
	char *path = xdg_user_dir_lookup_with_fallback("DOCUMENTS", getenv("HOME"));
	p->docsPath = path;
	p->gamePath = path;
	#ifdef OS_OSX
		p->journal = "_______.app";
	#elif defined OS_LINUX
		p->journal = "_______";
	#endif
	free(path);
#endif

	/**********
	 * MSGBOX
	 **********/
#ifdef OS_LINUX
#define LOAD_FUNC(name) *reinterpret_cast<void**>(&p->name) = dlsym(p->libgtk, #name)
	//Attempt to link to gtk (prefer gtk2 over gtk3 until I can figure that message box icon out)
	static const char *gtklibs[] =
	{
		"libgtk-x11-2.0.so",
		"libgtk-3.0.so",
	};

	for (size_t i = 0; i < ARRAY_SIZE(gtklibs); ++i)
	{
		if (!(p->libgtk = dlopen("libgtk-x11-2.0.so", RTLD_NOW)))
			p->libgtk = dlopen("libgtk-3.0.so", RTLD_NOW);
		if (p->libgtk)
		{
			//Load functions
			LOAD_FUNC(gtk_init);
			LOAD_FUNC(gtk_message_dialog_new);
			LOAD_FUNC(gtk_window_set_title);
			LOAD_FUNC(gtk_dialog_run);
			LOAD_FUNC(gtk_widget_destroy);
			LOAD_FUNC(gtk_main_quit);
			LOAD_FUNC(gtk_main);
			LOAD_FUNC(gdk_threads_add_idle);

			if (p->gtk_init
					&& p->gtk_message_dialog_new
					&& p->gtk_window_set_title
					&& p->gtk_dialog_run
					&& p->gtk_widget_destroy
					&& p->gtk_main_quit
					&& p->gtk_main
					&& p->gdk_threads_add_idle)
			{
				p->gtk_init(0, 0);
			}
			else
			{
				dlclose(p->libgtk);
				p->libgtk = 0;
			}
		}
		if (p->libgtk)
			break;
	}
#undef LOAD_FUNC
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
#if 0
	//Get native window handle
	SDL_SysWMinfo wminfo;
	SDL_version version;
	SDL_VERSION(&version);
	wminfo.version = version;
	SDL_GetWindowWMInfo(p->window, &wminfo);
	HWND hwnd = wminfo.info.win.window;

	//Construct flags
	UINT flags = 0;
	switch (type)
	{
	case MSG_INFO:
		flags = MB_ICONINFORMATION;
		break;
	case MSG_YESNO:
		flags = MB_ICONQUESTION | MB_YESNO;
		break;
	case MSG_WARN:
		flags = MB_ICONWARNING;
		break;
	case MSG_ERR:
		flags = MB_ICONERROR;
		break;
	}

	//Create message box
	WCHAR *wbody = w32_toWide(body);
	WCHAR *wtitle = w32_toWide(title);
	int result = MessageBoxW(N, wbody, wtitle, flags);
	delete [] title;
	delete [] body;

	//Interpret result
	return (result == IDOK || result == IDYES);
#else
	#if defined OS_LINUX
		if (p->libgtk)
		{
			linux_DialogData data = {p, type, body, title, 0};
			p->gdk_threads_add_idle(linux_dialog, &data);
			p->gtk_main();
			return data.result;
		}
	#endif

	//SDL message box

	//Button data
	static const SDL_MessageBoxButtonData buttonOk = {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "OK"};
	static const SDL_MessageBoxButtonData buttonsOk[] = {buttonOk};
	SDL_MessageBoxButtonData buttonYes = {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, p->txtYes.c_str()};
	SDL_MessageBoxButtonData buttonNo = {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, p->txtNo.c_str()};
	SDL_MessageBoxButtonData buttonsYesNo[] = {buttonNo, buttonYes};

	//Messagebox data
	SDL_MessageBoxData data;
	data.window = NULL;//p->window;
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

	//Set buttons
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

	//Show messagebox
#ifdef OS_W32
	PlaySoundW((LPCWSTR)sound, NULL, SND_ALIAS_ID | SND_ASYNC);
#endif
	int button;
	SDL_ShowMessageBox(&data, &button);
	return button ? true : false;
#endif
}

std::string Oneshot::textinput(const char* prompt, int char_limit, const char* fontName) {
	// SDL_Color textColor = {0xFF, 0xFF, 0xFF, 0xFF}; //Set text color as black
	// SDL_Renderer *gRenderer = SDL_CreateRenderer(threadData.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	// // SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	// LTexture gPromptTextTexture;
	// LTexture gInputTextTexture;

	// //Open the font
	// TTF_Font *gFont = TTF_OpenFont("VL-Gothic-Regular.ttf", 18); // XXX Implement font changing
	// if (gFont == NULL) {
	// 	printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
	// 	// success = false;
	// } else {
	// 	//Render the prompt
	// 	if (!gPromptTextTexture.loadFromRenderedText(prompt, textColor, gRenderer, gFont)) {
	// 		printf("Failed to render prompt text!\n");
	// 		// success = false;
	// 	}
	// }

	// gInputTextTexture.loadFromRenderedText(threadData.inputText.c_str(), textColor, gRenderer, gFont);
	
	std::vector<std::string> *fontNames = new std::vector<std::string>();
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

	//Main loop
	while (threadData.acceptingTextInput) {
		if (inputTextPrev != threadData.inputText) {
			inputBmp->clear();
			inputBmp->drawText(DEF_SCREEN_W / 2, DEF_SCREEN_H / 2, DEF_SCREEN_W, DEF_SCREEN_H, threadData.inputText.c_str(), 1);
			inputTextPrev = threadData.inputText;
			// if (threadData.inputText.length() > 0) gInputTextTexture.loadFromRenderedText(threadData.inputText.c_str(), textColor, gRenderer, gFont);
			// else gInputTextTexture.loadFromRenderedText(" ", textColor, gRenderer, gFont);
		}

		// //Clear screen
		// // SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		// SDL_RenderClear(gRenderer);

		// //Render text textures
		// gPromptTextTexture.render(gRenderer, (DEF_SCREEN_W - gPromptTextTexture.getWidth()) / 2,
		//                           (DEF_SCREEN_H / 2) - gPromptTextTexture.getHeight());
		// gInputTextTexture.render(gRenderer, (DEF_SCREEN_W - gInputTextTexture.getWidth()) / 2,
		//                          (DEF_SCREEN_H / 2));

		// //Update screen
		// SDL_RenderPresent(gRenderer);
	}

	//Disable text input
	SDL_StopTextInput();

	// //Free loaded images
	// gPromptTextTexture.free();
	// gInputTextTexture.free();

	// //Free global font
	// TTF_CloseFont(gFont);
	// gFont = NULL;

	// //Destroy renderer
	// SDL_DestroyRenderer(gRenderer);
	// gRenderer = NULL;
	// delete promptBmp;
	// delete inputBmp;

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
