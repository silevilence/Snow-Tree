cmake_minimum_required(VERSION 3.13)
project(Tree)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXE_LINKER_FLAGS -static)

INCLUDE_DIRECTORIES("E:\\include\\glad\\330\\include")
INCLUDE_DIRECTORIES("E:\\include\\glfw-3.2.1.bin.WIN32\\include")
INCLUDE_DIRECTORIES("E:\\include\\glm")
INCLUDE_DIRECTORIES("E:\\include\\copy")

link_directories("E:\\include\\glfw-3.2.1.bin.WIN32\\lib-mingw")

set(RESOURCE_FILES shaders textures)
foreach (_file ${RESOURCE_FILES})
    file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}\\${_file})
    #    message(${CMAKE_BINARY_DIR}\\${_file})
endforeach ()
file(INSTALL ${RESOURCE_FILES} DESTINATION ${CMAKE_BINARY_DIR})

set(SOURCE_FILES main.cpp glad.c ResourceManager.cpp Shader.cpp stb_image.cpp Texture2D.cpp Point.cpp MyTree.cpp Mesh.cpp SimpleTree.cpp SimpleTreeBranch.cpp LSystem.cpp BoundBox.cpp SnowParticle.cpp Snow.cpp SPlane.cpp SPlane.h)

add_executable(Tree ${SOURCE_FILES})

target_link_libraries(Tree glfw3 opengl32)