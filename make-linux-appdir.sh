#!/bin/bash -eu

LINUXDEPLOY=linuxdeploy-x86_64.AppImage
GDK_LOADERS_PATH=/usr/lib/x86_64-linux-gnu/gdk-pixbuf-2.0/2.10.0

if [ $# -lt 3 ]; then
  echo "usage: make-linux-appdir.sh SOURCE_PATH CONAN_INSTALL_PATH APPDIR"
  exit 1
fi

source_path="$1"
conan_install_path="$2"
appdir="$3"
tmp_appdir="$(mktemp -d)"
function atexit {
  [ -n "$tmp_appdir" ] && rm -rf "$tmp_appdir"
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
	     --appdir "$tmp_appdir"
shopt -s dotglob
cp -af "$tmp_appdir"/* "$appdir"


# Copy ruby standard library and ssl cert bundle
cp -af "$conan_install_path/bin/lib/" "$appdir/usr/bin/lib"
cp -af "$conan_install_path/bin/ssl/" "$appdir/usr/bin/ssl"

# GDK pixbuf retarget nonsense
sed "s,$GDK_LOADERS_PATH/loaders/,%RETARGET%/," \
    "$GDK_LOADERS_PATH/loaders.cache" > "$appdir/usr/lib/loaders.cache"
