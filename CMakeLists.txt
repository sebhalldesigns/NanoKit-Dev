cmake_minimum_required(VERSION 3.10)

add_subdirectory(NanoKit)

message(STATUS "NKGEN_CMAKE: ${NKGEN_CMAKE}")

include(${NKGEN_CMAKE})

project(NanoKit-Dev)

add_executable(NanoKit-Dev
    src/main.c
)

target_link_libraries(NanoKit-Dev PUBLIC
    NanoKit
)

target_include_directories(NanoKit-Dev PUBLIC

)


#generate_modules(NanoKit-Dev
#    src/SecondWindow
#    src/Window   
#)
