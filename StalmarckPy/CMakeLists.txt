cmake_minimum_required(VERSION 3.10)
project(stalmarckpy)

# Find Python
find_package(Python COMPONENTS Interpreter Development REQUIRED)

# Add pybind11
include(FetchContent)
FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG        v2.10.4  # Adjust version as needed
)
FetchContent_MakeAvailable(pybind11)

# Set position independent code flag for the entire project
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Include the main project to get access to its headers and libraries
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/stalmarck EXCLUDE_FROM_ALL)

# Create the Python module
pybind11_add_module(_stalmarckpy stalmarckpy_bindings.cpp)

# Link against the main project library
target_link_libraries(_stalmarckpy PRIVATE stalmarck)

# Install the Python module to the stalmarckpy directory
set_target_properties(_stalmarckpy PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/stalmarckpy
)

# Copy Python files
file(GLOB PYTHON_FILES ${CMAKE_CURRENT_SOURCE_DIR}/stalmarckpy/*.py)
file(COPY ${PYTHON_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/stalmarckpy)