# Modshot-Core

This is a even more specialized fork of a specialized fork of [mkxp by Ancurio](https://github.com/Ancurio/mkxp) designed for [*OneShot*](http://oneshot-game.com/) for OneShot mods.

Thanks to [hunternet93](https://github.com/hunternet93) for starting the reimplementation of the journal program!

Thanks to [rkevin-arch](https://github.com/rkevin-arch) for the docker build!

> mkxp is a project that seeks to provide a fully open source implementation of the Ruby Game Scripting System (RGSS) interface used in the popular game creation software "RPG Maker XP", "RPG Maker VX" and "RPG Maker VX Ace" (trademark by Enterbrain, Inc.), with focus on Linux. The goal is to be able to run games created with the above software natively without changing a single file.
>
> It is licensed under the GNU General Public License v2+.

*OneShot* also makes use of [steamshim](https://hg.icculus.org/icculus/steamshim/) for GPL compliance while making use of Steamworks features. See LICENSE.steamshim.txt for details.

# Purpose

> Modshot makes full use of all of these and is designed to add features not added in vanilla OneShot. It adds a number of new features and aims to make modding easier, whilst adding general purpose and specialized features, such as custom window titles, discord rich presence, chroma support, and much more. With this, oneshot now reads Scripts.rxdata instead of xScripts.rxdata, meaning modders won't have to delete and rename files constantly. Feel free to make pull requests of features you would like to see.

# Config

> Using features such as the GameJolt API, Discord Richpresence, and anything else that requires such data will need to be edited with in a config file inside the executable. This is for security purposes so nobody can steal your game/application keys. I apologize for the inconvenience. 

# Usage
Please credit the project in some wayy, either by a direct title card, or some other means.

<There are a large multitude of additional features modshot adds and it is iddifuclt to explain them all. Please refer to the wiki for in-depth usage.>
  
 Main features currently:
 
 Ruby gem support
 
 Vertical sprite mirroring
 
 Net::HTTP support
 
 Custom audio bindings
 
 Docker containers
 
 Simplified build process
 

# Wiki

> A wiki is in progress and will be made when more features are added.

### Supported image/audio formats
These depend on the SDL auxiliary libraries. *OneShot* only makes use of bmp/png for images and oggvorbis/wav for audio.

To run *OneShot*, you should have a graphics card capable of at least **OpenGL (ES) 2.0** with an up-to-date driver installed.

## Configuration

*OneShot* reads configuration data from the file "oneshot.conf". The format is ini-style. Do *not* use quotes around file paths (spaces won't break). Lines starting with '#' are comments. See 'oneshot.conf.sample' for a list of accepted entries.

All option entries can alternatively be specified as command line options. Any options that are not arrays (eg. preloaded scripts) specified as command line options will override entries in oneshot.conf. Note that you will have to wrap values containing spaces in quotes (unlike in oneshot.conf).

The syntax is: `--<option>=<value>`

Example: `./oneshot --gameFolder="oneshot" --vsync=true`

