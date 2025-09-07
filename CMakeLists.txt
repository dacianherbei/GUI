cmake_minimum_required(VERSION 4.0)
project(GUI)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(SDL2 REQUIRED)

# imgui section
set(IMGUI_PATH external/imgui)
set(IMGUI_NODE_PATH external/imgui-node-editor)
set(IMGUI_IMPL_OPENGL2 ON)

# Sets up polyfill libraries for Windows examples (e.g. GLFW)
include (${IMGUI_PATH}/backends/imgui_impl_sdl2.cmake)
# include (${IMGUI_PATH}/backends/imgui_impl_glfw.cmake)
set(OpenGL_GL_PREFERENCE LEGACY)
include (${IMGUI_PATH}/backends/imgui_impl_opengl.cmake)

set(IMGUI_SOURCES
        ${IMGUI_PATH}/imgui.cpp
        ${IMGUI_PATH}/imgui_draw.cpp
        ${IMGUI_PATH}/imgui_tables.cpp
        ${IMGUI_PATH}/imgui_widgets.cpp
)

include_directories(${IMGUI_PATH})

add_library (imgui ${IMGUI_SOURCES})

add_library(imgui::imgui ALIAS imgui)

# Create GUI executable
set(GUI_SOURCES
        ${IMGUI_SOURCES}

        src/main.cpp

        src/Application.cpp

        # Core windows
        src/window/StartupWindow.cpp
        src/window/ConnectionPanel.cpp
        src/window/ExchangeControlPanel.cpp
        src/window/ExchangeEditor.cpp

        # Trading panels
        src/panel/LoggingPanel.cpp
        src/panel/OrderBookPanel.cpp
        src/panel/TradingPanel.cpp

        # Explorer dialog components
        src/explorer/ExplorerDialog.cpp
        src/explorer/ExplorerFavourites.cpp
        src/explorer/ExplorerFolderView.cpp
        src/explorer/ExplorerInput.cpp
        src/explorer/ExplorerToolbar.cpp
        src/window/ExchangeEditor.cpp

        # Editor
        src/editor/UDPConnectionNode.cpp
        src/editor/AccountSelectorNode.cpp
        src/editor/AccountConfigurationLoaderNode.cpp
        src/editor/WebSocketConnectionNode.cpp
        src/editor/FixConnectionNode.cpp
        src/editor/EndPointNode.cpp
        src/editor/EditorNodePanel.cpp
        src/editor/Node.cpp
        src/editor/ExchangeEditor.cpp
        src/editor/StateUpdaters.cpp
        src/editor/DataUpdaters.cpp
        src/editor/MessageProcessors.cpp
        src/editor/MessageFilters.cpp
        src/editor/MessageExtractors.cpp
        src/editor/RequestEncoders.cpp
        src/editor/NodeData.cpp
        src/editor/Pin.cpp
        src/editor/DataType.cpp
        src/editor/PinType.cpp
)

set(GUI_HEADERS
        ${IMGUI_HEADERS}

        src/Application.h

        src/window/StartupWindow.h
        src/window/ConnectionPanel.h
        src/window/ExchangeControlPanel.h
        src/window/ExchangeEditor.h

        src/panel/LoggingPanel.h
        src/panel/OrderBookPanel.h
        src/panel/TradingPanel.h

        src/explorer/ExplorerDialog.h
        src/explorer/ExplorerFavourites.h
        src/explorer/ExplorerFolderView.h
        src/explorer/ExplorerInput.h
        src/explorer/ExplorerToolbar.h
        src/window/ExchangeEditor.h

        src/editor/UDPConnectionNode.h
        src/editor/AccountSelectorNode.h
        src/editor/AccountConfigurationLoaderNode.h
        src/editor/WebSocketConnectionNode.h
        src/editor/FixConnectionNode.h
        src/editor/EndPointNode.h
        src/editor/EditorNodePanel.h
        src/editor/Node.h
        src/editor/ExchangeEditor.h
        src/editor/StateUpdaters.h
        src/editor/DataUpdaters.h
        src/editor/MessageProcessors.h
        src/editor/MessageFilters.h
        src/editor/MessageExtractors.h
        src/editor/RequestEncoders.h
        src/editor/NodeData.h
        src/editor/Pin.h
        src/editor/DataType.h
        src/editor/PinType.h
)

add_executable(GUI ${GUI_SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
        imgui-sdl2
        imgui-opengl2
)

# Include directories
target_include_directories(GUI PRIVATE
        src/
)

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