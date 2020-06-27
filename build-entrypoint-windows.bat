:: there is no "set -e" equivalent for cmd
:: so bear with me while i add ||s everywhere

if not exist C:\work\build mkdir C:\work\build
cd \work\build

conan install \work\src --build=missing || (echo "CONAN INSTALL FAILED!" && exit /B 1)
conan build \work\src || (echo "CONAN BUILD FAILED!" && exit /B 1)
:: TODO: does this build the journal?

robocopy C:\work\build\bin C:\work\dist || (echo "COPY BINARIES FAILED!" && exit /B 1)
if not exist C:\work\data exit
robocopy C:\work\data C:\work\dist Audio Data Fonts Graphics Languages Wallpaper || (echo "COPY GAME DATA FAILED!" && exit /B 1)

echo "ModShot has been built. Enjoy."
