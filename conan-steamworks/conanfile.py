from conans import ConanFile, tools
import os.path

class SteamworksConan(ConanFile):
    name = "steamworks"
    version = "1.51"
    url = "None"
    license = "Proprietary"
    description = "Valve's Steamworks SDK"
    settings = {
        "os": ["Windows", "Linux"],
        "arch": ["x86_64"],
    }
    short_paths = True
    zipfile = "steamworks_sdk_{}.zip".format(version.replace(".", ""))
    exports_sources = zipfile
    root = "sdk"

    def build(self):
        tools.unzip(os.path.join(self.source_folder, self.zipfile))

    def package(self):
        # Pick the correct redistributable binary directory
        redistributable_bin = os.path.join(self.root, "redistributable_bin")
        if self.settings.os == "Windows":
            self.copy("steam_api64.dll", dst="bin", src=os.path.join(redistributable_bin, "win64"))
            self.copy("steam_api64.lib", dst="lib", src=os.path.join(redistributable_bin, "win64"))
        else:
            self.copy("libsteam_api.so", dst="lib", src=os.path.join(redistributable_bin, "linux64"))

        # Copy headers
        self.copy("*.h", dst="include", src=os.path.join(self.root, "public"))

    def package_info(self):
        if self.settings.os == "Windows":
            self.cpp_info.libs = ["steam_api64"]
        else:
            self.cpp_info.libs = ["steam_api"]
        self.cpp_info.includedirs = ["include"]
