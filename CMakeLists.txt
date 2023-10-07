# CMake Version
cmake_minimum_required(VERSION 3.12)

# Compiler Settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(APP_ICON_RESOURCE_WINDOWS ${CMAKE_CURRENT_SOURCE_DIR}/Assets/icon.rc)

# Set the project name
project(TaskbarBeGone)

add_library(stb_image)

add_subdirectory(ThirdParty/stb)
add_subdirectory(ThirdParty/json)

add_executable(
	TaskbarBeGone
	WIN32
	${CMAKE_CURRENT_SOURCE_DIR}/src/EntryPoint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/TaskbarBeGone.cpp

	# ImGui
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imconfig.h
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_draw.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_widgets.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_demo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_tables.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_tables.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_tables.cpp

	# ImGui DirectX 12 Backends
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/backends/imgui_impl_win32.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/backends/imgui_impl_dx12.cpp

	# Json
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/json/include
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/json/include/nlohmann/json.hpp

	${APP_ICON_RESOURCE_WINDOWS}
)

target_include_directories(
	TaskbarBeGone
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/stb/
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/backends
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/boost
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/json/include
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/json/include/nlohmann/
)

target_link_libraries(
	TaskbarBeGone
	PRIVATE
	stb_image
	d3d12
	dxgi
)