# mkxp-oneshot

This is a specialized fork of [mkxp by Ancurio](https://github.com/Ancurio/mkxp) designed for [*OneShot*](http://oneshot-game.com/).

Thanks to [hunternet93](https://github.com/hunternet93) for starting the reimplementation of the journal program!

> mkxp is a project that seeks to provide a fully open source implementation of the Ruby Game Scripting System (RGSS) interface used in the popular game creation software "RPG Maker XP", "RPG Maker VX" and "RPG Maker VX Ace" (trademark by Enterbrain, Inc.), with focus on Linux. The goal is to be able to run games created with the above software natively without changing a single file.
>
> It is licensed under the GNU General Public License v2+.

*OneShot* also makes use of [steamshim](https://hg.icculus.org/icculus/steamshim/) for GPL compliance while making use of Steamworks features. See LICENSE.steamshim.txt for details.

## Building (Supported on Windows, Ubuntu Linux, in progress on macOS)

Preface: This only supports Visual Studio on Windows and Xcode on macOS. Ubuntu should work with either GCC or clang. You can probably compile with other platforms/setups, but beware.

With Python 3 and pip installed, install Conan via `pip3 install conan`. Afterwards, add the necessary package repositories by adding running the following commands:

```sh
conan remote add eliza https://rkevin.jfrog.io/artifactory/api/conan/eliza
conan remote add bincrafters https://bincrafters.jfrog.io/artifactory/api/conan/public-conan
conan config set general.revisions_enabled=1
setx CONAN_USE_ALWAYS_SHORT_PATHS 1 (windows only)
```

Prepare to build *OneShot* by installing the necessary dependencies with Conan.

```sh
cd mkxp-oneshot
mkdir build
cd build
conan install .. --build=missing
```

Hopefully, this should complete without error. It may take quite a while to build all of the dependencies.

On Ubuntu, make sure you install the necessary dependencies before building *OneShot* proper:

```sh
sudo apt install libgtk2.0-dev libxfconf-0-dev
```

Finally, you can build the project by running the following:

```sh
conan build ..
```

On Linux, you likely want to generate an AppImage. Please refer to how to build the Journal app below, as this is a prerequisite for building the AppImage. Afterwards, you may run the command, from the root directory of the repository:

```sh
./make-appimage.sh . build /path/to/game/files /path/to/journal/_______ /some/path/OneShot.AppImage`
```

Requires [linuxdeploy](https://github.com/linuxdeploy/linuxdeploy) and [AppImageTool](https://github.com/AppImage/AppImageKit) in your `PATH`.

## Building the Journal app on Unix systems

As a prerequisite on Ubuntu, ensure that the following packages are installed.

```sh
sudo apt install python3-venv libxcb-xinerama
```

Then run the script. From the root of the repository:

```sh
./make-journal-linux.sh . /path/to/journal/parent/directory/
```

This will generate a file called `_______`.

### Supported image/audio formats
These depend on the SDL auxiliary libraries. *OneShot* only makes use of bmp/png for images and oggvorbis/wav for audio.

To run *OneShot*, you should have a graphics card capable of at least **OpenGL (ES) 2.0** with an up-to-date driver installed.

## Configuration

*OneShot* reads configuration data from the file "oneshot.conf". The format is ini-style. Do *not* use quotes around file paths (spaces won't break). Lines starting with '#' are comments. See 'oneshot.conf.sample' for a list of accepted entries.

All option entries can alternatively be specified as command line options. Any options that are not arrays (eg. preloaded scripts) specified as command line options will override entries in oneshot.conf. Note that you will have to wrap values containing spaces in quotes (unlike in oneshot.conf).

The syntax is: `--<option>=<value>`

Example: `./oneshot --gameFolder="oneshot" --vsync=true`
