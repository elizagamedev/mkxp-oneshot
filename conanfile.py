import os.path

from conans import CMake, ConanFile, tools


class MkxpConan(ConanFile):
    name = "oneshot"
    version = "0.0.0"
    license = "GPLv2"
    url = "https://github.com/elizagamedev/mkxp-oneshot"
    description = "OneShot game runtime"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    exports_sources = "*"
    requires = (
        "boost/1.73.0",
        "openal/1.18.2@bincrafters/stable",
        "physfs/3.0.1@bincrafters/stable",
        "pixman/0.34.0@bincrafters/stable",
        "ruby/2.5.3@eliza/testing",
        "sdl2/2.0.9@bincrafters/stable",
        "sdl2_image/2.0.5@bincrafters/stable",
        "sdl2_ttf/2.0.15@bincrafters/stable",
        "sdl_sound-mkxp/1.0.1@eliza/stable",
        "sigc++/2.10.0@bincrafters/stable",
        # Overrides
        "libpng/1.6.37",
        "zlib/1.2.11",
        "bzip2/1.0.8",
    )
    build_requires = ("ruby_installer/2.5.5@bincrafters/stable", )
    options = {
        "platform": ["standalone", "steam"],
    }
    default_options = (
        "platform=standalone",
        "boost:without_test=True",
        "cygwin_installer:packages=xxd",
        # Avoid dead url bitrot in cygwin_installer
        "cygwin_installer:with_pear=False",
    )

    #def build_requirements(self):
       # if tools.os_info.is_windows:
            # self.build_requires("cygwin_installer/2.9.0@bincrafters/stable")

    def requirements(self):
        if self.options.platform == "steam":
            self.requires("steamworks/1.42@eliza/stable")
        if tools.os_info.is_linux:
            # Overrides
            self.requires("sqlite3/3.29.0")
            self.requires("flac/1.3.3")
            self.requires("ogg/1.3.4")
            self.requires("vorbis/1.3.6")
            self.requires("libalsa/1.1.9")

    def configure(self):
        if tools.os_info.is_windows:
            # ???
            self.options["openal"].shared = True
            # Fix linker error in SDL_sound fork with SDL2
            self.options["sdl2"].shared = True

    def build_configure(self):
        # if we are on windows, git might clone files using the CRLF newline
        # this will cause issues when building for linux, as some scripts cannot run properly
        # we run dos2unix on them to convert their line endings
        for file in ['make-appimage.sh', 'assets/AppRun', 'assets/oneshot.desktop']:
            try:
                tools.dos2unix(os.path.join(self.source_folder, file))
            except FileNotFoundError:
                pass # in case windows users don't need those files

        cmake = CMake(self, msbuild_verbosity='minimal')
        if self.options.platform == "steam":
            cmake.definitions["STEAM"] = "ON"
        cmake.configure()
        cmake.build()

    def build(self):
        #if tools.os_info.is_windows:
        #    cygwin_bin = self.deps_env_info["cygwin_installer"].CYGWIN_BIN
        #    with tools.environment_append({
        #            "PATH": [cygwin_bin],
        #            "CONAN_BASH_PATH":
        #            os.path.join(cygwin_bin, "bash.exe")
        #    }):
        #        self.build_configure()
        #else:
        #    self.build_configure()
        self.build_configure()

    def package(self):
        self.copy("*", dst="bin", src="bin")

    def imports(self):
        self.do_copy_deps(self.copy)

    def deploy(self):
        self.copy("*")
        self.do_copy_deps(self.copy_deps)

    def do_copy_deps(self, copy):
        deps = set(self.deps_cpp_info.deps) - set(
            ("cygwin_installer", "msys2_installer", "ruby_installer"))
        for dep in deps:
            copy("*.dll",
                 dst="bin",
                 src="bin",
                 root_package=dep,
                 keep_path=False)
            copy("*.so*",
                 dst="lib",
                 src="lib",
                 root_package=dep,
                 keep_path=True)
            if self.settings.build_type == "Debug":
                copy("*.pdb", dst="bin", root_package=dep, keep_path=False)
