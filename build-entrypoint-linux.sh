#!/bin/bash
# docker entrypoint to build on linux

set -e

# directories
# /work/src: source code, mount mkxp-oneshot or Modshot-Core here
# /work/data: game data, mount the folder containing all the game files like Data, Graphics, etc here
# /work/dist: folder used for distribution, mount the folder you want to contain the output appimage here
# /work/build: will be automatically made, contains intermediate build files, you can mount something here if you want partial compilation

sudo chown `id -u` /work
[ ! -d /work/build ] && mkdir /work/build
cd /work/build

conan install /work/src --build=missing
conan build /work/src

# build journal if doesn't exist
[ ! -f /work/build/_______ ] && eval "$(pyenv init -)" && \
pyinstaller --onefile \
            --distpath /work/build \
            --workpath `mktemp -d` \
            --specpath=/work/src/journal/unix \
            /work/src/journal/unix/journal-linux.spec

# build appimage
if [ "$1" == '--keep-xscripts-behavior' ]; then
    KEEP_XSCRIPTS_BEHAVIOR=1 /work/src/make-appimage.sh /work/src /work/build /work/data /work/extra_unix_content /work/build/_______ /work/dist/ModShot.AppImage
else
    /work/src/make-appimage.sh /work/src /work/build /work/data /work/extra_unix_content /work/build/_______ /work/dist/ModShot.AppImage
fi
