:: docker entrypoint for building oneshot on windows

@echo off
:: there is no "set -e" equivalent for cmd
:: so bear with me while i add ||s everywhere

if not exist C:\work\build mkdir C:\work\build
cd \work\build

conan install \work\src --build=missing || (echo CONAN INSTALL FAILED! && exit 1)
:startbuild
conan build \work\src || (echo CONAN BUILD FAILED! && goto end)
:: TODO: does this build the journal?

robocopy C:\work\build\bin C:\work\dist || (echo COPY BINARIES FAILED! && goto end)
if not exist C:\work\data exit
robocopy C:\work\data C:\work\dist Audio Data Fonts Graphics Languages Wallpaper || (echo COPY GAME DATA FAILED! && goto end)

echo ModShot has been built. Enjoy.
:end
echo Press any key to rebuild, or Ctrl+C to quit the container.
pause >nul
echo Rebuilding...
goto startbuild
