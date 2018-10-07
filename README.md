# mkxp-oneshot

This is a specialized fork of [mkxp by Ancurio](https://github.com/Ancurio/mkxp) designed for [*OneShot*](http://oneshot-game.com/).

Thanks to [hunternet93](https://github.com/hunternet93) for starting the reimplementation of the journal program!

> mkxp is a project that seeks to provide a fully open source implementation of the Ruby Game Scripting System (RGSS) interface used in the popular game creation software "RPG Maker XP", "RPG Maker VX" and "RPG Maker VX Ace" (trademark by Enterbrain, Inc.), with focus on Linux. The goal is to be able to run games created with the above software natively without changing a single file.
>
> It is licensed under the GNU General Public License v2+.

*SyngleChance* also makes use of [steamshim](https://hg.icculus.org/icculus/steamshim/) for GPL compliance while making use of Steamworks features. See LICENSE.steamshim.txt for details.

## Dependencies / Building

* Qt5
* CMake
* Conan
* Boost.Unordered (headers only)
* Boost.Program_options
* Boost.CRC
* Vim XXD (vim-common in apt)
* libsigc++ 2.0
* PhysFS (latest hg)
* OpenAL
* SDL2
* SDL2_image
* SDL2_ttf
* [Ancurio's SDL_sound fork](https://github.com/Ancurio/SDL_sound)
* vorbisfile
* pixman
* OpenGL header (alternatively GLES2 with `DEFINES+=GLES2_HEADER`)
* Ruby
* Python 3 (journal reimplementation only)
* PyQt5 for Python 3 (journal reimplementation only)
* Steamworks SDK (optional, place contents of sdk folder in ZIP into "steamworks" folder in root)

### Building with Conan

*SyngleChance* used to employ the `qmake` build system, but is now migrating to `conan`, so you'll need to install that beforehand.  You'll also want to add the following remotes to `conan`:

```sh
conan remote add eliza "https://api.bintray.com/conan/eliza/conan"
conan remote add gooborg "https://api.bintray.com/conan/gooborgstudios/conan"
conan remote add bincrafters "https://api.bintray.com/conan/bincrafters/public-conan"
```

On Windows, you may need to edit `C:\Users\[username]\.conan\profiles\default` and add the following line in `[settings]` ("v141" is the current toolset version at the time of this writing):

```sh
compiler.toolset=v141
```

Next, you can install all the required packages:

```sh
conan install . --build=missing
```

Finally, you can build the project by running the following:

```sh
conan build .
```

### Building with QMake (Old)

The `make-oneshot-mac.command` and `make-oneshot-linux.sh` files automate the entire process for you.  Just run the applicable file to compile and install the engine into OneShot's default Steam directory.

qmake will use pkg-config to locate the respective include/library paths. If you installed any dependencies into non-standard prefixes, make sure to adjust your `PKG_CONFIG_PATH` variable accordingly.

The exception is boost, which is weird in that it still hasn't managed to pull off pkg-config support (seriously?). *If you installed boost in a non-standard prefix*, you will need to pass its include path via `BOOST_I` and library path via `BOOST_L`, either as direct arguments to qmake (`qmake BOOST_I="/usr/include" ...`) or via environment variables. You can specify a library suffix (eg. "-mt") via `BOOST_LIB_SUFFIX` if needed.

By default, *SyngleChance* switches into the directory where its binary is contained and then starts reading the configuration and resolving relative paths. In case this is undesired (eg. when the binary is to be installed to a system global, read-only location), it can be turned off by adding `DEFINES+=WORKDIR_CURRENT` to qmake's arguments.

pkg-config will look for `ruby-2.3.pc`, but you can override the version with `MRIVERSION=2.5` ('2.5' being an example). This is the default binding, so no arguments to qmake needed (`BINDING=MRI` to be explicit).

### Supported image/audio formats
These depend on the SDL auxiliary libraries. *SyngleChance* only makes use of bmp/png for images and oggvorbis/wav for audio.

To run *SyngleChance*, you should have a graphics card capable of at least **OpenGL (ES) 2.0** with an up-to-date driver installed.

## Configuration

*SyngleChance* reads configuration data from the file "oneshot.conf". The format is ini-style. Do *not* use quotes around file paths (spaces won't break). Lines starting with '#' are comments. See 'oneshot.conf.sample' for a list of accepted entries.

All option entries can alternatively be specified as command line options. Any options that are not arrays (eg. preloaded scripts) specified as command line options will override entries in oneshot.conf. Note that you will have to wrap values containing spaces in quotes (unlike in oneshot.conf).

The syntax is: `--<option>=<value>`

Example: `./oneshot --gameFolder="oneshot" --vsync=true`
