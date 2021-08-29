cmake_minimum_required(VERSION 3.11-3.18)

file(GLOB SHADER_FILES shaders/*.vert shaders/*.frag)
file(GLOB TEXTURE_FILES textures/*.jpg textures/*.png textures/*.txt)

file(GLOB SHADER_EXTRA_FILES shaders/*.glsl shaders/*.hlsl)
set_source_files_properties(${SHADER_EXTRA_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

source_group("Shaders" FILES ${SHADER_FILES} ${SHADER_EXTRA_FILES})
source_group("Textures" FILES ${TEXTURE_FILES})

add_custom_target(
	Assets 
	SOURCES ${SHADER_FILES} ${SHADER_EXTRA_FILES} ${TEXTURE_FILES})


