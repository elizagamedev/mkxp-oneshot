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

echo "-> ${cyan}Extracting images from EXE...${color_reset}"
./patches/mac/bin/upx -d -o ./uncompressed.exe "$ONESHOT_PATH/_______.exe"
./patches/mac/bin/extract-bmp

echo "-> ${cyan}Converting bitmap images to PNG...${color_reset}"
for bmp in ./images/*.bmp; do
  ./patches/mac/bin/convert "$bmp" -alpha on -transparent "#00ff00" "./images/$(basename "$bmp" .bmp).png"
done

echo "-> ${cyan}Copy images to application...${color_reset}"
cp -rf ./images ./_______.app/Contents/Resources/

echo "-> ${cyan}Install OneShot apps to Steam directory...${color_reset}"
cp -f "./xScripts.rxdata" "$ONESHOT_PATH/Data"
cp -rf "./OneShot.app" "$ONESHOT_PATH/OneShot.app"
cp -rf "./_______.app" "$ONESHOT_PATH/_______.app"
ln -sfh "$ONESHOT_PATH/OneShot.app" "$HOME/Applications/OneShot.app"

echo "-> ${cyan}Cleaning up...${color_reset}"
rm ./images/*.bmp

echo "\n${green}Complete!  ${white}Please report any issues to https://github.com/vinyldarkscratch/synglechance/issues${color_reset}"
open "$ONESHOT_PATH"