#include "etc.h"
#include "sharedstate.h"
#include "binding-util.h"
#include "binding-types.h"
#include "config.h"

static bool isCached = false;

#ifdef _WIN32
	#include <windows.h>
	static WCHAR szStyle[8] = {0};
	static WCHAR szTile[8] = {0};
	static WCHAR szFile[MAX_PATH+1] = {0};
	static DWORD oldcolor = 0;
	static DWORD szStyleSize = sizeof(szStyle) - 1;
	static DWORD szTileSize = sizeof(szTile) - 1;
	static bool setStyle = false;
	static bool setTile = false;
#else
	#ifdef __APPLE__
		#include "mac-desktop.h"
	#else
		#include <giomm/settings.h>
		#include <unistd.h>
		#include <sstream>
		Glib::RefPtr<Gio::Settings> bgsetting = Gio::Settings::create("org.gnome.desktop.background");
		std::string defPictureURI = bgsetting->get_string("picture-uri");
		std::string defPictureOptions = bgsetting->get_string("picture-options");
		std::string defPrimaryColor = bgsetting->get_string("primary-color");
	#endif
#endif

RB_METHOD(wallpaperSet)
{
	RB_UNUSED_PARAM;
	const char *iname;
	int color;
	rb_get_args(argc, argv, "zi", &iname, &color RB_ARG_END);
	std::string imageName = iname;
	std::string imgname = shState->config().gameFolder + "/Wallpaper/" + imageName + ".bmp";
#ifdef _WIN32
	// Crapify the slashes
	size_t index = 0;
	for (;;) {
		index = imgname.find("/", index);
		if (index == std::string::npos)
			break;
		imgname.replace(index, 1, "\\");
		index += 1;
	}
	WCHAR imgnameW[MAX_PATH];
	WCHAR imgnameFull[MAX_PATH];
	MultiByteToWideChar(CP_UTF8, 0, imgname.c_str(), -1, imgnameW, MAX_PATH);
	GetFullPathNameW(imgnameW, MAX_PATH, imgnameFull, NULL);


	int colorId = COLOR_BACKGROUND;
	WCHAR zero[2] = L"0";
	DWORD zeroSize = 4;

	HKEY hKey = NULL;
	if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		goto end;

	if (!isCached) {
		// QUERY

		// Style
		setStyle = RegQueryValueExW(hKey, L"WallpaperStyle", 0, NULL, (LPBYTE)(szStyle), &szStyleSize) == ERROR_SUCCESS;

		// Tile
		setTile = RegQueryValueExW(hKey, L"TileWallpaper", 0, NULL, (LPBYTE)(szTile), &szTileSize) == ERROR_SUCCESS;

		// File path
		if (!SystemParametersInfoW(SPI_GETDESKWALLPAPER, MAX_PATH, (PVOID)szFile, 0))
			goto end;

		// Color
		oldcolor = GetSysColor(COLOR_BACKGROUND);

		isCached = true;
	}

	RegCloseKey(hKey);
	hKey = NULL;
	if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", 0, KEY_WRITE, &hKey) != ERROR_SUCCESS)
		goto end;

	// SET

	// Set the style
	if (RegSetValueExW(hKey, L"WallpaperStyle", 0, REG_SZ, (const BYTE*)zero, zeroSize) != ERROR_SUCCESS)
		goto end;

	if (RegSetValueExW(hKey, L"TileWallpaper", 0, REG_SZ, (const BYTE*)zero, zeroSize) != ERROR_SUCCESS)
		goto end;

	// Set the wallpaper
	if (!SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (PVOID)imgnameFull, SPIF_UPDATEINIFILE))
		goto end;

	// Set the color
	if (!SetSysColors(1, &colorId, (const COLORREF *)&color))
		goto end;
end:
	if (hKey)
		RegCloseKey(hKey);
#else
	std::size_t found = imageName.find("w32");
	if (found != std::string::npos) imageName.replace(imageName.end()-3, imageName.end(), "unix");
	imgname = shState->config().gameFolder + "/Wallpaper/" + imageName + ".png";

	#ifdef __APPLE__
		if (!isCached) {
			MacDesktop::CacheCurrentBackground();
			isCached = true;
		}
		MacDesktop::ChangeBackground(imgname, ((color >> 16) & 0xFF) / 255.0, ((color >> 8) & 0xFF) / 255.0, ((color) & 0xFF) / 255.0);
	#else
		char gameDir[1024];
		if (getcwd(gameDir, sizeof(gameDir)) != NULL) {
			std::string gameDirStr(gameDir);
            std::stringstream hexColor;
            hexColor << "#" << std::hex << color;
			bgsetting->set_string("picture-uri", "file://" + gameDirStr + "/Wallpaper/" + imageName + ".png");
			bgsetting->set_string("picture-options", "scaled");
			bgsetting->set_string("primary-color", hexColor.str());
		} else {
			// Error handling?
		}
	#endif
#endif
	return Qnil;
}

RB_METHOD(wallpaperReset)
{
	RB_UNUSED_PARAM;
#ifdef _WIN32
	if (isCached) {
		int colorId = COLOR_BACKGROUND;
		HKEY hKey = NULL;
		if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Desktop", 0, KEY_WRITE, &hKey) != ERROR_SUCCESS)
			goto end;

		// Set the style
		if (setStyle)
			RegSetValueExW(hKey, L"WallpaperStyle", 0, REG_SZ, (const BYTE*)szStyle, szStyleSize);

		if (setTile)
			RegSetValueExW(hKey, L"TileWallpaper", 0, REG_SZ, (const BYTE*)szTile, szTileSize);

		// Set the wallpaper
		if (!SystemParametersInfoW(SPI_SETDESKWALLPAPER, 0, (PVOID)szFile, SPIF_UPDATEINIFILE))
			goto end;

		// Set the color
		if (!SetSysColors(1, &colorId, (const COLORREF *)&oldcolor))
			goto end;
	end:
		if (hKey)
			RegCloseKey(hKey);
	}
#else
	#ifdef __APPLE__
		MacDesktop::ResetBackground();
	#else
		bgsetting->set_string("picture-uri", defPictureURI);
		bgsetting->set_string("picture-options", defPictureOptions);
		bgsetting->set_string("primary-color", defPrimaryColor);
	#endif
#endif
	return Qnil;
}

void wallpaperBindingInit()
{
	VALUE module = rb_define_module("Wallpaper");

	// Functions
	_rb_define_module_function(module, "set", wallpaperSet);
	_rb_define_module_function(module, "reset", wallpaperReset);
}
