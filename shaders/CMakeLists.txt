file(GLOB shaders "${CMAKE_CURRENT_SOURCE_DIR}/*.frag"
				  "${CMAKE_CURRENT_SOURCE_DIR}/*.vert")

foreach (file ${shaders})
    execute_process(COMMAND glslangValidator -V ${file} -o ${file}.spv)
endforeach()

file(GLOB spirv "${CMAKE_CURRENT_SOURCE_DIR}/*.spv")
install(FILES ${spirv}
        DESTINATION bin/shaders)