FILE(GLOB SOURCES "*.cpp")
FILE(GLOB HEADERS "*.hpp")

add_executable(atlas ${SOURCES} ${HEADERS})
target_link_libraries(atlas ${LIBNAME})
install(TARGETS atlas
        RUNTIME DESTINATION bin)

set_target_properties(${NAME}
                      PROPERTIES
                      FOLDER "apps")
