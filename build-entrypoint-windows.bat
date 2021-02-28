:: docker entrypoint for building oneshot on windows

@echo off
:: there is no "set -e" equivalent for cmd
:: so bear with me while i add ||s everywhere

if not exist C:\work\build mkdir C:\work\build
cd \work\build

conan install \work\src --build=missing || (echo CONAN INSTALL FAILED! && exit 1)
:startbuild
conan build \work\src || (echo CONAN BUILD FAILED! && goto end)
:: TODO: compile the journal?

robocopy C:\work\build\bin\lib C:\work\dist\lib /e
robocopy C:\work\build\bin\ C:\work\dist\lib
if exist C:\work\data (robocopy C:\work\data C:\work\dist /e)

move C:\work\dist\lib\oneshot-shim.exe C:\work\dist\oneshot.exe

echo ModShot has been built. Enjoy.
:end
echo Press any key to rebuild, or Ctrl+C and then Y to quit the container.
pause >nul
echo Rebuilding...
goto startbuild
