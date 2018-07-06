#!/bin/sh
set -e

cd `dirname $0`

# User-configurable variables
linux_version="0.1.0"
make_threads=8
ONESHOT_PATH=$HOME/.steam/steam/steamapps/common/OneShot

# Colors
white="\033[0;37m"      # White - Regular
bold="\033[1;37m"       # White - Bold
cyan="\033[1;36m"       # Cyan - Bold
green="\033[1;32m"      # Green - Bold
color_reset="\033[0m"   # Reset Colors

echo "${white}Compiling ${bold}SyngleChance v${linux_version} ${white}engine for Linux...${color_reset}\n"

# Generate makefile and build main + journal
echo "-> ${cyan}Generate makefile...${color_reset}"
qmake
echo "-> ${cyan}Compile engine...${color_reset}"
make -j${make_threads}
echo "-> ${cyan}Compile journal...${color_reset}"
pyinstaller journal/unix/journal.spec --onefile --windowed

# Set version number
echo "-> ${cyan}Set version number...${color_reset}"

# Compile scripts
echo "-> ${cyan}Compile xScripts.rxdata...${color_reset}"
ruby rpgscript.rb ./scripts "$ONESHOT_PATH"

echo "-> ${cyan}Install OneShot apps to Steam directory...${color_reset}"
cp -rf "./OneShot" "$ONESHOT_PATH"
cp -rf "./dist/_______" "$ONESHOT_PATH"

# Cleanup
echo "-> ${cyan}Cleanup files...${color_reset}"
rm -rf journal/unix/__pycache__
rm -rf build
rm -rf dist

echo "\n${green}Complete!  ${white}Please report any issues to https://github.com/GooborgStudios/synglechance/issues${color_reset}"
