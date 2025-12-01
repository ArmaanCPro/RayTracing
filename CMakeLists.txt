cmake_minimum_required(VERSION 3.30)

#--------
# project
# -------
project(RayTracing CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# download CPM.cmake
file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.8/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
  EXPECTED_HASH SHA256=78ba32abdf798bc616bab7c73aac32a17bbd7b06ad9e26a6add69de8f3ae4791
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

CPMAddPackage("gh:TheLartians/Ccache.cmake@1.2.5")

if (WIN32 OR MINGW)
    add_compile_definitions(NOMINMAX _CRT_SECURE_NO_WARNINGS WIN32_LEAN_AND_MEAN)
endif ()

if(MSVC)
    add_compile_options(/MP /utf-8 /W4 /wd4201 /WX)
else()
    add_compile_options(-W -Wall -Werror -Wno-unused-parameter)
endif()

if (UNIX)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

add_subdirectory(RayTracing)
