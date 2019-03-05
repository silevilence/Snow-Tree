cmake_minimum_required(VERSION 3.13)
project(Tree)

set(CMAKE_CXX_STANDARD 11)

INCLUDE_DIRECTORIES("E:\\include\\glad\\include")
INCLUDE_DIRECTORIES("E:\\include\\glfw-3.2.1.bin.WIN32\\include")
INCLUDE_DIRECTORIES("E:\\include\\glm")
INCLUDE_DIRECTORIES("E:\\include\\copy")

link_directories("E:\\include\\glfw-3.2.1.bin.WIN32\\lib-mingw")

set(SOURCE_FILES main.cpp glad.c ResourceManager.cpp Shader.cpp stb_image.cpp Texture2D.cpp)

add_executable(Tree ${SOURCE_FILES})

target_link_libraries(Tree glfw3 opengl32)