#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

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
		#include <gdk/gdk.h>
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
		static std::vector<int> defColorStyles, defPictureStyles;
		static std::vector<bool> defColorExists;
		static std::vector<std::string> defPictureURIs;
		static std::vector<GdkRGBA> defColorValues = {};
		static std::vector<std::string> optionImages, optionColors, optionImageStyles, optionColorStyles;
		static std::vector<std::string> monitors;
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
			const char* displayName = std::getenv("DISPLAY");
			GdkDisplay* display = gdk_display_open(displayName);

  			int numberOfDisplays = gdk_display_get_n_monitors(display);

			for (int i = 0; i < numberOfDisplays; i++)
			{
				// Obtain monitor name
				std::string monitorName = gdk_monitor_get_model(gdk_display_get_monitor(display, i));
      				monitorName.erase(std::remove(monitorName.begin(), monitorName.end(),' '), monitorName.end());

      				monitors.push_back(monitorName);
  			}

			GError *xferror = NULL;
			if (xfconf_init(&xferror)) {
				bgchannel = xfconf_channel_get("xfce4-desktop");

				std::string optionImage = "last-image";
				std::string optionColor = "rgba1";
				std::string optionImageStyle = "image-style";
				std::string optionColorStyle = "color-style";
				std::string optionFirstPart = "/backdrop/screen0/monitor";
		                std::string optionLastPart = "/workspace0/";

				for (int i = 0; i < monitors.size(); i++)
				{
					optionImages.push_back(optionFirstPart + monitors.at(i) + optionLastPart + optionImage);
					defPictureURIs.push_back(xfconf_channel_get_string(bgchannel, 
											   optionImages.back().c_str(), ""));

					optionImageStyles.push_back(optionFirstPart + monitors.at(i) + optionLastPart + optionImageStyle);
					defPictureStyles.push_back(xfconf_channel_get_int(bgchannel, 
									 		  optionImageStyles.back().c_str(), -1));

					optionColors.push_back(optionFirstPart + monitors.at(i) + optionLastPart + optionColor);

					defColorValues.push_back({ });
					defColorExists.push_back(xfconf_channel_get_array(bgchannel, 
										     	  optionColors.back().c_str(),
											  G_TYPE_DOUBLE, &defColorValues.back().red,
											  G_TYPE_DOUBLE, &defColorValues.back().green, 
											  G_TYPE_DOUBLE, &defColorValues.back().blue,
											  G_TYPE_DOUBLE, &defColorValues.back().alpha,
											  G_TYPE_INVALID));


					optionColorStyles.push_back(optionFirstPart + monitors.at(i) + optionLastPart + optionColorStyle);
					defColorStyles.push_back(xfconf_channel_get_int(bgchannel, 
										        optionColorStyles.back().c_str(), -1));
				}
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
			std::string concatPath(gameDirStr + path);
			
			int r = (color >> 16) & 0xFF;
			int g = (color >> 8) & 0xFF;
			int b = color & 0xFF;
			double dr = (r * 256 + r) / 65535.0;
			double dg = (g * 256 + g) / 65535.0;
			double db = (b * 256 + b) / 65535.0;
			double alpha = 1;


			for (int i = 0; i < monitors.size(); i++)
			{
				xfconf_channel_set_string(bgchannel, optionImages.at(i).c_str(), concatPath.c_str());
				xfconf_channel_set_int(bgchannel, optionColorStyles.at(i).c_str(), 0);
				xfconf_channel_set_int(bgchannel, optionImageStyles.at(i).c_str(), 4);

				xfconf_channel_set_array(bgchannel, 
						  	 optionColors.at(i).c_str(),
						 	 G_TYPE_DOUBLE, &dr,
						 	 G_TYPE_DOUBLE, &dg, 
							 G_TYPE_DOUBLE, &db,
							 G_TYPE_DOUBLE, &alpha,
							 G_TYPE_INVALID);
			}
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
			for (int i = 0; i < monitors.size(); i++)
			{
				if (defColorExists.at(i)) {
					xfconf_channel_set_array(bgchannel, 
							    	 optionColors.at(i).c_str(),
								 G_TYPE_DOUBLE, &defColorValues.at(i).red,
								 G_TYPE_DOUBLE, &defColorValues.at(i).green, 
								 G_TYPE_DOUBLE, &defColorValues.at(i).blue,
								 G_TYPE_DOUBLE, &defColorValues.at(i).alpha,
								 G_TYPE_INVALID);
				} else {
					xfconf_channel_reset_property(bgchannel, optionColors.at(i).c_str(), false);
				}
				if (defPictureURIs.at(i) == "") {
					xfconf_channel_reset_property(bgchannel, optionImages.at(i).c_str(), false);
				} else {
					xfconf_channel_set_string(bgchannel, optionImages.at(i).c_str(), defPictureURIs.at(i).c_str());
				}
				if (defPictureStyles.at(i) == -1) {
					xfconf_channel_reset_property(bgchannel, optionImageStyles.at(i).c_str(), false);
				} else {
					xfconf_channel_set_int(bgchannel, optionImageStyles.at(i).c_str(), defPictureStyles.at(i));
				}
				if (defColorStyles.at(i) == -1) {
					xfconf_channel_reset_property(bgchannel, optionColorStyles.at(i).c_str(), false);
				} else {
					xfconf_channel_set_int(bgchannel, optionColorStyles.at(i).c_str(), defColorStyles.at(i));
				}
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
