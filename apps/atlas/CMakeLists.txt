FILE(GLOB SOURCES "*.cpp")
FILE(GLOB HEADERS "*.hpp")

set(APPNAME ${PROJECT_NAME}viewer)
add_executable(${APPNAME} ${SOURCES} ${HEADERS})
target_link_libraries(${APPNAME} ${LIBNAME})
install(TARGETS ${APPNAME}
        RUNTIME DESTINATION bin)

set_target_properties(${NAME}
                      PROPERTIES
                      FOLDER "apps")
