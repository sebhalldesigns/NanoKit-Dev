cmake_minimum_required(VERSION 3.17)

project(NanoKit-Dev)

include(${CMAKE_SOURCE_DIR}/NanoKit/tools/cmake/NanoKit.cmake)

add_subdirectory(NanoKit)

if (DEFINED BUILD_IOS)

    set(CMAKE_SYSTEM_NAME iOS)
    set(CMAKE_OSX_SYSROOT iphonesimulator)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0")

    set(CMAKE_OSX_ARCHITECTURES "arm64")
    set(CMAKE_IOS_INSTALL_COMBINED YES)

#    set(CMAKE_C_FLAGS "-x objective-c")

    add_executable(NanoKit-Dev MACOSX_BUNDLE
        src/main.c
    )
    
    set_target_properties(NanoKit-Dev  PROPERTIES 
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.company.app" 
        XCODE_ATTRIBUTE_EXECUTABLE_FILE "NanoKit-Dev" 
        MACOSX_BUNDLE YES 
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/NanoKit/lib/resources/iOS_Info.plist
        XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED NO
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


elseif(APPLE)
    add_executable(NanoKit-Dev MACOSX_BUNDLE
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

    set_target_properties(NanoKit-Dev PROPERTIES 
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.company.app" 
        XCODE_ATTRIBUTE_EXECUTABLE_FILE "NanoKit-Dev" 
        MACOSX_BUNDLE YES 
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/NanoKit/lib/resources/macOS_Info.plist
        XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED NO
    )


else()

    add_executable(NanoKit-Dev WIN32
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
endif()




#generate_html()