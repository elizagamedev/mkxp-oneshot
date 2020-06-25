# gcc10 doesn't like me, and it seems the official appimage is build using gcc 5.4.0-6ubuntu1~16.04.12
FROM conanio/gcc5
RUN conan remote add eliza "https://api.bintray.com/conan/eliza/conan" && \
conan remote add bincrafters "https://api.bintray.com/conan/bincrafters/public-conan"

RUN sudo apt update && DEBIAN_FRONTEND=noninteractive sudo apt install -y xorg-dev libx11-xcb-dev libxcb-render0-dev libxcb-render-util0-dev libgtk2.0-dev libxfconf-0-dev vim libwmf0.2-7-gtk librsvg2-common rsync
RUN sudo python -m pip install mako pyqt5 pyinstaller

RUN conan install boost/1.73.0@_/_ --build=missing -o boost:without_test=True
RUN conan install openal/1.18.2@bincrafters/stable --build=missing
RUN conan install physfs/3.0.1@bincrafters/stable --build=missing
RUN conan install pixman/0.34.0@bincrafters/stable --build=missing
RUN conan install ruby/2.5.3@eliza/testing --build=missing
RUN conan install sdl2/2.0.9@bincrafters/stable --build=missing
RUN conan install sdl2_image/2.0.5@bincrafters/stable --build=missing
RUN conan install sdl2_ttf/2.0.15@bincrafters/stable --build=missing
RUN conan install sdl_sound-mkxp/1.0.1@eliza/stable --build=missing
RUN conan install sigc++/2.10.0@bincrafters/stable --build=missing

ADD https://github.com/AppImage/AppImageKit/releases/download/12/appimagetool-x86_64.AppImage /tmp
# can't find a stable release for linuxdeploy, oh well
ADD https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage /tmp
# avoid having to run appimages by itself
# we don't want to force CAP_SYS_ADMIN into the container if possible, but running appimages require FUSE
# instead, we extract it and pray it works (it actually does!)
RUN cd /tmp && sudo chmod 755 appimagetool-x86_64.AppImage linuxdeploy-x86_64.AppImage && \
./appimagetool-x86_64.AppImage --appimage-extract && sudo mv squashfs-root /usr/local/bin/appimagetool && \
./linuxdeploy-x86_64.AppImage --appimage-extract && sudo mv squashfs-root /usr/local/bin/linuxdeploy && \
sudo ln -s /usr/local/bin/appimagetool/AppRun /usr/local/bin/appimagetool-x86_64.AppImage && \
sudo ln -s /usr/local/bin/linuxdeploy/AppRun /usr/local/bin/linuxdeploy-x86_64.AppImage

COPY build-entrypoint-linux.sh /
ENTRYPOINT ["/build-entrypoint-linux.sh"]

# build this docker image as follows:
# docker build -t oneshot-build-linux .

# run this docker image as follows:
# docker run -i -v /path/to/modshot-core:/work/src -v /path/to/gamefiles:/work/data -v /path/to/distrubution/folder:/work/dist oneshot-build-linux
# example that worked for me:
# docker run -i -v ~/o/Modshot-Core:/work/src -v ~/o/OneShotBkup:/work/data -v ~/o/conanbuildtest:/work/dist oneshot-build-linux
