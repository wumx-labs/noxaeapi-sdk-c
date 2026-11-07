cmake_minimum_required(VERSION 3.14)
project(noxaeapi-sdk-c VERSION 0.7.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(noxaeapi-c STATIC src/noxaeapi.c)
add_library(noxaeapi::c ALIAS noxaeapi-c)

target_include_directories(noxaeapi-c PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

find_package(CURL REQUIRED)
target_link_libraries(noxaeapi-c PUBLIC CURL::libcurl)

if (BUILD_EXAMPLES)
    add_executable(noxaeapi_example examples/main.c)
    target_link_libraries(noxaeapi_example PRIVATE noxaeapi-c)
endif()
