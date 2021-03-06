# PROJECT: bgfx_driver
cmake_minimum_required(VERSION 3.3)

file(GLOB SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.c*")
source_group(source FILES ${SOURCE_FILES})

if (APPLE)
    find_library(FOUNDATION_LIBRARY Foundation)
    if (NOT FOUNDATION_LIBRARY)
        message(FATAL_ERROR "Foundation Library not found")
    endif()

    find_library(METAL_LIBRARY Metal)
    if (NOT METAL_LIBRARY)
        message(FATAL_ERROR "Metal Library not found")
    endif()

    find_library(QUARTZ_LIBRARY QuartzCore)
    if (NOT QUARTZ_LIBRARY)
        message(FATAL_ERROR "QuartzCore Library not found")
    endif()

    if (NOT IOS)
        find_library(COCOA_LIBRARY Cocoa)
        if (NOT COCOA_LIBRARY)
            message(FATAL_ERROR "Cocoa Library not found")
        endif()
    endif()

    set(EXTRA_LIBS ${FOUNDATION_LIBRARY} ${METAL_LIBRARY} ${COCOA_LIBRARY} ${QUARTZ_LIBRARY} ${UIKIT_LIBRARY} ${COREAUDIO_LIBRARY})
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # ignore some compiler specific warnings
    add_compile_options(-Wno-enum-compare)
endif()

add_library(bgfx_driver ${BUILD_LIBRARY_TYPE} ${SOURCE_FILES})

target_include_directories(bgfx_driver PRIVATE ${BGFX_INCLUDE_DIR})
target_link_libraries(bgfx_driver PRIVATE bx bgfx bimg ${EXTRA_LIBS})
set_target_properties(bgfx_driver PROPERTIES FOLDER Plugins ${IOS_GENERAL_PROPERTIES})


