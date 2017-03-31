#!/bin/sh

mac_version="0.3.1"

# Generate makefile and build main + journal
qmake MRIVERSION=2.3
make -j4
pyinstaller journal/mac/journal.spec --onefile --windowed

# Create app bundles
mv OneShot.app OneShot_new.app
cp -r patches/mac/OneShot_template.app ./OneShot.app
cp OneShot_new.app/Contents/MacOS/OneShot OneShot.app/Contents/Resources/OneShot
cp -r dist/_______.app _______.app

# Set version number
rm OneShot.app/Contents/Info.plist
rm _______.app/Contents/Info.plist
m4 patches/mac/Info.plist.in -DONESHOTMACVERSION=$mac_version > OneShot.app/Contents/Info.plist
m4 patches/mac/JournalInfo.plist.in -DONESHOTMACVERSION=$mac_version > _______.app/Contents/Info.plist

# Compile scripts and install to Steam directory
ruby rpgscript.rb ./scripts ~/Library/Application\ Support/Steam/SteamApps/common/OneShot/
cp ~/Library/Application\ Support/Steam/SteamApps/common/OneShot/Data/xScripts.rxdata .
rm -r ~/Library/Application\ Support/Steam/SteamApps/common/OneShot/OneShot.app
rm -r ~/Library/Application\ Support/Steam/SteamApps/common/OneShot/_______.app
cp -r OneShot.app ~/Library/Application\ Support/Steam/SteamApps/common/OneShot/OneShot.app
cp -r _______.app ~/Library/Application\ Support/Steam/SteamApps/common/OneShot/_______.app

# Cleanup
make clean
rm -r OneShot_new.app
rm -r journal/mac/__pycache__
rm -r build
rm -r dist