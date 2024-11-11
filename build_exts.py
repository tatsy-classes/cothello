import platform

from setuptools.errors import CCompilerError, PackageDiscoveryError
from pybind11.setup_helpers import Pybind11Extension
from setuptools.command.build_ext import build_ext

ext_modules = [
    Pybind11Extension(
        "othello.libcpp",
        sources=[
            "src/pybind11.cpp",
            "src/bitboard.cpp",
        ],
        define_macros=[("NPY_NO_DEPRECATED_API", "NPY_1_7_API_VERSION")],
        language="c++",
    ),
]


class MyBuildExt(build_ext):
    def run(self):
        try:
            build_ext.run(self)
        except FileNotFoundError:
            raise Exception("File not found. Could not compile C extension.")

    def build_extension(self, ext):
        if self.compiler.compiler_type == "unix":
            for e in self.extensions:
                e.extra_compile_args.extend(["-std=c++17"])
                e.extra_link_args.extend([])

        elif self.compiler.compiler_type == "msvc":
            for e in self.extensions:
                e.extra_compile_args.extend(["/std:c17", "/utf-8", "/openmp"])

        if platform.system() == "Linux":
            for e in self.extensions:
                e.extra_compile_args.extend(["-fopenmp"])
                e.extra_link_args.extend(["-fopenmp"])

        if platform.system() == "Darwin":
            for e in self.extensions:
                e.extra_compile_args.extend(["-mmacosx-version-min=10.15"])

        try:
            build_ext.build_extension(self, ext)
        except (CCompilerError, PackageDiscoveryError, ValueError):
            raise Exception("Could not compile C extension.")


def build(setup_kwargs):
    setup_kwargs.update(
        {
            "ext_modules": ext_modules,
            "cmdclass": {"build_ext": MyBuildExt},
        }
    )
