cmake_minimum_required(VERSION 3.11-3.18)

# Include files
include_directories( 
.
./General
./Windows
)

set(GENERAL_FILES
	General/StandardTypes.h
)

set(WINDOWS_FILES
	Windows/ProcessInfo.h
	Windows/ProcessInfo.cpp
)

set(WINDOWS_CONSOLE_FILES
	Windows/Console/Print.h
	Windows/Console/Print.cpp
	Windows/Console/Config.h
	Windows/Console/Config.cpp
)

set(WINDOWS_THREAD_FILES
	Windows/Thread/CpuSampler.h
	Windows/Thread/CpuSampler.cpp
)

source_group("General" FILES ${GENERAL_FILES})
source_group("Windows" FILES ${WINDOWS_FILES})
source_group("Windows\\Console" FILES ${WINDOWS_CONSOLE_FILES})
source_group("Windows\\Thread" FILES ${WINDOWS_THREAD_FILES})

add_custom_target(
	Shared
	SOURCES ${GENERAL_FILES} ${WINDOWS_FILES} ${WINDOWS_CONSOLE_FILES} ${WINDOWS_THREAD_FILES})