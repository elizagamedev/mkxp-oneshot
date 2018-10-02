# Steamshim parent
OneShot uses [steamshim](https://hg.icculus.org/icculus/steamshim/) for GNU GPL v3 and Steam interoperability. There are five components that OneShot uses to communicate with Steam, in order:
1. Steamworks SDK (that can be obtained from [here](https://partner.steamgames.com)) that is used for directly communicating with Steam. It is closed-source, and because of license compatibility issues steamshim must be used as a layer between OneShot and Steamworks SDK to communicate with Steam.
2. steamshim parent (whose source is within this folder) is a separate application that will start up the main OneShot application and can communicate with the Steamworks SDK.
3. steamshim child (whose source is inside the `steamshim` folder) is the client used to communicate with the steamshim parent from the OneShot application through pipes.
4. Steam controller inside OneShot application (`src/steam.cpp`) controls the communication between OneShot and steamshim components.
5. Steam binding (`binding-mri/steam-binding.cpp`) creates Ruby objects so Steam functionalities (like achievements) can be controlled from the game's scripts/events and passes on the handling to the Steam controller

## Building
Download Steamworks SDK from [here](https://partner.steamgames.com) and run
```console
$ export LD_LIBRARY_PATH=/path/to/steamworks/sdk/redistributable_bin/platform
$ STEAMWORKS=/path/to/steamworks/sdk HOST=platform make
```
inside this directory, where `platform` can be `win64`, `osx32`, `linux64` or `linux32`

If you need debug output from the steamshim parent, compile with `DEBUG=1` in command-line arguments for `make`.

## Running
After building the steamshim parent, place it in the same directory as the OneShot application. Note that the OneShot application **must be named "oneshot"**. Changing the case ("OneShot") will make the game not launch.

To run the game with the Steam wrapper, run the steamshim parent.
