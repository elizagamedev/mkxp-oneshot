#!/bin/sh
set -e

# Credit to popkirby on GitHub for creating the majority of this file

cd `dirname $0`

# Colors
white="\033[0;37m"      # White - Regular
bold="\033[1;37m"       # White - Bold
cyan="\033[1;36m"       # Cyan - Bold
green="\033[1;32m"      # Green - Bold
color_reset="\033[0m"   # Reset Colors

ONESHOT_PATH=$HOME/Library/Application\ Support/Steam/steamapps/common/OneShot

echo "-> ${cyan}Removing old OneShot apps...${color_reset}"
rm -rf "$ONESHOT_PATH/OneShot.app"
rm -rf "$ONESHOT_PATH/_______.app"
rm -f "$ONESHOT_PATH/Data/xScripts.rxdata"
rm -rf "$ONESHOT_PATH/*.exe"
rm -rf "$ONESHOT_PATH/*.dll"

echo "-> ${cyan}Install OneShot apps to Steam directory...${color_reset}"
cp -rf "." "$ONESHOT_PATH"
rm -f "$ONESHOT_PATH/install.command"
ln -sfh "$ONESHOT_PATH/OneShot.app" "$HOME/Applications/OneShot.app"

rm -f /tmp/oneshot-pipe # Important for pre-release version cleanup

echo "\n${green}Complete!  ${white}Please report any issues to https://github.com/vinyldarkscratch/synglechance/issues${color_reset}"
open "$ONESHOT_PATH"