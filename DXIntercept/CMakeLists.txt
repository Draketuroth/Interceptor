cmake_minimum_required(VERSION 3.11-3.18)
include(../PrecompiledHeader.cmake)

set(CURRENT_PROJECT_NAME "DXIntercept")

set(D3D_FILE_OUTPUT "ddraw")

set(CMAKE_DEBUG_POSTFIX d)

# Include files
include_directories( 
.
../Shared
${CMAKE_SOURCE_DIR}/External/include)

# Source Groups
set(PRE_COMPILE_FILES
	pch.cpp
)

set(CORE_FILES
	Core/dllmain.cpp
)

set(GENERAL_FILES
	../Shared/General/StandardTypes.h
)

set(WINDOWS_FILES
	../Shared/Windows/ProcessInfo.h
	../Shared/Windows/ProcessInfo.cpp
)

set(WINDOWS_CONSOLE_FILES
	../Shared/Windows/Console/Print.h
	../Shared/Windows/Console/Print.cpp
	../Shared/Windows/Console/Config.h
	../Shared/Windows/Console/Config.cpp
)

set(ALL_FILES
	${CORE_FILES}
    ${GENERAL_FILES}
    ${WINDOWS_FILES}
    ${WINDOWS_CONSOLE_FILES}
	${PRE_COMPILE_FILES}
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

add_library(${CURRENT_PROJECT_NAME} SHARED ${ALL_FILES})

add_precompiled_header(${CURRENT_PROJECT_NAME} pch.h SOURCE_CXX pch.cpp FORCEINCLUDE)

set_target_properties(${CURRENT_PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)

target_compile_definitions(${CURRENT_PROJECT_NAME} PRIVATE
    "WIN32_LEAN_AND_MEAN;"
    "_MBCS"
)

set_target_properties(${CURRENT_PROJECT_NAME} PROPERTIES OUTPUT_NAME ${D3D_FILE_OUTPUT})

# External libraries
string(CONCAT ADDITIONAL_LIBRARY_DEPENDENCIES
    "$<$<CONFIG:Debug>:"
    ">"
    "$<$<CONFIG:Release>:"
    ">"
)
target_link_libraries(${CURRENT_PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

target_link_directories(${CURRENT_PROJECT_NAME} PUBLIC
    "${CMAKE_SOURCE_DIR}/External/$<CONFIG>/lib"
)