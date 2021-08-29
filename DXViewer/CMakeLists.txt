cmake_minimum_required(VERSION 3.11-3.18)
include(../PrecompiledHeader.cmake)

set(EXE_NAME "DXViewer")

# Include files
include_directories( 
.
./Core
./Platform
${CMAKE_SOURCE_DIR}/External/include)

# Source Groups
set(PRE_COMPILE_FILES
	pch.cpp
)

set(CORE_FILES
	Core/main.cpp
)

source_group("" FILES ${PRE_COMPILE_FILES})
source_group("Core" FILES ${CORE_FILES})

if(MSVC)
    add_compile_options(
        $<$<CONFIG:>:/MT>
        $<$<CONFIG:Debug>:/MTd>
        $<$<CONFIG:Release>:/MT>
    )
endif()

# Target 
add_executable(${EXE_NAME} 
    ${PRE_COMPILE_FILES}
	${CORE_FILES}
)

add_precompiled_header(${EXE_NAME} pch.h SOURCE_CXX pch.cpp FORCEINCLUDE)

# External libraries
string(CONCAT ADDITIONAL_LIBRARY_DEPENDENCIES
    "$<$<CONFIG:Debug>:"
		"glfw3;"
    ">"
    "$<$<CONFIG:Release>:"
		"glfw3;"
    ">"
)
target_link_libraries(${EXE_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

target_link_directories(${EXE_NAME} PUBLIC
    "${CMAKE_SOURCE_DIR}/External/$<CONFIG>/lib"
)

# Flags
target_compile_features(${EXE_NAME} PUBLIC cxx_std_20)

add_custom_command(
	TARGET ${EXE_NAME}
	PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/Assets/" $<TARGET_FILE_DIR:${EXE_NAME}>/Assets
)
