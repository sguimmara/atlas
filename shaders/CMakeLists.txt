file(GLOB shaders "${CMAKE_CURRENT_SOURCE_DIR}/*.frag"
                  "${CMAKE_CURRENT_SOURCE_DIR}/*.vert")

file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders)

foreach (file ${shaders})
    get_filename_component(CURRENT_SHADER ${file} NAME)
    set(SHADER_SPIRV_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders/${CURRENT_SHADER}.spv)
    execute_process(COMMAND glslc -I ${CMAKE_CURRENT_SOURCE_DIR}/include ${file} -o ${SHADER_SPIRV_PATH})
endforeach()
