cmake_minimum_required(VERSION 3.24)
project(zkEngine)

# C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# GLAD
add_library(glad STATIC vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include
)

# stb_image
add_library(stb_image STATIC vendor/stb_image/stb_image.cpp)
target_include_directories(stb_image PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb_image
)

# glm
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm
)

# imgui
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/)

# assimp
find_package(assimp REQUIRED)

# OpenGL
find_package(OpenGL REQUIRED)

file(GLOB_RECURSE SRC_FILES "src/*.cpp")
add_executable(zkEngine ${SRC_FILES})
# add_library(zkEngine STATIC ${SRC_FILES})

# src
target_include_directories(zkEngine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(zkEngine PRIVATE
    glfw
    glad
    stb_image
    OpenGL::GL
    glm
    imgui
    assimp
)

# Debug
target_compile_definitions(zkEngine PRIVATE $<$<CONFIG:Debug>:DEBUG>)
