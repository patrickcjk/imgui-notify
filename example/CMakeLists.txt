#[===[
        WARNING

        The compiled binary HAS to be in the example directory, NOT in the build directory.

        Check IconsFontAwesome.h for fix.
]===]

cmake_minimum_required(VERSION 3.10)

project(ImGuiNotify VERSION 0.0.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
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
	${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
	${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
)

add_executable(test ${SOURCES} ${IMGUI_SOURCES} ${BACKEND_SOURCES})

target_include_directories(test PRIVATE
	${IMGUI_DIR}/imgui
	${IMGUI_DIR}/backends
	${IMGUI_DIR}/src/sources
	${IMGUI_DIR}/src/headers
)

find_package(glfw3 REQUIRED)
target_link_libraries(test PRIVATE glfw)

if(UNIX AND NOT APPLE)
	find_package(Vulkan REQUIRED)
	target_link_libraries(test PRIVATE Vulkan::Vulkan)
	target_link_libraries(test PRIVATE pthread dl X11 Xrandr)
endif()