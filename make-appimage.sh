#!/bin/bash -eu

LINUXDEPLOY=linuxdeploy-x86_64.AppImage
APPIMAGETOOL=appimagetool-x86_64.AppImage

if [ $# -lt 6 ]; then
  echo "usage: make-appimage.sh SOURCE_PATH CONAN_INSTALL_PATH GAME_PATH EXTRA_UNIX_CONTENT_PATH JOURNAL_FILE APPIMAGE_FILE"
  exit 1
fi

source_path="$1"
conan_install_path="$2"
game_path="$3"
unix_content_path="$4"
journal_file="$5"
appimage_file="$6"
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

# Bundle game data
cp -af "$journal_file" "$appdir/usr/bin/_______"
cp -af "$source_path/assets/journal.png" "$appdir/usr/bin/_______.png"
for dir in Audio Data Fonts Graphics Languages Wallpaper testing_saves testing_saves_postgame; do
  cp -ra "$game_path/$dir" "$appdir/usr/bin/"
  [ -d "$unix_content_path/$dir/" ] && rsync -a "$unix_content_path/$dir/" "$appdir/usr/bin/$dir/"
done
find "$appdir/usr/bin" \( -iname '*.bmp' -o -iname 'thumbs.db' -o -iname '*.ini' \) -delete
cp -af "$conan_install_path/bin/Data/xScripts.rxdata" "$appdir/usr/bin/Data/"

# Create AppImage
ARCH=x86_64 $APPIMAGETOOL "$appdir" "$appimage_file"
