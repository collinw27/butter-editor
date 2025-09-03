cmake_minimum_required(VERSION 3.28)
project(Butter LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.x
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

set(FFMPEG_DIR "C:/Users/colli/Documents/Other/C++/ffmpeg")
include_directories(
    src
    ${FFMPEG_DIR}/include
)
link_directories(${FFMPEG_DIR}/lib)
file(
    GLOB butter_SRC
    "src/main.cpp"
    "src/editor/*.cpp"
    "src/editor/**/*.cpp"
    "src/graphics/*.cpp"
    "src/utility/*.cpp"
    )

add_executable(Butter ${butter_SRC})

target_compile_features(Butter PRIVATE cxx_std_17)
target_link_libraries(Butter PRIVATE
    SFML::Graphics
    avcodec
    avformat
    avutil
    swscale
)