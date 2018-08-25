macro(register_app NAME)
    add_executable(${NAME} ${NAME}.cpp)
    target_link_libraries(${NAME} atlas_dll)
    install(TARGETS ${NAME}
            RUNTIME DESTINATION bin)

    set_target_properties(${NAME} 
                          PROPERTIES 
                          FOLDER "apps")
endmacro()

add_subdirectory(viewer)