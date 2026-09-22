cmake_minimum_required(VERSION 3.28)
project(ButterProject LANGUAGES CXX)

# ------------------------------------------------------------------
# Default to Debug for single-config generators (Makefiles, Ninja)
# Multi-config generators (Visual Studio) ignore CMAKE_BUILD_TYPE
# ------------------------------------------------------------------

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()
if(DEFINED CACHE{CMAKE_BUILD_TYPE})
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(PROJECT_DIR "${PROJECT_SOURCE_DIR}")

# ------------------------------------------------------------------
# Dependencies
# All necessary libraries are currently included in the lib/ folder.
# Freetype: Only the include path is used.
# The library is provided as part of SFML.
# ------------------------------------------------------------------

set(GLM_DIR "${PROJECT_DIR}/lib/GLM")
set(FREETYPE_DIR "${PROJECT_DIR}/lib/FreeType")
set(SUBPROC_DIR "${PROJECT_DIR}/lib/Subprocess")

# ------------------------------------------------------------------
# Statically link Subprocess library
# ------------------------------------------------------------------

file(GLOB subprocess_SRC CONFIGURE_DEPENDS "${SUBPROC_DIR}/subprocess/*.cpp")
add_library(subprocess STATIC ${subprocess_SRC})
target_include_directories(subprocess PUBLIC "${SUBPROC_DIR}")
target_compile_features(subprocess PUBLIC cxx_std_17)

# ------------------------------------------------------------------
# Common interface target (include paths, libraries, language level)
# ------------------------------------------------------------------

add_library(common INTERFACE)
target_compile_features(common INTERFACE cxx_std_17)

target_include_directories(common INTERFACE
    "${PROJECT_DIR}/src"
    "${GLM_DIR}"
    "${FREETYPE_DIR}/include"
)

# ------------------------------------------------------------------
# FetchContent is used to easily build libraries with CMake
# ------------------------------------------------------------------

include(FetchContent)

set(SFML_BUILD_NETWORK OFF)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(glew
    GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
    GIT_TAG glew-cmake-2.2.0
    GIT_SHALLOW ON
    SYSTEM)
FetchContent_MakeAvailable(glew)

target_link_libraries(common INTERFACE
    SFML::Graphics
    SFML::Window
    SFML::System
    libglew_static
    subprocess
)

# ------------------------------------------------------------------
# Link with libraries installed by platform's graphics driver
# ------------------------------------------------------------------

if(WIN32)

    target_link_libraries(common INTERFACE
        opengl32
        gdi32
    )

elseif(UNIX AND NOT APPLE)

    target_link_libraries(common INTERFACE
        GL
    )

else()

    message(FATAL_ERROR "Invalid system, must be WIN32 or UNIX compatible")

endif()

# ------------------------------------------------------------------
# Sources
# I know glob is generally advised against for CMakeLists, but for now
# it's much easier while files are still being created/moved around.
# I might change this once the file layout settles down.
# ------------------------------------------------------------------

file(GLOB butter_SRC CONFIGURE_DEPENDS
    "src/command/*.cpp"
    "src/editor/*.cpp"
    "src/editor/**/*.cpp"
    "src/editor/**/**/*.cpp"
    "src/graphics/*.cpp"
    "src/graphics/**/*.cpp"
    "src/project/*.cpp"
    "src/project/**/*.cpp"
    "src/utility/*.cpp"
    "src/utility/**/*.cpp"
)

# ------------------------------------------------------------------
# Main executable
# DLLs should no longer be needed in the executable directory
# If you do need them, they can be found in `lib/bin/`
# ------------------------------------------------------------------

add_executable(Butter src/main.cpp ${butter_SRC})
set_target_properties(Butter PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(Butter PRIVATE common)

# ------------------------------------------------------------------
# Testing directory is also responsible for building executables
# ------------------------------------------------------------------

option(BUTTER_BUILD_TESTING "Build the testing executables" OFF)
if(BUTTER_BUILD_TESTING)
    add_subdirectory(src/testing)
endif()