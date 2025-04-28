#!/bin/bash
# Build script for StalmarckPy Python extension

# Get the directory of this script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

# Clean previous builds first
rm -rf "$SCRIPT_DIR/build" "$SCRIPT_DIR/dist" "$SCRIPT_DIR/*.egg-info"

# Install build dependencies
pip install scikit-build-core pybind11 cmake

# Build the package
pip install -e .

echo "Build complete!"
echo "You can now use the module with 'from stalmarckpy import solve'"