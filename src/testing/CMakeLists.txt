
# GLTest
# Used for testing custom OpenGL objects

add_executable(GLTest GLTest.cpp ${butter_SRC} ${butter_LINK})
target_link_libraries(GLTest PRIVATE common)

add_subdirectory("gtest")