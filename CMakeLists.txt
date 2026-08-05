cmake_minimum_required(VERSION 3.28)
project(ButterProject LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

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
    "src/command/CommandParser.cpp"
    "src/command/CommandResult.cpp"
    "src/command/CommandStructure.cpp"
    "src/command/exceptions.cpp"
    "src/editor/Editor.cpp"
    "src/editor/Editor_Commands.cpp"
    "src/editor/command_bar/CommandBar.cpp"
    "src/editor/core/DragDirectScroll.cpp"
    "src/editor/core/DragDivider.cpp"
    "src/editor/core/DragMouse.cpp"
    "src/editor/core/DragScroll.cpp"
    "src/editor/core/EditorModule.cpp"
    "src/editor/core/FlexTab.cpp"
    "src/editor/debug/DebugModule.cpp"
    "src/editor/log/LogModule.cpp"
    "src/editor/media/DragMedia.cpp"
    "src/editor/media/MediaModule.cpp"
    "src/editor/project/ProjectModule.cpp"
    "src/editor/timeline/TimelineModule.cpp"
    "src/editor/timeline/Clip.cpp"
    "src/editor/timeline/mouse/ExtendClip.cpp"
    "src/graphics/GLContainer.cpp"
    "src/graphics/GLFont.cpp"
    "src/graphics/GLNode.cpp"
    "src/graphics/GLOutlinedRectangle.cpp"
    "src/graphics/GLRectangle.cpp"
    "src/graphics/GLRootNode.cpp"
    "src/graphics/GLText.cpp"
    "src/graphics/text_formatting/TextFormat.cpp"
    "src/main.cpp"
    "src/project/Project.cpp"
    "src/project/clip/ClipData.cpp"
    "src/project/clip/ColorClipData.cpp"
    "src/project/exceptions.cpp"
    "src/utility/Debugger.cpp"
    "src/utility/FileManager.cpp"
    "src/utility/Graphics.cpp"
    "src/utility/Input.cpp"
    "src/utility/Logger.cpp"
    "src/utility/core.cpp"
)
file(
    GLOB butter_LINK
    "${SUBPROC_DIR}/subprocess/*.cpp"
)

# MAIN EXECUTABLE

add_executable(Butter src/main.cpp ${butter_SRC} ${butter_LINK})
target_link_libraries(Butter PRIVATE common)

add_subdirectory("src/testing")