macro(register_test NAME FOLDER)
	set(TEST_NAME test_${NAME})
    add_executable(${TEST_NAME} ${NAME}.cpp)
    add_test(NAME ${TEST_NAME}
             COMMAND ${TEST_NAME})

    set_target_properties(${TEST_NAME}
                          PROPERTIES
                          FOLDER "test/${FOLDER}")

    target_link_libraries(${TEST_NAME} atlas_dll)
endmacro()

add_subdirectory(core)
add_subdirectory(graphics)