cmake_minimum_required(VERSION 3.31)
set(CMAKE_CXX_STANDARD 20)
project(Proc_animation)


set(CMAKE_CXX_STANDARD 20)

# Set these to wherever raylib is installed
include_directories("D:/msys64/mingw64/include")
link_directories("D:/msys64/mingw64/lib")

add_executable(Proc_animation main.cpp)

# Link raylib and system libraries
target_link_libraries(Proc_animation raylib winmm gdi32 opengl32)

