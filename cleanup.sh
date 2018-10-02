#!/bin/sh

rm -rf journal/mac/__pycache__
rm -rf steamshim_parent/steamshim
rm -rf build
rm -rf dist
rm -rf _______.app
rm -rf OneShot.app
rm -rf OneShot
rm -rf conanbuildinfo.cmake
rm -rf CMakeFiles
rm -rf cmake_install.cmake
rm -rf CMakeCache.txt
rm -rf conanbuildinfo.txt
rm -rf conaninfo.txt
if [ -f .qmake.stash ]
then
	make distclean
else 
	echo ""
fi