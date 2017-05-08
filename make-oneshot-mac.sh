#!/bin/sh
set -e

# User-configurable variables
mac_version="0.3.3"
steam_game_dir=~/Library/Application\ Support/Steam/SteamApps/common/OneShot
make_threads=4

# Colors
white="\033[0;37m"      # White - Regular
bold="\033[1;37m"       # White - Bold
cyan="\033[1;36m"       # Cyan - Bold
green="\033[1;32m"      # Green - Bold
color_reset="\033[0m"   # Reset Colors

echo "${white}Compiling ${bold}SyngleChance v${mac_version} ${white}engine for macOS...${color_reset}\n"

# Generate makefile and build main + journal
echo "-> ${cyan}Generate makefile...${color_reset}"
qmake MRIVERSION=2.3
echo "-> ${cyan}Compile engine...${color_reset}"
make -j${make_threads}
echo "-> ${cyan}Compile journal...${color_reset}"
pyinstaller journal/mac/journal.spec --onefile --windowed

# Create app bundles
echo "-> ${cyan}Create app bundles...${color_reset}"
mv OneShot.app OneShot_new.app
cp -r patches/mac/OneShot_template.app ./OneShot.app
cp OneShot_new.app/Contents/MacOS/OneShot OneShot.app/Contents/Resources/OneShot
cp -r dist/_______.app _______.app

# Set version number
echo "-> ${cyan}Set version number...${color_reset}"
rm -f OneShot.app/Contents/Info.plist
rm -f _______.app/Contents/Info.plist
m4 patches/mac/Info.plist.in -DONESHOTMACVERSION=$mac_version > OneShot.app/Contents/Info.plist
m4 patches/mac/JournalInfo.plist.in -DONESHOTMACVERSION=$mac_version > _______.app/Contents/Info.plist

# Compile scripts
echo "-> ${cyan}Compile xScripts.rxdata...${color_reset}"
ruby rpgscript.rb ./scripts "${steam_game_dir}"

# Install to Steam directory
echo "-> ${cyan}Install to Steam directory...${color_reset}"
cp "${steam_game_dir}/Data/xScripts.rxdata" .
rm -rf "${steam_game_dir}/OneShot.app"
rm -rf "${steam_game_dir}/_______.app"
cp -r OneShot.app "${steam_game_dir}/OneShot.app"
cp -r _______.app "${steam_game_dir}/_______.app"

# Cleanup
echo "-> ${cyan}Cleanup files...${color_reset}"
# make clean
rm -rf OneShot_new.app
rm -rf journal/mac/__pycache__
rm -rf build
rm -rf dist

echo "\n${green}Complete!  ${white}Please report any issues to https://github.com/vinyldarkscratch/mkxp-oneshot/issues${color_reset}"