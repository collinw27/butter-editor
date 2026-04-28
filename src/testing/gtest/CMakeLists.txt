
# GoogleTest linked with FetchContent as per Google's specification

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)
set_target_properties(
    gtest gmock gtest_main gmock_main PROPERTIES
    FOLDER "third_party"
    EXCLUDE_FROM_ALL TRUE
)

add_executable(CommandTest CommandTest.cpp ${butter_SRC})
target_link_libraries(CommandTest PRIVATE common GTest::gtest_main)