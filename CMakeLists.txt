cmake_minimum_required(VERSION 3.20)
project(Proyecto_PAG)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

file( GLOB MY_FILES *.cpp )
file( GLOB GLAD_FILES GLAD/src/*.c )
file( GLOB IMGUI_FILES ImGui/src/*.cpp )

add_executable(${PROJECT_NAME} ${MY_FILES} ${GLAD_FILES} ${IMGUI_FILES}
        Listener.h
        ShaderProgram.cpp
        ShaderProgram.h
        ImGui/src/imgui_stdlib.cpp
        Camara.cpp
        Camara.h)

target_include_directories( ${PROJECT_NAME} PUBLIC GLAD/include ImGui/include )

target_link_libraries( ${PROJECT_NAME} PRIVATE
        OpenGL::GL glfw
        glm::glm
        assimp::assimp)
