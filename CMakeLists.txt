cmake_minimum_required(VERSION 2.8.12...3.5.0)
project(MyOpenGLEngine VERSION 0.1.0 LANGUAGES C CXX)

link_directories(src/libs/glfw)
link_directories(src/libs/spdlog)

include_directories(src/libs)
include_directories(src/libs/glad)
include_directories(src/libs/glfw)
include_directories(src/libs/glm)
include_directories(src/libs/glm/gtc)
include_directories(src/libs/glm/gtx)
include_directories(src/libs/KHR)
include_directories(src/libs/imgui)
include_directories(src/libs/stb_image)
include_directories(src/libs/spdlog)
include_directories(src)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_executable(MyOpenGLEngine ${SOURCES})

target_link_libraries(MyOpenGLEngine PRIVATE glfw3)
target_link_libraries(MyOpenGLEngine PRIVATE spdlog)
target_compile_definitions(MyOpenGLEngine PRIVATE SPDLOG_COMPILED_LIB)