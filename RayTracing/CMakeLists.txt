cmake_minimum_required(VERSION 3.30)

project(RT CXX)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/src/*.h"
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

find_package(imgui CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# xoshiro random header-only
include(FetchContent)
FetchContent_Declare(xoshiro URL https://github.com/nessan/xoshiro/releases/download/v1.1.0/xoshiro.zip)
FetchContent_MakeAvailable(xoshiro)

target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui SDL3::SDL3 glm::glm xoshiro::xoshiro)

execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink
                "${CMAKE_BINARY_DIR}/compile_commands.json"
                "${CMAKE_SOURCE_DIR}/compile_commands.json"
)

add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_SOURCE_DIR}/imgui.ini"
        "${CMAKE_CURRENT_BINARY_DIR}/imgui.ini"
        COMMENT "Copying imgui.ini into working directory"
)
