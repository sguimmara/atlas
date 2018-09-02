file(GLOB shaders "${CMAKE_CURRENT_SOURCE_DIR}/*.frag"
				  "${CMAKE_CURRENT_SOURCE_DIR}/*.vert")

file(READ shaderlib.inc SHADER_LIB)

file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders)

foreach (file ${shaders})
	get_filename_component(CURRENT_SHADER ${file} NAME)
	set(INPUT_SHADER ${CMAKE_CURRENT_SOURCE_DIR}/tmp-${CURRENT_SHADER})
	configure_file(${file} ${INPUT_SHADER})
	set(SHADER_SPIRV_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders/${CURRENT_SHADER}.spv)
    execute_process(COMMAND glslangValidator -V ${INPUT_SHADER} -o ${SHADER_SPIRV_PATH})
    file(REMOVE ${INPUT_SHADER})
endforeach()

file(GLOB spirv "${CMAKE_CURRENT_SOURCE_DIR}/*.spv")
install(FILES ${spirv}
        DESTINATION bin/shaders)