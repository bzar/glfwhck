CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
project(glfwhck)

include("cmake/subproject.cmake")

add_subdirectory(lib)

include_directories(
  include
  ${GLFW_SOURCE_DIR}/include
)

file(GLOB SOURCES src/*.c)

add_library(glfwhck
    ${SOURCES}
)

