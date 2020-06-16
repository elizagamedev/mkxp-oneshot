#!/bin/bash -eu

LINUXDEPLOY=linuxdeploy-x86_64.AppImage
APPIMAGETOOL=appimagetool-x86_64.AppImage

if [ $# -lt 3 ]; then
  echo "usage: linuxdeploy.sh SOURCE_PATH CONAN_INSTALL_PATH GAME_PATH APPIMAGE_FILE"
fi

source_path="$1"
conan_install_path="$2"
game_path="$3"
appimage_file="$4"
appdir="$(mktemp -d)"

function atexit {
  [ -n "$appdir" ] && rm -rf "$appdir"
}

trap atexit EXIT

$LINUXDEPLOY -e "$conan_install_path/bin/oneshot" \
	     -l "$conan_install_path/lib/libruby.so.2.5.3" \
	     -i "$source_path/assets/oneshot.png" \
	     -d "$source_path/assets/oneshot.desktop" \
	     --appdir "$appdir"

# Populate remaining AppImage spec files
# ln -sf bin/oneshot "$appdir/AppRun"
# cp -f "$source_path/assets/oneshot.png" "$appdir/oneshot.png"

# Bundle game data
for dir in Audio Data Fonts Graphics Languages Wallpaper; do
  cp -ra "$game_path/$dir" "$appdir/usr/bin/"
done
cp -a "$conan_install_path/bin/Data/xScripts.rxdata" "$appdir/usr/bin/Data/"

# Create AppImage
ARCH=x86_64 $APPIMAGETOOL "$appdir" "$appimage_file"
