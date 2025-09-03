cmake_minimum_required(VERSION 3.12)
project(GUI)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

# Add subdirectories for dependencies
add_subdirectory(external/imgui)

# TODO: Add imgui-node-editor if needed
# add_subdirectory(external/imgui-node-editor)

# Create GUI executable
set(GUI_SOURCES
        src/main.cpp
        src/MainWindow.cpp
        src/MainWindow.h

        # ImGui backend files for SDL2 + OpenGL3
        external/imgui/backends/imgui_impl_sdl2.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
        src/MainWindow.cpp
        src/MainWindow.h
)

add_executable(GUI ${GUI_SOURCES})

# Link libraries
target_link_libraries(GUI PRIVATE
        imgui
        ${SDL2_LIBRARIES}
        OpenGL::GL
)

# Include directories
target_include_directories(GUI PRIVATE
        src/
        external/imgui/
        external/imgui/backends/
        ${SDL2_INCLUDE_DIRS}
)

# Add gl3w or GLAD for OpenGL loading
# Option 1: Using gl3w (recommended for simplicity)
set(GL3W_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/gl3w)
if(EXISTS ${GL3W_DIR})
    target_sources(GUI PRIVATE ${GL3W_DIR}/src/gl3w.c)
    target_include_directories(GUI PRIVATE ${GL3W_DIR}/include)
endif()

# Option 2: If you prefer GLAD, uncomment this instead:
# set(GLAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/glad)
# if(EXISTS ${GLAD_DIR})
#     target_sources(GUI PRIVATE ${GLAD_DIR}/src/glad.c)
#     target_include_directories(GUI PRIVATE ${GLAD_DIR}/include)
# endif()

# Platform-specific libraries
if(WIN32)
    # Windows-specific libraries
    target_link_libraries(GUI PRIVATE
            ${CMAKE_DL_LIBS}
    )
    set_target_properties(GUI PROPERTIES
            WIN32_EXECUTABLE TRUE
    )
elseif(UNIX AND NOT APPLE)
    # Linux-specific libraries
    target_link_libraries(GUI PRIVATE
            ${CMAKE_DL_LIBS}
            pthread
    )
elseif(APPLE)
    # macOS-specific settings
    set_target_properties(GUI PROPERTIES
            MACOSX_BUNDLE TRUE
    )
    target_link_libraries(GUI PRIVATE
            ${CMAKE_DL_LIBS}
    )
endif()

# Set output directory
set_target_properties(GUI PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Copy any required data files to output directory if needed
# file(COPY data/ DESTINATION ${CMAKE_BINARY_DIR}/bin/data/)

# Compiler-specific options
if(MSVC)
    target_compile_options(GUI PRIVATE /W4)
else()
    target_compile_options(GUI PRIVATE -Wall -Wextra -Wpedantic)
endif()