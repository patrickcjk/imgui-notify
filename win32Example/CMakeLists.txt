#[===[
        WARNING

        The compiled binary HAS to be in the example directory, NOT in the build directory.

        Check IconsFontAwesome.h for fix.
]===]

cmake_minimum_required(VERSION 3.10)

project(ImGuiNotify VERSION 0.0.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCES main.cpp)

set(IMGUI_SOURCES
	${IMGUI_DIR}/imgui/imgui.cpp
	${IMGUI_DIR}/imgui/imgui_demo.cpp
	${IMGUI_DIR}/imgui/imgui_draw.cpp
	${IMGUI_DIR}/imgui/imgui_tables.cpp
	${IMGUI_DIR}/imgui/imgui_widgets.cpp
	${IMGUI_DIR}/imgui/implot.cpp
	${IMGUI_DIR}/imgui/implot_items.cpp
	${IMGUI_DIR}/imgui/implot_demo.cpp
)

set(BACKEND_SOURCES
	${IMGUI_DIR}/backends/imgui_impl_win32.cpp
	${IMGUI_DIR}/backends/imgui_impl_dx12.cpp
)

add_executable(test ${SOURCES} ${IMGUI_SOURCES} ${BACKEND_SOURCES})

target_include_directories(test PRIVATE
	${IMGUI_DIR}/imgui
	${IMGUI_DIR}/backends
)

target_link_libraries(test PRIVATE "d3d12.lib" "dxgi.lib" "d3dcompiler.lib")