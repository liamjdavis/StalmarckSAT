cmake_minimum_required(VERSION 3.10)
project(StalmarckSAT VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

# Find GTest package
find_package(GTest REQUIRED)

# Add source files
set(SOURCES
    src/core/stalmarck.cpp
    src/core/formula.cpp
    src/solver/solver.cpp
    src/parser/parser.cpp
)

# Add header files
set(HEADERS
    src/core/stalmarck.hpp
    src/core/formula.hpp
    src/solver/solver.hpp
    src/parser/parser.hpp
)

# Create main library
add_library(stalmarck STATIC ${SOURCES} ${HEADERS})
target_include_directories(stalmarck 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
)

# Add tests
enable_testing()
add_subdirectory(test)

# Installation
install(TARGETS stalmarck
    EXPORT stalmarck-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY src/
    DESTINATION include/stalmarck
    FILES_MATCHING PATTERN "*.hpp"
)

# Export targets
install(EXPORT stalmarck-targets
    FILE stalmarck-config.cmake
    NAMESPACE stalmarck::
    DESTINATION lib/cmake/stalmarck
) 