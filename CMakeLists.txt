cmake_minimum_required(VERSION 3.30)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#--------
# project
# -------
project(RayTracing CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32 OR MINGW)
    add_compile_definitions(NOMINMAX _CRT_SECURE_NO_WARNINGS WIN32_LEAN_AND_MEAN)
endif ()

if(MSVC)
    add_compile_options(/MP /utf-8 /W4 /wd4201 /WX)
else()
    add_compile_options(-W -Wall -Werror -Wno-unused-parameter)
endif()

add_subdirectory(RayTracing)
