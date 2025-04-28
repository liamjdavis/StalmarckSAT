cmake_minimum_required(VERSION 3.15...3.26)

project(stalmarckpy LANGUAGES CXX)

# Find Python and pybind11
find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
find_package(pybind11 CONFIG REQUIRED)

# Set position independent code flag for the entire project
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Add the StalmarckSAT library as a subdirectory
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/stalmarck EXCLUDE_FROM_ALL)

# Create the Python module
pybind11_add_module(_stalmarckpy stalmarckpy_bindings.cpp)

# Link against the main project library
target_link_libraries(_stalmarckpy PRIVATE stalmarck)

# Set the output directory for the Python module
set_target_properties(_stalmarckpy PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/stalmarckpy
)

# Install the Python module to the correct location
install(TARGETS _stalmarckpy 
    DESTINATION stalmarckpy
    COMPONENT python_modules
)

# Install Python files
install(FILES stalmarckpy/__init__.py 
    DESTINATION stalmarckpy
    COMPONENT python_modules
)