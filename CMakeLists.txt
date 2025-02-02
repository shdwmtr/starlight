cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(Installer VERSION 1.0 LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "-w -fpermissive -fno-diagnostics-show-option")

# Define source files
set(IMGUI_DIR "../..")
set(SOURCES
    src/main.cc
    src/texture.cc
    src/wndproc.cc
    src/renderer.cc
    src/routes/home.cc
    src/routes/install_prompt.cc
    src/routes/installer.cc
    src/routes/uninstall_select.cc
    src/components/titlebar.cc
    src/components/checkbox.cc
    src/animate.cc
    src/router.cc
    src/dpi.cc
    vendor/imgui/src/imgui.cpp
    vendor/imgui/src/imgui_demo.cpp
    vendor/imgui/src/imgui_draw.cpp
    vendor/imgui/src/imgui_tables.cpp
    vendor/imgui/src/imgui_widgets.cpp
    vendor/imgui/src/imgui_impl_glfw.cpp
    vendor/imgui/src/imgui_impl_opengl3.cpp
    vendor/imgui/include/misc/freetype/imgui_freetype.cpp
    vendor/imgui/src/imgui_stdlib.cpp
)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")

# Include directories
include_directories(include)
include_directories(vendor/imgui/include)

# Platform-specific settings
if(UNIX AND NOT APPLE)
    message(STATUS "Building on Linux")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLFW REQUIRED glfw3)

    target_compile_options(${PROJECT_NAME} PRIVATE ${GLFW_CFLAGS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${GLFW_LDFLAGS} GL)

elseif(APPLE)
    message(STATUS "Building on macOS")
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    find_library(IOKIT_LIBRARY IOKit REQUIRED)
    find_library(COREVIDEO_LIBRARY CoreVideo REQUIRED)
    find_library(OPENGL_LIBRARY OpenGL REQUIRED)
    find_library(GLFW_LIBRARY glfw REQUIRED)

    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${COCOA_LIBRARY}
        ${IOKIT_LIBRARY}
        ${COREVIDEO_LIBRARY}
        ${OPENGL_LIBRARY}
        ${GLFW_LIBRARY}
    )

elseif(WIN32)
    message(STATUS "Building on Windows")
    find_package(Freetype REQUIRED)
    find_package(Stb REQUIRED)
    find_package(glfw3 CONFIG REQUIRED)
    find_package(glew CONFIG REQUIRED)

    target_include_directories(${PROJECT_NAME} PRIVATE ${Stb_INCLUDE_DIR})

    target_link_libraries(${PROJECT_NAME} PRIVATE
        Freetype::Freetype
        glfw
        GLEW::GLEW
        gdi32
        opengl32
        imm32
    )
endif()

# Add compilation flags
target_compile_options(${PROJECT_NAME} PRIVATE
    -g
    -Wall
    -Wformat
)
