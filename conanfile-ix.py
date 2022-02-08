from conans import ConanFile, CMake


class TeckosConan(ConanFile):
    settings = ["os", "compiler", "build_type", "arch"]
    requires = ["nlohmann_json/3.10.4", "sigslot/1.2.1", "libdeflate/1.8", "zlib/1.2.11", "spdlog/1.9.2"]
    generators = "cmake", "cmake_find_package", "json"

    def configure(self):
        if self.settings.os == "Windows":
            self.requires.add("mbedtls/3.0.0")
        if self.settings.os == "Linux":
            self.requires.add("openssl/1.1.1l")
