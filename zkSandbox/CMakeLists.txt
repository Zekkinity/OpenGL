cmake_minimum_required(VERSION 3.24)
project(zkSandbox)

# C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SRC_FILES "src/*.cpp")
add_executable(zkSandbox ${SRC_FILES})

# src
target_include_directories(zkSandbox PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# Link zkEngine
target_link_libraries(zkSandbox PRIVATE zkEngine)

add_custom_target(link_sandbox_assets ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:zkSandbox>
    COMMAND ${CMAKE_COMMAND} -E remove $<TARGET_FILE_DIR:zkSandbox>/res
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_CURRENT_SOURCE_DIR}/res
        $<TARGET_FILE_DIR:zkSandbox>/res
    COMMENT "Linking sandbox assets"
)

add_dependencies(zkSandbox link_sandbox_assets)

# Debug
target_compile_definitions(zkSandbox PRIVATE $<$<CONFIG:Debug>:DEBUG>)
