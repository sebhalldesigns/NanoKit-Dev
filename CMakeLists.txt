cmake_minimum_required(VERSION 3.10)

project(NanoKit-Dev)

add_subdirectory(NanoKit/lib)

add_executable(NanoKit-Dev 
    src/main.c
)

target_link_libraries(NanoKit-Dev 
    NanoKit-Extern
    NanoKit-PAL
    NanoKit
)

target_include_directories(NanoKit-Dev PUBLIC 
    NanoKit/lib
)

if(EMSCRIPTEN)
    # Path to the input file
    set(INPUT_FILE "${CMAKE_SOURCE_DIR}/NanoKit/lib/resources/index.html")

    # Path to the output file
    set(OUTPUT_FILE "${CMAKE_SOURCE_DIR}/build/index.html")

    # Read the original file
    file(READ ${INPUT_FILE} FILE_CONTENTS)

    # Replace a placeholder string with the project or target name
    string(REPLACE "@TARGET_NAME@" "${CMAKE_PROJECT_NAME}" FILE_CONTENTS "${FILE_CONTENTS}")

    message("OUTPUT_NAME: ${CMAKE_PROJECT_NAME}")

    # Write the modified content to a new file
    file(WRITE ${OUTPUT_FILE} "${FILE_CONTENTS}")
endif()
