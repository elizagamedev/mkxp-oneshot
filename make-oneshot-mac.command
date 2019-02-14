#!/bin/sh
set -e

cd `dirname $0`

# User-configurable variables
mac_version="1.1.1"
make_threads=8
ONESHOT_PATH=$HOME/Library/Application\ Support/Steam/steamapps/common/OneShot
# Colors
white="\033[0;37m"      # White - Regular
bold="\033[1;37m"       # White - Bold
cyan="\033[1;36m"       # Cyan - Bold
green="\033[1;32m"      # Green - Bold
color_reset="\033[0m"   # Reset Colors

use_qmake=True

echo "${white}Compiling ${bold}SyngleChance v${mac_version} ${white}engine for macOS...${color_reset}\n"

# Generate makefile and build main + journal
if [[ $use_qmake == True ]]
	then
	echo "-> ${cyan}Generate makefile...${color_reset}"
	qmake MRIVERSION=2.5
	echo "-> ${cyan}Compile engine...${color_reset}"
	make -j${make_threads}
	echo "-> ${cyan}Compile steamshim...${color_reset}"
	# cd steamshim_parent
	# mkdir build && cd build
	# cmake ..
	# STEAMWORKS=./steamworks make -j${make_threads}
	# cd ../..
else
	echo "${bold}WARNING: Conan/CMake method not ready yet.${color_reset}"
fi
echo "-> ${cyan}Compile journal...${color_reset}"
pyinstaller journal/unix/journal.spec --onefile --windowed

# Create app bundles
echo "-> ${cyan}Create app bundles...${color_reset}"
OSX_App="OneShot.app"
ContentsDir="$OSX_App/Contents"
LibrariesDir="$OSX_App/Contents/Libraries"
ResourcesDir="$OSX_App/Contents/Resources"

# create directories in the @target@.app bundle
if [ ! -e $LibrariesDir ]
	then
	mkdir -p "$LibrariesDir"
fi

if [ ! -e $ResourcesDir ]
	then
	mkdir -p "$ResourcesDir"
fi

cp steamshim_parent/steamshim ./OneShot.app/Contents/Resources/steamshim
# cp patches/mac/libsteam_api.dylib ./OneShot.app/Contents/Libraries/libsteam_api.dylib
install_name_tool -change @loader_path/libsteam_api.dylib "$( cd "$(dirname "$0")" ; pwd -P )"/steamworks/redistributable_bin/osx32/libsteam_api.dylib ./OneShot.app/Contents/Resources/steamshim
cmake -P patches/mac/CompleteBundle.cmake
cp assets/icon.icns ./OneShot.app/Contents/Resources/icon.icns
cp steam_appid.txt ./OneShot.app/Contents/Resources/steam_appid.txt
cp patches/mac/oneshot.sh ./OneShot.app/Contents/MacOS/oneshot.sh
mv OneShot.app/Contents/MacOS/OneShot OneShot.app/Contents/Resources/OneShot
cp -r dist/_______.app _______.app

# Set version number
echo "-> ${cyan}Set version number...${color_reset}"
rm -f OneShot.app/Contents/Info.plist
rm -f _______.app/Contents/Info.plist
m4 patches/mac/Info.plist.in -DONESHOTMACVERSION=$mac_version > OneShot.app/Contents/Info.plist
m4 patches/mac/JournalInfo.plist.in -DONESHOTMACVERSION=$mac_version > _______.app/Contents/Info.plist

# Compile scripts
echo "-> ${cyan}Compile xScripts.rxdata...${color_reset}"
ruby rpgscript.rb ./scripts "$ONESHOT_PATH"
cp "$ONESHOT_PATH/Data/xScripts.rxdata" .

echo "-> ${cyan}Install OneShot apps to Steam directory...${color_reset}"
cp -rf "./OneShot.app" "$ONESHOT_PATH"
cp -rf "./_______.app" "$ONESHOT_PATH"

# Cleanup
echo "-> ${cyan}Cleanup files...${color_reset}"
# make clean
rm -rf journal/unix/__pycache__
rm -rf build
rm -rf dist

echo "\n${green}Complete!  ${white}Please report any issues to https://github.com/GooborgStudios/synglechance/issues${color_reset}"
