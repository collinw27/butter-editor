cmake_minimum_required(VERSION 3.28)
project(ButterProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Expects this directory structure:
# 
# build
# └─ lib
#    ├─ SFML
#    │  ├─ bin
#    │  ├─ include
#    │  └─ lib
#    ├─ GLEW
#    │  ├─ include
#    │  └─ lib
#    ├─ GLM
#    │  └─ glm
#    └─ FreeType
#       └─ include

# User must manually download SFML 3.0 (https://github.com/SFML/SFML/releases)
# Then, it is dynamically linked, meaning all DLLs must be copied to bin

set(SFML_DIR ${CMAKE_BINARY_DIR}/lib/SFML)
link_directories(${SFML_DIR}/lib)

# User must manually download GLEW (https://glew.sourceforge.net/ binaries)
# Then, it is dynamically linked, meaning glew32.dll must be copied to bin

set(GLEW_DIR ${CMAKE_BINARY_DIR}/lib/GLEW)
link_directories(${GLEW_DIR}/lib)

# User must manually download GLM (https://github.com/g-truc/glm)

set(GLM_DIR ${CMAKE_BINARY_DIR}/lib/GLM)
link_directories(${GLM_DIR}/lib)

# User must manually download FreeType 2.14.1 (https://download.savannah.gnu.org/releases/freetype/)
# Note that only the include path is used
# The library file is provided as part of SFML

set(FREETYPE_DIR ${CMAKE_BINARY_DIR}/lib/FreeType)

# Link files & generate executables

add_library(common INTERFACE)
target_include_directories(common INTERFACE
    src
    ${SFML_DIR}/include
    ${GLEW_DIR}/include
    ${GLM_DIR}
    ${FREETYPE_DIR}/include
)
target_link_libraries(common INTERFACE
    sfml-graphics-d sfml-window-d sfml-system-d
    freetyped
    ${GLEW_DIR}/lib/glew32.lib
    opengl32
    gdi32
)
target_compile_features(common INTERFACE cxx_std_17)
file(
    GLOB butter_SRC
    "src/editor/*.cpp"
    "src/editor/**/*.cpp"
    "src/graphics/*.cpp"
    "src/graphics/**/*.cpp"
    "src/utility/*.cpp"
    "src/utility/**/*.cpp"
    "src/command/*.cpp"
)

# MAIN EXECUTABLE

add_executable(Butter src/main.cpp ${butter_SRC})
target_link_libraries(Butter PRIVATE common)

add_subdirectory("src/testing")