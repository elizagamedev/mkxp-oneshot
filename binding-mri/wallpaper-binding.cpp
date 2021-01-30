#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include <boost/algorithm/string/replace.hpp>

#include "etc.h"
#include "sharedstate.h"
#include "binding-util.h"
#include "binding-types.h"
#include "config.h"
#include "oneshot.h"
#include "debugwriter.h"

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
		#include <gio/gio.h>
		#include <xfconf/xfconf.h>
		#include <unistd.h>
		#include <algorithm>
		#include <iostream>
		#include <string>
		#include <sstream>
		static std::string desktop = "uninitialized";
		// GNOME settings
		static GSettings *bgsetting;
		static std::string defPictureURI, defPictureOptions, defPrimaryColor, defColorShading;
		// XFCE settings
		static XfconfChannel* bgchannel;
		static int defPictureStyle;
		static int defColorStyle;
		static GValue defColor = G_VALUE_INIT;
		static bool defColorExists;
		static std::string optionImage, optionColor, optionImageStyle, optionColorStyle;
		// KDE settings
		static std::map<std::string, std::string> defPlugins, defPictures, defColors, defModes;
		static std::map<std::string, bool> defBlurs;
		// Fallback settings
		static std::string fallbackPath;
	#endif
#endif

#ifdef __linux__
	void desktopEnvironmentInit()
	{
		if (desktop != "uninitialized") {
			return;
		}
		desktop = shState->oneshot().desktopEnv;
		if (desktop == "cinnamon" || desktop == "gnome" || desktop == "mate" || desktop == "deepin") {
			if (desktop == "cinnamon" || desktop == "gnome" || desktop == "deepin") {
				if (desktop == "cinnamon") bgsetting = g_settings_new("org.cinnamon.desktop.background");
				else if (desktop == "deepin") bgsetting = g_settings_new("com.deepin.wrap.gnome.desktop.background");
				else bgsetting = g_settings_new("org.gnome.desktop.background");
				defPictureURI = g_settings_get_string(bgsetting, "picture-uri");
			} else {
				bgsetting = g_settings_new("org.mate.background");
				defPictureURI = g_settings_get_string(bgsetting, "picture-filename");
			}
			defPictureOptions = g_settings_get_string(bgsetting, "picture-options");
			defPrimaryColor = g_settings_get_string(bgsetting, "primary-color");
			defColorShading = g_settings_get_string(bgsetting, "color-shading-type");
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
		} else if (desktop == "kde") {
			std::ifstream configFile;
			configFile.open(std::string(getenv("HOME")) + "/.config/plasma-org.kde.plasma.desktop-appletsrc", std::ios::in);
			if (configFile.is_open()) {
				std::string line;
				std::vector<std::string> sections;
				std::size_t undefined = 999999999;
				bool readPlugin = false, readOther = false;
				std::string containment;
				while (getline(configFile, line)) {
					std::size_t index = undefined, lastIndex = undefined;
					if (line.size() == 0) {
						readPlugin = false;
						readOther = false;
					} else if (readPlugin) {
						index = line.find('=');
						if (line.substr(0, index) == "wallpaperplugin") {
							defPlugins[containment] = line.substr(index + 1);
						}
					} else if (readOther) {
						index = line.find('=');
						std::string key = line.substr(0, index);
						std::string val = line.substr(index + 1);
						if (key == "Image") {
							defPictures[containment] = val;
						} else if (key == "Color") {
							defColors[containment] = val;
						} else if (key == "FillMode") {
							defModes[containment] = val;
						} else if (key == "Blur") {
							defBlurs[containment] = (val == "true");
						}
					} else if (line.at(0) == '[') {
						sections.clear();
						while (true) {
							index = line.find(lastIndex == undefined ? '[' : ']', index == undefined ? 0 : index);
							if (index == std::string::npos) {
								break;
							}
							if (lastIndex == undefined) {
								lastIndex = index;
							} else {
								sections.push_back(line.substr(lastIndex + 1, index - lastIndex - 1));
								lastIndex = undefined;
							}
						}
						if (sections.size() == 2 && sections[0] == "Containments") {
							readPlugin = true;
							containment = sections[1];
						} else if (
							sections.size() == 5 &&
							sections[0] == "Containments" &&
							sections[2] == "Wallpaper" &&
							sections[3] == "org.kde.image" &&
							sections[4] == "General"
						) {
							readOther = true;
							containment = sections[1];
						}
					}
				}
				configFile.close();
			} else {
				Debug() << "FATAL: Cannot find desktop configuration!";
				desktop = "kde_error";
			}
		} else {
			fallbackPath = std::string(getenv("HOME")) + "/Desktop/ONESHOT_hint.png";
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
	Debug() << "Setting wallpaper to" << path;
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

	Debug() << "Setting wallpaper to " << path;

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
		if (desktop == "cinnamon" || desktop == "gnome" || desktop == "mate" || desktop == "deepin") {
			std::stringstream hexColor;
			hexColor << "#" << std::hex << color;
			g_settings_set_string(bgsetting, "picture-options", "scaled");
			g_settings_set_string(bgsetting, "primary-color", hexColor.str().c_str());
			g_settings_set_string(bgsetting, "color-shading-type", "solid");
			if (desktop == "cinnamon" || desktop == "gnome" || desktop == "deepin") {
				g_settings_set_string(bgsetting, "picture-uri", ("file://" + gameDirStr + path).c_str());
			} else {
				g_settings_set_string(bgsetting, "picture-filename", (gameDirStr + path).c_str());
			}
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
					Debug() << "WALLPAPER ERROR: xfconf-query call returned" << colorCommandRes;
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
		} else if (desktop == "kde") {
			std::stringstream command;
			std::string concatPath(gameDirStr + path);
			boost::replace_all(concatPath, "\\", "\\\\");
			boost::replace_all(concatPath, "\"", "\\\"");
			boost::replace_all(concatPath, "'", "\\x27");
			command << "qdbus org.kde.plasmashell /PlasmaShell org.kde.PlasmaShell.evaluateScript 'string:" <<
				"var allDesktops = desktops();" <<
				"for (var i = 0, l = allDesktops.length; i < l; ++i) {" <<
					"var d = allDesktops[i];" <<
					"d.wallpaperPlugin = \"org.kde.image\";" <<
					"d.currentConfigGroup = [\"Wallpaper\", \"org.kde.image\", \"General\"];" <<
					"d.writeConfig(\"Image\", \"file://" << concatPath << "\");" <<
					"d.writeConfig(\"FillMode\", \"6\");" <<
					"d.writeConfig(\"Blur\", false);" <<
					"d.writeConfig(\"Color\", [\"" <<
						std::to_string((color >> 16) & 0xFF) << "\", \"" <<
						std::to_string((color >> 8) & 0xFF) << "\", \"" <<
						std::to_string(color & 0xFF) <<
					"\"]);" <<
				"}" <<
			"'";
			Debug() << "Wallpaper command:" << command.str();
			int result = system(command.str().c_str());
			Debug() << "Result:" << result;
		} else {
			std::ifstream srcHint(gameDirStr + path);
			std::ofstream dstHint(fallbackPath);
			dstHint << srcHint.rdbuf();
			srcHint.close();
			dstHint.close();
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
		if (desktop == "cinnamon" || desktop == "gnome" || desktop == "mate" || desktop == "deepin") {
			if (desktop == "cinnamon" || desktop == "gnome" || desktop == "deepin") {
				g_settings_set_string(bgsetting, "picture-uri", defPictureURI.c_str());
			} else {
				g_settings_set_string(bgsetting, "picture-filename", defPictureURI.c_str());
			}
			g_settings_set_string(bgsetting, "picture-options", defPictureOptions.c_str());
			g_settings_set_string(bgsetting, "primary-color", defPrimaryColor.c_str());
			g_settings_set_string(bgsetting, "color-shading-type", defColorShading.c_str());
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
		} else if (desktop == "kde") {
			std::stringstream command;
			command << "qdbus org.kde.plasmashell /PlasmaShell org.kde.PlasmaShell.evaluateScript 'string:" <<
					"var allDesktops = desktops();" <<
					"var data = {";
			// Plugin, picture, color, mode, blur
			for (auto const& x : defPlugins) {
				command << "\"" << x.first << "\": {"
						<< "plugin: \"" << x.second << "\"";
				if (defPictures.find(x.first) != defPictures.end()) {
					std::string picture = defPictures[x.first];
					boost::replace_all(picture, "\\", "\\\\");
					boost::replace_all(picture, "\"", "\\\"");
					boost::replace_all(picture, "'", "\\x27");
					command << ", picture: \"" << picture << "\"";
				}
				if (defColors.find(x.first) != defColors.end()) {
					command << ", color: \"" << defColors[x.first] << "\"";
				}
				if (defModes.find(x.first) != defModes.end()) {
					command << ", mode: \"" << defModes[x.first] << "\"";
				}
				if (defBlurs.find(x.first) != defBlurs.end() && defBlurs[x.first]) {
					command << ", blur: true";
				}
				command << "},";
			}
			command << "\"no\": {}};" <<
				"for (var i = 0, l = allDesktops.length; i < l; ++i) {" <<
					"var d = allDesktops[i];" <<
					"var dat = data[d.id];" <<
					"d.wallpaperPlugin = dat.plugin;" <<
					"d.currentConfigGroup = [\"Wallpaper\", \"org.kde.image\", \"General\"];" <<
					"if (dat.picture) {" <<
						"d.writeConfig(\"Image\", dat.picture);" <<
					"}" <<
					"if (dat.color) {" <<
						"d.writeConfig(\"Color\", dat.color.split(\",\"));" <<
					"}" <<
					"if (dat.mode) {" <<
						"d.writeConfig(\"FillMode\", dat.mode);" <<
					"}" <<
					"if (dat.blur) {" <<
						"d.writeConfig(\"Blur\", dat.blur);" <<
					"}" <<
				"}" <<
			"'";
			Debug() << "Reset wallpaper command:" << command.str();
			int result = system(command.str().c_str());
			Debug() << "Reset result:" << result;
		} else {
			if (remove(fallbackPath.c_str()) != 0) {
				Debug() << "Failed to delete:" << fallbackPath;
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
	// Clean up.
	if (desktop == "xfce") {
		xfconf_shutdown();
	}
}
#endif
