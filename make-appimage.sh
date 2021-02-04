#!/bin/bash -eu

LINUXDEPLOY=linuxdeploy-x86_64.AppImage
APPIMAGETOOL=appimagetool-x86_64.AppImage
GDK_LOADERS_PATH=/usr/lib/x86_64-linux-gnu/gdk-pixbuf-2.0/2.10.0

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
	     -l "$GDK_LOADERS_PATH/loaders/io-wmf.so" \
             -l "$GDK_LOADERS_PATH/loaders/libpixbufloader-bmp.so" \
	     -l "$GDK_LOADERS_PATH/loaders/libpixbufloader-icns.so" \
	     -l "$GDK_LOADERS_PATH/loaders/libpixbufloader-jpeg.so" \
	     -l "$GDK_LOADERS_PATH/loaders/libpixbufloader-pnm.so" \
	     -l "$GDK_LOADERS_PATH/loaders/libpixbufloader-svg.so" \
	     -l "$GDK_LOADERS_PATH/loaders/libpixbufloader-tiff.so" \
	     -l "$GDK_LOADERS_PATH/loaders/libpixbufloader-xpm.so" \
	     -l "$GDK_LOADERS_PATH/loaders/libpixbufloader-ani.so" \
	     -l "$GDK_LOADERS_PATH/loaders/libpixbufloader-gif.so" \
	     -l "$GDK_LOADERS_PATH/loaders/libpixbufloader-ico.so" \
	     -l "$GDK_LOADERS_PATH/loaders/libpixbufloader-png.so" \
	     -l "$GDK_LOADERS_PATH/loaders/libpixbufloader-qtif.so" \
	     -l "$GDK_LOADERS_PATH/loaders/libpixbufloader-tga.so" \
	     -l "$GDK_LOADERS_PATH/loaders/libpixbufloader-xbm.so" \
	     -i "$source_path/assets/oneshot.png" \
	     -d "$source_path/assets/oneshot.desktop" \
	     --custom-apprun="$source_path/assets/AppRun" \
	     --appdir "$appdir"

# Copy ruby standard library and ssl cert bundle
cp -af "$conan_install_path/bin/lib/" "$appdir/usr/bin/lib"
cp -af "$conan_install_path/bin/ssl/" "$appdir/usr/bin/ssl"

# Bundle game data
cp -af "$journal_file" "$appdir/usr/bin/_______"
cp -af "$source_path/assets/journal.png" "$appdir/usr/bin/_______.png"
for dir in Audio Data Fonts Graphics Languages Wallpaper testing_saves testing_saves_postgame; do
  [ -d "$game_path/$dir/" ] && rsync -a "$game_path/$dir" "$appdir/usr/bin/"
  [ -d "$unix_content_path/$dir/" ] && rsync -a "$unix_content_path/$dir/" "$appdir/usr/bin/$dir/"
done
# GDK pixbuf retarget nonsense
sed "s,$GDK_LOADERS_PATH/loaders/,%RETARGET%/," \
    "$GDK_LOADERS_PATH/loaders.cache" > "$appdir/usr/lib/loaders.cache"
# Delete some files
find "$appdir/usr/bin" -type f \( \
     -iname '*.bmp' -o \
     -iname 'thumbs.db' -o \
     -iname '*.ini' -o \
     -iname '*conflicted*' \) \
     -delete
# copyright :(
find "$appdir/usr/bin" -type f \( -iname '*.loc' -a \! -iname 'en.loc' \) -delete
# more copyright :(
find "$appdir/usr/bin" -type d \( \
     -iname 'es' -o \
     -iname 'fr' -o \
     -iname 'ja' -o \
     -iname 'ko' -o \
     -iname 'pt_BR' -o \
     -iname 'zh_CN' \) \
     -exec rm -rf {} +
# International fonts are now taking up space
find "$appdir/usr/bin/Fonts" -type f \! -iname 'TerminusTTF-Bold.ttf' -delete
# no need for xScripts anymore
#cp -af "$conan_install_path/bin/Data/xScripts.rxdata" "$appdir/usr/bin/Data/"

# Create AppImage
ARCH=x86_64 $APPIMAGETOOL "$appdir" "$appimage_file"
