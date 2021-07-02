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

conan install /work/src --build=missing -o platform=steam
conan build /work/src

dos2unix /work/src/make-linux-dist.sh
/work/src/make-linux-dist.sh /work/build /work/dist /work/data
