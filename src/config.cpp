/*
** config.cpp
**
** This file is part of mkxp.
**
** Copyright (C) 2013 Jonas Kulla <Nyocurio@gmail.com>
**
** mkxp is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** mkxp is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with mkxp.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "config.h"

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <SDL_filesystem.h>

#include <fstream>
#include <stdint.h>
#include <cstdlib>

#include "debugwriter.h"
#include "util.h"
#include "sdl-util.h"

static std::string prefPath(const char *org, const char *app)
{
	char *path = SDL_GetPrefPath(org, app);

	if (!path)
		return std::string();

	std::string str(path);
	SDL_free(path);

	return str;
}

typedef std::vector<std::string> StringVec;
namespace po = boost::program_options;

#define CONF_FILE "oneshot.conf"

Config::Config()
{}

void Config::read(int argc, char *argv[])
{
#define PO_DESC_ALL \
	PO_DESC(debugMode, bool, false) \
	PO_DESC(screenMode, bool, false) \
	PO_DESC(printFPS, bool, false) \
	PO_DESC(fullscreen, bool, false) \
	PO_DESC(fixedAspectRatio, bool, true) \
	PO_DESC(smoothScaling, bool, false) \
	PO_DESC(vsync, bool, true) \
	PO_DESC(fixedFramerate, int, 0) \
	PO_DESC(frameSkip, bool, true) \
	PO_DESC(syncToRefreshrate, bool, false) \
	PO_DESC(solidFonts, bool, false) \
	PO_DESC(subImageFix, bool, false) \
	PO_DESC(gameFolder, std::string, ".") \
	PO_DESC(allowSymlinks, bool, false) \
	PO_DESC(iconPath, std::string, "") \
	PO_DESC(SE.sourceCount, int, 6) \
	PO_DESC(pathCache, bool, true)

// Not gonna take your shit boost
#define GUARD_ALL( exp ) try { exp } catch(...) {}

#define PO_DESC(key, type, def) (#key, po::value< type >()->default_value(def))

	po::options_description podesc;
	podesc.add_options()
	        PO_DESC_ALL
	        ("preloadScript", po::value<StringVec>()->composing())
	        ("fontSub", po::value<StringVec>()->composing())
	        ("rubyLoadpath", po::value<StringVec>()->composing())
	        ;

	po::variables_map vm;

	/* Parse command line options */
	try
	{
		po::parsed_options cmdPo =
			po::command_line_parser(argc, argv).options(podesc).run();
		po::store(cmdPo, vm);
	}
	catch (po::error &error)
	{
		Debug() << "Command line:" << error.what();
	}

	/* Parse configuration file */
	SDLRWStream confFile(CONF_FILE, "r");

	if (confFile)
	{
		try
		{
			po::store(po::parse_config_file(confFile.stream(), podesc, true), vm);
			po::notify(vm);
		}
		catch (po::error &error)
		{
			Debug() << CONF_FILE":" << error.what();
		}
	}

#undef PO_DESC
#define PO_DESC(key, type, def) GUARD_ALL( key = vm[#key].as< type >(); )

	PO_DESC_ALL;

	GUARD_ALL( preloadScripts = vm["preloadScript"].as<StringVec>(); );

	GUARD_ALL( fontSubs = vm["fontSub"].as<StringVec>(); );

	GUARD_ALL( rubyLoadpaths = vm["rubyLoadpath"].as<StringVec>(); );

#undef PO_DESC
#undef PO_DESC_ALL

	SE.sourceCount = clamp(SE.sourceCount, 1, 64);

	commonDataPath = prefPath(".", "Oneshot");

	//Hardcode some ini/version settings
	rgssVersion = 1;
	game.title = "OneShot";
	game.scripts = "Data/xScripts.rxdata";
	defScreenW = 640;
	defScreenH = 480;

#ifdef STEAM
	/* Override fullscreen config if Big Picture */
	if (const char *env = std::getenv("SteamTenfoot"))
	{
		if (!strcmp(env, "1"))
			fullscreen = true;
	}
#endif
}
