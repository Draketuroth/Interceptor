cmake_minimum_required(VERSION 3.11-3.18)

set(PROJECT_NAME "Interceptor")

project(
    ${PROJECT_NAME}
	LANGUAGES C CXX
    VERSION 0.1
    DESCRIPTION "3D Graphics Interceptor")

add_subdirectory(Assets)
add_subdirectory(Shared)
add_subdirectory(DXIntercept)
add_subdirectory(DXViewer)
set_property (DIRECTORY PROPERTY VS_STARTUP_PROJECT "DXIntercept")