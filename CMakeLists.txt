# CMake Bulid File
#
# CMake Version
cmake_minimum_required(VERSION 3.12)

# Compiler Settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the project name
project(TaskbarBeGone)

add_library(stb_image)

add_subdirectory(ThirdParty/glfw)
add_subdirectory(ThirdParty/stb)

add_executable(
	TaskbarBeGone
	# WIN32
	${CMAKE_CURRENT_SOURCE_DIR}/src/TaskbarBeGone.cpp

	# ImGui
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imconfig.h
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui.h
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_draw.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_internal.h
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_widgets.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imstb_rectpack.h
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imstb_textedit.h
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imstb_truetype.h
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_demo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_tables.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_tables.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_tables.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/backends/imgui_impl_glfw.h
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/backends/imgui_impl_opengl3.h
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/backends/imgui_impl_glfw.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/backends/imgui_impl_opengl3.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/backends/imgui_impl_win32.h
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/backends/imgui_impl_dx12.h
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/backends/imgui_impl_win32.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/backends/imgui_impl_dx12.cpp
)

target_include_directories(
	TaskbarBeGone
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/stb/
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/backends
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glfw
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glfw/include
)

target_link_libraries(
	TaskbarBeGone
	PRIVATE
	glfw
	opengl32.lib
	stb_image
	d3d12
	dxgi
)
