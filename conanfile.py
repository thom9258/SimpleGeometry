from conan import ConanFile
from conan.tools.files import copy

class conantestRecipe(ConanFile):
    name = "simple-geometry"
    version = "1.0"
    package_type = "library"
    license = "<Put the package license here>"
    author = "Thomas Alexgaard Jensen"
    url = "https://github.com/thom9258/SimpleGeometry/"
    description = "Generate simple geometry primitives for rendering."
    topics = ("3D", "Rendering", "OpenGL", "DirectX", "Vulkan", "Vertices")

    no_copy_source = True
    exports_sources = "simple_geometry.h"
    package_type = "header-library"

    def package(self):
        source_dir = self.source_folder + "/"
        include_dir = self.package_folder + "/include/SimpleGeometry"
        copy(self, "simple_geometry.h", source_dir, include_dir)
        print(">> copied simple_geometry.h" + " from " + source_dir + " to " + include_dir)

    def package_info(self):
        # For header-only packages, libdirs and bindirs are not used
        # so it's recommended to set those as empty.
        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []

