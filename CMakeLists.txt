cmake_minimum_required(VERSION 3.28)
project(ButterProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(PROJECT_DIR ${CMAKE_BINARY_DIR}/../)

# Currently, all necessary libraries are included in the lib/ folder
# !!! All DLLs from lib/bin must be copied to the executable's directory

set(SFML_DIR ${PROJECT_DIR}/lib/SFML)
link_directories(${SFML_DIR}/lib)
set(GLEW_DIR ${PROJECT_DIR}/lib/GLEW)
link_directories(${GLEW_DIR}/lib)
set(GLM_DIR ${PROJECT_DIR}/lib/GLM)
link_directories(${GLM_DIR}/lib)
set(SUBPROC_DIR ${PROJECT_DIR}/lib/Subprocess)

# Freetype: Note that only the include path is used
# The library file is provided as part of SFML

set(FREETYPE_DIR ${PROJECT_DIR}/lib/FreeType)

# Link files & generate executables

add_library(common INTERFACE)
target_include_directories(common INTERFACE
    src
    ${SFML_DIR}/include
    ${GLEW_DIR}/include
    ${GLM_DIR}
    ${FREETYPE_DIR}/include
    ${SUBPROC_DIR}
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
file(
    GLOB butter_LINK
    "${SUBPROC_DIR}/subprocess/*.cpp"
)

# MAIN EXECUTABLE

add_executable(Butter src/main.cpp ${butter_SRC} ${butter_LINK})
target_link_libraries(Butter PRIVATE common)

add_subdirectory("src/testing")