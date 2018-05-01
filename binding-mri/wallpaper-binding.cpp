#include <iostream>

#include "etc.h"
#include "sharedstate.h"
#include "binding-util.h"
#include "binding-types.h"
#include "config.h"
#include "oneshot.h"

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
	static bool isCached = false;
#else
	#ifdef __APPLE__
		#include "mac-desktop.h"
		static bool isCached = false;
	#else
		#include <giomm/settings.h>
		#include <xfconf/xfconf.h>
		#include <unistd.h>
		#include <algorithm>
		#include <iostream>
		#include <string>
		#include <sstream>
		static std::string desktop = "uninitialized";
		// GNOME settings
		static Glib::RefPtr<Gio::Settings> bgsetting;
		static std::string defPictureURI, defPictureOptions, defPrimaryColor, defColorShading;
		// XFCE settings
		static XfconfChannel* bgchannel;
		static int defPictureStyle;
		static int defColorStyle;
		static GValue defColor = G_VALUE_INIT;
		static bool defColorExists;
		static std::string optionImage, optionColor, optionImageStyle, optionColorStyle;
	#endif
#endif

#ifdef __linux__
void desktopEnvironmentInit()
{
	if (desktop != "uninitialized") {
		return;
	}
	desktop = shState->oneshot().desktopEnv;
	if (desktop == "gnome" || desktop == "mate") {
		if (desktop == "gnome") {
			bgsetting = Gio::Settings::create("org.gnome.desktop.background");
			defPictureURI = bgsetting->get_string("picture-uri");
		} else {
			bgsetting = Gio::Settings::create("org.mate.background");
			defPictureURI = bgsetting->get_string("picture-filename");
		}
		defPictureOptions = bgsetting->get_string("picture-options");
		defPrimaryColor = bgsetting->get_string("primary-color");
		defColorShading = bgsetting->get_string("color-shading-type");
	} else if (desktop == "xfce") {
		GError *xferror = NULL;
		if (xfconf_init(&xferror)) {
			bgchannel = xfconf_channel_get("xfce4-desktop");
			std::string optionPrefix = "/backdrop/screen0/monitor0/workspace0/";
			optionImage = optionPrefix + "last-image";
			optionColor = optionPrefix + "color1";
			optionImageStyle = optionPrefix + "image-style";
			optionColorStyle = optionPrefix + "color-style";
			defPictureURI = xfconf_channel_get_string(bgchannel, optionImage.c_str(), "");
			defPictureStyle = xfconf_channel_get_int(bgchannel, optionImageStyle.c_str(), -1);
			defColorExists = xfconf_channel_get_property(bgchannel, optionColor.c_str(), &defColor);
			defColorStyle = xfconf_channel_get_int(bgchannel, optionColorStyle.c_str(), -1);
		} else {
			// Configuration failed to initialize, we won't set the wallpaper
			desktop = "xfce_error";
			g_error_free(xferror);
		}
	}
}
#endif

RB_METHOD(wallpaperSet)
{
	RB_UNUSED_PARAM;
	const char *name;
	int color;
	rb_get_args(argc, argv, "zi", &name, &color RB_ARG_END);
	std::string path;
#ifdef _WIN32
	path = shState->config().gameFolder + "\\Wallpaper\\" + name + ".bmp";
	std::cout << "Setting wallpaper to " << path << std::endl;
	// Crapify the slashes
	size_t index = 0;
	for (;;) {
		index = path.find("/", index);
		if (index == std::string::npos)
			break;
		path.replace(index, 1, "\\");
		index += 1;
	}
	WCHAR imgnameW[MAX_PATH];
	WCHAR imgnameFull[MAX_PATH];
	MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, imgnameW, MAX_PATH);
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
	std::string nameFix(name);
	std::size_t found = nameFix.find("w32");
	if (found != std::string::npos) {
		nameFix.replace(nameFix.end()-3, nameFix.end(), "unix");
	}
	path = "/Wallpaper/" + nameFix + ".png";

	std::cout << "Setting wallpaper to " << path << std::endl;

	#ifdef __APPLE__
		if (!isCached) {
			MacDesktop::CacheCurrentBackground();
			isCached = true;
		}
		MacDesktop::ChangeBackground(shState->config().gameFolder + path, ((color >> 16) & 0xFF) / 255.0, ((color >> 8) & 0xFF) / 255.0, (color & 0xFF) / 255.0);
	#else
		char gameDir[PATH_MAX];
		if (getcwd(gameDir, sizeof(gameDir)) == NULL) {
			return Qnil;
		}
		std::string gameDirStr(gameDir);
		desktopEnvironmentInit();
		if (desktop == "gnome" || desktop == "mate") {
			std::stringstream hexColor;
			hexColor << "#" << std::hex << color;
			if (desktop == "gnome") {
				bgsetting->set_string("picture-uri", "file://" + gameDirStr + path);
			} else {
				bgsetting->set_string("picture-filename", gameDirStr + path);
			}
			bgsetting->set_string("picture-options", "scaled");
			bgsetting->set_string("primary-color", hexColor.str());
			bgsetting->set_string("color-shading-type", "solid");
		} else if (desktop == "xfce") {
			int r = (color >> 16) & 0xFF;
			int g = (color >> 8) & 0xFF;
			int b = color & 0xFF;
			unsigned int ur = r * 256 + r;
			unsigned int ug = g * 256 + g;
			unsigned int ub = b * 256 + b;
			unsigned int alpha = 65535;
			std::string concatPath(gameDirStr + path);
			xfconf_channel_set_string(bgchannel, optionImage.c_str(), concatPath.c_str());
			xfconf_channel_set_int(bgchannel, optionColorStyle.c_str(), 0);
			xfconf_channel_set_int(bgchannel, optionImageStyle.c_str(), 4);
			GValue colorValue = G_VALUE_INIT;
			GPtrArray *colorArr = g_ptr_array_sized_new(4);
			GType colorArrType = g_type_from_name("GPtrArray_GValue_");
			if (!colorArrType) {
				std::stringstream colorCommand;
				colorCommand << "xfconf-query -c xfce4-desktop -n -p " << optionColor
							 << " -t uint -t uint -t uint -t uint -s " << ub
							 << " -s " << ug << " -s " << ub << " -s " << alpha;
				int colorCommandRes = system(colorCommand.str().c_str());
				defColorExists = xfconf_channel_get_property(bgchannel, optionColor.c_str(), &defColor);
				colorArrType = g_type_from_name("GPtrArray_GValue_");
				if (!colorArrType) {
					// Let's do some debug output here and skip changing the color
					std::cout << "WALLPAPER ERROR: xfconf-query call returned" << colorCommandRes;
					return Qnil;
				}
			}
			g_value_init(&colorValue, colorArrType);
			GValue *vr = g_new0(GValue, 1);
			GValue *vg = g_new0(GValue, 1);
			GValue *vb = g_new0(GValue, 1);
			GValue *va = g_new0(GValue, 1);
			g_value_init(vr, G_TYPE_UINT);
			g_value_init(vg, G_TYPE_UINT);
			g_value_init(vb, G_TYPE_UINT);
			g_value_init(va, G_TYPE_UINT);
			g_value_set_uint(vr, ur);
			g_value_set_uint(vg, ug);
			g_value_set_uint(vb, ub);
			g_value_set_uint(va, alpha);
			g_ptr_array_add(colorArr, vr);
			g_ptr_array_add(colorArr, vg);
			g_ptr_array_add(colorArr, vb);
			g_ptr_array_add(colorArr, va);
			g_value_set_boxed(&colorValue, colorArr);
			xfconf_channel_set_property(bgchannel, optionColor.c_str(), &colorValue);
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
		desktopEnvironmentInit();
		if (desktop == "gnome" || desktop == "mate") {
			if (desktop == "gnome") {
				bgsetting->set_string("picture-uri", defPictureURI);
			} else {
				bgsetting->set_string("picture-filename", defPictureURI);
			}
			bgsetting->set_string("picture-options", defPictureOptions);
			bgsetting->set_string("primary-color", defPrimaryColor);
			bgsetting->set_string("color-shading-type", defColorShading);
		} else if (desktop == "xfce") {
			if (defColorExists) {
				xfconf_channel_set_property(bgchannel, optionColor.c_str(), &defColor);
			} else {
				xfconf_channel_reset_property(bgchannel, optionColor.c_str(), false);
			}
			if (defPictureURI == "") {
				xfconf_channel_reset_property(bgchannel, optionImage.c_str(), false);
			} else {
				xfconf_channel_set_string(bgchannel, optionImage.c_str(), defPictureURI.c_str());
			}
			if (defPictureStyle == -1) {
				xfconf_channel_reset_property(bgchannel, optionImageStyle.c_str(), false);
			} else {
				xfconf_channel_set_int(bgchannel, optionImageStyle.c_str(), defPictureStyle);
			}
			if (defColorStyle == -1) {
				xfconf_channel_reset_property(bgchannel, optionColorStyle.c_str(), false);
			} else {
				xfconf_channel_set_int(bgchannel, optionColorStyle.c_str(), defColorStyle);
			}
		}
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

#ifdef __linux__
void wallpaperBindingTerminate()
{
	// Clean up
	// We assume Gio::Settings destructor will be automatically called
	if (desktop == "xfce") {
		xfconf_shutdown();
	}
}
#endif
