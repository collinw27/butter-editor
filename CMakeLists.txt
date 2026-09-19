cmake_minimum_required(VERSION 3.28)
project(ButterProject LANGUAGES CXX)

# Default to Debug for single-config generators (Makefiles, Ninja).
# Multi-config generators (Visual Studio) ignore CMAKE_BUILD_TYPE.
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
# The library file is provided as part of SFML and linked by name.
# ------------------------------------------------------------------

set(SFML_DIR    "${PROJECT_DIR}/lib/SFML")
set(GLEW_DIR    "${PROJECT_DIR}/lib/GLEW")
set(GLM_DIR     "${PROJECT_DIR}/lib/GLM")
set(SUBPROC_DIR "${PROJECT_DIR}/lib/Subprocess")
set(FREETYPE_DIR "${PROJECT_DIR}/lib/FreeType")

set(SFML_SUFFIX     "$<$<CONFIG:Debug>:-d>")
set(FREETYPE_SUFFIX "$<$<CONFIG:Debug>:d>")

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
    "${SFML_DIR}/include"
    "${GLEW_DIR}/include"
    "${GLM_DIR}"
    "${FREETYPE_DIR}/include"
)

target_link_directories(common INTERFACE
    "${SFML_DIR}/lib"
    "${GLEW_DIR}/lib"
)

target_link_libraries(common INTERFACE
    "sfml-graphics${SFML_SUFFIX}"
    "sfml-window${SFML_SUFFIX}"
    "sfml-system${SFML_SUFFIX}"
    "freetype${FREETYPE_SUFFIX}"
    "${GLEW_DIR}/lib/glew32.lib"
    subprocess
    opengl32
    gdi32
)

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
# ------------------------------------------------------------------

add_executable(Butter src/main.cpp ${butter_SRC})
target_link_libraries(Butter PRIVATE common)

# ------------------------------------------------------------------
# Copy DLLs into executable directory (expected in `lib/bin/`)
# ------------------------------------------------------------------

file(GLOB BUTTER_DLLS CONFIGURE_DEPENDS "${PROJECT_DIR}/lib/bin/*.dll")
if(BUTTER_DLLS)
    add_custom_command(TARGET Butter POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${BUTTER_DLLS}
            "$<TARGET_FILE_DIR:Butter>"
        VERBATIM
    )
endif()


# ------------------------------------------------------------------
# Testing directory is also responsible for building executables
# ------------------------------------------------------------------

add_subdirectory(src/testing)