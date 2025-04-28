#!/bin/bash
# Build script for StalmarckPy Python extension

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure with CMake
cmake ..

# Build the extension
make -j

# Copy the built extension to the Python module directory
# The .so file is already in the correct location thanks to set_target_properties in CMakeLists.txt

echo "Build complete!"
echo "You can now use the module with 'from stalmarckpy import solve'"