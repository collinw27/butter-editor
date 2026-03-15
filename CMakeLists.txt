cmake_minimum_required(VERSION 3.28)
project(ButterProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.x
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

include_directories(
    src
)
link_directories(${FFMPEG_DIR}/lib)
file(
    GLOB butter_SRC
    "src/editor/*.cpp"
    "src/editor/**/*.cpp"
    "src/graphics/*.cpp"
    "src/utility/*.cpp"
)

add_executable(Butter src/main.cpp ${butter_SRC})
target_compile_features(Butter PRIVATE cxx_std_17)
target_link_libraries(Butter PRIVATE
    SFML::Graphics
)

add_executable(NodeTest src/testing/NodeTest.cpp ${butter_SRC})
target_compile_features(NodeTest PRIVATE cxx_std_17)
target_link_libraries(NodeTest PRIVATE SFML::Graphics)