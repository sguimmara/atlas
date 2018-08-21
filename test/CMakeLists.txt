macro(register_test NAME FOLDER)
    add_executable(test_${NAME} ${NAME}.cpp)
    install(TARGETS test_${NAME}
            RUNTIME DESTINATION test)

    add_test(NAME test_${NAME}
             COMMAND test_${NAME})

    set_target_properties(test_${NAME} 
                          PROPERTIES 
                          FOLDER "test/${FOLDER}")
endmacro()

add_subdirectory(core)
add_subdirectory(graphics)