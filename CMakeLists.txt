cmake_minimum_required(VERSION 3.17)

project(NanoKit-Dev)

include(${CMAKE_SOURCE_DIR}/NanoKit/tools/cmake/NanoKit.cmake)

add_subdirectory(NanoKit)

add_executable(NanoKit-Dev 
    src/main.c
)

target_link_libraries(NanoKit-Dev 
    NanoKit-Extern
    NanoKit
)

generate_modules(NanoKit-Dev
    src/Window
    src/SecondWindow
)

target_include_directories(NanoKit-Dev PUBLIC 
    NanoKit/lib
    ${CMAKE_BINARY_DIR}/generated
)


#generate_html()