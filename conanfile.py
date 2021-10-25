import os.path
import datetime

from conans import CMake, ConanFile, tools
from conans.client.tools import win
from conans.errors import ConanException

MODSHOT_VERSION_H='''#ifndef MODSHOT_VERSION
#define MODSHOT_VERSION "%s"
#endif
'''

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
        "physfs/3.0.1@astrabit/testing",
        "pixman/0.34.0@astrabit/testing",
        "ruby/3.0.2@astrabit/testing",
        "sdl2_image/2.0.5@bincrafters/stable",
        "sdl2_ttf/2.0.15@bincrafters/stable",
        "sdl_sound-mkxp/1.0.1@eliza/stable",
        "sigc++/2.10.0@astrabit/testing",
        # Overrides
        "libpng/1.6.37",
        "zlib/1.2.11",
        "bzip2/1.0.8",
    )
    build_requires = ("ruby_installer/3.0.2@astrabit/testing")
    options = {
        "platform": ["standalone", "steam"],
        "msys2": [True, False],
    }
    default_options = (
        "platform=standalone",
        "boost:without_test=True",
        "cygwin_installer:packages=xxd",
        # Avoid dead url bitrot in cygwin_installer
        "cygwin_installer:with_pear=False",
        "ruby:with_openssl=True",
        "msys2=False",
    )

    #def build_requirements(self):
       # if tools.os_info.is_windows:
            # self.build_requires("cygwin_installer/2.9.0@bincrafters/stable")

    def requirements(self):
        if tools.os_info.is_linux:
            # Overrides
            self.requires("sqlite3/3.29.0")
            self.requires("flac/1.3.3")
            self.requires("ogg/1.3.4")
            self.requires("vorbis/1.3.6")
            self.requires("libalsa/1.1.9")
            self.requires("sdl2/2.0.9@bincrafters/stable")
            self.requires("openssl/1.1.1l")
        if tools.os_info.is_windows:
            self.requires("sdl2/2.0.14@bincrafters/stable")
            self.requires("openssl/1.1.1k")
    
    def build_requirements(self):
        if self.options.msys2:
            self.build_requires("msys2/cci.latest")
            self.build_requires("mingw-w64/8.1")
	    self.conf_info["tools.microsoft.bash:subsystem"] = "msys2";
            self.conf_info["tools.microsoft.bash:path"] =  "C:\msys64\usr\bin\bash.exe";

    def configure(self):
        if tools.os_info.is_windows:
            # ???
            self.options["openal"].shared = True
            # Fix linker error in SDL_sound fork with SDL2
            self.options["sdl2"].shared = True
            self.options["openssl"].shared = True
            win_bash = self.settings.compiler != "Visual Studio"
            if win_bash:
                self.win_bash = win_bash

    def generate_version_number(self):
        try:
            git = tools.Git(self.source_folder)
            revision = git.get_commit()[0:7]
            if not git.is_pristine():
                revision += "-dirty"
        except ConanException:
            # this is either not a git repo, or we don't have git on the system
            revision = "nongit"
        revision += datetime.datetime.now().strftime("-%y%m%d-%H%M%S")

        tools.save(os.path.join(self.source_folder, 'src/version.h'), MODSHOT_VERSION_H%revision)

    def build_configure(self):
        self.generate_version_number()

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

        # ship certificates into the ssl folder in the game directory
        # openssl will use this folder since we hardcoded it in binding-mri.cpp
        tools.download("https://curl.haxx.se/ca/cacert.pem", "bin/lib/cacert.pem", overwrite=True)

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
        # copy the ruby standard library
	# this is a very ugly way of doing this (putting it in bin instead of lib)
	# but this makes distributing mods easier, and also makes sure windows and linux are mostly the same
        copy("*",
            dst="bin/lib/ruby/",
            src="lib/ruby/3.0.0/",
            root_package="ruby",
            keep_path=True)
