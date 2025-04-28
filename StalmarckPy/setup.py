import os
import re
import subprocess
import sys
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext

# A CMakeExtension to use CMake to build the extension
class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)

# Custom build extension that uses CMake
class CMakeBuild(build_ext):
    def run(self):
        # Check if CMake is installed
        try:
            subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError(
                "CMake must be installed to build the following extensions: " +
                ", ".join(e.name for e in self.extensions))

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        # Ensure the output directory exists
        os.makedirs(self.build_temp, exist_ok=True)
        
        # Define the directory for the extension
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        
        # Use the build.sh script which we know works
        build_script = os.path.join(ext.sourcedir, 'build.sh')
        
        # Make the script executable
        os.chmod(build_script, 0o755)
        
        # Run the build script
        subprocess.check_call([build_script], cwd=ext.sourcedir)
        
        # Copy built extension files to the package directory
        # The extension is already built and placed in stalmarckpy/
        # We just need to make sure the directory exists
        os.makedirs(os.path.join(extdir, 'stalmarckpy'), exist_ok=True)

setup(
    name='stalmarckpy',
    version='0.1.0',
    author='StalmarckSAT Team',
    author_email='your.email@example.com',
    description='Python bindings for StalmarckSAT solver',
    long_description='A lightweight Python API for the StalmarckSAT solver',
    packages=['stalmarckpy'],
    ext_modules=[CMakeExtension('stalmarckpy._stalmarckpy')],
    cmdclass={'build_ext': CMakeBuild},
    zip_safe=False,
    python_requires='>=3.6',
    install_requires=['pybind11>=2.6.0'],
)