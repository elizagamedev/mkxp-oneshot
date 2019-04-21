#!/bin/sh
set -e

cd `dirname $0`

# User-configurable variables.
linux_version="0.1.0"
make_threads=8
oneshot_id=420530
ONESHOT_PATH=$HOME/.local/share/Steam/steamapps/common/OneShot
STEAMWORKS_PATH=$(realpath ..)/steamworks

# Colors.
white="\033[0;37m"      # White - Regular
bold="\033[1;37m"       # White - Bold
cyan="\033[1;36m"       # Cyan - Bold
green="\033[1;32m"      # Green - Bold
color_reset="\033[0m"   # Reset Colors

echo -e "${white}Compiling ${bold}SyngleChance v${linux_version} ${white}engine for Linux...${color_reset}\n"

# Generate makefile.
echo -e "-> ${cyan}Generate makefile...${color_reset}"
export MRIVERSION=$(echo "puts RUBY_VERSION.split('.').slice(0, 2).join('.')" | ruby)
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig
qmake mkxp.pro MRIVERSION=2.3 > oneshot.qmake.out

# Compile OneShot.
echo -e "-> ${cyan}Compile engine...${color_reset}"
make -j${make_threads} > oneshot.make.out

# Compile steamshim.
echo -e "-> ${cyan}Compile steamshim...${color_reset}"
cd steamshim_parent
mkdir build
cd build
cmake -DSTEAMWORKS_PATH=${STEAMWORKS_PATH} .. > steamshim.cmake.out
cp "$STEAMWORKS_PATH/redistributable_bin/linux64/libsteam_api.so" .
make -j${make_threads} > steamshim.make.out
cd ../..

# Compile Journal.
echo -e "-> ${cyan}Compile journal...${color_reset}"
pyinstaller journal/unix/journal.spec --windowed

# Compile scripts.
echo -e "-> ${cyan}Compile xScripts.rxdata...${color_reset}"
ruby rpgscript.rb ./scripts "$ONESHOT_PATH" > rpgscript.out

# Copy results.
echo -e "-> ${cyan}Install OneShot apps to Steam directory...${color_reset}"
yes | cp -r dist/_______/* "$ONESHOT_PATH"
yes | cp oneshot "$ONESHOT_PATH"
yes | cp steamshim_parent/build/steamshim "$ONESHOT_PATH"
echo "$oneshot_id" > "$ONESHOT_PATH/steam_appid.txt"

# Copy libraries.
echo -e "-> ${cyan}Install OneShot libraries to Steam directory...${color_reset}"
mkdir libs
ldd oneshot | ruby libraries.rb
ldd steamshim_parent/build/steamshim | ruby libraries.rb
yes | cp libs/* "$ONESHOT_PATH"

# Cleanup.
echo -e "-> ${cyan}Cleanup files...${color_reset}"
rm -rf journal/unix/__pycache__
rm -rf build
rm -rf dist
rm -rf steamshim_parent/build
rm -rf libs
make clean > clean.out
rm -f *.out
rm Makefile
rm oneshot
rm .qmake.stash

echo -e "\n${green}Complete!  ${white}Please report any issues to https://github.com/GooborgStudios/synglechance/issues${color_reset}"
